#include "FuzzyMeasure.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <cmath>
#include <chrono>

FuzzyMeasure::FuzzyMeasure(LEARNING_DATA& data)
	:universe_(data.variables), data_(data), errorMax_(-1.0f)
{}

FuzzyMeasure::FuzzyMeasure(std::vector<std::string>& elements, std::unordered_map<BINSETREP, float>& coefficients)
	: universe_(elements), data_(LEARNING_DATA()), errorMax_(-1.0f)
{
	coefficients_ = coefficients;
}


FuzzyMeasure::~FuzzyMeasure()
{}

float FuzzyMeasure::ChoquetIntegral(
	std::unordered_map<BINSETREP, float>& coeffs,
	std::map<BINSETREP, float>& values,
	std::vector<BINSETREP>& path,
	std::vector<float>& sortedValues)
{
	float choquet = 0.0f;
	std::vector<BINSETREP> Ai;

	//Get the measure coefficient of the subset represented by A(i) when the integral in calculated
	auto getCoeffAi = [&coeffs, &Ai, &path](unsigned int i) -> float
	{
		BINSETREP set = 0;

		for (unsigned int j = i - 1; j < Ai.size(); ++j)
		{
			set = bsr::join(set, Ai[j]);
		}

		path.push_back(set);

		return coeffs[set];
	};


	//Sort the universe's elements by their associated values
	for (unsigned int i = 0; i < values.size(); ++i)
	{
		Ai.push_back((BINSETREP)pow(2, i));
	}

	std::sort(Ai.begin(), Ai.end(),
		[&values](BINSETREP A, BINSETREP B)
		{
			return values[A] < values[B];
		});

	//Randomly swap elements with the same associated values
	for (int j = 0; j < 9; ++j)
	{
		for (int i = 0; i < Ai.size() - 1; ++i)
		{
			if (values[Ai[i]] == values[Ai[i + 1]])
			{
				unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
				std::default_random_engine g(seed);
				std::uniform_int_distribution<int> distribution(0, 1);

				if (distribution(g) == 1) { std::swap(Ai[i], Ai[i + 1]); }
			}
		}
	}


	sortedValues.push_back(0);

	for (auto v : values)
	{
		sortedValues.push_back(v.second);
	}

	std::sort(sortedValues.begin(), sortedValues.end());

	//Compute the integral
	for (int i = 1; i <= values.size(); ++i)
	{
		choquet += ((sortedValues[i] - sortedValues[i - 1]) * getCoeffAi(i));
	}

	path.push_back(0);

	return choquet;
}

void FuzzyMeasure::InitMeasure()
{
	//Initialize the measure at the equilibrium state

	unsigned int nodesNumber = universe_.powersetCardinal();

	coefficients_.reserve(nodesNumber);
	unmodified_.reserve(nodesNumber);

	for (BINSETREP set = 0; set < nodesNumber; ++set)
	{
		coefficients_[set] = (float)bsr::cardinal(set) / (float)universe_.cardinal();

		if (set != 0 && set != nodesNumber - 1) { unmodified_.push_back(set); }
	}
}

void FuzzyMeasure::Reset()
{
	//Reset variable that need to be reseted before each new iteration
	unmodified_.clear();
	unsigned int nodesNumber = universe_.powersetCardinal();

	for (BINSETREP set = 0; set < nodesNumber; ++set)
	{
		if (set != 0 && set != nodesNumber - 1) { unmodified_.push_back(set); }
	}
}

void FuzzyMeasure::FindFuzzyMeasuresUntil(int iterationNumber, std::vector<float>& errors, PARAMS params)
{
	InitMeasure();
	ComputeErrorMax();

	float alpha = params.alpha;
	float beta = params.beta;

	for (int n = 0; n < iterationNumber; ++n)
	{
		FindFuzzyMeasures(errors, alpha, beta);

		if (params.decreaseAlpha && alpha > params.alphaLimit) { alpha -= params.deltaAlpha; }
		if (params.decreaseBeta && beta > params.betaLimit) { beta -= params.deltaBeta; }

		Reset();
	}
}

void FuzzyMeasure::FindFuzzyMeasuresUntil(float errorLimit, int iterationLimit, std::vector<float>& errors, PARAMS params)
{
	float limit = INFINITY;
	int iteration = 0;

	InitMeasure();
	ComputeErrorMax();

	float alpha = params.alpha;
	float beta = params.beta;

	while (limit > errorLimit)
	{
		//May loop forever because the path on the choquet integral is not always the same because equal value are choose randomly.
		//So the expected minError may not be reached so we set a limit on the number of iterations
		//May also loop forerver because of floating point precision
		if (iteration > iterationLimit) { break; }

		FindFuzzyMeasures(errors, alpha, beta);

		if (params.decreaseAlpha && alpha > params.alphaLimit) { alpha -= params.deltaAlpha; }
		if (params.decreaseBeta && beta > params.betaLimit) { beta -= params.deltaBeta; }

		limit = errors.back();
		++iteration;

		Reset();
	}
}

void FuzzyMeasure::FindFuzzyMeasures(std::vector<float>& errors, float alpha, float beta)
{
	auto lowerNeighbors = universe_.HasseDiagramLowerNeighbors();
	auto upperNeighbors = universe_.HasseDiagramUpperNeighbors();

	std::vector<BINSETREP> path;
	std::vector<float> sortedValues;

	float MSE = 0.0f;

	for (int i = 0; i < data_.values.size(); ++i)
	{
		path.clear();
		sortedValues.clear();

		//Compute the mean squared error
		float error = ChoquetIntegral(coefficients_, data_.values[i], path, sortedValues) - data_.results[i];
		MSE += pow(error, 2);

		//Ajust the measure's coefficients
		if (error > 0)
		{
			int j = 1;

			for (BINSETREP p = path.size() - 2; p > 0; --p)
			{
				coefficients_[path[p]] = coefficients_[path[p]] - (alpha * (error / errorMax_) * (sortedValues[(sortedValues.size() - 1) - j] - sortedValues[(sortedValues.size() - 1) - j - 1]));

				unmodified_.erase(std::remove(unmodified_.begin(), unmodified_.end(), path[p]), unmodified_.end());

				for (BINSETREP lowerNeighbor : lowerNeighbors[path[p]])
				{
					if (coefficients_[path[p]] < coefficients_[lowerNeighbor])
					{
						coefficients_[path[p]] = coefficients_[lowerNeighbor];
					}
				}

				++j;
			}
		}
		else if (error < 0)
		{
			int j = path.size() - 2;

			for (BINSETREP p = 1; p < path.size() - 1; ++p)
			{
				coefficients_[path[p]] = coefficients_[path[p]] - (alpha * (error / errorMax_) * (sortedValues[(sortedValues.size() - 1) - j] - sortedValues[(sortedValues.size() - 1) - j - 1]));

				unmodified_.erase(std::remove(unmodified_.begin(), unmodified_.end(), path[p]), unmodified_.end());

				for (BINSETREP upperNeighbor : upperNeighbors[path[p]])
				{
					if (coefficients_[path[p]] > coefficients_[upperNeighbor])
					{
						coefficients_[path[p]] = coefficients_[upperNeighbor];
					}
				}

				--j;
			}
		}
	}

	MSE /= data_.values.size();
	errors.push_back(MSE);

	AjustUnmodifiedNodes(beta);
}

void FuzzyMeasure::AjustUnmodifiedNodes(float beta)
{
	auto lowerNeighbors = universe_.HasseDiagramLowerNeighbors();
	auto upperNeighbors = universe_.HasseDiagramUpperNeighbors();


	auto getMeanValue = [this](BINSETREP A, std::unordered_map<BINSETREP, std::vector<BINSETREP>>& neighbors) -> float
	{
		float mean = 0.0f;

		for (BINSETREP neighbor : neighbors[A])
		{
			mean += coefficients_[neighbor];
		}

		return (mean / (float)neighbors[A].size());
	};

	auto getMinDistance = [this](BINSETREP A, std::unordered_map<BINSETREP, std::vector<BINSETREP>>& neighbors) -> float
	{
		float minDistance = INFINITY;

		for (BINSETREP neighbor : neighbors[A])
		{
			float distance = abs(coefficients_[A] - coefficients_[neighbor]);
			minDistance = distance < minDistance ? distance : minDistance;
		}

		return minDistance;
	};

	unmodified_.erase(std::remove(unmodified_.begin(), unmodified_.end(), BINSETREP(0)), unmodified_.end());
	unmodified_.erase(std::remove(unmodified_.begin(), unmodified_.end(), BINSETREP(pow(2, universe_.cardinal()) - 1)), unmodified_.end());

	//Sort unmodified nodes by their levels in the lattices
	std::sort(unmodified_.begin(), unmodified_.end(), [](BINSETREP A, BINSETREP B)
		{
			return bsr::cardinal(A) < bsr::cardinal(B);
		});

	//Check monocity
	for (BINSETREP set : unmodified_)
	{
		for (BINSETREP upperNeighbor : upperNeighbors[set])
		{
			if (coefficients_[set] > coefficients_[upperNeighbor]) { coefficients_[set] = coefficients_[upperNeighbor]; }
		}

		for (BINSETREP lowerNeighbor : lowerNeighbors[set])
		{
			if (coefficients_[set] < coefficients_[lowerNeighbor]) { coefficients_[set] = coefficients_[lowerNeighbor]; }
		}
	}


	//Ajust nodes values
	for (BINSETREP node : unmodified_)
	{
		float meanValueOfUN = getMeanValue(node, upperNeighbors);
		float meanValueOfLN = getMeanValue(node, lowerNeighbors);

		float minDistanceBetweenUN = getMinDistance(node, upperNeighbors);
		float minDistanceBetweenLN = getMinDistance(node, lowerNeighbors);

		float numerator = meanValueOfUN + meanValueOfLN - (2 * coefficients_[node]);
		float denominator = 2 * (meanValueOfUN + meanValueOfLN);

		if (numerator > 0)
		{
			coefficients_[node] = coefficients_[node] + (beta * (numerator / denominator) * minDistanceBetweenUN);
		}
		else
		{
			coefficients_[node] = coefficients_[node] + (beta * (numerator / denominator) * minDistanceBetweenLN);
		}
	}
}

void FuzzyMeasure::ComputeImportanceIndex()
{
	//A is the vector of all subset of the universe where the given riterion is not included
	std::vector<BINSETREP> A;

	auto getA = [this](BINSETREP criterion, std::vector<BINSETREP>& A)
	{
		BINSETREP complement = bsr::complement(criterion);

		for (BINSETREP set = 0; set < universe_.powersetCardinal(); ++set)
		{
			BINSETREP a = bsr::meet(set, complement);
			auto it = std::find(A.begin(), A.end(), a);

			if (it == A.end()) { A.push_back(a); }
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


	for (int i = 0; i < universe_.cardinal(); ++i)
	{
		A.clear();
		BINSETREP criterion = pow(2, i);

		float ii = 0;
		getA(criterion, A);

		for (BINSETREP set : A)
		{
			float gamma = (float)(factorial(universe_.cardinal() - bsr::cardinal(set) - 1) * factorial(bsr::cardinal(set))) / (float)factorial(universe_.cardinal());

			ii += gamma * (coefficients_[bsr::join(set, criterion)] - coefficients_[set]);
		}

		importanceIndex_[criterion] = ii;
	}
}


void FuzzyMeasure::ComputeErrorMax()
{
	if (errorMax_ != -1.0f) { return; }

	errorMax_ = 0.0f;

	std::vector<BINSETREP> path;
	std::vector<float> sortedValues;

	for (int i = 0; i < data_.values.size(); ++i)
	{
		path.clear();
		sortedValues.clear();

		errorMax_ += pow(ChoquetIntegral(coefficients_, data_.values[i], path, sortedValues) - data_.results[i], 2);
	}

	errorMax_ /= (float)data_.values.size();

	if (errorMax_ == 0.0f) { errorMax_ = 0.000001f; }
}

void FuzzyMeasure::Evaluate(LEARNING_DATA& data, std::vector<float>& output)
{
	std::vector<BINSETREP> path;
	std::vector<float> sortedValues;

	for (int i = 0; i < data.values.size(); ++i)
	{
		path.clear();
		sortedValues.clear();
		
		float integral = ChoquetIntegral(coefficients_, data.values[i], path, sortedValues);

		output.push_back(integral);
	}
}


void FuzzyMeasure::CheckLatticeMonotocy()
{
	std::vector<std::pair<BINSETREP, BINSETREP>> involved;

	for (auto& node : universe_.HasseDiagramUpperNeighbors())
	{
		for (auto set : node.second)
		{
			if (coefficients_[node.first] > coefficients_[set])
			{
				involved.emplace_back(node.first, set);
			}
		}
	}

	if (!involved.empty())
	{
		auto powerset = universe_.binaryToString();

		std::cout << "Monotocy breaked! Node involved: " << std::endl;

		for (auto& node : involved)
		{
			std::cout << "u({" << powerset[node.first] << "}) = " << coefficients_[node.first]
				<< " and "
				<< "u({" << powerset[node.second] << "}) = " << coefficients_[node.second]
				<< std::endl;
		}
	}
	else { std::cout << "No error found!" << std::endl; }
}

std::unordered_map<BINSETREP, float> FuzzyMeasure::coefficients() { return coefficients_; }

std::unordered_map<BINSETREP, float> FuzzyMeasure::importanceIndex() { return importanceIndex_; }

Universe FuzzyMeasure::universe() { return universe_; }