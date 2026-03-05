/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "world/level/BlockSource.h"
#include "lovika/io/ObjectGroupFile.h"

class LovikaBlockSource : public BlockSource {
public:

	LovikaBlockSource(Level& level, Dimension& dimension, io::ObjectGroup* objGroup);
	~LovikaBlockSource();

	DataID getData(const BlockPos& p) override;

	FullBlock getBlockAndData(const BlockPos& p) override;

	const Block& getBlock(const BlockPos& pos) override;

	const Material& getMaterial(const BlockPos& pos) override;

	BlockID getBlockID(int x, int y, int z) override;
	BlockID getBlockID(const BlockPos& pos) override;

	bool isEmptyBlock(int x, int y, int z) override;
	bool isEmptyBlock(const BlockPos& pos) override;

	bool isSolidBlockingBlock(int x, int i, int z) override;
	bool isSolidBlockingBlock(const BlockPos& p) override;

	bool setBlockAndData(int x, int y, int z, FullBlock block, int updateFlags) override;
	bool setBlockAndData(const BlockPos& pos, FullBlock block, int updateFlags, Entity* placer = nullptr) override;
	bool setBlockAndData(int x, int y, int z, BlockID id, DataID data, int updateFlags) override;
	bool setBlockAndData(const BlockPos& p, BlockID id, DataID data, int updateFlags, Entity* placer = nullptr) override;

// 	bool isTopSolidBlocking(const BlockPos& pos) override;
// 	bool isTopSolidBlocking(Block* block, DataID data) override;

	bool isConsideredSolidBlock(int x, int i, int z) override;
	bool isConsideredSolidBlock(const BlockPos& pos) override;

	bool setExtraData(const BlockPos& p, uint16_t value) override;
	uint16_t getExtraData(const BlockPos& p) override;

	int getGrassColor(const BlockPos& pos) override;

	Biome& getBiome(const BlockPos& pos) override;
	Biome* tryGetBiome(const BlockPos& pos) override;

private:
	io::ObjectGroup* mObjGroup;
};
