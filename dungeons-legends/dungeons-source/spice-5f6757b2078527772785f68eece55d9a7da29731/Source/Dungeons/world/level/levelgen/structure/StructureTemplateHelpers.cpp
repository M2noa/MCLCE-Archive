/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
// 
// To: You
// From: Me
// Subject: Structure Block Coming >>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// 
//   It`s true. Let me start off by saying this is not one of those
// Internet scams. This is 100% legit and has been working for people
// all around the world. How do I know? I ported it. And now I'm
// working on something even better.
//
// Stay Online. >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// 
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

#include "Dungeons.h"

#include "world/level/levelgen/structure/StructureTemplateHelpers.h"
#include "world/level/levelgen/structure/StructureTemplate.h"
#include "world/level/Level.h"
#include "world/level/BlockSource.h"
#include "nbt/ListTag.h"
#include "Core/Debug/Log.h"

//////////////////////////////////////////////////////////////////////////
// RotationUtil

namespace RotationUtil {
	Rotation getRotated(Rotation original, Rotation rotation) {
		uint16_t rotated = ((uint16_t(original) * 90) + (uint16_t(rotation) * 90)) % 360;
		return Rotation(rotated / 90);
	}
}

//////////////////////////////////////////////////////////////////////////
// StructureBlockInfo

StructureBlockInfo::StructureBlockInfo(const BlockPos &pos, const CompoundTag &tag, const FullBlock &block)
	: mPos(pos), mBlock(block) {

	mTag = tag.clone();
}

StructureBlockInfo::StructureBlockInfo(const StructureBlockInfo& rhs) 
	: mPos(rhs.mPos), mBlock(rhs.mBlock) {

	mTag = rhs.mTag->clone();
}

//////////////////////////////////////////////////////////////////////////
// StructureEntityInfo

StructureEntityInfo::StructureEntityInfo(const Vec3 &pos, const CompoundTag &tag, const FullBlock &block)
	: mPos(pos), mBlock(block) {

	mTag = tag.clone();
}

StructureEntityInfo::StructureEntityInfo(const StructureEntityInfo& rhs) 
	: mPos(rhs.mPos), mBlock(rhs.mBlock) {

	mTag = rhs.mTag->clone();
};

//////////////////////////////////////////////////////////////////////////
// BlockPalette

BlockPalette::BlockPalette() : mMapper() {
}

void BlockPalette::clearMap() {
	mMapper.clear();
}

int BlockPalette::getSize()
{
	return mMapper.size();
}

void BlockPalette::addMapping(int id, FullBlock block) {
	mMapper[id] = block;
}

const FullBlock &BlockPalette::getBlock(int id) {
	const auto &found = mMapper.find(id);
	if (found == mMapper.end()) {
		return FullBlock::AIR;
	}

	return found->second;
}

int BlockPalette::getId(const FullBlock &block) {
	// Reverse map lookup, not super great...
	for (auto &found : mMapper) {
		if (found.second == block) {
			return found.first;
		}
	}

	// Was not found, add the mapping
	int newID = mMapper.size();
	mMapper[newID] = block;
	return newID;
}

//////////////////////////////////////////////////////////////////////////
// StructureSettings

StructureSettings::StructureSettings() :
	mIntegrity(1.0f),
	mSeed(0),
	mMirror(Mirror::NONE),
	mRotation(Rotation::NONE),
	mIgnoreEntities(false),
	mIgnoreStructureBlocks(true),
	mIgnoreBlock(nullptr),
	mChunkPos(ChunkPos::INVALID),
	mBoundingBox(BoundingBox::getUnknownBox())
{}

StructureSettings::StructureSettings(Mirror mirror, Rotation rotation, bool ignoreEntities, const Block* ignoreBlock, const BoundingBox& boundingBox)
	: StructureSettings() {
	mMirror = mirror;
	mRotation = rotation;
	mIgnoreEntities = ignoreEntities;
	mIgnoreBlock = ignoreBlock;
	mBoundingBox = boundingBox;
}

void StructureSettings::retrieveRandom(Random &random) const {
	if (mSeed == 0) {
		// Reset the random to default
		random = Random();
	}

	random.setSeed(mSeed);
}

void StructureSettings::setMirror(Mirror mirror) {
	mMirror = mirror;
}

void StructureSettings::setRotation(Rotation rotation) {
	mRotation = rotation;
}

void StructureSettings::setIgnoreEntities(bool ignoreEntities) {
	mIgnoreEntities = ignoreEntities;
}

void StructureSettings::setIgnoreBlock(const Block* ignoreBlock) {
	mIgnoreBlock = ignoreBlock;
}

void StructureSettings::setSeed(RandomSeed seed) {
	mSeed = seed;
}

void StructureSettings::setIntegrity(float integrity) {
	mIntegrity = integrity;
}

const Block *StructureSettings::getIgnoreBlock() const {
	return mIgnoreBlock;
}

void StructureSettings::setChunkPos(const ChunkPos& chunkPos) {
	mChunkPos = chunkPos;
}

void StructureSettings::setBoundingBox(const BoundingBox& boundingBox) {
	mBoundingBox = boundingBox;
}

const Mirror& StructureSettings::getMirror() const {
	return mMirror;
}

void StructureSettings::ignoreStructureBlocks(bool ignore) {
	mIgnoreStructureBlocks = ignore;
}

const Rotation& StructureSettings::getRotation() const {
	return mRotation;
}

const ChunkPos& StructureSettings::getChunkPos() const {
	return mChunkPos;
}

bool StructureSettings::isIgnoreEntities() {
	return mIgnoreEntities;
}

const Block* StructureSettings::isIgnoreBlock() {
	return mIgnoreBlock;
}

const BoundingBox& StructureSettings::getBoundingBox() {
	if (!mBoundingBox.isValid() && mChunkPos != ChunkPos::INVALID) {
		updateBoundingBoxFromChunkPos();
	}
	return mBoundingBox;
}

bool StructureSettings::isIgnoreStructureBlocks() const {
	return mIgnoreStructureBlocks;
}

void StructureSettings::updateBoundingBoxFromChunkPos() {
	mBoundingBox = _calculateBoundingBox(mChunkPos);
}

RandomSeed StructureSettings::getSeed() const {
	return mSeed;
}

float StructureSettings::getIntegrity() const {
	return mIntegrity;
}

BoundingBox StructureSettings::_calculateBoundingBox(const ChunkPos& chunk) {
	if (chunk == ChunkPos::INVALID) {
		return BoundingBox::getUnknownBox();
	}
	int chunkSize = StructureTemplate::CHUNK_SIZE;
	int chunkX = chunk.x * chunkSize;
	int chunkZ = chunk.z * chunkSize;
	return BoundingBox(chunkX, 0, chunkZ, chunkX + chunkSize - 1, Level::CLOUD_HEIGHT - 1, chunkZ + chunkSize - 1);
}

const float StructureSettings::INTEGRITY_MAX = 1.0f;
const float StructureSettings::INTEGRITY_MIN = 0.0f;
const int StructureSettings::MAX_STRUCTURE_SIZE = 32;

//////////////////////////////////////////////////////////////////////////
// BlockPlacementProcessor

BlockPlacementProcessor::BlockPlacementProcessor(const StructureSettings &settings) {
	mChance = settings.getIntegrity();
	settings.retrieveRandom(mRandom);
}

bool BlockPlacementProcessor::canPlace() {
	return mChance >= 1.0f || mRandom.nextFloat() <= mChance;
}
