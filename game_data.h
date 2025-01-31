#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <string>
#include <algorithm>
#include <cmath>
#include <random>
#include "player_chara.h"
// #include "stage_select.h"
#include "vec2.h"
#include "backend_sdl.h"
#include "tiles.h"
#include "inputs.h"
#include "vec2.h"
#include "actors.h"
#include "items.h"
#include "projectiles.h"
#include "level_rooms.h"
#include "hp_bar.h"
#include "settings_file.h"

struct GameData {
public:
	enum play_inputs : int {
		up = 0
	,	down
	,	left
	,	right
	,	jump
	,	shoot
	,	confirm
	,	cancel
	,	quit
	,	special
	,	count
	};
	
	GameData ();
	~GameData ();
	
	static constexpr auto frames_per_sec = 60.0;
	static constexpr auto frame_rate = 1000.0 / frames_per_sec;
	Uint64 old_ticks;
	Uint64 start_ticks;
	
	void Poll ();
	bool IsRunning () const;
	
	Actors			actors;
	Actors			bosses;
	Actors			beams;
	Actors			lasers;
	Inputs			game_input;
	
	// Selecting stages!
	// StageSelect		stage_select;
	
	// Gameplay stuff!
	PlayerChara		chara;
	Projectiles		projectiles;
	Items			items;
	Anims			anims;
	Anims			laser_anims;
	Anims			shutter_anims;
	HpBarFilling	bar_filling;
	int				shake_timer;
	
	void GiveLife (int count = 1) {}
	void GiveEnergyCan (int count = 1) {}
	
	// Current room tiles and transition areas!
	Tiles			tiles;				// Current collision tiles
	TransitionAreas	transition_areas;	// Touching these causes a room transition
	Room			current_room;		// Where you're currently playing
	Room			leaving_room;		// The room that scrolls out during a room transition
	Camera			camera;
	
	// Tracking for room transitions! The four fields x, y, room and dir are storage which is read
	// when the room transition actually begins.
	int room_transition_timer;
	int room_transition_frames;
	int room_transition_x;
	int room_transition_y;
	Vec2 room_transition_chara_p1;
	Vec2 room_transition_chara_p2;
	Vec2 room_transition_chara_warp_pos;
	level_rooms room_transition_room;
	Direction room_transition_dir;
	RoomTransitionInfo room_transition_info;
	
	// Stage Select
	// StageSelect stage_select;
	
	// For the start animation.
	static constexpr auto start_anim_ticks = 120;
	int start_anim_timer;
	
	// Resources and definitions:
	Vec2 play_size;
	Vec2 room_size;
	double play_scale;
	bool nearest_play_scale;
	bool invincible_cheat;
	bool skip_start;
	int start_x_tile;
	int start_y_tile;
	level_rooms start_room;
	std::mt19937 rng_dev;
	SettingsFile settings;
	
	// Handles to resources
	int chara_sprite;
	int stage_sprite;
	int force_beam_sprite;
	int enemy_sprite;
	int big_enemy_sprite;
	int item_sprite;
	int anim_sprite;
	int quick_man_stage_sprite;
	int crash_man_stage_sprite;
	int heat_man_stage_sprite;
	int quick_man_sprite;
	int checker_sprite;
	int bar_sprite;
	int font_sprite;
	
	// Debug visuals
	int showing_level_collision;
	int showing_room_transitions;
	int showing_actor_hitboxes;
	int showing_proj_hitboxes;
	
	// Collision check helpers
	void TransitionToRoom (level_rooms which_room, int x, int y, Direction dir);
	bool HitboxIntersectsHitbox (Vec2 p1, Vec2 s1, Vec2 p2, Vec2 s2) const;
	bool CircleIntersectsHitbox (Vec2 cp, double cr, Vec2 hp, Vec2 hs) const;
	
	// RNG. (Note this is not const-qualified, because the rng_dev will change.)
	int Random (int low, int high);
	double Frandom (double low, double high);
	
	// OK function to keep here that is true if and only if the input string reads "true".
	static bool TrueFalseString (std::string&& str);
	void ShakeScreen (int duration);

private:
	bool running;
	int timer;
	
	SdlState		*sdl_state		= nullptr;
	SdlRenderer		*renderer		= nullptr;
	GlState			*gl_state		= nullptr;
	SDL_Texture		*sdl_target		= nullptr;
	BackendParams	backend_params;
};

#endif
