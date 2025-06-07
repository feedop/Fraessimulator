
export module parser;

import std;

import <glm/vec3.hpp>;
import <glm/vec4.hpp>;
import <glm/geometric.hpp>;
import <glm/ext/matrix_transform.hpp>;
import math;

void validateFileName(const std::string& filePath)
{
	auto fileName = std::filesystem::path(filePath).filename().string();
	try
	{
		static const std::regex regex(".*\.[fk]\d{2}");
		std::regex_match(fileName, regex);
	}
	catch(...)
	{
		throw std::exception("Invalid file name");
	}
}

void parseGCodeLine(const std::string& line, std::vector<glm::vec3>& path)
{
	std::istringstream stream(line);
	char N, G, X, Y, Z;
	int lineNumber, type;
	float xValue, yValue, zValue;

	// Parse the line following the format N<number> G<number> X<float> Y<float> Z<float>
	if (stream >> N >> lineNumber >> G >> type >> X >> xValue >> Y >> yValue >> Z >> zValue)
	{
		// Check if the TYPE is 01
		if (type == 1)
		{
			glm::vec3 parsedPosition = { xValue / 10, yValue / 10, zValue / 10 };
			glm::vec3 axis(-1.0f, 0.0f, 0.0f);

			// Create a 90-degree rotation matrix around the negative X-axis
			glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));

			// Apply the rotation matrix to the vector (note the multiplication by vec4)
			glm::vec3 newPosition = rotationMatrix * glm::vec4(parsedPosition, 1.0f);

			if (path.size() != 0) [[likely]]
			{
				glm::vec3 oldPosition = path.back();
				auto diff = newPosition - oldPosition;

				if (glm::length(diff) <= math::eps) // Points too close => skip
					return;

				diff = glm::normalize(diff);
				if (diff == glm::vec3{ 0.0f, -1.0f, 0.0f })
					throw std::exception(std::format("An illegal downward move, line: {}", lineNumber).c_str());
			}
			
			path.push_back(newPosition);
		}
	}
	else
	{
		throw std::exception(std::format("Error parsing line: {}", lineNumber).c_str());
	}
}

std::vector<glm::vec3> extractPath(const std::string& filePath)
{
	std::vector<glm::vec3> path;
	std::ifstream file(filePath);

	if (!file.is_open())
		throw std::exception("Could not open the file");

	std::string line;

	// Read each line from the file
	while (std::getline(file, line))
	{
		parseGCodeLine(line, path);  // Parse each line individually
	}

	return path;
}

export namespace Parser
{
	std::vector<glm::vec3> parse(const std::string& filePath, bool& flat, int& size)
	{
		validateFileName(filePath);

		auto dotIndex = filePath.find('.');
		auto type = filePath[dotIndex + 1];
		flat = type == 'f';
		std::string substr = filePath.substr(dotIndex + 2);
		size = std::stoi(substr);

		return extractPath(filePath);
	}
}