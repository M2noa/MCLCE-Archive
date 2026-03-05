/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/DaylightDetectorBlock.h"

#include "util/Math.h"
#include "world/Facing.h"
// #include "world/level/block/entity/DaylightDetectorBlockEntity.h"
#include "world/level/BlockSource.h"
#include "world/level/dimension/Dimension.h"
// #include "world/level/LightLayer.h"
#include "world/level/material/Material.h"
// #include "world/redstone/circuit/CircuitSystem.h"
// #include "world/redstone/circuit/components/ProducerComponent.h"
#include "world/redstone/Redstone.h"
#include "world/level/Level.h"
#include "world/entity/player/Player.h"

DaylightDetectorBlock::DaylightDetectorBlock(const std::string& nameId, int id, bool isInverted)
	: EntityBlock(nameId, id, Material::getMaterial(MaterialType::Wood))
	, mIsInverted(isInverted) {

	setVisualShape(Vec3(0, 0, 0), Vec3(1, 6.0f / 16.0f, 1));

	setSolid(false);
	setPushesOutItems(true);

	mProperties = BlockProperty::Unspecified;
    mBlockEntityType = BlockEntityType::DaylightDetector;
}

void DaylightDetectorBlock::updateShape(BlockSource& level, const BlockPos& pos) {
	setVisualShape(Vec3(0, 0, 0), Vec3(1, 6.0f / 16.0f, 1));
}

void DaylightDetectorBlock::onLoaded(BlockSource& region, const BlockPos& pos) const {
// 	if (!region.getLevel().isClientSide()) {
// 		auto producer = region.getDimension().getCircuitSystem().create<ProducerComponent>(pos, &region);
// 		if (producer) {
// 			int currentStrength = getBlockState(BlockState::RedstoneSignal).get<int>(region.getData(pos));
// 			producer->setStrength(currentStrength);
// 			producer->setAllowPowerUp(true);
// 		}
// 	}
}

void DaylightDetectorBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	onLoaded(region, pos);
}


void DaylightDetectorBlock::onRemove(BlockSource& region, const BlockPos& pos) const {
// 	if (!region.getLevel().isClientSide()) {
// 		region.getDimension().getCircuitSystem().removeComponents(pos);
// 	}
}

void DaylightDetectorBlock::updateSignalStrength(BlockSource& region, const BlockPos& pos) const {
// 	auto& dimension = region.getDimension();
// 	auto& skyLight = dimension.getSkyLightLayer();
// 	if (skyLight.getSurrounding() == Brightness::MIN) {
// 		return;
// 	}
// 	
// 	int targetStrength = static_cast<int>(region.getBrightness(skyLight, pos) - dimension.getSkyDarken());
// 	float sunAngle = dimension.getSunAngle(1);
// 
// 	// tilt sunAngle towards zenith (to make the transition to night
// 	// smoother)
// 	if (sunAngle < PI) {
// 		sunAngle = sunAngle + (0 - sunAngle) * 0.2f;
// 	} else {
// 		sunAngle = sunAngle + (PI * 2.0f - sunAngle) * 0.2f;
// 	}
// 
// 	targetStrength = static_cast<int>(round(static_cast<float>(targetStrength) * Math::cos(sunAngle)));
// 	targetStrength = Math::clamp(targetStrength, 0, Redstone::SIGNAL_MAX);
// 
// 	if (mIsInverted) {
// 		targetStrength = Redstone::SIGNAL_MAX - targetStrength;
// 	}
// 
// 	FullBlock ft = region.getBlockAndData(pos);
// 	getBlockState(BlockState::RedstoneSignal).set(ft.data, targetStrength);
// 	region.setBlockAndData(pos, ft, UPDATE_ALL);
// 
// 	if (!region.getLevel().isClientSide()) {
// 		dimension.getCircuitSystem().setStrength(pos, targetStrength);
// 	}
}

bool DaylightDetectorBlock::use(Player& player, const BlockPos& pos) const {
// 	BlockSource& region = player.getRegion();
// 
// 	if (!region.getLevel().isClientSide()) {
// 		CircuitSystem& system = region.getDimension().getCircuitSystem();
// 		system.lockGraph(true);
// 		DataID data = Redstone::SIGNAL_MAX - getBlockState(BlockState::RedstoneSignal).get<int>(region.getData(pos));
// 		if (mIsInverted) {
// 			player.getRegion().setBlockAndData(pos, FullBlock(Block::mDaylightDetector->mID, data), Block::UPDATE_CLIENTS, &player);
// 			static_cast<const DaylightDetectorBlock*>(Block::mDaylightDetector)->updateSignalStrength(player.getRegion(), pos);
// 		}
// 		else {
// 			player.getRegion().setBlockAndData(pos, FullBlock(Block::mDaylightDetectorInverted->mID, data), Block::UPDATE_CLIENTS, &player);
// 			static_cast<const DaylightDetectorBlock*>(Block::mDaylightDetectorInverted)->updateSignalStrength(player.getRegion(), pos);
// 		}
// 		system.lockGraph(false);
// 		return true;
// 	}

	return true;
}

bool DaylightDetectorBlock::shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const
{
	return true;
}

int DaylightDetectorBlock::getVariant(int data) const {
	return mIsInverted ? 1 : 0;
}

int DaylightDetectorBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return mDaylightDetector->mID;
}

ItemInstance DaylightDetectorBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(mDaylightDetector);
}

ItemInstance DaylightDetectorBlock::getSilkTouchItemInstance(DataID data) const {
	return ItemInstance(mDaylightDetector);
}
