#ifndef INPUTS_H
#define INPUTS_H
#include <vector>

struct Inputs {
	Inputs (int count) : timers(count) {}
	
	bool IsPressed (int kk = 0, int ff = 1) const;
	bool IsHeld (int kk = 0, int ff = 0) const;
	bool IsReleased (int kk = 0, int ff = 1) const;
	bool IsUp (int kk = 0, int ff = 1) const;
	void Tick (int kk = 0, bool cond = false);
	int Add ();
	
	std::vector <int> timers;
};

#endif
