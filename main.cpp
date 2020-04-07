#include "FuzzyMeasure.h"
#include "utilities.h"
#include <iostream>
#include <map>
#include <algorithm>

int main()
{


	FuzzyMeasure measure;
	std::vector<float> errors;

	measure.ReadVariablesValuesFromFile("./learning data/Fromage.txt");
	measure.FindFuzzyMeasuresUntil((int)100, errors, 0.15);

	float MSEmin = *std::min_element(errors.begin(), errors.end());
	float MSEmax = *std::max_element(errors.begin(), errors.end());

	std::cout << "MSEmin: " << MSEmin << std::endl;
	std::cout << "MSEmax: " << MSEmax << std::endl;

	//utilities::printErrorInCSV("./output/fromage.csv", errors, measure.space().powerset());

	errors.clear();

	measure.FindFuzzyMeasuresUntil(MSEmin, errors, 0.15);

	measure.ComputeImportanceIndex();
	measure.CheckLatticeMonotocy();

	//utilities::printImportanceIndexInCSV("./output/fromage.csv", measure.importanceIndex(), measure.space().powerset());
}

