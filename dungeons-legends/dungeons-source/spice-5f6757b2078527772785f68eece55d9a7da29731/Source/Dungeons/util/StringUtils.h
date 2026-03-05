/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

#include "common_header.h"

#include "Base64.h"

#include <type_traits>

#define STRING_COMPRESSION_LEVEL 7

namespace leveldb{
	class Compressor;
}

namespace Util {

	extern const std::string EMPTY_STRING;
	extern const std::string NEW_LINE;
	extern const std::string EMPTY_GUID;

	extern const int REPLACEMENT_CHARACTER_CODE;

	std::string compress(const std::string& input);

	std::string decompress(const std::string& input);
	std::string decompress(const std::string& input, bool& returnValue);

	/// @return true if <s> starts with <start>, false if not
	bool startsWith(const std::string& s, const std::string& start);

	bool endsWith(const std::string& s, const std::string& ends);

	std::string getExtension(const std::string& s);

	std::string getFileName(const std::string& path);

	std::string getFileNameWithExt(const std::string& path);

	std::string getDirectory(const std::string& path);

	bool stringIsInVector(const std::vector<std::string>& stringVector, const std::string& searchString);

	/// @return A reference to s
	std::string& stringReplace(std::string& s, const std::string& src, const std::string& dst, int maxCount = -1);

	/// @return A string trimmed from white space characters on both ends
	std::string stringTrim(const std::string& S);

	/// @return A string trimmed from given characters on any end
	std::string stringTrim(const std::string& S, const std::string& chars, bool left = true, bool right = true);

	std::vector<std::string>& trimVectorStrings(std::vector<std::string>& vectorStrings);

	std::vector<std::string> split(const std::string& str, char delim);

	std::vector<std::string> split(const std::string& str, const std::vector<std::string>& delims, bool includeDelimCharsInResult = false);

	std::vector<std::string> splitLines(const std::string& content);

	void removeAll(std::string& s, const char** rep, int repCount);

	std::string removeAllColorCodes(const std::string& s);

	/// @return The "Java" implementation for string hash codes
	unsigned int hashCode(const std::string& s);

	int utf8len(const std::string& str, bool includeColor = false);
	std::string utf8substring(const std::string& str, int startIndex, int endIndex = -1);
	void appendUtf8Character(std::string& str, int utf8Char);

	// Format close to c-style using c++ (you don't specify type of data, just '%')
	// /server/%/join -> /server/1/join
	std::string simpleFormat(const std::string& format, const std::vector<std::string>& parameters);

	// sprintf into std::string
	std::string format(const char* format, ...);

	/// @return A string containing the formatted integer
	template <typename IntType = int, typename std::enable_if<std::is_integral<IntType>::value>::type* = nullptr, typename std::enable_if<std::is_unsigned<IntType>::value>::type* = nullptr>
	std::string toString(IntType i) {
		if (i == 0) {
			return "0";
		}

		std::string returnValue;

		while (i != 0) {
			char digit = '0' + (i % 10);
			returnValue.insert(0, 1, digit);

			i /= 10;
		}

		return returnValue;
	}

	/// @return A string containing the formatted integer
	template <typename IntType = int, typename std::enable_if<std::is_integral<IntType>::value>::type* = nullptr, typename std::enable_if<std::is_signed<IntType>::value>::type* = nullptr>
	std::string toString(IntType i) {
		if (i == 0) {
			return "0";
		}

		const bool isNegative = i < 0;
		if (isNegative) {
			i = -i;
		}

		std::string returnValue = toString(static_cast<typename std::make_unsigned<IntType>::type>(i));

		if (isNegative) {
			returnValue.insert(0, 1, '-');
		}

		return returnValue;
	}

	///returns true if the string formatted as a valid path
	bool isValidPath(const std::string& str);

/// @return A string containing the formatted float
	std::string toString(float f);

	std::string toNiceString(int i);

	std::string toLower(const std::string& str);

	std::string toUpper(const std::string& str);

	bool compareNoCase(const std::string& first, const std::string& second);

	std::string removeIllegalChars(const std::string& str);

	char toHex(char code);

	void freeStringMemory(std::string& toFree);

	//reliably frees the underlying buffer of the string
	template<typename T>
	void freeVectorMemory(std::vector<T>& toFree) {
		std::vector<T>().swap(toFree);
	}

	int countWordOccurrences(const std::string& str, const std::string& word);

	std::string formatTickDuration(int ticks);
	std::string getFilesizeString(uint64_t filesize);

	template<typename T>
	std::string toBase64(const T& value) {
		return base64_encode((const unsigned char*)&value, sizeof(value));
	}

	std::vector<std::string>& splitString(const std::string& s, char delim, std::vector<std::string>& elems);

	bool stringContains(const std::string& s, char character);

	enum class NumberConversionResult {
		Succeed,

		Invalid,
		TooSmall,
		TooLarge,

		Count
	};

	template
	<typename IntType = int, typename std::enable_if<std::is_integral<IntType>::value>::type* = nullptr>
	NumberConversionResult toInt(const std::string& inputStr, IntType& destination) {
		// check for invalid starting character
		if (inputStr.empty() || !('-' == inputStr[0] || std::isdigit(inputStr[0]))) {
			return NumberConversionResult::Invalid;
		}

		// try to convert
		IntType returnValue = 0;
		std::istringstream input(inputStr);
		if ((input >> returnValue).fail()) {
			return NumberConversionResult::Invalid;
		}

		// check for remaining characters
		char c;
		if (!(input >> c).eof()) {
			return NumberConversionResult::Invalid;
		}

		// success!
		destination = returnValue;
		return NumberConversionResult::Succeed;
	}

	template
	<typename FloatType = float, typename std::enable_if<std::is_floating_point<FloatType>::value>::type* = nullptr>
	NumberConversionResult toFloat(const std::string& inputStr, FloatType& destination) {
		// check for invalid starting character
		if (inputStr.empty() || !('-' == inputStr[0] || '.' == inputStr[0] || std::isdigit(inputStr[0]))) {
			return NumberConversionResult::Invalid;
		}

		// try to convert
		FloatType returnValue = 0;
		std::istringstream input(inputStr);
		if ((input >> returnValue).fail()) {
			return NumberConversionResult::Invalid;
		}

		// check for remaining characters
		char c;
		if (!(input >> c).eof()) {
			return NumberConversionResult::Invalid;
		}

		// success!
		destination = returnValue;
		return NumberConversionResult::Succeed;
	}


	NumberConversionResult toInt(const std::string& inputStr, int& destination, int min, int max);

	std::string boolToString(bool value);
	bool toBool(const std::string& input, bool& destination);

	std::wstring toWideString(const std::string& inputStr);
	std::string toString(const std::wstring& inputStr);
	std::string toString(const std::string& inputStr);

	std::string urlEncode(const std::string &value);

	bool isValidIPv4(const std::string& ipAddrStr);
	bool isValidIPv6(const std::string& ipAddrStr);
	bool isValidIP(const std::string& ipStr, bool considerIPv4Valid = true, bool considerIPv6Valid = true);
	/*
	This method can parse a value or 2 values as a "range" 
	templatized for use with other data types.
	Supported inputs:
	0
	[1, 10]
	[5]

	*/
	template<typename T>
	bool parseRange(const std::string& input, T& min, T& max) {
		//Originally from EntityActionHandler.cpp setVariant
		std::stringstream stream;
		size_t index = input.find('[');
		if (index != std::string::npos) {
			size_t tindex = input.find(']', index);
			if (tindex != std::string::npos) {
				std::vector<std::string> split = Util::split(input.substr(index + 1, tindex - 1), ',');
				stream << split[0];

				if (split.size() > 1) {
					stream << " " << split[1];
				}
				stream >> min;
				max = min;
				stream >> max;
				if (min > max) {
					std::swap(min, max);
				}
				return true;
			}
			//else
			//invalid, leave at 0
			return false;
		}
		stream << input;
		stream >> min;
		max = min;
		return true;
	}

	void normalizeLineEndings(std::string& str);

	std::string cp437ToUTF8(const std::string& path);

}	// end namespace Util
