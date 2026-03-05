/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/Sapling.h"
#include "world/level/BlockSource.h"
#include "world/level/block/NewLeafBlock.h"
#include "world/level/block/OldLeafBlock.h"
#include "world/level/block/OldLogBlock.h"
#include "world/level/levelgen/feature/SpruceFeature.h"
#include "world/level/levelgen/feature/BirchFeature.h"
#include "world/level/levelgen/feature/OakFeature.h"
#include "world/level/levelgen/feature/MegaJungleTreeFeature.h"
#include "world/level/levelgen/feature/SavannaTreeFeature.h"
#include "world/level/levelgen/feature/RoofTreeFeature.h"
#include "world/level/levelgen/feature/FancyTreeFeature.h"
#include "world/level/levelgen/feature/MegaPineTreeFeature.h"
#include "world/entity/player/Player.h"
#include "world/level/Level.h"
#include "locale/I18n.h"

const int Sapling::TYPE_DEFAULT = enum_cast(OldLeafBlock::LeafType::Oak);
const int Sapling::TYPE_EVERGREEN = enum_cast(OldLeafBlock::LeafType::Spruce);
const int Sapling::TYPE_BIRCH = enum_cast(OldLeafBlock::LeafType::Birch);
const int Sapling::TYPE_JUNGLE = enum_cast(OldLeafBlock::LeafType::Jungle);
const int Sapling::TYPE_ACACIA = NEW_LEAF_OFFSET + NewLeafBlock::ACACIA_LEAF;
const int Sapling::TYPE_ROOFED_OAK = NEW_LEAF_OFFSET + NewLeafBlock::BIG_OAK_LEAF;

Sapling::Sapling(const std::string& nameId, int id) :
	BushBlock(nameId, id, Material::getMaterial(MaterialType::Plant)) {
	float ss = 0.4f;
	setVisualShape(Vec3(0.5f - ss, 0, 0.5f - ss), Vec3(0.5f + ss, ss * 2, 0.5f + ss));
	mRenderLayer = RENDERLAYER_ALPHATEST;
}

DataID Sapling::getSpawnResourcesAuxValue(DataID data) const {
	int type = getBlockState(BlockState::MappedType).get<DataID>(data);

	DataID newData = 0;
	getBlockState(BlockState::MappedType).set(newData, type);

	return newData;
}

bool Sapling::onFertilized(BlockSource& region, const BlockPos& pos, Entity* entity) const {
	Random& random = region.getLevel().getRandom();
	
	//	If the player is in creative mode just let them instantly grow the tree with
	//	a single bonemeal vs having to attempt to advance it in growth.
	if (entity != nullptr && entity->isCreative()) {
		growTree(region, pos, random, entity);
	} else if (random.nextFloat() < 0.45f) {
		advanceTree(region, pos, random, entity);
	}

	return true;
}

bool Sapling::isSapling(BlockSource& region, const BlockPos& pos, int type) const {
	int typeData = getBlockState(BlockState::MappedType).get<int>(region.getData(pos));
	return (region.getBlockID(pos.x, pos.y, pos.z) == mID) && (typeData == type);
}

void Sapling::advanceTree(BlockSource& region, const BlockPos& pos, Random& random, Entity* placer) const {
	DataID data = region.getData(pos);
	if (getBlockState(BlockState::AgeBit).getBool(data) == 0) {
		getBlockState(BlockState::AgeBit).set(data, true);
		region.setBlockAndData(pos, mID, data, Block::UPDATE_NONE);
	} else {
		growTree(region, pos, random, placer);
	}
}

void Sapling::growTree(BlockSource& region, const BlockPos& pos, Random& random, Entity* placer) const {
	if (region.getLevel().isClientSide()) {
		return;
	}

	int type = getBlockState(BlockState::MappedType).get<int>(region.getData(pos));

	DataID data = 0;
	getBlockState(BlockState::MappedType).set(data, type);

	FullBlock sapling(mID, data);

	int ox = 0, oz = 0;
	bool multiblock = false;

	std::unique_ptr<Feature> f;

	switch(sapling.data) {
	case TYPE_DEFAULT:
		if (random.nextInt(10) == 0) {
			f = make_unique<FancyTreeFeature>(placer);
		} else{
			f = make_unique<OakFeature>(placer);
		}
		break;
	case TYPE_EVERGREEN:
		f = generateRedwoodTree(ox, oz, multiblock, pos, region, placer);
		if (!multiblock) {
			ox = oz = 0;
			f = make_unique<SpruceFeature>(placer);
		}
		break;
	case TYPE_BIRCH:
		f = make_unique<BirchFeature>(placer);
		break;
	case TYPE_JUNGLE: {
		f = generateJungleTree(ox, oz, multiblock, pos, region, placer);
		// In case we didn't find a big one, use a little!
		if(!multiblock) {
			ox = oz = 0;
			f = make_unique<TreeFeature>(placer, enum_cast(OldLogBlock::LogType::Jungle), enum_cast(OldLeafBlock::LeafType::Jungle));
		}
		break;
	}
	case TYPE_ACACIA:
		f = make_unique<SavannaTreeFeature>(placer);
		break;
	case TYPE_ROOFED_OAK: {
		f = generateRoofTree(ox, oz, multiblock, pos, region, placer);
		if(!multiblock) {
			return;
		}
		break;
	}
	default:
		f = make_unique<OakFeature>(placer);
		break;
	}

	if(multiblock) {
		region.setBlockNoUpdate(pos.x + ox, pos.y, pos.z + oz, BlockID::AIR);
		region.setBlockNoUpdate(pos.x + ox + 1, pos.y, pos.z + oz, BlockID::AIR);
		region.setBlockNoUpdate(pos.x + ox, pos.y, pos.z + oz + 1, BlockID::AIR);
		region.setBlockNoUpdate(pos.x + ox + 1, pos.y, pos.z + oz + 1, BlockID::AIR);
	} else {
		region.setBlockNoUpdate(pos.x, pos.y, pos.z, BlockID::AIR);
	}

	if(!f->place(region, pos.offset(ox, 0, oz), random)) {
		if(multiblock) {
			region.setBlockAndDataNoUpdate(pos.x + ox, pos.y, pos.z + oz, sapling);
			region.setBlockAndDataNoUpdate(pos.x + ox + 1, pos.y, pos.z + oz, sapling);
			region.setBlockAndDataNoUpdate(pos.x + ox, pos.y, pos.z + oz + 1, sapling);
			region.setBlockAndDataNoUpdate(pos.x + ox + 1, pos.y, pos.z + oz + 1, sapling);
		} else {
			region.setBlockAndDataNoUpdate(pos.x, pos.y, pos.z, sapling);
		}
	}
}

int Sapling::getVariant(int data) const{
	return getBlockState(BlockState::MappedType).get<int>(data);
}

void Sapling::tick(BlockSource& region, const BlockPos& pos, Random& random) const{
	if(region.getLevel().isClientSide()) {
		return;
	}

	BushBlock::tick(region, pos, random);

	if(region.getRawBrightness( pos.above() ) >= Brightness::MAX - 6) {
		if(random.nextInt(7) == 0) {
			advanceTree(region, pos, random, nullptr);
		}
	}
}

std::unique_ptr<Feature> Sapling::generateRedwoodTree(int& ox, int& oz, bool& multiblock, const BlockPos& pos, BlockSource& region, Entity* placer) const {
	for (ox = 0; ox >= -1; ox--) {
		for (oz = 0; oz >= -1; oz--) {
			if (isSapling(region, pos.offset(ox, 0, oz), TYPE_EVERGREEN) && isSapling(region, pos.offset(ox + 1, 0, oz), TYPE_EVERGREEN) && isSapling(region, pos.offset(ox, 0, oz + 1), TYPE_EVERGREEN)
				&& isSapling(region, pos.offset(ox + 1, 0, oz + 1), TYPE_EVERGREEN)) {
				multiblock = true;
				return make_unique<MegaPineTreeFeature>(placer, true);
			}
		}
	}

	return nullptr;
}

std::unique_ptr<Feature> Sapling::generateJungleTree(int& ox, int& oz, bool& multiblock, const BlockPos& pos, BlockSource& region, Entity* placer) const {
	for(ox = 0; ox >= -1; ox--) {
		for(oz = 0; oz >= -1; oz--) {
			if(isSapling(region, pos.offset(ox, 0, oz), TYPE_JUNGLE) && isSapling(region, pos.offset(ox + 1, 0, oz), TYPE_JUNGLE) && isSapling(region, pos.offset(ox, 0, oz + 1), TYPE_JUNGLE)
				&& isSapling(region, pos.offset(ox + 1, 0, oz + 1), TYPE_JUNGLE)) {
				multiblock = true;
				return make_unique<MegaJungleTreeFeature>(placer, 10, 20, enum_cast(OldLogBlock::LogType::Jungle), enum_cast(OldLeafBlock::LeafType::Jungle));
			}
		}
	}

	return nullptr;
}

std::unique_ptr<Feature> Sapling::generateRoofTree(int& ox, int& oz, bool& multiblock, const BlockPos& pos, BlockSource& region, Entity* placer) const {
	for(ox = 0; ox >= -1; ox--) {
		for(oz = 0; oz >= -1; oz--) {
			if(isSapling(region, pos.offset(ox, 0, oz), TYPE_ROOFED_OAK) && isSapling(region, pos.offset(ox + 1, 0, oz), TYPE_ROOFED_OAK)
				&& isSapling(region, pos.offset(ox, 0, oz + 1), TYPE_ROOFED_OAK) && isSapling(region, pos.offset(ox + 1, 0, oz + 1), TYPE_ROOFED_OAK)) {
				multiblock = true;
				return make_unique<RoofTreeFeature>(placer);
			}
		}
	}

	return nullptr;
}

std::string Sapling::buildDescriptionName(DataID data) const {
	int type = getBlockState(BlockState::MappedType).get<int>(data);
	if(type < 0 || type > TYPE_ROOFED_OAK) {
		type = 0;
	}

	static const std::array<std::string, 6> SAPLING_NAMES = {
		"oak", "spruce", "birch", "jungle", "acacia", "big_oak"
	};

	return I18n::get(mDescriptionId + "." + SAPLING_NAMES[type] + ".name");
}
