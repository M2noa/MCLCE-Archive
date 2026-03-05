/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/storage/LevelStorage.h"

#include "nbt/NbtIo.h"
#include "util/StringUtils.h"
#include "util/StringByteOutput.h"
#include "world/entity/player/Player.h"

const std::string LevelStorage::LOCAL_PLAYER_TAG = "~local_player";

LevelStorage::~LevelStorage() {
}

Unique<CompoundTag> LevelStorage::loadServerPlayerData(const Player& client) {
	std::stringstream ss;
	ss << client.mClientRandomId;
	return loadPlayerData(client.mUniqueName, ss.str());
}

std::string LevelStorage::loadData(const std::string& key) {
	return Util::EMPTY_STRING;
}

void LevelStorage::saveData(const std::string& key, const CompoundTag& tag) {

	std::string buffer;
	StringByteOutput out(buffer);

	NbtIo::write(&tag, out);

	saveData(key, std::move(buffer));
}

bool LevelStorage::clonePlayerData(const std::string& fromKey, const std::string& toKey) {
	auto existingPlayerData = loadPlayerData(fromKey);
	if (existingPlayerData) {
		std::string buffer;
		StringByteOutput out(buffer);

		NbtIo::write(existingPlayerData.get(), out);

		_savePlayerDataSync(toKey, std::move(buffer));
		return true;
	}

	return false;
}

void LevelStorage::savePlayerData(const std::string& key, const CompoundTag& tag) {

	std::string buffer;
	StringByteOutput out(buffer);

	NbtIo::write(&tag, out);

	savePlayerData(key, std::move(buffer));
}
