#include "FileManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>


FileManager::FileManager()
{}

FileManager::~FileManager()
{}

void FileManager::ReadVariablesValues(const std::string& filename, 
	std::vector<std::string>& variables, 
	std::vector<std::map<uint, float>>& values,
	std::vector<float>& choquetIntegral)
{
	std::fstream file(filename);

	if (file.is_open())
	{
		std::string line;

		int i = 0, j = 0, size;

		while (getline(file, line))
		{
			if (line.empty()) { continue; }

			std::stringstream stream(line);

			if (i == 0) 
			{ 
				stream >> size;

				values.reserve(size);

				for (int k = 0; k < size; ++k)
				{
					values.push_back(std::map<uint, float>());
				}
				
				++i;  
				continue; 
			}

			if (i == 1)
			{
				std::string varaible;

				while (stream >> varaible)
				{
					variables.push_back(varaible);
				}

				variables.pop_back();
				++i;
				continue;
			}

			float value;
			int l = 0;

			while (stream >> value)
			{
				if (l < variables.size()) { values[j].emplace((uint)pow(2, l), value); }
				else { choquetIntegral.push_back(value); }
				
				++l;
			}

			++j;

		}
	}
	else { std::cout << "Unable to open " << filename << std::endl; }

	file.close();
}