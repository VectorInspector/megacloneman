#ifndef PLAYERCHARA_H
#define PLAYERCHARA_H
#include "vec2.h"
#include "game_data.h"

struct GameData;
class PlayerChara {
public:
	PlayerChara ();
	void Tick (GameData& d);		// Handle movement, shooting, getting hurt, ladders, ...
	void TickHurt ();				// Subroutine to tick!
	void TickDrop (GameData& d);	// Handle the drop-in animation and also switching animation timing!
	void StartDropAnim ();			// Trigger the drop-in animation!
	void StartSwitchAnim ();		// Trigger the switch weapon animation (also landing animation)!
	
	int AnimFrame (GameData& d) const;
	int SubAnimFrame (GameData& d) const;
	bool Hurt (GameData& d, int damage = 2, int dir = 0, bool ignore_invul = false);
	void Kill ();
	void TickHitboxes ();
	
	Vec2 old_pos;
	Vec2 pos;
	Vec2 vel;
	Vec2 hitbox_pos;
	Vec2 hitbox_size;
	Vec2 tile_collision_size;
	
	Vec2 drop_pos;
	Vec2 drop_start_pos;
	Vec2 drop_end_pos;
	Vec2 drop_vel;
	
	int dir;
	// Vec2 input_vel;
	double hor_input;
	
	int hurt_dir;
	int hp;
	bool intangible;
	bool allow_slide;
	bool allow_uppercut;
	bool allow_grab;
	bool allow_charge;
	int deal_damage;
	
	// Settings pertaining to the duration of certain things happening!
	int death_anim_ticks;
	int invul_ticks;
	int hurt_ticks;
	int kill_ticks;
	int shoot_ticks;
	int slide_ticks;
	int ladder_transition_ticks;
	int switch_ticks;
	
	// Tracking movement, getting hurt, sliding, that kind of stuff...
	int ground_timer;
	int slide_timer;
	int hurt_timer;
	int ladder_timer;
	int move_timer;
	int kill_timer;
	int shoot_timer;
	int invul_timer;
	int jump_timer;
	int momentum_timer;
	bool enters_ladder;
	int drop_timer;
	int switch_timer;
	
	// Yuka stuff.
	int grab_timer;
	int uppercut_timer;
	int crash_timer;
	int charge_timer;
};

#endif
