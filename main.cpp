#include "FuzzyMeasure.h"
#include "FileManager.h"
#include "EVALUATION_DATA.h"
#include <iostream>
#include <map>
#include <algorithm>

int main()
{
	std::vector<float> output;
	std::vector<float> errors;
	
	std::vector<LEARNING_DATA> kfoldData;
	LEARNING_DATA data;

	EVALUATION_DATA evalutationData;

	PARAMS params;

	params.alpha = 0.15;
	params.beta = 0.15;

	int k = 3;
	int iterationNumber = 100;
	
	std::cout << "." << std::endl;

	data = FileManager::ReadLearningData("./learning data/LearningBreastCancer.txt");

	//Divide the training data in k set
	kfoldData = FileManager::PartLearningData(k, data);

	for (int i = 0; i < k; ++i)
	{
		//The ith set is the test set the others are used for training
		LEARNING_DATA trainingData;

		for (int j = 0; j < k; ++j)
		{
			if (j == i) { continue; }

			trainingData.aggregate(kfoldData[j]);
		}

		FuzzyMeasure measure(trainingData);

		std::cout << "." << std::endl;

		measure.FindFuzzyMeasuresUntil(iterationNumber, errors, params);

		float MSEmin = *std::min_element(errors.begin(), errors.end());

		evalutationData.errors.push_back(errors);
		errors.clear();

		std::cout << "." << std::endl;

		measure.FindFuzzyMeasuresUntil(MSEmin, iterationNumber * 2, errors, params);

		//measure.CheckLatticeMonotocy();
		FileManager::PrintLattice("./lattice/lat-" + std::to_string(i) + ".txt", measure);

		measure.ComputeImportanceIndex();

		std::cout << "." << std::endl;
		measure.Evaluate(kfoldData[i], output);


		//Compute the recall
		float recall0 = 0.0f, recall1 = 0.0f;
		
		int fn0 = 0, fn1 = 0;

		for (int n = 0; n < kfoldData[i].results.size(); ++n)
		{
			if (kfoldData[i].results[n] == 0)
			{
				if (output[n] < 0.5) { ++recall0; }
				else { ++fn0; }
			}
			if (kfoldData[i].results[n] == 1)
			{
				if (output[n] >= 0.5) { ++recall1; }
				else { ++fn1; }
			}
		}

		evalutationData.recall[i] = { recall0 / (float)(recall0 + fn0), recall1 / (float)(recall1 + fn1) };

		evalutationData.data.push_back(kfoldData[i]);
		evalutationData.output.push_back(output);
		evalutationData.impindex.push_back(measure.importanceIndex());
		evalutationData.alpha = params.alpha;
		evalutationData.beta = params.beta;
		evalutationData.k = k;
		evalutationData.iterationNumber = iterationNumber;


		output.clear();
		errors.clear();
	}

	std::cout << "." << std::endl;

	FileManager::PrintResume("./evaluation/resume.html", evalutationData, [](float o) -> std::string
		{
			if (o < 0.5) { return "benigne"; }

			return "maligne";
		});

	std::cout << "Summary of the operation at ./evaluation/resume.html" << std::endl;
	/*
	Or if the lattice is known
	FileManager::LoadLattice(file, elements, coefficients);
	FuzzyMeasure measure(elements, coefficients);

	measure.Evaluate(data, output);
	FileManager::PrintEvaluationResult(filename, data, output, classSpecifierFunc);

	*/
}

