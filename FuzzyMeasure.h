#pragma once
#include "FuzzyMeasureSpace.h"
#include "FileManager.h"

class FuzzyMeasure
{
public:
	FuzzyMeasure();
	~FuzzyMeasure();

	float ChoquetIntegral(std::unordered_map<uint, float>& m_coeffs, 
		std::map<uint, float>& values, 
		std::vector<uint>& path, 
		std::vector<float>& sortedValues);

	void FindFuzzyMeasuresUntil(int iterationNumber, std::vector<float>& errors, float alpha = 1);
	void FindFuzzyMeasuresUntil(float errorLimit, std::vector<float>& errors, float alpha = 1);

	void FindFuzzyMeasures(std::vector<float>& errors, float alpha);
	void ComputeImportanceIndex();

	void CheckLatticeMonotocy();

	void ReadVariablesValuesFromFile(const std::string& filename);


	FuzzyMeasureSpace space();
	std::unordered_map<uint, float> coefficients();
	std::unordered_map<uint, float> importanceIndex();


private:
	void InitMeasureCoefficients(std::vector<uint>& unmodified);
	void ComputeErrorMax();

	void ComputeLattice();

	std::unordered_map<float, std::vector<float>> m_lattice;
	std::unordered_map<uint, float> m_coefficients;
	std::unordered_map<uint, float> m_importanceIndex;


	std::vector<std::map<uint, float>> m_sampleVariablesValues;
	std::vector<float> m_sampleChoquetIntegral;

	FuzzyMeasureSpace m_space;
	FileManager m_fileManager;


	float m_errorMax;
};