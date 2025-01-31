#ifndef HP_BAR_H
#define HP_BAR_H

struct GameData;

enum hp_bar_fill : int {
	fill_player_hp	= 0
,	fill_player_wp
,	fill_all_player_bars
,	fill_boss_hp
};

struct HpBarFilling {
	void Tick (GameData& d);
	void Start (int what, int amount);
	bool IsDone () const;

	int what_to_fill;
	int fill_timer;
	int beep_timer;
	int fills_left;
};

#endif