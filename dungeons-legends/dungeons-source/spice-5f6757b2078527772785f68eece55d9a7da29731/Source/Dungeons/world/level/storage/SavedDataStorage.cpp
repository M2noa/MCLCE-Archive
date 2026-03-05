/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/storage/SavedDataStorage.h"

#include "nbt/CompoundTag.h"
#include "nbt/NbtIo.h"
#include "nbt/ShortTag.h"
#include "nbt/Tag.h"
#include "Core/Debug/Log.h"
#include "world/level/saveddata/SavedData.h"
#include "world/level/storage/LevelStorage.h"
#include "util/StringByteInput.h"
#include "util/StringByteOutput.h"


SavedDataStorage::SavedDataStorage(LevelStorage* levelStorage)
	: levelStorage(levelStorage) {
}

bool SavedDataStorage::load(SavedData* inoutData, const std::string& id) {
	// If already loaded, return it
	auto i = savedDatas.find(id);
	if (i != savedDatas.end()) {
		return true;
	}

	if (levelStorage == nullptr) {
		return false;
	}

	auto uncompressedBuf = levelStorage->loadData(id);
	if (uncompressedBuf.empty()) {
		return false;
	}

	// Read data from buffer to tags
	StringByteInput input(uncompressedBuf);
	Unique<CompoundTag> root = NbtIo::read(input);

	// Call the SavedData::load() interface to pass loaded tags along
	if (auto data = root->getCompound("data")) {
		inoutData->load(*data);
	} else {
		return false;
	}

	// Now that data is loaded, save the data pointer if get() is called again.
	savedDatas[id] = inoutData;

	return true;
}

void SavedDataStorage::set(const std::string& id, SavedData* data) {
	if (data == nullptr) {
		LOGI("ERROR: SavedDataStorage::set() Can't set null data\n");
		return;
	}

	// override existing id
	savedDatas[id] = data;
}

void SavedDataStorage::save() {
	for(auto&& i : savedDatas) {
		if (i.second->isDirty()) {
			_save(i.second);
			i.second->setDirty(false);
		}
	}
}

void SavedDataStorage::_save(SavedData* data) {
	if (levelStorage == nullptr) {
		return;
	}

	auto dataTag = make_unique<CompoundTag>();
	data->save(*dataTag.get());

	auto tag = make_unique<CompoundTag>();
	tag->putCompound("data", std::move(dataTag));

	levelStorage->saveData(data->getId(), *tag);
}
