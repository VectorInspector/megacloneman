#include "stage_select.h"

void StageSelect::BeginStage (GameData& d, int which) {
	
}

void Menu::Tick (GameData& d) {
	const auto& inputs = d.game_input;
	auto new_x = x;
	auto new_y = y;
	
	if(inputs.IsPressed(d.left))	{ new_x--; }
	if(inputs.IsPressed(d.right))	{ new_x++; }
	if(inputs.IsPressed(d.up))		{ new_y--; }
	if(inputs.IsPressed(d.down))	{ new_y++; }
	
	if(new_x < 0) { new_x = max_x; }
	if(max_x < new_x) { new_x = 0; }
	if(new_y < 0) { new_y = max_y; }
	if(max_y < new_y) { new_y = 0; }
	
	if(new_x != x || new_y != y) {
		on_select(d);
	}
	
	x = new_x;
	y = new_y;
	
	if(inputs.IsPressed(d.shoot) && !items [x] [y].disabled) {
		on_press(d);
	}
}
