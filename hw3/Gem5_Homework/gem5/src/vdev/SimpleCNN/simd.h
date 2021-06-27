#pragma once

struct Data
{
	float data[4];
	Data(float a, float b, float c, float d)
	{
		data[0] = a;
		data[1] = b;
		data[2] = c;
		data[3] = d;
	}
};


// Multiply Accumulate
// Return the cycles needed for calculation
inline int mac(Data& sum, const Data& in1, const Data& in2)
{
	for (int i = 0; i < 4; i++)
	{
		sum.data[i] += in1.data[i] * in2.data[i];
	}
	return 1;
}

// Return the cycles needed for calculation
inline int add(Data& sum, const Data& in)
{
	for (int i = 0; i < 4; i++)
	{
		sum.data[i] += in.data[i];
	}
	return 1;
}

// Rectified Linear Unit
// Return the cycles needed for calculation
inline int relu(Data& sum)
{
	for (int i = 0; i < 4; i++)
	{
		sum.data[i] = std::max(sum.data[i], 0.f);
	}
	return 1;
}

// Return the cycles needed for calculation
inline int max(Data& a, const Data& b)
{
	for (int i = 0; i < 4; i++)
	{
		a.data[i] = std::max(a.data[i], b.data[i]);
	}
	return 1;
}