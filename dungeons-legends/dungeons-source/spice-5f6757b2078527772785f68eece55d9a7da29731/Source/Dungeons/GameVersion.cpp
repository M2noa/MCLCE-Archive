#include "Dungeons.h"

#include "GameVersion.h"
#include "nbt/ListTag.h"
#include "SharedConstants.h"

GameVersion GameVersion::current() {
	return{
		static_cast<uint32_t>(SharedConstants::MajorVersion),
		static_cast<uint32_t>(SharedConstants::MinorVersion),
		static_cast<uint32_t>(SharedConstants::PatchVersion),
		static_cast<uint32_t>(SharedConstants::RevisionVersion)
	};
}

GameVersion::GameVersion(uint32_t major, uint32_t minor, uint32_t patch, uint32_t revision) {
	digit[0] = major;
	digit[1] = minor;
	digit[2] = patch;
	digit[3] = revision;
}

GameVersion::GameVersion(const ListTag& tag) : GameVersion(tag.getInt(0), tag.getInt(1), tag.getInt(2), tag.getInt(3)) {}

GameVersion::GameVersion() : GameVersion(0, 0, 0, 0) {}

std::unique_ptr<ListTag> GameVersion::serialize() const {
	return ListTagIntAdder
		(static_cast<int>(digit[0]))
		(static_cast<int>(digit[1]))
		(static_cast<int>(digit[2]))
		(static_cast<int>(digit[3])).done();
}

bool GameVersion::operator<(const GameVersion& other) const {
	for (int i = 0; i < 4; ++i) {
		if (digit[i] < other.digit[i]) {
			return true;
		}
		if (digit[i] > other.digit[i]) {
			return false;
		}
	}
	return false;
}

