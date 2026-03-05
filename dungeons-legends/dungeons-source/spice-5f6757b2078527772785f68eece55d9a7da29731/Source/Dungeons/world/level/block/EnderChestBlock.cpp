#include "Dungeons.h"
#include "world/level/block/EnderChestBlock.h"
#include "world/level/material/Material.h"
#include "world/entity/player/Player.h"
// #include "world/inventory/EnderChestContainer.h"
#include "world/level/Level.h"
#include "world/Facing.h"

EnderChestBlock::EnderChestBlock(const std::string& nameId, int id)
	: ChestBlock(nameId, id, TYPE_ENDER, MaterialType::Stone) {
	mBlockEntityType = BlockEntityType::EnderChest;
	mProperties = BlockProperty::Immovable;

}

void EnderChestBlock::animateTick(BlockSource& region, const BlockPos& pos, Random& random) const {
	for (int i = 0; i < 30; i++) { // Java is 3, so increase this given that Java calls animateTick 10 times more frequently
		const int flipX = random.nextInt(2) * 2 - 1;
		const int flipZ = random.nextInt(2) * 2 - 1;
		const float dirx = random.nextFloat() * flipX;
		const float diry = (random.nextFloat() - 0.5f) * 0.125f;
		const float dirz = random.nextFloat() * flipZ;

		Vec3 position(pos.x + 0.5f + 0.25f * flipX, pos.y + random.nextFloat(), pos.z + 0.5f + 0.25f * flipZ);
		// TODO: rherlitz
// 		region.getLevel().addParticle(ParticleType::Portal, position, Vec3(dirx, diry, dirz));
	}
}

bool EnderChestBlock::canBeSilkTouched() const {
	return true;
}

int EnderChestBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Block::mObsidian->mID;
}

int EnderChestBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 8;
}

