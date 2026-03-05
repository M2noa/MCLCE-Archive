/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/TopSnowBlock.h"
#include "world/entity/Mob.h"
#include "world/level/Level.h"
#include "world/level/material/Material.h"
#include "world/level/BlockSource.h"
#include "world/level/biome/Biome.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/Weather.h"
#include "world/item/Item.h"
// #include "world/entity/item/ItemEntity.h"
#include "world/entity/player/Player.h"
#include "world/item/ItemInstance.h"
#include "world/Facing.h"
#include "world/level/Weather.h"
// #include "world/level/Spawner.h"

const int TopSnowBlock::MAX_HEIGHT = 8;
const int TopSnowBlock::HEIGHT_IMPASSABLE = 5;	// if these many steps high, then can't path over it
const int TopSnowBlock::HEIGHT_MASK = 0x7;		// 3 bits for height (0-7)
const int TopSnowBlock::HAS_COVERED_MASK = 0x8;	// 4th bit for 'is covering something' true/false

std::unordered_set<int> TopSnowBlock::mRecoverableLookupSet;

void TopSnowBlock::registerRecoverableBlock(BlockID blockId) {
	TopSnowBlock::mRecoverableLookupSet.insert(blockId.value);
}

bool TopSnowBlock::checkIsRecoverableBlock(BlockID block){
	return mRecoverableLookupSet.find(block) != mRecoverableLookupSet.end();
}

FullBlock TopSnowBlock::dataIDToRecoverableFullBlock(BlockSource& region, const BlockPos& pos, DataID dataId) {
	// Is 4th bit set?
	if (Block::mTopSnow->getBlockState(BlockState::CoveredBit).getBool(dataId)) {
		uint16_t extraData = region.getExtraData(pos);
		if (extraData != 0) {
			return FullBlock((BlockID)(extraData & 0xFF), (DataID)(extraData >> 8));
		}
	}

	return FullBlock(BlockID::AIR);
}

int TopSnowBlock::dataIDToHeight(DataID dataId) {
	int height = TopSnowBlock::getHeight(dataId) + 1;
	return height;
}

DataID TopSnowBlock::buildData(BlockSource& region, BlockPos const& pos, int height, bool additive) {

	DEBUG_ASSERT(height <= MAX_HEIGHT, "height delta is too large");
	DataID data = 0;

	// If the block id we are placing top snow over is a 'recoverable block id'
	// Then add that to the block's data.
	FullBlock onTopOfBlock = region.getBlockAndData(pos);
	if (onTopOfBlock.id == Block::mTopSnow->mID) {
		// Add 1 to existing height within first 3 bits of data.
		data = region.getData(pos);

		// old height + delta
		if (additive) {
			height = std::min(height + TopSnowBlock::getHeight(data), MAX_HEIGHT - 1);
		} else {
			// when setting rather than adding, -1. because data 0 == 1 layer.
			height -= 1;
		}

		//     has_covered | height
		Block::mTopSnow->getBlockState(BlockState::Height).set(data, height);
	} 
	else {	// replacing existing 'non top snow' block
			// new top snow, so set the height
		Block::mTopSnow->getBlockState(BlockState::Height).set(data, height - 1);

		if (onTopOfBlock.id != Block::mAir->mID) {
			// If we are placing top snow over a 'recoverable' block then remember that in our data field.
			auto i = TopSnowBlock::mRecoverableLookupSet.find(onTopOfBlock.id.value);
			if (i != TopSnowBlock::mRecoverableLookupSet.end()) {

				// Save extra data representing the block we are covering up
				uint16_t extraData = ((onTopOfBlock.data << 8) & 0xFF00) | (onTopOfBlock.id & 0xFF);
				region.setExtraData(pos, extraData);

				// Add the 'has covered' bit
				Block::mTopSnow->getBlockState(BlockState::CoveredBit).set(data, true);
			}
		}
	}

	return data;
}

// *******************************************************************************************
// *******************************************************************************************
// *******************************************************************************************

TopSnowBlock::TopSnowBlock(const std::string& nameId, int id)
	: HeavyBlock(nameId, id, Material::getMaterial(MaterialType::TopSnow)) {
	setVisualShape(Vec3::ZERO, Vec3(1, 1.0f / 8.0f, 1));
	setTicking(true);

	mCanBuildOver = true;
	mBrightnessGamma = 0.45f;
	setSolid(false);
	mProperties = BlockProperty::TopSnow | BlockProperty::Heavy | BlockProperty::BreakOnPush;
	mThickness = 0.13f;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

bool TopSnowBlock::checkIsPathable(Entity& entity, const BlockPos& lastPathPos, const BlockPos& pathPos) const {
	// If layer height is max height, then we cannot pathfind through this block.
	auto &region = entity.getRegion();
	DataID data = region.getData(pathPos);
	int height = TopSnowBlock::dataIDToHeight(data);
	if (height == TopSnowBlock::MAX_HEIGHT) {
		return false;
	}

	// If we are on the same elevation and we are not MAX_HEIGHT, assume pathable.
	if (lastPathPos.y == pathPos.y) {
		return true;
	}

	// If dropping down, then we are pathable if above us is 'air'.
	if (lastPathPos.y > pathPos.y) {
		//DEBUG_ASSERT(entity->region.getBlock(x, y + 1, z) == BlockID::AIR,
		//			 "Shouldn't test this tile unless it's open above us right pathfinder?");
		return true;
	}
	// assume fromPos.y < y

	// Are we way too high?
	int blockYDiff = pathPos.y - lastPathPos.y;
	if (blockYDiff >= 2) {
		return false;
	}

	// if our top snow height is < 4 then the AABB.max.y is 0.
	// Which means you can jump onto this block, ie we are pathable.
	if (height < 4) {
		return true;
	}

	// When pathing upward, test fromPos AABB to our top snow height to determine
	// if we can jump onto this block or not.
	auto& fromBlock = region.getBlock(lastPathPos);
	AABB fromBox;

	if (fromBlock.getCollisionShape(fromBox, entity.getRegion(), lastPathPos, &entity)) {

		float topY = static_cast<float>(pathPos.y) + (static_cast<float>(height) / 8.0f);
		//DEBUG_ASSERT(topY >= fromBox.max.y, "topY should be >= box.max.y");
		float ydiff = topY - fromBox.max.y;
		if (ydiff <= 1.0f) {
			return true;// we can jump to make this
		}
	}

	// can't reach it
	return false;
}

const AABB& TopSnowBlock::getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping) const {
	return getVisualShape(region.getData(pos), bufferAABB, isClipping);
}

const AABB& TopSnowBlock::getVisualShape(DataID data, AABB& bufferAABB, bool isClipping) const {
	int heightData = TopSnowBlock::dataIDToHeight(data);
	DEBUG_ASSERT(heightData <= TopSnowBlock::MAX_HEIGHT, "top snow tile data is too large");

	// Occluding Shape
	float height = static_cast<float>(heightData);
	bufferAABB.set(Vec3::ZERO, Vec3(1.0f, height / 8.0f, 1.0f));
	return bufferAABB;
}

const AABB& TopSnowBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	int heightData = TopSnowBlock::dataIDToHeight(region.getData(pos));
	DEBUG_ASSERT(heightData <= TopSnowBlock::MAX_HEIGHT, "top snow tile data is too large");

	if (!isClipping) {
		if (heightData == 0) {
			return AABB::EMPTY;	// When height <= 1 (walk right over it as though it's not there)

		}
	}

	// Collision Shape
	float height = static_cast<float>(heightData);
	bufferValue.set((float)pos.x, (float)pos.y, (float)pos.z, (float)pos.x + 1.0f, (float)pos.y + (height / 8.0f), (float)pos.z + 1.0f);
	return bufferValue;
}

bool TopSnowBlock::getCollisionShape(AABB& outAABB, BlockSource& region, const BlockPos& pos, Entity* entity) const {
	int heightData = TopSnowBlock::dataIDToHeight(region.getData(pos));
	DEBUG_ASSERT(heightData <= TopSnowBlock::MAX_HEIGHT, "top snow tile data is too large");

	// Compute collision shape in either LOW or HIGH rather than exact height so it
	// doesn't feel 'bumpy' to the player when moving over varying top snow heights.
	float height = heightData < 4 ? 0.0f : 4.0f;

	if (entity != nullptr) {
		if (entity->getEntityTypeId() == EntityType::Rabbit ||
			entity->getEntityTypeId() == EntityType::PolarBear) {
			height = heightData > 1 ? heightData - 1.0f : 1.0f;
		}
		else if (entity->getEntityTypeId() == EntityType::ItemEntity || entity->getEntityTypeId() == EntityType::Arrow) {
			height = static_cast<float>(heightData);
		}
	}

	// Collision Shape
	outAABB.set((float)pos.x, (float)pos.y, (float)pos.z, (float)pos.x + 1.0f, (float)pos.y + (height / 8.0f), (float)pos.z + 1.0f);
	return true;
}

void TopSnowBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	// Don't want to call parent behavior here
}

bool TopSnowBlock::mayPlace(BlockSource& region, const BlockPos& pos) const {
	const Block& belowBlock = region.getBlock(pos.below());
	const Block& block = region.getBlock(pos);

	// Is there a block already in our location that we can't cover up?
	if (!block.isType(Block::mAir) && !block.isSolid() && !checkIsRecoverableBlock(block.mID)) {
		if (block.mID != mID) {
			return false;
		}
	}

	// You can place topSnow on top of a 'full block' of topSnow
	if (belowBlock.isType(Block::mTopSnow)) {
		int height = TopSnowBlock::dataIDToHeight(region.getData(pos.below()));
		if (height == TopSnowBlock::MAX_HEIGHT) {
			return true;
		}
	}

	const Material& blockMaterial = belowBlock.getMaterial();
	if (blockMaterial.isType(MaterialType::Ice)) {
		return false;
	}

	if (blockMaterial.isType(MaterialType::Leaves)) {
		return true;
	}

	return region.getMaterial(pos.below()).getBlocksMotion() && belowBlock.isSolid();
}

bool TopSnowBlock::tryToPlace(BlockSource& region, const BlockPos& pos, DataID data) const {
	FullBlock block = region.getBlockAndData(pos);

	if (mayPlace(region, pos)) {
		if (block.id == mID || block.id == 0) {
			BlockPos tp(pos);

			DataID retData = 0;

			for (int slicesToPlace = 0; slicesToPlace <= data; ++slicesToPlace) {
				retData = TopSnowBlock::buildData(region, tp, 1, true);
				region.setBlockAndData(tp, {mID, retData}, Block::UPDATE_ALL);
				if (retData == MAX_HEIGHT - 1) {
					++tp.y;
				}
			}

			return true;
		} else {
			BlockPos tp(pos);
			int height = getHeight(data);
			DataID retData = 0;

			for (int slicesToPlace = 0; slicesToPlace <= height; ++slicesToPlace) {
				retData = TopSnowBlock::buildData(region, tp, 1, true);
				region.setBlockAndData(tp, {mID, retData}, Block::UPDATE_ALL);
				if (retData == MAX_HEIGHT - 1) {
					++tp.y;
				}
			}
		}

		return true;
	} else {
		return HeavyBlock::tryToPlace(region, pos, data);
	}
}

int TopSnowBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	return TopSnowBlock::buildData(by.getRegion(), pos, 1, true);
}

void TopSnowBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	if (neighborPos.y < pos.y && pos.x == neighborPos.x && pos.z == neighborPos.z) {
		HeavyBlock::neighborChanged(region, pos, neighborPos);
	}
}

void TopSnowBlock::playerDestroy(Player* player, const BlockPos& pos, int data) const {
	Level& level = player->getLevel();
	if (level.isClientSide()) {
		return;
	}

	int resourceCount = getResourceCount(player->mRandom, data);
	int type = Item::mSnowBall->getId();
	const float s = 0.7f;

// 	for (int i = 0; i < resourceCount; ++i) {
// 		const float xo = level.getRandom().nextFloat() * s + (1 - s) * 0.5f;
// 		const float yo = level.getRandom().nextFloat() * s + (1 - s) * 0.5f;
// 		const float zo = level.getRandom().nextFloat() * s + (1 - s) * 0.5f;
// 		level.getSpawner().spawnItem(player->getRegion(), ItemInstance(type, 1, 0), nullptr, Vec3(pos.x + xo, pos.y + yo, pos.z + zo), 10);
// 	}

	player->getRegion().setBlock(pos.x, pos.y, pos.z, BlockID::AIR, Block::UPDATE_ALL);
}

bool TopSnowBlock::playerWillDestroy(Player& player, const BlockPos& pos, int data) const {
	// if top snow was over a 'recoverable' block id, then this function will return that block ID so its used instead of AIR.
	player.getRegion().setBlockAndData(pos, dataIDToRecoverableFullBlock(player.getRegion(), pos, data), Block::UPDATE_ALL, &player);
	return true;
}

void TopSnowBlock::onRemove(BlockSource& region, const BlockPos& pos) const {
	HeavyBlock::onRemove(region, pos);

	// remove extra data
	region.setExtraData(pos, 0);
}

int TopSnowBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Item::mSnowBall->getId();
}

int TopSnowBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return std::max(1, dataIDToHeight(data) / 2);
}

ItemInstance TopSnowBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(Block::mTopSnow);
}

void TopSnowBlock::tick(BlockSource& region, const BlockPos& pos, Random& random) const {
	// Is there a snow block above us?
	// If so don't melt until the snow above us melts.
	BlockID aboveID = region.getBlockID(pos.above());
	if (aboveID.hasProperty(BlockProperty::TopSnow | BlockProperty::Snow)) {
		return;
	}

	int meltHeight = 0;

	// [Melt by not snowing]
	if (region.getLevel().getRandom().nextInt(16) == 0) {
		Weather& weather = region.getDimension().getWeather();
		Biome& biome = region.getBiome(pos);
		if (!weather.isSnowingAt(region, pos) || biome.getTemperature() > Biome::RAIN_TEMP_THRESHOLD) {

			// Don't take away more snow than snow level min
			int minSnowLayers = biome.getMinSnowLayers();
			if (minSnowLayers <= 0) {
				++meltHeight;
			} else {
				// check snow depth vs. minSnowLayers
				int snowDepth = weather.calcSnowBlockDepth(region, pos, minSnowLayers + 1);
				if (snowDepth > minSnowLayers) {
					++meltHeight;	// we can melt this snow
				}
			}
		}
	}

	// [Melt By Brightness]
// 	float brightness = region.getBrightness(LightLayer::BLOCK, pos);
	float brightness = 0;
	if (brightness > 11) {
		meltHeight = std::max(meltHeight, std::min(static_cast<int>(brightness) - 11, TopSnowBlock::MAX_HEIGHT));
	}

	// Remove height?
	if (meltHeight > 0) {
		DataID data = region.getData(pos);
		int currentHeight = TopSnowBlock::dataIDToHeight(data);

		if (meltHeight >= currentHeight) {
			// Remove *all* top snow
			FullBlock block = TopSnowBlock::dataIDToRecoverableFullBlock(region, pos, data);
			region.setBlockAndData(pos.x, pos.y, pos.z, block, Block::UPDATE_ALL);
			return;
		} else {
			// remove some top snow but not all.
			DEBUG_ASSERT(currentHeight - meltHeight >= 0, "setting an invalid data value");
			DataID newData = TopSnowBlock::buildData(region, pos, -meltHeight, true);
			region.setBlockAndData(pos.x, pos.y, pos.z, Block::mTopSnow->mID, newData, Block::UPDATE_CLIENTS);
		}
	}

	BlockID belowID = region.getBlockID( pos.below() );
	if (belowID == 0) {
		HeavyBlock::tick(region, pos, random);
	}
}

bool TopSnowBlock::shouldRenderFace(BlockSource& region, const BlockPos& pos, FacingID face, const AABB& shape) const {
	const Material& m = region.getMaterial(pos);
	if (face == Facing::UP) {
		return true;
	}

	if (m == mMaterial) {
		return false;
	}

	return Block::shouldRenderFace(region, pos, face, shape);
}

float TopSnowBlock::calcGroundFriction(Mob& mob, BlockPos const& blockPos) const {
	return 0.6f;// regular ground friction
}

float TopSnowBlock::calcSnowThickness(Mob* mob, BlockPos const& blockPos) const {
	auto &region = mob->getRegion();
	int heightData = TopSnowBlock::dataIDToHeight(region.getData(blockPos));
	if (heightData >= MAX_HEIGHT) {
		return 1.0f;// Thick snow!
	}

	// Snow under us? if so then assume THICK snow.
	BlockID belowID = region.getBlockID(blockPos.x, blockPos.y - 1, blockPos.z);
	if (belowID.hasProperty(BlockProperty::TopSnow | BlockProperty::Snow)) {
		return 1.0f;// Thick snow!
	}

	// If its only 1 layer of snow, no snow thickness
	if (heightData <= 1) {
		return 0.0f;
	}

	// Friction is based on snow height
	float thickness = static_cast<float>(heightData) / 8.0f;
	DEBUG_ASSERT(thickness > 0.0f && thickness < 1.0f, "top snow height ratio should be between 0 - 1.");
	return thickness;
}

bool TopSnowBlock::canBeBuiltOver(BlockSource& region, const BlockPos& pos) const {
	DataID data = region.getData(pos);
	int height = TopSnowBlock::dataIDToHeight(data);
	return height < TopSnowBlock::MAX_HEIGHT;
}

bool TopSnowBlock::canHaveExtraData() const {
	return true;
}

Color TopSnowBlock::getDustColor(DataID data) const {
	return Color::WHITE;
}

bool TopSnowBlock::shouldStopFalling(Entity& entity) const {
	if (entity.mOnGround) {
		return true;
	}

	return false;
}

bool TopSnowBlock::canBeSilkTouched() const {
	return false;
}

bool TopSnowBlock::isFree(BlockSource& region, const BlockPos& pos) const {
	if (!HeavyBlock::isFree(region, pos)) {
		const Block* block = Block::mBlocks[region.getBlockID(pos)];
		if (block == nullptr) {
			return true;
		}

		if (block->getMaterial().isType(MaterialType::Leaves)) {
			return true;
		}

		return false;
	} else {
		return true;
	}
}

void TopSnowBlock::startFalling(BlockSource& region, const BlockPos& pos, const Block* oldBlock, bool creative) const {
	FullBlock block = region.getBlockAndData(pos);
	FullBlock recoveredBlock = dataIDToRecoverableFullBlock(region, pos, block.data);

	if (recoveredBlock != FullBlock::AIR) {
		getBlockState(BlockState::CoveredBit).set(block.data, false);
		region.setBlockAndData(pos, block, Block::UPDATE_CLIENTS);

		Block::mBlocks[recoveredBlock.id]->spawnResources(region, pos, recoveredBlock.data);
	}

	HeavyBlock::startFalling(region, pos, oldBlock, creative);
}

bool TopSnowBlock::isCovering(const DataID& data) const {
	return getBlockState(BlockState::CoveredBit).getBool(data);
}

int TopSnowBlock::getHeight(const DataID& data) {
	return Block::mTopSnow->getBlockState(BlockState::Height).get<int>(data);
}
