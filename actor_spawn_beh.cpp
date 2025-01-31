#include "actors.h"

void Actors::ActorBehSpawn (GameData& d, Actor& a, int actor_kk) {
	auto tile_size = d.tiles.tile_size;
	
	switch(a.type_id) {
	case quick_man:
		a.sprite_index = d.quick_man_sprite;
		a.BeginTimer(0);
		a.BeginTimer(3);
		a.frame = 2;
		a.AddHitbox(Vec2(-8, -20),  Vec2(16, 20),  6);
		a.hp = 0;
		a.dont_die = true;
		break;
	
	case crash_wall: {
		a.frame = 28;
		a.AddHitbox(tile_size / 2 * Vec2(-1, -4), tile_size * Vec2(1, 2),  0,  true);
		a.pos += tile_size * Vec2(0.5, 2);
		a.old_pos = a.pos;
		auto tx = std::floor(a.pos.x / tile_size);
		auto ty = std::floor(a.pos.y / tile_size) - 2;
		d.tiles.Point(1, tx, ty);
		d.tiles.Point(1, tx, 1 + ty);
		break;
	}
	
	case yoku_block:
		a.frame = 21;
		a.BeginTimer(0);
		break;
		
	case big_explosion:
		a.BeginTimer(0);
		a.frame = 21;
		a.hp = 1;
		break;
		
	case sniper_joe:
		a.BeginTimer(0);
		a.BeginTimer(2);
		a.AddHitbox(Vec2(-5, -20),  Vec2(10, 20),  4,  true);
		a.hp = 6;
		a.frame = 0;
		a.dir = -1;
		break;
	
	case telly:
		a.remove_off_screen = true;
		a.BeginTimer(0);
		a.AddHitbox(Vec2(-4, -16),  Vec2(8, 16),  4);
		a.hp = 1;
		a.frame = 22;
		a.dir = -1;
		break;
	
	case telly_spawner:
		a.BeginTimer(0);
		a.hp = 1;
		a.frame = 21;
		break;
	
	case fly_boy:
		a.BeginTimer(0);
		a.AddHitbox(Vec2(-4, -32),  Vec2(8, 32),  12);
		a.hp = 6;
		a.frame = 29;
		a.dir = -1;
		a.camera_width = 35;
		break;
	
	case fire_maker:
		a.BeginTimer(0);
		a.AddHitbox(Vec2(-8, -24), Vec2(16, 24), 12);
		a.hp = 12;
		a.frame = 16;
		a.dir = -1;
		a.camera_width = 35;
		break;
		
	case springer:
		a.AddHitbox(Vec2(-7, -9), Vec2(14, 9), 4, true);
		a.hp = 8;
		a.frame = 10;
		a.dir = -1;
		a.BeginTimer(0);
		a.camera_width = 30;
		break;
	
	case scworm:
		a.BeginTimer(0);
		// a.dir = random [megaman] (0, 1) < 1 ? -1 : 1;
		a.frame = 8;
		a.hp = 2;
		a.AddHitbox(Vec2(-4, -16),  Vec2(8, 16),  2);
		break;
	
	case scworm_spawner:
		a.BeginTimer(0);
		a.dir = -1;
		a.hp = 2;
		a.AddHitbox(Vec2(-7, -9),  Vec2(14, 9),  4,  true);
		a.frame = 2;
		break;
	
	case armor_joe:
		a.BeginTimer(0);
		a.dir = -1;
		a.frame = 0;
		a.sprite_index = d.big_enemy_sprite;
		a.AddHitbox(Vec2(-20, -56),  Vec2(40, 56),  8);
		a.hp = 16;
		a.pos.y -= 1; // kd: This guy has a tendency to start inside the floor lel
		a.camera_width = 35;
		break;
	}
}
