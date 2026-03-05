/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/MonsterEggBlock.h"
#include "world/level/material/Material.h"
// #include "world/entity/monster/Silverfish.h"
#include "world/level/Level.h"
#include "world/level/BlockSource.h"
// #include "world/level/Spawner.h"
#include "locale/I18n.h"

MonsterEggBlock::MonsterEggBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Clay)){
}

int MonsterEggBlock::getVariant(int data) const{
	auto type = getBlockState(BlockState::MappedType).get<HostBlockType>(data);

	switch (type) {
	case HostBlockType::Cobblestone:
		return 0;
	case HostBlockType::StoneBrick:
		return 1;
	case HostBlockType::MossyStoneBrick:
		return 2;
	case HostBlockType::CrackedStoneBrick:
		return 3;
	case HostBlockType::ChiseledStoneBrick:
		return 4;
	default:
		return 5;
	}
}

int MonsterEggBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 0;
}

int MonsterEggBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Block::mCobblestone->mID;
}

ItemInstance MonsterEggBlock::getSilkTouchItemInstance(DataID data) const {
	return ItemInstance(_getBlockIdFromData(data), 1, 0);
}

int MonsterEggBlock::_getBlockIdFromData(int data) const {
	int id = -1;
	auto type = getBlockState(BlockState::MappedType).get<HostBlockType>(data);

	switch (type) {
	case HostBlockType::Cobblestone:
		id = Block::mCobblestone->mID;
		break;

	case HostBlockType::StoneBrick:
		id = Block::mStoneBrick->mID;
		break;

	case HostBlockType::MossyStoneBrick:
		id = Block::mStoneBrick->mID;
		break;

	case HostBlockType::CrackedStoneBrick:
		id = Block::mStoneBrick->mID;
		break;

	case HostBlockType::ChiseledStoneBrick:
		id = Block::mStoneBrick->mID;
		break;

	default:
		id = Block::mStone->mID;
		break;
	}

	return id;
}

void MonsterEggBlock::spawnResources(BlockSource& region, const BlockPos& pos, int data, float odds, int bonusLootLevel) const{
// 	if (!region.getLevel().isClientSide()) {
// 		auto silverfish = region.getLevel().getSpawner().spawnMob(region, EntityTypeToString(EntityType::Silverfish, EntityTypeNamespaceRules::ReturnWithNamespace), nullptr, pos);
//       if(silverfish) {
//         silverfish->setPersistent();
//       }
// 	}
}

std::string MonsterEggBlock::buildDescriptionName(DataID data) const {
	std::string locKey = "tile.monster_egg.";

	auto hostBlock = getBlockState(BlockState::MappedType).get<HostBlockType>(data);
	switch (hostBlock) {
	case HostBlockType::Cobblestone:
		locKey += "cobble";
		break;
	case HostBlockType::StoneBrick:
		locKey += "brick";
		break;
	case HostBlockType::MossyStoneBrick:
		locKey += "mossybrick";
		break;
	case HostBlockType::CrackedStoneBrick:
		locKey += "crackedbrick";
		break;
	case HostBlockType::ChiseledStoneBrick:
		locKey += "chiseledbrick";
		break;

	// Note: This default intentionally falls through to HOST_ROCK
	default:
		MCEWARN("Invalid MonsterEggTile type in getName");
	case HostBlockType::Stone:
		locKey += "stone";
	}

	locKey += ".name";

	return I18n::get(locKey);
}

bool MonsterEggBlock::isCompatibleHostBlock(int blockId, int blockData) {
	if (blockId == Block::mStone->mID || blockId == Block::mCobblestone->mID) {
		return blockData == 0;
	}

	return blockId == Block::mStoneBrick->mID;
}

int MonsterEggBlock::getDataForHostBlock(int block, int data) {
	const Block* blockPtr = Block::mBlocks[block];
	if (blockPtr == nullptr) {
		return enum_cast(HostBlockType::Stone);
	}

	int type = Block::mMonsterStoneEgg->getBlockState(BlockState::MappedType).get<int>(data);

	if (type == 0) {
		if (blockPtr->isType(Block::mCobblestone)) {
			return enum_cast(HostBlockType::Cobblestone);
		} else if (blockPtr->isType(Block::mStoneBrick)) {
			return enum_cast(HostBlockType::StoneBrick);
		}
	} else {
		if (blockPtr->isType(Block::mStoneBrick)) {
			switch (type) {
			case 1:
				return enum_cast(HostBlockType::MossyStoneBrick);
			case 2:
				return enum_cast(HostBlockType::CrackedStoneBrick);
			case 3:
				return enum_cast(HostBlockType::ChiseledStoneBrick);
			}
		}
	}

	return enum_cast(HostBlockType::Stone);
}
