/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/TallGrass.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/entity/player/Player.h"
#include "world/item/Item.h"
// #include "world/item/ShearsItem.h"
#include "world/level/block/DoublePlantBlock.h"
#include "world/level/biome/Biome.h"

TallGrass::TallGrass(const std::string& nameId, int id)
	: BushBlock(nameId, id, Material::getMaterial(MaterialType::ReplaceablePlant)) {
	float ss = 0.4f;
	setVisualShape(Vec3(0.5f - ss, 0, 0.5f - ss), Vec3(0.5f + ss, 0.8f, 0.5f + ss));
	mCanBuildOver = true;
	mRenderLayer = RENDERLAYER_ALPHATEST;
}

int TallGrass::getColor( int auxData ) const {
	return Block::mGrass->getColor(auxData);
}

int TallGrass::getColor( BlockSource& region, const BlockPos& pos, DataID data) const {
	return Block::mGrass->getColor(region, pos, data);
}

Color TallGrass::getMapColor(BlockSource& region, const BlockPos& pos) const {
	return Block::mGrass->getMapColor(region, pos);
}

int TallGrass::getResource(Random& random, int data, int bonusLootLevel) const {
	if (random.nextInt(8) == 0) {
		return Item::mSeeds_wheat->getId();
	}

	return -1;
}

void TallGrass::playerDestroy(Player* player, const BlockPos& pos, int data) const {
	if (!player->getLevel().isClientSide() && player->getSelectedItem() != nullptr && player->getSelectedItem()->isInstance( Item::mShears )) {
		//player->awardStat(Stats.blockMined[id], 1);

		// drop leaf block instead of sapling
		ItemInstance itemInstance(Block::mTallgrass, 1, data);
		popResource(player->getRegion(), pos, itemInstance);
	} else {
		BushBlock::playerDestroy(player, pos, data);
	}
}

std::string TallGrass::buildDescriptionName(DataID data) const {
	TallGrassType type = getBlockState(BlockState::MappedType).get<TallGrassType>(data);
	switch(type) {
	case TallGrassType::Tall:
		return I18n::get(mDescriptionId + ".grass.name");
	default:
		return I18n::get(mDescriptionId + ".fern.name");
	}
}

Vec3 TallGrass::randomlyModifyPosition(const BlockPos& pos) const {
	Vec3 v = pos;

	float randomScale = 0.35f;
	int32_t seed = (pos.x * 3129871) ^ (pos.z * 116129781l) ^ (pos.y);
	seed = seed * seed * 42317861 + seed * 11;

	v.x += ((((seed >> 16) & 0xf) / 15.0f) - 0.5f) * randomScale;
	v.y += ((((seed >> 20) & 0xf) / 15.0f) - 1.0f) * randomScale;
	v.z += ((((seed >> 24) & 0xf) / 15.0f) - 0.5f) * randomScale;

	return v;
}

const AABB& TallGrass::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	if (isClipping) {
		const auto& shape = getVisualShape(region, pos, bufferValue, isClipping);
		return bufferValue.set(shape.min, shape.max).move(randomlyModifyPosition(pos));
	}

	return AABB::EMPTY;
}

bool TallGrass::canSurvive(BlockSource& region, const BlockPos& pos) const {
	return mayPlaceOn(region.getBlock(pos.below()));
}

bool TallGrass::onFertilized(BlockSource& region, const BlockPos& pos, Entity* entity) const {
	TallGrassType type = getBlockState(BlockState::MappedType).get<TallGrassType>(region.getData(pos));
	DataID plantType = enum_cast(type) & enum_cast(TallGrassType::Tall) ? enum_cast(DoublePlantType::Grass) : enum_cast(DoublePlantType::Fern);

	if(region.getMaterial(pos.above()).isType(MaterialType::Air) && BushBlock::canSurvive(region, pos)) {
		((const DoublePlantBlock*)Block::mDoublePlant)->placeAt(region, pos, plantType, Block::UPDATE_ALL, entity);
	} else {
		return false;
	}

	return true;
}

bool TallGrass::mayConsumeFertilizer(BlockSource& region) const {
	//only consume the bonemeal one in 10 times
	return region.getLevel().getRandom().nextInt(10) == 0;
}

void TallGrass::onGraphicsModeChanged(bool fancy_, bool preferPolyTessellation, bool transparentLeaves) {
	BushBlock::onGraphicsModeChanged(fancy_, preferPolyTessellation, transparentLeaves);
	if(preferPolyTessellation) {
		mRenderLayer = RENDERLAYER_DOUBLE_SIDED;
	} else {
		mRenderLayer = RENDERLAYER_ALPHATEST;
	}
}

bool TallGrass::canBeSilkTouched() const {
	return false;
}
