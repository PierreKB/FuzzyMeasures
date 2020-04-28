#include "Universe.h"
#include <deque>
#include <iostream>
#include <algorithm>
#include <numeric>



Universe::Universe(std::vector<std::string>& elements)
{
	elements_ = elements;
	powersetCardinal_ = pow(2, elements_.size());

	//Associate each subsets of the universe with its binary representation
	for (BINSETREP set = 0; set < powersetCardinal_; ++set)
	{
		std::string strset;
		strset.reserve(bsr::cardinal(set));

		for (unsigned int i = 0; i < elements_.size(); ++i)
		{
			unsigned int bitValue = (set >> i) & 1;

			if (bitValue == 1) 
			{
				if (!strset.empty()) { strset.append(" "); }

				strset.append(elements_[i]);
			}
		}
		
		binaryToString_.emplace(set, strset);
	}

	ComputeHasseDiagram();
}

Universe::~Universe() 
{}

void Universe::ComputeHasseDiagram()
{
	//Implementation of the Border Algorithm

	std::vector<BINSETREP> powerset(powersetCardinal_);

	std::iota(powerset.begin(), powerset.end(), BINSETREP(0));
	std::sort(powerset.begin(), powerset.end(),
		[](BINSETREP A, BINSETREP B) -> bool { return bsr::cardinal(A) > bsr::cardinal(B); });

	std::vector<BINSETREP> border;
	std::vector<BINSETREP> candidates;
	
	unsigned int candidatesMinSize = bsr::cardinal(powerset[0]);

	for (BINSETREP set : powerset)
	{
		for (auto b : border)
		{
			BINSETREP candidate = bsr::join(set, b);
			
			unsigned int cardinal = bsr::cardinal(candidate);
			candidatesMinSize = cardinal < candidatesMinSize ? cardinal : candidatesMinSize;

			candidates.push_back(candidate);
		}

		//Remove duplicates
		std::sort(candidates.begin(), candidates.end());
		candidates.erase(std::unique(candidates.begin(), candidates.end()), candidates.end());

		for (BINSETREP candidate : candidates)
		{
			if (bsr::cardinal(candidate) > candidatesMinSize) { continue; }

			//m_hasseDiagram[set].push_back(candidate);

			hasseDiagramUpperNeighbors_[set].push_back(candidate);
			hasseDiagramLowerNeighbors_[candidate].push_back(set);

			border.erase(std::remove(border.begin(), border.end(), candidate), border.end());
		}

		border.push_back(set);
		candidates.clear();
	}
}

std::unordered_map<BINSETREP, std::vector<BINSETREP>>& Universe::HasseDiagramLowerNeighbors() { return hasseDiagramLowerNeighbors_; }

std::unordered_map<BINSETREP, std::vector<BINSETREP>>& Universe::HasseDiagramUpperNeighbors() { return hasseDiagramUpperNeighbors_; }

std::vector<std::string>& Universe::elements() { return elements_; }

std::unordered_map<BINSETREP, std::string>& Universe::binaryToString() { return binaryToString_; }

size_t Universe::cardinal() { return elements_.size(); }

size_t Universe::powersetCardinal() { return powersetCardinal_; }