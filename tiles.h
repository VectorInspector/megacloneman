#ifndef TILES_H
#define TILES_H

#include <vector>
#include <string>
#include "vec2.h"
#include "level_rooms.h"

// Clipping with level tiles is done like so.
struct Tiles {
	static constexpr int max_x_size = 512;
	static constexpr int max_y_size = 256;
	
	int t [max_x_size] [max_y_size];
	int x_size;
	int y_size;
	int cursor_x;
	int cursor_y;
	int cursor_dir_x;
	int cursor_dir_y;
	int total_size;
	int tile_size;
	double push_len;
	Vec2 tile_vec;
	
	Tiles (int ts = 16, double pl = 0.05);
	void Resize (int x = -1, int y = -1);
	
	int AtPos (Vec2 pos) const;
	int At (int x, int y) const;
	void InfoAtPos (Vec2 pos);
	
	Vec2 __tl;
	Vec2 __br;
	int __type;
	
	int Xcell (Vec2 pos) const;
	int Ycell (Vec2 pos) const;
	
	// Allow placing tiles too.
	void Point (int tile_type, int x, int y);
	void Rect (int tile_type, int x1, int y1, int x2, int y2);
	void Xline (int tile_type, int x1, int x2, int yy);
	void Yline (int tile_type, int y1, int y2, int xx);
	void Fill (int tile_type);
	void StringCursor (int cursor_x, int cursor_y, std::string s, int tile_type = 1);
	
	// Tile types are defined here.
	bool IsSolid (int tt) const;
	bool IsFree (int tt) const;
	bool IsLadder (int tt) const;
};

// Occasionally, you'd like to find information not about one tile, but a group of neighbouring
// tiles. Think of how in most games, you can stand on the very edge of a tile. That kinda stuff!
struct TileScan {
	TileScan (const Tiles& t_);
	void Col (int u, int v1, int v2);
	void Row (int v, int u1, int u2);
	void Pt (int u, int v);
	
	void ColPx (Vec2 p, double h);
	void RowPx (Vec2 p, double w);
	void PtPx (Vec2 p);
	
	bool operator () ();
	
	bool __can_push;
	bool __has_solid_tile;
	bool __has_ladder_top;
	Vec2 __top_left;
	Vec2 __bottom_right;
	Vec2 __push_tl;
	Vec2 __push_br;
	
	int x1, y1;
	int x2, y2;
	const Tiles& t;
};

// And this allows finding out a pushed out position from a position and velocity, width and height,
// which effectively is the game's tile collision detection.
struct TileClipping {
	TileClipping (const Tiles& t);
	Vec2 operator () (Vec2 pos, Vec2 vel, double width = 0, double height = 0);
	
	// From which Direction did the clipping occur, if at all?
	bool Above () const;
	bool Below () const;
	bool Left ()  const;
	bool Right () const;
	bool Horizontal () const;
	bool Vertical () const;
	
	bool __clipped_from_above;
	bool __clipped_from_below;
	bool __clipped_from_left;
	bool __clipped_from_right;
	bool __hor_collision;
	bool __ver_collision;
	int __clip_tile;
	
	const Tiles& tiles;
};

// If for some reason you ever want line collision...
struct LineCollision {
	LineCollision (const Vec2& pos, const Vec2& vel, const Vec2& line_pos, const Vec2& line_dir, double line_len);
	operator bool () const;
	
	bool has_collided;
	Vec2 col_pos;
	Vec2 new_vel;
};

struct TransitionAreas {
	TransitionAreas (int _tile_size = 16);
	void Add (int tile_x1, int tile_y1, int tile_x2, int tile_y2, level_rooms where_to, Direction which_dir);
	void Clear ();
	
	static constexpr int max_count = 8;
	int tile_size;
	int count;
	Vec2 pos		[max_count];
	Vec2 size		[max_count];
	Direction dir			[max_count];
	level_rooms leads_to	[max_count];
};

// Rooms are tile cutouts on a big image, which contains the whole level visuals! As you can imagine
// it is easy to assort them into their individual rooms on the picture, and then select how many
// tiles to pick! (x1, y1) is the top left most "room tile" to use, and (x2, y2) the bottom right
// most. So a room of (2, 2) to (3, 4) would have a total size of 2-by-3 (2 screens wide, and 3
// screens tall).

// "which" means "which room setup". You might be wondering, "hey, a virtual means you don't have
// to carry that around", but think about it - virtuals are compiler food, which means that if down
// the line a programmer decides "time to export level setup to a scripting language so this game
// doesn't need to be recompiled all the time", they'll be more trouble than if they simply had to
// pick apart an identification -> room setup function.
struct Room {
	void TilesTo (int u1 = - 1, int v1 = -1, int u2 = -1, int v2 = -1);
	int x1, y1, x2, y2, which;
	
	Vec2 Size (Vec2 room_size) const;
	Vec2 RoomTilePos (Vec2 room_size, Vec2 pos_in_level) const;
};

struct RoomTransitionInfo {
	RoomTransitionInfo ();
	RoomTransitionInfo (Vec2 room_size_, const Room& current, const Room& next, int dest_x_, int dest_y_, Direction dir_, int tile_size);
	
	// Rooms scroll in the transition aniamtion. This helps you determine the scroll animation
	// offset which the two things will have!
	Vec2 RoomScrollOffset (double f = 0) const;
	
	// During the scrolling animation, the player and camera are still in the previous room's
	// coordinate space visually. The room that scrolls in is offset to compensate. At the end of
	// the transition, the player and camera are teleported...
	
	// .. to the appropriate positions in the next room.
	void operator () (Vec2 src_pos);
	
	Direction	scroll_dir;
	Vec2		room_size;
	Vec2		unit_dir;				// Scroll Direction in general
	Vec2		next_room_dir_offset;	// The visual offset of the next room (opposite to unit_dir)
	Vec2		next_room_visual_offset;// The extra visual offset due to landing in a different room tile
	int			dest_x;
	int			dest_y;
	
	// Outputs:
	Vec2 scroll_anim_pos1;
	Vec2 scroll_anim_pos2;
	Vec2 src_room_pos;
	Vec2 dest_room_pos;
	Vec2 pos_in_dest_room;
};

// Finally, a camera that can move within boundaries given to it.
struct Camera {
	Vec2 outer_size;
	Vec2 inner_size;
	Vec2 inner_focus_point;
	
	Vec2 movement_tl;
	Vec2 movement_br;
	Vec2 pos;
	Vec2 target_pos;
	
	void InnerOuter (Vec2 inner, Vec2 outer);
	void Tick (Vec2 target_pos_);
	void ForceTo (Vec2 target_pos_);
	Vec2 DrawPos () const;
	bool IsPosVisible (Vec2 pos, int size = 0) const;
};

#endif
