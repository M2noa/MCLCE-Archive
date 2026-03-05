/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

#include "common_header.h"
class Color;

namespace ColorFormat {

	static const std::string ESCAPE     = "\xc2\xa7";

	static const std::string BLACK      = "\xc2\xa7" "0";
	static const std::string DARK_BLUE  = "\xc2\xa7" "1";
	static const std::string DARK_GREEN = "\xc2\xa7" "2";
	static const std::string DARK_AQUA  = "\xc2\xa7" "3";
	static const std::string DARK_RED   = "\xc2\xa7" "4";
	static const std::string DARK_PURPLE = "\xc2\xa7" "5";
	static const std::string GOLD       = "\xc2\xa7" "6";
	static const std::string GRAY       = "\xc2\xa7" "7";
	static const std::string DARK_GRAY  = "\xc2\xa7" "8";
	static const std::string BLUE       = "\xc2\xa7" "9";
	static const std::string GREEN      = "\xc2\xa7" "a";
	static const std::string AQUA       = "\xc2\xa7" "b";
	static const std::string RED        = "\xc2\xa7" "c";
	static const std::string LIGHT_PURPLE = "\xc2\xa7" "d";
	static const std::string YELLOW     = "\xc2\xa7" "e";
	static const std::string WHITE      = "\xc2\xa7" "f";

	static const std::string OBFUSCATED = "\xc2\xa7" "k";
	static const std::string BOLD       = "\xc2\xa7" "l";
	static const std::string ITALIC     = "\xc2\xa7" "o";

	static const std::string RESET      = "\xc2\xa7" "r";

	const std::string* FromString(const std::string& str);

	const Color* ColorFromChar(char colorCode);
	const Color* ColorFromColorCode(const std::string& colorCode);

	bool IsColorCode(char c);

	std::string ColorCodeFromColor(const Color& color);

}
