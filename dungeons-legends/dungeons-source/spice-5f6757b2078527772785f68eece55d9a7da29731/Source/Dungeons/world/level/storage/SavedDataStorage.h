/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <type_traits>

class LevelStorage;
class SavedData;
class CompoundTag;

class SavedDataStorage {
	LevelStorage* levelStorage;
	std::unordered_map<std::string, SavedData*> savedDatas;

public:

	SavedDataStorage(LevelStorage* levelStorage);
	virtual ~SavedDataStorage() {
	}

	template
	<typename StorageType>
	StorageType* get(const std::string& id) {
		static_assert(std::is_base_of<SavedData, StorageType>::value, "Invalid SavedData class provided!");
		auto saveDataItr = savedDatas.find(id);
		if (saveDataItr != savedDatas.end()) {
			return static_cast<StorageType*>(saveDataItr->second);
		}
		else {
			StorageType* newData = new StorageType(id);
			if (!load(newData, id)) {
				delete newData;
			}
			else {
				return newData;
			}
		}
		return nullptr;
	}


	bool load(SavedData* inoutData, const std::string& id);
	void set(const std::string& id, SavedData* data);
	void save();

private:
	void _save(SavedData* data);

};
