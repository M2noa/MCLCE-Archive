/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include <string>

namespace Json {
	class Reader;
}
class JSONReadVerifier {
public:
	/*
	Use like this:
	Json::Value root...
	Json::Reader reader...

	JSONReadVerifier::verifyRead(file, reader, reader.parse(file, root));
	*/
	static void verifyRead(const std::string& file, Json::Reader& reader, bool result);
};
