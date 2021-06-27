#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS // disable error C4996 in visual studio
#endif

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cassert> 
#include <algorithm>

#include "rram_array.h"

#define TOTAL_NUM 300 // no more than 5135

std::int8_t fc1_w[1024][1024] = { 0 };
std::int8_t fc2_w[1024][1024] = { 0 };
std::int8_t fc3_w[1024][1024] = { 0 };

float bn1_b[1024] = { 0 };
float bn1_w[1024] = { 0 };
float bn1_var[1024] = { 0 };
float bn1_mean[1024] = { 0 };

float bn2_b[1024] = { 0 };
float bn2_w[1024] = { 0 };
float bn2_var[1024] = { 0 };
float bn2_mean[1024] = { 0 };

float bn3_b[1024] = { 0 };
float bn3_w[1024] = { 0 };
float bn3_var[1024] = { 0 };
float bn3_mean[1024] = { 0 };

void load_weights()
{
	// load weight data from files
	FILE* fdata = NULL;

	fdata = fopen("data/fc1_weight.txt", "r");
	assert(fdata != NULL);
	for (int i = 0; i < 64; i++)
	{
		for (int j = 0; j < 784; j++)
		{
			fscanf(fdata, "%hhd,", &fc1_w[i][j]);
		}
	}
	fclose(fdata);

	fdata = fopen("data/fc2_weight.txt", "r");
	assert(fdata != NULL);
	for (int i = 0; i < 256; i++)
	{
		for (int j = 0; j < 64; j++)
		{
			fscanf(fdata, "%hhd,", &fc2_w[i][j]);
		}
	}
	fclose(fdata);

	fdata = fopen("data/fc3_weight.txt", "r");
	assert(fdata != NULL);
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 256; j++)
		{
			fscanf(fdata, "%hhd,", &fc3_w[i][j]);
		}
	}
	fclose(fdata);

	fdata = fopen("data/bn1_bias.txt", "r");
	assert(fdata != NULL);
	for (int i = 0; i < 64; i++)
	{
		fscanf(fdata, "%f,", &bn1_b[i]);
	}
	fclose(fdata);

	fdata = fopen("data/bn1_weight.txt", "r");
	assert(fdata != NULL);
	for (int i = 0; i < 64; i++)
	{
		fscanf(fdata, "%f,", &bn1_w[i]);
	}
	fclose(fdata);

	fdata = fopen("data/bn1_var.txt", "r");
	assert(fdata != NULL);
	for (int i = 0; i < 64; i++)
	{
		fscanf(fdata, "%f,", &bn1_var[i]);
	}
	fclose(fdata);

	fdata = fopen("data/bn1_mean.txt", "r");
	assert(fdata != NULL);
	for (int i = 0; i < 64; i++)
	{
		fscanf(fdata, "%f,", &bn1_mean[i]);
	}
	fclose(fdata);

	fdata = fopen("data/bn2_bias.txt", "r");
	assert(fdata != NULL);
	for (int i = 0; i < 256; i++)
	{
		fscanf(fdata, "%f,", &bn2_b[i]);
	}
	fclose(fdata);

	fdata = fopen("data/bn2_weight.txt", "r");
	assert(fdata != NULL);
	for (int i = 0; i < 256; i++)
	{
		fscanf(fdata, "%f,", &bn2_w[i]);
	}
	fclose(fdata);

	fdata = fopen("data/bn2_var.txt", "r");
	assert(fdata != NULL);
	for (int i = 0; i < 256; i++)
	{
		fscanf(fdata, "%f,", &bn2_var[i]);
	}
	fclose(fdata);

	fdata = fopen("data/bn2_mean.txt", "r");
	assert(fdata != NULL);
	for (int i = 0; i < 256; i++)
	{
		fscanf(fdata, "%f,", &bn2_mean[i]);
	}
	fclose(fdata);

	fdata = fopen("data/bn3_bias.txt", "r");
	assert(fdata != NULL);
	for (int i = 0; i < 10; i++)
	{
		fscanf(fdata, "%f,", &bn3_b[i]);
	}
	fclose(fdata);

	fdata = fopen("data/bn3_weight.txt", "r");
	assert(fdata != NULL);
	for (int i = 0; i < 10; i++)
	{
		fscanf(fdata, "%f,", &bn3_w[i]);
	}
	fclose(fdata);

	fdata = fopen("data/bn3_var.txt", "r");
	assert(fdata != NULL);
	for (int i = 0; i < 10; i++)
	{
		fscanf(fdata, "%f,", &bn3_var[i]);
	}
	fclose(fdata);

	fdata = fopen("data/bn3_mean.txt", "r");
	assert(fdata != NULL);
	for (int i = 0; i < 10; i++)
	{
		fscanf(fdata, "%f,", &bn3_mean[i]);
	}
	fclose(fdata);
}

int main()
{
	load_weights();

	RRAM_Array * array = new RRAM_Array;

	FILE* fdata = fopen("data/images-mnist-test-01347", "rb");
	assert(fdata != NULL);

	int finalresult[TOTAL_NUM];
	for (int count = 0; count < TOTAL_NUM; count++)
	{
		std::int8_t temp_int8[1024];
		float temp_float[1024];

		std::uint8_t input_byte[784];
		fread(input_byte, sizeof(input_byte), 1, fdata); //read input img
		for (int i = 0; i < 784; i++)
		{
			temp_int8[i] = input_byte[i] > 0 ? 1 : 0;
		}

		// the 1st fully connected layer
		array->load_weight(fc1_w, bn1_mean, bn1_var, bn1_w, bn1_b);
		array->calculate(temp_int8, temp_float);
		array->sign(temp_float, temp_int8);


		// the 2nd fc layer
		array->load_weight(fc2_w, bn2_mean, bn2_var, bn2_w, bn2_b);
		array->calculate(temp_int8, temp_float);
		array->sign(temp_float, temp_int8);

		// the 3rd fc layer
		array->load_weight(fc3_w, bn3_mean, bn3_var, bn3_w, bn3_b);
		array->calculate(temp_int8, temp_float);

		// get output
		finalresult[count] = std::max_element(temp_float, temp_float + 10) - temp_float;
		printf("Got result: %d\n", finalresult[count]);
	}
	delete array;
	fclose(fdata);


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