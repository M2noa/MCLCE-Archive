/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/

#include "Dungeons.h"

#include "world/level/levelgen/structure/StructureTemplate.h"
// #include "world/level/block/entity/StructureBlockEntity.h"
#include "world/level/BlockSource.h"
#include "world/level/block/entity/BlockEntity.h"
#include "nbt/CompoundTag.h"
#include "nbt/DoubleTag.h"
#include "nbt/IntTag.h"
#include "nbt/ListTag.h"
#include "nbt/StringTag.h"
#include "Core/Debug/Log.h"
#include "world/entity/Entity.h"
// #include "world/entity/monster/ZombieVillager.h"
// #include "world/entity/EntityFactory.h"
#include "world/level/Level.h"

//////////////////////////////////////////////////////////////////////////
// Constants

const std::string StructureTemplate::SIZE_TAG = "size";
const std::string StructureTemplate::AUTHOR_TAG = "author";
const std::string StructureTemplate::VERSION_TAG = "version";
const std::string StructureTemplate::PALETTE_TAG = "palette";

const std::string StructureTemplate::BLOCKS_TAG = "blocks";
const std::string StructureTemplate::BLOCK_TAG_POS = "pos";
const std::string StructureTemplate::BLOCK_TAG_STATE = "state";
const std::string StructureTemplate::BLOCK_TAG_NBT = "nbt";

const std::string StructureTemplate::ENTITIES_TAG = "entities";
const std::string StructureTemplate::ENTITY_TAG_POS = "pos";
const std::string StructureTemplate::ENTITY_TAG_BLOCKPOS = "blockPos";
const std::string StructureTemplate::ENTITY_TAG_NBT = "nbt";

const std::string StructureTemplate::MINECRAFT_PREFIX = "minecraft:";
const int StructureTemplate::STRUCTURE_VERSION = 1;
const int StructureTemplate::CHUNK_SIZE = 16;

const StructureSettings StructureTemplate::defaultSettings = StructureSettings();

//////////////////////////////////////////////////////////////////////////
// StructureTemplate

StructureTemplate::StructureTemplate() {
}

StructureTemplate::StructureTemplate(const StructureTemplate &rhs) :
	mAuthor(rhs.mAuthor),
	mSize(rhs.mSize),
	mPalette(rhs.mPalette) {

	mBlockInfo.reserve(rhs.mBlockInfo.size());
	for (auto& blockInfo : rhs.mBlockInfo) {
		mBlockInfo.push_back(blockInfo);
	}
	mEntityInfo.reserve(rhs.mEntityInfo.size());
	for(auto& entityInfo : rhs.mEntityInfo) {
		mEntityInfo.push_back(entityInfo);
	}
}

StructureTemplate& StructureTemplate::operator=(const StructureTemplate& rhs) {
	mAuthor = rhs.mAuthor;
	mSize = rhs.mSize;
	mPalette = rhs.mPalette;
	mBlockInfo.clear();
	mEntityInfo.clear();

	mBlockInfo.reserve(rhs.mBlockInfo.size());
	for (auto& blockInfo : rhs.mBlockInfo) {
		mBlockInfo.push_back(blockInfo);
	}
	mEntityInfo.reserve(rhs.mEntityInfo.size());
	for (auto& entityInfo : rhs.mEntityInfo) {
		mEntityInfo.push_back(entityInfo);
	}

	return *this;
}

BlockPos StructureTemplate::calculateConnectedPosition(const StructureSettings& settings1, const BlockPos& connection1, const StructureSettings& settings2, const BlockPos& connection2) const {
	BlockPos markerPos1 = _calculateRelativePosition(connection1, settings1);
	BlockPos markerPos2 = _calculateRelativePosition(connection2, settings2);
	return markerPos1 - markerPos2;
}

std::unordered_map<BlockPos, std::string> StructureTemplate::getMarkers(const BlockPos& position, StructureSettings& settings) {
	std::unordered_map<BlockPos, std::string> result;

	auto& boundingBox = settings.getBoundingBox();
	for (auto&& blockInfo : mBlockInfo) {
		BlockPos blockPos = _calculateRelativePosition(blockInfo.mPos, settings).offset(position.x, position.y, position.z);
		if (boundingBox.isValid() && !boundingBox.isInside(blockPos)) {
			continue;
		}
		FullBlock state = blockInfo.mBlock;
		if ((Block::mStructureBlock && state.getBlock().getId() != Block::mStructureBlock->mID) ||
			blockInfo.mTag == nullptr || blockInfo.mTag->isEmpty()) {
			continue;
		}
// 		StructureBlockEntity::StructureType mode = StructureBlockEntity::stringToStructureType(blockInfo.mTag->getString("mode"));
// 		if (mode != StructureBlockEntity::StructureType::DATA) {
// 			continue;
// 		}
		const std::string metaData = blockInfo.mTag->getString("metadata");
		result.insert(std::make_pair(blockPos, metaData));
	}
	return result;
}

void StructureTemplate::placeInWorld(BlockSource& region, const BlockPos& pos, StructureSettings& settings) {
	BlockPlacementProcessor blockProcessor(settings);

	const Block *ignoreBlock = settings.getIgnoreBlock();
	auto& boundingBox = settings.getBoundingBox();

	for (StructureBlockInfo &blockInfo : mBlockInfo) {
		BlockID currID = blockInfo.mBlock.getBlock().getId();

		// Check if the block is to be ignored
		if (ignoreBlock && currID == ignoreBlock->getId()) {
			continue;
		}

		// Check if the block is a structure block and it should be ignored
		if (settings.isIgnoreStructureBlocks() && Block::mStructureBlock && currID == Block::mStructureBlock->mID) {
			continue;
		}

		// Check if the block can be placed using structure integrity
		if (blockProcessor.canPlace()) {
			BlockPos blockPos = _calculateRelativePosition(blockInfo.mPos, settings) + pos;

			if (boundingBox.isValid() && !boundingBox.isInside(blockPos)) {
				continue;
			}

			DataID data = _mapToData(currID, blockInfo, settings);

			// Clear the block if it's a block entity so the data can be refreshed
			if (blockInfo.mTag && !blockInfo.mTag->isEmpty()) {
				BlockEntity *blockEntity = region.getBlockEntity(blockPos);
				if (blockEntity) {
					region.setBlock(blockPos, Block::mAir->getId(), Block::UPDATE_INVISIBLE);
				}
			}

			// Place the block, hopefully everything worked fine
			if (region.setBlockAndData(blockPos, currID, data, Block::UPDATE_CLIENTS)) {
				// For block entities, handle the extra data
				if (blockInfo.mTag && !blockInfo.mTag->isEmpty()) {
					BlockEntity *blockEntity = region.getBlockEntity(blockPos);
					if (blockEntity) {
						blockInfo.mTag->putInt("x", blockPos.x);
						blockInfo.mTag->putInt("y", blockPos.y);
						blockInfo.mTag->putInt("z", blockPos.z);
						blockEntity->load(*blockInfo.mTag); // TODO (venvious): Need to translate from Java to MC++ right here
					}
				}
			}
			else {
				// Most likely a block of that type and data already existed there
				LOGW("Block [%s] could not be placed from structure template\n", blockInfo.mBlock.getBlock().getRawNameId().data());
			}
		}
	}
	for (StructureBlockInfo &blockInfo : mBlockInfo) {
		BlockID currID = blockInfo.mBlock.getBlock().getId();
		// Check if the block is to be ignored
		if (ignoreBlock && currID == ignoreBlock->getId()) {
			continue;
		}

		// Refresh the block entities so they update
		BlockPos blockPos = _calculateRelativePosition(blockInfo.mPos, settings) + pos;
		if (blockInfo.mTag && !blockInfo.mTag->isEmpty()) {
			BlockEntity *blockEntity = region.getBlockEntity(blockPos);
			if (blockEntity) {
				blockEntity->setChanged();
			}
		}
	}
}

void StructureTemplate::placeInWorldChunk(BlockSource& region, const BlockPos& position, StructureSettings& settings) {
	settings.updateBoundingBoxFromChunkPos();
	placeInWorld(region, position, settings);
}

void StructureTemplate::placeEntities(BlockSource& region, const BlockPos& position, Mirror mirror, Rotation rotation, const BoundingBox& boundingBox) {

	if (!boundingBox.isValid()) {
		return;
	}

	bool zombieVillagerSpawned = false;
	bool villagerSpawned = false;
	const Vec3 centerOffset = Vec3(0.5f, 0.0f, 0.5f);

	for (StructureEntityInfo &entityInfo : mEntityInfo) {
		// remove decimal component of position
		BlockPos entityPos = BlockPos(entityInfo.mPos);

		// ensure the entities being spawned are within the current chunk
		BlockPos blockPos = _transform(BlockPos(entityPos.x, entityPos.y, entityPos.z), mirror, rotation).offset(position.x, position.y, position.z);
		if (!boundingBox.isInside(blockPos)) {
			continue;
		}

		Vec3 relativePos = _transform(entityPos, mirror, rotation);
		Vec3 pos = relativePos.add((float)position.x, (float)position.y, (float)position.z);
		pos += centerOffset;

// 		if (!villagerSpawned) {
// 			auto result = EntityFactory::createSpawnedEntity(EntityDefinitionIdentifier("minecraft", EntityTypeToString(EntityType::Villager), "become_cleric"), nullptr, pos, Vec2::ZERO);
// 			region.getLevel().addEntity(region, std::move(result));
// 			villagerSpawned = true;
// 			continue;
// 		}
// 
// 		if (!zombieVillagerSpawned) {
// 			auto result = EntityFactory::createSpawnedEntity(EntityDefinitionIdentifier("minecraft", EntityTypeToString(EntityType::ZombieVillager), "become_cleric"), nullptr, pos, Vec2::ZERO);
// 			result->setPersistent();
// 			region.getLevel().addEntity(region, std::move(result));
// 			zombieVillagerSpawned = true;
// 			continue;
// 		}
	}
}

CompoundTag &StructureTemplate::save(CompoundTag &tag) {
	// Store all block data
	ListTag blockList;
	for (StructureBlockInfo &blockInfo : mBlockInfo) {
		CompoundTag blockTag;

		Unique<ListTag> blockPos = make_unique<ListTag>();
		blockPos->add(make_unique<IntTag>("", blockInfo.mPos.x));
		blockPos->add(make_unique<IntTag>("", blockInfo.mPos.y));
		blockPos->add(make_unique<IntTag>("", blockInfo.mPos.z));
		blockTag.put(BLOCK_TAG_POS, std::move(blockPos));

		blockTag.putInt(BLOCK_TAG_STATE, mPalette.getId(blockInfo.mBlock));
		if (!blockInfo.mTag->isEmpty()) {
			blockTag.put(BLOCK_TAG_NBT, blockInfo.mTag->copy());
		}
		blockList.add(blockTag.copy());
	}

	// Store all entity data
	ListTag entityList;
	for (StructureEntityInfo &entityInfo : mEntityInfo) {
		CompoundTag entityTag;

		Unique<ListTag> blockPos = make_unique<ListTag>();
		// java saves position data with double tags. to read generically we do the same
		blockPos->add(make_unique<DoubleTag>("", entityInfo.mPos.x));
		blockPos->add(make_unique<DoubleTag>("", entityInfo.mPos.y));
		blockPos->add(make_unique<DoubleTag>("", entityInfo.mPos.z));
		entityTag.put(ENTITY_TAG_POS, std::move(blockPos));

		entityTag.putInt(BLOCK_TAG_STATE, mPalette.getId(entityInfo.mBlock));
		if (!entityInfo.mTag->isEmpty()) {
			entityTag.put(ENTITY_TAG_NBT, entityInfo.mTag->copy());
		}
		entityList.add(entityTag.copy());
	}

	// Store the palette
	ListTag paletteList;
	for (int i = 0; i < mPalette.getSize(); ++i) {
		CompoundTag paletteTag;
		const FullBlock &blockAndData = mPalette.getBlock(i);

		// Get block name from ID
		int id = blockAndData.id;
		std::string saveName(MINECRAFT_PREFIX);
		for (auto &block : Block::mBlockLookupMap) {
			if (block.second->getId() == id) {
				saveName.append(block.first);
				break;
			}
		}
		paletteTag.putString("Name", saveName);

		// currently we are reading in the java format NBT files, so we will save them in the same format
		// until we care about saving out structure blocks, this will not need to be replaced
		_mapToString(paletteTag, blockAndData.data);

		paletteList.add(paletteTag.copy());
	}

	// Add all gathered data do the tag
	tag.put(PALETTE_TAG, paletteList.copy());
	tag.put(BLOCKS_TAG, blockList.copy());
	tag.put(ENTITIES_TAG, entityList.copy());

	// Put the structure size
	Unique<ListTag> blockPos = make_unique<ListTag>();
	blockPos->add(make_unique<IntTag>("", mSize.x));
	blockPos->add(make_unique<IntTag>("", mSize.y));
	blockPos->add(make_unique<IntTag>("", mSize.z));
	tag.put(SIZE_TAG, std::move(blockPos));

	tag.putInt(VERSION_TAG, STRUCTURE_VERSION);
	tag.putString(AUTHOR_TAG, mAuthor);

	return tag;
}

void StructureTemplate::load(CompoundTag &tag) {
	mBlockInfo.clear();
	mEntityInfo.clear();
	mPalette.clearMap();

	const ListTag *sizeTag = tag.getList(SIZE_TAG);
	mSize = BlockPos(sizeTag->getInt(0), sizeTag->getInt(1), sizeTag->getInt(2));
	mAuthor = tag.getString(AUTHOR_TAG);

	const ListTag *paletteList = tag.getList(PALETTE_TAG);
	for (int i = 0; i < paletteList->size(); ++i) {
		// Get the block name (as a copy since MINECRAFT_PREFIX must be stripped off)
		std::string blockName = paletteList->getCompound(i)->getString("Name");
		// Strip "minecraft:" off the name, I know this is minecraft!
		auto found = blockName.find(MINECRAFT_PREFIX);
		if (found != std::string::npos) {
			blockName = blockName.substr(MINECRAFT_PREFIX.length());
		}

		// Get the properties
		const CompoundTag *propertiesTag = paletteList->getCompound(i)->getCompound("Properties");
		int dataVariant = 0;
		if (propertiesTag && !propertiesTag->isEmpty()) {
			//////////////////////////////////////////////////////////////////////////
			// Just for MC++, takes precedence over the JervaMC string states
			//////////////////////////////////////////////////////////////////////////
			if (propertiesTag->contains("dataID", Tag::Type::Int)) {
				dataVariant = propertiesTag->getInt("dataID");
			}
			else {
				// Do some LOVELY string compares, tell me moar pls
				for (auto &propertyString : *propertiesTag) {
					const std::string &valueString = propertiesTag->getString(propertyString.first);
					dataVariant |= _mapToProperty(propertyString.first, valueString, blockName);
				}
			}
		}

		// TODO: @hack: NBT names don't map MCPE name.
		_mapToBlock(blockName, dataVariant);

		// Store the block data
		const Block* block = Block::lookupByName(blockName);

		// Quick hack for now TODO: Make a translator
		if (!block) {
			block = Block::mAir;
			dataVariant = 0;
		}

		mPalette.addMapping(i, FullBlock(block->getId(), dataVariant));
	}

	// Read in blocks
	const ListTag *blockList = tag.getList(BLOCKS_TAG);
	for (int i = 0; i < blockList->size(); ++i) {
		const CompoundTag *blockTag = blockList->getCompound(i);
		const ListTag *posTag = blockTag->getList(BLOCK_TAG_POS);

		BlockPos pos(posTag->getInt(0), posTag->getInt(1), posTag->getInt(2));
		const FullBlock &fullBlock = mPalette.getBlock(blockTag->getInt(BLOCK_TAG_STATE));
		if (blockTag->getCompound(BLOCK_TAG_NBT)) {
			Unique<CompoundTag> infoTag = _mapTag(blockTag->getCompound(BLOCK_TAG_NBT)->clone());
			mBlockInfo.emplace_back(pos, *infoTag.get(), fullBlock);
		}
		else {
			CompoundTag dummy;
			dummy.setName(BLOCK_TAG_NBT);
			mBlockInfo.emplace_back(pos, dummy, fullBlock);
		}
		
	}

	// Read in entities  TODO: Actually do the stuff
	const ListTag *entityList = tag.getList(ENTITIES_TAG);
	for (int i = 0; i < entityList->size(); ++i) {
		const CompoundTag *entityTag = entityList->getCompound(i);
		const ListTag *posTag = entityTag->getList(ENTITY_TAG_POS);
		// double is required because java saves the position with double tags
		Vec3 pos = Vec3((float)posTag->getDouble(0), (float)posTag->getDouble(1), (float)posTag->getDouble(2));
		const FullBlock &fullBlock = mPalette.getBlock(i);
		const CompoundTag *infoTag = entityTag->getCompound(ENTITY_TAG_NBT);
	    mEntityInfo.emplace_back(pos, *infoTag, fullBlock);
	}
}

void StructureTemplate::fillFromWorld(BlockSource& region, const BlockPos& pos, const BlockPos& size, bool includeEntities /*, Block ignoreBlock*/) {
	// Get the starting positions
	BlockPos corner2 = pos.offset(size.x, size.y, size.z).offset(-1, -1, -1);
	BlockPos minCorner(std::min(pos.x, corner2.x), std::min(pos.y, corner2.y), std::min(pos.z, corner2.z));
	BlockPos maxCorner(std::max(pos.x, corner2.x), std::max(pos.y, corner2.y), std::max(pos.z, corner2.z));
	mSize = size;

	// Prepare for the new blocks
	mBlockInfo.clear();
	mEntityInfo.clear();
	mPalette.clearMap();

	// Go through all blocks from min to max and fill the block array with them
	for (int xx = minCorner.x; xx <= maxCorner.x; ++xx) {
		for (int yy = minCorner.y; yy <= maxCorner.y; ++yy) {
			for (int zz = minCorner.z; zz <= maxCorner.z; ++zz) {
				BlockPos currPos(xx, yy, zz);
				BlockPos relativePos = currPos - minCorner;
				FullBlock block = region.getBlockAndData(currPos);

				// If the block is a structure void, leave a hole
				if (block.getBlock().getId() == Block::mStructureVoid->getId()) {
					continue;
				}

				int paletteBlockID = mPalette.getId(block);

				// Add needed extra data if the position is a block entity
				CompoundTag tag;
				BlockEntity *blockEntity = region.getBlockEntity(relativePos);
				if (blockEntity) {
					blockEntity->save(tag);
					tag.remove("x");
					tag.remove("y");
					tag.remove("z");
				}

				mBlockInfo.emplace_back(relativePos, tag, mPalette.getBlock(paletteBlockID));
			}
		}
	}

	// TODO: Fill with entities if "includeEntities"
	if (includeEntities) {
		
	}
	else {
		mEntityInfo.clear();
	}
}

void StructureTemplate::fillEntityList(BlockSource& region, const BlockPos& minCorner, BlockPos& maxCorner) {
	//std::list<Entity> entities = region.getEntities(EntityType::
}

void StructureTemplate::setAuthor(std::string author) {
	mAuthor = author;
}

BlockPos StructureTemplate::getSize(Rotation rotation) {
	switch (rotation) {
	case Rotation::ROTATE_90:
	case Rotation::ROTATE_270:
		return BlockPos(mSize.z, mSize.y, mSize.x);
	default:
		return mSize;
	}
}

BlockPos StructureTemplate::_calculateRelativePosition(BlockPos pos, const StructureSettings& settings) const {
	Vec3 vecForm = _calculateRelativePosition(Vec3((float)pos.x, (float)pos.y, (float)pos.z), settings);
	return BlockPos(vecForm.x, vecForm.y, vecForm.z);
}

BlockPos StructureTemplate::_transform(BlockPos pos, Mirror mirror, Rotation rotation) const {
	Vec3 vecForm = _transform(Vec3((float)pos.x, (float)pos.y, (float)pos.z), mirror, rotation);
	return BlockPos(vecForm.x, vecForm.y, vecForm.z);
}

Vec3 StructureTemplate::_calculateRelativePosition(Vec3 pos, const StructureSettings& settings) const {
	return _transform(pos, settings.getMirror(), settings.getRotation());
}

Vec3 StructureTemplate::_transform(Vec3 pos, Mirror mirror, Rotation rotation) const {
	switch (mirror) {
	case Mirror::LEFT_RIGHT:
		pos.z = -pos.z;
		break;
	case Mirror::FRONT_BACK:
		pos.x = -pos.x;
		break;
	default:
		break;
	}

	switch (rotation) {
	case Rotation::ROTATE_90:
		std::swap(pos.x, pos.z);
		pos.x = -pos.x;
		break;
	case Rotation::ROTATE_180:
		pos.x = -pos.x;
		pos.z = -pos.z;
		break;
	case Rotation::ROTATE_270:
		std::swap(pos.x, pos.z);
		pos.z = -pos.z;
		break;
	default:
		break;
	}

	return pos;
}
