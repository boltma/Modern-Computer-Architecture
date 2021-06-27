#include "memhub.h"

void MemHub::addMemory(std::string memName, int xLen, int yLen, int zLen, int wLen)
{
	_memories.emplace(memName, new Memory(xLen, yLen, zLen, wLen));
}

void MemHub::addSharedMemory(std::string memName, float* data, int xLen, int yLen, int zLen, int wLen)
{
	_memories.emplace(memName, new SharedMemory(data, xLen, yLen, zLen, wLen));
}

Memory& MemHub::getMemory(std::string memName)
{
	return *_memories.at(memName);
}
