#include "Dungeons.h"

#include "CommonTypes.h"
#include "util/StringUtils.h"
#include "Core/Debug/Log.h"
#include "Core/Utility/utf8proc.h"
#include "SharedConstants.h"
// #include "leveldb/zlib_compressor.h"
// #include "leveldb/compressor.h"
// #include "locale/Localization.h"
#include "util/ZLibUtil.h"
#include "locale/I18n.h"

namespace Util {

	const std::string ILLEGAL_FILE_CHARACTERS = "/\n\r\t\f`?*\\<>|\":";
	const std::string EMPTY_STRING = "";
	const std::string NEW_LINE = "\n";
	const std::string EMPTY_GUID = "00000000-0000-0000-0000-000000000000";

	// The UTF codepoint for a replaced character (U+FFFD), to be used when we don't support a given codepoint (e.g. emoji)
	const int REPLACEMENT_CHARACTER_CODE = 0xFFFD;

	static std::unordered_map<unsigned char, std::string> CP437_TO_UTF8 = {
		{ 128, "\xc3\x87" },
		{ 129, "\xc3\xbc" },
		{ 130, "\xc3\xa9" },
		{ 131, "\xc3\xa2" },
		{ 132, "\xc3\xa4" },
		{ 133, "\xc3\xa0" },
		{ 134, "\xc3\xa5" },
		{ 135, "\xc3\xa7" },
		{ 136, "\xc3\xaa" },
		{ 137, "\xc3\xab" },
		{ 138, "\xc3\xa8" },
		{ 139, "\xc3\xaf" },
		{ 140, "\xc3\xae" },
		{ 141, "\xc3\xac" },
		{ 142, "\xc3\x84" },
		{ 143, "\xc3\x85" },
		{ 144, "\xc3\x89" },
		{ 145, "\xc3\xa6" },
		{ 146, "\xc3\x86" },
		{ 147, "\xc3\xb4" },
		{ 148, "\xc3\xb6" },
		{ 149, "\xc3\xb2" },
		{ 150, "\xc3\xbb" },
		{ 151, "\xc3\xb9" },
		{ 152, "\xc3\xbf" },
		{ 153, "\xc3\x96" },
		{ 154, "\xc3\x9c" },
		{ 155, "\xc2\xa2" },
		{ 156, "\xc2\xa3" },
		{ 157, "\xc2\xa5" },
		{ 158, "\xe2\x82\xa7" },
		{ 159, "\xc6\x92" },
		{ 160, "\xc3\xa1" },
		{ 161, "\xc3\xad" },
		{ 162, "\xc3\xb3" },
		{ 163, "\xc3\xba" },
		{ 164, "\xc3\xb1" },
		{ 165, "\xc3\x91" },
		{ 166, "\xc2\xaa" },
		{ 167, "\xc2\xba" },
		{ 168, "\xc2\xbf" },
		{ 169, "\xe2\x8c\x90" },
		{ 170, "\xc2\xac" },
		{ 171, "\xc2\xbd" },
		{ 172, "\xc2\xbc" },
		{ 173, "\xc2\xa1" },
		{ 174, "\xc2\xab" },
		{ 175, "\xc2\xbb" },
		{ 176, "\xe2\x96\x91" },
		{ 177, "\xe2\x96\x92" },
		{ 178, "\xe2\x96\x93" },
		{ 179, "\xe2\x94\x82" },
		{ 180, "\xe2\x94\xa4" },
		{ 181, "\xe2\x95\xa1" },
		{ 182, "\xe2\x95\xa2" },
		{ 183, "\xe2\x95\x96" },
		{ 184, "\xe2\x95\x95" },
		{ 185, "\xe2\x95\xa2" },
		{ 186, "\xe2\x95\x91" },
		{ 187, "\xe2\x95\x97" },
		{ 188, "\xe2\x95\x9d" },
		{ 189, "\xe2\x95\x9c" },
		{ 190, "\xe2\x95\x9b" },
		{ 191, "\xe2\x94\x90" },
		{ 192, "\xe2\x94\x94" },
		{ 193, "\xe2\x94\xb4" },
		{ 194, "\xe2\x94\xac" },
		{ 195, "\xe2\x94\x9c" },
		{ 196, "\xe2\x94\x80" },
		{ 197, "\xe2\x94\xbc" },
		{ 198, "\xe2\x95\x9e" },
		{ 199, "\xe2\x95\x9f" },
		{ 200, "\xe2\x95\x9a" },
		{ 201, "\xe2\x95\x94" },
		{ 202, "\xe2\x95\xa9" },
		{ 203, "\xe2\x95\xa6" },
		{ 204, "\xe2\x95\xa0" },
		{ 205, "\xe2\x95\x90" },
		{ 206, "\xe2\x95\xac" },
		{ 207, "\xe2\x95\xa7" },
		{ 208, "\xe2\x95\xa8" },
		{ 209, "\xe2\x95\xa4" },
		{ 210, "\xe2\x95\xa5" },
		{ 211, "\xe2\x95\x99" },
		{ 212, "\xe2\x95\x98" },
		{ 213, "\xe2\x95\x92" },
		{ 214, "\xe2\x95\x93" },
		{ 215, "\xe2\x95\xab" },
		{ 216, "\xe2\x95\xaa" },
		{ 217, "\xe2\x94\x98" },
		{ 218, "\xe2\x94\x8c" },
		{ 219, "\xe2\x96\x88" },
		{ 220, "\xe2\x96\x84" },
		{ 221, "\xe2\x96\x8c" },
		{ 222, "\xe2\x96\x90" },
		{ 223, "\xe2\x96\x80" },
		{ 224, "\xce\xb1" },
		{ 225, "\xc2\xb2" },
		{ 226, "\xce\x93" },
		{ 227, "\xcf\x80" },
		{ 228, "\xce\xa3" },
		{ 229, "\xcf\x83" },
		{ 230, "\xc2\xb5" },
		{ 231, "\xcf\x84" },
		{ 232, "\xce\xa6" },
		{ 233, "\xcf\xb4" },
		{ 234, "\xce\xa9" },
		{ 235, "\xce\xb4" },
		{ 236, "\xe2\x88\x9e" },
		{ 237, "\xcf\x86" },
		{ 238, "\xce\xb5" },
		{ 239, "\xe2\x88\xa9" },
		{ 240, "\xe2\x89\xa1" },
		{ 241, "\xc2\xb1" },
		{ 242, "\xe2\x89\xa5" },
		{ 243, "\xe2\x89\xa4" },
		{ 244, "\xe2\x8c\xa0" },
		{ 245, "\xe2\x8c\xa1" },
		{ 246, "\xc3\xb7" },
		{ 247, "\xe2\x89\x88" },
		{ 248, "\xc2\xb0" },
		{ 249, "\xe2\x88\x99" },
		{ 250, "\xc2\xb7" },
		{ 251, "\xe2\x88\x9a" },
		{ 252, "\xe2\x81\xbf" },
		{ 253, "\xc2\xb2" },
		{ 254, "\xe2\x96\xa0" },
		{ 255, "\xc2\xa0" },
	};

	std::string compress(const std::string& input) {
		std::string compressed;
		ZLibUtil(STRING_COMPRESSION_LEVEL).compress(input, compressed);
		return compressed;
	}

	std::string decompress(const std::string& input) {
		bool dummy = false;
		return decompress(input, dummy);
	}

	std::string decompress(const std::string& input, bool& returnValue) {
		std::string output;
		returnValue = ZLibUtil(STRING_COMPRESSION_LEVEL).decompress(input.c_str(), input.size(), output);
		return output;
	}

	char toHex(char code) {
		static const char hex[] = "0123456789abcdef";
		return hex[code & 15];
	}

	std::string removeIllegalChars(const std::string& str) {
		std::string res = str;

		for (auto&& c : ILLEGAL_FILE_CHARACTERS) {
			res = Util::stringReplace(res, std::string(1, c), "");
		}
		return res;
	}

	/// @return true if <s> starts with <start>, false if not
	bool startsWith(const std::string& s, const std::string& start) {

		if (s.length() < start.length()) {
			return false;
		}

		for (int i = 0; i < (int)start.length(); ++i) {
			if (start[i] != s[i]) {
				return false;
			}
		}
		return true;
	}

	std::string getExtension(const std::string& s) {
		auto idx = s.rfind('.');
		return (idx == std::string::npos) ? "" : s.substr(idx + 1, s.size() - idx);
	}

	std::string getFileName(const std::string& path) {

		auto start = path.find_last_of("/\\");
		if (start == std::string::npos) {
			start = 0;
		}
		else {
			++start;
		}

		auto end = path.rfind('.');
		auto len = (end != std::string::npos && end > start) ? (end - start) : std::string::npos;

		return path.substr(start, len);
	}

	std::string getFileNameWithExt(const std::string& path) {

		auto start = path.find_last_of("/\\");
		if (start == std::string::npos) {
			start = 0;
		}
		else {
			++start;
		}

		return path.substr(start);
	}

	std::string getDirectory(const std::string& path) {
		return path.substr(0, path.find_last_of("/\\"));
	}

	bool stringIsInVector(const std::vector<std::string>& stringVector, const std::string& searchString) {
		return std::any_of(std::begin(stringVector), std::end(stringVector), [&searchString](const std::string& directon) {return compareNoCase(directon, searchString); });
	}

	bool endsWith(const std::string& s, const std::string& ends) {
		return s.rfind(ends, s.size() - ends.size()) != std::string::npos;
	}

	// Naive (e.g. slow and stupid) implementation. @todo: fix
	/// @return A reference to s
	std::string& stringReplace(std::string& s, const std::string& src, const std::string& dst, int maxCount) {
		const int srcLength = src.length();
		std::string::size_type pos = 0;
		while (maxCount--) {
			pos = s.find(src, pos);
			if (pos == std::string::npos) {
				break;
			}
			s.replace(pos, srcLength, dst);
			pos += dst.length();
		}
		return s;
	}

	/// @return A string trimmed from white space characters on both ends
	std::string stringTrim(const std::string& s) {
		return stringTrim(s, " \t\n\r", true, true);
	}

	/// @return A string trimmed from given characters on any end
	std::string stringTrim(const std::string& s, const std::string& chars, bool left, bool right) {
		const int len = s.length();
		const int lenChars = chars.length();

		if (len == 0 || lenChars == 0 || ((left || right) == false)) {
			return "";
		}

		int i = 0, j = len - 1;

		if (left) {
			for (; i < len; ++i) {
				if (std::find(chars.begin(), chars.end(), s[i]) == chars.end()) {
					break;
				}
			}
		}
		if (right) {
			for (; j >= i; --j) {
				if (std::find(chars.begin(), chars.end(), s[j]) == chars.end()) {
					break;
				}
			}
		}
		return s.substr(i, j - i + 1);
	}

	std::vector<std::string>& trimVectorStrings(std::vector<std::string>& vectorStrings) {
		for (std::vector<std::string>::size_type i = 0; i != vectorStrings.size(); i++) {
			vectorStrings[i] = stringTrim(vectorStrings[i]);
		}
		return vectorStrings;
	}

	std::vector<std::string> split(const std::string& str, char delim) {
		std::vector<std::string> res;
		size_t start = 0;

		while (start < str.size()) {
			auto idx = str.find_first_of(delim, start);

			auto len = (idx != std::string::npos) ? (idx - start) : (str.length() - start);

			res.emplace_back(str.substr(start, len));

			start += len + 1;
		}

		return res;
	}

	std::vector<std::string> split(const std::string& str, const std::vector<std::string>& delims, bool includeDelimCharsInResult) {
		std::vector<std::string> returnValue;
		std::stringstream stringStream(str);
		std::string currentLine;

		while (std::getline(stringStream, currentLine)) {
			std::string currentSubStr;

			for (size_t i = 0; i < currentLine.length(); ++i) {
				currentSubStr += currentLine[i];

				std::string longestDelimMatch;

				for (auto& delim : delims) {
					if (currentSubStr.length() >= delim.length() && delim.length() > longestDelimMatch.length()) { // Take the longest delim str
						auto offset = currentSubStr.length() - delim.length();
						if (currentSubStr.compare(offset, delim.length(), delim) == 0) { // Token match
							longestDelimMatch = currentSubStr.substr(currentSubStr.length() - delim.length());
						}
					}
				}

				if (!longestDelimMatch.empty()) {
					if (currentSubStr.length() > longestDelimMatch.length()) {
						returnValue.push_back(currentSubStr.substr(0, currentSubStr.length() - longestDelimMatch.length()));
					}

					if (includeDelimCharsInResult) {
						returnValue.push_back(longestDelimMatch);
					}

					currentSubStr.clear();
				}
			}

			if (!currentSubStr.empty()) {
				returnValue.push_back(currentSubStr);
			}
		}

		return returnValue;
	}

	std::vector<std::string> splitLines(const std::string& content) {
		std::stringstream stream;
		stream << content;

		std::vector<std::string> allLines;
		std::string temp;
		if (stream.good()) {
			std::getline(stream, temp);

			while (stream.good()) {
				allLines.emplace_back(temp);
				std::getline(stream, temp);
			}
		}

		return allLines;
	}

	/// @return The "Java" implementation for string hash codes
	unsigned int hashCode(const std::string& s) {
		const int len = s.length();

		unsigned int hash = 0;

		for (int i = 0; i < len; i++) {
			hash = ((hash << 5) - hash) + s[i];
		}
		return hash;
	}

	void removeAll(std::string& s, const char** rep, int repCount) {
		for (int i = 0; i < repCount; ++i) {
			stringReplace(s, rep[i], "");
		}
	}

	std::string removeAllColorCodes(const std::string& s) {
		std::string retVal = "";
		auto strings = split(s, { "\xc2\xa7" }, false);
		if (strings.size() >= 1) {
			retVal = strings[0];
		}
		if (strings.size() >= 2) {
			for (int i = 1; i < (int)strings.size(); ++i) {
				if (strings[i].size() >= 1) {
					retVal += strings[i].substr(1, strings[i].size() - 1);
				}
			}
		}
		return retVal;
	}

	int utf8len(const std::string& str, bool includeColor) {
		const unsigned char* strPtr = (const unsigned char*)str.c_str();
		int c;
		ssize_t remaining = str.length();
		ssize_t skip;
		int length = 0;

		while ((skip = utf8proc_iterate(strPtr, remaining, &c)) > 0) {
			strPtr += skip;
			remaining -= skip;

			length++;

			if (!includeColor && c == 167 && remaining > 0) {
				strPtr++;
				remaining--;
				--length;
			}
		}
		return length;
	}

	std::string utf8substring(const std::string& str, int startIndex, int endIndex) {
		const unsigned char* strPtr = (const unsigned char*)str.c_str();
		int c;
		ssize_t remaining = str.length();
		ssize_t skip;
		std::string results;
		int index = 0;

		while ((skip = utf8proc_iterate(strPtr, remaining, &c)) > 0) {
			if (index >= startIndex && (index < endIndex || endIndex < 0)) {
				for (int i = 0; i < skip; i++) {
					results += strPtr[i];
				}
			}

			strPtr += skip;
			remaining -= skip;

			index++;
			if (endIndex >= 0 && index >= endIndex) {
				break;
			}
		}

		return results;
	}

	void appendUtf8Character(std::string& str, int utf8Char) {
		uint8_t utf8Buf[4] = { 0 };
		ssize_t charCount = utf8proc_encode_char(utf8Char, utf8Buf);
		for (ssize_t i = 0; i < charCount; ++i) {
			str.push_back(utf8Buf[i]);
		}
	}

	std::string simpleFormat(const std::string& format, const std::vector<std::string>& parameters) {
		std::string returnString;
		bool foundUniChar = false;
		bool foundPercent = false;
		size_t currentParam = 0;

		for (auto&& c : format) {
			if (foundPercent) {
				foundPercent = false;
				// Found two % in a row, want to write it literally
				if (c == '%') {
					returnString += '%';
				}
				else {
					if (parameters.size() > currentParam) {
						returnString += parameters[currentParam];
						currentParam++;
					}
					returnString += c;
				}
			}
			else if (foundUniChar) {
				foundUniChar = false;
				returnString += c;
			}
			else if (c == 127) {
				foundUniChar = true;
				returnString += c;
			}
			else if (c == '%') {
				foundPercent = true;
			}
			else {
				returnString += c;
			}
		}
		if (foundPercent) {
			if (parameters.size() > currentParam) {
				returnString += parameters[currentParam];
			}
		}
		return returnString;
	}

	std::string format(const char* format, ...) {
		DEBUG_ASSERT(format, "Nullptr passed to Utils::format");

		va_list args;
		va_list copy;

		// Initialize variable arguments
		va_start(args, format);
		// Create a copy for the second pass
		va_copy(copy, args);

		// first pass: measure the required buffer size passing nullptr
		int bufferSize = vsnprintf(nullptr, 0, format, args);
		// reset variable argument list
		va_end(args);
		if (bufferSize < 0) {
			DEBUG_FAIL("Encoding error has occured");
			// reset copy of argument list
			va_end(copy);
			return Util::EMPTY_STRING;
		}
		else if (bufferSize == 0) {
			// looks like we expect an empty string, no need to measure again
			// reset copy of argument list
			va_end(copy);
			return Util::EMPTY_STRING;
		}

		// add 1 for the terminating character
		std::vector<char> buffer(bufferSize + 1);

		// second pass: move format to buffer
		int finalSize = vsnprintf(buffer.data(), buffer.size(), format, copy);
		int finalBufferSize = static_cast<int>(buffer.size());
		// reset copy of argument list
		va_end(copy);

		DEBUG_ASSERT(bufferSize == finalSize, "Second iteration didn't give same output");
		DEBUG_ASSERT(finalSize < finalBufferSize, "Buffer is to small to contain data");

		if (bufferSize != finalSize || finalSize >= finalBufferSize) {
			return Util::EMPTY_STRING;
		}

		return std::string(buffer.data());
	}

	std::string urlEncode(const std::string& value) {
		std::ostringstream escaped;
		escaped.fill('0');
		escaped << std::hex;

		for (auto& c : value) {
			// Keep alphanumeric and other accepted characters intact
			if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
				escaped << c;
				continue;
			}

			// Any other characters are percent-encoded
			escaped << std::uppercase;
			escaped << '%' << std::setw(2) << int((unsigned char)c);
			escaped << std::nouppercase;
		}

		return escaped.str();
	}

	std::string toNiceString(int i) {
		if (i < 10000) {
			return Util::toString(i);
		}

		else if (i < 10000000) {
			return Util::toString(i / 1000) + "k";
		}
		else {
			return Util::toString(i / 1000000) + "M";
		}
	}

	/// @return A string containing the formatted float
	std::string toString(float f) {
		//TODO do that for real
		return toString((int)f);
	}

	std::string toLower(const std::string& str) {
		std::string l = str;
		std::transform(l.begin(), l.end(), l.begin(), ::tolower);
		return l;
	}

	std::string toUpper(const std::string& str) {
		std::string l = str;
		std::transform(l.begin(), l.end(), l.begin(), ::toupper);
		return l;
	}

	bool compareNoCase(const std::string& lhs, const std::string& rhs) {
		if (lhs.length() == rhs.length()) {

			return std::equal(rhs.begin(), rhs.end(), lhs.begin(),
				[](const std::string::value_type lhschar, const std::string::value_type rhschar) {

				return std::toupper(lhschar) == std::toupper(rhschar);
			} );
		}
		else {
			return false;
		}
	}

	//actually frees the underlying buffer of the string reliably
	void freeStringMemory(std::string& toFree) {
		std::string dummy;
		toFree.swap(dummy);
	}

	int countWordOccurrences(const std::string& str, const std::string& word) {
		int count = 0;
		size_t start = 0;

		while (true) {
			start = str.find(word, start);
			if (start == std::string::npos) {
				return count;
			}
			else {
				start += word.length();
				++count;
			}
		}
	}

	bool isValidPath(const std::string& str) {
		for (auto& c : str) {
			if (c == '\n' || c == '\r') {
				return false;
			}

			//TODO more rules? there are other invalid characters
		}

		return true;
	}

	std::string formatTickDuration(int ticks) {
		int seconds = ticks / SharedConstants::TicksPerSecond;
		const int minutes = seconds / 60;
		seconds %= 60;

		if (seconds < 10) {
			return toString(minutes) + ":0" + toString(seconds);
		}
		return toString(minutes) + ":" + toString(seconds);
	}

	std::string getFilesizeString(uint64_t sizeOnDisk) {
		const float MBUnit = 1024 * 1024;
		const float GBUnit = 1024 * 1024 * 1024;

		float fileSize = static_cast<float>(sizeOnDisk);

		std::stringstream formator;
		formator << std::fixed << std::showpoint;

		std::string fileSizeUnits;
		int precision;
		float displaySize;
		if (fileSize < MBUnit) {
			// size smaller than 1MB, increase decimal precision
			fileSizeUnits = I18n::get("playscreen.fileSize.MB");
			displaySize = fileSize / MBUnit;
			precision = 2;
		}
		else if (fileSize < GBUnit) {
			// size between 1MB and 1GB
			fileSizeUnits = I18n::get("playscreen.fileSize.MB");
			displaySize = fileSize / MBUnit;
			precision = 1;
		}
		else {
			// size bigger than 1GB
			fileSizeUnits = I18n::get("playscreen.fileSize.GB");
			displaySize = fileSize / GBUnit;
			precision = 1;
		}

		formator << std::setprecision(precision) << displaySize << fileSizeUnits;

		std::string sizeString = formator.str();

// 		if (I18n::getCurrentLanguage().unwrap().getCommaSeperator()) {
// 			std::replace(sizeString.begin(), sizeString.end(), '.', ',');
// 		}

		return sizeString;
	}

	std::vector<std::string>& splitString(const std::string& s, char delim, std::vector<std::string>& elems) {
		std::stringstream ss(s);
		std::string item;

		while (std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}

	bool stringContains(const std::string& s, char character) {
		std::size_t found = s.find(character);
		return found != std::string::npos;
	}

	NumberConversionResult toInt(const std::string& inputStr, int& destination, int min, int max) {
		int returnValue = destination;

		NumberConversionResult code = toInt(inputStr, returnValue);
		if (code == NumberConversionResult::Succeed) {
			if (returnValue < min) {
				return NumberConversionResult::TooSmall;
			}
			else if (returnValue > max) {
				return NumberConversionResult::TooLarge;
			}
			else {
				destination = returnValue;
				return NumberConversionResult::Succeed;
			}
		}

		return NumberConversionResult::Invalid;
	}

	std::string boolToString(bool value) {
		return value ? "true" : "false";
	}

	bool toBool(const std::string& input, bool& destination) {
		std::string s = Util::toLower(input);
		if (s == "true" || s == "1" || s == "yes") {
			destination = true;
			return true;
		}
		if (s == "false" || s == "0" || s == "no") {
			destination = false;
			return true;
		}
		return false;
	}

	std::string toString(const std::wstring& inputStr) {
#if defined(MCPE_ALLOW_CODE_CVT)
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
		return converter.to_bytes(inputStr);
#else
		size_t len = wcstombs(nullptr, inputStr.data(), 0);
		std::string outStr(len, 0);
		char* charPtr = const_cast<char*>(outStr.data());
		wcstombs(charPtr, inputStr.data(), len);
		return outStr;
#endif
	}

	std::string toString(const std::string& inputStr) {
		return inputStr;
	}

	std::wstring toWideString(const std::string& inputStr) {
#if defined(MCPE_ALLOW_CODE_CVT)
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
		return converter.from_bytes(inputStr);
#else
		size_t len = mbstowcs(nullptr, inputStr.data(), 0);
		std::wstring outStr(len, 0);
		wchar_t* ptr = const_cast<wchar_t*>(outStr.data());
		mbstowcs(ptr, inputStr.data(), len);
		return outStr;
#endif
	}

	bool isValidIPv4(const std::string& ipAddrStr) {
		return isValidIP(ipAddrStr, true, false);
	}

	bool isValidIPv6(const std::string& ipAddrStr) {
		return isValidIP(ipAddrStr, false, true);
	}

	bool isValidIP(const std::string& ipAddrStr, bool considerIPv4Valid, bool considerIPv6Valid) {
		// Number of parts in the IP address (ipv4 has 4, ipv6 has 8)
		unsigned int parts = 0;		
		// Max characters in an ip part + null terminated character
		unsigned int bufSize = 0;
		// pointer to the converted IP address parts
		int* ip = nullptr;
		// pointer to an individual part c-str
		char* partBuf = nullptr;
		// The character that delimits the ip address parts
		char partDelimiter = ' ';
		// The maximum value a part can be assigned
		int maxPartValue = 0;
		// Conversion function for a part to an integer
		int(*convertIpPart)(char* part) = nullptr;
		// Determines whether shorthand notation is allowed (e.g. "::" or "..")
		bool shortHandAllowed = false;

		const char ipv6PartDelimiter = ':';
		const unsigned int ipv6PartsSize = 8;
		const unsigned int ipv6BufSize = 5;
		const unsigned int ipv4PartsSize = 4;
		const unsigned int ipv4BufSize = 4;
		
		union {
			int ipv6[ipv6PartsSize];
			int ipv4[ipv4PartsSize];
		} ipBuffer;
		
		union {
			char ipv6[ipv6BufSize];
			char ipv4[ipv4BufSize];
		} ipPartBuffer;
		
		// Check if the ip address string format assumes IPv6
		if (std::find(ipAddrStr.begin(), ipAddrStr.end(), ipv6PartDelimiter) != ipAddrStr.end()) {
			// IPv6 not supported, so the IP is not valid
			if (!considerIPv6Valid) {
				return false;
			}

			// Assign the variables used by the validate IP lambda
			parts = ipv6PartsSize;
			bufSize = ipv6BufSize;
			ip = &ipBuffer.ipv6[0];
			partBuf = &ipPartBuffer.ipv6[0];
			partDelimiter = ipv6PartDelimiter;
			maxPartValue = 65535; // 0xffff
			shortHandAllowed = true;
			convertIpPart = [](char* part) { 
				// no data since the last delimiter, then asssume 0.
				// IPv6 addresses will be displayed in this way to save space. A "::" is assumed to fill in remaining WORDs until 8 is matched.
				// i.e. ff02::1 == ff02:0:0:0:0:0:0:1
				if (part[0] == 0) {
					return 0;
				}
				return (int)strtol(part, NULL, 16); 
			};
		}
		else {
			// IPv4 not supported, so the IP is not valid
			if (!considerIPv4Valid) {
				return false;
			}

			// Otherwise, default to IPv4
			// Assign the variables used by the validate IP lambda
			parts = ipv4PartsSize;
			bufSize = ipv4BufSize;
			ip = &ipBuffer.ipv4[0];
			partBuf = &ipPartBuffer.ipv4[0];
			partDelimiter = '.';
			maxPartValue = 255;
			shortHandAllowed = false;
			convertIpPart = [](char* part) -> int {
				// no data since the last delimiter, then fail.
				if (part[0] == 0) {
					return -1;
				}
				return atoi(part);
			};
		}		

		// Generic Algorithm for validating IP addresses (v4 & v6)
		for (unsigned int i = 0; i < parts; ++i)
		{
			ip[i] = -1;

			// null-out the ip part buffer
			if (i < bufSize)
			{
				partBuf[i] = 0;
			}
		}

		bool shortHandUsed = false;
		unsigned int bufIdx = 0;
		unsigned int ipIdx = 0;
		auto itr = ipAddrStr.begin();

		while (ipIdx < parts) {
			if ((itr == ipAddrStr.end()) || (*itr == partDelimiter)) {
				if (itr != ipAddrStr.begin() && *(itr - 1) == partDelimiter) {
					// Can only use shorthand once and not at the end of address
					if (shortHandUsed || itr == ipAddrStr.end()) {
						return false;
					}
					shortHandUsed = shortHandAllowed;
				}

				ip[ipIdx] = convertIpPart(partBuf);

				// Either the converted value is actually -1 or the consecutive delimiter '::' or '..' isn't supported.
				// Both are failures either way.
				if (ip[ipIdx] == -1) {
					return false;
				}

				ipIdx++;
				bufIdx = 0;

				// null-out the ip part buffer
				for (unsigned int i = 0; i < bufSize; ++i)
				{
					partBuf[i] = 0;
				}

				if (itr != ipAddrStr.end()) {
					if (ipIdx == parts) {
						//more in this string than is allowed in an IP address
						return false;
					}
					itr++;
					continue;
				}
				break;
			}

			//can't have more than the max allowed characters in a single IP part (minus 1 to not count null terminator).
			if (bufIdx == (bufSize - 1)) {
				return false;
			}

			//validate that this character is within the '0' to '9' range.
			char tmp[2];
			tmp[0] = *itr;
			tmp[1] = 0;

			//use atoi, since the characters are not necessarily in the ASCII space where a range check on '0' to '9' is
			// valid
			if ((*itr != '0') && (0 == convertIpPart(tmp))) {
				return false;
			}

			partBuf[bufIdx] = *itr;
			bufIdx++;
			itr++;
		}

		// If short hand was used then the remaining IP parts will be -1. Set them to 0.
		if (shortHandUsed) {
			for (unsigned int i = ipIdx; i < parts; ++i) {
				ip[i] = 0;
			}
		}

		//check that the ip parts are present and valid
		for (unsigned int i = 0; i < parts; ++i) {
			if ((ip[i] < 0) || (ip[i] > maxPartValue)) {
				return false;
			}
		}

		return true;
	}

	void normalizeLineEndings(std::string& str) {
		Util::stringReplace(str, "\r\n", Util::NEW_LINE);
		Util::stringReplace(str, "\r", Util::NEW_LINE);
	}

	std::string cp437ToUTF8(const std::string& path) {
		std::string result;
		for (unsigned char c : path) {
			if (c < 128) {
				result.push_back(c);
			}
			else {
				result.append(CP437_TO_UTF8[c]);
			}
		}

		return result;
	}

}	// end namespace Util
