#ifndef EVALUATION_DATA_H
#define EVALUATION_DATA_H

#include "LEARNING_DATA.h"
#include<unordered_map>

struct EVALUATION_DATA
{
	EVALUATION_DATA()
	{}

	std::vector<LEARNING_DATA> data;
	std::vector<std::vector<float>> output;
	std::vector<std::vector<float>> errors;
	std::map<int, std::pair<float, float>> recall;
	std::vector<std::unordered_map<BINSETREP, float>> impindex;
	int k;
	float alpha;
	float beta;
	int iterationNumber;
};

#endif