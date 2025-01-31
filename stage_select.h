#ifndef STAGE_SELECT_H
#define STAGE_SELECT_H

#include "game_data.h"
#include <functional>

struct StageSelect {
	
	// void LoadStageResources (GameData& d, int which);
	void BeginStage (GameData& d, int which);
	void Tick (GameData& d);
	bool IsBlinking (GameData& d);
	
	int timer = 0;
	int select_timer = 0;
	int target_stage = 0;
};

void StageSelect::Tick (GameData& d) {
	if(1 == select_timer) {
	}
	
	else if(150 == select_timer) {
	}
	
	if(0 < select_timer) {
		select_timer++;
	}
}

bool StageSelect::IsBlinking (GameData& d) {
	return 0 < select_timer && (select_timer % 30) < 15;
}

struct MenuItem {
	bool disabled = false;
};

struct Menu {
	void Tick (GameData& d);
	
	static constexpr int max_x_size = 8;
	static constexpr int max_y_size = 8;
	
	int x = 0;
	int y = 0;
	int max_x = 0;
	int max_y = 0;
	int menu_index = 0;
	MenuItem items [max_x_size] [max_y_size];
	
	std::function <void (GameData& d)> on_press;
	std::function <void (GameData& d)> on_select;
};

#endif
