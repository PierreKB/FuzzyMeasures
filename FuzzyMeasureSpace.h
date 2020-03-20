#pragma once
#include <vector>
#include <unordered_map>


using uint = uint32_t;
#define MAX_COEFF 32

class FuzzyMeasureSpace
{
public:
	FuzzyMeasureSpace();
	FuzzyMeasureSpace(std::vector<std::string>& space);

	~FuzzyMeasureSpace();

	void Init(std::vector<std::string>& space);

	std::vector<std::string>& space();
	std::unordered_map<uint, std::vector<uint>>& lowerNeighbors();
	std::unordered_map<uint, std::vector<uint>>& upperNeighbors();
	std::unordered_map<uint, std::string>& powerset();
	std::vector<std::vector<uint>>& hasseDiagram();
	size_t size();

private:
	uint Meet(uint set1, uint set2);
	uint Join(uint set1, uint set2);

	void ComputeHasseDiagram();


	std::vector<std::string> m_space;
	std::unordered_map<uint, std::string> m_powerset;
	std::vector<std::vector<uint>> m_hasseDiagram;

	std::unordered_map<uint, std::vector<uint>> m_lowerNeighbors;
	std::unordered_map<uint, std::vector<uint>> m_upperNeighbors;

	int m_powersetCardinal;
};