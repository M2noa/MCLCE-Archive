/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/DropperBlock.h"
//#include "world/level/block/entity/DropperBlockEntity.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/level/block/LevelEvent.h"
#include "world/level/material/Material.h"
#include "world/item/ItemInstance.h"
#include "world/item/Item.h"
#include "world/Facing.h"

DropperBlock::DropperBlock(const std::string& nameId, int id)
	: DispenserBlock(nameId, id) {
	mBlockEntityType = BlockEntityType::Dropper;
}

int DropperBlock::getAttachedFace(int data) {
	return Facing::DIRECTIONS[Block::mDropper->getBlockState(BlockState::FacingDirection).get<int>(data) % 6];
}

int DropperBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Block::mDropper->mID;
}

ItemInstance DropperBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(Block::mDropper, 1, Facing::SOUTH);
}

ItemInstance DropperBlock::getSilkTouchItemInstance(DataID data) const {
	return ItemInstance(Block::mDropper, 1, Facing::SOUTH);
}

void DropperBlock::dispenseFrom(BlockSource &region, const BlockPos& pos) const {
// 	DropperBlockEntity *dropper = static_cast<DropperBlockEntity*>(region.getBlockEntity(pos));
// 	if (dropper == nullptr) {
// 		return;
// 	}
// 
// 	int slot = dropper->getRandomSlot();
// 	if (slot < 0) {
// 		region.getLevel().broadcastLevelEvent(LevelEvent::SoundClickFail, Vec3(pos), 1200);
// 	} else {
// 		Container* container = dropper->getAttachedContainer(region);
// 		//	If we're attached to a container, we'll want to try to push items into that.
// 		if (container != nullptr) {
// 			dropper->pushOutItems(region, container);
// 		}
// 		//	Else we can dispense out the items normally.
// 		else {
// 			ItemInstance* itemInstance = dropper->getItem(slot);
// 			if (itemInstance != nullptr) {
// 				Vec3 dispensePosition = getDispensePosition(region, pos);
// 				FacingID face = getFacing(region.getData(pos));
// 
// 				ejectItem(region, dispensePosition, face, itemInstance, *dropper, slot);
// 
// 				region.getLevel().broadcastLevelEvent(LevelEvent::ParticlesShoot, dispensePosition, Facing::STEP_X[face] + 1 + (Facing::STEP_Z[face] + 1) * 3);
// 				region.getLevel().broadcastLevelEvent(LevelEvent::SoundClick, Vec3(pos), 1000);
// 
// 				dropper->setChanged();
// 				dropper->setContainerChanged(slot);
// 			}
// 		}
// 	}
}
