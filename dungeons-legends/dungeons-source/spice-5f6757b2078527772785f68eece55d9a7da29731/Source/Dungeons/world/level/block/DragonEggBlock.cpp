/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/

#include "Dungeons.h"

#include "world/level/block/DragonEggBlock.h"
#include "world/level/material/Material.h"
#include "world/level/Level.h"
#include "world/entity/player/Player.h"
#include "world/level/BlockSource.h"

DragonEggBlock::DragonEggBlock(const std::string& nameId, int id) :
	HeavyBlock(nameId, id, Material::getMaterial(MaterialType::Stone)) {

	setSolid(false);
	setPushesOutItems(true);

	mProperties = BlockProperty::Heavy | BlockProperty::BreakOnPush;
}

Color DragonEggBlock::getDustColor(DataID data) const {
	return Color::NIL;
}

bool DragonEggBlock::attack(Player* player, const BlockPos& pos) const {
	_teleport(player->getRegion(), player->mRandom, pos);

	return true;
}

bool DragonEggBlock::use(Player& player, const BlockPos& pos) const {
	_teleport(player.getRegion(), player.mRandom, pos);
	
	return true;
}

void DragonEggBlock::_teleport(BlockSource& region, Random &random, const BlockPos& pos) const {
	// Don't run twice, only on the server
	if (region.getLevel().isClientSide()) {
		return;
	}

	int tests = 0;
	int airFound = 1000; // Max number of tests

	while (tests < airFound) {
		int offsetX = random.nextGaussianInt(16);
		int offsetY = Math::clamp(random.nextGaussianInt(8), 0, int(region.getMaxHeight()));
		int offsetZ = random.nextGaussianInt(16);
		BlockPos testPos = pos.offset(offsetX, offsetY, offsetZ);
		if (region.isEmptyBlock(testPos)) {
			int offX = pos.x - testPos.x;
			int offY = pos.y - testPos.y;
			int offZ = pos.z - testPos.z;

			// Store the offset between the egg's start position and its new position in a single int of space
			unsigned int data = 0;              // [0] [0] [0] [0] 4 bytes, each with its own meaning:
			data |= (std::abs(offX)) << 16;		// [0] [x] [0] [0] 2nd left byte is X offset
			data |= (std::abs(offY)) << 8;		// [0] [x] [y] [0] 2nd right byte is Y offset
			data |= (std::abs(offZ));			// [0] [x] [y] [z] rightmost byte is Z offset

			// Leftmost byte uses first 3 bits to store sign of each other byte, in order:
			// [0] [0] [0] [0] [0] [x] [y] [z] 
			data |= (offX < 0 ? 1 : 0) << 24;
			data |= (offY < 0 ? 1 : 0) << (24 + 1);
			data |= (offZ < 0 ? 1 : 0) << (24 + 2);

			// Broadcast an event to create particles, then move the block
// 			region.getLevel().broadcastLevelEvent(LevelEvent::ParticlesDragonEgg, pos, (int)data);
			region.setBlockAndData(testPos, Block::mDragonEgg->mID, Block::UPDATE_ALL);
			region.removeBlock(pos);

			// No more searching necessary
			tests = airFound;
		}
		else {
			++tests;
		}
	}
}
