#include "actors.h"

int Actors::ActorBeh (GameData& d, Actor& a, int actor_kk, int timer_kk, int timer) {
	TileClipping clipping(d.tiles);
	TileScan ts(d.tiles);
	auto tile_size = d.tiles.tile_size;
	
	switch(a.type_id) {
		
		case quick_man: {
		auto width = 10;
		auto height = 20;
		auto gravity = 0.225;
		
		switch(timer_kk) {
		
		// Handling gravity and such.
		case 0:
			a.pos = clipping(a.pos, a.vel, width, height);
			
			if(clipping.Above()) {
				a.is_grounded = true;
			}
			
			if(a.is_grounded) {
				a.vel.y = 0;
				ts.RowPx(a.pos + Vec2(-width, 2), 2 * width);
				if(!ts()) { a.is_grounded = false; }
			}
			
			else {
				a.vel.y = std::min(6.0, a.vel.y + gravity);
			}
			
			break;
		
		// He also runs and jumps and stuff
		case 1:
			a.vel.x = a.dir * 2;
			a.pos = clipping(a.pos, a.vel, width, height);
			if(clipping.Horizontal()) {
				a.dir *= -1;
			}
			
			a.frame = 5 + (timer / 3) % 4;
			
			// WHEEEEE
			if(65 == timer) {
				a.counter = d.Random(2, 3);
				a.BeginTimer(2);
				return 0;
			}
			
			break;
		
		case 2:
			if(a.is_grounded) {
				
				if(0 == a.counter) {
					a.BeginTimer(1);
					return 0;
				}
				
				else {
					static const int jump_times [] = { 30, 60, 30 };
					
					// The first jump targets in front of you. The second one on top of you.
					// the third one behind you. This is all from Quick Man's perspective, so
					// turning away from him won't make him overshoot you.
					auto target_pos			= d.chara.pos;
					auto len					= a.pos.x - target_pos.x;
					auto dir_to_quickman		= sgn(len);
					auto offset				= 50;
					target_pos.x			+= (a.counter - 2) * dir_to_quickman * offset;
					
					// Gravity can be solved for if you also provide time.
					a.vel = GravityVel(gravity, a.pos, target_pos, jump_times [a.counter - 1]);
					a.dir = -dir_to_quickman;
					a.frame = 2;
					a.is_grounded = false;
					a.counter--;
					return 1;
				}
			}
			
			else if(1 == a.counter) {
				if(30 == timer) {
					for(auto kk = 0; kk < 3; kk++) {
						d.projectiles.Spawn(boomerang, a.pos + Vec2(a.dir * 3, -6), Vec2(a.dir * 3, 0) + Vec2((kk - 1) * 3, 4));
					}
					
					a.frame = 3;
				}
				
				else if(50 == timer) {
					a.frame = 2;
				}
			}
			
			break;
		
		// POSE FOR THE CAMERA, QUICK MAN!
		case 3:
			a.frame = 2;
			
			if(a.is_grounded) {
				auto frame_offset = a.counter / 5;
				if(8 < frame_offset) {
					frame_offset = 1;
				}
				
				a.frame = 9 + frame_offset;
				a.counter++;
				
				if(70 == a.counter) {
					d.bar_filling.Start(1, 28);
				}
				
				else if(70 < a.counter) {
					if(28 <= a.hp) {
						a.dont_die = false;
						a.BeginTimer(1);
						return 0;
					}
				}
			}
			
			break;
		}
		break; }
		
		case yoku_block:
		switch(timer_kk) {
		case 0:
			auto t0 = a.hp;
			auto t1 = int(std::floor(a.vel.x));
			auto t2 = int(std::floor(a.vel.y));
			auto t3 = a.dir;
			
			if(t1 == timer) {
				// Object.S_StartSound("megaman/yoku", 1337);
				auto tx = std::floor(a.pos.x / tile_size);
				auto ty = std::floor(a.pos.y / tile_size) - 2;
				d.tiles.Point(1, tx, ty);
			}
			
			if(t1 <= timer && timer < t2) {
				a.frame = 33;
				auto t = timer - t1;
				
				if(t < 18) {
					a.frame += std::min(t / 3, 4);
				}
			}
			
			if(t2 == timer) {
				auto tx = std::floor(a.pos.x / tile_size);
				auto ty = std::floor(a.pos.y / tile_size) - 2;
				d.tiles.Point(0, tx, ty);
				a.frame = 21;
			}
			
			if(t3 <= timer) {
				return t0 - 1;
			}
			
			break;
		
		break;
		}
		
		case sniper_joe:
		switch(timer_kk) {
		
		// Waiting to shoot, grounded!
		case 0:
			if(50 == timer) {
				a.BeginTimer(1);
				return 0;
			}
			
			break;
		
		// Grounded, shooting stuff!
		case 1:
			if(30 == timer || 60 == timer || 90 == timer) {
				// Object.S_StartSound("megaman/enemy_buster", 0);
				d.projectiles.Spawn(sniper_joe_shot, a.pos + Vec2(a.dir * 5, -11), Vec2(a.dir * 2, 0));
			}
			
			if(160 == timer) {
				a.BeginTimer(0);
				return 0;
			}
			break;
		
		// Falling!
		case 2:
			a.vel.y += 0.3;
			a.pos = clipping(a.pos, a.vel);
			
			if(clipping.Above()) {
				a.vel = Vec2(0, 0);
			}
			
			break;
		}
		
		break;
		
		case fly_boy:
		switch(timer_kk) {
		
		// Falling!
		case 0:
			a.vel.y += 0.3;
			a.pos = clipping(a.pos, a.vel);
			if(clipping.Above()) {
				// Object.S_StartSound("megaman/enemy_jump", 13);
				a.vel = Vec2(0, 0);
				a.BeginTimer(2);
				a.frame = 29;
				return 0;
			}
			break;
		
		// Wait before rising!
		case 2:
			if(20 < timer) {
				a.BeginTimer(1);
				return 0;
			}
			break;
		
		// Rising animation!
		case 1:
			auto rate = 20;
			auto repeats = 3;
			auto anim_time = rate * repeats;
			
			if(timer < anim_time) {
				auto t = timer % rate;
				a.frame = 29 + std::min(t / 7, 3);
				
				// if(1 == t) {
				// Object.S_StartSound("megaman/enemy_jump", 13);	
				// }
				
			}
			
			else if(timer == anim_time) {
				a.BeginTimer(0);
			}
			
			if(anim_time <= timer && timer < 105) {
				a.vel.y = -2;
				a.frame = 31 + ((timer % 3) < 2);
			}
			
			else if(105 == timer) {
				a.dir = sgn(d.chara.pos.x - a.pos.x);
				a.vel = GravityVel(0.5, a.pos, d.chara.pos - Vec2(0, 10), 70);
				return 0;
			}
			
			break;
		
		}
		break;
		
		case telly:
		switch(timer_kk) {
		case 0:
			a.frame = 22 + (timer / 6) % 6;
			
			// kd: home in on the player!
			auto diff = d.chara.pos - a.pos;
			if(diff != Vec2(0, 0)) {
				a.pos += 0.35 * diff.Unit();
			}
			break;
		}
		break;
		
		case telly_spawner:
		switch(timer_kk) {
		case 0:
			if(20 == timer && CountType(telly) < 3) {
				Spawn(d, telly, a.pos + tile_size * Vec2(0.5, 1));
			}
			
			else if(120 == timer) {
				return 1;
			}
			
			break;
		}
		break;
	
		case fire_maker:
		switch(timer_kk) {
		case 0:
			a.frame = 15 + ((timer % 20) < 10);
			
			if(timer < 110) {
				a.dir = sgn(d.chara.pos.x - a.pos.x);
			}
			
			if(110 <= timer) {
				a.frame = 17;
			}
			
			if(120 <= timer) {
				a.frame = 18;
			}
			
			if(125 == timer) {
				auto start_pos		= a.pos + Vec2(a.dir * 9, -25);
				auto end_pos		= d.chara.pos + Vec2(0, -20);	// HIT YOU IN THE HEAD!!!
				
				if(d.projectiles.Spawn(fire_maker_shot, start_pos, Vec2(0, 5))) {
					auto kk = d.projectiles.LastSpawnIndex();
					auto vel = GravityHorVel(d.projectiles.gravity [kk], start_pos, end_pos, 1.5);
					vel.x = a.dir * std::abs(vel.x);
					d.projectiles.vel [kk] = vel;
				}
				
				// Object.S_StartSound("megaman/fire", 7);
			}
			
			if(130 == timer) {
				return 1;
			}
			
			break;
			
		}
		
		break;
		
		case springer:
		switch(timer_kk) {
		case 0: {
			if(a.HasHitboxCollided(0)) {
				a.BeginTimer(1);
				return 0;
			}
			
			auto check_pos = a.pos + Vec2(a.dir * 6, -4);
			
			if(0 < d.tiles.AtPos(check_pos) || 0 == d.tiles.AtPos(check_pos + Vec2(0, tile_size))) {
				a.dir *= -1;
			}
			
			a.pos += Vec2(a.dir * (std::abs(d.chara.pos.y - a.pos.y) < 4 ? 3 : 1), 0);
			break;
		}
		
		case 1: {
			auto anim_rate = 7;
			auto anim_loops = 3;
			
			a.frame = 11 + ((timer / anim_rate) % 4);
			
			if(4 * anim_loops * anim_rate + anim_rate <= timer) {
				a.BeginTimer(0);
				a.frame = 10;
				return 0;
			}
			
			break;
		}
		
		}	
		break;
		
		case scworm: {
		switch(timer_kk) {
		case 0:
			if(0 == timer % 8) {
				a.dir *= -1;
			}
			
			a.vel.y += 0.5;
			a.pos = clipping(a.pos, a.vel);
			
			if(clipping.Above()) {
				a.vel = Vec2(0, 0);
			}
			
			if(170 <= timer) {
				a.hp = 0;
			}
			
			break;
		}
		break;
		}
		
		case scworm_spawner:
		switch(timer_kk) {
		case 0:
			if(1 == timer % 70) {
				a.BeginTimer(1);
			}
			
			break;
		
		case 1:
			a.frame = 2 + (timer / 8);
			
			if(7 < a.frame) {
				a.frame = 2;
				auto deg = d.Random(45, 135);
				d.actors.Spawn(d, scworm, a.pos + Vec2(0, -8), 6 * DegToVec2(-deg) );
				return 0;
			}
			break;
		}
		break;
		
		case armor_joe:
		switch(timer_kk) {
		case 0:
			if(15 == timer) {
				a.dir = sgn(d.chara.pos.x - a.pos.x);
			}
			
			if(30 == timer) {
				a.frame = 1;
			}
			
			if(45 == timer) {
				a.frame = 2;
				a.vel = Vec2(a.dir * 2, -5);
				// Object.S_StartSound("megaman/enemy_jump", 13);
			}
			
			if(2 == a.frame) {
				a.vel.y += 0.35;
				a.pos = clipping(a.pos, a.vel, 20);
				
				if(clipping.Above()) {
					// Object.S_StartSound("megaman/enemy_mech", 13);
					a.vel = Vec2(0, 0);
					a.BeginTimer(1);
					return 0;
				}
			}
			
			break;
		
		case 1:
			if(1 == timer) {
				a.frame = 1;
			}
			
			else if(15 == timer) {
				a.frame = 0;
			}
			
			else if(70 == timer) {
				a.BeginTimer(0);
				return 0;
			}
			break;
		}
		break;
		
		case big_explosion:
		switch(timer_kk) {
		case 0:
			if(1 == timer % a.dir) {
				if(a.vel.x <= 0) {
					a.hp = 0;
				}
				
				else {
					// Object.S_StartSound("megaman/enemy_damage", 3030);
					a.vel.x -= 1;
					for(auto kk = 0; kk < 5; kk++) {
						d.anims.Spawn(
							explosion_anim,
							a.pos + Vec2(d.Frandom(-a.vel.y, a.vel.y), d.Frandom(-a.vel.y, a.vel.y)),
							Vec2(0, 0),
							0,
							d.Random(1, 5));
					}
				}
			}
			
			break;
		}
		break;
	}
	
	return -1; // kd: This means the behaviour has not finished! Keep the timer the same!
}
