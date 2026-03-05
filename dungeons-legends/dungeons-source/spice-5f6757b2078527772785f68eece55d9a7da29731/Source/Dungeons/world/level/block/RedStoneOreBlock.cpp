/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/RedStoneOreBlock.h"
#include "world/level/material/Material.h"
#include "world/level/Level.h"
#include "world/level/BlockSource.h"
#include "world/entity/player/Player.h"
#include "world/item/Item.h"
// #include "world/entity/ParticleType.h"

RedStoneOreBlock::RedStoneOreBlock(const std::string& nameId, int id, bool lit)
	: Block(nameId, id, Material::getMaterial(MaterialType::Stone)) {
	if (lit) {
		setTicking(true);
	}

	mLit = lit;
}

bool RedStoneOreBlock::attack(Player* player, const BlockPos& pos) const {
	_interact(player->getRegion(), pos);
	return Block::attack(player, pos);
}

void RedStoneOreBlock::onStepOn(Entity& entity, const BlockPos& pos) const {
	_interact(entity.getRegion(), pos);
	Block::onStepOn(entity, pos);
}

bool RedStoneOreBlock::use(Player& player, const BlockPos& pos) const {
	_interact(player.getRegion(), pos);
	return Block::use(player, pos);
}

int RedStoneOreBlock::getTickDelay() const {
	return 30;
}

void RedStoneOreBlock::tick(BlockSource& region, const BlockPos& pos, Random& random) const {
	if (mID == Block::mLitRedStoneOre->mID) {
		region.setBlock(pos.x, pos.y, pos.z, Block::mRedStoneOre->mID, Block::UPDATE_CLIENTS);
	}
}

void RedStoneOreBlock::animateTick(BlockSource& region, const BlockPos& pos, Random& random) const {
	if (mLit) {
		_poofParticles(region, pos);
	}
}

int RedStoneOreBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Item::mRedStone->getId();
}

int RedStoneOreBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 4 + random.nextInt(2 + bonusLootLevel);
}

int RedStoneOreBlock::getExperienceDrop(Random& random) const {
	return random.nextInt(1, 5);
}

ItemInstance RedStoneOreBlock::getSilkTouchItemInstance(DataID data) const {
	return ItemInstance(Block::mRedStoneOre);
}

void RedStoneOreBlock::_poofParticles(BlockSource& region, const BlockPos& pos) const {
	auto& level = region.getLevel();
	Random& random = level.getRandom();
	float r = 1 / 16.0f;

	for (int i = 0; i < 6; i++) {
		Vec3 poofPos = Vec3(pos) + random.nextVec3();

		if (i == 0 && !region.isSolidBlockingBlock(pos.above())) {
			poofPos.y = pos.y + 1 + r;
		}
		if (i == 1 && !region.isSolidBlockingBlock(pos.below())) {
			poofPos.y = pos.y + 0 - r;
		}
		if (i == 2 && !region.isSolidBlockingBlock(pos.south())) {
			poofPos.z = pos.z + 1 + r;
		}
		if (i == 3 && !region.isSolidBlockingBlock(pos.north())) {
			poofPos.z = pos.z + 0 - r;
		}
		if (i == 4 && !region.isSolidBlockingBlock(pos.east())) {
			poofPos.x = pos.x + 1 + r;
		}
		if (i == 5 && !region.isSolidBlockingBlock(pos.west())) {
			poofPos.x = pos.x + 0 - r;
		}
		if (poofPos.x < pos.x || poofPos.x > pos.x + 1 || poofPos.y < 0 || poofPos.y > pos.y + 1 || poofPos.z < pos.z || poofPos.z > pos.z + 1) {
			// TODO: rherlitz
// 			level.addParticle(ParticleType::RedDust, poofPos, Vec3(0.0f));
		}
	}
}

void RedStoneOreBlock::_interact(BlockSource& region, const BlockPos& pos) const {
	_poofParticles(region, pos);
	if (mID == Block::mRedStoneOre->mID) {
		region.setBlock(pos, Block::mLitRedStoneOre->mID, Block::UPDATE_CLIENTS);
	}
}
