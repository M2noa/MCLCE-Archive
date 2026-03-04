#pragma once

#include "stdafx.h"
#include <glm/glm.hpp>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

namespace Esteem
{
	/// \brief Extra string parsing functions
	/// \author Y. Schaeffer
	/// \date June 2015
	class StringParser
	{
	public:
		static float ParseStringToFloat(std::string value);
		static glm::vec3* ParseStringToVec3(std::string value);
		static glm::vec2* ParseStringToVec2(std::string value);
		static std::vector<float> ParseStringToVector(std::string value, uint maxNumbers = 0);

		static inline void ReplaceAll(std::string& string, char find, char replace)
		{
			std::replace(string.begin(), string.end(), find, replace);
		}

		static inline void ReplaceAll(std::string& string, std::string_view find, std::string_view replace)
		{
			size_t startPos = 0;
			while ((startPos = string.find(find, startPos)) != std::string::npos)
			{
				string.replace(startPos, find.size(), replace);
				startPos += replace.size(); // don't find pattern in replaced area
			}
		}

		static inline void StripAll(std::string& string, std::string_view find)
		{
			const size_t n = find.size();
			for (size_t i = string.find(find); i != std::string::npos; i = string.find(find))
				string.erase(i, n);
		}

		static inline void ReplaceAll(std::string& string, const std::map<std::string, std::string>& replaceMap)
		{
			for (auto& [find, replace] : replaceMap)
				ReplaceAll(string, find, replace);
		}

		static inline void ReplaceAll(std::string& string, const std::unordered_map<std::string, std::string>& replaceMap)
		{
			for (auto& [find, replace] : replaceMap)
				ReplaceAll(string, find, replace);
		}
		
		static inline void ReplaceAll(std::string& string, const std::vector<std::pair<std::string, std::string>>& replaceMap)
		{
			for (auto& [find, replace] : replaceMap)
				ReplaceAll(string, find, replace);
		}

		template<size_t N>
		static inline void ReplaceAll(std::string& string, const std::pair<std::string, std::string>(&replaceMap)[N])
		{
			for (auto& [find, replace] : replaceMap)
				ReplaceAll(string, find, replace);

			/*for (size_t i = 0; i < N; ++i)
			{
				const auto& replacePair = replaceMap[i];
				ReplaceAll(string, replacePair.first, replacePair.second);
			}*/
		}

		// trim from start
		static inline void TrimLeft(std::string& string)
		{
			string.erase(string.begin(), std::find_if(string.begin(), string.end(), [](int c) {return !std::isspace(c); }));
		}

		// trim from end
		static inline void TrimRight(std::string& string)
		{
			string.erase(std::find_if(string.rbegin(), string.rend(), [](int c) {return !std::isspace(c); }).base(), string.end());
		}

		// trim from both ends
		static inline void Trim(std::string& string)
		{
			TrimLeft(string);
			TrimRight(string);
		}

		//static inline void InsertAfterLinesContaining(std::string& string, const std::vector<std::string>& contains, const std::string& insert);
	};
}