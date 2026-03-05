/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/MyceliumBlock.h"
#include "world/Facing.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/level/material/Material.h"
// #include "world/entity/ParticleType.h"

const Brightness MyceliumBlock::MIN_BRIGHTNESS(4);

MyceliumBlock::MyceliumBlock(const std::string& nameId, int id) :
	Block(nameId, id, Material::getMaterial(MaterialType::Dirt)) {

	setTicking(true);
}

void MyceliumBlock::tick(BlockSource& region, const BlockPos& pos, Random& random) const{
	if (region.getLevel().isClientSide()) {
		return;
	}

	if (region.getRawBrightness(pos.above()) < MIN_BRIGHTNESS && (region.getMaterial(pos.above()).isSolid() || !region.getMaterial(pos.above()).isLiquid())) {
		region.setBlockAndData(pos, Block::mDirt->mID, Block::UPDATE_ALL);
	} else {
		if(region.getRawBrightness(pos.above()) >= Brightness::MAX - 6) {
			for (int i = 0; i < 4; i++) {
				const int randZ = random.nextInt(3) - 1;
				const int randY = random.nextInt(5) - 3;
				const int randX = random.nextInt(3) - 1;
				BlockPos t = BlockPos(randX, randY, randZ) + pos;

				auto block = region.getBlockAndData(t);
				const Material& aboveMaterial = region.getMaterial(t.above());
				if (block.id == Block::mDirt->mID) {
					int mappedType = Block::mDirt->getBlockState(BlockState::MappedType).get<int>(block.data);
					if (mappedType == 0 && region.getRawBrightness(t.above()) >= MIN_BRIGHTNESS && !aboveMaterial.isSolid() && !aboveMaterial.isLiquid()) {
						region.setBlockAndData(t, mID, Block::UPDATE_ALL);
					}
				}
			}
		}
	}
}

void MyceliumBlock::animateTick(BlockSource& region, const BlockPos& pos, Random& random) const {
	Block::animateTick(region, pos, random);

// 	float posx = pos.x + random.nextFloat();
// 	float posz = pos.z + random.nextFloat();
// 	region.getLevel().addParticle(ParticleType::TownAura, Vec3(posx, pos.y + 1.1f, posz), Vec3(0.0f));
}

int MyceliumBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Block::mDirt->mID;
}
