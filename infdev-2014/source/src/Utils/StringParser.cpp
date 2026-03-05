#include "StringParser.h"
#include "Utils/Debug.h"

namespace Esteem
{
	float StringParser::ParseStringToFloat(std::string value)
	{
		std::vector<float> parsedValues = ParseStringToVector(value, 1);
		float f = parsedValues[0];
		return f;
	}

	glm::vec2* StringParser::ParseStringToVec2(std::string value)
	{
		std::vector<float> parsedValues = ParseStringToVector(value, 2);
		glm::vec2* vector2 = new glm::vec2(parsedValues[0], parsedValues[1]);
		return vector2;
	}

	glm::vec3* StringParser::ParseStringToVec3(std::string value)
	{
		std::vector<float> parsedValues = ParseStringToVector(value, 3);
		glm::vec3* vector3 = new glm::vec3(parsedValues[0], parsedValues[1], parsedValues[2]);
		return vector3;
	}

	std::vector<float> StringParser::ParseStringToVector(std::string value, uint maxNumbers)
	{
		std::string tempValue = "";
		std::vector<float> parsedValues;

		for(unsigned int i = 0; i < value.length(); i++)
		{
			if(maxNumbers != 0 && parsedValues.size() >= maxNumbers)
				break;

			if(isdigit(value[i]) || value[i] == '.')
				tempValue += value[i];
			else
			{
				parsedValues.push_back(std::stof(tempValue));
				tempValue = "";
			}
		}

		return parsedValues;
	}

	/*void StringParser::InsertAfterLinesContaining(std::string& string, const std::vector<std::string>& contains, const std::string& insert)
	{
		uint startPos = 0;
		uint containsIndex = 0;
		bool doneWithIndex = false;

		while(containsIndex <= contains.size() && (doneWithIndex = ((startPos = string.find(contains[containsIndex], startPos)) != std::string::npos)))
		{
			if(doneWithIndex)
				++containsIndex;
			else
			{
				ignore(std::numeric_limits<std::streamsize>::max(), '\n')
			}
		}

		string.insert(startPos, insert);
		startPos += insert.length(); // Handles case where 'to' is a substring of 'from'
	}*/
}