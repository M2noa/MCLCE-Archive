#include "Dungeons.h"

#include "util/Token.h"
#include "util/StringUtils.h"
#include "util/Random.h"

#include <string>

Token::Token(const std::string& str) 
	: mText(str) {
	//try to extract a number from the string
	mType = sscanf(str.c_str(), "%d", &mIntValue) != 0 ? Type::Number : Type::String;

	std::string toLower(Util::toLower(mText));
	if (toLower == "true") {
		mType = Type::Bool;
		mBoolValue = true;
	}
	else if (toLower == "false") {
		mType = Type::Bool;
		mBoolValue = false;
	}

	//random numerals
	if (mType == Type::String && _parseRandom()) {
		mType = Type::Number;
	}

	mIsDefault = mText.empty();
}

int Token::getValue(int defaultValue) const {
	return (mIsDefault && compatibleWith(Type::Number)) ? defaultValue : mIntValue;
}

float Token::getValueAsFloat(float defaultValue) const {
	if (mIsDefault && compatibleWith(Type::Number)) {
		return defaultValue;
	}

	float ret = 0.0f;
	if (sscanf(mText.c_str(), "%f", &ret) != 1) {
		return defaultValue;
	}

	return ret;
}

bool Token::getBool(bool defaultValue) const {
	if (mIsDefault && compatibleWith(Type::Bool)) {
		return defaultValue;
	}
	
	switch (mType) {
	case Type::Bool:
		return mBoolValue;
	case Type::Number:
		return mIntValue != 0;
	default:
		return defaultValue;
	}
}

const std::string& Token::getText(const std::string& defaultValue) const {
	return mIsDefault ? defaultValue : mText;
}

bool Token::compatibleWith(Type t) const {
	switch (t) {
	case Type::String:
		return true;
	case Type::Bool:
		return mType == Type::Bool || mType == Type::Number;
	case Type::Number:
		return mType == Type::Number;
	default:
		return false;
	}
}

bool Token::_parseRandom(){
	if (mText[0] == '[' && mText.back() == ']') {	//could be a random range
		int idx = mText.find(':');

		if (idx == std::string::npos) {
			return false;
		}

		// TODO support float
		Token a(mText.substr(1, idx - 1));
		Token b(mText.substr(idx + 1, mText.length() - 2));

		if (!a.isNumber() || !b.isNumber()) {
			return false;
		}

		if (a.mIntValue >= b.mIntValue) {
			return false;
		}

		static Random r;
		mIntValue = a.mIntValue + r.nextInt(b.mIntValue - a.mIntValue);
		return true;
	}
	else{
		return false;
	}
}

Token::List Token::tokenize(const std::string& str){
	List tokens;
	enum {
		S_NONE, S_TOKEN, S_QUOTES
	}

	state = S_NONE;
	std::string currentToken;

	for (int i = 1; i <= (int)str.size(); ++i) {
		char c = (i >= (int)str.size()) ? 0 : str[i];

		if (state == S_NONE) {
			if (c == '"') {
				state = S_QUOTES;	//start reading quotes
			}
			else if (!(c == '\t' || c == ' ' || c == 0)) {
				//not whitespace, start reading a token
				state = S_TOKEN;
				currentToken += c;
			}
		}
		else if (state == S_TOKEN) {
			if (c == '\t' || c == ' ' || c == 0) {
				state = S_NONE;
				tokens.push_back(currentToken);
				currentToken.clear();
			}
			else{
				currentToken += c;
			}
		}
		else if (state == S_QUOTES) {
			if (c == '"' || c == 0) {	//end quotes
				state = S_NONE;
				tokens.push_back(currentToken);
				currentToken.clear();
			}
			else{
				currentToken += c;
			}
		}
	}
	return tokens;
}
