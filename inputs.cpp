#include "inputs.h"
	
int Inputs::Add () {
	timers.push_back(0);
	return timers.size() - 1;
}

void Inputs::Tick (int kk, bool cond) {
	if(cond) {
		timers [kk] = timers [kk] < 1 ? 1 : 1 + timers [kk];
	}
	
	else if(0 < timers [kk]) {
		timers [kk] = -1;
	}
	
	else {
		// timers [kk] = timers [kk] < 1 ? 0 : timers [kk] - 1;
		timers [kk] = 0;
	}
}

bool Inputs::IsPressed (int kk, int ff) const { return ff == timers [kk]; }
bool Inputs::IsHeld (int kk, int ff) const { return ff < timers [kk]; }
bool Inputs::IsReleased (int kk, int ff) const { return -ff == timers [kk]; }
bool Inputs::IsUp  (int kk, int ff) const { return timers [kk] < 0; }
