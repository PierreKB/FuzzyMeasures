#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "BINSETREP.h"
#include "LEARNING_DATA.h"
#include "EVALUATION_DATA.h"
#include "FuzzyMeasure.h"
#include <string>
#include <vector>
#include <map>


class FileManager
{
public:

	static LEARNING_DATA ReadLearningData(const std::string& filename);

	static std::vector<LEARNING_DATA> PartLearningData(int k, LEARNING_DATA& data);

	static void PrintEvaluationResult(const std::string& filename, LEARNING_DATA& evaluationSet, std::vector<float>& output, std::string(*classSpecifier)(float));

	static void LoadLattice(const std::string& filename, std::vector<std::string>& elements, std::unordered_map<BINSETREP, float>& coefficients);
	static void PrintLattice(const std::string& filename, FuzzyMeasure& measure);

	static void PrintResume(const std::string& filename, EVALUATION_DATA& evaluationData, std::string(*classSpecifier)(float));

private:
};

#endif