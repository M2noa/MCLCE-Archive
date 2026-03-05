#pragma once

#include "common_header.h"

class ListTag;

//Pseudo-SemVer class for the game's version - unfortunately we don't use semver for reals so we can't use the SemVer class. Too bad.
class GameVersion {
public:
	static GameVersion current();

	GameVersion();
	GameVersion(const ListTag& tag);

	std::unique_ptr<ListTag> serialize() const;

	bool operator<(const GameVersion& other) const;

protected:
	GameVersion(uint32_t major, uint32_t minor, uint32_t patch, uint32_t revision);
	uint32_t digit[4];

};


