#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS // disable error C4996 in visual studio
#endif

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "../../../util/m5/m5op.h"
#include "peripheral.h"

#define TOTAL_NUM 300 // no more than 5135

std::int8_t fc1_w[1024][1024] = {0};
std::int8_t fc2_w[1024][1024] = {0};
std::int8_t fc3_w[1024][1024] = {0};

float bn1_b[1024] = {0};
float bn1_w[1024] = {0};
float bn1_var[1024] = {0};
float bn1_mean[1024] = {0};

float bn2_b[1024] = {0};
float bn2_w[1024] = {0};
float bn2_var[1024] = {0};
float bn2_mean[1024] = {0};

float bn3_b[1024] = {0};
float bn3_w[1024] = {0};
float bn3_var[1024] = {0};
float bn3_mean[1024] = {0};

// Overall weight, only load once
std::int8_t fc_w[1024][1024] = {0};
float bn_b[1024] = {0};
float bn_w[1024] = {0};
float bn_var[1024] = {0};
float bn_mean[1024] = {0};

void load_weights()
{
	// load weight data from files
	FILE *fdata = NULL;

	int fc1_row_bias = 10, fc1_col_bias = 0;
	int fc2_row_bias = 768, fc2_col_bias = fc1_row_bias;
	int fc3_row_bias = 0, fc3_col_bias = fc2_row_bias;

	fdata = fopen("data/fc1_weight.txt", "r");
	assert(fdata != NULL);
	for (int i = fc1_row_bias; i < fc1_row_bias + 64; i++)
	{
		for (int j = fc1_col_bias; j < fc1_col_bias + 784; j++)
		{
			fscanf(fdata, "%hhd,", &fc1_w[i][j]);
		}
	}
	fclose(fdata);

	fdata = fopen("data/fc2_weight.txt", "r");
	assert(fdata != NULL);
	for (int i = fc2_row_bias; i < fc2_row_bias + 256; i++)
	{
		for (int j = fc2_col_bias; j < fc2_col_bias + 64; j++)
		{
			fscanf(fdata, "%hhd,", &fc2_w[i][j]);
		}
	}
	fclose(fdata);

	fdata = fopen("data/fc3_weight.txt", "r");
	assert(fdata != NULL);
	for (int i = fc3_row_bias; i < fc3_row_bias + 10; i++)
	{
		for (int j = fc3_col_bias; j < fc3_col_bias + 256; j++)
		{
			fscanf(fdata, "%hhd,", &fc3_w[i][j]);
		}
	}
	fclose(fdata);

	fdata = fopen("data/bn1_bias.txt", "r");
	assert(fdata != NULL);
	for (int i = fc1_row_bias; i < fc1_row_bias + 64; i++)
	{
		fscanf(fdata, "%f,", &bn1_b[i]);
	}
	fclose(fdata);

	fdata = fopen("data/bn1_weight.txt", "r");
	assert(fdata != NULL);
	for (int i = fc1_row_bias; i < fc1_row_bias + 64; i++)
	{
		fscanf(fdata, "%f,", &bn1_w[i]);
	}
	fclose(fdata);

	fdata = fopen("data/bn1_var.txt", "r");
	assert(fdata != NULL);
	for (int i = fc1_row_bias; i < fc1_row_bias + 64; i++)
	{
		fscanf(fdata, "%f,", &bn1_var[i]);
	}
	fclose(fdata);

	fdata = fopen("data/bn1_mean.txt", "r");
	assert(fdata != NULL);
	for (int i = fc1_row_bias; i < fc1_row_bias + 64; i++)
	{
		fscanf(fdata, "%f,", &bn1_mean[i]);
	}
	fclose(fdata);

	fdata = fopen("data/bn2_bias.txt", "r");
	assert(fdata != NULL);
	for (int i = fc2_row_bias; i < fc2_row_bias + 256; i++)
	{
		fscanf(fdata, "%f,", &bn2_b[i]);
	}
	fclose(fdata);

	fdata = fopen("data/bn2_weight.txt", "r");
	assert(fdata != NULL);
	for (int i = fc2_row_bias; i < fc2_row_bias + 256; i++)
	{
		fscanf(fdata, "%f,", &bn2_w[i]);
	}
	fclose(fdata);

	fdata = fopen("data/bn2_var.txt", "r");
	assert(fdata != NULL);
	for (int i = fc2_row_bias; i < fc2_row_bias + 256; i++)
	{
		fscanf(fdata, "%f,", &bn2_var[i]);
	}
	fclose(fdata);

	fdata = fopen("data/bn2_mean.txt", "r");
	assert(fdata != NULL);
	for (int i = fc2_row_bias; i < fc2_row_bias + 256; i++)
	{
		fscanf(fdata, "%f,", &bn2_mean[i]);
	}
	fclose(fdata);

	fdata = fopen("data/bn3_bias.txt", "r");
	assert(fdata != NULL);
	for (int i = fc3_row_bias; i < fc3_row_bias + 10; i++)
	{
		fscanf(fdata, "%f,", &bn3_b[i]);
	}
	fclose(fdata);

	fdata = fopen("data/bn3_weight.txt", "r");
	assert(fdata != NULL);
	for (int i = fc3_row_bias; i < fc3_row_bias + 10; i++)
	{
		fscanf(fdata, "%f,", &bn3_w[i]);
	}
	fclose(fdata);

	fdata = fopen("data/bn3_var.txt", "r");
	assert(fdata != NULL);
	for (int i = fc3_row_bias; i < fc3_row_bias + 10; i++)
	{
		fscanf(fdata, "%f,", &bn3_var[i]);
	}
	fclose(fdata);

	fdata = fopen("data/bn3_mean.txt", "r");
	assert(fdata != NULL);
	for (int i = fc3_row_bias; i < fc3_row_bias + 10; i++)
	{
		fscanf(fdata, "%f,", &bn3_mean[i]);
	}
	fclose(fdata);

	/*****************************/
	/* Put all weights together, actually no need to use code above
	/*****************************/

	for (int i = fc1_row_bias; i < fc1_row_bias + 64; i++)
	{
		for (int j = fc1_col_bias; j < fc1_col_bias + 784; j++)
		{
			fc_w[i][j] = fc1_w[i][j];
		}
	}

	for (int i = fc2_row_bias; i < fc2_row_bias + 256; i++)
	{
		for (int j = fc2_col_bias; j < fc2_col_bias + 64; j++)
		{
			fc_w[i][j] = fc2_w[i][j];
		}
	}

	for (int i = fc3_row_bias; i < fc3_row_bias + 10; i++)
	{
		for (int j = fc3_col_bias; j < fc3_col_bias + 256; j++)
		{
			fc_w[i][j] = fc3_w[i][j];
		}
	}

	for (int i = fc1_row_bias; i < fc1_row_bias + 64; i++)
	{
		bn_b[i] = bn1_b[i];
		bn_w[i] = bn1_w[i];
		bn_var[i] = bn1_var[i];
		bn_mean[i] = bn1_mean[i];
	}

	for (int i = fc2_row_bias; i < fc2_row_bias + 256; i++)
	{
		bn_b[i] = bn2_b[i];
		bn_w[i] = bn2_w[i];
		bn_var[i] = bn2_var[i];
		bn_mean[i] = bn2_mean[i];
	}

	for (int i = fc3_row_bias; i < fc3_row_bias + 10; i++)
	{
		bn_b[i] = bn3_b[i];
		bn_w[i] = bn3_w[i];
		bn_var[i] = bn3_var[i];
		bn_mean[i] = bn3_mean[i];
	}
}

int main(int argc, char **argv)
{
	bool load_once = (argc == 2);

	/*****************************/
	m5_dumpreset_stats(0, 0);
	/*****************************/

	printf("Program Start.\n");

	volatile uint8_t *data = (uint8_t *)mmap(PERI_ADDR[0], 10 * 1024 * 1024, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	printf("Peripherals Registered.\n");

	periInit(data, 0);
	printf("Initiated.\n");

	load_weights();
	printf("weights loaded.\n");

	// Send weights to vdev
	size_t vdev_offset = 2 + 1024 * 1024 * 2;

	if (load_once)
	{
		/*****************************/
		/* Only load weight once
		/*****************************/

		periWrite(data, vdev_offset, fc_w, 1024 * 1024 * sizeof(std::int8_t));
		vdev_offset += 1024 * 1024 * sizeof(std::int8_t);
		printf("fc_w Written to Vdev.\n");

		periWrite(data, vdev_offset, bn_b, 1024 * sizeof(float));
		vdev_offset += 1024 * sizeof(float);
		printf("bn_b Written to Vdev.\n");

		periWrite(data, vdev_offset, bn_w, 1024 * sizeof(float));
		vdev_offset += 1024 * sizeof(float);
		printf("bn_w Written to Vdev.\n");

		periWrite(data, vdev_offset, bn_var, 1024 * sizeof(float));
		vdev_offset += 1024 * sizeof(float);
		printf("bn_var Written to Vdev.\n");

		periWrite(data, vdev_offset, bn_mean, 1024 * sizeof(float));
		vdev_offset += 1024 * sizeof(float);
		printf("bn_mean Written to Vdev.\n");
	}
	else
	{
		periWrite(data, vdev_offset, fc1_w, 1024 * 1024 * sizeof(std::int8_t));
		vdev_offset += 1024 * 1024 * sizeof(std::int8_t);
		printf("fc1_w Written to Vdev.\n");

		periWrite(data, vdev_offset, bn1_b, 1024 * sizeof(float));
		vdev_offset += 1024 * sizeof(float);
		printf("bn1_b Written to Vdev.\n");

		periWrite(data, vdev_offset, bn1_w, 1024 * sizeof(float));
		vdev_offset += 1024 * sizeof(float);
		printf("bn1_w Written to Vdev.\n");

		periWrite(data, vdev_offset, bn1_var, 1024 * sizeof(float));
		vdev_offset += 1024 * sizeof(float);
		printf("bn1_var Written to Vdev.\n");

		periWrite(data, vdev_offset, bn1_mean, 1024 * sizeof(float));
		vdev_offset += 1024 * sizeof(float);
		printf("bn1_mean Written to Vdev.\n");

		periWrite(data, vdev_offset, fc2_w, 1024 * 1024 * sizeof(std::int8_t));
		vdev_offset += 1024 * 1024 * sizeof(std::int8_t);
		printf("fc2_w Written to Vdev.\n");

		periWrite(data, vdev_offset, bn2_b, 1024 * sizeof(float));
		vdev_offset += 1024 * sizeof(float);
		printf("bn2_b Written to Vdev.\n");

		periWrite(data, vdev_offset, bn2_w, 1024 * sizeof(float));
		vdev_offset += 1024 * sizeof(float);
		printf("bn2_w Written to Vdev.\n");

		periWrite(data, vdev_offset, bn2_var, 1024 * sizeof(float));
		vdev_offset += 1024 * sizeof(float);
		printf("bn2_var Written to Vdev.\n");

		periWrite(data, vdev_offset, bn2_mean, 1024 * sizeof(float));
		vdev_offset += 1024 * sizeof(float);
		printf("bn2_mean Written to Vdev.\n");

		periWrite(data, vdev_offset, fc3_w, 1024 * 1024 * sizeof(std::int8_t));
		vdev_offset += 1024 * 1024 * sizeof(std::int8_t);
		printf("fc3_w Written to Vdev.\n");

		periWrite(data, vdev_offset, bn3_b, 1024 * sizeof(float));
		vdev_offset += 1024 * sizeof(float);
		printf("bn3_b Written to Vdev.\n");

		periWrite(data, vdev_offset, bn3_w, 1024 * sizeof(float));
		vdev_offset += 1024 * sizeof(float);
		printf("bn3_w Written to Vdev.\n");

		periWrite(data, vdev_offset, bn3_var, 1024 * sizeof(float));
		vdev_offset += 1024 * sizeof(float);
		printf("bn3_var Written to Vdev.\n");

		periWrite(data, vdev_offset, bn3_mean, 1024 * sizeof(float));
		vdev_offset += 1024 * sizeof(float);
		printf("bn3_mean Written to Vdev.\n");
	}

	FILE *fdata = fopen("data/images-mnist-test-01347", "rb");
	assert(fdata != NULL);

	/*****************************/
	m5_dumpreset_stats(0, 0);
	/*****************************/

	int finalresult[TOTAL_NUM];
	for (int count = 0; count < TOTAL_NUM; count++)
	{
		std::uint8_t input_byte[784];
		fread(input_byte, sizeof(input_byte), 1, fdata); //read input img
		periWrite(data, 2 + 1024 * 1024, input_byte, sizeof(input_byte));

		for (int j = 1; j <= 7; j++)
		{
			periInit(data, j);
		}

		// get output
		assert(periIsFinished(data));
		periRead(data, 2, finalresult + count, sizeof(unsigned char));
		printf("Got result: %d\n", finalresult[count]);	
	}
	fclose(fdata);
	periLogout(0);

	/*****************************/
	m5_dumpreset_stats(0, 0);
	/*****************************/

	/**********************************************************/

	uint8_t target_list[TOTAL_NUM];
	fdata = fopen("data/labels-mnist-test-01347", "rb"); // read ground truth
	assert(fdata != NULL);
	fread(target_list, sizeof(target_list), 1, fdata);
	fclose(fdata);
	int correct = 0;
	for (int i = 0; i < TOTAL_NUM; i++)
	{
		if (finalresult[i] == target_list[i])
		{
			correct++;
		}
	}
	printf("accu: %f\n", (double)correct / (double)TOTAL_NUM);

	return 0;
}
