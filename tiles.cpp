#include "tiles.h"
#include <algorithm>
#include <string>

Tiles::Tiles (int ts, double pl) : tile_size(std::max(1, ts)), push_len(pl), tile_vec(tile_size, tile_size) {}

void Tiles::Resize (int x, int y) {
	if(x < 0) { x = max_x_size; }
	if(y < 0) { y = max_y_size; }
	
	x_size = std::clamp(0, x, max_x_size);
	y_size = std::clamp(0, y, max_y_size);
	total_size = x_size * y_size;
}

int Tiles::Xcell (Vec2 pos) const { return std::clamp((int)std::floor(pos.x / tile_size), 0, x_size - 1); }
int Tiles::Ycell (Vec2 pos) const { return std::clamp((int)std::floor(pos.y / tile_size), 0, y_size - 1); }

void Tiles::InfoAtPos (Vec2 pos) {
	auto x = (int)(pos.x / tile_size);
	auto y = (int)(pos.y / tile_size);
	
	if(x < 0 || y < 0 || x_size <= x || y_size <= y) {
		__type = 1;
	}
	
	else {
		__type = t [x] [y];
	}
	
	__tl = Vec2(x * tile_size, y * tile_size);
	__br = __tl + tile_vec;
}

int Tiles::AtPos (Vec2 pos) const {
	return At((int)(pos.x / tile_size), (int)(pos.y / tile_size));
}

int Tiles::At (int x, int y) const {
	
	// This also defines what is out of the boundary.
	if(x < 0 || y < 0 || x_size <= x || y_size <= y) {
		return 1;
	}
	
	return t [x] [y];
}

void Tiles::Point (int tile_type, int x, int y) {
	x = std::clamp(x, 0, x_size - 1);
	y = std::clamp(y, 0, y_size - 1);
	t [x] [y] = tile_type;
}

void Tiles::Rect (int tile_type, int x1, int y1, int x2, int y2) {
	Xline(tile_type, x1, x2, y1);
	Xline(tile_type, x1, x2, y2);
	Yline(tile_type, y1, y2, x1);
	Yline(tile_type, y1, y2, x2);
}

void Tiles::Xline (int tile_type, int x1, int x2, int yy) {
	Order2(x1, x2);
	x1 = std::clamp(x1, 0, x_size - 1);
	x2 = std::clamp(x2, 0, x_size - 1);
	yy = std::clamp(yy, 0, y_size - 1);
	
	for(auto xx = x1; xx <= x2; xx++) {
		t [xx] [yy] = tile_type;
	}
}

void Tiles::Yline (int tile_type, int y1, int y2, int xx) {
	Order2(y1, y2);
	xx = std::clamp(xx, 0, x_size - 1);
	y1 = std::clamp(y1, 0, y_size - 1);
	y2 = std::clamp(y2, 0, y_size - 1);
	// auto count = y2 - y1;
	// auto pch = t.data() + xx + x_size * y1;
	
	for(auto yy = y1; yy <= y2; yy++) {
		t [xx] [yy] = tile_type;
	}
}

void Tiles::Fill (int tile_type) {
	for(auto yy = 0; yy < y_size; yy++)
	for(auto xx = 0; xx < x_size; xx++) {
		t [xx] [yy] = tile_type;
	}
}

void Tiles::StringCursor (int cursor_x, int cursor_y, std::string s, int tile_type) {
	// No memes.
	if(s.empty()) {
		return;
	}
	
	// A little shortcut that makes parsing the end of the string easier.
	// s = " " + s + " ";
	s += " ";
	
	std::size_t token_start = 0;
	std::size_t token_end = 0;
	auto reading_token = false;
	auto get_token = false;
	auto kk = 0;
	auto dir_x = 0;
	auto dir_y = 0;
	int mode = 0;
	
	Point(tile_type, cursor_x, cursor_y);
	
	for(auto& c: s) {
		if(' ' == c) {
			if(reading_token) {
				token_end = kk;
				get_token = true;
			}
		}
		
		else {
			if(!reading_token) {
				token_start = kk;
				reading_token = true;
			}
		}
		
		if(get_token) {
			auto token = s.substr(token_start, token_end - token_start);
			reading_token = false;
			get_token = false;
			
			// Get a Direction!
			if(0 == mode) {
				auto dir = token [0];
				
				switch(dir) {
				default:
				case 'D': case 'd': dir_x = 0; dir_y = 1; break;
				case 'U': case 'u': dir_x = 0; dir_y = -1; break;
				case 'L': case 'l': dir_x = -1; dir_y = 0; break;
				case 'R': case 'r': dir_x = 1; dir_y = 0; break;
				}
				
				mode = 1;
			}
			
			// Get how many tiles to go and fill in the tiles in the Direction!
			else {
				auto count = std::stoi(token);
				
				for(auto cc = 0; cc < count; cc++) {
					cursor_x += dir_x;
					cursor_y += dir_y;
					Point(tile_type, cursor_x, cursor_y);
				}
				
				mode = 0;
			}
		}
		
		kk++;
	}
}

// kd: Tile types defined in these functions implicitly.
bool Tiles::IsSolid (int tt) const { return 0 < tt; }
bool Tiles::IsFree (int tt) const { return 0 == tt; }
bool Tiles::IsLadder (int tt) const { return tt < 0; }

// Scanner:
TileScan::TileScan (const Tiles& t_) : t(t_) {}
void TileScan::Col (int u, int v1, int v2) { x1 = u; x2 = u; y1 = v1; y2 = v2; }
void TileScan::Row (int v, int u1, int u2) { y1 = v; y2 = v; x1 = u1; x2 = u2; }
void TileScan::Pt (int u, int v) { x1 = u; x2 = u; y1 = v; y2 = v; }

// Start a scan at this world position, and go up h-many world units. Check all the tiles included
// within that range.
void TileScan::ColPx (Vec2 p, double h) {
	x1 = std::floor(p.x / t.tile_size);
	x2 = x1;
	y1 = std::floor((p.y - h) / t.tile_size);
	y2 = std::floor(p.y / t.tile_size);
}

// Check all the tiles which contain the given world position and the width in world units to the
// right of it.
void TileScan::RowPx (Vec2 p, double w) {
	y1 = std::floor(p.y / t.tile_size);
	y2 = y1;
	x1 = std::floor(p.x / t.tile_size);
	x2 = std::floor((p.x + w) / t.tile_size);
}

// Check only the one tile which contains this point.
void TileScan::PtPx (Vec2 p) {
	x1 = std::floor(p.x / t.tile_size);
	y1 = std::floor(p.y / t.tile_size);
	x2 = x1;
	y2 = y1;
}

bool TileScan::operator () () {
	__has_solid_tile	= false;
	__has_ladder_top	= false;
	__top_left			= Vec2(-1000000,  1000000);
	__bottom_right		= Vec2( 1000000, -1000000);
	
	for(auto yy = y1; yy <= y2; yy++)
	for(auto xx = x1; xx <= x2; xx++) {
		auto tile			= t.At(xx, yy);
		auto tile2			= t.At(xx, yy - 1);
		auto is_solid		= t.IsSolid(tile);
		auto is_ladder_top	= t.IsLadder(tile) && t.IsFree(tile2);
		
		// These will count towards collision and height detection.
		if(is_solid || is_ladder_top) {
			__has_solid_tile	= is_solid || __has_solid_tile;
			__has_ladder_top	= is_ladder_top || __has_ladder_top;
			__top_left.x		= std::max(__top_left.x, (double)xx * t.tile_size);
			__top_left.y		= std::min(__top_left.y, (double)yy * t.tile_size);
			__bottom_right.x	= std::min(__bottom_right.x, (double)(1 + xx) * t.tile_size);
			__bottom_right.y	= std::max(__bottom_right.y, (double)(1 + yy) * t.tile_size);
		}
	}
	
	__can_push	= __has_solid_tile || __has_ladder_top;
	__push_tl	= __top_left - t.push_len * Vec2(1, 1);
	__push_br	= __bottom_right + t.push_len * Vec2(1, 1);
	return __can_push;
}

TileClipping::TileClipping (const Tiles& t) : tiles(t) {}

Vec2 TileClipping::operator () (Vec2 pos, Vec2 vel, double width, double height) {
	
	// pos, width and height effectively describe a hitbox, which is what the comments are referring
	// to at times. This allows checking for clipping with different parameters easily.
	
	// Reset all the results we have so far.
	auto new_pos		= pos;
	auto hor_collision	= false;
	auto ver_collision	= false;
	TileScan ts(tiles); 
	
	__clipped_from_left			= false;
	__clipped_from_right		= false;
	__clipped_from_below		= false;
	__clipped_from_above		= false;
	
	// int tile, old_tile, above_tile;
	Vec2 tile_pos, old_tile_pos, above_tile_pos;
	
	// Check if horizontal clipping is needed (x-step).
	new_pos += Vec2(vel.x, 0);
	ts.ColPx(new_pos + Vec2(sgn(vel.x) * width, 0), height); ts();
	if(ts.__has_solid_tile) {
		if(0 < vel.x) {
			new_pos.x				= ts.__push_tl.x - width;
			hor_collision			= true;
			__clipped_from_left		= true;
		}
		
		else if(vel.x < 0) {
			new_pos.x				= ts.__push_br.x + width;
			hor_collision			= true;
			__clipped_from_right	= true;
		}
	}
	
	// Check if vertical clipping is needed (y-step).
	new_pos += Vec2(0, vel.y);
	
	// You're falling, so check at the "feet" of the hitbox! It uses the TileScanner to check all
	// tiles that are between the bottom left and bottom right points of the hitbox!
	if(0 < vel.y) {
		ts.RowPx(new_pos - Vec2(width, 0), 2 * width); ts();
		
		if((ts.__has_ladder_top || ts.__has_solid_tile)) {
			new_pos.y				= ts.__push_tl.y;
			ver_collision			= true;
			__clipped_from_above	= true;
		}
	}
	
	// You're rising, so check at the "head" of the hitbox! (Note the offset above by height).
	else if(vel.y < 0) {
		ts.RowPx(new_pos - Vec2(width, height), 2 * width); ts();
		if(ts.__has_solid_tile) {
			new_pos.y				= ts.__push_br.y + height;
			ver_collision			= true;
			__clipped_from_below	= true;
		}
	}
	
	__hor_collision = hor_collision;
	__ver_collision = ver_collision;
	__clip_tile = 0;
	return new_pos;
}

bool TileClipping::Above () const { return __clipped_from_above; }
bool TileClipping::Below () const { return __clipped_from_below; }
bool TileClipping::Left ()  const { return __clipped_from_left; }
bool TileClipping::Right () const { return __clipped_from_right; }
bool TileClipping::Horizontal () const { return __clipped_from_left || __clipped_from_right; }
bool TileClipping::Vertical () const { return __clipped_from_above || __clipped_from_below; }

TransitionAreas::TransitionAreas (int _tile_size) {
	tile_size	= _tile_size;
	count		= 0;
}

void TransitionAreas::Add (int tile_x1, int tile_y1, int tile_x2, int tile_y2, level_rooms where_to, Direction which_dir) {
	if(count < max_count) {
		pos	[count]			= tile_size * Vec2(tile_x1, tile_y1);
		size [count]		= tile_size * Vec2(1 + tile_x2 - tile_x1, 1 + tile_y2 - tile_y1);
		leads_to [count]	= where_to;
		dir [count]			= which_dir;
		count++;
	}
}

void TransitionAreas::Clear () {
	for(auto kk = 0; kk < max_count; kk++) {
		pos [kk]		= Vec2(0, 0);
		size [kk]		= Vec2(0, 0);
		dir [kk]		= dir_none;
		leads_to [kk]	= quickman_start;
	}
	count = 0;
}

LineCollision::LineCollision (const Vec2& pos, const Vec2& vel, const Vec2& line_pos, const Vec2& line_dir, double line_len) {
	auto new_pos		= pos + vel;
	auto diff1			= pos - line_pos;
	auto diff2			= new_pos - line_pos;
	auto len1			= diff1.Dot(line_dir);
	// auto len2			= diff2.Dot(line_dir);

	if(len1 < 0 || line_len < len1) {
		has_collided = false;
		col_pos = new_pos;
		new_vel = vel;
	}

	else {
		auto in_len1		= diff1.InsDot(line_dir);
		auto in_len2		= diff2.InsDot(line_dir);
		auto r_dir			= line_dir.Rot90is();
		
		has_collided = (in_len1 < 0 && 0 <= in_len2) || (in_len2 < 0 && 0 <= in_len1);
		col_pos = line_pos + len1 * line_dir;
		new_vel = vel.Dot(line_dir) * line_dir - (in_len1 < 0 ? -1 : 1) * vel.Dot(r_dir) * r_dir;
	}
}

LineCollision::operator bool () const { return has_collided; }

void Room::TilesTo (int u1, int v1, int u2, int v2) {
	
	// Some defaults... if you only specify the first two arguments, then you will create a room
	// that is one "room tile" in size, so it's just a single screen.
	if(u1 < 0) { u1 = 0; }
	if(v1 < 0) { v1 = 0; }
	if(u2 < 0) { u2 = u1; }
	if(v2 < 0) { v2 = v1; }
	
	x1 = u1;
	x2 = u2;
	y1 = v1;
	y2 = v2;
}

Vec2 Room::Size (Vec2 room_size) const {
	return Vec2(
		room_size.x * (x2 - x1 + 1),
		room_size.y * (y2 - y1 + 1));
}

Vec2 Room::RoomTilePos (Vec2 room_size, Vec2 pos_in_level) const {
	int xx = (int)std::floor(pos_in_level.x / room_size.x);
	int yy = (int)std::floor(pos_in_level.y / room_size.y);
	return Vec2(xx * room_size.x, yy * room_size.y);
}

RoomTransitionInfo::RoomTransitionInfo () {
	unit_dir				= Vec2(1, 0);
	next_room_dir_offset	= Vec2(0, 0);
	next_room_visual_offset	= Vec2(0, 0);
	dest_x					= 0;
	dest_y					= 0;
}

RoomTransitionInfo::RoomTransitionInfo (Vec2 room_size_, const Room& current, const Room& next, int dest_x_, int dest_y_, Direction dir_, int tile_size) {
	scroll_dir				= dir_;
	room_size				= room_size_;
	dest_x					= dest_x_;
	dest_y					= dest_y_;
	unit_dir				= Vec2(1, 0);
	
	// Find the direction vector to scroll into.
	switch(scroll_dir) {
	default:
	case dir_down: unit_dir = Vec2(0, 1); break;	// You leave upwards; rooms scroll down
	case dir_left: unit_dir = Vec2(-1, 0); break;	// You leave right; rooms scroll left
	case dir_right: unit_dir = Vec2(1, 0); break;	// You leave left; rooms scroll right
	case dir_up: unit_dir = Vec2(0, -1); break;		// You leave down; rooms scroll up
	}
	
	next_room_dir_offset	= CompVec2(room_size, unit_dir);
	next_room_visual_offset	= CompVec2(Vec2(dest_x, dest_y), room_size);
	
	// Scrolling down, with default Mega Man level tile dumps, you'd see a row of cut-off tiles.
	if(dir_up == scroll_dir) {
		next_room_visual_offset.y += tile_size;
	}
}

Vec2 RoomTransitionInfo::RoomScrollOffset (double f) const {
	// It starts at (0, 0) with no offset, and scrolling is ALWAYS maximally by one screen size,
	// so, there you go!
	return Linear(f, 0, Vec2(0, 0), 1, next_room_dir_offset);
}

void RoomTransitionInfo::operator () (Vec2 src_pos) {
	// Take the current room tile you're in. At this point, it would be nice to have your offset
	// inside of the current room tile you're in! If you have your current room tile...
	auto src_x	= (int)std::floor(src_pos.x / room_size.x);
	auto src_y	= (int)std::floor(src_pos.y / room_size.y);
	
	// This is the top left corner position of your current room tile. Note that rooms are attached
	// on a per-room-tile basis.
	auto room_tile_pos = CompVec2(Vec2(src_x, src_y), room_size);
	
	auto src_offset		= src_pos - room_tile_pos;
	auto dest_offset	= src_offset;
	
	// However, due to directional shenanigans, if you leave through the floor of the first room,
	// and come out of the ceiling of the second room, for example, a coordinate has to be adjusted.
	auto spacing = 40;
	
	switch(scroll_dir) {
	default:
	case dir_up: dest_offset.y = room_size.y - spacing; break;
	case dir_right: dest_offset.x = spacing; break;
	case dir_down: dest_offset.y = spacing; break;
	case dir_left: dest_offset.x = room_size.x - spacing; break;
	}
	
	// Now do some outputs.
	scroll_anim_pos1	= room_tile_pos + src_offset;
	scroll_anim_pos2	= room_tile_pos + dest_offset;
	src_room_pos		= room_tile_pos;
	dest_room_pos		= CompVec2(Vec2(dest_x, dest_y), room_size);
	pos_in_dest_room	= dest_room_pos + dest_offset;
}

void Camera::Tick (Vec2 target_pos_) {
	// Clamp the camera movement target within the boundaries so it does not scroll outside what
	// should be visible.
	target_pos = ClampVec2(target_pos_, movement_tl,  movement_br);

	// Move towards the intended target.
	auto move_f = 0.5;
	pos = move_f * target_pos + (1 - move_f) * pos;
}

void Camera::ForceTo (Vec2 target_pos_) {
	Tick(target_pos_);
	pos = target_pos;
}

void Camera::InnerOuter (Vec2 inner, Vec2 outer) {
	inner_size			= inner;
	outer_size			= outer;
	
	// Create the bounds in which the camera may move.
	auto outer_mid		= 0.5 * outer_size;
	inner_focus_point	= CompVec2(Vec2(0.5, 0.5), inner_size);
	movement_tl			= Vec2(0, 0) + inner_focus_point;
	movement_br			= outer_size - inner_focus_point;
	auto movement_size	= movement_br - movement_tl;

	if(movement_size.x <= 0) { movement_tl.x = outer_mid.x; movement_br.x = outer_mid.x; }
	if(movement_size.y <= 0) { movement_tl.y = outer_mid.y; movement_br.y = outer_mid.y; }
}

bool Camera::IsPosVisible (Vec2 a_pos, int size) const {
	auto tl = pos - inner_focus_point;
	auto br = tl + inner_size;
	
	if(
	a_pos.x - size < tl.x ||
	br.x < a_pos.x + size ||
	a_pos.y - size < tl.y ||
	br.y < a_pos.y + size) {
		return false;
	}
	
	return true;
}

Vec2 Camera::DrawPos () const {
	return pos - inner_focus_point;
}
