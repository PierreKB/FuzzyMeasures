#include "FuzzyMeasureSpace.h"
#include "utilities.h"
#include <deque>
#include <iostream>
#include <algorithm>


FuzzyMeasureSpace::FuzzyMeasureSpace()
	: m_powersetCardinal(0)
{}

FuzzyMeasureSpace::FuzzyMeasureSpace(std::vector<std::string>& space)
	: m_hasseDiagram(pow(2, space.size()))
{
	m_space = std::move(space);
	m_powersetCardinal = pow(2, m_space.size());

	for (uint binset = 0; binset < m_powersetCardinal; ++binset)
	{
		std::string strset;
		strset.reserve(utilities::numberOfSetBits(binset));

		for (int i = 0; i < m_space.size(); ++i)
		{
			auto bitValue = (binset >> i) & 1;

			if (bitValue == 1) 
			{
				if (!strset.empty()) { strset.append(" "); }

				strset.append(m_space[i]); 
			}
		}

		m_powerset.emplace(binset, strset);
	}

	ComputeHasseDiagram();
}

void FuzzyMeasureSpace::Init(std::vector<std::string>& space)
{
	(*this) = FuzzyMeasureSpace(space);
}



FuzzyMeasureSpace::~FuzzyMeasureSpace()
{}

uint FuzzyMeasureSpace::Meet(uint set1, uint set2)
{
	if (set1 < 0 || set1 >= m_powersetCardinal || set2 < 0 || set2 >= m_powersetCardinal)
	{
		return (uint)0;
	}

	return set1 & set2;
}

uint FuzzyMeasureSpace::Join(uint set1, uint set2)
{
	if (set1 < 0 || set1 >= m_powersetCardinal || set2 < 0 || set2 >= m_powersetCardinal)
	{
		return (uint)0;
	}

	return set1 | set2;
}

void FuzzyMeasureSpace::ComputeHasseDiagram()
{
	std::vector<uint> powersetReverseSorted;

	for (uint set = 0; set < m_powersetCardinal; ++set)
	{
		powersetReverseSorted.push_back(set);
	}

	std::sort(powersetReverseSorted.begin(), powersetReverseSorted.end(),
		[](uint set1, uint set2) -> bool { return utilities::numberOfSetBits(set1) > utilities::numberOfSetBits(set2); });

	std::vector<uint> border;
	std::vector<uint> candidates;

	int candidatesMinSize = utilities::numberOfSetBits(powersetReverseSorted[0]);

	for (auto set : powersetReverseSorted)
	{
		for (auto b : border)
		{
			auto candidate = Join(set, b);

			auto cardinal = utilities::numberOfSetBits(candidate);
			candidatesMinSize = cardinal < candidatesMinSize ? cardinal : candidatesMinSize;

			candidates.push_back(candidate);
		}

		//Remove duplicates
		std::sort(candidates.begin(), candidates.end());
		candidates.erase(std::unique(candidates.begin(), candidates.end()), candidates.end());

		for (auto candidate : candidates)
		{
			if (utilities::numberOfSetBits(candidate) > candidatesMinSize) { continue; }

			m_hasseDiagram[set].push_back(candidate);

			m_upperNeighbors[set].push_back(candidate);
			m_lowerNeighbors[candidate].push_back(set);

			border.erase(std::remove(border.begin(), border.end(), candidate), border.end());
		}

		border.push_back(set);
		candidates.clear();
	}
}

std::vector<std::string>& FuzzyMeasureSpace::space() { return m_space; }

std::unordered_map<uint, std::vector<uint>>& FuzzyMeasureSpace::lowerNeighbors() { return m_lowerNeighbors; }

std::unordered_map<uint, std::vector<uint>>& FuzzyMeasureSpace::upperNeighbors() { return m_upperNeighbors; }

std::unordered_map<uint, std::string>& FuzzyMeasureSpace::powerset() { return m_powerset; }

std::vector<std::vector<uint>>& FuzzyMeasureSpace::hasseDiagram() { return m_hasseDiagram; }

size_t FuzzyMeasureSpace::size() { return m_space.size(); }