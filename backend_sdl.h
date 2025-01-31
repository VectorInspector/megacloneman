#ifndef BACKEND_SDL_H
#define BACKEND_SDL_H

#include "sdl.h"
#include "vec2.h"
#include <vector>
#include <unordered_map>
#include <string>

struct GlState { int x; };

struct BackendParams {
	int window_width		= 400;
	int window_height		= 300;
	bool borderless			= false;
	bool resizable			= true;
};

// Constructor and destructor setup makes sure that the following data is cleaned up and
// initialised nicely and when it should be.
struct SdlState {
	SdlState (BackendParams& params);
	~SdlState ();
	
	// Note this function cannot be const because name_to_scancode is an unordered_map. Even if
	// merely accessed it can affect the map.
	bool IsKeyDown (std::string&& name);
	
	SDL_Renderer *renderer;
	SDL_Window *window;
	SDL_Event event;
	
	// Keyboard state
	const Uint8* key_state;
	int num_keys;
	std::unordered_map <std::string, SDL_Scancode> name_to_scancode;
};

struct SdlSprite {
	SdlSprite (SDL_Renderer* renderer, unsigned char *data, unsigned width_, unsigned height_, int ts = -1, Vec2 ofs = Vec2(0.5, 1));
	SdlSprite (SdlSprite&& s);
	~SdlSprite ();

	SDL_Rect sdl_rect;
	SDL_Texture *texture;
	Vec2 size;
	Vec2 offset;
	int tile_size;
	int hor_frames;
	int ver_frames;
};

struct AlignText {
	AlignText (int cs = 8) : char_size(cs) {}
	
	int char_size		= 8;
	Vec2 align_pos		= Vec2(0, 0);

	const std::string& operator () (const std::string& text, Vec2 base_pos, Vec2 offset = Vec2(0.5, 0.5)) {
		align_pos = base_pos - CompVec2(offset, Vec2(char_size * text.length(), char_size));
		return text;
	};
};

class SdlRenderer {
	
	std::vector <SdlSprite> sprites;
	std::vector <SDL_Texture*> textures_to_clear;
	SDL_Renderer* renderer;
	
public:
	SdlRenderer (SDL_Renderer* r);
	~SdlRenderer ();
	
	// Render state and drawing stuff!
	Vec2 size;
	SDL_RendererFlip flip_flag;
	SDL_Texture* current_tex;
	Vec2 sprite_size;
	Vec2 tex_size;
	
	// Current target (texture that is being drawn to; default is the screen!)
	SDL_Texture*	target;
	Vec2			target_size;
	Uint32			target_format;
	
	// Default target, the one that SDL chooses to draw to in order to output to the screen!
	SDL_Texture*	default_target;
	Vec2			default_target_size;
	Uint32			default_format;
	
	int r, g, b, a;
	int sprite_index;
	int font_index;
	
	SdlSprite const& Sprite () const;
	SdlSprite const& Font () const;
	SDL_Texture* CreateTarget (unsigned width, unsigned height);
	void Target (SDL_Texture* sdl_target = nullptr);
	void Alpha (int a_ = 255);
	void Rgb (int r_ = 255, int g_ = 255, int b_ = 255);
	void Reset ();
	void Flip (bool h = false, bool v = false);
	void Clear ();
	int LoadPngSprite (const std::string path, int tile_size = -1, Vec2 ofs = Vec2(0.5, 1));
	void FreeSprites ();
	void SpriteTo (int si = 0);
	void FontTo (int si = 0);
	void TextureTo (SDL_Texture* which);
	void DrawLineRect (Vec2 pos, Vec2 rect_size) const;
	void DrawRect (Vec2 pos, Vec2 rect_size) const;
	void DrawMonospaceText (const std::string& t, Vec2 pos = Vec2(0, 0), Vec2 offset = Vec2(0, 0), int space = -1) const;
	
	// Draw a tile of the currently selected sprite (SpriteTo) at the specified position in the
	// current render target (TargetTo). A tile is selected by the tile size you passed into a
	// Load___Sprite function, and (x, y) is the selected cell.
	void DrawTile (Vec2 pos, int x = 0, int y = 0, bool use_offset = true) const;
	void DrawTileRgba (Vec2 pos, int x, int y, float r, float g, float b, float a, bool use_offset = true) const;
	void DrawSplice (Vec2 pos, int x1, int y1, int x2, int y2, bool use_offset = true) const;
	void DrawCustomTiles (Vec2 pos, Vec2 tile_size, int x1 = 0, int y1 = 0, int x2 = 1, int y2 = 1, bool use_offset = true) const;
	void DrawExtendTile (Vec2 pos, int x, int y, Direction dir, int len) const;
	
	// Draw the currently selected sprite (SpriteTo) at the specified position in the current
	// render target (TargetTo).
	void DrawSprite (Vec2 pos, bool use_offset = true) const;
	
	// Draw the currently selected texture (TextureTo) at the specified position in the current
	// render target (TargetTo).
	void DrawTexture (Vec2 pos) const;
	
	// Draw the texture at the position, but scale it.
	void DrawTextureScale (Vec2 pos, double scale = 1) const;
	
	// Draw the current sprite repeatedly with the offset to fill the rect of pos to pos + size!
	// This does not clip the result inside the rect! Use canvas for that!
	void FillSprite (Vec2 offset, Vec2 pos, Vec2 size) const;
	
	// Procedurally generated sprite in case a sprite does not load.
	static constexpr unsigned bpp = 4;
	static constexpr unsigned fallback_image_width = 32;
	static constexpr unsigned fallback_image_height = 32;
	static constexpr unsigned fallback_image_size = fallback_image_width * fallback_image_height * bpp;
	unsigned char fallback_image_data [fallback_image_size];
};

#endif
