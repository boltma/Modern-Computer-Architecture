#pragma once
#include <string>
#include <unordered_map>
#include <memory>

#include "memory.h"



class MemHub
{
public:
	void addMemory(
		std::string memName,
		int xLen = 1,
		int yLen = 1,
		int zLen = 1,
		int wLen = 1
	);
	void addSharedMemory(
		std::string memName,
		float* data,
		int xLen = 1,
		int yLen = 1,
		int zLen = 1,
		int wLen = 1
	);
	Memory& getMemory(std::string memName);
private:
	std::unordered_map<std::string, std::unique_ptr<Memory>> _memories;
};
