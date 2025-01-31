#include "hp_bar.h"
#include "game_data.h"

void HpBarFilling::Tick (GameData& d) {
	
	// Nothing to do?
	if(fill_timer < 1) {
		return;
	}
	
	// Just started filling.
	if(1 == fill_timer) {
	}
	
	beep_timer++;

	if(4 <= beep_timer) {
		switch(what_to_fill) {
		
		// Fill up player's HP:
		default:
		case 0:
			if(28 <= d.chara.hp || fills_left <= 0) {
				fill_timer = 0;
			}
			
			else {
				d.chara.hp++;
			}
			
			fills_left--;
			break;
		
		// Filling up boss HP.
		case 1:
			
			// Find a boss.
			if(0 < d.bosses.count) {
				auto& boss = d.bosses.mo [0];
				
				if(boss.hp < 28) {
					boss.hp++;
				}
				
				else {
					fill_timer = 0;
				}
			}
			
			// There's no boss so don't bother trying to fill a HP bar.
			else {
				fill_timer = 0;
			}
			
			fills_left--;
			break;
		
		// Fill up all weapon energy in the pause menu.
		}
		
		// Reset for next tick.
		beep_timer = 0;
	}
	
	// Just finished.
	if(0 == fill_timer) {
		fills_left = 0;
		fill_timer = 0;
		beep_timer = 0;
	}
}

bool HpBarFilling::IsDone () const {
	return fill_timer <= 0;
}

void HpBarFilling::Start (int what, int amount) {
	if(fill_timer <= 0 ) {
		fill_timer = 1;
		beep_timer = 0;
		fills_left = amount;
		what_to_fill = what;
	}
}
