#include "FuzzyMeasure.h"
#include "utilities.h"
#include <iostream>
#include <map>
#include <algorithm>

int main()
{


	FuzzyMeasure measure;

	measure.ReadVariablesValuesFromFile("C:\\Users\\pierr\\Desktop\\M1 VMI\\Projet tuteuré\\Entreprise.txt");
	measure.FindFuzzyMeasures(100);
	auto powerset = measure.space().powerset();



	std::cout << std::endl;
	std::cout << "Final" << std::endl;

	for (auto coef : measure.coefficients())
	{
		std::cout << "U(" << powerset[coef.first] << ")  = " << coef.second << std::endl;
	}



























	/*std::map<uint, float> coeffs
	{
		{1, 0.3}, {2, 0.4}, {4, 0.75}, {3, 0.2}, {5, 0.9}, {6, 0.6}, {7, 1}
	};

	std::map<uint, float> values
	{
		{1, 0.8}, {2, 0.5}, {4, 0.9}
	};

	std::vector<uint> Ai;

	for (uint i = 0; i < values.size(); ++i)
	{
		Ai.push_back(pow(2, i));
	}

	std::vector<uint> path;


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

	std::vector<float> sortedValues = { 0 };

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


	std::cout << "Path: ";

	for (auto p : path)
	{
		std::cout << p << " ";
	}

	std::cout << std::endl;

	std::cout << "Choquet integral value: " << choquet << std::endl;*/
}

