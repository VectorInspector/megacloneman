#include "items.h"
#include "game_data.h"

Items::Items () {
	count = 0;
}

bool Items::Spawn (item_types which, Vec2 start_pos, Vec2 start_vel) {
	if(max_items <= count) {
		return false;
	}
	
	auto kk = count;
	
	pos [kk]					= start_pos;
	vel [kk]					= start_vel;
	frame_count [kk]			= 2;
	frame [kk]					= 0;
	type [kk]					= which;
	
	switch(which) {
	default:
	case small_hp: frame [kk] = 0; break;
	case big_hp: frame [kk] = 2; break;
	case small_wp: frame [kk] = 4; break;
	case big_wp: frame [kk] = 6; break;
	case energy_can: frame [kk] = 8; break;
	case extra_life: frame [kk] = 10; frame_count [kk] = 1; break;
	}
	
	count++;
	return true;
}

void Items::Remove (int kk) {
	if(0 < count) {
		count--;
		pos [kk]				= pos [count];
		vel [kk]				= vel [count];
		frame [kk]				= frame [count];
		frame_count [kk]		= frame_count [count];
		type [kk]				= type [count];
	}
}

void Items::Tick (GameData& d) {
	
	// Each item can fall. Fall and eventually come to a halt on the floor is all they do.
	auto clipping = TileClipping(d.tiles);
	
	for(auto kk = 0; kk < count; kk++) {
		vel [kk].y += gravity;
		pos [kk] = clipping(pos [kk], vel [kk]);
		
		if(clipping.Above()) {
			vel [kk] = Vec2(0, 0);
		}
	}
	
	// Handle collection too.
	// auto chara			= d.chara;
	auto collect_size	= Vec2(20, 32);
	auto collect_tl		= d.chara.pos - Vec2(collect_size.x / 2, collect_size.y);
	auto collect_br		= d.chara.pos + collect_size;
	
	// This runs in reverse and thanks to the structure Remove has, it will still touch every
	// single item only once.
	for(auto kk = count - 1; 0 <= kk; kk--) {
		auto cur_pos = pos [kk];
		
		if(
		collect_tl.x <= cur_pos.x && cur_pos.x <= collect_br.x &&
		collect_tl.y <= cur_pos.y && cur_pos.y <= collect_br.y) {
			// Handle item effect.
			switch(type [kk]) {
			default:
			case small_hp: d.bar_filling.Start(0, 2); break;
			case big_hp: d.bar_filling.Start(0, 12); break;
			case small_wp: d.bar_filling.Start(1, 2); break;
			case big_wp: d.bar_filling.Start(1, 12); break;
			case energy_can: d.GiveEnergyCan(); break;
			case extra_life: d.GiveLife(); break;
			}
			
			// Item was used! Yay.
			Remove(kk);
		}
	}
}

void Items::Clear () {
	count = 0;
}
