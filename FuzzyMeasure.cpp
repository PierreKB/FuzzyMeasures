#include "FuzzyMeasure.h"
#include "utilities.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

FuzzyMeasure::FuzzyMeasure()
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

	float choquet = 0;

	for (int i = 1; i <= values.size(); ++i)
	{
		choquet += ((sortedValues[i] - sortedValues[i - 1]) * getCoeffAi(i));
	}

	path.push_back(0);

	return choquet;
}

void FuzzyMeasure::FindFuzzyMeasures(int iterationNumber)
{
	float alpha = 0.15, beta = 0.15, errorMax = 0;
	uint nodesNumber = m_space.powerset().size();

	auto lowerNeighbors = m_space.lowerNeighbors();
	auto upperNeighbors = m_space.upperNeighbors();

	std::vector<uint> path;
	std::vector<float> sortedValues;
	std::vector<float> errors;

	std::vector<uint> unmodified;

	m_coefficients.reserve(nodesNumber);
	unmodified.reserve(nodesNumber);

	for (uint i = 0; i < nodesNumber; ++i)
	{
		m_coefficients[i] = (float)utilities::numberOfSetBits(i) / (float)m_space.size();

		if (i != 0) { unmodified.push_back(i); }
	}

	for (int i = 0; i < m_sampleVariablesValues.size(); ++i)
	{
		errorMax += pow(ChoquetIntegral(m_coefficients, m_sampleVariablesValues[i], path, sortedValues) - m_sampleChoquetIntegral[i], 2);
	}

	errorMax /= m_sampleVariablesValues.size();

	for (int n = 0; n < iterationNumber; ++n)
	{
		for (int i = 0; i < m_sampleVariablesValues.size(); ++i)
		{
			path.clear();
			sortedValues.clear();

			float error = ChoquetIntegral(m_coefficients, m_sampleVariablesValues[i], path, sortedValues) - m_sampleChoquetIntegral[i];
			errors.push_back(error);

			if (error > 0)
			{
				for (int p = path.size() - 2; p > 0; --p)
				{
					int j = 1;
					m_coefficients[path[p]] = m_coefficients[path[p]] - (alpha * (error / errorMax) * (sortedValues[m_space.size() - j] - sortedValues[m_space.size() - j - 1]));

					unmodified.erase(std::remove(unmodified.begin(), unmodified.end(), path[p]), unmodified.end());

					for (auto ln : lowerNeighbors[path[p]])
					{
						if (m_coefficients[path[p]] < m_coefficients[ln])
						{
							m_coefficients[path[p]] = m_coefficients[ln];
						}
					}

					/*if (m_coefficients[path[p]] < m_coefficients[path[p + 1]])
					{
						m_coefficients[path[p]] = m_coefficients[path[p + 1]];
					}*/

					++j;
				}
			}
			else if (error < 0)
			{
				for (int p = 1; p < path.size() - 1; ++p)
				{
					int j = m_space.size() - 1;
					m_coefficients[path[p]] = m_coefficients[path[p]] - (alpha * (error / errorMax) * (sortedValues[m_space.size() - j] - sortedValues[m_space.size() - j - 1]));

					unmodified.erase(std::remove(unmodified.begin(), unmodified.end(), path[p]), unmodified.end());

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
	}

	utilities::printCoeff("coeffs.txt", m_coefficients, m_space.powerset());

	/*if (!unmodified.empty())
	{
		std::sort(unmodified.begin(), unmodified.end(),
			[](uint set1, uint set2) -> bool { return utilities::numberOfSetBits(set1) < utilities::numberOfSetBits(set2); });

		for (auto um : unmodified)
		{

			for (auto ln : lowerNeighbors[um])
			{
				if (m_coefficients[um] < m_coefficients[ln])
				{
					m_coefficients[um] = m_coefficients[ln];
				}
			}

			for (auto un : upperNeighbors[um])
			{
				if (m_coefficients[um] > m_coefficients[un])
				{
					m_coefficients[um] = m_coefficients[un];
				}
			}
		}

		for (auto um : unmodified)
		{
			std::vector<uint> lowerneigh;


			float upperMean = 0, lowerMean = 0, upperMinDist = INFINITY, lowerMinDist = INFINITY, t = 0;

			for (auto un : upperNeighbors[um])
			{
				upperMean += m_coefficients[un];
				upperMinDist = abs(m_coefficients[um] - m_coefficients[un]) < upperMinDist ? abs(m_coefficients[um] - m_coefficients[un]) : upperMinDist;
				++t;
			}

			upperMean /= t;
			t = 0;

			for (auto ln : lowerNeighbors[um])
			{
				lowerMean += m_coefficients[ln];
				lowerMinDist = abs(m_coefficients[um] - m_coefficients[ln]) < lowerMinDist ? abs(m_coefficients[um] - m_coefficients[ln]) : lowerMinDist;
				++t;
			}

			lowerMean /= t;

			float key = upperMean + lowerMean - (2 * m_coefficients[um]);

			if (key > 0)
			{
				m_coefficients[um] = m_coefficients[um] + ( beta * ( (key * upperMinDist) / (2 * (upperMean + lowerMean)) ) );
			}
			else if (key < 0)
			{
				m_coefficients[um] = m_coefficients[um] + (beta * ((key * lowerMinDist) / (2 * (upperMean + lowerMean))));
			}
		}
	}*/
}

void FuzzyMeasure::ReadVariablesValuesFromFile(const std::string& filename)
{
	std::vector<std::string> variables;
	
	m_fileManager.ReadVariablesValues(filename, variables, m_sampleVariablesValues, m_sampleChoquetIntegral);
	m_space.Init(variables);

	//utilities::displayNeighborhood(m_space);
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


void FuzzyMeasure::DisplayLattice()
{
	auto powerset = m_space.powerset();
	auto diagram = m_space.hasseDiagram();

	std::cout << "----Display the lattice with subset and value----" << std::endl;
	std::cout << "" << std::endl;

	for (auto set : powerset)
	{
		std::cout << "Node: u({" << powerset[set.first] << "}) (value = " << m_coefficients[set.first] << ") ----> children: ";

		for (auto child : diagram[set.first])
		{
			std::cout << "u({" << powerset[child] << "}) (value = " << m_coefficients[child] << "), ";
		}

		std::cout << "" << std::endl;
	}

	std::cout << "" << std::endl;
	std::cout << "" << std::endl;


	std::cout << "----Display the lattice with value only----" << std::endl;
	std::cout << "" << std::endl;

	for (auto node : m_lattice)
	{
		std::cout << "Node: " << node.first << " ----> children: ";

		for (auto child : node.second)
		{
			std::cout << child << ", ";
		}

		std::cout << "" << std::endl;
	}
}


std::unordered_map<uint, float> FuzzyMeasure::coefficients() { return m_coefficients; }

FuzzyMeasureSpace FuzzyMeasure::space() { return m_space; }