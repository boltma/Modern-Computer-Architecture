#pragma once
#include<memory>


class Memory
{
public:
	Memory(int xLen, int yLen = 1, int zLen = 1, int wLen = 1);

	float* getPtr(int xIndex = 0, int yIndex = 0, int zIndex = 0, int wIndex = 0) const;
	int getMemSizeByByte(void) const;
	int getMemSizeByFloat(void) const;
protected:
	Memory() {}

	float* _data;
	std::unique_ptr<float[]> _dataSource;
	int _xLen, _yLen, _zLen, _wLen;
	int _wzLen, _wzyLen, _wzyxLen;
};

class SharedMemory: public Memory
{
public:
	SharedMemory(float* data, int xLen = 1, int yLen = 1, int zLen = 1, int wLen = 1);
};