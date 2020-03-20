#pragma once
#include <string>
#include <vector>
#include <map>


using uint = uint32_t;

class FileManager
{
public:
	FileManager();
	~FileManager();

	void ReadVariablesValues(const std::string& filename, 
		std::vector<std::string>& variables, 
		std::vector<std::map<uint, float>>& values, 
		std::vector<float>& choquetIntegral);

private:
};