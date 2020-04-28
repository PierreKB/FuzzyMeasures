#include "FileManager.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <map>
#include <random>
#include <chrono>
#include <unordered_map>


LEARNING_DATA FileManager::ReadLearningData(const std::string& filename)
{
	std::fstream file(filename);
	LEARNING_DATA data;

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

				data.values.reserve(size);

				for (int k = 0; k < size; ++k)
				{
					data.values.push_back(std::map<BINSETREP, float>());
				}
				
				++i;  
				continue; 
			}

			if (i == 1)
			{
				std::string variable;

				while (stream >> variable)
				{
					data.variables.push_back(variable);
				}

				data.variables.pop_back();
				++i;
				continue;
			}

			float value;
			int l = 0;

			while (stream >> value)
			{
				if (l < data.variables.size()) { data.values[j].emplace((BINSETREP)pow(2, l), value); }
				else { data.results.push_back(value); }
				
				++l;
			}

			++j;
		}

		file.close();
	}
	else { std::cout << "Unable to open " << filename << std::endl; file.close(); }

	return data;
}

std::vector<LEARNING_DATA> FileManager::PartLearningData(int k, LEARNING_DATA& data)
{
	std::vector<LEARNING_DATA> kfoldData;

	if (k <= 1) { return kfoldData; }

	if (k > data.values.size())
	{
		std::cout << "k can't be greater than the data size" << std::endl;
		return kfoldData;
	}

	for (int i = 0; i < data.values.size(); ++i)
	{
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

		std::default_random_engine g(seed);

		std::uniform_int_distribution <unsigned int> distribution(0, data.values.size() - 1);

		int rand = distribution(g);

		std::swap(data.values[i], data.values[rand]);
		std::swap(data.results[i], data.results[rand]);
	}


	//Split the training data in k partitions
	int div = (int)data.values.size() / k;
	int mod = data.values.size() % k;

	for (int i = 0; i < k; ++i)
	{
		LEARNING_DATA learningData;

		learningData.results.reserve(div + mod);
		learningData.values.reserve(div + mod);

		if (i < k - 1)
		{
			learningData.values = std::vector<std::map<BINSETREP, float>>(data.values.begin() + (i * div), data.values.begin() + ((i + 1) * div));
			learningData.results = std::vector<float>(data.results.begin() + (i * div), data.results.begin() + ((i + 1) * div));
			learningData.variables = data.variables;

			kfoldData.push_back(learningData);
		}
		else
		{
			learningData.values = std::vector<std::map<BINSETREP, float>>(data.values.begin() + (i * div), data.values.begin() + ((i + 1) * div) + mod);
			learningData.results = std::vector<float>(data.results.begin() + (i * div), data.results.begin() + ((i + 1) * div) + mod);
			learningData.variables = data.variables;

			kfoldData.push_back(learningData);
		}
	}

	return kfoldData;
}

void FileManager::PrintEvaluationResult(const std::string& filename, LEARNING_DATA& evaluationSet, std::vector<float>& output, std::string(*classSpecifier)(float))
{
	std::fstream file(filename, std::fstream::out);
	file.precision(2);

	if (file.is_open())
	{
		for (std::string& variable : evaluationSet.variables)
		{
			file << variable << '\t';
		}

		file << "Classe" << '\t' << "Raw Evaluation" << '\t' << "Error" << '\t' << '\t' << "Evaluation" << std::endl;

		int i = 0;

		for (auto& values : evaluationSet.values)
		{
			for (auto& value : values)
			{
				file << value.second << '\t';
			}

			file
				<< evaluationSet.results[i]
				<< '\t' << output[i]
				<< '\t' << '\t' << output[i] - evaluationSet.results[i]
				<< '\t' << '\t' << classSpecifier(output[i]);
			++i;

			file << std::endl;
		}
	}
	else { std::cout << "Unable to open " << filename << std::endl; }

	file.close();
}

void FileManager::PrintLattice(const std::string& filename, FuzzyMeasure& measure)
{
	std::fstream file(filename, std::fstream::out);

	if (file.is_open())
	{
		file << measure.universe().powersetCardinal() << std::endl;

		auto elements = measure.universe().elements();

		for (auto variable : elements)
		{
			file << variable << " ";
		}

		file << std::endl;
		file << "BINSETREP | COEFFICIENT | VALUE" << std::endl;

		for (auto& coefficient : measure.coefficients())
		{
			std::string textCoeff = "u({" + measure.universe().binaryToString()[coefficient.first] + "})";
			std::replace(textCoeff.begin(), textCoeff.end(), ' ', ',');

			file << coefficient.first << " " << textCoeff<< " " << coefficient.second << std::endl;
		}
	}
	else { std::cout << "Unable to open " << filename << std::endl; }

	file.close();
}

void FileManager::LoadLattice(const std::string& filename, std::vector<std::string>& elements, std::unordered_map<BINSETREP, float>& coefficients)
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

				elements.reserve(size);
				coefficients.reserve(size);

				++i;
				continue;
			}
			if (i == 1)
			{
				std::string element;

				while (stream >> element)
				{
					elements.push_back(element);
				}

				++i;
				continue;
			}
			if (i == 2) { ++i; continue; }
			if (i == 3)
			{
				BINSETREP set;
				std::string coefficient;
				float value;

				stream >> set;
				stream >> coefficient;
				stream >> value;

				coefficients[set] = value;

				continue;
			}			
		}
	}
	else { std::cout << "Unable to open " << filename << std::endl; }

	file.close();
}

void FileManager::PrintResume(const std::string& filename, EVALUATION_DATA& evaluationData, std::string(*classSpecifier)(float))
{
	std::fstream file(filename, std::fstream::out);
	file.precision(2);

	if (file.is_open())
	{
		file << R"(
				<!doctype html>
				<html lang="fr">
					<head>
						<meta charset="utf-8">
						<meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
						<link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.4.1/css/bootstrap.min.css"
						 integrity="sha384-Vkoo8x4CGsO3+Hhxv8T/Q5PaXtkKtu6ug5TOeNV6gBiFeWPGFN9MuhOf23Q9Ifjh" crossorigin="anonymous">
						<link rel="stylesheet" href="https://unpkg.com/bootstrap-table@1.16.0/dist/bootstrap-table.min.css">
						<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.9.3/Chart.min.css">
						<title>R&#233;sume</title>
					</head>
					<body>
					<nav class="navbar fixed-top navbar-light bg-light">
						<a class="navbar-brand" href="#">Resultat de la classification avec validation par )" + std::to_string(evaluationData.k) + R"(-fold</a>
					</nav>
					<br />
					<div class="container-fluid" style="padding-top: 80px;">
						<div class="row">
							<div class="col-sm">
								<h5 class="h5 text-center">Classification</h5>
								<br/>
				)";

		for (int i = 0; i < evaluationData.k; ++i)
		{
			file << R"(
						<table id="table-)" + std::to_string(i + 1) + R"(" data-virtual-scroll="true">
							<thead>
								<tr>
						)";

			std::string classCell;

			for (int j = 0; j < evaluationData.data[0].variables.size(); ++j)
			{
				classCell.append("<th data-field=\"" + evaluationData.data[0].variables[j] + "\">" + evaluationData.data[0].variables[j] + "</th>");
			}

			file << classCell;
			file << R"(
							<th data-field="classe">Classe</th>
							<th data-field="integrale">Int&#233;grale</th>
							<th data-field="evaluation">Evaluation</th>
						</tr>
					</thead>
				</table>
				<br/>
					)";
		}

		file << "</div>";

		file << R"(
				<div class="col-sm">
					<h5 class="h5 text-center">Taux de reconnaissance</h5>
					<br />
					<table id="TR">
						<thead>
							<tr>
				)";

		std::string TRcell;

		for (int i = 0; i < evaluationData.k; ++i)
		{
			TRcell.append("<th data-field=\"k" + std::to_string(i + 1) + "TR\">k = " + std::to_string(i + 1) + "</th>");
		}

		file << TRcell;

		file << R"(
						</tr>
					</thead>
				</table>
				<br />
                <h5 class="h5 text-center">Param&#232;tres</h5>
                <p class="lead">)"
					"&#945; = " + std::to_string(evaluationData.alpha) +
                    "<br />"
                    "&#946; = " + std::to_string(evaluationData.beta) +
                    "<br />"
                    "nombre d'iterations = " + std::to_string(evaluationData.iterationNumber) +
				"</p>" 
				R"(<table id = "errors">
					<thead>
						<tr>
				)";

		std::string errorCell;

		for (int i = 0; i < evaluationData.k; ++i)
		{
			errorCell.append("<th data-field=\"k" + std::to_string(i + 1) + "E\">k = " + std::to_string(i + 1) + "</th>");
		}

		file << errorCell;

		file << R"(
						</tr>
					</thead>
				</table>
            </div>
        </div>
        <br /><br />
        <div class="row">
            <div class="col-sm">
                <h5 class="h5 text-center">Indices d'importances</h5>
                <br />
				)";


		for (int j = 0; j < evaluationData.k; ++j)
		{
			file << R"(
					<table id="importance-index-)" + std::to_string(j + 1) + "\">"
						"<thead>"
							"<tr>";

			std::string iiCell;

			for (int i = 0; i < evaluationData.data[0].variables.size(); ++i)
			{
				iiCell.append("<th data-field=\"ii" + evaluationData.data[0].variables[i] + "\">&#963;(" + evaluationData.data[0].variables[i] + ")</th>");
			}

			file << iiCell;
			file << R"(
						</tr>
                    </thead>
                </table>
                <br />
				)";
		}

		file << R"(
				</div>
				<div class="col-sm">
					<h5 class="h5 text-center">Courbes d'erreurs</h5>
					<br />
					<canvas id="canvas"></canvas>
				</div>
			</div>
		</div>

		<script src="https://code.jquery.com/jquery-3.4.1.slim.min.js"
			integrity="sha384-J6qa4849blE2+poT4WnyKhv5vZF5SrPo0iEjwBvKU7imGFAV0wwj1yYfoRSJoZ+n"
			crossorigin="anonymous"></script>
		<script src="https://cdn.jsdelivr.net/npm/popper.js@1.16.0/dist/umd/popper.min.js"
			integrity="sha384-Q6E9RHvbIyZFJoft+2mJbHaEWldlvI9IOYy5n3zV9zzTtmI3UksdQRVvoxMfooAo"
			crossorigin="anonymous"></script>
		<script src="https://stackpath.bootstrapcdn.com/bootstrap/4.4.1/js/bootstrap.min.js"
			integrity="sha384-wfSDF2E50Y2D1uUdj0O3uMBJnjuUD4Ih7YwaYd1iqfktj0Uod8GCExl3Og8ifwB6"
			crossorigin="anonymous"></script>
		<script src="https://unpkg.com/bootstrap-table@1.16.0/dist/bootstrap-table.min.js"></script>
		<script src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.9.3/Chart.min.js" type="text/javascript"></script>
		)";

		file << "<script type=\"text/javascript\">";
		file << std::endl;

		for (int i = 1; i <= evaluationData.k; ++i)
		{
			file << "var $table" + std::to_string(i) + " = $('#table-" + std::to_string(i) + "')";
			file << std::endl;
			file << "var $ii" + std::to_string(i) + " = $('#importance-index-" + std::to_string(i) + "')";
			file << std::endl;
		}

		file << "var $tr = $('#TR')";
		file << std::endl;
		file << "var $errors = $('#errors')";
		file << std::endl;
		file << "$(function () {";
		file << std::endl;

		std::string d;
		d.reserve(evaluationData.data[0].results.size());

		for (int i = 0; i < evaluationData.k; ++i)
		{
			d.append("var data" + std::to_string(i + 1) + " = [ \n");

			for (int j = 0; j < evaluationData.data[i].results.size(); ++j)
			{
				std::string obj = "{";

				for (int k = 0; k < evaluationData.data[i].variables.size(); ++k)
				{
					obj.append("'" + evaluationData.data[i].variables[k] + "': " + std::to_string(evaluationData.data[i].values[j][(BINSETREP)pow(2, k)]) + ", \n");
				}

				obj.append("'classe': " + std::to_string(evaluationData.data[i].results[j]) + ", \n");
				obj.append("'integrale': " + std::to_string(evaluationData.output[i][j]) + ", \n");
				obj.append("'evaluation': '" + classSpecifier(evaluationData.output[i][j]) + "' }");

				if (j < evaluationData.data[i].results.size() - 1)
				{
					obj.append(",");
				}

				obj.append("\n");
				d.append(obj);

			}

			d.append("]");
			d.append("\n");

			file << d;

			d.clear();
		}

		for (int i = 0; i < evaluationData.k; ++i)
		{
			file << "$table" + std::to_string(i + 1) + ".bootstrapTable({data: data" + std::to_string(i + 1) + "})";
			file << std::endl;
		}

		for (int j = 0; j < evaluationData.k; ++j)
		{
			file << "var iidata" + std::to_string(j + 1) + " = [ \n {";

			for (int i = 0; i < evaluationData.data[0].variables.size(); ++i)
			{
				file << "'ii" + evaluationData.data[0].variables[i] + "': " + std::to_string(evaluationData.impindex[j][(BINSETREP)pow(2, i)] * evaluationData.data[0].variables.size()) + ", \n";
			}

			file << "} \n ] \n";
		}

		for (int i = 0; i < evaluationData.k; ++i)
		{
			file << "$ii" + std::to_string(i + 1) + ".bootstrapTable({data: iidata" + std::to_string(i + 1) + "})";
			file << std::endl;
		}


		file << "var trdata = [ \n {";

		for (int i = 0; i < evaluationData.recall.size(); ++i)
		{
			file << "'k" + std::to_string(i + 1) + "TR': 'Rappel(" + classSpecifier(0.0) + ") = " + std::to_string(evaluationData.recall[i].first);
			file << " <br/> Rappel(" + classSpecifier(1.0) + ") = " + std::to_string(evaluationData.recall[i].second) + "', \n";
		}

		file << "} \n ] \n";

		file << "$tr.bootstrapTable({ data: trdata }) \n";


		file << "var errordata = [ \n {";

		for (int i = 0; i < evaluationData.errors.size(); ++i)
		{
			auto it = std::min_element(evaluationData.errors[i].begin(), evaluationData.errors[i].end());

			file << "'k" + std::to_string(i + 1) + "E': 'Erreur minimale = " + std::to_string(*it);
			file << "<br/> iteration n&#176; " + std::to_string(abs(it - evaluationData.errors[i].end())) + "', \n";
		}

		file << "} \n ] \n";

		file << "$errors.bootstrapTable({ data: errordata })";

		file << "\n })";

		auto rand = [](int min, int max)
		{
			unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
			std::default_random_engine g(seed);
			std::uniform_int_distribution<int> distribution(min, max);

			return distribution(g);
		};

		file << R"(
				var config = {
					type: 'line',
					data: {
					labels: [...Array()"+ std::to_string(evaluationData.iterationNumber) + R"().keys()],
					datasets: [
					)";

		std::string errData;
		errData.reserve(evaluationData.iterationNumber);

		for (int i = 0; i < evaluationData.k; ++i)
		{
			std::string color = "'rgba(" + std::to_string(rand(0, 255)) + ", " + std::to_string(rand(0, 255)) + ", " + std::to_string(rand(0, 255)) + ")'";
			file << "{ \n label: '" + std::to_string(i) + "', \n";
			file << "backgroudColor: " + color + ", \n";
			file << "borderColor: " + color + ", \n";

			for (int j = 0; j < evaluationData.errors[i].size(); ++j)
			{
				errData.append(std::to_string(evaluationData.errors[i][j]));

				if (j < evaluationData.errors[i].size() - 1)
				{
					errData.append(",");
				}
			}

			file << "data: [" << errData << "], \n";
			file << "fill: false \n }, \n";

			errData.clear();
		}

		file << "]}, \n";
		file << R"(
				options: {
					responsive: true,
					title: {
						display: true,
						text: 'Chart.js Line Chart'
					},
					tooltips: {
						mode: 'index',
						intersect: false,
					},
					hover: {
						mode: 'nearest',
						intersect: true
					},
					scales: {
						x: {
							display: true,
							scaleLabel: {
								display: true,
								labelString: 'Month'
							}
						},
						y: {
							display: true,
							scaleLabel: {
								display: true,
								labelString: 'Value'
							}
						}
					}
				}
			};

			window.onload = function () {
				var ctx = document.getElementById('canvas').getContext('2d');
				window.myLine = new Chart(ctx, config);
			};
			)";

		file << "\n </script>";

	}
	else { std::cout << "Unable to open " << filename << std::endl; }

	file.close();
}