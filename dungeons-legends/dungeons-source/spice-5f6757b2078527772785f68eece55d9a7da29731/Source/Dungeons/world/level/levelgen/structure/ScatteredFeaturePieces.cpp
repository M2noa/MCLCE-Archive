#include "Dungeons.h"

#include "ScatteredFeaturePieces.h"
#include "world/level/BlockSource.h"
#include "world/level/BlockPos.h"
#include "world/level/block/Block.h"
#include "world/level/block/CauldronBlock.h"
#include "world/level/block/ColoredBlock.h"
#include "world/level/block/WoodBlock.h"
//#include "world/level/block/entity/CauldronBlockEntity.h"
//#include "world/level/block/entity/FlowerPotBlockEntity.h"
#include "world/level/Level.h"
#include "world/level/dimension/Dimension.h"
// #include "world/level/Spawner.h"
// #include "world/entity/monster/Witch.h"
#include "world/level/block/SandStoneBlock.h"
#include "world/level/block/StoneSlabBlock.h"
// #include "world/item/DyePowderItem.h"
// #include "world/item/alchemy/Potion.h"
#include "world/level/block/TripWireBlock.h"
#include "world/level/block/TripWireHookBlock.h"
#include "world/level/block/VineBlock.h"
#include "world/level/block/PistonBlock.h"
#include "world/level/levelgen/structure/StructureManager.h"
// #include "world/level/block/entity/StructureBlockEntity.h"
// #include "world/level/block/entity/ChestBlockEntity.h"
#include "world/level/storage/LevelStorage.h"
// #include "platform/AppPlatform.h"
#include "util/LootTableUtil.h"

const std::string Igloo::STRUCTURE_LOCATION_IGLOO_TRAPDOOR = "igloo/igloo_top_trapdoor";
const std::string Igloo::STRUCTURE_LOCATION_IGLOO_NO_TRAPDOOR = "igloo/igloo_top_no_trapdoor";
const std::string Igloo::STRUCTURE_LOCATION_LADDER = "igloo/igloo_middle";
const std::string Igloo::STRUCTURE_LOCATION_LABORATORY = "igloo/igloo_bottom";

ScatteredFeaturePiece::ScatteredFeaturePiece(int west, int floor, int north, int width, int height, int depth)
	: StructurePiece(0) {

	mWidth = width;
	mHeight = height;
	mDepth = depth;

	orientation = 0;// Direction::Plane.HORIZONTAL.getRandomFace(random);

	switch (orientation) {
	case Direction::NORTH:
	case Direction::SOUTH:
		boundingBox = BoundingBox(west, floor, north, west + width - 1, floor + height - 1, north + depth - 1);
		break;
	default:
		boundingBox = BoundingBox(west, floor, north, west + depth - 1, floor + height - 1, north + width - 1);
		break;
	}
}

void ScatteredFeaturePiece::addAdditionalSaveData(CompoundTag& tag) const {
	tag.putInt("Width", mWidth);
	tag.putInt("Height", mHeight);
	tag.putInt("Depth", mDepth);
	tag.putInt("HPos", mHeightPosition);
}

void ScatteredFeaturePiece::readAdditionalSaveData(const CompoundTag& tag) {
	mWidth = tag.getInt("Width");
	mHeight = tag.getInt("Height");
	mDepth = tag.getInt("Depth");
	mHeightPosition = tag.getInt("HPos");
}

bool ScatteredFeaturePiece::updateAverageGroundHeight(BlockSource *region, const BoundingBox& chunkBB, int offset) {
	if (mHeightPosition >= 0) {
		return true;
	}

	int total = 0;
	int count = 0;
	BlockPos pos;
	for (int z = boundingBox.z0; z <= boundingBox.z1; z++) {
		for (int x = boundingBox.x0; x <= boundingBox.x1; x++) {
			pos.set(x, region->getMaxHeight() - 1, z);
			if (chunkBB.isInside(pos)) {
				total += Math::max(region->getAboveTopSolidBlock(pos, true), region->getDimension().getSpawnYPosition());
				count++;
			}
		}
	}

	if (count == 0) {
		return false;
	}
	mHeightPosition = total / count;
	boundingBox.move(0, mHeightPosition - boundingBox.y0 + offset, 0);
	return true;
}

DesertPyramidPiece::DesertPyramidPiece(int west, int north)
	: ScatteredFeaturePiece(west, 64, north, 21, 15, 21) {
	mHasPlacedChest[0] = mHasPlacedChest[1] = mHasPlacedChest[2] = mHasPlacedChest[3] = false;
}

void DesertPyramidPiece::addAdditionalSaveData(CompoundTag& tag) const {
	ScatteredFeaturePiece::addAdditionalSaveData(tag);

	tag.putBoolean("hasPlacedChest0", mHasPlacedChest[0]);
	tag.putBoolean("hasPlacedChest1", mHasPlacedChest[1]);
	tag.putBoolean("hasPlacedChest2", mHasPlacedChest[2]);
	tag.putBoolean("hasPlacedChest3", mHasPlacedChest[3]);
}

void DesertPyramidPiece::readAdditionalSaveData(const CompoundTag& tag) {
	ScatteredFeaturePiece::readAdditionalSaveData(tag);
	mHasPlacedChest[0] = tag.getBoolean("hasPlacedChest0");
	mHasPlacedChest[1] = tag.getBoolean("hasPlacedChest1");
	mHasPlacedChest[2] = tag.getBoolean("hasPlacedChest2");
	mHasPlacedChest[3] = tag.getBoolean("hasPlacedChest3");
}

bool DesertPyramidPiece::postProcess(BlockSource *region, Random& random, const BoundingBox& chunkBB) {
	// pyramid
	generateBox(region, chunkBB, 0, -4, 0, mWidth - 1, 0, mDepth - 1, Block::mSandStone->mID, Block::mSandStone->mID, false);
	for (int pos = 1; pos <= 9; pos++) {
		generateBox(region, chunkBB, pos, pos, pos, mWidth - 1 - pos, pos, mDepth - 1 - pos, Block::mSandStone->mID, Block::mSandStone->mID, false);
		generateBox(region, chunkBB, pos + 1, pos, pos + 1, mWidth - 2 - pos, pos, mDepth - 2 - pos, Block::mAir->mID, Block::mAir->mID, false);
	}
	for (int x = 0; x < mWidth; x++) {
		for (int z = 0; z < mDepth; z++) {
			int startY = -5;
			fillColumnDown(region, Block::mSandStone->mID, x, startY, z, chunkBB);
		}
	}

	int stairsNorth = getOrientationData(Block::mSandstoneStairs, 3);
	int stairsSouth = getOrientationData(Block::mSandstoneStairs, 2);
	int stairsEast = getOrientationData(Block::mSandstoneStairs, 0);
	int stairsWest = getOrientationData(Block::mSandstoneStairs, 1);
// 	int baseDecoColor = ColoredBlock::getBlockDataForItemAuxValue(DyePowderItem::ORANGE);
// 	int blue = ColoredBlock::getBlockDataForItemAuxValue(DyePowderItem::BLUE);
	// TODO: rherlitz
	int baseDecoColor = 0;
	int blue = 0;

	// towers
	generateBox(region, chunkBB, 0, 0, 0, 4, 9, 4, Block::mSandStone->mID, Block::mAir->mID, false);
	generateBox(region, chunkBB, 1, 10, 1, 3, 10, 3, Block::mSandStone->mID, Block::mSandStone->mID, false);
	placeBlock(region, Block::mSandstoneStairs->mID, stairsNorth, 2, 10, 0, chunkBB);
	placeBlock(region, Block::mSandstoneStairs->mID, stairsSouth, 2, 10, 4, chunkBB);
	placeBlock(region, Block::mSandstoneStairs->mID, stairsEast, 0, 10, 2, chunkBB);
	placeBlock(region, Block::mSandstoneStairs->mID, stairsWest, 4, 10, 2, chunkBB);
	generateBox(region, chunkBB, mWidth - 5, 0, 0, mWidth - 1, 9, 4, Block::mSandStone->mID, Block::mAir->mID, false);
	generateBox(region, chunkBB, mWidth - 4, 10, 1, mWidth - 2, 10, 3, Block::mSandStone->mID, Block::mSandStone->mID, false);
	placeBlock(region, Block::mSandstoneStairs->mID, stairsNorth, mWidth - 3, 10, 0, chunkBB);
	placeBlock(region, Block::mSandstoneStairs->mID, stairsSouth, mWidth - 3, 10, 4, chunkBB);
	placeBlock(region, Block::mSandstoneStairs->mID, stairsEast, mWidth - 5, 10, 2, chunkBB);
	placeBlock(region, Block::mSandstoneStairs->mID, stairsWest, mWidth - 1, 10, 2, chunkBB);

	// entrance
	generateBox(region, chunkBB, 8, 0, 0, 12, 4, 4, Block::mSandStone->mID, Block::mAir->mID, false);
	generateBox(region, chunkBB, 9, 1, 0, 11, 3, 4, Block::mAir->mID, Block::mAir->mID, false);
	placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, 9, 1, 1, chunkBB);
	placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, 9, 2, 1, chunkBB);
	placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, 9, 3, 1, chunkBB);
	placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, 10, 3, 1, chunkBB);
	placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, 11, 3, 1, chunkBB);
	placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, 11, 2, 1, chunkBB);
	placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, 11, 1, 1, chunkBB);

	// tower pathways
	generateBox(region, chunkBB, 4, 1, 1, 8, 3, 3, Block::mSandStone->mID, Block::mAir->mID, false);
	generateBox(region, chunkBB, 4, 1, 2, 8, 2, 2, Block::mAir->mID, Block::mAir->mID, false);
	generateBox(region, chunkBB, 12, 1, 1, 16, 3, 3, Block::mSandStone->mID, Block::mAir->mID, false);
	generateBox(region, chunkBB, 12, 1, 2, 16, 2, 2, Block::mAir->mID, Block::mAir->mID, false);

	// hall floor and pillars
	generateBox(region, chunkBB, 5, 4, 5, mWidth - 6, 4, mDepth - 6, Block::mSandStone->mID, Block::mSandStone->mID, false);
	generateBox(region, chunkBB, 9, 4, 9, 11, 4, 11, Block::mAir->mID, Block::mAir->mID, false);
	generateBox(region, chunkBB, 8, 1, 8, 8, 3, 8, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, Block::mSandStone->mID, 0, false);
	generateBox(region, chunkBB, 12, 1, 8, 12, 3, 8, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, Block::mSandStone->mID, 0, false);
	generateBox(region, chunkBB, 8, 1, 12, 8, 3, 12, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, Block::mSandStone->mID, 0, false);
	generateBox(region, chunkBB, 12, 1, 12, 12, 3, 12, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, Block::mSandStone->mID, 0, false);

	// catwalks
	generateBox(region, chunkBB, 1, 1, 5, 4, 4, 11, Block::mSandStone->mID, Block::mSandStone->mID, false);
	generateBox(region, chunkBB, mWidth - 5, 1, 5, mWidth - 2, 4, 11, Block::mSandStone->mID, Block::mSandStone->mID, false);
	generateBox(region, chunkBB, 6, 7, 9, 6, 7, 11, Block::mSandStone->mID, Block::mSandStone->mID, false);
	generateBox(region, chunkBB, mWidth - 7, 7, 9, mWidth - 7, 7, 11, Block::mSandStone->mID, Block::mSandStone->mID, false);
	generateBox(region, chunkBB, 5, 5, 9, 5, 7, 11, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, false);
	generateBox(region, chunkBB, mWidth - 6, 5, 9, mWidth - 6, 7, 11, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, false);
	placeBlock(region, Block::mAir->mID, 5, 5, 10, chunkBB);
	placeBlock(region, Block::mAir->mID, 5, 6, 10, chunkBB);
	placeBlock(region, Block::mAir->mID, 6, 6, 10, chunkBB);
	placeBlock(region, Block::mAir->mID, mWidth - 6, 5, 10, chunkBB);
	placeBlock(region, Block::mAir->mID, mWidth - 6, 6, 10, chunkBB);
	placeBlock(region, Block::mAir->mID, mWidth - 7, 6, 10, chunkBB);

	// tower stairs
	generateBox(region, chunkBB, 2, 4, 4, 2, 6, 4, Block::mAir->mID, Block::mAir->mID, false);
	generateBox(region, chunkBB, mWidth - 3, 4, 4, mWidth - 3, 6, 4, Block::mAir->mID, Block::mAir->mID, false);
	placeBlock(region, Block::mSandstoneStairs->mID, stairsNorth, 2, 4, 5, chunkBB);
	placeBlock(region, Block::mSandstoneStairs->mID, stairsNorth, 2, 3, 4, chunkBB);
	placeBlock(region, Block::mSandstoneStairs->mID, stairsNorth, mWidth - 3, 4, 5, chunkBB);
	placeBlock(region, Block::mSandstoneStairs->mID, stairsNorth, mWidth - 3, 3, 4, chunkBB);
	generateBox(region, chunkBB, 1, 1, 3, 2, 2, 3, Block::mSandStone->mID, Block::mSandStone->mID, false);
	generateBox(region, chunkBB, mWidth - 3, 1, 3, mWidth - 2, 2, 3, Block::mSandStone->mID, Block::mSandStone->mID, false);
	placeBlock(region, Block::mSandstoneStairs->mID, 0, 1, 1, 2, chunkBB);
	placeBlock(region, Block::mSandstoneStairs->mID, 0, mWidth - 2, 1, 2, chunkBB);
	placeBlock(region, Block::mStoneSlab->mID, (DataID)StoneSlabBlock::StoneSlabType::Sandstone, 1, 2, 2, chunkBB);
	placeBlock(region, Block::mStoneSlab->mID, (DataID)StoneSlabBlock::StoneSlabType::Sandstone, mWidth - 2, 2, 2, chunkBB);
	placeBlock(region, Block::mSandstoneStairs->mID, stairsWest, 2, 1, 2, chunkBB);
	placeBlock(region, Block::mSandstoneStairs->mID, stairsEast, mWidth - 3, 1, 2, chunkBB);

	// indoor decoration
	generateBox(region, chunkBB, 4, 3, 5, 4, 3, 18, Block::mSandStone->mID, Block::mSandStone->mID, false);
	generateBox(region, chunkBB, mWidth - 5, 3, 5, mWidth - 5, 3, 17, Block::mSandStone->mID, Block::mSandStone->mID, false);
	generateBox(region, chunkBB, 3, 1, 5, 4, 2, 16, Block::mAir->mID, Block::mAir->mID, false);
	generateBox(region, chunkBB, mWidth - 6, 1, 5, mWidth - 5, 2, 16, Block::mAir->mID, Block::mAir->mID, false);
	for (int z = 5; z <= 17; z += 2) {
		placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, 4, 1, z, chunkBB);
		placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Heiroglyphs, 4, 2, z, chunkBB);
		placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, mWidth - 5, 1, z, chunkBB);
		placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Heiroglyphs, mWidth - 5, 2, z, chunkBB);
	}
	placeBlock(region, Block::mStainedClay->mID, baseDecoColor, 10, 0, 7, chunkBB);
	placeBlock(region, Block::mStainedClay->mID, baseDecoColor, 10, 0, 8, chunkBB);
	placeBlock(region, Block::mStainedClay->mID, baseDecoColor, 9, 0, 9, chunkBB);
	placeBlock(region, Block::mStainedClay->mID, baseDecoColor, 11, 0, 9, chunkBB);
	placeBlock(region, Block::mStainedClay->mID, baseDecoColor, 8, 0, 10, chunkBB);
	placeBlock(region, Block::mStainedClay->mID, baseDecoColor, 12, 0, 10, chunkBB);
	placeBlock(region, Block::mStainedClay->mID, baseDecoColor, 7, 0, 10, chunkBB);
	placeBlock(region, Block::mStainedClay->mID, baseDecoColor, 13, 0, 10, chunkBB);
	placeBlock(region, Block::mStainedClay->mID, baseDecoColor, 9, 0, 11, chunkBB);
	placeBlock(region, Block::mStainedClay->mID, baseDecoColor, 11, 0, 11, chunkBB);
	placeBlock(region, Block::mStainedClay->mID, baseDecoColor, 10, 0, 12, chunkBB);
	placeBlock(region, Block::mStainedClay->mID, baseDecoColor, 10, 0, 13, chunkBB);
	placeBlock(region, Block::mStainedClay->mID, blue, 10, 0, 10, chunkBB);

	// outdoor decoration
	for (int x = 0; x <= mWidth - 1; x += mWidth - 1) {
		placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, x, 2, 1, chunkBB);
		placeBlock(region, Block::mStainedClay->mID, baseDecoColor, x, 2, 2, chunkBB);
		placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, x, 2, 3, chunkBB);
		placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, x, 3, 1, chunkBB);
		placeBlock(region, Block::mStainedClay->mID, baseDecoColor, x, 3, 2, chunkBB);
		placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, x, 3, 3, chunkBB);
		placeBlock(region, Block::mStainedClay->mID, baseDecoColor, x, 4, 1, chunkBB);
		placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Heiroglyphs, x, 4, 2, chunkBB);
		placeBlock(region, Block::mStainedClay->mID, baseDecoColor, x, 4, 3, chunkBB);
		placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, x, 5, 1, chunkBB);
		placeBlock(region, Block::mStainedClay->mID, baseDecoColor, x, 5, 2, chunkBB);
		placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, x, 5, 3, chunkBB);
		placeBlock(region, Block::mStainedClay->mID, baseDecoColor, x, 6, 1, chunkBB);
		placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Heiroglyphs, x, 6, 2, chunkBB);
		placeBlock(region, Block::mStainedClay->mID, baseDecoColor, x, 6, 3, chunkBB);
		placeBlock(region, Block::mStainedClay->mID, baseDecoColor, x, 7, 1, chunkBB);
		placeBlock(region, Block::mStainedClay->mID, baseDecoColor, x, 7, 2, chunkBB);
		placeBlock(region, Block::mStainedClay->mID, baseDecoColor, x, 7, 3, chunkBB);
		placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, x, 8, 1, chunkBB);
		placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, x, 8, 2, chunkBB);
		placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, x, 8, 3, chunkBB);
	}
	for (int x = 2; x <= mWidth - 3; x += mWidth - 3 - 2) {
		placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, x - 1, 2, 0, chunkBB);
		placeBlock(region, Block::mStainedClay->mID, baseDecoColor, x, 2, 0, chunkBB);
		placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, x + 1, 2, 0, chunkBB);
		placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, x - 1, 3, 0, chunkBB);
		placeBlock(region, Block::mStainedClay->mID, baseDecoColor, x, 3, 0, chunkBB);
		placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, x + 1, 3, 0, chunkBB);
		placeBlock(region, Block::mStainedClay->mID, baseDecoColor, x - 1, 4, 0, chunkBB);
		placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Heiroglyphs, x, 4, 0, chunkBB);
		placeBlock(region, Block::mStainedClay->mID, baseDecoColor, x + 1, 4, 0, chunkBB);
		placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, x - 1, 5, 0, chunkBB);
		placeBlock(region, Block::mStainedClay->mID, baseDecoColor, x, 5, 0, chunkBB);
		placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, x + 1, 5, 0, chunkBB);
		placeBlock(region, Block::mStainedClay->mID, baseDecoColor, x - 1, 6, 0, chunkBB);
		placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Heiroglyphs, x, 6, 0, chunkBB);
		placeBlock(region, Block::mStainedClay->mID, baseDecoColor, x + 1, 6, 00, chunkBB);
		placeBlock(region, Block::mStainedClay->mID, baseDecoColor, x - 1, 7, 0, chunkBB);
		placeBlock(region, Block::mStainedClay->mID, baseDecoColor, x, 7, 0, chunkBB);
		placeBlock(region, Block::mStainedClay->mID, baseDecoColor, x + 1, 7, 0, chunkBB);
		placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, x - 1, 8, 0, chunkBB);
		placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, x, 8, 0, chunkBB);
		placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, x + 1, 8, 0, chunkBB);
	}
	generateBox(region, chunkBB, 8, 4, 0, 12, 6, 0, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, false);
	placeBlock(region, Block::mAir->mID, 8, 6, 0, chunkBB);
	placeBlock(region, Block::mAir->mID, 12, 6, 0, chunkBB);
	placeBlock(region, Block::mStainedClay->mID, baseDecoColor, 9, 5, 0, chunkBB);
	placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Heiroglyphs, 10, 5, 0, chunkBB);
	placeBlock(region, Block::mStainedClay->mID, baseDecoColor, 11, 5, 0, chunkBB);

	// tombs
	generateBox(region, chunkBB, 8, -14, 8, 12, -11, 12, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, false);
	generateBox(region, chunkBB, 8, -10, 8, 12, -10, 12, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Heiroglyphs, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Heiroglyphs, false);
	generateBox(region, chunkBB, 8, -9, 8, 12, -9, 12, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, false);
	generateBox(region, chunkBB, 8, -8, 8, 12, -1, 12, Block::mSandStone->mID, Block::mSandStone->mID, false);
	generateBox(region, chunkBB, 9, -11, 9, 11, -1, 11, Block::mAir->mID, Block::mAir->mID, false);
	placeBlock(region, Block::mStonePressurePlate->mID, 10, -11, 10, chunkBB);
	generateBox(region, chunkBB, 9, -13, 9, 11, -13, 11, Block::mTNT->mID, Block::mAir->mID, false);
	placeBlock(region, Block::mAir->mID, 8, -11, 10, chunkBB);
	placeBlock(region, Block::mAir->mID, 8, -10, 10, chunkBB);
	placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Heiroglyphs, 7, -10, 10, chunkBB);
	placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, 7, -11, 10, chunkBB);
	placeBlock(region, Block::mAir->mID, 12, -11, 10, chunkBB);
	placeBlock(region, Block::mAir->mID, 12, -10, 10, chunkBB);
	placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Heiroglyphs, 13, -10, 10, chunkBB);
	placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, 13, -11, 10, chunkBB);
	placeBlock(region, Block::mAir->mID, 10, -11, 8, chunkBB);
	placeBlock(region, Block::mAir->mID, 10, -10, 8, chunkBB);
	placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Heiroglyphs, 10, -10, 7, chunkBB);
	placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, 10, -11, 7, chunkBB);
	placeBlock(region, Block::mAir->mID, 10, -11, 12, chunkBB);
	placeBlock(region, Block::mAir->mID, 10, -10, 12, chunkBB);
	placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Heiroglyphs, 10, -10, 13, chunkBB);
	placeBlock(region, Block::mSandStone->mID, (DataID)SandStoneBlock::SandstoneType::Smoothside, 10, -11, 13, chunkBB);

	// chests!
	//TODO::Remove this hack when get orientation is moved into each block
	const int hackedChestDirection[4] = {
		Direction::SOUTH,
		Direction::EAST,
		Direction::NORTH,
		Direction::WEST,
	};
	for (int direction = Direction::SOUTH; direction <= Direction::EAST; ++direction) {
		if (!mHasPlacedChest[direction]) {
			int xo = Direction::STEP_X[direction] * 2;
			int zo = Direction::STEP_Z[direction] * 2;
			mHasPlacedChest[direction] = createChest(region, chunkBB, random, 10 + xo, -11, 10 + zo, hackedChestDirection[direction], "loot_tables/chests/desert_pyramid.json");
		}
	}

	return true;
}

MossStoneSelector JunglePyramidPiece::stoneSelector;
void MossStoneSelector::next(Random& random, int worldX, int worldY, int worldZ, bool isEdge)
{
	if (random.nextFloat() < .4f) {
		nextBlock = Block::mCobblestone->mID;
	}
	else {
		nextBlock = Block::mMossyCobblestone->mID;
	}
}


JunglePyramidPiece::JunglePyramidPiece(int west, int north)
	: ScatteredFeaturePiece(west, 64, north, 12, 10, 15) {
	mHasPlacedMainChest = mHasPlacedHiddenChest = mHasPlacedTrap[0] = mHasPlacedTrap[1] = false;
}

void JunglePyramidPiece::addAdditionalSaveData(CompoundTag& tag) const {
	ScatteredFeaturePiece::addAdditionalSaveData(tag);
	tag.putBoolean("hasPlacedMainChest", mHasPlacedMainChest);
	tag.putBoolean("hasPlacedHiddenChest", mHasPlacedHiddenChest);
	tag.putBoolean("hasPlacedTrap0", mHasPlacedTrap[0]);
	tag.putBoolean("hasPlacedTrap1", mHasPlacedTrap[1]);
}

void JunglePyramidPiece::readAdditionalSaveData(const CompoundTag& tag) {
	ScatteredFeaturePiece::readAdditionalSaveData(tag);
	mHasPlacedMainChest = tag.getBoolean("hasPlacedMainChest");
	mHasPlacedHiddenChest = tag.getBoolean("hasPlacedHiddenChest");
	mHasPlacedTrap[0] = tag.getBoolean("hasPlacedTrap0");
	mHasPlacedTrap[1] = tag.getBoolean("hasPlacedTrap1");
}

bool JunglePyramidPiece::postProcess(BlockSource* region, Random& random, const BoundingBox& chunkBB) {
	if (!updateAverageGroundHeight(region, chunkBB, 0)) {
		return false;
	}

	int stairsNorth = getOrientationData(Block::mStoneStairs, 3);
	int stairsSouth = getOrientationData(Block::mStoneStairs, 2);
	int stairsEast = getOrientationData(Block::mStoneStairs, 0);
	int stairsWest = getOrientationData(Block::mStoneStairs, 1);

	// floor
	generateBox(region, chunkBB, 0, -4, 0, mWidth - 1, 0, mDepth - 1, false, random, &stoneSelector);

	// first floor walls
	generateBox(region, chunkBB, 2, 1, 2, 9, 2, 2, false, random, &stoneSelector);
	generateBox(region, chunkBB, 2, 1, 12, 9, 2, 12, false, random, &stoneSelector);
	generateBox(region, chunkBB, 2, 1, 3, 2, 2, 11, false, random, &stoneSelector);
	generateBox(region, chunkBB, 9, 1, 3, 9, 2, 11, false, random, &stoneSelector);

	// second floor walls
	generateBox(region, chunkBB, 1, 3, 1, 10, 6, 1, false, random, &stoneSelector);
	generateBox(region, chunkBB, 1, 3, 13, 10, 6, 13, false, random, &stoneSelector);
	generateBox(region, chunkBB, 1, 3, 2, 1, 6, 12, false, random, &stoneSelector);
	generateBox(region, chunkBB, 10, 3, 2, 10, 6, 12, false, random, &stoneSelector);

	// roof regions
	generateBox(region, chunkBB, 2, 3, 2, 9, 3, 12, false, random, &stoneSelector);
	generateBox(region, chunkBB, 2, 6, 2, 9, 6, 12, false, random, &stoneSelector);
	generateBox(region, chunkBB, 3, 7, 3, 8, 7, 11, false, random, &stoneSelector);
	generateBox(region, chunkBB, 4, 8, 4, 7, 8, 10, false, random, &stoneSelector);

	// clear interior
	generateAirBox(region, chunkBB, 3, 1, 3, 8, 2, 11);
	generateAirBox(region, chunkBB, 4, 3, 6, 7, 3, 9);
	generateAirBox(region, chunkBB, 2, 4, 2, 9, 5, 12);
	generateAirBox(region, chunkBB, 4, 6, 5, 7, 6, 9);
	generateAirBox(region, chunkBB, 5, 7, 6, 6, 7, 8);

	// doors and windows
	generateAirBox(region, chunkBB, 5, 1, 2, 6, 2, 2);
	generateAirBox(region, chunkBB, 5, 2, 12, 6, 2, 12);
	generateAirBox(region, chunkBB, 5, 5, 1, 6, 5, 1);
	generateAirBox(region, chunkBB, 5, 5, 13, 6, 5, 13);
	placeBlock(region, Block::mAir->mID, 1, 5, 5, chunkBB);
	placeBlock(region, Block::mAir->mID, 10, 5, 5, chunkBB);
	placeBlock(region, Block::mAir->mID, 1, 5, 9, chunkBB);
	placeBlock(region, Block::mAir->mID, 10, 5, 9, chunkBB);

	// outside decoration
	for (int z = 0; z <= 14; z += 14) {
		generateBox(region, chunkBB, 2, 4, z, 2, 5, z, false, random, &stoneSelector);
		generateBox(region, chunkBB, 4, 4, z, 4, 5, z, false, random, &stoneSelector);
		generateBox(region, chunkBB, 7, 4, z, 7, 5, z, false, random, &stoneSelector);
		generateBox(region, chunkBB, 9, 4, z, 9, 5, z, false, random, &stoneSelector);
	}
	generateBox(region, chunkBB, 5, 6, 0, 6, 6, 0, false, random, &stoneSelector);
	for (int x = 0; x <= 11; x += 11) {
		for (int z = 2; z <= 12; z += 2) {
			generateBox(region, chunkBB, x, 4, z, x, 5, z, false, random, &stoneSelector);
		}
		generateBox(region, chunkBB, x, 6, 5, x, 6, 5, false, random, &stoneSelector);
		generateBox(region, chunkBB, x, 6, 9, x, 6, 9, false, random, &stoneSelector);
	}
	generateBox(region, chunkBB, 2, 7, 2, 2, 9, 2, false, random, &stoneSelector);
	generateBox(region, chunkBB, 9, 7, 2, 9, 9, 2, false, random, &stoneSelector);
	generateBox(region, chunkBB, 2, 7, 12, 2, 9, 12, false, random, &stoneSelector);
	generateBox(region, chunkBB, 9, 7, 12, 9, 9, 12, false, random, &stoneSelector);
	generateBox(region, chunkBB, 4, 9, 4, 4, 9, 4, false, random, &stoneSelector);
	generateBox(region, chunkBB, 7, 9, 4, 7, 9, 4, false, random, &stoneSelector);
	generateBox(region, chunkBB, 4, 9, 10, 4, 9, 10, false, random, &stoneSelector);
	generateBox(region, chunkBB, 7, 9, 10, 7, 9, 10, false, random, &stoneSelector);
	generateBox(region, chunkBB, 5, 9, 7, 6, 9, 7, false, random, &stoneSelector);
	placeBlock(region, Block::mStoneStairs->mID, stairsNorth, 5, 9, 6, chunkBB);
	placeBlock(region, Block::mStoneStairs->mID, stairsNorth, 6, 9, 6, chunkBB);
	placeBlock(region, Block::mStoneStairs->mID, stairsSouth, 5, 9, 8, chunkBB);
	placeBlock(region, Block::mStoneStairs->mID, stairsSouth, 6, 9, 8, chunkBB);

	// front stairs
	placeBlock(region, Block::mStoneStairs->mID, stairsNorth, 4, 0, 0, chunkBB);
	placeBlock(region, Block::mStoneStairs->mID, stairsNorth, 5, 0, 0, chunkBB);
	placeBlock(region, Block::mStoneStairs->mID, stairsNorth, 6, 0, 0, chunkBB);
	placeBlock(region, Block::mStoneStairs->mID, stairsNorth, 7, 0, 0, chunkBB);

	// indoor stairs up
	placeBlock(region, Block::mStoneStairs->mID, stairsNorth, 4, 1, 8, chunkBB);
	placeBlock(region, Block::mStoneStairs->mID, stairsNorth, 4, 2, 9, chunkBB);
	placeBlock(region, Block::mStoneStairs->mID, stairsNorth, 4, 3, 10, chunkBB);
	placeBlock(region, Block::mStoneStairs->mID, stairsNorth, 7, 1, 8, chunkBB);
	placeBlock(region, Block::mStoneStairs->mID, stairsNorth, 7, 2, 9, chunkBB);
	placeBlock(region, Block::mStoneStairs->mID, stairsNorth, 7, 3, 10, chunkBB);
	generateBox(region, chunkBB, 4, 1, 9, 4, 1, 9, false, random, &stoneSelector);
	generateBox(region, chunkBB, 7, 1, 9, 7, 1, 9, false, random, &stoneSelector);
	generateBox(region, chunkBB, 4, 1, 10, 7, 2, 10, false, random, &stoneSelector);

	// indoor hand rail
	generateBox(region, chunkBB, 5, 4, 5, 6, 4, 5, false, random, &stoneSelector);
	placeBlock(region, Block::mStoneStairs->mID, stairsEast, 4, 4, 5, chunkBB);
	placeBlock(region, Block::mStoneStairs->mID, stairsWest, 7, 4, 5, chunkBB);

	// indoor stairs down
	for (int i = 0; i < 4; i++) {
		placeBlock(region, Block::mStoneStairs->mID, stairsSouth, 5, 0 - i, 6 + i, chunkBB);
		placeBlock(region, Block::mStoneStairs->mID, stairsSouth, 6, 0 - i, 6 + i, chunkBB);
		generateAirBox(region, chunkBB, 5, 0 - i, 7 + i, 6, 0 - i, 9 + i);
	}

	// underground corridors
	generateAirBox(region, chunkBB, 1, -3, 12, 10, -1, 13);
	generateAirBox(region, chunkBB, 1, -3, 1, 3, -1, 13);
	generateAirBox(region, chunkBB, 1, -3, 1, 9, -1, 5);
	for (int z = 1; z <= 13; z += 2) {
		generateBox(region, chunkBB, 1, -3, z, 1, -2, z, false, random, &stoneSelector);
	}
	for (int z = 2; z <= 12; z += 2) {
		generateBox(region, chunkBB, 1, -1, z, 3, -1, z, false, random, &stoneSelector);
	}
	generateBox(region, chunkBB, 2, -2, 1, 5, -2, 1, false, random, &stoneSelector);
	generateBox(region, chunkBB, 7, -2, 1, 9, -2, 1, false, random, &stoneSelector);
	generateBox(region, chunkBB, 6, -3, 1, 6, -3, 1, false, random, &stoneSelector);
	generateBox(region, chunkBB, 6, -1, 1, 6, -1, 1, false, random, &stoneSelector);

	DataID data = 0;
	Block::mTripwire->getBlockState(BlockState::AttachedBit).set(data, true);

	// trip wire trap 1
	Block::mTripwire->getBlockState(BlockState::Direction).set(data, getOrientationData(Block::mTripwireHook, Direction::EAST));
	placeBlock(region, FullBlock(Block::mTripwireHook->mID, data), 1, -3, 8, chunkBB);

	Block::mTripwire->getBlockState(BlockState::Direction).set(data, getOrientationData(Block::mTripwireHook, Direction::WEST));
	placeBlock(region, FullBlock(Block::mTripwireHook->mID, data), 4, -3, 8, chunkBB);

	Block::mTripwire->getBlockState(BlockState::Direction).set(data, 0);
	placeBlock(region, FullBlock(Block::mTripwire->mID, data), 2, -3, 8, chunkBB);
	placeBlock(region, FullBlock(Block::mTripwire->mID, data), 3, -3, 8, chunkBB);

	placeBlock(region, FullBlock(Block::mRedStoneDust->mID), 5, -3, 7, chunkBB);
	placeBlock(region, FullBlock(Block::mRedStoneDust->mID), 5, -3, 6, chunkBB);
	placeBlock(region, FullBlock(Block::mRedStoneDust->mID), 5, -3, 5, chunkBB);
	placeBlock(region, FullBlock(Block::mRedStoneDust->mID), 5, -3, 4, chunkBB);
	placeBlock(region, FullBlock(Block::mRedStoneDust->mID), 5, -3, 3, chunkBB);
	placeBlock(region, FullBlock(Block::mRedStoneDust->mID), 5, -3, 2, chunkBB);
	placeBlock(region, FullBlock(Block::mRedStoneDust->mID), 5, -3, 1, chunkBB);
	placeBlock(region, FullBlock(Block::mRedStoneDust->mID), 4, -3, 1, chunkBB);
	placeBlock(region, FullBlock(Block::mMossyCobblestone->mID), 3, -3, 1, chunkBB);
	if (!mHasPlacedTrap[0]) {
		mHasPlacedTrap[0] = createDispenser(region, chunkBB, random, 3, -2, 1, Facing::SOUTH, "loot_tables/chests/dispenser_trap.json");
	}
	placeBlock(region, FullBlock(Block::mVine->mID, VineBlock::VINE_ALL), 3, -2, 2, chunkBB);

	// trip wire trap 2
	Block::mTripwire->getBlockState(BlockState::Direction).set(data, getOrientationData(Block::mTripwireHook, Direction::SOUTH));
	placeBlock(region, FullBlock(Block::mTripwireHook->mID, data), 7, -3, 1, chunkBB);
	Block::mTripwire->getBlockState(BlockState::Direction).set(data, getOrientationData(Block::mTripwireHook, Direction::NORTH));
	placeBlock(region, FullBlock(Block::mTripwireHook->mID, data), 7, -3, 5, chunkBB);

	Block::mTripwire->getBlockState(BlockState::Direction).set(data, 0);
	placeBlock(region, FullBlock(Block::mTripwire->mID, data), 7, -3, 2, chunkBB);
	placeBlock(region, FullBlock(Block::mTripwire->mID, data), 7, -3, 3, chunkBB);
	placeBlock(region, FullBlock(Block::mTripwire->mID, data), 7, -3, 4, chunkBB);

	placeBlock(region, FullBlock(Block::mRedStoneDust->mID), 8, -3, 6, chunkBB);
	placeBlock(region, FullBlock(Block::mRedStoneDust->mID), 9, -3, 6, chunkBB);
	placeBlock(region, FullBlock(Block::mRedStoneDust->mID), 9, -3, 5, chunkBB);
	placeBlock(region, FullBlock(Block::mMossyCobblestone->mID), 9, -3, 4, chunkBB);
	placeBlock(region, FullBlock(Block::mRedStoneDust->mID), 9, -2, 4, chunkBB);
	if (!mHasPlacedTrap[1]) {
		mHasPlacedTrap[1] = createDispenser(region, chunkBB, random, 9, -2, 3, Facing::WEST, "loot_tables/chests/dispenser_trap.json");
	}
	placeBlock(region, FullBlock(Block::mVine->mID, VineBlock::VINE_ALL), 8, -1, 3, chunkBB);
	placeBlock(region, FullBlock(Block::mVine->mID, VineBlock::VINE_ALL), 8, -2, 3, chunkBB);

	// main chest
	if (!mHasPlacedMainChest) {
		mHasPlacedMainChest = createChest(region, chunkBB, random, 8, -3, 3, Direction::WEST, "loot_tables/chests/jungle_temple.json");
	}
	placeBlock(region, Block::mMossyCobblestone->mID, 9, -3, 2, chunkBB);
	placeBlock(region, Block::mMossyCobblestone->mID, 8, -3, 1, chunkBB);
	placeBlock(region, Block::mMossyCobblestone->mID, 4, -3, 5, chunkBB);
	placeBlock(region, Block::mMossyCobblestone->mID, 5, -2, 5, chunkBB);
	placeBlock(region, Block::mMossyCobblestone->mID, 5, -1, 5, chunkBB);
	placeBlock(region, Block::mMossyCobblestone->mID, 6, -3, 5, chunkBB);
	placeBlock(region, Block::mMossyCobblestone->mID, 7, -2, 5, chunkBB);
	placeBlock(region, Block::mMossyCobblestone->mID, 7, -1, 5, chunkBB);
	placeBlock(region, Block::mMossyCobblestone->mID, 8, -3, 5, chunkBB);
	generateBox(region, chunkBB, 9, -1, 1, 9, -1, 5, false, random, &stoneSelector);

	//// hidden room
	generateAirBox(region, chunkBB, 8, -3, 8, 10, -1, 10);
	placeBlock(region, FullBlock(Block::mStoneBrick->mID, 3), 8, -2, 11, chunkBB);
	placeBlock(region, FullBlock(Block::mStoneBrick->mID, 3), 9, -2, 11, chunkBB);
	placeBlock(region, FullBlock(Block::mStoneBrick->mID, 3), 10, -2, 11, chunkBB);
	placeBlock(region, FullBlock(Block::mLever->mID, getOrientationData(Block::mLever, Facing::SOUTH)), 8, -2, 12, chunkBB);
	placeBlock(region, FullBlock(Block::mLever->mID, getOrientationData(Block::mLever, Facing::SOUTH)), 9, -2, 12, chunkBB);
	placeBlock(region, FullBlock(Block::mLever->mID, getOrientationData(Block::mLever, Facing::SOUTH)), 10, -2, 12, chunkBB);
	generateBox(region, chunkBB, 8, -3, 8, 8, -3, 10, false, random, &stoneSelector);
	generateBox(region, chunkBB, 10, -3, 8, 10, -3, 10, false, random, &stoneSelector);
	placeBlock(region, Block::mMossyCobblestone->mID, 10, -2, 9, chunkBB);
	placeBlock(region, Block::mRedStoneDust->mID, 8, -2, 9, chunkBB);
	placeBlock(region, Block::mRedStoneDust->mID, 8, -2, 10, chunkBB);
	placeBlock(region, Block::mRedStoneDust->mID, 10, -1, 9, chunkBB);
	placeBlock(region, FullBlock(Block::mStickyPiston->mID, Facing::UP), 9, -2, 8, chunkBB);
	placeBlock(region, FullBlock(Block::mStickyPiston->mID, getOrientationData(Block::mStickyPiston, Facing::EAST)), 10, -2, 8, chunkBB);
	placeBlock(region, FullBlock(Block::mStickyPiston->mID, getOrientationData(Block::mStickyPiston, Facing::EAST)), 10, -1, 8, chunkBB);
	placeBlock(region, FullBlock(Block::mUnpoweredRepeater->mID, getOrientationData(Block::mUnpoweredRepeater, Direction::SOUTH)), 10, -2, 10, chunkBB);
	if (!mHasPlacedHiddenChest)
	{
		mHasPlacedHiddenChest = createChest(region, chunkBB, random, 9, -3, 10, Direction::SOUTH, "loot_tables/chests/jungle_temple.json");
	}

	return true;
}

SwamplandHut::SwamplandHut(int west, int north) : ScatteredFeaturePiece(west, 64, north, 7, 7, 9) {
}

void SwamplandHut::addAdditionalSaveData(CompoundTag& tag) const {
	ScatteredFeaturePiece::addAdditionalSaveData(tag);
	tag.putBoolean("Witch", mSpawnedWitch);
}

void SwamplandHut::readAdditionalSaveData(const CompoundTag& tag) {
	ScatteredFeaturePiece::readAdditionalSaveData(tag);
	mSpawnedWitch = tag.getBoolean("Witch");
}

void SwamplandHut::placeCauldron(BlockSource* region, Random& random, int x, int y, int z, const BoundingBox& chunkBB) {
	BlockPos world = _getWorldPos(x, y, z);

	if (!chunkBB.isInside(world)) {
		return;
	}

	// TODO: rherlitz
// 	if (!region->getBlock(world).isType(Block::mCauldron)) {
// 		region->setBlockAndData(world, FullBlock(Block::mCauldron->mID, random.nextInt(2, CauldronBlock::MAX_FILL_LEVEL)), Block::UPDATE_NONE);
// 		CauldronBlockEntity* blockEntity = static_cast<CauldronBlockEntity*>(region->getBlockEntity(world));
// 		int potionWeight = random.nextInt(100);
// 		int potionIndex = Potion::Healing->getPotionId();
// 		if (potionWeight < 10) {
// 			potionIndex = Potion::WaterBreathing->getPotionId();
// 		}
// 		else if (potionWeight < 15) {
// 			potionIndex = Potion::FireResistance->getPotionId();
// 		}
// 		else if (potionWeight < 30) {
// 			potionIndex = Potion::Swiftness->getPotionId();
// 		}
// 		else if (potionWeight < 40) {
// 			potionIndex = Potion::Slowness->getPotionId();
// 		}
// 		else if (potionWeight < 65) {
// 			potionIndex = Potion::Poison->getPotionId();
// 		}
// 		else if (potionWeight < 75) {
// 			potionIndex = Potion::Weakness->getPotionId();	// ->setPotionValue(PotionBrewing.POTION_ID_SPLASH_WEAKNESS);
// 		}
// 
// 		blockEntity->setPotionId(potionIndex);
// 	}
}

bool SwamplandHut::postProcess(BlockSource* region, Random& random, const BoundingBox& chunkBB) {
	if (!updateAverageGroundHeight(region, chunkBB, 1)) {
		return false;
	}

	// Create a box of air so the hut isn't full of dirt
	generateBox(region, chunkBB, 0, 2, 0, 6, 3, 8, Block::mAir->mID, 0, Block::mAir->mID, 0, true);

	// floor and ceiling
	generateBox(region, chunkBB, 1, 1, 1, 5, 1, 7, Block::mWoodPlanks->mID, (DataID)WoodBlockType::Spruce, Block::mWoodPlanks->mID, (DataID)WoodBlockType::Spruce, false);
	generateBox(region, chunkBB, 1, 5, 2, 5, 5, 7, Block::mWoodPlanks->mID, (DataID)WoodBlockType::Spruce, Block::mWoodPlanks->mID, (DataID)WoodBlockType::Spruce, false);
	generateBox(region, chunkBB, 2, 1, 0, 4, 1, 0, Block::mWoodPlanks->mID, (DataID)WoodBlockType::Spruce, Block::mWoodPlanks->mID, (DataID)WoodBlockType::Spruce, false);

	// walls
	generateBox(region, chunkBB, 2, 2, 2, 3, 4, 2, Block::mWoodPlanks->mID, (DataID)WoodBlockType::Spruce, Block::mWoodPlanks->mID, (DataID)WoodBlockType::Spruce, false);
	generateBox(region, chunkBB, 1, 2, 3, 1, 4, 6, Block::mWoodPlanks->mID, (DataID)WoodBlockType::Spruce, Block::mWoodPlanks->mID, (DataID)WoodBlockType::Spruce, false);
	generateBox(region, chunkBB, 5, 2, 3, 5, 4, 6, Block::mWoodPlanks->mID, (DataID)WoodBlockType::Spruce, Block::mWoodPlanks->mID, (DataID)WoodBlockType::Spruce, false);
	generateBox(region, chunkBB, 2, 2, 7, 4, 4, 7, Block::mWoodPlanks->mID, (DataID)WoodBlockType::Spruce, Block::mWoodPlanks->mID, (DataID)WoodBlockType::Spruce, false);

	// pillars
	generateBox(region, chunkBB, 1, 0, 2, 1, 4, 2, Block::mLog->mID, 0, Block::mLog->mID, 0, false);
	generateBox(region, chunkBB, 5, 0, 2, 5, 4, 2, Block::mLog->mID, 0, Block::mLog->mID, 0, false);
	generateBox(region, chunkBB, 1, 0, 7, 1, 4, 7, Block::mLog->mID, 0, Block::mLog->mID, 0, false);
	generateBox(region, chunkBB, 5, 0, 7, 5, 4, 7, Block::mLog->mID, 0, Block::mLog->mID, 0, false);

	// windows
	placeBlock(region, Block::mFence->mID, 2, 3, 2, chunkBB);
	placeBlock(region, Block::mFence->mID, 3, 3, 7, chunkBB);
	placeBlock(region, Block::mAir->mID, 1, 3, 4, chunkBB);
	placeBlock(region, Block::mAir->mID, 5, 3, 4, chunkBB);
	placeBlock(region, Block::mAir->mID, 5, 3, 5, chunkBB);
	placeBlock(region, Block::mFlowerPot->mID, 1, 3, 5, chunkBB);
	placeBlock(region, Block::mWoodPlanks->mID, (DataID)WoodBlockType::Spruce, 4, 4, 2, chunkBB);

	BlockPos flowerPos = _getWorldPos(1, 3, 5);
	BlockEntity *entity = region->getBlockEntity(flowerPos);
	// TODO: rherlitz
// 	if (entity != nullptr && entity->getType() == BlockEntityType::FlowerPot) {
// 		FlowerPotBlockEntity* fbEntity = static_cast<FlowerPotBlockEntity*>(entity);
// 		fbEntity->setPlantItem(Block::mRedMushroom, 0);
// 	}

	// decoration
	placeBlock(region, Block::mWorkBench->mID, 3, 2, 6, chunkBB);
	placeCauldron(region, random, 4, 2, 6, chunkBB);

	// front railings
	placeBlock(region, Block::mFence->mID, 1, 2, 1, chunkBB);
	placeBlock(region, Block::mFence->mID, 5, 2, 1, chunkBB);

	// ceiling edges
	int south = getOrientationData(Block::mOakStairs, 3);
	int east = getOrientationData(Block::mOakStairs, 1);
	int west = getOrientationData(Block::mOakStairs, 0);
	int north = getOrientationData(Block::mOakStairs, 2);

	generateBox(region, chunkBB, 0, 5, 1, 6, 5, 1, Block::mSpruceStairs->mID, (DataID)south, Block::mSpruceStairs->mID, (DataID)south, false);
	generateBox(region, chunkBB, 0, 5, 2, 0, 5, 7, Block::mSpruceStairs->mID, (DataID)west, Block::mSpruceStairs->mID, (DataID)west, false);
	generateBox(region, chunkBB, 6, 5, 2, 6, 5, 7, Block::mSpruceStairs->mID, (DataID)east, Block::mSpruceStairs->mID, (DataID)east, false);
	generateBox(region, chunkBB, 0, 5, 8, 6, 5, 8, Block::mSpruceStairs->mID, (DataID)north, Block::mSpruceStairs->mID, (DataID)north, false);

	// fill pillars down to solid ground
	for (int z = 2; z <= 7; z += 5) {
		for (int x = 1; x <= 5; x += 4) {
			fillColumnDown(region, FullBlock(Block::mLog->mID, 0), x, -1, z, chunkBB);
		}
	}

	return true;
}

void SwamplandHut::postProcessMobsAt(BlockSource* region, Random& random, const BoundingBox& chunkBB) {
	if (!mSpawnedWitch) {
		BlockPos spawnPos(getWorldX(2, 5), getWorldY(2), getWorldZ(2, 5));

		mSpawnedWitch = true;

		// TODO: rherlitz
// 		Mob* spawned = region->getLevel().getSpawner().spawnMob(*region, EntityTypeToString(EntityType::Witch, EntityTypeNamespaceRules::ReturnWithNamespace), nullptr, spawnPos + Vec3(0.5f, 0.0f, 0.5f));
// 		// Witch is hostile, so spawn will return null on peaceful difficulty
// 		if (spawned) {
// 			spawned->setPersistent();
// 		}
	}
}

Igloo::Igloo(int west, int north)
	: ScatteredFeaturePiece(west, 64, north, 7, 5, 8) {
}

void Igloo::postProcessMobsAt(BlockSource* region, Random& random, const BoundingBox& chunkBB) {
	mLabSettings.updateBoundingBoxFromChunkPos();
// 	StructureManager& structureManager = region->getLevel().getStructureManager();
// 	StructureTemplate& laboratory = structureManager.getOrCreate(STRUCTURE_LOCATION_LABORATORY);
// 	laboratory.placeEntities(*region, mLabPos, mLabSettings.getMirror(), mLabSettings.getRotation(), chunkBB);
}

bool Igloo::postProcess(BlockSource* region, Random& random, const BoundingBox& chunkBB) {
	if (!updateAverageGroundHeight(region, chunkBB, -1)) {
		return false;
	}

	const BoundingBox& boundingBox = getBoundingBox();
	const int yLevel = region->getAboveTopSolidBlock(boundingBox.x1 - (boundingBox.x1 - boundingBox.x0), boundingBox.z1 - (boundingBox.z1 - boundingBox.z0)) - 1;
	BlockPos pos1(boundingBox.x0, yLevel, boundingBox.z0);
	std::vector<Rotation> rotations;
	for (int i = 0; i < enum_cast(Rotation::TOTAL); ++i) {
		rotations.push_back(static_cast<Rotation>(i));
	}

	StructureManager& structureManager = region->getLevel().getStructureManager();
	Rotation rot = rotations[random.nextInt(rotations.size())];
	Mirror mirror = Mirror::NONE;

	mLabSettings = StructureSettings(mirror, rot, false, Block::mAnvil, boundingBox);

	// igloo without trapdoor
	if (random.nextDouble() < 0.5) {
		StructureTemplate& iglooRound = structureManager.getOrCreate(STRUCTURE_LOCATION_IGLOO_NO_TRAPDOOR);
		iglooRound.placeInWorldChunk(*region, pos1, mLabSettings);
	}
	// igloo with trapdoor
	else {
		StructureTemplate& iglooRound = structureManager.getOrCreate(STRUCTURE_LOCATION_IGLOO_TRAPDOOR);
		iglooRound.placeInWorldChunk(*region, pos1, mLabSettings);

		StructureTemplate& ladder = structureManager.getOrCreate(STRUCTURE_LOCATION_LADDER);
		StructureTemplate& laboratory = structureManager.getOrCreate(STRUCTURE_LOCATION_LABORATORY);

		int depth = random.nextInt(8) + 4;
		for (int i = 0; i < depth; i++) {
			BlockPos pos2 = iglooRound.calculateConnectedPosition(mLabSettings, BlockPos(3, -1 - i * 3, 5), mLabSettings, BlockPos(1, 2, 1));

			ladder.placeInWorldChunk(*region, pos1.offset(pos2.x, pos2.y, pos2.z), mLabSettings);
		}

		BlockPos offsetPos = iglooRound.calculateConnectedPosition(mLabSettings, BlockPos(3, -1 - depth * 3, 5), mLabSettings, BlockPos(3, 5, 7));
		mLabPos = pos1.offset(offsetPos.x, offsetPos.y, offsetPos.z);

		laboratory.placeInWorldChunk(*region, mLabPos, mLabSettings);


		// the chest code is commented out because Adrian is currently working on a fix for chests in generated structures

		auto markers = laboratory.getMarkers(mLabPos, mLabSettings);
		for (auto it = markers.begin(); it != markers.end(); ++it) {
			if (it->second != "chest") {
				continue;
			}

			BlockPos pos = it->first;
			region->setBlock(pos, Block::mAir->mID, Block::UPDATE_ALL);

			// TODO: rherlitz
// 			BlockEntity* blockEntity = region->getBlockEntity(pos.below());
// 			if (blockEntity != nullptr && blockEntity->isType(BlockEntityType::Chest)) {
// 				ChestBlockEntity* chest = static_cast<ChestBlockEntity*>(blockEntity);
// 				Util::LootTableUtils::fillChest(region->getLevel(), *chest, random, "loot_tables/chests/igloo_chest.json");
// 			}
		}
	}
	return true;
}
