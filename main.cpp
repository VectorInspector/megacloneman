#include "game_data.h"

int main (int argc, char * argv []) {
	GameData data;
	
	while(data.IsRunning()) {
		data.Poll();
	}
	
	return 0;
}
