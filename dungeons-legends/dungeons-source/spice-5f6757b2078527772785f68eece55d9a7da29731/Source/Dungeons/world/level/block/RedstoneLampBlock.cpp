/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/RedstoneLampBlock.h"
#include "world/item/ItemInstance.h"
#include "world/level/BlockSource.h"
// #include "world/level/BlockTickingQueue.h"
#include "world/level/Level.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/material/Material.h"
#include "world/redstone/Redstone.h"
// #include "world/redstone/circuit/CircuitSystem.h"
// #include "world/redstone/circuit/components/ConsumerComponent.h"

RedstoneLampBlock::RedstoneLampBlock(const std::string& nameId, int id, bool isLit)
	: Block(nameId, id, Material::getMaterial(MaterialType::BuildableGlass))
	, mIsLit(isLit) {

	if (mIsLit) {
		setLightEmission(1.0f);
	}
}

int RedstoneLampBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Block::mUnlitRedStoneLamp->mID;
}

ItemInstance RedstoneLampBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(Block::mUnlitRedStoneLamp);
}

ItemInstance RedstoneLampBlock::getSilkTouchItemInstance(DataID data) const {
	return ItemInstance(mUnlitRedStoneLamp);
}

bool RedstoneLampBlock::shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const {
	return false;
}

void RedstoneLampBlock::onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const {
// 	if (!region.getLevel().isClientSide()) {
// 		auto &circuit = region.getDimension().getCircuitSystem();
// 		//Minecraft will change, but we don't want our graph to change during this time
// 		circuit.lockGraph(true);
// 
// 		if (strength == 0) {
// 			region.getTickQueue(pos)->add(region, pos, mID, 4);
// 		} else {
// 			region.getTickQueue(pos)->remove(pos, mID);
// 			if (!mIsLit) {
// 				region.setBlock(pos, Block::mLitRedStoneLamp->mID, UPDATE_ALL);
// 			}
// 		}	
// 
// 		circuit.lockGraph(false);
// 	}
} 

void RedstoneLampBlock::onLoaded(BlockSource& region, const BlockPos& pos) const {
// 	if (!region.getLevel().isClientSide()) {
// 		auto &circuit = region.getDimension().getCircuitSystem();
// 		auto consumer = circuit.create<ConsumerComponent>(pos, &region, Facing::DOWN);
// 		if (consumer) {
// 			consumer->setPropagatePower(true);
// 		}
// 	}
}

void RedstoneLampBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	Block::onPlace(region, pos);
	onLoaded(region, pos);
}

void RedstoneLampBlock::tick(BlockSource& region, const BlockPos& pos, Random& random) const {
// 	if (!region.getLevel().isClientSide()) {
// 		auto &circuit = region.getDimension().getCircuitSystem();
// 		int strength = circuit.getStrength(pos);
// 
// 		if (mIsLit && strength == Redstone::SIGNAL_MIN) {
// 			circuit.lockGraph(true);
// 			region.setBlock(pos, Block::mUnlitRedStoneLamp->mID, UPDATE_ALL);
// 			circuit.lockGraph(false);
// 		}
// 	}
}
