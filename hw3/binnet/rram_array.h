#pragma once
#include <cstdint>
#include <cstring>
#include <cmath>

class RRAM_Array
{
public:
	// return the cycles it takes to complete this operation
	int load_weight(
		std::int8_t weight[1024][1024],
		float batchnorm_mean[1024],
		float batchnorm_var[1024],
		float batchnorm_weight[1024],
		float batchnorm_bias[1024]
		) 
	{
		for (int i = 0; i < 1024; i++)
		{
			for (int j = 0; j < 1024; j++)
			{
				weight_positive_[i][j] = weight_negtive_[i][j] = 0;
				if (weight[i][j] > 0)
				{
					weight_positive_[i][j] = 1;
				}
				if (weight[i][j] < 0)
				{
					weight_negtive_[i][j] = 1;
				}
			}
		}
		std::memcpy(batchnorm_mean_, batchnorm_mean, 1024);
		std::memcpy(batchnorm_var_, batchnorm_var, 1024);
		std::memcpy(batchnorm_weight_, batchnorm_weight, 1024);
		std::memcpy(batchnorm_bias_, batchnorm_bias, 1024);
		return 100000;
	}
	// return the cycles it takes to complete this operation
	int calculate(std::int8_t input[1024], float output[1024])
	{
		for (int i = 0; i < 1024; i++)
		{
			float value = 0;
			for (int j = 0; j < 1024; j++)
			{
				value += input[j] * weight_positive_[i][j];
			}
			for (int j = 0; j < 1024; j++)
			{
				value -= input[j] * weight_negtive_[i][j];
			}
			output[i] = (value - batchnorm_mean_[i]) / std::sqrt(batchnorm_var_[i] + 1e-5f)
				* batchnorm_weight_[i] + batchnorm_bias_[i];
		}
		return 100;
	}
	// return the cycles it takes to complete this operation
	int sign(float input[1024], std::int8_t output[1024])
	{
		for (int i = 0; i < 1024; i++)
		{
			output[i] =
				input[i] > 0 ? 1 :
				input[i] < 0 ? -1 :
				0;
		}
		return 1;
	}
private:
	bool weight_positive_[1024][1024];
	bool weight_negtive_[1024][1024];
	float batchnorm_mean_[1024];
	float batchnorm_var_[1024];
	float batchnorm_weight_[1024];
	float batchnorm_bias_[1024];
};