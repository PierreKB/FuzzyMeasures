#ifndef UNIVERSE_H
#define UNIVERSE_H

#include "BINSETREP.h"
#include <vector>
#include <unordered_map>


class Universe
{
public:
	Universe(std::vector<std::string>& elements);
	~Universe();

	std::unordered_map<BINSETREP, std::vector<BINSETREP>>& HasseDiagramLowerNeighbors();
	std::unordered_map<BINSETREP, std::vector<BINSETREP>>& HasseDiagramUpperNeighbors();

	std::vector<std::string>& elements();
	std::unordered_map<BINSETREP, std::string>& binaryToString();

	size_t cardinal();
	size_t powersetCardinal();

private:
	void ComputeHasseDiagram();


	std::vector<std::string> elements_;
	
	std::unordered_map<BINSETREP, std::vector<BINSETREP>> hasseDiagramLowerNeighbors_;
	std::unordered_map<BINSETREP, std::vector<BINSETREP>> hasseDiagramUpperNeighbors_;

	std::unordered_map<BINSETREP, std::string> binaryToString_;
	
	unsigned int powersetCardinal_;
};

#endif