/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/SpongeBlock.h"
#include "world/level/material/Material.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/Facing.h"                  // Directions for water search
// #include "world/entity/ParticleType.h"     // Particles for water droplets
#include "LevelEvent.h"

#define MAX_DEPTH_SPONGE 6
#define MAX_COUNT_SPONGE 4 * 4 * 4
#define DROPLET_OFFSET_SPONGE 0.05f

SpongeBlock::SpongeBlock(const std::string& nameId, int id, const Material& material)
	: Block(nameId, id, material) {
}

std::string SpongeBlock::buildDescriptionName(DataID data) const {
	std::string ret = mDescriptionId;

	// Creates the name of the this block based on the current type
	auto type = static_cast<SpongeType>(data);
	switch (type) {
	case SpongeType::Dry:
		ret += ".dry";
		break;
	case SpongeType::Wet:
		ret += ".wet";
		break;
	}

	return I18n::get(ret + ".name");
}

DataID SpongeBlock::getSpawnResourcesAuxValue(DataID data) const {
	// Return the current data, override the default of returning 0
	return data;
}

bool SpongeBlock::isValidAuxValue(int value) const {
	if (value == enum_cast(SpongeType::Dry) || value == enum_cast(SpongeType::Wet)) {
		return true;
	}
	return false;
}

void SpongeBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	_attemptAbsorbWater(region, pos);
}

void SpongeBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	_attemptAbsorbWater(region, pos);
}

void SpongeBlock::_attemptAbsorbWater(BlockSource& region, const BlockPos& pos) const {
	// For the sponge, the data is the sponge type
	DataID id = region.getData(pos);

	if (id == enum_cast(SpongeType::Dry) && _performAbsorbWater(region, pos)) {
		region.setBlockAndData(pos, mID, enum_cast(SpongeType::Wet), Block::UPDATE_ALL);

		// Bubbles rise
		for (int i = 0; i < 4; ++i) {
			float offx = region.getLevel().getRandom().nextFloat(0.0f, 1.0f);
			float offz = region.getLevel().getRandom().nextFloat(0.0f, 1.0f);
			float dirx = region.getLevel().getRandom().nextFloat(-0.25f, 0.25f);
			float dirz = region.getLevel().getRandom().nextFloat(-0.25f, 0.25f);

			// TODO: rherlitz
// 			region.getLevel().addParticle(
// 				ParticleType::Bubble,
// 				Vec3(pos) + Vec3(offx, 0.25f, offz),
// 				Vec3(dirx, 1.0f, dirz));
		}
// 		region.getLevel().broadcastDimensionEvent(region, LevelEvent::ParticlesDestroyBlock, pos, Block::mFlowingWater->getId()); // Effect used in Java Minecraft
	}
}

bool SpongeBlock::_performAbsorbWater(BlockSource& region, const BlockPos& startPos) const {
	// Breadth first search to destroy all surrounding water blocks
	std::queue< std::pair<BlockPos, unsigned> > waterBlocks;
	waterBlocks.push({ startPos, 0 });

	unsigned currentCount = 0;
	while (!waterBlocks.empty()) {
		auto currentBlock = waterBlocks.front();
		waterBlocks.pop();
		BlockPos currentPosition = currentBlock.first;
		unsigned currentDepth = currentBlock.second;

		// Check all directions to find if there is water
		for (int i = 0; i < Facing::MAX; ++i) {
			BlockPos tempPos = Facing::DIRECTION[i] + currentPosition;

			// If it is water, now it is air, the sponge sucked it up
			if (region.getMaterial(tempPos).isType(MaterialType::Water)) {
				region.setBlock(tempPos, Block::mAir->getId(), Block::UPDATE_ALL);
				++currentCount;
				
				if (currentDepth < MAX_DEPTH_SPONGE) {
					waterBlocks.push({ tempPos, currentDepth + 1 });
				}
			}
		}

		if (currentCount > MAX_COUNT_SPONGE) {
			break;
		}
	}

	// Sponge successfully soaked some water
	return currentCount > 0;
}

void SpongeBlock::animateTick(BlockSource& region, const BlockPos& pos, Random& random) const {
	// Sprout water droplets
	if (region.getData(pos) == enum_cast(SpongeType::Dry)) {
		return;
	}

	int randomFace = Facing::getRandomFace(random);
	
	// Early out since water can't fall up, or if something is in the way
	if (randomFace == Facing::UP || region.canProvideSupport(pos.relative(randomFace), Facing::OPPOSITE_FACING[randomFace], BlockSupportType::Edge)) {
		return;
	}

	Vec3 particlePosition = Vec3(pos);

	if (randomFace == Facing::DOWN) {
		particlePosition.y -= DROPLET_OFFSET_SPONGE;
		particlePosition.x += random.nextFloat();
		particlePosition.z += random.nextFloat();
	}
	else {
		particlePosition.y += random.nextFloat() * 0.8f;

		// Direction is along the x-axis
		if (Facing::NORMAL[randomFace].x != 0) {
			particlePosition.z += random.nextFloat();
			if (randomFace == Facing::EAST) {
				particlePosition.x += 1.0f + DROPLET_OFFSET_SPONGE;
			}
			else {
				particlePosition.x -= DROPLET_OFFSET_SPONGE;
			}
		}
		else {
			particlePosition.x += random.nextFloat();
			if (randomFace == Facing::SOUTH) {
				particlePosition.z += 1.0f + DROPLET_OFFSET_SPONGE;
			}
			else {
				particlePosition.z -= DROPLET_OFFSET_SPONGE;
			}
		}
	}

// 	region.getLevel().addParticle(ParticleType::DripWater, particlePosition, Vec3::ZERO);
}
