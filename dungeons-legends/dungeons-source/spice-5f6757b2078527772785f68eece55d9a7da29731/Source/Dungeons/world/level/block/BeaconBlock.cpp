/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/

#include "Dungeons.h"

#include "BeaconBlock.h"
#include "world/entity/player/Player.h"
#include "world/level/BlockSource.h"
// #include "world/level/block/entity/BeaconBlockEntity.h"
#include "world/level/LevelConstants.h"
#include "world/level/Level.h"
#include "world/level/material/Material.h"

BeaconBlock::BeaconBlock(const std::string& nameId, int id)
	: EntityBlock(nameId, id, Material::getMaterial(MaterialType::Glass)) {

	//mBlockShape = BlockShape::BEACON;
	mBlockEntityType = BlockEntityType::Beacon;
	mRenderLayer = RENDERLAYER_ALPHATEST_SINGLE_SIDE;

	setLightBlock(Brightness(14));
}

BeaconBlock::~BeaconBlock() {

}

bool BeaconBlock::use(Player& player, const BlockPos& pos) const {

	if (player.getLevel().isClientSide()) {
		return true;
	}

// 	BlockEntity* blockEntity = player.getRegion().getBlockEntity(pos);
// 	if (blockEntity != nullptr && blockEntity->isType(BlockEntityType::Beacon)) {
// 		// TODO: award the interact_with_beacon stat
// 		player.openBeacon(static_cast<BeaconBlockEntity*>(blockEntity)->getContainerSize(), blockEntity->getPosition());
// 	}

	return true;
}
