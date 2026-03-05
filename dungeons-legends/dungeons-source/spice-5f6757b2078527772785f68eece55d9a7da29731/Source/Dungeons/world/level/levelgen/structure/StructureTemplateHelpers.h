/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/

#pragma once

#include "world/level/block/Block.h"
#include "nbt/CompoundTag.h"
#include "util/Random.h"
#include "world/level/ChunkPos.h"
#include "world/level/levelgen/structure/BoundingBox.h"

// Enumerations

enum class Mirror {
	NONE,
	LEFT_RIGHT,
	FRONT_BACK,
	TOTAL
};

enum class Rotation : uint16_t {
	NONE = 0,
	ROTATE_90,
	ROTATE_180,
	ROTATE_270,

	TOTAL
};

namespace RotationUtil {
	Rotation getRotated(Rotation original, Rotation rotation);
}

// Contains data needed for stored blocks
class StructureBlockInfo {
public:
	StructureBlockInfo(const BlockPos &pos, const CompoundTag &tag, const FullBlock &block);
	StructureBlockInfo(const StructureBlockInfo& rhs);

	BlockPos mPos;
	Unique<CompoundTag> mTag;
	const FullBlock &mBlock;
};

// Contains data needed for stored entities
class StructureEntityInfo {
public:
	StructureEntityInfo(const Vec3 &pos, const CompoundTag &tag, const FullBlock &block);
	StructureEntityInfo(const StructureEntityInfo& rhs);

	Vec3 mPos;
	Unique<CompoundTag> mTag;
	const FullBlock &mBlock;
};

typedef std::unordered_map<int, FullBlock> FullBlockMap;

// This palette maps the structure's internal IDs to the matching block types
class BlockPalette {
public:
	BlockPalette();

	void addMapping(int id, FullBlock block);
	const FullBlock &getBlock(int id);
	int getId(const FullBlock &block);

	// Empties the map
	void clearMap();

	// Returns the number of mappings
	int getSize();

private:
	FullBlockMap mMapper;
};

// Contains all options that a structure can be spawned with
class StructureSettings {
public:
	StructureSettings();
	StructureSettings(Mirror mirror, Rotation rotation, bool ignoreEntities, const Block* ignoreBlock, const BoundingBox& boundingBox);
	StructureSettings& operator=(const StructureSettings &rhs) = default;

	void setMirror(Mirror mirror);
	void setRotation(Rotation rotation);
	void setIgnoreEntities(bool ignoreEntities);
	void setIgnoreBlock(const Block* ignoreBlock);
	void setSeed(RandomSeed seed);
	void setIntegrity(float integrity);
	const Block *getIgnoreBlock() const;
	void setChunkPos(const ChunkPos& chunkPos);
	void setBoundingBox(const BoundingBox& boundingBox);
	void ignoreStructureBlocks(bool ignore);
	const Mirror& getMirror() const;
	const Rotation& getRotation() const;
	const ChunkPos& getChunkPos() const;
	bool isIgnoreEntities();
	const Block* isIgnoreBlock();
	const BoundingBox& getBoundingBox();
	bool isIgnoreStructureBlocks() const;
	void updateBoundingBoxFromChunkPos();
	void retrieveRandom(Random &random) const;
	RandomSeed getSeed() const;
	float getIntegrity() const;

	static const float INTEGRITY_MAX;
	static const float INTEGRITY_MIN;
	static const int MAX_STRUCTURE_SIZE;

private:
	BoundingBox _calculateBoundingBox(const ChunkPos& chunk);
	
	float mIntegrity;
	RandomSeed mSeed;

	Mirror mMirror;
	Rotation mRotation;
	bool mIgnoreEntities;
	bool mIgnoreStructureBlocks;
	const Block* mIgnoreBlock;
	ChunkPos mChunkPos;
	BoundingBox mBoundingBox;
};

// Determines if a block should be placed
class BlockPlacementProcessor {
public:
	BlockPlacementProcessor(const StructureSettings &settings);

	// If the integrity is less than 1, this uses the random to determine placement
	bool canPlace();

private:
	float mChance;
	Random mRandom;
};
