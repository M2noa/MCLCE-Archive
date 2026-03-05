/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

#include "common_header.h"

#include "util/StringUtils.h"

struct Token {

	typedef std::vector<Token> List;

	enum class Type {
		String,
		Number,
		Bool,
	};

	Token(const std::string& str);

	bool isNumber() const {
		return mType == Type::Number;
	}

	int getValue(int defaultValue = 0) const;
	bool getBool(bool defaultValue = true) const;
	float getValueAsFloat (float defaultValue = 0.0f) const;

	const std::string& getText(const std::string& defaultValue = Util::EMPTY_STRING) const;

	bool compatibleWith(Type t) const;

protected:

	std::string mText;

	union {
		int mIntValue;
		bool mBoolValue;
	};

	Type mType;
	bool mIsDefault;

	bool _parseRandom();

public:

	///splits a string in a list of command tokens
	static List tokenize(const std::string& str);

};
