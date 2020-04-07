#include "FuzzyMeasure.h"
#include "utilities.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

FuzzyMeasure::FuzzyMeasure()
	:m_errorMax(-1.0f)
{}


FuzzyMeasure::~FuzzyMeasure()
{}

float FuzzyMeasure::ChoquetIntegral(std::unordered_map<uint, float>& coeffs, 
	std::map<uint, float>& values, 
	std::vector<uint>& path,
	std::vector<float>& sortedValues)
{
	std::vector<uint> Ai;

	for (uint i = 0; i < values.size(); ++i)
	{
		Ai.push_back(pow(2, i));
	}

	std::sort(Ai.begin(), Ai.end(),
		[&values](uint a, uint b)
		{
			return values[a] < values[b];
		});


	auto getCoeffAi = [&coeffs, &Ai, &path](int i) -> float
	{
		uint set = 0;

		for (uint j = i - 1; j < Ai.size(); ++j)
		{
			set = set | Ai[j];
		}
		path.push_back(set);

		return coeffs[set];
	};
	
	sortedValues.push_back(0);

	for (auto v : values)
	{
		sortedValues.push_back(v.second);
	}

	std::sort(sortedValues.begin(), sortedValues.end());

	float choquet = 0.0f;

	for (int i = 1; i <= values.size(); ++i)
	{
		choquet += ((sortedValues[i] - sortedValues[i - 1]) * getCoeffAi(i));
	}

	path.push_back(0);

	return choquet;
}

void FuzzyMeasure::FindFuzzyMeasuresUntil(int iterationNumber, std::vector<float>& errors, float alpha)
{
	std::vector<uint> unmodified;

	InitMeasureCoefficients(unmodified);
	ComputeErrorMax();

	for (int n = 0; n < iterationNumber; ++n)
	{
		FindFuzzyMeasures(errors, alpha);
	}
}

void FuzzyMeasure::FindFuzzyMeasuresUntil(float errorLimit, std::vector<float>& errors, float alpha)
{
	float limit = INFINITY;

	std::vector<uint> unmodified;

	InitMeasureCoefficients(unmodified);
	ComputeErrorMax();


	while(limit > errorLimit)
	{
		FindFuzzyMeasures(errors, alpha);

		limit = errors.back();
	}
}

void FuzzyMeasure::FindFuzzyMeasures(std::vector<float>& errors, float alpha)
{
	auto lowerNeighbors = m_space.lowerNeighbors();
	auto upperNeighbors = m_space.upperNeighbors();

	std::vector<uint> path;
	std::vector<float> sortedValues;

	float MSE = 0.0f;
	
	for (int i = 0; i < m_sampleVariablesValues.size(); ++i)
	{
		path.clear();
		sortedValues.clear();

		float error = ChoquetIntegral(m_coefficients, m_sampleVariablesValues[i], path, sortedValues) - m_sampleChoquetIntegral[i];
		MSE += pow(error, 2);

		if (error > 0)
		{
			int j = 1;

			for (int p = path.size() - 2; p > 0; --p)
			{
				m_coefficients[path[p]] = m_coefficients[path[p]] - ( alpha * (error / m_errorMax) * (sortedValues[(sortedValues.size() - 1) - j] - sortedValues[(sortedValues.size() - 1) - j - 1]) );

				for (auto ln : lowerNeighbors[path[p]])
				{
					if (m_coefficients[path[p]] < m_coefficients[ln])
					{
						m_coefficients[path[p]] = m_coefficients[ln];
					}
				}

				++j;
			}
		}
		else if (error < 0)
		{
			int j = path.size() - 2;

			for (int p = 1; p < path.size() - 1; ++p)
			{
				m_coefficients[path[p]] = m_coefficients[path[p]] - (alpha * (error / m_errorMax) * (sortedValues[(sortedValues.size() - 1) - j] - sortedValues[(sortedValues.size() - 1) - j - 1]));

				for (auto un : upperNeighbors[path[p]])
				{
					if (m_coefficients[path[p]] > m_coefficients[un])
					{
						m_coefficients[path[p]] = m_coefficients[un];
					}
				}

				--j;
			}
		}
	}

	MSE /= m_sampleVariablesValues.size();
	errors.push_back(MSE);
}

void FuzzyMeasure::ComputeImportanceIndex()
{

	std::vector<uint> A;

	auto getA = [this](uint c, std::vector<uint>& A)
	{
		uint nc = ~c;
		
		for (auto set : m_space.powerset())
		{
			uint a = set.first & nc;
			auto it = std::find(A.begin(), A.end(), a);

			if(it == A.end()) { A.push_back(a); }
		}
	};
	
	
	auto factorial = [](unsigned int n)
	{
		unsigned int f = 1;

		for (unsigned int i = 1; i <= n; ++i)
		{
			f *= i;
		}
		return f;
	};


	for (uint i = 0; i < m_space.size(); ++i)
	{
		A.clear();
		uint criterion = pow(2, i);

		float ii = 0;
		getA(criterion, A);

		for (auto set : A)
		{
			float gamma = (float)( factorial(m_space.size() - utilities::numberOfSetBits(set) - 1) * factorial(utilities::numberOfSetBits(set)) ) / (float)factorial(m_space.size());

			ii += gamma * (m_coefficients[set | criterion] - m_coefficients[set]);
		}

		m_importanceIndex[criterion] = ii;
	}


	//utilities::displayImportanceIndex(m_importanceIndex, m_space.powerset());
	//utilities::printCoeff("coeffs4.txt", m_coefficients, m_space.powerset());
}

void FuzzyMeasure::InitMeasureCoefficients(std::vector<uint>& unmodified)
{
	uint nodesNumber = m_space.powerset().size();

	m_coefficients.reserve(nodesNumber);
	unmodified.reserve(nodesNumber);

	for (uint i = 0; i < nodesNumber; ++i)
	{
		m_coefficients[i] = (float)utilities::numberOfSetBits(i) / (float)m_space.size();

		if (i != 0) { unmodified.push_back(i); }
	}
}

void FuzzyMeasure::ComputeErrorMax()
{
	if (m_errorMax != -1.0f) { return; }

	m_errorMax = 0.0f;

	std::vector<uint> path;
	std::vector<float> sortedValues;

	for (int i = 0; i < m_sampleVariablesValues.size(); ++i)
	{
		path.clear();
		sortedValues.clear();

		m_errorMax += pow(ChoquetIntegral(m_coefficients, m_sampleVariablesValues[i], path, sortedValues) - m_sampleChoquetIntegral[i], 2);
	}

	m_errorMax /= m_sampleVariablesValues.size();

	if (m_errorMax == 0.0f) { m_errorMax = 0.000001f; }

	std::cout << "errorMax: " << m_errorMax << std::endl;
}

void FuzzyMeasure::ReadVariablesValuesFromFile(const std::string& filename)
{
	std::vector<std::string> variables;

	m_fileManager.ReadVariablesValues(filename, variables, m_sampleVariablesValues, m_sampleChoquetIntegral);
	m_space.Init(variables);

	//utilities::displayNeighborhood(m_space);
}

void FuzzyMeasure::CheckLatticeMonotocy()
{
	std::vector<std::pair<uint, uint>> involved;

	for (auto& node : m_space.upperNeighbors())
	{
		for (auto set : node.second)
		{
			if (m_coefficients[node.first] > m_coefficients[set])
			{
				involved.emplace_back(node.first, set);
			}
		}
	}

	if (!involved.empty())
	{
		auto powerset = m_space.powerset();

		std::cout << "Monotocy breaked! Node involved: " << std::endl;

		for (auto& node : involved)
		{
			std::cout << "u({" << powerset[node.first] << "}) = " << m_coefficients[node.first]
				<< " and "
				<< "u({" << powerset[node.second] << "}) = " << m_coefficients[node.second]
				<< std::endl;
		}
	}
	else { std::cout << "No error found!" << std::endl; }
}


void FuzzyMeasure::ComputeLattice()
{
	auto diagram = m_space.hasseDiagram();

	for (auto coef : m_coefficients)
	{
		if (diagram[coef.first].empty()) { m_lattice[coef.second] = std::vector<float>(); }

		for (auto node : diagram[coef.first])
		{
			m_lattice[coef.second].push_back(m_coefficients[node]);
		}
	}
}

std::unordered_map<uint, float> FuzzyMeasure::coefficients() { return m_coefficients; }

std::unordered_map<uint, float> FuzzyMeasure::importanceIndex() { return m_importanceIndex; }

FuzzyMeasureSpace FuzzyMeasure::space() { return m_space; }