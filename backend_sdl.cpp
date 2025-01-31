#include "backend_sdl.h"
#include "lodepng.h"

// This map adds a layer of independence on SDL keywords specifically. While it doesn't erase the
// presence of SDL on the top layer, it does allow for reading key binds from a settings file, as
// a hash map allows indexing the SDL scancodes with "nice" strings.
std::unordered_map <std::string, SDL_Scancode> MakeStringToScancodeMap () {
	std::unordered_map <std::string, SDL_Scancode> r {
		{ "left", SDL_SCANCODE_LEFT },
		{ "right", SDL_SCANCODE_RIGHT },
		{ "up", SDL_SCANCODE_UP },
		{ "down", SDL_SCANCODE_DOWN },
		{ "a", SDL_SCANCODE_A },
		{ "b", SDL_SCANCODE_B },
		{ "c", SDL_SCANCODE_C },
		{ "d", SDL_SCANCODE_D },
		{ "e", SDL_SCANCODE_E },
		{ "f", SDL_SCANCODE_F },
		{ "g", SDL_SCANCODE_G },
		{ "h", SDL_SCANCODE_H },
		{ "i", SDL_SCANCODE_I },
		{ "j", SDL_SCANCODE_J },
		{ "k", SDL_SCANCODE_K },
		{ "l", SDL_SCANCODE_L },
		{ "m", SDL_SCANCODE_M },
		{ "n", SDL_SCANCODE_N },
		{ "o", SDL_SCANCODE_O },
		{ "p", SDL_SCANCODE_P },
		{ "q", SDL_SCANCODE_Q },
		{ "r", SDL_SCANCODE_R },
		{ "s", SDL_SCANCODE_S },
		{ "t", SDL_SCANCODE_T },
		{ "u", SDL_SCANCODE_U },
		{ "v", SDL_SCANCODE_V },
		{ "w", SDL_SCANCODE_W },
		{ "x", SDL_SCANCODE_X },
		{ "y", SDL_SCANCODE_Y },
		{ "z", SDL_SCANCODE_Z },
		{ "1", SDL_SCANCODE_1 },
		{ "2", SDL_SCANCODE_2 },
		{ "3", SDL_SCANCODE_3 },
		{ "4", SDL_SCANCODE_4 },
		{ "5", SDL_SCANCODE_5 },
		{ "6", SDL_SCANCODE_6 },
		{ "7", SDL_SCANCODE_7 },
		{ "8", SDL_SCANCODE_8 },
		{ "9", SDL_SCANCODE_9 },
		{ "0", SDL_SCANCODE_0 },
		{ "space", SDL_SCANCODE_SPACE },
		{ "return", SDL_SCANCODE_RETURN },
		{ "escape", SDL_SCANCODE_ESCAPE },
		// { "", SDL_SCANCODE_ },
	};
	
	return r;
}

SdlState::SdlState (BackendParams& params) {
	std::cout << "Window opens." << std::endl;
	SDL_Init(SDL_INIT_EVERYTHING);
	
	int flags = 0;
	flags |= params.resizable ? SDL_WINDOW_RESIZABLE : 0;
	flags |= params.borderless ? SDL_WINDOW_BORDERLESS : 0;
	
	window = SDL_CreateWindow("Megayuka", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, params.window_width, params.window_height, flags);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_ACCELERATED);
	
	// The keyboard state only needs to be stored once. From here, a hashmap allows disposing of the
	// SDL_Scancode dependency in the call site. This allows key binds from the settings file.
	key_state = SDL_GetKeyboardState(&num_keys);
	name_to_scancode = MakeStringToScancodeMap();
}

SdlState::~SdlState () {
	std::cout << "Window closes." << std::endl;
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

bool SdlState::IsKeyDown (std::string&& name) {
	return key_state [name_to_scancode [name]];
}

SdlSprite::SdlSprite (SDL_Renderer* renderer, unsigned char *data, unsigned width_, unsigned height_, int ts, Vec2 ofs) : size(width_, height_) {
	if(ts < 0) { ts = std::min(width_, height_); }
	
	// This is a very easy way to create a texture from pixel data.
	auto surface = SDL_CreateRGBSurfaceFrom(
		data,
		width_,
		height_,
		32,
		4 * width_,
		0x000000ff,
		0x0000ff00,
		0x00ff0000,
		0xff000000);
	
	// Make sure that the color key isn't something weird.
	SDL_SetColorKey(surface, 0, 0);
	
	texture		= SDL_CreateTextureFromSurface(renderer, surface);
	tile_size	= ts;
	offset		= Vec2(-ofs.x * tile_size, -ofs.y * tile_size);
	hor_frames	= width_ / tile_size;
	ver_frames	= height_ / tile_size;
	sdl_rect	= SDL_Rect { 0, 0, (int)width_, (int)height_ };
	
	SDL_FreeSurface(surface);
}

SdlSprite::SdlSprite (SdlSprite&& s) {
	texture		= s.texture;
	tile_size	= s.tile_size;
	size		= s.size;
	hor_frames	= s.hor_frames;
	ver_frames	= s.ver_frames;
	offset		= s.offset;
	s.texture	= nullptr;
}

// kd: right here, vector going out of scope causes all SDL memory to be freed properly.
// That is good. (Or calling std::vector::clear.)
SdlSprite::~SdlSprite () {
	if(texture) {
		SDL_DestroyTexture(texture);
	}
}

SdlRenderer::SdlRenderer (SDL_Renderer* r) : renderer(r) {
	// Give us a nice default state.
	Reset();
	
	// Generate the fallback sprite image in case image loading fails. This prevents crashing, but
	// it looks funky.
	std::cout << "Generating fallback image." << std::endl;
	
	// unsigned pitch = bpp * fallback_image_width;
	auto data = fallback_image_data;
	
	for(std::size_t kk = 0; kk < fallback_image_width * fallback_image_height; kk += bpp) {
		data [0 + kk] = 0xff;
		data [1 + kk] = 0x0;
		data [2 + kk] = 0x0;
		data [3 + kk] = 0xff;
		data += bpp;
	}
	
	std::cout << "Fallback image generated." << std::endl;
	
	// Get default infos.
	int width			= 0;
	int height			= 0;
	default_target		= SDL_GetRenderTarget(renderer);
	SDL_QueryTexture(default_target, &default_format, nullptr, &width, &height);
	SDL_GetRendererOutputSize(renderer, &width, &height);
	std::cout << "Default sizes: " << width << ", " << height << std::endl;
	default_target_size	= Vec2(width, height);
}

SdlRenderer::~SdlRenderer () {
	// Call the destructor on all textures created by this thing!
	std::cout << "Clearing SDL render data..." << std::endl;
	
	for(auto sdl_tex: textures_to_clear) {
		if(sdl_tex) {
			SDL_DestroyTexture(sdl_tex);
		}
	}
	
	textures_to_clear.clear();
}
	
void SdlRenderer::Alpha (int a_) {
	a = a_;
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

void SdlRenderer::Rgb (int r_, int g_, int b_) {
	r = r_; g = g_; b = b_;
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

void SdlRenderer::Reset () {
	
	// A nice default state.
	r = 255;
	g = 255;
	b = 255;
	a = 255;
	flip_flag = SDL_FLIP_NONE;
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

void SdlRenderer::Flip (bool h, bool v) {
	flip_flag = SDL_RendererFlip(
		(h ? SDL_FLIP_HORIZONTAL : 0) |
		(v ? SDL_FLIP_VERTICAL : 0));
}

void SdlRenderer::Clear () {
	SDL_RenderClear(renderer);
}

int SdlRenderer::LoadPngSprite (const std::string path, int tile_size, Vec2 ofs) {
	// path = std::string(YUKA_GRAPHICS_DIR) + path;
	
	// kd: I'm not gonna include sdl image for this though!!!
	unsigned char* data = nullptr;
	unsigned width		= 0;
	unsigned height		= 0;
	auto result			= lodepng_decode32_file(&data, &width, &height, path.c_str());
	
	// There was an error, so use the fallbak sprite instead.
	if(result) {
		std::cout << "Warning: Could not load sprite at [" << path << "]. Using fallback." << std::endl;
		sprites.push_back(std::move(SdlSprite(renderer, fallback_image_data, fallback_image_width, fallback_image_height, std::min(fallback_image_width, fallback_image_height))) );
	}
	
	else {
		tile_size = tile_size <= 0 ? std::min(width, height) : tile_size;
		sprites.push_back(std::move(SdlSprite(renderer, data, width, height, tile_size, ofs)));
	}
	
	return sprites.size() - 1;
}

void SdlRenderer::FreeSprites () {
	sprites.clear();
}

void SdlRenderer::SpriteTo (int si) { sprite_index = si; }
void SdlRenderer::FontTo (int si) { font_index = si; }

SdlSprite const& SdlRenderer::Sprite () const { return sprites [sprite_index]; }
SdlSprite const& SdlRenderer::Font () const { return sprites [font_index]; }

void SdlRenderer::TextureTo (SDL_Texture* which) {
	if(which) {
		current_tex = which;
		
		// Also get dimensions.
		int width	= 0;
		int height	= 0;
		
		SDL_QueryTexture(current_tex, nullptr, nullptr, &width, &height);
		tex_size = Vec2(width, height);
	}
}

// Allows creating a target for drawing to (think canvas, and you can blit the canvas).
SDL_Texture* SdlRenderer::CreateTarget (unsigned width, unsigned height) {
	auto tex = SDL_CreateTexture(
		renderer,
		// SDL_GetWindowPixelFormat(window),
		SDL_PIXELFORMAT_RGB24,
		SDL_TEXTUREACCESS_TARGET,
		width,
		height);
	
	if(tex) {
		textures_to_clear.push_back(tex);
	}
	
	else {
		std::cout << "Warning: Could not create extra SDL render target (canvas) of size " << width << " * " << height << "! Returning window render target." << std::endl;
		return default_target;
	}
	
	return tex;
}

void SdlRenderer::Target (SDL_Texture* sdl_target) {
	SDL_SetRenderTarget(renderer, sdl_target);
	
	// Note that QueryTexture on the default render target does not retrieve the correct sizes of
	// the target.
	int width					= 0;
	int height					= 0;
	Uint32 format				= 0;
	auto current_render_target	= SDL_GetRenderTarget(renderer);
	SDL_QueryTexture(current_render_target, &format, nullptr, &width, &height);
	
	if(nullptr == sdl_target) {
		SDL_GetRendererOutputSize(renderer, &width, &height);
	}
	
	target			= current_render_target;
	target_size		= Vec2(width, height);
	target_format	= format;
}

void SdlRenderer::DrawTile (Vec2 pos, int x, int y, bool use_offset) const {
	auto& current_sprite = sprites [sprite_index];
	
	x %= current_sprite.hor_frames;
	y %= current_sprite.ver_frames;
	
	auto ts = current_sprite.tile_size;
	if(use_offset) { pos += current_sprite.offset; }
	
	SDL_Rect sr = { x * ts, y * ts, ts, ts };
	SDL_Rect dr = { (int)pos.x, (int)pos.y, ts, ts };
	SDL_RenderCopyEx(renderer, current_sprite.texture, &sr, &dr, 0, nullptr, flip_flag);
}

void SdlRenderer::DrawTileRgba (Vec2 pos, int x, int y, float r, float g, float b, float a, bool use_offset) const {
	auto& current_sprite = sprites [sprite_index];
	auto& sdl_tex = current_sprite.texture;
	
	Uint8 ur = (Uint8)(255 * r);
	Uint8 ug = (Uint8)(255 * g);
	Uint8 ub = (Uint8)(255 * b);
	Uint8 ua = (Uint8)(255 * a);
	
	// Source rgb mods to reset to! SDL stores these within the SDL_Texture.
	Uint8 sr;
	Uint8 sg;
	Uint8 sb;
	Uint8 sa;
	SDL_BlendMode s_blend;
	SDL_GetTextureColorMod(sdl_tex, &sr, &sg, &sb);
	SDL_GetTextureBlendMode(sdl_tex, &s_blend);
	SDL_GetTextureAlphaMod(sdl_tex, &sa);
	
	// Blend, draw, reset!
	DrawTile(pos, x, y, use_offset);
	SDL_SetTextureColorMod(sdl_tex, ur, ug, ub);
	SDL_SetTextureAlphaMod(sdl_tex, ua);
	SDL_SetTextureBlendMode(sdl_tex, SDL_BLENDMODE_ADD);
	DrawTile(pos, x, y, use_offset);
	SDL_SetTextureColorMod(sdl_tex, sr, sg, sb);
	SDL_SetTextureAlphaMod(sdl_tex, sa);
	SDL_SetTextureBlendMode(sdl_tex, s_blend);
}

void SdlRenderer::DrawSplice (Vec2 pos, int x1, int y1, int x2, int y2, bool use_offset) const {
	auto& current_sprite = sprites [sprite_index];
	
	Order2(x1, x2);
	Order2(y1, y2);
	
	if(use_offset) { pos += current_sprite.offset; }
	
	SDL_Rect sr = { x1, y1, x2 - x1, y2 - y1 };
	SDL_Rect dr = { (int)pos.x, (int)pos.y, x2 - x1, y2 - y1 };
	SDL_RenderCopyEx(renderer, current_sprite.texture, &sr, &dr, 0, nullptr, flip_flag);
}

void SdlRenderer::DrawCustomTiles (Vec2 pos, Vec2 tile_size, int x1, int y1, int x2, int y2, bool use_offset) const {
	auto& current_sprite = sprites [sprite_index];
	
	Order2(x1, x2);
	Order2(y1, y2);
	
	if(use_offset) { pos += current_sprite.offset; }
	
	for(auto xx = x1; xx <= x2; xx++)
	for(auto yy = y1; yy <= y2; yy++) {
		auto draw_pos = pos + CompVec2(tile_size, Vec2(xx - x1, yy - y1));
		SDL_Rect sr = { (int)(xx * tile_size.x), (int)(yy * tile_size.y), (int)tile_size.x, (int)tile_size.y };
		SDL_Rect dr = { (int)draw_pos.x, (int)draw_pos.y, (int)tile_size.x, (int)tile_size.y };
		SDL_RenderCopyEx(renderer, current_sprite.texture, &sr, &dr, 0, nullptr, flip_flag);
	}
}

void SdlRenderer::DrawTexture (Vec2 pos) const {
	SDL_Rect dr = { (int)pos.x, (int)pos.y, (int)tex_size.x, (int)tex_size.y };
	SDL_RenderCopyEx(renderer, current_tex, nullptr, &dr, 0, nullptr, flip_flag);
}

void SdlRenderer::DrawTextureScale (Vec2 pos, double scale) const {
	SDL_Rect dr = { (int)pos.x, (int)pos.y, (int)(scale * tex_size.x), (int)(scale * tex_size.y) };
	SDL_RenderCopyEx(renderer, current_tex, nullptr, &dr, 0, nullptr, flip_flag);
}

void SdlRenderer::DrawSprite (Vec2 pos, bool use_offset) const {
	auto& current_sprite = sprites [sprite_index];
	if(use_offset) { pos += current_sprite.offset; }
	SDL_Rect dr = { (int)pos.x, (int)pos.y, (int)current_sprite.size.x, (int)current_sprite.size.y };
	SDL_RenderCopyEx(renderer, current_sprite.texture, nullptr, &dr, 0, nullptr, flip_flag);
}

void SdlRenderer::DrawExtendTile (Vec2 pos, int x, int y, Direction dir, int len) const {
	auto dir_vec				= DirToVec2(dir);
	const auto& cur_sprite		= Sprite();
	int full_count				= 0;
	
	// All the tiles that are drawn fully.
	auto tile_w		= (int)cur_sprite.tile_size;
	auto tile_h		= (int)cur_sprite.tile_size;
	auto tile_x		= x * tile_w;
	auto tile_y		= y * tile_h;
	auto rem_len	= 0;
	SDL_Rect sr = { tile_x, tile_y, tile_w, tile_h };
	SDL_Rect dr;
	
	// How many full pieces are going to be drawn before a clipped remainder is appended at the end?
	switch(dir) {
	default:
	case dir_up:
	case dir_down:
		full_count		= std::floor(1.0 * len / tile_w);
		rem_len			= len - full_count * tile_w;
		break;
	case dir_left:
	case dir_right:
		full_count		= std::floor(1.0 * len / tile_h);
		rem_len			= len - full_count * tile_h;
		break;
	}
	
	// Offset the start of the rect. Since it extends into the given direction, you don't want the
	// tile to start_extending from the bottom right corner of the image.
	switch(dir) {
	default: break;
	case dir_left:		pos.x -= tile_w; break;
	case dir_up:		pos.y -= tile_w; break;
	}
	
	auto step_vec	= CompVec2(Vec2(tile_w, tile_h), dir_vec);
	
	for(auto kk = 0; kk < full_count; kk++) {
		SDL_Rect dr = { (int)pos.x, (int)pos.y, tile_w, tile_h };
		SDL_RenderCopyEx(renderer, cur_sprite.texture, &sr, &dr, 0, nullptr, SDL_FLIP_NONE);
		pos += step_vec;
	}
	
	// And a partial extra appended at the end. The directions require different offsets as well,
	// since SDL cannot simply extend a texture and repeat it over a certain rect.
	switch(dir) {
	default:
	case dir_right:
		sr = SDL_Rect { tile_x, tile_y, rem_len, tile_h };
		dr = SDL_Rect { (int)pos.x, (int)pos.y, rem_len, tile_h };
		break;
	case dir_left:
		sr = SDL_Rect { tile_x + tile_w - rem_len, tile_y, rem_len, tile_h };
		dr = SDL_Rect { (int)pos.x + tile_w - rem_len, (int)pos.y, rem_len, tile_h };
		break;
	case dir_down:
		sr = SDL_Rect { tile_x, tile_y, tile_w, rem_len };
		dr = SDL_Rect { (int)pos.x, (int)pos.y, tile_w, rem_len };
		break;
	case dir_up:
		sr = SDL_Rect { tile_x, tile_y + tile_h - rem_len, tile_w, rem_len };
		dr = SDL_Rect { (int)pos.x, (int)pos.y + tile_h - rem_len, tile_w, rem_len };
		break;
	}
	
	SDL_RenderCopyEx(renderer, cur_sprite.texture, &sr, &dr, 0, nullptr, SDL_FLIP_NONE);
}

void SdlRenderer::DrawRect (Vec2 pos, Vec2 rect_size) const {
	SDL_Rect rect = { (int)pos.x, (int)pos.y, (int)rect_size.x, (int)rect_size.y };
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
	SDL_RenderFillRect(renderer, &rect);
}

void SdlRenderer::DrawLineRect (Vec2 pos, Vec2 rect_size) const {
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
	
	auto tl = pos;
	auto br = pos + rect_size;
	
	// 5 points in order to draw 4 lines.
	SDL_Point rect_points [5] = {
		{ (int)tl.x, (int)tl.y },
		{ (int)br.x, (int)tl.y },
		{ (int)br.x, (int)br.y },
		{ (int)tl.x, (int)br.y },
		{ (int)tl.x, (int)tl.y }
	};
	
	SDL_RenderDrawLines(renderer, rect_points, 5);
}

void SdlRenderer::FillSprite (Vec2 offset, Vec2 pos, Vec2 size) const {
	auto& cur_sprite	= sprites [sprite_index];
	auto ct		= cur_sprite.texture;
	auto cs		= cur_sprite.size;
	auto x_count = 1 + std::ceil(size.x / cs.x);
	auto y_count = 1 + std::ceil(size.y / cs.y);
	auto cur_pos = pos;
	
	// SDL does not have a fill flag for textures, so repeat it up manually isntead.
	for(auto xx = 0; xx < x_count; xx++) {
		for(auto yy = 0; yy < y_count; yy++) {
			SDL_Rect rect = { (int)cur_pos.x, (int)cur_pos.y, (int)cs.x, (int)cs.y };
			SDL_RenderCopyEx(renderer, ct, nullptr, &rect, 0, nullptr, flip_flag);
			cur_pos.y += cs.y;
		}
		
		cur_pos.y = 0;
		cur_pos.x += cs.x;
	}
}

void SdlRenderer::DrawMonospaceText (const std::string& t, Vec2 pos, Vec2 offset, int space) const {
	auto& font		= Font();
	auto char_size	= font.tile_size;
	
	if(space <= 0) {
		space = char_size;
	}
	
	auto text_len		= t.length();
	auto text_width		= text_len * space;
	auto text_pos		= pos + CompVec2(offset, Vec2(-(int)text_width, char_size));
	
	SDL_Rect dr = { (int)text_pos.x, (int)text_pos.y, (int)char_size, (int)char_size };
	SDL_Rect sr = { 0, 0, (int)char_size, (int)char_size };
	
	auto x_tile = 0;
	auto y_tile = 0;
	
	auto get_tiles = [&x_tile, &y_tile] (char c) {
		if(' ' == c) {
			x_tile = 5;
			y_tile = 3;
		}

		else if('a' <= c && c <= 'z') {
			x_tile = c - 'a';
			y_tile = 1;
		}

		else if('A' <= c && c <= 'Z') {
			x_tile = c - 'A';
			y_tile = 0;
		}

		else if('0' <= c && c <= '9') {
			x_tile = c - '0';
			y_tile = 0;
		}

		else {
			y_tile = 2;
			switch(c) {
			case '!': x_tile = 10; break;
			case '"': x_tile = 11; break;
			case '#': x_tile = 12; break;
			case '$': x_tile = 13; break;
			case '%': x_tile = 14; break;
			case '&': x_tile = 15; break;
			case '\'': x_tile = 16; break;
			case '(': x_tile = 17; break;
			case ')': x_tile = 18; break;
			case '*': x_tile = 19; break;
			case '+': x_tile = 20; break;
			case ',': x_tile = 21; break;
			case '-': x_tile = 22; break;
			case '.': x_tile = 23; break;
			case '/': x_tile = 24; break;
			case ':': x_tile = 25; break;
			case ';': y_tile = 3; x_tile = 0; break;
			case '<': y_tile = 3; x_tile = 1; break;
			case '=': y_tile = 3; x_tile = 2; break;
			case '>': y_tile = 3; x_tile = 3; break;
			case '?': y_tile = 3; x_tile = 4; break;
			}
		}
	};
	
	// for(auto c: t) {}
	for(std::size_t kk = 0; kk < text_len; kk++) {
		auto c = t [kk]; get_tiles(c);
		sr.x = (int)(char_size * x_tile);
		sr.y = (int)(char_size * y_tile);
		SDL_RenderCopyEx(renderer, font.texture, &sr, &dr, 0, nullptr, SDL_FLIP_NONE);
		dr.x += (int)char_size;
	}
}
