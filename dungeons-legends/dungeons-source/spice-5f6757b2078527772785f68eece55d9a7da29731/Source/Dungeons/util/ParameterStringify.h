/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

#include "common_header.h"

class ParameterStringify {
public:

	template<typename ... Args>
	static std::vector<std::string> stringify(Args ... args);

	template<typename Value, typename ... Args>
	static void stringifyNext(std::vector<std::string>& returnVector, Value& value, Args ... args);

	template<typename Value>
	static void stringifyNext(std::vector<std::string>& returnVector, Value value);

	static void stringifyNext(std::vector<std::string>& returnVector) {
		UNUSED_PARAMETER(returnVector);
	}

};

template<typename ... Args>
std::vector<std::string>
ParameterStringify::stringify(Args ... args) {
	std::vector<std::string> returnVector;
	stringifyNext(returnVector, args ...);
	return returnVector;
}

template<typename Value, typename ... Args>
void ParameterStringify::stringifyNext(std::vector<std::string>& returnVector, Value& value, Args ... args) {
	std::stringstream ss;
	ss << value;
	returnVector.push_back(ss.str());
	stringifyNext(returnVector, args ...);
}

template<typename Value>
void ParameterStringify::stringifyNext(std::vector<std::string>& returnVector, Value value) {
	std::stringstream ss;
	ss << value;
	returnVector.push_back(ss.str());
}
