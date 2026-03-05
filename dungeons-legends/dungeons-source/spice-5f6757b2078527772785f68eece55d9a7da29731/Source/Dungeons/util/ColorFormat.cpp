#include "Dungeons.h"

#include "util/ColorFormat.h"
#include "Core/Math/Color.h"

static const std::array<Color, 16> COLORS = {
	Color::fromARGB(0xff000000),	// Black
	Color::fromARGB(0xff0000aa),	// Dark Blue
	Color::fromARGB(0xff00aa00),	// Dark Green
	Color::fromARGB(0xff00aaaa),	// Dark Aqua
	Color::fromARGB(0xffaa0000),	// Dark Red
	Color::fromARGB(0xffaa00aa),	// Dark Purple
	Color::fromARGB(0xffffaa00),	// Gold
	Color::fromARGB(0xffaaaaaa),	// Gray
	Color::fromARGB(0xff555555),	// Dark Gray
	Color::fromARGB(0xff5555ff),	// Blue
	Color::fromARGB(0xff55ff55),	// Green
	Color::fromARGB(0xff55ffff),	// Aqua
	Color::fromARGB(0xffff5555),	// Red
	Color::fromARGB(0xffff55ff),	// Light Purple
	Color::fromARGB(0xffffff55),	// Yellow
	Color::fromARGB(0xffffffff)		// White
};

const std::string* ColorFormat::FromString(const std::string& str) {

	// Lazy Init of static lookup map
	static std::unordered_map<std::string, const std::string*> ENUM_MAP;
	if (ENUM_MAP.empty()) {
		ENUM_MAP["escape"] = &ColorFormat::ESCAPE;

		ENUM_MAP["black"] = &ColorFormat::BLACK;
		ENUM_MAP["dark_blue"] = &ColorFormat::DARK_BLUE;
		ENUM_MAP["dark_green"] = &ColorFormat::DARK_GREEN;
		ENUM_MAP["dark_aqua"] = &ColorFormat::DARK_AQUA;
		ENUM_MAP["dark_red"] = &ColorFormat::DARK_RED;
		ENUM_MAP["dark_purple"] = &ColorFormat::DARK_PURPLE;
		ENUM_MAP["gold"] = &ColorFormat::GOLD;
		ENUM_MAP["gray"] = &ColorFormat::GRAY;
		ENUM_MAP["dark_gray"] = &ColorFormat::DARK_GRAY;
		ENUM_MAP["blue"] = &ColorFormat::BLUE;
		ENUM_MAP["green"] = &ColorFormat::GREEN;
		ENUM_MAP["aqua"] = &ColorFormat::AQUA;
		ENUM_MAP["red"] = &ColorFormat::RED;
		ENUM_MAP["light_purple"] = &ColorFormat::LIGHT_PURPLE;
		ENUM_MAP["yellow"] = &ColorFormat::YELLOW;
		ENUM_MAP["white"] = &ColorFormat::WHITE;

		ENUM_MAP["obfuscated"] = &ColorFormat::OBFUSCATED;
		ENUM_MAP["bold"] = &ColorFormat::BOLD;
		ENUM_MAP["italic"] = &ColorFormat::ITALIC;

		ENUM_MAP["reset"] = &ColorFormat::RESET;
	}

	// Lookup case insensitive by using *lower case* keys.
	auto i = ENUM_MAP.find(Util::toLower(str));
	if (i != ENUM_MAP.end()) {
		return i->second;
	}

	return nullptr;
}

const Color* ColorFormat::ColorFromChar(char colorCode) {
	return ColorFromColorCode(ESCAPE + colorCode);
}

const Color* ColorFormat::ColorFromColorCode(const std::string& colorCode) {
	static const std::unordered_map<std::string, const Color*> StringToColor = {
		{ ColorFormat::BLACK, &COLORS[0] },
		{ ColorFormat::DARK_BLUE, &COLORS[1] },
		{ ColorFormat::DARK_GREEN, &COLORS[2] },
		{ ColorFormat::DARK_AQUA, &COLORS[3] },
		{ ColorFormat::DARK_RED, &COLORS[4] },
		{ ColorFormat::DARK_PURPLE, &COLORS[5] },
		{ ColorFormat::GOLD, &COLORS[6] },
		{ ColorFormat::GRAY, &COLORS[7] },
		{ ColorFormat::DARK_GRAY, &COLORS[8] },
		{ ColorFormat::BLUE, &COLORS[9] },
		{ ColorFormat::GREEN, &COLORS[10] },
		{ ColorFormat::AQUA, &COLORS[11] },
		{ ColorFormat::RED, &COLORS[12] },
		{ ColorFormat::LIGHT_PURPLE, &COLORS[13] },
		{ ColorFormat::YELLOW, &COLORS[14] },
		{ ColorFormat::WHITE, &COLORS[15] }
	};

	auto colorItr = StringToColor.find(colorCode);
	if (colorItr != StringToColor.end()) {
		return colorItr->second;
	}

	return nullptr;
}

bool ColorFormat::IsColorCode(char c) {
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
}

std::string ColorFormat::ColorCodeFromColor(const Color& color) {
	static const std::unordered_map<int, std::string> ColorToString = {
		{ COLORS[0].toARGB(),  ColorFormat::BLACK},
		{ COLORS[1].toARGB(),  ColorFormat::DARK_BLUE },
		{ COLORS[2].toARGB(),  ColorFormat::DARK_GREEN},
		{ COLORS[3].toARGB(),  ColorFormat::DARK_AQUA },
		{ COLORS[4].toARGB(),  ColorFormat::DARK_RED },
		{ COLORS[5].toARGB(),  ColorFormat::DARK_PURPLE },
		{ COLORS[6].toARGB(),  ColorFormat::GOLD },
		{ COLORS[7].toARGB(),  ColorFormat::GRAY },
		{ COLORS[8].toARGB(),  ColorFormat::DARK_GRAY },
		{ COLORS[9].toARGB(),  ColorFormat::BLUE },
		{ COLORS[10].toARGB(), ColorFormat::GREEN },
		{ COLORS[11].toARGB(), ColorFormat::AQUA },
		{ COLORS[12].toARGB(), ColorFormat::RED },
		{ COLORS[13].toARGB(), ColorFormat::LIGHT_PURPLE },
		{ COLORS[14].toARGB(), ColorFormat::YELLOW },
		{ COLORS[15].toARGB(), ColorFormat::WHITE }
	};

	auto colorItr = ColorToString.find(color.toARGB());
	if (colorItr != ColorToString.end()) {
		return colorItr->second;
	}

	return Util::EMPTY_STRING;
}

