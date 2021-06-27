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
		cyclesUsed = core.op_input("activation_mem2", "data/padding/in");
	}
	//padding1
	else if (counter == 2)
	{
		cyclesUsed = core.op_padding(
			"data/padding/in", "data/padding/out|conv2d/in",
			2, 28, 28, 1, 1
		);
	}
	//conv1
	else if (counter == 3)
	{
		cyclesUsed = core.op_conv(
			"data/padding/out|conv2d/in", "data/conv2d/out|pool/in",
			"model/conv2d/kernel", "model/conv2d/bias",
			1, Range(0, 32), Range(0, 28), Range(0, 28),
			5, 1
		);
	}
	//pool1
	else if (counter == 4)
	{
		cyclesUsed = core.op_pool(
			"data/conv2d/out|pool/in", "data/pool/out|padding_1/in",
			Range(0, 32), Range(0, 14), Range(0, 14),
			2, 2
		);
	}
	//padding2
	else if (counter == 5)
	{
		cyclesUsed = core.op_padding(
			"data/pool/out|padding_1/in", "data/padding_1/out|conv2d_1/in",
			2, 14, 14, 32, 1
		);
	}
	//conv2
	else if (counter == 6)
	{
		cyclesUsed = core.op_conv(
			"data/padding_1/out|conv2d_1/in", "data/conv2d_1/out|pool_1/in",
			"model/conv2d_1/kernel", "model/conv2d_1/bias",
			32, Range(0, 64), Range(0, 14), Range(0, 14),
			5, 1
		);
	}
	//pool2
	else if (counter == 7)
	{
		cyclesUsed = core.op_pool(
			"data/conv2d_1/out|pool_1/in", "data/pool_1/out|dense/in",
			Range(0, 64), Range(0, 7), Range(0, 7),
			2, 2
		);
	}
	//fc1
	else if (counter == 8)
	{
		cyclesUsed = core.op_fc(
			"data/pool_1/out|dense/in", "data/dense/out|dense_1/in",
			"model/dense/kernel", "model/dense/bias",
			7 * 7 * 64, Range(0, 512)
		);
	}
	//fc2
	else if (counter == 9)
	{
		cyclesUsed = core.op_fc(
			"data/dense/out|dense_1/in", "data/dense_1/out",
			"model/dense_1/kernel", "model/dense_1/bias",
			512, Range(0, 10)
		);
	}
	//output
	else if (counter == 10)
	{
		cyclesUsed = core.op_output("data/dense_1/out");

		*(unsigned char*)(mem1) = core.outputIndex;
		finished = true;
	}

	return cyclesUsed * tickPerCycle;
}
