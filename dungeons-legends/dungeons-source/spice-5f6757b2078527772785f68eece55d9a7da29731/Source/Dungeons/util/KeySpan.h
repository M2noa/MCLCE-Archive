/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

#include "common_header.h"

struct KeySpan {
	const std::string name;
	const int start;
	int end;

	KeySpan(const std::string& name, int start, int end) :
		name(name)
		, start(start)
		, end(end){

	}

	KeySpan(const std::string& key, int start) :
		KeySpan(key, start, start) {

	}

	int length() const {
		return end - start;
	}

};

typedef std::vector<KeySpan> KeySpanList;
