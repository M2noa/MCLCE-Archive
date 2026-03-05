/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

#include <string>

namespace Util {
	std::string base64_encode(const std::string& str, bool pad = true);
	std::string base64_encode(unsigned char const*, unsigned int len, bool pad = true);
	std::string base64_decode(std::string const& s);
}
