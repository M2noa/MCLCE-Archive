#include "Dungeons.h"

#include "LovikaBlockSource.h"
#include "world/level/block/Block.h"
#include "world/level/LevelConstants.h"
#include "world/level/BlockPos.h"
#include "world/level/block/StairBlock.h"
#include "world/level/block/SlabBlock.h"
#include "world/level/block/TopSnowBlock.h"
#include "world/level/FoliageColor.h"
#include "world/level/material/Material.h"
#include "world/level/biome/Biome.h"

LovikaBlockSource::LovikaBlockSource(Level& level, Dimension& dimension, io::ObjectGroup* objGroup)
	: BlockSource(level, dimension)
	, mObjGroup(objGroup)
{

}

LovikaBlockSource::~LovikaBlockSource()
{
}

DataID LovikaBlockSource::getData(const BlockPos& p) {
	if (p.y >= 0 && p.y < LEVEL_HEIGHT_DEPRECATED) {
		auto t = getBlockAndData(p);
		return t.data;
	}

	return 0;
}

BlockID LovikaBlockSource::getBlockID(int x, int y, int z) {
	return getBlockID(BlockPos(x, y, z));
}

BlockID LovikaBlockSource::getBlockID(const BlockPos& pos) {
	if (pos.y >= 0 && pos.y < LEVEL_HEIGHT_DEPRECATED) {
		for (auto& object : mObjGroup->objects) {
			if (object.bounds.containsX(pos.x) && object.bounds.containsY(pos.y) && object.bounds.containsZ(pos.z)) {
				BlockPos p(pos.x - object.bounds.minInclusive.x, pos.y - object.bounds.minInclusive.y, pos.z - object.bounds.minInclusive.z);
				FullBlock fullBlock = object.blocks->getBlock(p);
				return fullBlock.id;
			}
		}

		return BlockID::AIR;
	}
	else {
		return BlockID::AIR;
	}
}

const Block& LovikaBlockSource::getBlock(const BlockPos& pos) {
	return *Block::mBlocks[getBlockID(pos)];
}

const Material& LovikaBlockSource::getMaterial(const BlockPos& pos) {
	return getBlock(pos).getMaterial();
}

bool LovikaBlockSource::isSolidBlockingBlock(int x, int y, int z) {
	auto& block = getBlock({ x, y, z });
	return block.getMaterial().isSolidBlocking() && (block.hasProperty(BlockProperty::CubeShaped) || block.hasProperty(BlockProperty::SolidBlocking));
}

bool LovikaBlockSource::isSolidBlockingBlock(const BlockPos& p) {
	return isSolidBlockingBlock(p.x, p.y, p.z);
}

FullBlock LovikaBlockSource::getBlockAndData(const BlockPos& p) {
	if (p.y >= 0 && p.y < LEVEL_HEIGHT_DEPRECATED) {
		for (auto& object : mObjGroup->objects) {
			if (object.bounds.containsX(p.x) && object.bounds.containsY(p.y) && object.bounds.containsZ(p.z)) {
				BlockPos pos(p.x - object.bounds.minInclusive.x, p.y - object.bounds.minInclusive.y, p.z - object.bounds.minInclusive.z);
				return object.blocks->getBlock(pos);
			}
		}
	}
	return FullBlock();
}

bool LovikaBlockSource::isEmptyBlock(int x, int y, int z) {
	return isEmptyBlock(BlockPos(x, y, z));
}

bool LovikaBlockSource::isEmptyBlock(const BlockPos& pos) {
	return getBlockID(pos) == 0;
}

bool LovikaBlockSource::setBlockAndData(int x, int y, int z, FullBlock block, int updateFlags) {
	return setBlockAndData(BlockPos(x, y, z), block, updateFlags);
}

bool LovikaBlockSource::setBlockAndData(const BlockPos& pos, FullBlock block, int updateFlags, Entity* placer) {
	if (pos.y < 0 || pos.y >= LEVEL_HEIGHT_DEPRECATED) {
		return false;
	}

	FullBlock old = getBlockAndData(pos);
	if (old != block) {
		for (auto& object : mObjGroup->objects) {
			if (object.bounds.containsX(pos.x) && object.bounds.containsY(pos.y) && object.bounds.containsZ(pos.z)) {
				BlockPos p(pos.x - object.bounds.minInclusive.x, pos.y - object.bounds.minInclusive.y, pos.z - object.bounds.minInclusive.z);
				object.blocks->setBlock(p, block);
				return true;
			}
		}
	}
	return false;
}

bool LovikaBlockSource::setBlockAndData(const BlockPos& p, BlockID id, DataID data, int updateFlags, Entity* placer) {
	return setBlockAndData(p, FullBlock(id, data), updateFlags);
}

bool LovikaBlockSource::setBlockAndData(int x, int y, int z, BlockID id, DataID data, int updateFlags) {
	return setBlockAndData(x, y, z, FullBlock(id, data), updateFlags);
}

bool LovikaBlockSource::isConsideredSolidBlock(int x, int y, int z) {
	return getBlock({ x, y, z }).isSolid();
}

bool LovikaBlockSource::isConsideredSolidBlock(const BlockPos& pos) {
	return isConsideredSolidBlock(pos.x, pos.y, pos.z);
}

bool LovikaBlockSource::setExtraData(const BlockPos& p, uint16_t extraData) {
	if (p.y >= 0 && p.y < LEVEL_HEIGHT_DEPRECATED) {
		//LevelChunk* c = getChunk(p);
		//if (c) {
		//	if (c->setBlockExtraData((ChunkBlockPos)p, extraData)) {
		//		// Update change to clients
		//		mLevel.broadcastLevelEvent(LevelEvent::UpdateExtraData, Vec3(p.x + 0.5f, p.y + 0.5f, p.z + 0.5f), extraData);
		//	}
		//	return true;
		//}
	}
	return false;
}

uint16_t LovikaBlockSource::getExtraData(const BlockPos& p) {
	if (p.y >= 0 && p.y < LEVEL_HEIGHT_DEPRECATED) {
		//LevelChunk* c = getChunk(p);
		//if (c) {
		//	return c->getBlockExtraData((ChunkBlockPos)p);
		//}
	}
	return 0;
}

int LovikaBlockSource::getGrassColor(const BlockPos& pos) {
	return FoliageColor::getGrassColor(1.0f, 1.0f);
}

Biome* LovikaBlockSource::tryGetBiome(const BlockPos& pos) {
// 	BlockPos fixedPos(pos.x, 0, pos.z);
// 
// 	LevelChunk* c = getChunkAt(fixedPos);
// 	if (c == nullptr || fixedPos.y < 0 || fixedPos.y >= mMaxHeight) {
// 		return nullptr;
// 	}
// 	return &c->getBiome(ChunkBlockPos(fixedPos));
	return Biome::ocean.get();
}

Biome& LovikaBlockSource::getBiome(const BlockPos& pos) {
// 	Biome* found = tryGetBiome(pos);
// 	return found ? *found : *Biome::ocean; // default biome
	return *Biome::ocean;
}
