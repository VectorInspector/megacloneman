#include "projectiles.h"
#include "game_data.h"

Projectiles::Projectiles () {
	count = 0;
}

void Projectiles::Tick (GameData& d) {
	Vec2 seek_pos = Vec2(0, 0);
	for(auto kk = count - 1; 0 <= kk; kk--) {
		// kd: REALLY DANGEROUS SUPER REMOVAL TECHNIQUE GO
		if(timer [kk] <= 0) {
			Remove(kk);
		}
		
		vel [kk].y += gravity [kk];
		pos [kk] += vel [kk];
		timer [kk]--;
		beh_timer [kk]++;
		
		if(beh_timer [kk] == stop_time [kk]) { vel [kk] = Vec2(0, 0); }
		if(beh_timer [kk] == seek_time [kk]) { vel [kk] = 5 * (seek_pos - pos [kk]).Unit(); }
		
		anim_timer [kk]++;
		if(frame_rate [kk] <= anim_timer [kk]) {
			anim_timer [kk] = 0;
			frame [kk]++;
			
			if(end_frame [kk] < frame [kk]) {
				frame [kk] = start_frame [kk];
			}
		}
	}
}

void Projectiles::TickDisappear (GameData &d) {
	for(auto kk = count - 1; 0 <= kk; kk--) {
		if(remove_when_outside [kk] && !d.camera.IsPosVisible(pos [kk])) {
			Remove(kk);
		}
	}
}

void Projectiles::Clear () { count = 0; }
void Projectiles::Remove (int kk) {
	if(0 < count) {
		// kd: If you implement more fields, remember to handle removal here!
		count--;
		
		if(0 < count) {
			timer			[kk] = timer [count];
			beh_timer		[kk] = beh_timer [count];
			stop_time		[kk] = stop_time [count];
			seek_time		[kk] = seek_time [count];
			pos				[kk] = pos [count];
			vel				[kk] = vel [count];
			hit_counter		[kk] = hit_counter [count];
			hits_enemies	[kk] = hits_enemies [count];
			hits_players	[kk] = hits_players [count];
			start_frame		[kk] = frame [count];
			end_frame		[kk] = frame [count];
			anim_timer		[kk] = anim_timer [count];
			frame			[kk] = frame [count];
			type			[kk] = type [count];
			gravity			[kk] = gravity [count];
			rips			[kk] = rips [count];
		}
	}
}

// This pretty much defines projectile types as well, similarly to how actors are defined over the
// ActorBeh* functions...
bool Projectiles::Spawn (projectile_types which, Vec2 start_pos, Vec2 start_vel) {
	if(max_count <= count) {
		return false;
	}
	
	auto kk = count;
	ResetIndex(kk);
	
	pos [kk] = start_pos;
	vel [kk] = start_vel;
	remove_when_outside [kk] = true;
	
	switch(which) {
	default:
	case sniper_joe_shot:
		frame [kk] = 1;
		end_frame [kk] = 1;
		break;
	
	case fire_maker_shot:
		gravity [kk] = 0.4;
		frame [kk] = 3;
		end_frame [kk] = 4;
		frame_rate [kk] = 16;
		remove_when_outside [kk] = false;
		timer [kk] = 300;
		break;
	
	// Quick Man's chasing boomerang!
	case boomerang:
		frame [kk] = 39;
		end_frame [kk] = 42;
		frame_rate [kk] = 8;
		stop_time [kk] = 30;
		seek_time [kk] = 70;
		break;
	
	// Small Mega Buster shot!
	case buster_shot:
		frame [kk] = 0;
		end_frame [kk] = 0;
		hits_enemies [kk] = true;
		hits_players [kk] = false;
		break;
	
	// Mega Buster charge shot!
	case charge_shot:
		frame [kk] = 16;
		end_frame [kk] = 19;
		frame_rate [kk] = 5;
		hits_enemies [kk] = true;
		hits_players [kk] = false;
		size [kk] = 15;
		rips [kk] = true;
		break;
	}
	
	start_frame [kk] = frame [kk];
	count++;
	return true;
}

// Remember this is only valid if the last spawn succeeded. Use this only immediately after a
// successful spawn or know pain.
int Projectiles::LastSpawnIndex () const {
	return std::clamp(count - 1, 0, max_count - 1);
}

// This turns the projectile in index kk into a default projectile.
void Projectiles::ResetIndex (int kk) {
	if(kk < 0 || max_count <= kk) {
		return;
	}
	
	timer [kk] = 500;
	beh_timer [kk] = 0;
	stop_time [kk] = -1;
	seek_time [kk] = -1;
	pos [kk] = Vec2(0, 0);
	vel [kk] = Vec2(0, 0);
	hit_counter [kk] = 0;
	hits_players [kk] = true;
	hits_enemies [kk] = false;
	frame [kk] = 0;
	start_frame [kk] = 0;
	end_frame [kk] = 0;
	frame_rate [kk] = 4;
	anim_timer [kk] = 0;
	size [kk] = 5;
	type [kk] = sniper_joe_shot;
	gravity [kk] = 0;
	rips [kk] = 0;
}

Anims::Anims () {
	count = 0;
}
	
void Anims::Clear () { count = 0; }

void Anims::SpawnQuadRing (
fx_animations		fx_type,
Vec2				pos,
double				speed,
int					count,
int					loop_count,
double				start_ang) {
	auto ang		= start_ang;
	auto step_ang	= 90.0 / count;
	
	for(auto kk = 0; kk < count; kk++) {
		auto v = speed * DegToVec2(ang);
		ang += step_ang;
		
		Spawn(fx_type, pos, v, loop_count);
		Spawn(fx_type, pos, v.Rot90is(), loop_count);
		Spawn(fx_type, pos, -v, loop_count);
		Spawn(fx_type, pos, v.Rot90as(), loop_count);
	}
}

void Anims::Spawn (fx_animations anim, Vec2 new_pos, Vec2 new_vel, int loop_count, int anim_rate) {
	if(max_count <= count) {
		return;
	}
	
	auto kk = count;
	count++;
	
	pos [kk]		= new_pos;
	vel [kk]		= new_vel;
	loops [kk]		= std::max(1, loop_count);
	frame_rate [kk]	= 3;
	timer [kk]		= 0;
	
	switch(anim) {
	default:
	case explosion_anim:
		start_frame [kk]	= 6;
		end_frame [kk]		= 9;
		break;
	
	case death_anim:
		start_frame [kk]	= 10;
		end_frame [kk]		= 13;
		break;
	
	case smoke_anim:
		start_frame [kk]	= 20;
		end_frame [kk]		= 23;
		break;
	}
	
	frame [kk]		= start_frame [kk];
	
	if(0 < anim_rate) {
		frame_rate [kk]	= anim_rate;
	}
}

void Anims::AddRaw (int sf, int ef, int fr, Vec2 new_pos, Vec2 new_vel, int loop_count, int anim_rate) {
	if(count < max_count) {
		pos [count]			= new_pos;
		vel [count]			= new_vel;
		start_frame [count]	= sf;
		end_frame [count]	= ef;
		frame_rate [count]	= fr;
		frame [count]		= start_frame [count];
		loops [count]		= std::max(1, loop_count);
		timer [count]		= 0;
		count++;
	}
}

void Anims::AddShutter (Vec2 p, bool closed) {
	if(count < max_count) {
		auto f = closed ? 16 : 0;
		pos [count]	= p;
		vel [count] = Vec2(0, 0);
		start_frame [count] = f;
		end_frame [count] = f;
		frame [count] = f;
		frame_rate [count] = 3;
		timer [count] = 0;
		loops [count] = closed;
		count++;
	}
}

void Anims::Remove (int kk) {
	if(0 < count) {
		auto last_kk = std::max(0, count - 1);
		pos [kk]			= pos [last_kk];
		vel [kk]			= vel [last_kk];
		frame_rate [kk]		= frame_rate [last_kk];
		start_frame [kk]	= start_frame [last_kk];
		end_frame [kk]		= end_frame [last_kk];
		frame [kk]			= frame [last_kk];
		loops [kk]			= loops [last_kk];
		timer [kk]			= timer [last_kk];
		count				= last_kk;
	}
}

void Anims::TriggerShutters (bool close_them_up) {
	for(auto kk = 0; kk < count; kk++) {
		loops [kk] = close_them_up;
	}
}

void Anims::TickShutters () {
	for(auto kk = 0; kk < count; kk++) {
		timer [kk]++;
		
		if(2 <= timer [kk]) {
			auto old_frame = frame [kk];
			timer [kk] = 0;
			if(0 < loops [kk]) { frame [kk] = std::min(16, 1 + frame [kk]); }
			else { frame [kk] = std::max(0, frame [kk] - 1); }
			
			if(old_frame != frame [kk] && 0 == frame [kk] % 6) {
				// Object.S_StartSound("megaman/shutter", 0);
			}
		}
	}
}

void Anims::Tick () {
	for(auto kk = count - 1; 0 <= kk; kk--) {
		pos [kk] += vel [kk];
		timer [kk]++;
		
		if(frame_rate [kk] <= timer [kk]) {
			timer [kk] = 0;
			frame [kk]++;
			
			if(end_frame [kk] < frame [kk]) {
				loops [kk]--;
				
				if(loops [kk] <= 0) {
					Remove(kk);
				}
				
				else {
					frame [kk] = start_frame [kk];
				}
			}
		}
	}
}
