#ifndef ACTORS_H
#define ACTORS_H

#include "vec2.h"
#include "tiles.h"
#include "projectiles.h"
#include <algorithm>

struct PlayerChara;
struct Anims;
struct GameData;

enum actor_types : int {
	sniper_joe = 0
,	fire_maker
,	tackle_fire
,	scworm
,	scworm_spawner
,	armor_joe
,	springer
,	big_explosion
,	fly_boy
,	telly
,	telly_spawner
,	yoku_block
,	crash_wall
,	quick_man
,	heat_man
,	actor_type_count
};

struct Actor {
	Vec2						old_pos;
	Vec2						pos;
	Vec2						vel;
	int							dir;		// positive means right, else means left (including 0)!
	int							type_id;
	int							hp;
	int							frame;
	int							hurt_flash_timer; // Separate from other timers cuz it's not behaviour related!
	int							sprite_index;
	double						camera_width; // Helps adjust when this actor should become active
	bool						remove_off_screen;
	bool						dont_die;
	
	Actor ();
	Actor (const Actor& src);
	
	static constexpr int max_timers = 4;
	int timers [max_timers];
	int grab_timer;
	static constexpr int max_hitboxes = 4;
	int hitbox_count;
	Vec2 hitbox_offset [max_hitboxes];
	Vec2 hitbox_size [max_hitboxes];
	bool is_hitbox_active [max_hitboxes];
	bool is_hitbox_deadly [max_hitboxes];	// Does this hurt Mega Man?
	bool is_hitbox_reflective [max_hitboxes];	// Does this plink Mega Man's shots?
	bool is_hitbox_shootable [max_hitboxes];	// can Mega Man shoot this thing at all?
	int hitbox_damage [max_hitboxes];		// How much hp damage is dealt to Mega Man
	int collisions_this_tick [max_hitboxes];
	int collisions_prev_tick [max_hitboxes];
	bool is_grounded;
	int 	counter;
	
	static constexpr int hurt_flash_ticks = 10;
	
	void TakeDamage (int damage = 0);
	double FlashF () const;
	Vec2 HitboxPos (int kk) const;
	Vec2 HitboxSize (int kk) const;
	// Vec2, Vec2 FullHitbox (int kk) const;
	Vec2 ForceBeamHitboxPos (const GameData& d) const;
	Vec2 ForceBeamHitboxSize (const GameData& d) const;
	// Vec2, Vec2 LaserHitbox () const;
	int AddHitbox (Vec2 offset = Vec2(-8, -8), Vec2 size = Vec2(16, 16), int damage = 0, bool reflective = false, bool shootable = true);
	void SetHitboxReflective (int kk, bool reflective = true);
	void SetHitboxDeadly (int kk, bool deadly = true);
	void SetHitboxShootable (int kk, bool shootable = true);
	void CountCollision (int kk);
	bool HasHitboxCollided (int kk) const;
	void TickCollisionCount ();
	void BeginTimer (int kk = 0);
	void ResetTimer (int kk, int t);
};

class Actors {
public:
	Actors ();
	
	static constexpr int max_count = 100;
	int count;
	
	Actor default_actor;
	Actor mo [max_count];
	
	int CountType (int actor_type = sniper_joe) const;
	void Clear ();
	void Remove (int kk);
	void Kill (GameData& d, int kk);
	void Spawn (GameData& d, int actor_type = sniper_joe, Vec2 pos = Vec2(0, 0), Vec2 vel = Vec2(0, 0), int dir = -1, int hp = 1);
	void SpawnForceBeam (GameData& d, Vec2 pos = Vec2(0, 0), int start_time = 30, double speed = 8, int dir = -1);
	void SpawnLaser (GameData& d, Anims& anims, Vec2 offset = Vec2(0, 0), int frame_rate = 2, int frame = 29, int frame_count = 9);
	virtual void TickLasers (GameData& d, Anims& anims);
	virtual void Tick (GameData& d);
	void TickForceBeams (GameData& d);
	
	int ActorBeh (GameData& d, Actor& a, int actor_kk, int timer_kk, int timer);
	void ActorBehDeath (GameData& d, Actor& a, int actor_kk);
	void ActorBehStart (GameData& d, Actor& a, int actor_kk, int timer_kk);
	void ActorBehEnd (GameData& d, Actor& a, int actor_kk, int timer_kk, int timer);
	void ActorBehSpawn (GameData& d, Actor& a, int actor_kk);
	Vec2 GrabOffset (GameData& d, Actor& a, PlayerChara& c) const;
};



#endif
