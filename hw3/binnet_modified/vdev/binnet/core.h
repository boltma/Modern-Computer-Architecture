#pragma once
#include <fstream>
#include <string>

#include "memhub.h"
#include "tools.h"
#include "simd.h"

class Core
{
public:
	//the return values of operation functions below represent
	//how many cycles it takes
	int op_init();
	int op_input(std::string fromMemName, std::string toMemName);
	int op_output(std::string memName);
	int op_fc_calculate(
		std::string inMemName,
		std::string outMemName,
		std::string fc_weightMemName,
		std::string bn_varMemName,
		std::string bn_meanMemName,
		std::string bn_weightMemName,
		std::string bn_biasMemName,
		RRAM_Array& array,
		bool load_weight = true
	);
	int op_sign(
		std::string inMemName,
		std::string outMemName,
		RRAM_Array& array
	);

	int outputIndex = -1;
	MemHub memhub;
};