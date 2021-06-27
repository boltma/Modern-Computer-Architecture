#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/mman.h>
#include <stdint.h>
#include <assert.h>
#include "../../../util/m5/m5op.h"

#include "peripheral.h"
#include "output_label.h"

#define BATCH 100

#define SIZE_CONV2D_KERNEL (5 * 5 * 1 * 32)
#define SIZE_CONV2D_BIAS 32

#define SIZE_CONV2D1_KERNEL (5 * 5 * 32 * 64)
#define SIZE_CONV2D1_BIAS 64

#define SIZE_DENSE_KERNEL (3136 * 512)
#define SIZE_DENSE_BIAS 512

#define SIZE_DENSE1_KERNEL (512 * 10)
#define SIZE_DENSE1_BIAS 10

int main()
{
	/*****************************/
	m5_dumpreset_stats(0, 0);
	/*****************************/

	printf("Program Start.\n");

	volatile uint8_t *data = (uint8_t *)mmap(PERI_ADDR[0], 10 * 1024 * 1024, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	printf("Peripherals Registered.\n");

	periInit(data, 0);
	printf("Inited.\n");

	FILE *fmodel = NULL;
	fmodel = fopen("data/ModelData", "rb");
	if (NULL == fmodel)
	{
		printf("Error Read: data/ModelData.\n");
		return -1;
	}

	size_t vdev_offset = 2 + 1024 * 1024 * 2;

	float *conv2d_kernel = (float *)malloc(SIZE_CONV2D_KERNEL * sizeof(float));
	fread(conv2d_kernel, sizeof(float), SIZE_CONV2D_KERNEL, fmodel);
	periWrite(data, vdev_offset, conv2d_kernel, SIZE_CONV2D_KERNEL * sizeof(float));
	free(conv2d_kernel);
	vdev_offset += SIZE_CONV2D_KERNEL * sizeof(float);
	printf("conv2d_kernel Writen to Vdev.\n");

	float *conv2d_bias = (float *)malloc(SIZE_CONV2D_BIAS * sizeof(float));
	fread(conv2d_bias, sizeof(float), SIZE_CONV2D_BIAS, fmodel);
	periWrite(data, vdev_offset, conv2d_bias, SIZE_CONV2D_BIAS * sizeof(float));
	free(conv2d_bias);
	vdev_offset += SIZE_CONV2D_BIAS * sizeof(float);
	printf("conv2d_bias Writen to Vdev.\n");

	float *conv2d1_kernel = (float *)malloc(SIZE_CONV2D1_KERNEL * sizeof(float));
	fread(conv2d1_kernel, sizeof(float), SIZE_CONV2D1_KERNEL, fmodel);
	periWrite(data, vdev_offset, conv2d1_kernel, SIZE_CONV2D1_KERNEL * sizeof(float));
	free(conv2d1_kernel);
	vdev_offset += SIZE_CONV2D1_KERNEL * sizeof(float);
	printf("conv2d1_kernel Writen to Vdev.\n");

	float *conv2d1_bias = (float *)malloc(SIZE_CONV2D1_BIAS * sizeof(float));
	fread(conv2d1_bias, sizeof(float), SIZE_CONV2D1_BIAS, fmodel);
	periWrite(data, vdev_offset, conv2d1_bias, SIZE_CONV2D1_BIAS * sizeof(float));
	free(conv2d1_bias);
	vdev_offset += SIZE_CONV2D1_BIAS * sizeof(float);
	printf("conv2d1_bias Writen to Vdev.\n");

	float *dense_kernel = (float *)malloc(SIZE_DENSE_KERNEL * sizeof(float));
	fread(dense_kernel, sizeof(float), SIZE_DENSE_KERNEL, fmodel);
	periWrite(data, vdev_offset, dense_kernel, SIZE_DENSE_KERNEL * sizeof(float));
	free(dense_kernel);
	vdev_offset += SIZE_DENSE_KERNEL * sizeof(float);
	printf("dense_kernel Writen to Vdev.\n");

	float *dense_bias = (float *)malloc(SIZE_DENSE_BIAS * sizeof(float));
	fread(dense_bias, sizeof(float), SIZE_DENSE_BIAS, fmodel);
	periWrite(data, vdev_offset, dense_bias, SIZE_DENSE_BIAS * sizeof(float));
	free(dense_bias);
	vdev_offset += SIZE_DENSE_BIAS * sizeof(float);
	printf("dense_bias Writen to Vdev.\n");

	float *dense1_kernel = (float *)malloc(SIZE_DENSE1_KERNEL * sizeof(float));
	fread(dense1_kernel, sizeof(float), SIZE_DENSE1_KERNEL, fmodel);
	periWrite(data, vdev_offset, dense1_kernel, SIZE_DENSE1_KERNEL * sizeof(float));
	free(dense1_kernel);
	vdev_offset += SIZE_DENSE1_KERNEL * sizeof(float);
	printf("dense1_kernel Writen to Vdev.\n");

	float *dense1_bias = (float *)malloc(SIZE_DENSE1_BIAS * sizeof(float));
	fread(dense1_bias, sizeof(float), SIZE_DENSE1_BIAS, fmodel);
	periWrite(data, vdev_offset, dense1_bias, SIZE_DENSE1_BIAS * sizeof(float));
	free(dense1_bias);
	vdev_offset += SIZE_DENSE1_BIAS * sizeof(float);
	printf("dense1_bias Writen to Vdev.\n");
	
	fclose(fmodel);

	FILE *fdata = NULL;
	fdata = fopen("data/t10k-images.idx3-ubyte", "rb");
	if (NULL == fdata)
	{
		printf("Error Read: data/t10k-images.idx3-ubyte.\n");
		return -1;
	}
	uint8_t header[16];
	fread(header, sizeof(header), 1, fdata); //read mnist data header

	/*****************************/
	m5_dumpreset_stats(0, 0);
	/*****************************/

	uint8_t input[28 * 28];
	uint8_t finalresult[BATCH];
	int count;
	uint8_t j;
	for (count = 0; count < BATCH; count++)
	{
		fread(input, sizeof(input), 1, fdata); //read input img
		periWrite(data, 2 + 1024 * 1024, input, sizeof(input));
		for (j = 1; j <= 10; j++)
		{
			periInit(data, j);
		}
		assert(periIsFinished(data));
		periRead(data, 2, finalresult + count, sizeof(unsigned char));
		printf("Got result: %d\n", finalresult[count]);
	}
	fclose(fdata);
	periLogout(0);

	/*****************************/
	m5_dumpreset_stats(0, 0);
	/*****************************/

	int i;
	int correct = 0;
	for (i = 0; i < BATCH; i++)
	{
		if (finalresult[i] == target_list[i])
			correct++;
	}
	printf("accu: %f\n", (double)correct / (double)BATCH);

	return 0;
}
