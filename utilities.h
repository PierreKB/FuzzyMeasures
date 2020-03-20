#pragma once
#include "FuzzyMeasure.h"
#include <cstdint>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

using uint = uint32_t;

class utilities
{
public:
	static int numberOfSetBits(uint i)
	{
		i = i - ((i >> 1) & 0x55555555);
		i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
		return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
	}


	static void displayHasseDiagram(std::vector<std::vector<uint>>& hasseDiagram, std::unordered_map<uint, std::string>& powerset)
	{
		std::cout << "Display: Nodes ----> Children." << std::endl;

		for (uint node = 0; node < hasseDiagram.size(); ++node)
		{
			std::cout << "Node: { " << powerset[node] << " } child: ";

			for (uint child : hasseDiagram[node])
			{
				std::cout << "{ " << powerset[child] << " }, ";
			}

			std::cout << "" << std::endl;
		}
	}

	static void displayPowerSet(std::unordered_map<uint, std::string>& powerset)
	{
		for (auto& set : powerset)
		{
			std::cout << "Set: {" << set.second << "}" << " ----> Bin: " << set.first << std::endl;
		}
	}

	static void displayNeighborhood(FuzzyMeasureSpace& space)
	{
		auto lowerNeighbors = space.lowerNeighbors();
		auto upperNeighbors = space.upperNeighbors();
		auto powerset = space.powerset();

		std::cout << "Upper neighbords" << std::endl;

		for (auto un : upperNeighbors)
		{
			std::cout << "{ " << powerset[un.first] << " } -----> ";

			for (auto n : un.second)
			{
				std::cout << "{ " << powerset[n] << " }, ";
			}
			std::cout << std::endl;
		}

		std::cout << std::endl;

		std::cout << "Lower neighbords" << std::endl;

		for (auto ln : lowerNeighbors)
		{
			std::cout << "{ " << powerset[ln.first] << " } -----> ";

			for (auto n : ln.second)
			{
				std::cout << "{ " << powerset[n] << " }, ";
			}
			std::cout << std::endl;
		}
	}

	static void printCoeff(const std::string& filename, std::unordered_map<uint, float>& coeffs, std::unordered_map<uint, std::string>& powerset)
	{
		std::fstream file(filename, std::fstream::out);

		if (file.is_open())
		{

			for (auto& coeff : coeffs)
			{
				file << "u({" << powerset[coeff.first] << "}) = " << coeff.second << std::endl;
			}
		}
		else { std::cout << "Unable to open " << filename << std::endl; }

		file.close();
	}

private:
};