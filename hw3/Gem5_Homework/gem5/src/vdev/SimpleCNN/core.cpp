#include <cassert>
#include <algorithm>
#include <cstring>
#include <memory>

#include "core.h"
#include "simd.h"

int Core::op_init(void)
{
	/*set up memories*/
	//create weight & bias mem
	float* mem3 = memhub.getMemory("weight_and_bias_mem").getPtr();
	memhub.addSharedMemory("model/conv2d/kernel", mem3, 5, 5, 1, 32);
	mem3 += 5 * 5 * 1 * 32;
	memhub.addSharedMemory("model/conv2d/bias", mem3, 32);
	mem3 += 32;

	memhub.addSharedMemory("model/conv2d_1/kernel", mem3, 5, 5, 32, 64);
	mem3 += 5 * 5 * 32 * 64;
	memhub.addSharedMemory("model/conv2d_1/bias", mem3, 64);
	mem3 += 64;

	memhub.addSharedMemory("model/dense/kernel", mem3, 3136, 512);
	mem3 += 3136 * 512;
	memhub.addSharedMemory("model/dense/bias", mem3, 512);
	mem3 += 512;

	memhub.addSharedMemory("model/dense_1/kernel", mem3, 512, 10);
	mem3 += 512 * 10;
	memhub.addSharedMemory("model/dense_1/bias", mem3, 10);

	//create activation data memory mapping
	float* mem1 = memhub.getMemory("activation_mem1").getPtr();
	float* mem2 = memhub.getMemory("activation_mem2").getPtr();

	memhub.addSharedMemory("data/padding/in", mem1, 28, 28);
	memhub.addSharedMemory("data/padding/out|conv2d/in", mem2, 32, 32);
	memhub.addSharedMemory("data/conv2d/out|pool/in", mem1, 28, 28, 32);
	memhub.addSharedMemory("data/pool/out|padding_1/in", mem2, 14, 14, 32);
	memhub.addSharedMemory("data/padding_1/out|conv2d_1/in", mem1, 18, 18, 32);
	memhub.addSharedMemory("data/conv2d_1/out|pool_1/in", mem2, 14, 14, 64);
	memhub.addSharedMemory("data/pool_1/out|dense/in", mem1, 7, 7, 64);
	memhub.addSharedMemory("data/dense/out|dense_1/in", mem2, 512);
	memhub.addSharedMemory("data/dense_1/out", mem1, 10);

	return 1;
}

int Core::op_input(std::string fromMemName, std::string toMemName)
{
	unsigned char* from = (unsigned char*)memhub.getMemory(fromMemName).getPtr();
	float* to = memhub.getMemory(toMemName).getPtr();
	for (int x = 0; x < 28; ++x)
	{
		for (int y = 0; y < 28; y++)
		{
			to[x * 28 + y] = float(from[x * 28 + y]) / 255.f - 0.5f;
		}
	}

	return 10;
}

int Core::op_output(std::string memName)
{
	float *ptr = memhub.getMemory(memName).getPtr();
	outputIndex = std::max_element(ptr, ptr + 10) - ptr;
	return 10;
}

int Core::op_conv(
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
)
{
	Memory &in = memhub.getMemory(inMemName);
	Memory &out = memhub.getMemory(outMemName);
	Memory &weight = memhub.getMemory(weightMemName);
	Memory &bias = memhub.getMemory(biasMemName);

	int cycles = 0;

	for (int outX = outXRange.begin; outX < outXRange.end; outX++)
	{
		for (int outY = outYRange.begin; outY < outYRange.end; outY++)
		{
			for (int outChannel = outChannelRange.begin; outChannel < outChannelRange.end; outChannel += 4)
			{
				Data sum(0.f, 0.f, 0.f, 0.f);

				for (int wX = 0; wX < filterSize; wX++)
				{
					const int inX = outX * stride + wX;
					for (int wY = 0; wY < filterSize; wY++)
					{
						const int inY = outY * stride + wY;

						for (int inChannel = 0; inChannel < inChannelNum; inChannel++)
						{
							Data w(
								*weight.getPtr(wX, wY, inChannel, outChannel),
								*weight.getPtr(wX, wY, inChannel, outChannel + 1),
								*weight.getPtr(wX, wY, inChannel, outChannel + 2),
								*weight.getPtr(wX, wY, inChannel, outChannel + 3)
							);
							Data i(
								*in.getPtr(inX, inY, inChannel),
								*in.getPtr(inX, inY, inChannel),
								*in.getPtr(inX, inY, inChannel),
								*in.getPtr(inX, inY, inChannel)
							);
							cycles += mac(sum, i, w);
						}
					}
				}
				//add bias
				Data b(
					*bias.getPtr(outChannel),
					*bias.getPtr(outChannel + 1),
					*bias.getPtr(outChannel + 2),
					*bias.getPtr(outChannel + 3)
				);
				cycles += add(sum, b);

				//ReLU
				cycles += relu(sum);

				//output
				*out.getPtr(outX, outY, outChannel) = sum.data[0];
				*out.getPtr(outX, outY, outChannel + 1) = sum.data[1];
				*out.getPtr(outX, outY, outChannel + 2) = sum.data[2];
				*out.getPtr(outX, outY, outChannel + 3) = sum.data[3];
			}
		}
	}

	// we assume we have 64 simd ALU unit running parallel
	return cycles / 64;
}

int Core::op_fc(
	std::string inMemName, 
	std::string outMemName, 
	std::string weightMemName,
	std::string biasMemName,
	int inDataLength, 
	Range outRange
)
{
	Memory &in = memhub.getMemory(inMemName);
	Memory &out = memhub.getMemory(outMemName);
	Memory &weight = memhub.getMemory(weightMemName);
	Memory &bias = memhub.getMemory(biasMemName);

	int cycles = 0;

	for (int outIndex = outRange.begin; outIndex < outRange.end; outIndex += 4)
	{
		Data sum(0.f, 0.f, 0.f, 0.f);
		for (int inIndex = 0; inIndex < inDataLength; inIndex++)
		{
			Data i(
				in.getPtr()[inIndex],
				in.getPtr()[inIndex],
				in.getPtr()[inIndex],
				in.getPtr()[inIndex]
			);
			Data w(
				*weight.getPtr(inIndex, outIndex),
				*weight.getPtr(inIndex, outIndex + 1),
				outIndex + 4 > outRange.end ? 0.f : *weight.getPtr(inIndex, outIndex + 2),
				outIndex + 4 > outRange.end ? 0.f : *weight.getPtr(inIndex, outIndex + 3)
			);

			cycles += mac(sum, i, w);
		}
		
		//add bias
		Data b(
			*bias.getPtr(outIndex),
			*bias.getPtr(outIndex + 1),
			outIndex + 4 > outRange.end ? 0.f : *bias.getPtr(outIndex + 2),
			outIndex + 4 > outRange.end ? 0.f : *bias.getPtr(outIndex + 3)
		);
		cycles += add(sum, b);

		//ReLU
		cycles += relu(sum);
		//output
		*out.getPtr(outIndex) = sum.data[0];
		*out.getPtr(outIndex + 1) = sum.data[1];
		if (outIndex + 4 <= outRange.end)
		{
			*out.getPtr(outIndex + 2) = sum.data[2];
			*out.getPtr(outIndex + 3) = sum.data[3];
		}
	}

	// we assume we have 64 simd ALU unit running parallel
	return cycles / 64;
}

int Core::op_pool(
	std::string inMemName, 
	std::string outMemName, 
	Range outChannelRange, 
	Range outXRange,
	Range outYRange, 
	int filterSize, 
	int stride
)
{
	Memory &in = memhub.getMemory(inMemName);
	Memory &out = memhub.getMemory(outMemName);

	int cycles = 0;

	for (int outX = outXRange.begin; outX < outXRange.end; outX++)
	{
		for (int outY = outYRange.begin; outY < outYRange.end; outY++)
		{
			for (int z = outChannelRange.begin; z < outChannelRange.end; z += 4)
			{
				Data m(
					std::numeric_limits<float>::min(),
					std::numeric_limits<float>::min(),
					std::numeric_limits<float>::min(),
					std::numeric_limits<float>::min()
				);

				for (int inX = outX * stride; inX < outX * stride + filterSize; inX++)
				{
					for (int inY = outY * stride; inY < outY * stride + filterSize; inY++)
					{
						Data i(
							*in.getPtr(inX, inY, z),
							*in.getPtr(inX, inY, z + 1),
							*in.getPtr(inX, inY, z + 2),
							*in.getPtr(inX, inY, z + 3)
						);
						cycles += max(m, i);
					}
				}
				*out.getPtr(outX, outY, z) = m.data[0];
				*out.getPtr(outX, outY, z + 1) = m.data[1];
				*out.getPtr(outX, outY, z + 2) = m.data[2];
				*out.getPtr(outX, outY, z + 3) = m.data[3];
			}
		}
	}

	// we assume we have 64 simd ALU unit running parallel
	return cycles / 64;
}

int Core::op_padding(
	std::string inMemName, 
	std::string outMemName, 
	int padLength, 
	int inXLen, 
	int inYLen, 
	int inZLen,
	int inWLen
)
{
	Memory &in = memhub.getMemory(inMemName);
	Memory &out = memhub.getMemory(outMemName);

	//first, make output memory empty
	std::memset(out.getPtr(), 0, out.getMemSizeByByte());

	for (int x = 0; x < inXLen; x++)
	{
		for (int y = 0; y < inYLen;	y++)
		{
			for (int z = 0; z < inZLen; z++)
			{
				for (int w = 0; w < inWLen; w++)
				{
					*out.getPtr(x + padLength, y + padLength, z, w) =
						*in.getPtr(x, y, z, w);
				}
			}
		}
	}

	return 1;
}
