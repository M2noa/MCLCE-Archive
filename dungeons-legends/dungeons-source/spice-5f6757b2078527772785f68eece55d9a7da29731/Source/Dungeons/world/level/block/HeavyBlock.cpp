/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/HeavyBlock.h"
#include "world/level/material/Material.h"
#include "world/level/Level.h"
// #include "world/entity/item/FallingBlock.h"
#include "world/level/BlockSource.h"
// #include "world/level/BlockTickingQueue.h"
// #include "world/entity/ParticleType.h"
#include "Core/Math/Color.h"
// #include "world/entity/EntityFactory.h"

HeavyBlock::HeavyBlock(const std::string& nameId, int id, const Material& material) :
	Block(nameId, id, material) {
	mCanSlide = true;
	mHeavy = true;
}

void HeavyBlock::_scheduleCheck(BlockSource& region, const BlockPos& pos, const Block* oldBlock, int delay) const {
// 	if (delay < 0 ) {
// 		delay = getTickDelay() + region.getLevel().getRandom().nextInt(6);
// 	}
// 
// 	auto& queue = *region.getTickQueue(pos);
// 	if (!queue.isInstaticking()) {	//do not tick neighbors when instaticking
// 		queue.add(region, pos, oldBlock->mID, delay );
// 	}
}

void HeavyBlock::_scheduleCheckIfSliding(BlockSource& region, const BlockPos& pos, const Block* oldBlock) const {
	auto& t = region.getBlock(pos);
	if (t.canSlide()) {
		_scheduleCheck(region, pos, &t);
	}
}

BlockPos HeavyBlock::_findBottomSlidingBlock(BlockSource& region, const BlockPos& start) const {
	BlockPos pos = start;
	for (; pos.y > 0; --pos.y) {

		const Block& block = region.getBlock(pos);

		if (!block.canSlide()) {
			break;
		}
	}

	return pos.above();
}

void HeavyBlock::_tickBlocksAround2D(BlockSource& region, const BlockPos& pos, const Block* oldBlock) const {
	_scheduleCheckIfSliding(region, pos.west(), oldBlock);
	_scheduleCheckIfSliding(region, pos.east(), oldBlock);
	_scheduleCheckIfSliding(region, pos.north(), oldBlock);
	_scheduleCheckIfSliding(region, pos.south(), oldBlock);
}

void HeavyBlock::startFalling(BlockSource& region, const BlockPos& pos, const Block* oldBlock, bool creative) const {
// 	Vec3 fpos = Vec3(pos) + 0.5f;
// 
// 	Unique<Entity> result = EntityFactory::createSpawnedEntity(EntityType::FallingBlock, nullptr, fpos, Vec2::ZERO);
// 	if (result) {
// 		// Move block data to the entity
// 		static_cast<FallingBlock*>(result.get())->setBlock(FullBlock(oldBlock->mID, region.getData(pos)), creative);
// 		region.setBlockAndData(pos, FullBlock::AIR, Block::UPDATE_ALL);
// 
// 		_tickBlocksAround2D(region, pos.above(), oldBlock);
// 		_tickBlocksAround2D(region, pos.below(), oldBlock);
// 
// 		region.getLevel().addEntity(region, std::move(result));
// 	}
}

bool HeavyBlock::isFree(BlockSource& region, const BlockPos& pos) const {
	const Block* t = Block::mBlocks[region.getBlockID(pos)];

	if (t == nullptr || t->isType(Block::mAir)) {
		return true;
	}

	if (t == (const Block*)Block::mFire) {
		return true;
	}

	const Material& material = t->getMaterial();
	if (material.isType(MaterialType::Air)) {
		return true;
	}

	if (material.isType(MaterialType::Water)) {
		return true;
	}

	if (material.isType(MaterialType::Lava)) {
		return true;
	} else{
		return false;
	}
}

void HeavyBlock::checkSlide(BlockSource& region, const BlockPos& pos) const {
	BlockPos bottom = _findBottomSlidingBlock(region, pos);
	if (bottom.y <= 0) {
		return;
	}

	if (isFree(region, bottom.below())) {
		bool isCreative = region.getLevel().getLevelData().getGameType() == GameType::Creative;
		startFalling(region, bottom, &region.getBlock(bottom), isCreative);
	}
}

int HeavyBlock::getTickDelay() const {
	return 2;
}

void HeavyBlock::tick(BlockSource& region, const BlockPos& pos, Random& random) const {
	if (!region.getLevel().isClientSide()) {
		checkSlide(region, pos);
	}
}

void HeavyBlock::triggerFallCheck(BlockSource& region, const BlockPos& pos) const {
	_scheduleCheck(region, pos, this);
}

void HeavyBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	_scheduleCheck(region, pos, this);

	// Check if any entities are within
// 	Entity::checkEntityOnewayCollision(region, pos);
}

void HeavyBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {

	int delay = (neighborPos.y < pos.y && pos.x == neighborPos.x && neighborPos.z == pos.z) ? 1 : -1;
	_scheduleCheck(region, pos, this, delay );//fall instantly when the block changed is the one under us
}

void HeavyBlock::animateTick(BlockSource& region, const BlockPos& pos, Random& random) const {
	if (random.nextInt(4) == 0) {
		BlockPos below = pos.below();

		if (isFree(region, below)) {
			auto data = region.getData(pos);
			float particlex = pos.x + random.nextFloat();
			float particley = pos.y + 0.3f;
			float particlez = pos.z + random.nextFloat();

// 			region.getLevel().addParticle(ParticleType::FallingDust, Vec3(particlex, particley, particlez), Vec3::ZERO, getDustColor(data).toARGB());
		}
	}
}

void HeavyBlock::onLand(BlockSource& region, const BlockPos& pos) const {
}

bool HeavyBlock::falling() const {
	return false;
}
