#ifndef ITEMS_H
#define ITEMS_H

#include "vec2.h"
struct GameData;

enum item_types : int {
	small_hp = 0
,	big_hp
,	small_wp
,	big_wp
,	energy_can
,	extra_life
,	item_type_count
};

struct Items {
	Items ();
	
	bool Spawn (item_types which, Vec2 start_pos, Vec2 start_vel = Vec2(0, 0));
	void Remove (int kk);
	void Tick (GameData& d);
	void Clear ();
	
	static constexpr int max_items = 30;
	static constexpr double gravity = 0.5;
	
	int count;
	Vec2 pos [max_items];
	Vec2 vel [max_items];
	item_types type [max_items];
	int frame [max_items];
	int frame_count [max_items];
};

#endif
