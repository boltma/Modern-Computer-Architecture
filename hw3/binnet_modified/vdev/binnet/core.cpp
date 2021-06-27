#include <cassert>
#include <algorithm>
#include <cstring>
#include <memory>

#include "core.h"

int Core::op_init(void)
{
	/*set up memories*/
	//create weight & bias mem
	float* mem3 = memhub.getMemory("weight_and_bias_mem").getPtr();
	memhub.addSharedMemory("model/fc1/weight", mem3, 1024 * 1024 * sizeof(std::int8_t) / sizeof(float));
	mem3 += 1024 * 1024 * sizeof(std::int8_t) / sizeof(float);
	memhub.addSharedMemory("model/bn1/bias", mem3, 1024);
	mem3 += 1024;
	memhub.addSharedMemory("model/bn1/weight", mem3, 1024);
	mem3 += 1024;
	memhub.addSharedMemory("model/bn1/var", mem3, 1024);
	mem3 += 1024;
	memhub.addSharedMemory("model/bn1/mean", mem3, 1024);
	mem3 += 1024;

	memhub.addSharedMemory("model/fc2/weight", mem3, 1024 * 1024 * sizeof(std::int8_t) / sizeof(float));
	mem3 += 1024 * 1024 * sizeof(std::int8_t) / sizeof(float);
	memhub.addSharedMemory("model/bn2/bias", mem3, 1024);
	mem3 += 1024;
	memhub.addSharedMemory("model/bn2/weight", mem3, 1024);
	mem3 += 1024;
	memhub.addSharedMemory("model/bn2/var", mem3, 1024);
	mem3 += 1024;
	memhub.addSharedMemory("model/bn2/mean", mem3, 1024);
	mem3 += 1024;
	
	memhub.addSharedMemory("model/fc3/weight", mem3, 1024 * 1024 * sizeof(std::int8_t) / sizeof(float));
	mem3 += 1024 * 1024 * sizeof(std::int8_t) / sizeof(float);
	memhub.addSharedMemory("model/bn3/bias", mem3, 1024);
	mem3 += 1024;
	memhub.addSharedMemory("model/bn3/weight", mem3, 1024);
	mem3 += 1024;
	memhub.addSharedMemory("model/bn3/var", mem3, 1024);
	mem3 += 1024;
	memhub.addSharedMemory("model/bn3/mean", mem3, 1024);
	mem3 += 1024;

	//create activation data memory mapping
	float* mem1 = memhub.getMemory("activation_mem1").getPtr();
	float* mem2 = memhub.getMemory("activation_mem2").getPtr();

	memhub.addSharedMemory("fc1/in", mem1, 1024);
	memhub.addSharedMemory("fc1/out|sign1/in", mem2, 1024);
	memhub.addSharedMemory("sign1/out|fc2/in", mem1, 1024);
	memhub.addSharedMemory("fc2/out|sign2/in", mem2, 1024);
	memhub.addSharedMemory("sign2/out|fc3/in", mem1, 1024);
	memhub.addSharedMemory("fc3/out", mem2, 1024);

	return 1;
}

int Core::op_input(std::string fromMemName, std::string toMemName)
{
	std::uint8_t* from = (std::uint8_t*)memhub.getMemory(fromMemName).getPtr();
	std::int8_t* to = (std::int8_t*)memhub.getMemory(toMemName).getPtr();
	for (int i = 0; i < 784; i++)
	{
		to[i] = from[i] > 0 ? 1 : 0;
	}

	return 10;
}

int Core::op_output(std::string memName)
{
	float *ptr = memhub.getMemory(memName).getPtr();
	outputIndex = std::max_element(ptr, ptr + 10) - ptr;
	return 10;
}

int Core::op_fc_calculate(
	std::string inMemName,
	std::string outMemName,
	std::string fc_weightMemName,
	std::string bn_varMemName,
	std::string bn_meanMemName,
	std::string bn_weightMemName,
	std::string bn_biasMemName,
	RRAM_Array& array,
	bool load_weight
)
{
	Memory &in = memhub.getMemory(inMemName);
	Memory &out = memhub.getMemory(outMemName);
	Memory &fc_w = memhub.getMemory(fc_weightMemName);
	Memory &bn_var = memhub.getMemory(bn_varMemName);
	Memory &bn_mean = memhub.getMemory(bn_meanMemName);
	Memory &bn_w = memhub.getMemory(bn_weightMemName);
	Memory &bn_b = memhub.getMemory(bn_biasMemName);

	int cycles = 0;

	if (load_weight)
	{
		cycles += array.load_weight((std::int8_t(*)[1024])(fc_w.getPtr()),
									 (bn_mean.getPtr()),
									 bn_var.getPtr(),
									 bn_w.getPtr(),
									 bn_b.getPtr());
	}
	cycles += array.calculate((std::int8_t*)(in.getPtr()), out.getPtr());

	return cycles;
}

int Core::op_sign(
	std::string inMemName,
	std::string outMemName,
	RRAM_Array& array
)
{
	Memory &in = memhub.getMemory(inMemName);
	Memory &out = memhub.getMemory(outMemName);

	int cycles = 0;

	cycles += array.sign(in.getPtr(), (std::int8_t*)(out.getPtr()));

	return cycles;
}
