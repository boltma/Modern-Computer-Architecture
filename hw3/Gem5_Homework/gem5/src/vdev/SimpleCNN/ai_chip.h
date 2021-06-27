#pragma once
#include "core.h"

class AIChip
{
public:
	int tickPerCycle = 1;

	int run();

	void* mem1;
	void* mem2;
	void* mem3;
	
	bool finished = false;
	unsigned char counter = 0;

	AIChip()
	{
		core.memhub.addMemory("activation_mem1", 28 * 28 * 32);
		core.memhub.addMemory("activation_mem2", 28 * 28 * 32);
		core.memhub.addMemory("weight_and_bias_mem", 1663371);

		mem1 = core.memhub.getMemory("activation_mem1").getPtr();
		mem2 = core.memhub.getMemory("activation_mem2").getPtr();
		mem3 = core.memhub.getMemory("weight_and_bias_mem").getPtr();
	}
private:
	
	Core core;
};