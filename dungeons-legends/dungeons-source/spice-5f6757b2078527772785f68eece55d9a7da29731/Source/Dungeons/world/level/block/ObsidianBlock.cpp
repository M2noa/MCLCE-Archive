/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/ObsidianBlock.h"
#include "world/level/material/Material.h"
#include "world/level/BlockSource.h"
// #include "world/entity/ParticleType.h"
#include "world/level/Level.h"

ObsidianBlock::ObsidianBlock(const std::string& nameId, int id, bool isGlowing)
	: Block(nameId, id, Material::getMaterial(MaterialType::Stone))
	, mIsGlowing(isGlowing) { 
	mBrightnessGamma = 2.f;
	 
	mProperties = BlockProperty::Immovable | BlockProperty::CubeShaped;
}

int ObsidianBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 1;
}

int ObsidianBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Block::mObsidian->mID;
}

void ObsidianBlock::animateTick(BlockSource& region, const BlockPos& pos, Random& random) const {
	if (mIsGlowing) {
		poofParticles(region, pos);
	}
}

void ObsidianBlock::poofParticles(BlockSource& region, const BlockPos& pos) const {
// 	auto& level = region.getLevel();
// 	Random& random = level.getRandom();
// 	float r = 1 / 16.0f;
// 
// 	for (int i = 0; i < 6; i++) {
// 		Vec3 poofPos = random.nextVec3();
// 
// 		if (i == 0 && !region.isSolidBlockingBlock(pos.above())) {
// 			poofPos.y = pos.y + 1 + r;
// 		}
// 		if (i == 1 && !region.isSolidBlockingBlock(pos.below())) {
// 			poofPos.y = pos.y + 0 - r;
// 		}
// 		if (i == 2 && !region.isSolidBlockingBlock(pos.south())) {
// 			poofPos.z = pos.z + 1 + r;
// 		}
// 		if (i == 3 && !region.isSolidBlockingBlock(pos.north())) {
// 			poofPos.z = pos.z + 0 - r;
// 		}
// 		if (i == 4 && !region.isSolidBlockingBlock(pos.east())) {
// 			poofPos.x = pos.x + 1 + r;
// 		}
// 		if (i == 5 && !region.isSolidBlockingBlock(pos.west())) {
// 			poofPos.x = pos.x + 0 - r;
// 		}
// 		if (poofPos.x < pos.x || poofPos.x > pos.x + 1 || poofPos.y < 0 || poofPos.y > pos.y + 1 || poofPos.z < pos.z || poofPos.z > pos.z + 1) {
// 			level.addParticle(ParticleType::RedDust, poofPos, Vec3(0.0f));
// 		}
// 	}
}
