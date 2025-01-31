#include "actors.h"

void Actors::ActorBehDeath (GameData& d, Actor& a, int actor_kk) {
	auto tile_size = d.tiles.tile_size;
	switch(a.type_id) {
	case quick_man:
		// Object.S_ChangeMusic("");
		// Object.S_StartSound("megaman/defeat", 1337);
		d.anims.SpawnQuadRing(death_anim, a.pos - Vec2(0, 16), 3, 3, 10, 0);
		break;
	default:
	case scworm:
	case telly:
	case sniper_joe:
	case springer:
		d.anims.Spawn(explosion_anim, a.pos - Vec2(0, 8));
		break;
	case fly_boy:
	case fire_maker:
		d.actors.Spawn(d, big_explosion, a.pos + Vec2(0, -10), Vec2(5, 30), 4);
		break;
	case armor_joe:
		d.actors.Spawn(d, big_explosion, a.pos + Vec2(0, -40), Vec2(5, 30), 4);
		d.actors.Spawn(d, sniper_joe, a.pos + Vec2(0, -40), Vec2(0, -3), a.dir);
		break;
	case crash_wall:
		d.actors.Spawn(d, big_explosion, a.pos + Vec2(0, -20), Vec2(2, 25), 3);
		auto tx = std::floor(a.pos.x / tile_size);
		auto ty = std::floor(a.pos.y / tile_size) - 2;
		d.tiles.Point(0, tx, ty);
		d.tiles.Point(0, tx, 1 + ty);
		break;
	}
}

// kd: This is called whenever an actor timer is started. It allows you to prepare to do
// certain behaviour.
void Actors::ActorBehStart (GameData& d, Actor& a, int actor_kk, int timer_kk) {
	// auto tile_size = d.tiles.tile_size;
	switch(a.type_id) {
	case quick_man:
		if(1 == timer_kk) {
			a.counter = 0;
		}
		
		break;
	
	case sniper_joe:
		if(1 == timer_kk) {
			// kd: Drop shield and face towards the player!
			a.frame = 1;
			a.dir = sgn(d.chara.pos.x - a.pos.x);
			a.SetHitboxReflective(0, false);
		}
		break;
		
	case fire_maker:
		break;
	}
}

void Actors::ActorBehEnd (GameData& d, Actor& a, int actor_kk, int timer_kk, int timer) {
	auto tile_size = d.tiles.tile_size;
	switch(a.type_id) {
	case sniper_joe:
		if(1 == timer_kk) {
			a.frame = 0;
			a.SetHitboxReflective(0, true);
		}
		break;
	
	case fire_maker:
		break;
	}
}
