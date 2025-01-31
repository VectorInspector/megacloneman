#include "actors.h"
#include "game_data.h"

// Cursed accumulation of code, but not entirely unheard of either for now (using -c specifier in
// g++ only allows one source file; but there's nothing speaking against source files including
// source files for the sake of organisation).
#include "actor_spawn_beh.cpp"
#include "actor_misc_beh.cpp"
#include "actor_beh.cpp"

Actors::Actors () {
	count = 0;
}

int Actors::CountType (int actor_type) const {
	auto counter = 0;
	for(auto kk = 0; kk < count; kk++) if(actor_type == mo [kk].type_id) counter++;
	return counter;
}

void Actors::Clear () {
	for(auto kk = count - 1; 0 <= kk; kk--) { Remove(kk); }
	count = 0;
}

void Actors::Remove (int kk) {
	if(1 < count) { mo [kk] = mo [count - 1]; count--; }
	else { count = 0; }
}

void Actors::Spawn (GameData& d, int actor_type, Vec2 pos, Vec2 vel, int dir, int hp) {
	if(count < max_count) {
		mo [count] = Actor(default_actor);
		auto& cur_mo = mo [count];
		cur_mo.pos			= pos;
		cur_mo.old_pos		= pos;
		cur_mo.vel			= vel;
		cur_mo.type_id		= actor_type;
		cur_mo.dir			= dir;
		cur_mo.sprite_index	= d.enemy_sprite;
		cur_mo.camera_width	= -10;
		cur_mo.hp			= std::max(1, hp);
		ActorBehSpawn(d, cur_mo, count);
		count++;
	}
}

void Actors::SpawnForceBeam (GameData& d, Vec2 pos, int start_time, double speed, int dir) {
	if(count < max_count) {
		pos.y += d.tiles.tile_size / 2;
		mo [count] = Actor(default_actor);
		auto& cur_mo = mo [count];
		cur_mo.old_pos		= pos;
		cur_mo.pos			= pos;
		cur_mo.vel			= Vec2(speed * dir, 0);
		cur_mo.dir			= dir;
		cur_mo.type_id		= 0;
		cur_mo.sprite_index	= d.force_beam_sprite;
		cur_mo.timers [0]	= -start_time;
		count++;
	}
}

void Actors::SpawnLaser (GameData& d, Anims& anims, Vec2 offset, int frame_rate, int frame, int frame_count) {
	if(count < max_count) {
		auto p = d.chara.pos + offset;
		anims.AddRaw(frame, frame + frame_count, frame_rate, p);
		mo [count] = Actor(default_actor);
		auto cur_mo = mo [count];
		cur_mo.old_pos		= p;
		cur_mo.pos			= p;
		cur_mo.vel			= offset;
		cur_mo.dir			= d.chara.dir;
		cur_mo.timers [0]	= 0;
		cur_mo.timers [1]	= frame_count * frame_rate;
		count++;
	}
}

void Actors::TickForceBeams (GameData& d) {
	auto clipping = TileClipping(d.tiles);
	
	for(auto kk = 0; kk < count; kk++) {
		auto& cur_act	= mo [kk];
		auto timer		= cur_act.timers [0];
		
		// Beam starts! (You can set a negative initial timer value to make the beam come out
		// with a delay.
		if(0 == timer) {
			// Object.S_StartSound("megaman/force_beam", 14330);
			cur_act.old_pos = cur_act.pos;
		}
		
		// Beam extends until it hits a tile!
		if(0 <= timer && cur_act.vel.x != 0) {
			cur_act.pos = clipping(cur_act.pos, cur_act.vel, 0, 0);
			
			if(clipping.Horizontal()) {
				cur_act.vel = Vec2(0, 0);
			}
		}
		
		timer++;
		cur_act.timers [0] = timer;
	}
}

void Actors::TickLasers (GameData& d, Anims& anims) {
	for(auto kk = count - 1; 0 <= kk; kk--) {
		
		// animate the beam
		mo [kk].timers [0]++;
		mo [kk].old_pos = mo [kk].pos;
		mo [kk].pos = d.chara.pos + mo [kk].vel;
		if(mo [kk].timers [1] <= mo [kk].timers [0]) {
			Remove(kk);
			anims.Remove(kk);
		}
	}
}

Vec2 Actors::GrabOffset (GameData& d, Actor& a, PlayerChara& c) const {
	auto hb_size = a.HitboxSize(0);
		
	if(0 <= c.crash_timer && c.crash_timer < 10) {
		return c.pos + Vec2(c.dir * std::max(10.0, hb_size.x), 20);
	}
	
	else if(c.crash_timer < -1 || 10 <= c.crash_timer) {
		return c.pos + Vec2(0, 5 + hb_size.y / 2);
	}
	
	return a.pos;
}

void Actors::Kill (GameData& d, int aa) {
	mo [aa].hp = 0;
	ActorBehDeath(d, mo [aa], aa);
	Remove(aa);
}

// Actors are things you can interact with and who can also INTERACT WITH YOU :D
void Actors::Tick (GameData& d) {
	auto& grab_player = d.chara;
	
	// cast out the dead
	for(auto aa = count - 1; 0 <= aa; aa--) {
		if(mo [aa].hp <= 0 && !mo [aa].dont_die) {
			ActorBehDeath(d, mo [aa], aa);
			Remove(aa);
		}
	}
	
	// cast out those that want to be cast out.
	for(auto aa = count - 1; 0 <= aa; aa--) {
		if(mo [aa].remove_off_screen && !d.camera.IsPosVisible(mo [aa].pos, mo [aa].camera_width)) {
			Remove(aa);
		}
	}
	
	// The rest behave!
	for(auto aa = 0; aa < count; aa++) {
		if(!d.camera.IsPosVisible(mo [aa].pos, -mo [aa].camera_width) && 0 == mo [aa].grab_timer) {
			continue;
		}
		
		mo [aa].old_pos				= mo [aa].pos;
		mo [aa].hurt_flash_timer	= std::max(0, mo [aa].hurt_flash_timer - 1);
		auto grab_timer				= mo [aa].grab_timer;
		
		if(0 != grab_timer) {
			mo [aa].grab_timer++;
			mo [aa].pos = GrabOffset(d, mo [aa], grab_player);
			
			if(grab_player.crash_timer == -1) {
				// mo [aa].TakeDamage(3);
				Kill(d, aa);
				mo [aa].vel.x = d.Random(-4, 4);
				mo [aa].vel.y = -7;
				mo [aa].grab_timer = 0;
			}
			
			else {
				continue;
			}
		}
		
		for(auto tt = 0; tt < Actor::max_timers; tt++) {
			
			// Handle behaviour!
			auto timer						= mo [aa].timers [tt];
			
			// Do actor logic.
			if(1 == timer) {
				ActorBehStart(d, mo [aa], aa, tt);
			}
			
			if(0 < timer) {
				auto new_timer_val = ActorBeh(d, mo [aa], aa, tt, timer);
				
				if(0 == new_timer_val) {
					ActorBehEnd(d, mo [aa], aa, tt, timer);
					timer = 0;
				}
				
				else if(0 < new_timer_val) {
					timer = new_timer_val;
				}
			}
			
			if(0 < timer) {
				timer++;
			}
			
			else {
				timer = 0;
			}
			
			// Make sure the timer counts or resets.
			mo [aa].timers [tt] = timer;
		}
	}
}

Actor::Actor () {
	old_pos				= Vec2(0, 0);
	pos					= old_pos;
	vel					= Vec2(0, 0);
	dir					= 1;
	type_id				= sniper_joe;
	hp					= 1;
	frame				= 1;
	sprite_index		= 0;
	hurt_flash_timer	= 0;
	hitbox_count		= 0;
	camera_width		= 32;
	grab_timer			= 0;
	remove_off_screen	= false;
	dont_die			= false;
	
	for(auto kk = 0; kk < max_timers; kk++) {
		timers [kk] = 0;
	}
}

Actor::Actor (const Actor& src) {
	old_pos				= src.old_pos;
	pos					= src.pos;
	vel					= src.vel;
	dir					= src.dir;
	type_id				= src.type_id;
	hp					= src.hp;
	frame				= src.frame;
	sprite_index		= src.sprite_index;
	hurt_flash_timer	= src.hurt_flash_timer;
	hitbox_count		= src.hitbox_count;
	camera_width		= src.camera_width;
	grab_timer			= src.grab_timer;
	remove_off_screen	= src.remove_off_screen;
	dont_die			= src.dont_die;
	
	for(auto kk = 0; kk < max_timers; kk++) {
		timers [kk] = src.timers [kk];
	}
	
	for(auto kk = 0; kk < hitbox_count; kk++) {
		hitbox_offset [kk]			= src.hitbox_offset [kk];
		hitbox_size [kk]			= src.hitbox_size [kk];
		is_hitbox_active [kk]		= src.is_hitbox_active [kk];
		is_hitbox_deadly [kk]		= src.is_hitbox_deadly [kk];
		is_hitbox_reflective [kk]	= src.is_hitbox_reflective [kk];
		hitbox_damage [kk]			= src.hitbox_damage [kk];
		collisions_this_tick [kk]	= src.collisions_this_tick [kk];
		collisions_prev_tick [kk]	= src.collisions_prev_tick [kk];
		is_hitbox_shootable [kk]	= src.is_hitbox_shootable [kk];
	}
}

void Actor::TakeDamage (int damage) {
	if(0 < damage) {
		hp -= damage;
		hurt_flash_timer = hurt_flash_ticks;
	}
}

double Actor::FlashF () const {
	if(0 < hurt_flash_timer) {
		return 1.0 * hurt_flash_timer / hurt_flash_ticks;
	}
	return 0;
}

Vec2 Actor::HitboxPos (int kk) const { return pos + hitbox_offset [kk]; }
Vec2 Actor::HitboxSize (int kk) const { return hitbox_size [kk]; }
// Vec2, Vec2 Actor::FullHitbox (int kk) const { return pos + hitbox_offset [kk], hitbox_size [kk]; }

Vec2 Actor::ForceBeamHitboxSize (const GameData& d) const {
	return Vec2(std::abs(pos.x - old_pos.x), d.tiles.tile_size);
}

Vec2 Actor::ForceBeamHitboxPos (const GameData& d) const {
	return Vec2(std::min(pos.x, old_pos.x), std::min(pos.y, old_pos.y) - d.tiles.tile_size / 2);
}

// Vec2, Vec2 Actor::LaserHitbox () const {
	// auto len = 1000;
	// if(dir < 0) { return (pos.x - len, pos.y - 16), (len, 32); }
	// return (pos.x, pos.y - 16), (len, 32);
// }

int Actor::AddHitbox (Vec2 offset, Vec2 size, int damage, bool reflective, bool shootable) {
	if(hitbox_count < max_hitboxes) {
		auto kk = hitbox_count;
		hitbox_offset [kk]			= offset;
		hitbox_size [kk]			= size;
		hitbox_damage [kk]			= std::abs(damage);
		is_hitbox_deadly [kk]		= 0 < std::abs(damage);
		is_hitbox_active [kk]		= true;
		is_hitbox_reflective [kk]	= reflective;
		is_hitbox_shootable [kk]	= shootable;
		collisions_this_tick [kk]	= 0;
		collisions_prev_tick [kk]	= 0;
		
		hitbox_count++;
		return hitbox_count - 1;
	}
	
	return 0;
}

void Actor::SetHitboxReflective (int kk, bool reflective) { is_hitbox_reflective [kk] = reflective; }
void Actor::SetHitboxDeadly (int kk, bool deadly) { is_hitbox_deadly [kk] = deadly; }
void Actor::SetHitboxShootable (int kk, bool shootable) { is_hitbox_shootable [kk] = shootable; }
void Actor::CountCollision (int kk) { collisions_this_tick [kk]++; }

bool Actor::HasHitboxCollided (int kk) const {
	return 0 <= kk && kk < hitbox_count && 0 < collisions_this_tick [kk] && collisions_prev_tick [kk] < 1;
}

void Actor::TickCollisionCount () {
	for(auto kk = 0; kk < hitbox_count; kk++) {
		collisions_prev_tick [kk] = collisions_this_tick [kk];
		collisions_this_tick [kk] = 0;
	}
}

void Actor::BeginTimer (int kk) {
	if(timers [kk] < 1) {
		timers [kk] = 1;
	}
}

void Actor::ResetTimer (int kk, int t) {
	t = std::max(0, t);
	if(t < timers [kk]) {
		timers [kk] = t;
	}
}