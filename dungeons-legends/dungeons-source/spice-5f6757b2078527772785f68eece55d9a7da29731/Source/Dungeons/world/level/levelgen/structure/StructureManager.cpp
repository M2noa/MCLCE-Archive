/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/

#include "Dungeons.h"

#include "world/level/levelgen/structure/StructureManager.h"
//#include "world/level/block/entity/StructureBlockEntity.h"
#include "Core/Resource/ResourceHelper.h"
#include "util/StringByteInput.h"
#include "util/StringByteOutput.h"
//#include "platform/AppPlatform.h"
#include "Core/Debug/Log.h"
#include "util/GzipUtils.h"

#define BUFFERSIZE 0x1000

StructureManager::StructureManager() {
}

StructureTemplate& StructureManager::getOrCreate(const std::string& structurePath) {
	StructureTemplate* structureTemplate = get(structurePath);
	if (!structureTemplate) {
		UniqueLock lock(mRepositoryMutex);
		auto inserted = mStructureRepository.insert(std::make_pair(structurePath, std::make_unique<StructureTemplate>()));
		structureTemplate = inserted.first->second.get();
	}

	DEBUG_ASSERT(structureTemplate != nullptr, "Should never try to return a null StructureTemplate!");
	return *structureTemplate;
}

StructureTemplate* StructureManager::get(const std::string& structurePath) {
	{
		SharedLock lock(mRepositoryMutex);
		auto iter = mStructureRepository.find(structurePath);
		if (iter != mStructureRepository.end()) {
			return iter->second.get();
		}
	}

	return _readStructure(structurePath);
}

bool StructureManager::load(StructureTemplate& structure, std::string& data) {
	BigEndianStringByteInput input(data);
	Tag::Type readType = static_cast<Tag::Type>(input.readByte());

	if (readType == Tag::Type::Compound) {
		// To keep compatible with older versions, skip 2 bytes
		input.readShort();

		//////// Load ////////

		CompoundTag tag(Tag::NullString);
		tag.load(input);
		structure.load(tag);

		return true;
	}

	return false;
}

bool StructureManager::save(const std::string& structurePath) {
	return false;
}

StructureTemplate* StructureManager::_readStructure(const std::string& name) {
	std::string data;
	UniqueLock lock(mRepositoryMutex);

	// Attempt to read from default storage
// 	std::string templatePath = AppPlatform::singleton().getAssetFileFullPath(StructureBlockEntity::STRUCTURE_ASSET_FILE_PREFIX + name + StructureBlockEntity::STRUCTURE_FILE_POSTFIX);
// 	if (!GzipUtil::decompressNBTFromAssets(templatePath, data)) {
// 		return nullptr;
// 	}

	Unique<StructureTemplate> structure = std::make_unique<StructureTemplate>();
	if (load(*structure, data) && name.length() > 0) {
		auto inserted = mStructureRepository.insert(std::make_pair(name, std::move(structure)));
		return inserted.first->second.get();
	}

	return nullptr;
}
