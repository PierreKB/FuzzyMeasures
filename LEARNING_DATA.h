#ifndef LEARNING_DATA_H
#define LEARNING_DATA_H

#include "BINSETREP.h"
#include <vector>
#include <map>
#include <string>

struct LEARNING_DATA
{
	LEARNING_DATA() {}

	LEARNING_DATA(
		std::vector<std::string> var,
		std::vector<std::map<BINSETREP, float>> val,
		std::vector<float> res)

		: variables(var), values(val), results(res)
	{}

	void aggregate(LEARNING_DATA& data)
	{
		if (values.empty() && results.empty())
		{
			values.reserve(data.values.size());
			results.reserve(data.results.size());
		}

		if (variables.empty()) { variables = data.variables; }

		values.insert(values.end(), data.values.begin(), data.values.end());
		results.insert(results.end(), data.results.begin(), data.results.end());
	}
		

	std::vector<std::string> variables;
	std::vector<std::map<BINSETREP, float>> values;
	std::vector<float> results;
};

#endif