/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/DoublePlantBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/level/block/TallGrass.h"
#include "world/entity/player/Player.h"
#include "world/item/Item.h"

DoublePlantBlock::DoublePlantBlock(const std::string& nameId, int id) :
	BushBlock(nameId, id, Material::getMaterial(MaterialType::ReplaceablePlant)) {
	mRenderLayer = RENDERLAYER_ALPHATEST;

	float ss = 0.2f;
	mBottomVisualShape.set(Vec3(0.5f - ss, 0, 0.5f - ss), Vec3(0.5f + ss, 1, 0.5f + ss));
	setVisualShape(Vec3(0.5f - ss, 0, 0.5f - ss), Vec3(0.5f + ss, ss * 3, 0.5f + ss));
}

int DoublePlantBlock::getType(BlockSource& region, const BlockPos& pos) const {
	int data = region.getData(pos);
	int plantType = 0;
	if (isTop(data)) {
		plantType = getBlockState(BlockState::MappedType).get<int>(region.getData(pos.below()));
	}
	else {
		plantType = getBlockState(BlockState::MappedType).get<int>(data);
	}

	if (plantType >= enum_cast(DoublePlantType::Count)) {
		plantType = enum_cast(DoublePlantType::Grass);
	}

	return plantType;
}

int DoublePlantBlock::getOrientation(BlockSource& region, const BlockPos& pos) {
	int data = region.getData(pos);
	if (isTop(data)) {
		return getBlockState(BlockState::Direction).get<int>(data);
	}
	else {
		return getBlockState(BlockState::Direction).get<int>(region.getData(pos.above()));
	}
}

bool DoublePlantBlock::mayPlace(BlockSource& region, const BlockPos& pos, FacingID face) const {
	return BushBlock::mayPlace(region, pos) && region.isEmptyBlock(pos.above());
}

bool DoublePlantBlock::mayPlace(BlockSource& region, const BlockPos& pos) const {
	return mayPlace(region, pos, 0);
}

bool DoublePlantBlock::_isCoveredByTopSnow(BlockSource& region, const BlockPos& pos) const {
	if (isTop(region, pos) && region.getBlockID(pos.below()) == Block::mTopSnow->getId()) {
		return true;
	}
	return false;
}

void DoublePlantBlock::checkAlive(BlockSource& region, const BlockPos& pos) const {
	if (!canSurvive(region, pos)) {
		if (!_isCoveredByTopSnow(region, pos)) {
// 			region.getLevel().broadcastDimensionEvent(region, LevelEvent::ParticlesDestroyBlock, pos + Vec3(0.5), mID + (region.getData(pos) << Block::TILE_NUM_SHIFT));
			region.setBlockAndData(pos, FullBlock::AIR, Block::UPDATE_ALL);
		}
	}
}

void DoublePlantBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	if (!canSurvive(region, pos)) {
		// spawn resource if block underneath the double plant is removed
		auto data = region.getData(pos);
		if (!isTop(data)) {
			if (pos.below() == neighborPos) {
				spawnResources(region, pos, data);
			}
		}

		if (!_isCoveredByTopSnow(region, pos)) {
// 			region.getLevel().broadcastDimensionEvent(region, LevelEvent::ParticlesDestroyBlock, pos + Vec3(0.5), mID + (data << Block::TILE_NUM_SHIFT));
			region.setBlockAndData(pos, FullBlock::AIR, Block::UPDATE_ALL);
		}
	}
}

bool DoublePlantBlock::canSurvive(BlockSource& region, const BlockPos& pos) const {
	int data = region.getData(pos);
	if (isTop(data)) {
		return region.getBlockID(pos.below()) == mID && BushBlock::canSurvive(region, pos.below());
	}
	else {
		return region.getBlockID(pos.above()) == mID && BushBlock::canSurvive(region, pos);
	}
}

int DoublePlantBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	// note - forced to assume the data from the lower block is passed
	auto type = static_cast<DoublePlantType>(getBlockState(BlockState::MappedType).get<int>(data));
	switch (type) {
	case DoublePlantType::Fern:
		return -1;
	case DoublePlantType::Grass:
		if (random.nextInt(8) == 0) {
			return Item::mSeeds_wheat->getId();
		}

		return -1;
	default:
		return mID;
	}
}

int DoublePlantBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 1 + random.nextInt(1 + (bonusLootLevel * 2));
}

ItemInstance DoublePlantBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(this, 1, getType(region, pos));
}

DataID DoublePlantBlock::getSpawnResourcesAuxValue(DataID data) const {
	return getBlockState(BlockState::MappedType).get<DataID>(data);
}

bool DoublePlantBlock::isTop(BlockSource& region, const BlockPos& pos) const {
	return isTop(region.getData(pos));
}

bool DoublePlantBlock::isTop(DataID data) const {
	return getBlockState(BlockState::UpperBlockBit).getBool(data);
}

int DoublePlantBlock::getColorForParticle(BlockSource& region, const BlockPos& pos, int auxData) const {
	auto type = static_cast<DoublePlantType>(getType(region, pos));
	if (type == DoublePlantType::Grass || type == DoublePlantType::Fern) {
		return Block::mGrass->getColor(region, pos, auxData);
	}

	return 0xffffff;
}

int DoublePlantBlock::getColor(BlockSource& region, const BlockPos& pos, DataID data) const {
	auto type = static_cast<DoublePlantType>(getType(region, pos));
	if (type == DoublePlantType::Grass || type == DoublePlantType::Fern) {
		return Block::mGrass->getColor(region, pos, data);
	}

	return 0xffffff;
}

void DoublePlantBlock::placeAt(BlockSource& region, const BlockPos& pos, DataID plantType, int updateType, Entity* placer) const {
	region.setBlockAndData(pos, {mID, plantType}, updateType, placer);
	getBlockState(BlockState::UpperBlockBit).set(plantType, true);
	region.setBlockAndData(pos.above(), {mID, plantType}, updateType, placer);
}

void DoublePlantBlock::playerDestroy(Player* player, const BlockPos& pos, int data) const {
// 	if(!player->getLevel().isClientSide() && player->getSelectedItem() != nullptr && player->getSelectedItem()->getItem() == Item::mShears) {
// 		if (popGrassResources(player->getRegion(), pos, data, player)) {
// 			return;
// 		}
// 	}

	BushBlock::playerDestroy(player, pos, data);
}

bool DoublePlantBlock::playerWillDestroy(Player& player, const BlockPos& pos, int data) const {
	auto &region = player.getRegion();
	if (_isCoveredByTopSnow(region, pos)) {
		region.setBlockAndData(pos, FullBlock::AIR, Block::UPDATE_ALL);
// 		region.getLevel().broadcastDimensionEvent(region, LevelEvent::ParticlesDestroyBlock, pos + Vec3(0.5f), mID + (data << Block::TILE_NUM_SHIFT));
		return false;
	}
	
	return BushBlock::playerWillDestroy(player, pos, data);
}

bool DoublePlantBlock::popGrassResources(BlockSource& region, const BlockPos& pos, int data, Player* player) const {
	auto type = static_cast<DoublePlantType>(getType(region, pos));
	if ((type == DoublePlantType::Fern || type == DoublePlantType::Grass)) {
		int newType = enum_cast(TallGrassType::Tall);
		if (type == DoublePlantType::Fern) {
			newType = enum_cast(TallGrassType::Fern);
		}

		// drop two tallgrass items
		popResource(region, pos, ItemInstance(Block::mTallgrass, 2, newType));
		return true;
	}

	return false;
}

bool DoublePlantBlock::popFlowerResources(BlockSource& region, const BlockPos& pos, int data, Entity* entity) const {
	int type = getType(region, pos);
	popResource(region, pos, ItemInstance(Block::mDoublePlant, 1, type));

	return true;
}

void DoublePlantBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	DataID data = region.getData(pos);
	if (isTop(data)) {
		return;
	}

	getBlockState(BlockState::UpperBlockBit).set(data, true);
	region.setBlockAndData(pos.above(), mID, data, Block::UPDATE_ALL);
}

void DoublePlantBlock::onLoaded(BlockSource& region, const BlockPos& pos) const {
	DataID data = region.getData(pos);
	if (isTop(data)) {
		int topType = getType(region, pos);
		int bottomType = getType(region, pos.below());
		if (topType != bottomType) {
			getBlockState(BlockState::MappedType).set(data, bottomType);
			region.setBlockAndData(pos, mID, data, Block::UPDATE_ALL);
		}
	}
}

bool DoublePlantBlock::onFertilized(BlockSource& region, const BlockPos& pos, Entity* entity) const {

	int type = getType(region, pos);

	if (type == enum_cast(DoublePlantType::Grass) ||
		type == enum_cast(DoublePlantType::Fern)) {
		return false;
	}

	popFlowerResources(region, pos, type, entity);
	return true;
}


std::string DoublePlantBlock::buildDescriptionName(DataID data) const {
	if (data < 0 || data >= 6) {
		data = 0;
	}

	static const std::string DOUBLE_PLANT__NAMES[6] = {
		"sunflower", "syringa", "grass", "fern", "rose", "paeonia"
	};

	return I18n::get(mDescriptionId + "." + DOUBLE_PLANT__NAMES[data] + ".name");
}

bool DoublePlantBlock::getSecondPart(BlockSource& region, const BlockPos& pos, BlockPos& out) const {
	auto data = region.getData(pos);
	out = isTop(data) ? pos.below() : pos.above();

	return true;
}

void DoublePlantBlock::onGraphicsModeChanged(bool fancy_, bool preferPolyTessellation, bool transparentLeaves) {
	BushBlock::onGraphicsModeChanged(fancy_, preferPolyTessellation, transparentLeaves);
	if (preferPolyTessellation) {
		mRenderLayer = RENDERLAYER_DOUBLE_SIDED;
	}
	else {
		mRenderLayer = RENDERLAYER_ALPHATEST;
	}
}

bool DoublePlantBlock::canBeBuiltOver(BlockSource& region, const BlockPos& pos) const {
	int type = getType(region, pos);
	return type == enum_cast(DoublePlantType::Fern) || type == enum_cast(DoublePlantType::Grass);
}

bool DoublePlantBlock::canBeSilkTouched() const {
	return false;
}


Vec3 DoublePlantBlock::randomlyModifyPosition(const BlockPos& pos, int32_t& seed) const {
	Vec3 v = pos;

	seed = (pos.x * 3129871) ^ (pos.z * 116129781l);
	seed = seed * seed * 42317861 + seed * 11;

	v.x += ((((seed >> 16) & 0xf) / 15.0f) - 0.5f) * 0.3f;
	v.z += ((((seed >> 24) & 0xf) / 15.0f) - 0.5f) * 0.3f;

	return v;
}

Vec3 DoublePlantBlock::randomlyModifyPosition(const BlockPos& pos) const {
	int32_t seed;
	return randomlyModifyPosition(pos, seed);
}

const AABB& DoublePlantBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	if (isClipping) {
		const auto& shape = getVisualShape(region, pos, bufferValue, isClipping);
		return bufferValue.set(shape.min, shape.max).move(randomlyModifyPosition(pos));
	}

	return AABB::EMPTY;
}

const AABB& DoublePlantBlock::getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping) const {
	return getVisualShape(region.getData(pos), bufferAABB, isClipping);
}

const AABB& DoublePlantBlock::getVisualShape(DataID data, AABB& bufferAABB, bool isClipping) const {
	if (isTop(data)) {
		return BushBlock::getVisualShape(data, bufferAABB, isClipping);
	}

	bufferAABB.set(mBottomVisualShape);
	return bufferAABB;
}