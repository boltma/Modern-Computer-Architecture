#pragma once
#include <fstream>
#include <string>

#include "memhub.h"
#include "tools.h"

class Core
{
public:
	//the return values of operation functions below represent
	//how many cycles it takes
	int op_init();
	int op_input(std::string fromMemName, std::string toMemName);
	int op_output(std::string memName);
	int op_conv(
		std::string inMemName,
		std::string outMemName,
		std::string weightMemName,
		std::string biasMemName,
		int inChannelNum,
		Range outChannelRange,
		Range outXRange,
		Range outYRange,
		int filterSize,
		int stride
	);
	int op_fc(
		std::string inMemName,
		std::string outMemName,
		std::string weightMemName,
		std::string biasMemName,
		int inDataLength,
		Range outRange
	);
	int op_pool(
		std::string inMemName,
		std::string outMemName,
		Range outChannelRange,
		Range outXRange,
		Range outYRange,
		int filterSize,
		int stride
	);
	int op_padding(
		std::string inMemName,
		std::string outMemName,
		int padLength,
		int inXLen,
		int inYLen,
		int inZLen,
		int inWLen
	);

	int outputIndex = -1;
	MemHub memhub;
};