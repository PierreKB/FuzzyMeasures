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

	void FindFuzzyMeasures(int iterationNumber);
	void ReadVariablesValuesFromFile(const std::string& filename);


	FuzzyMeasureSpace space();
	std::unordered_map<uint, float> coefficients();

	void DisplayLattice();

private:
	void ComputeLattice();

	std::unordered_map<float, std::vector<float>> m_lattice;
	std::unordered_map<uint, float> m_coefficients;


	std::vector<std::map<uint, float>> m_sampleVariablesValues;
	std::vector<float> m_sampleChoquetIntegral;

	FuzzyMeasureSpace m_space;
	FileManager m_fileManager;
};