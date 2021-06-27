#include "ai_chip.h"

int AIChip::run()
{
	int cyclesUsed = 0;
	finished = false;

	if (counter == 0)
	{
		cyclesUsed = core.op_init();
	}
	else if (counter == 1)
	{
		cyclesUsed = core.op_input("activation_mem2", "fc1/in");
	}
	//fc1
	else if (counter == 2)
	{
		cyclesUsed = core.op_fc_calculate(
			"fc1/in", "fc1/out|sign1/in", "model/fc1/weight", "model/bn1/var",
			"model/bn1/mean", "model/bn1/weight", "model/bn1/bias", array, !load_once || !weight_loaded
		);
		weight_loaded = true;
	}
	//sign1
	else if (counter == 3)
	{
		cyclesUsed = core.op_sign("fc1/out|sign1/in", "sign1/out|fc2/in", array);
	}
	//fc2
	else if (counter == 4)
	{
		cyclesUsed = core.op_fc_calculate(
			"sign1/out|fc2/in", "fc2/out|sign2/in", "model/fc2/weight", "model/bn2/var",
			"model/bn2/mean", "model/bn2/weight", "model/bn2/bias", array, !load_once
		);
	}
	//sign2
	else if (counter == 5)
	{
		cyclesUsed = core.op_sign("fc2/out|sign2/in", "sign2/out|fc3/in", array);
	}
	//fc3
	else if (counter == 6)
	{
		cyclesUsed = core.op_fc_calculate(
			"sign2/out|fc3/in", "fc3/out", "model/fc3/weight", "model/bn3/var",
			"model/bn3/mean", "model/bn3/weight", "model/bn3/bias", array, !load_once
		);
	}
	//output
	else if (counter == 7)
	{
		cyclesUsed = core.op_output("fc3/out");

		*(unsigned char*)(mem1) = core.outputIndex;
		finished = true;
	}

	return cyclesUsed * tickPerCycle;
}
