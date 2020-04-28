#ifndef FUZZY_MEASURE_H
#define FUZZY_MEASURE_H

#include "Universe.h"
#include "LEARNING_DATA.h"
#include "PARAMS.h"

class FuzzyMeasure
{
public:
	FuzzyMeasure(LEARNING_DATA& data);
	FuzzyMeasure(std::vector<std::string>& elements, std::unordered_map<BINSETREP, float>& coefficients);

	~FuzzyMeasure();

	float ChoquetIntegral(
		std::unordered_map<BINSETREP, float>& m_coeffs,
		std::map<BINSETREP, float>& values,
		std::vector<BINSETREP>& path,
		std::vector<float>& sortedValues
		);

	void FindFuzzyMeasuresUntil(int iterationNumber, std::vector<float>& errors, PARAMS params = PARAMS());
	void FindFuzzyMeasuresUntil(float errorLimit, int iterationLimit, std::vector<float>& errors, PARAMS params = PARAMS());

	void Evaluate(LEARNING_DATA& data, std::vector<float>& output);

	void ComputeImportanceIndex();

	Universe universe();

	std::unordered_map<BINSETREP, float> coefficients();
	std::unordered_map<BINSETREP, float> importanceIndex();

	void CheckLatticeMonotocy();


private:
	void InitMeasure();
	void Reset();

	void FindFuzzyMeasures(std::vector<float>& errors, float alpha, float beta);
	void AjustUnmodifiedNodes(float beta);

	void ComputeErrorMax();

	std::unordered_map<BINSETREP, float> coefficients_;
	std::unordered_map<BINSETREP, float> importanceIndex_;

	Universe universe_;
	LEARNING_DATA data_;

	std::vector<BINSETREP> unmodified_;
	float errorMax_;
};

#endif