#ifndef PROJECTILES_H
#define PROJECTILES_H

enum fx_animations : int {
	explosion_anim = 0
,	death_anim
,	smoke_anim
,	star1_anim
,	star2_anim
,	star3_anim
,	fx_type_count
};

enum projectile_types : int {
	buster_shot = 0
,	charge_shot
,	sniper_joe_shot
,	fire_maker_shot
,	boomerang
,	projectile_type_count
};

#include "vec2.h"

struct GameData;

struct Projectiles {
	Projectiles ();
	
	void Tick (GameData& d);
	void TickDisappear (GameData& d);
	void Clear ();
	void Remove (int ii);
	void ResetIndex (int kk);
	bool Spawn (projectile_types which, Vec2 start_pos, Vec2 start_vel);
	int LastSpawnIndex () const;
	
	static constexpr int max_count = 100;
	int	count;
	int timer [max_count];
	int beh_timer [max_count];
	int stop_time [max_count];
	int seek_time [max_count];
	int start_frame [max_count];
	int end_frame [max_count];
	int frame [max_count];
	int frame_rate [max_count];
	int anim_timer [max_count];
	int type [max_count];
	int damage [max_count];
	double gravity [max_count];
	Vec2 pos [max_count];
	Vec2 vel [max_count];
	double size [max_count];
	int pre_hit_counter [max_count];
	int hit_counter [max_count];
	bool hits_players [max_count];
	bool hits_enemies [max_count];
	bool rips [max_count];
	bool remove_when_outside [max_count];
};

struct Anims {
	Anims ();
	
	static constexpr int max_count = 128;
	int count;
	Vec2 pos [max_count];
	Vec2 vel [max_count];
	int frame_rate [max_count];
	int start_frame [max_count];
	int end_frame [max_count];
	int frame [max_count];
	int loops [max_count];
	int timer [max_count];
	
	void Clear ();
	void SpawnQuadRing (
	fx_animations		fx_type,
	Vec2				pos = Vec2(0, 0),
	double				speed = 3,
	int					count = 2,
	int					loop_count = 1,
	double				start_ang = 0);
	
	void Spawn (fx_animations anim, Vec2 new_pos = Vec2(0, 0), Vec2 new_vel = Vec2(0, 0), int loop_count = 1, int anim_rate = -1);
	void AddRaw (int sf, int ef, int fr, Vec2 new_pos = Vec2(0, 0), Vec2 new_vel = Vec2(0, 0), int loop_count = 1, int anim_rate = -1);
	void AddShutter (Vec2 p, bool closed = true);
	void Remove (int ii);
	void TriggerShutters (bool close_them_up = true);
	void TickShutters ();
	void Tick ();
};

#endif
