/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/levelgen/structure/StrongholdPieces.h"

#include "nbt/CompoundTag.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/level/block/Block.h"
#include "world/level/block/DoorBlock.h"
#include "world/level/block/StoneBlock.h"
#include "world/level/block/StoneSlabBlock.h"
#include "world/level/block/MonsterEggBlock.h"
#include "world/level/block/EndPortalFrameBlock.h"
// #include "world/level/block/entity/MobSpawnerBlockEntity.h"
#include "world/item/Item.h"
// #include "world/item/DyePowderItem.h"
#include "world/Facing.h"
#include "world/entity/EntityTypes.h"

void SmoothStoneSelector::next(Random& random, int worldX, int worldY, int worldZ, bool isEdge){
	if (isEdge) {
		nextBlock = Block::mStoneBrick->mID;

		float selection = random.nextFloat();
		if (selection < .2f) {
			nextBlock.data = 2;
		}
		else if (selection < .5f) {
			nextBlock.data = 1;
		}
		else if (selection < .55f) {
			nextBlock.id = Block::mMonsterStoneEgg->mID;
			nextBlock.data = enum_cast(MonsterEggBlock::HostBlockType::StoneBrick);
		}
		else {
			nextBlock.data = 0;
		}
	}
	else {
		nextBlock = FullBlock::AIR;
	}
}

SmoothStoneSelector StrongholdPiece::smoothStoneSelector;

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

void StrongholdPiece::addAdditionalSaveData(CompoundTag& tag) const {
	tag.putInt("EntryDoor", (int)entryDoor);
}

void StrongholdPiece::readAdditionalSaveData(const CompoundTag& tag) {
	entryDoor = (SmallDoorType)tag.getInt("EntryDoor");
}

Unique<StructurePiece> StrongholdPiece::findAndCreatePieceFactory(const std::string& pieceClass, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int depth){
	if (pieceClass == "Straight") {
		return SHStraight::createPiece(pieces, random, footX, footY, footZ, direction, depth);
	}
	else if (pieceClass == "PrisonHall") {
		return SHPrisonHall::createPiece(pieces, random, footX, footY, footZ, direction, depth);
	}
	else if (pieceClass == "LeftTurn") {
		return SHLeftTurn::createPiece(pieces, random, footX, footY, footZ, direction, depth);
	}
	else if (pieceClass == "RightTurn") {
		return SHRightTurn::createPiece(pieces, random, footX, footY, footZ, direction, depth);
	}
	else if (pieceClass == "RoomCrossing") {
		return SHRoomCrossing::createPiece(pieces, random, footX, footY, footZ, direction, depth);
	}
	else if (pieceClass == "StraightStairsDown") {
		return SHStraightStairsDown::createPiece(pieces, random, footX, footY, footZ, direction, depth);
	}
	else if (pieceClass == "StairsDown") {
		return SHStairsDown::createPiece(pieces, random, footX, footY, footZ, direction, depth);
	}
	else if (pieceClass == "FiveCrossing") {
		return SHFiveCrossing::createPiece(pieces, random, footX, footY, footZ, direction, depth);
	}
	else if (pieceClass == "ChestCorridor") {
		return SHChestCorridor::createPiece(pieces, random, footX, footY, footZ, direction, depth);
	}
	else if (pieceClass == "Library") {
		return SHLibrary::createPiece(pieces, random, footX, footY, footZ, direction, depth);
	}
	else if (pieceClass == "PortalRoom") {
		return SHPortalRoom::createPiece(pieces, random, footX, footY, footZ, direction, depth);
	}
	return nullptr;
}

Unique<StructurePiece> StrongholdPiece::generatePieceFromSmallDoor(SHStartPiece* startPiece, PieceList& pieces, const Random& randomRef, int footX, int footY, int footZ, int direction, int depth) {
	// The reason this is here is to fix Bug 25284 which is a crash when generating a strong hold on Android.
	// Random used to be passed by value, but with that we would get some weird behavior and passing it in by ref fixes
	// the issue.  To keep the same behavior as passing by value, we make a copy of Random here and use that.
	Random random = randomRef;

	if (startPiece->imposedPiece != "") {

		auto strongholdPiece = findAndCreatePieceFactory(startPiece->imposedPiece, pieces, random, footX, footY, footZ, direction, depth);
		startPiece->imposedPiece = "";

		if (strongholdPiece != nullptr) {
			return strongholdPiece;
		}
	}

	int totalWeight = StructurePiece::getTotalWeight(startPiece->pieceWeights);
	if (totalWeight <= 0) {
		return nullptr;
	}

	int numAttempts = 0;

	while (numAttempts < 5) {
		numAttempts++;

		int weightSelection = random.nextInt(totalWeight);
		PieceWeightList& pieceWeights = startPiece->pieceWeights;
		PieceWeightList::iterator pieceIt = pieceWeights.begin();

		while (pieceIt != pieceWeights.end()) {
			weightSelection -= (*pieceIt).weight;
			if (weightSelection < 0) {

				if (!(*pieceIt).doPlace(depth) || ((*pieceIt).pieceClass == startPiece->previousPiece && pieceWeights.size() > 1)) {
					break;
				}

				auto strongholdPiece = findAndCreatePieceFactory((*pieceIt).pieceClass, pieces, random, footX, footY, footZ, direction, depth);
				if (strongholdPiece != nullptr) {
					(*pieceIt).placeCount++;
					startPiece->previousPiece = (*pieceIt).pieceClass;

					if (!(*pieceIt).isValid()) {
						pieceWeights.erase(pieceIt);
					}
					return strongholdPiece;
				}
			}
			++pieceIt;
		}
	}
	BoundingBox box = SHFillerCorridor::findPieceBox(pieces, random, footX, footY, footZ, direction);
	if (box.isValid() && box.y0 > 1) {
		return make_unique<SHFillerCorridor>(depth, random, box, direction);
	}

	return nullptr;

}

StructurePiece* StrongholdPiece::generateAndAddPiece(SHStartPiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int depth){
	if (depth > MAX_DEPTH) {
		return nullptr;
	}
	if (std::abs(footX - startPiece->getBoundingBox().x0) > 7 * 16 || std::abs(footZ - startPiece->getBoundingBox().z0) > 7 * 16) {
		return nullptr;
	}

	if (auto newPiece = generatePieceFromSmallDoor(startPiece, pieces, random, footX, footY, footZ, direction, depth + 1)) {
		auto ptr = newPiece.get();

		pieces.emplace_back(std::move(newPiece));
		startPiece->pendingChildren.push_back(ptr);
		return ptr;
	}

	return nullptr;
}

void StrongholdPiece::generateSmallDoor(BlockSource* level, Random& random, const BoundingBox& chunkBB, SmallDoorType doorType, int footX, int footY, int footZ){
	DataID upperDoorData = 0;
	Block::mWoodenDoor->getBlockState(BlockState::UpperBlockBit).set(upperDoorData, true);

	switch (doorType) {
	default:
	case OPENING:
		generateBox(level, chunkBB, footX, footY, footZ, footX + SMALL_DOOR_WIDTH - 1, footY + SMALL_DOOR_HEIGHT - 1, footZ, BlockID::AIR, BlockID::AIR, false);
		break;
	case WOOD_DOOR:
		placeBlock(level, Block::mStoneBrick->mID, 0, footX, footY, footZ, chunkBB);
		placeBlock(level, Block::mStoneBrick->mID, 0, footX, footY + 1, footZ, chunkBB);
		placeBlock(level, Block::mStoneBrick->mID, 0, footX, footY + 2, footZ, chunkBB);
		placeBlock(level, Block::mStoneBrick->mID, 0, footX + 1, footY + 2, footZ, chunkBB);
		placeBlock(level, Block::mStoneBrick->mID, 0, footX + 2, footY + 2, footZ, chunkBB);
		placeBlock(level, Block::mStoneBrick->mID, 0, footX + 2, footY + 1, footZ, chunkBB);
		placeBlock(level, Block::mStoneBrick->mID, 0, footX + 2, footY, footZ, chunkBB);
		placeBlock(level, Block::mWoodenDoor->mID, 0, footX + 1, footY, footZ, chunkBB);
		placeBlock(level, Block::mWoodenDoor->mID, upperDoorData, footX + 1, footY + 1, footZ, chunkBB);
		break;
	case GRATES:
		placeBlock(level, BlockID::AIR, 0, footX + 1, footY, footZ, chunkBB);
		placeBlock(level, BlockID::AIR, 0, footX + 1, footY + 1, footZ, chunkBB);
		placeBlock(level, Block::mIronFence->mID, 0, footX, footY, footZ, chunkBB);
		placeBlock(level, Block::mIronFence->mID, 0, footX, footY + 1, footZ, chunkBB);
		placeBlock(level, Block::mIronFence->mID, 0, footX, footY + 2, footZ, chunkBB);
		placeBlock(level, Block::mIronFence->mID, 0, footX + 1, footY + 2, footZ, chunkBB);
		placeBlock(level, Block::mIronFence->mID, 0, footX + 2, footY + 2, footZ, chunkBB);
		placeBlock(level, Block::mIronFence->mID, 0, footX + 2, footY + 1, footZ, chunkBB);
		placeBlock(level, Block::mIronFence->mID, 0, footX + 2, footY, footZ, chunkBB);
		break;
	case IRON_DOOR:
		placeBlock(level, Block::mStoneBrick->mID, 0, footX, footY, footZ, chunkBB);
		placeBlock(level, Block::mStoneBrick->mID, 0, footX, footY + 1, footZ, chunkBB);
		placeBlock(level, Block::mStoneBrick->mID, 0, footX, footY + 2, footZ, chunkBB);
		placeBlock(level, Block::mStoneBrick->mID, 0, footX + 1, footY + 2, footZ, chunkBB);
		placeBlock(level, Block::mStoneBrick->mID, 0, footX + 2, footY + 2, footZ, chunkBB);
		placeBlock(level, Block::mStoneBrick->mID, 0, footX + 2, footY + 1, footZ, chunkBB);
		placeBlock(level, Block::mStoneBrick->mID, 0, footX + 2, footY, footZ, chunkBB);
		placeBlock(level, Block::mIronDoor->mID, 0, footX + 1, footY, footZ, chunkBB);
		placeBlock(level, Block::mWoodButton->mID, getOrientationData(Block::mWoodButton, 4), footX + 2, footY + 1, footZ + 1, chunkBB);
		placeBlock(level, Block::mWoodButton->mID, getOrientationData(Block::mWoodButton, 3), footX + 2, footY + 1, footZ - 1, chunkBB);
		break;
	}

}

StructurePiece* StrongholdPiece::generateSmallDoorChildForward(SHStartPiece* startPiece, PieceList& pieces, Random& random, int xOff, int yOff){
	switch (orientation) {
	case Direction::NORTH:
		return generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 + xOff, boundingBox.y0 + yOff, boundingBox.z0 - 1, orientation, getGenDepth());
	case Direction::SOUTH:
		return generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 + xOff, boundingBox.y0 + yOff, boundingBox.z1 + 1, orientation, getGenDepth());
	case Direction::WEST:
		return generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 - 1, boundingBox.y0 + yOff, boundingBox.z0 + xOff, orientation, getGenDepth());
	case Direction::EAST:
		return generateAndAddPiece(startPiece, pieces, random, boundingBox.x1 + 1, boundingBox.y0 + yOff, boundingBox.z0 + xOff, orientation, getGenDepth());
	}
	return nullptr;

}

StructurePiece* StrongholdPiece::generateSmallDoorChildLeft(SHStartPiece* startPiece, PieceList& pieces, Random& random, int yOff, int zOff){
	switch (orientation) {
	case Direction::NORTH:
		return generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 - 1, boundingBox.y0 + yOff, boundingBox.z0 + zOff, Direction::WEST, getGenDepth());
	case Direction::SOUTH:
		return generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 - 1, boundingBox.y0 + yOff, boundingBox.z0 + zOff, Direction::WEST, getGenDepth());
	case Direction::WEST:
		return generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 + zOff, boundingBox.y0 + yOff, boundingBox.z0 - 1, Direction::NORTH, getGenDepth());
	case Direction::EAST:
		return generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 + zOff, boundingBox.y0 + yOff, boundingBox.z0 - 1, Direction::NORTH, getGenDepth());
	}
	return nullptr;

}

StructurePiece* StrongholdPiece::generateSmallDoorChildRight(SHStartPiece* startPiece, PieceList& pieces, Random& random, int yOff, int zOff){
	switch (orientation) {
	case Direction::NORTH:
		return generateAndAddPiece(startPiece, pieces, random, boundingBox.x1 + 1, boundingBox.y0 + yOff, boundingBox.z0 + zOff, Direction::EAST, getGenDepth());
	case Direction::SOUTH:
		return generateAndAddPiece(startPiece, pieces, random, boundingBox.x1 + 1, boundingBox.y0 + yOff, boundingBox.z0 + zOff, Direction::EAST, getGenDepth());
	case Direction::WEST:
		return generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 + zOff, boundingBox.y0 + yOff, boundingBox.z1 + 1, Direction::SOUTH, getGenDepth());
	case Direction::EAST:
		return generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 + zOff, boundingBox.y0 + yOff, boundingBox.z1 + 1, Direction::SOUTH, getGenDepth());
	}
	return nullptr;

}

StrongholdPiece::SmallDoorType StrongholdPiece::randomSmallDoor(Random& random){
	int selection = random.nextInt(5);

	switch (selection) {
	case 0:
	case 1:
		return StrongholdPiece::OPENING;
	case 2:
		return StrongholdPiece::WOOD_DOOR;
	case 3:
		return StrongholdPiece::GRATES;
		//case 4:
		//	return StrongholdPiece::IRON_DOOR;
	}

	return StrongholdPiece::OPENING;
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

SHStairsDown::SHStairsDown()
	: StrongholdPiece()
	, isSource(false){

}

SHStairsDown::SHStairsDown(int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: StrongholdPiece(genDepth)
	, isSource(false){
	orientation = direction;
	entryDoor = randomSmallDoor(random);
	boundingBox = stairsBox;
}

SHStairsDown::SHStairsDown(int genDepth, Random& random, int west, int north)
	: StrongholdPiece(genDepth)
	, isSource(true){
	orientation = random.nextInt(4);
	entryDoor = OPENING;

	switch (orientation) {
	case Direction::NORTH:
	case Direction::SOUTH:
		boundingBox = BoundingBox(west, 64, north, west + width - 1, 64 + height - 1, north + depth - 1);
		break;
	default:
		boundingBox = BoundingBox(west, 64, north, west + depth - 1, 64 + height - 1, north + width - 1);
		break;
	}
}

Unique<StrongholdPiece> SHStairsDown::createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth){
	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, -1, 4 - height, 0, width, height, depth, direction);

	if (!isOkBox(box) || StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<SHStairsDown>(genDepth, random, box, direction);
}

bool SHStairsDown::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB){
	if (CHECK_WATER_EDGE && edgesLiquid(level, chunkBB)) {
		return false;
	}

	// bounding walls
	generateBox(level, chunkBB, 0, 0, 0, width - 1, height - 1, depth - 1, CHECK_AIR, random, &smoothStoneSelector);
	// entry door
	generateSmallDoor(level, random, chunkBB, entryDoor, 1, height - SMALL_DOOR_HEIGHT - 1, 0);
	// exit door
	generateSmallDoor(level, random, chunkBB, OPENING, 1, 1, depth - 1);

	// stair steps
	placeBlock(level, Block::mStoneBrick->mID, 0, 2, 6, 1, chunkBB);
	placeBlock(level, Block::mStoneBrick->mID, 0, 1, 5, 1, chunkBB);
	placeBlock(level, Block::mStoneSlab->mID, enum_cast(StoneSlabBlock::StoneSlabType::StoneBrick), 1, 6, 1, chunkBB);
	placeBlock(level, Block::mStoneBrick->mID, 0, 1, 5, 2, chunkBB);
	placeBlock(level, Block::mStoneBrick->mID, 0, 1, 4, 3, chunkBB);
	placeBlock(level, Block::mStoneSlab->mID, enum_cast(StoneSlabBlock::StoneSlabType::StoneBrick), 1, 5, 3, chunkBB);
	placeBlock(level, Block::mStoneBrick->mID, 0, 2, 4, 3, chunkBB);
	placeBlock(level, Block::mStoneBrick->mID, 0, 3, 3, 3, chunkBB);
	placeBlock(level, Block::mStoneSlab->mID, enum_cast(StoneSlabBlock::StoneSlabType::StoneBrick), 3, 4, 3, chunkBB);
	placeBlock(level, Block::mStoneBrick->mID, 0, 3, 3, 2, chunkBB);
	placeBlock(level, Block::mStoneBrick->mID, 0, 3, 2, 1, chunkBB);
	placeBlock(level, Block::mStoneSlab->mID, enum_cast(StoneSlabBlock::StoneSlabType::StoneBrick), 3, 3, 1, chunkBB);
	placeBlock(level, Block::mStoneBrick->mID, 0, 2, 2, 1, chunkBB);
	placeBlock(level, Block::mStoneBrick->mID, 0, 1, 1, 1, chunkBB);
	placeBlock(level, Block::mStoneSlab->mID, enum_cast(StoneSlabBlock::StoneSlabType::StoneBrick), 1, 2, 1, chunkBB);
	placeBlock(level, Block::mStoneBrick->mID, 0, 1, 1, 2, chunkBB);
	placeBlock(level, Block::mStoneSlab->mID, enum_cast(StoneSlabBlock::StoneSlabType::StoneBrick), 1, 1, 3, chunkBB);

	if (isSource) {
		//LOGE("STRONGHOLD START ROOM (%d,%d,%d)\n", getWorldX(0, 0), getWorldY(0), getWorldZ(0, 0));
	}

	return true;
}

void SHStairsDown::addAdditionalSaveData(CompoundTag& tag) const {
	StrongholdPiece::addAdditionalSaveData(tag);
	tag.putBoolean("Source", isSource);
}

void SHStairsDown::readAdditionalSaveData(const CompoundTag& tag) {
	StrongholdPiece::readAdditionalSaveData(tag);
	isSource = tag.getBoolean("Source");
}

void SHStairsDown::addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random){
	if (isSource) {
		// place a crossing to force larger dungeons
		((SHStartPiece*)startPiece)->imposedPiece = "FiveCrossing";
	}
	generateSmallDoorChildForward((SHStartPiece*) startPiece, pieces, random, 1, 1);
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

SHStartPiece::SHStartPiece()
	: SHStairsDown(){

}

SHStartPiece::SHStartPiece(int genDepth, Random& random, int west, int north)
	: SHStairsDown(genDepth, random, west, north){
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

SHChestCorridor::SHChestCorridor()
	: StrongholdPiece(){

}

SHChestCorridor::SHChestCorridor(int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: StrongholdPiece(genDepth){
	orientation = direction;
	entryDoor = randomSmallDoor(random);
	boundingBox = stairsBox;
	hasPlacedChest = false;
}

Unique<StrongholdPiece> SHChestCorridor::createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth){
	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, -1, -1, 0, width, height, depth, direction);

	if (!isOkBox(box) || StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<SHChestCorridor>(genDepth, random, box, direction);
}

bool SHChestCorridor::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB){
	if (CHECK_WATER_EDGE && edgesLiquid(level, chunkBB)) {
		return false;
	}

	// bounding walls
	generateBox(level, chunkBB, 0, 0, 0, width - 1, height - 1, depth - 1, CHECK_AIR, random, &smoothStoneSelector);
	// entry door
	generateSmallDoor(level, random, chunkBB, entryDoor, 1, height - SMALL_DOOR_HEIGHT - 1, 0);
	// exit door
	generateSmallDoor(level, random, chunkBB, OPENING, 1, 1, depth - 1);

	// chest placement
	generateBox(level, chunkBB, 3, 1, 2, 3, 1, 4, Block::mStoneBrick->mID, BlockID::AIR, false);
	placeBlock(level, Block::mStoneSlab->mID, enum_cast(StoneSlabBlock::StoneSlabType::StoneBrick), 3, 1, 1, chunkBB);
	placeBlock(level, Block::mStoneSlab->mID, enum_cast(StoneSlabBlock::StoneSlabType::StoneBrick), 3, 1, 5, chunkBB);
	placeBlock(level, Block::mStoneSlab->mID, enum_cast(StoneSlabBlock::StoneSlabType::StoneBrick), 3, 2, 2, chunkBB);
	placeBlock(level, Block::mStoneSlab->mID, enum_cast(StoneSlabBlock::StoneSlabType::StoneBrick), 3, 2, 4, chunkBB);

	for (int z = 2; z <= 4; z++) {
		placeBlock(level, Block::mStoneSlab->mID, enum_cast(StoneSlabBlock::StoneSlabType::StoneBrick), 2, 1, z, chunkBB);
	}

	if (!hasPlacedChest) {
		int y = getWorldY(2);
		int x = getWorldX(3, 3), z = getWorldZ(3, 3);
		if (chunkBB.isInside(x, y, z)) {
			hasPlacedChest = true;
			createChest(level, chunkBB, random, 3, 2, 3, Direction::WEST, "loot_tables/chests/stronghold_corridor.json");
		}
	}

	return true;
}

void SHChestCorridor::addAdditionalSaveData(CompoundTag& tag) const {
	StrongholdPiece::addAdditionalSaveData(tag);
	tag.putBoolean("Chest", hasPlacedChest);
}

void SHChestCorridor::readAdditionalSaveData(const CompoundTag& tag) {
	StrongholdPiece::readAdditionalSaveData(tag);
	hasPlacedChest = tag.getBoolean("Chest");
}

void SHChestCorridor::addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random){
	generateSmallDoorChildForward((SHStartPiece*) startPiece, pieces, random, 1, 1);
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

SHFillerCorridor::SHFillerCorridor()
	: StrongholdPiece()
	, steps(0){

}

SHFillerCorridor::SHFillerCorridor(int genDepth, Random& random, const BoundingBox& corridorBox, int direction)
	: StrongholdPiece(genDepth){
	orientation = direction;
	entryDoor = randomSmallDoor(random);
	boundingBox = corridorBox;
	steps = direction == Direction::NORTH || direction == Direction::SOUTH ? corridorBox.getZSpan() : corridorBox.getXSpan();
}

BoundingBox SHFillerCorridor::findPieceBox(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction){
	const int maxLength = 3;

	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, -1, -1, 0, 5, 5, maxLength + 1, direction);

	StructurePiece* collisionPiece = StructurePiece::findCollisionPiece(pieces, box);
	if (collisionPiece == nullptr) {
		// the filler must collide with something in order to be generated
		return BoundingBox::getUnknownBox();
	}

	if (collisionPiece->getBoundingBox().y0 == box.y0) {
		// attempt to make a smaller piece until it fits
		for(auto depth : reverse_range(1, maxLength + 1)) {
			box = BoundingBox::orientBox(footX, footY, footZ, -1, -1, 0, 5, 5, depth - 1, direction);
			if (!collisionPiece->getBoundingBox().intersects(box)) {
				// the corridor has shrunk enough to fit, but make it one step
				// too big to build an entrance into the other block
				return BoundingBox::orientBox(footX, footY, footZ, -1, -1, 0, 5, 5, depth, direction);
			}
		}
	}

	return BoundingBox::getUnknownBox();
}

bool SHFillerCorridor::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB){
	if (CHECK_WATER_EDGE && edgesLiquid(level, chunkBB)) {
		return false;
	}

	// filler corridor
	for (int i = 0; i < steps; i++) {
		// row 0
		placeBlock(level, Block::mStoneBrick->mID, 0, 0, 0, i, chunkBB);
		placeBlock(level, Block::mStoneBrick->mID, 0, 1, 0, i, chunkBB);
		placeBlock(level, Block::mStoneBrick->mID, 0, 2, 0, i, chunkBB);
		placeBlock(level, Block::mStoneBrick->mID, 0, 3, 0, i, chunkBB);
		placeBlock(level, Block::mStoneBrick->mID, 0, 4, 0, i, chunkBB);

		// row 1-3
		for (int y = 1; y <= 3; y++) {
			placeBlock(level, Block::mStoneBrick->mID, 0, 0, y, i, chunkBB);
			placeBlock(level, BlockID::AIR, 0, 1, y, i, chunkBB);
			placeBlock(level, BlockID::AIR, 0, 2, y, i, chunkBB);
			placeBlock(level, BlockID::AIR, 0, 3, y, i, chunkBB);
			placeBlock(level, Block::mStoneBrick->mID, 0, 4, y, i, chunkBB);
		}
		// row 4
		placeBlock(level, Block::mStoneBrick->mID, 0, 0, 4, i, chunkBB);
		placeBlock(level, Block::mStoneBrick->mID, 0, 1, 4, i, chunkBB);
		placeBlock(level, Block::mStoneBrick->mID, 0, 2, 4, i, chunkBB);
		placeBlock(level, Block::mStoneBrick->mID, 0, 3, 4, i, chunkBB);
		placeBlock(level, Block::mStoneBrick->mID, 0, 4, 4, i, chunkBB);
	}

	return true;
}

void SHFillerCorridor::addAdditionalSaveData(CompoundTag& tag) const {
	StrongholdPiece::addAdditionalSaveData(tag);
	tag.putInt("Steps", steps);
}

void SHFillerCorridor::readAdditionalSaveData(const CompoundTag& tag) {
	StrongholdPiece::readAdditionalSaveData(tag);
	steps = tag.getInt("Steps");
}

void SHFillerCorridor::addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random){

}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

SHFiveCrossing::SHFiveCrossing()
	: StrongholdPiece(){

}

SHFiveCrossing::SHFiveCrossing(int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: StrongholdPiece(genDepth){
	orientation = direction;
	entryDoor = randomSmallDoor(random);
	boundingBox = stairsBox;

	leftLow = random.nextBoolean();
	leftHigh = random.nextBoolean();
	rightLow = random.nextBoolean();
	rightHigh = random.nextInt(3) > 0;
}

Unique<StrongholdPiece> SHFiveCrossing::createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth){
	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, -4, -3, 0, width, height, depth, direction);

	if (!isOkBox(box) || StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<SHFiveCrossing>(genDepth, random, box, direction);
}

bool SHFiveCrossing::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB){
	if (CHECK_WATER_EDGE && edgesLiquid(level, chunkBB)) {
		return false;
	}

	// bounding walls
	generateBox(level, chunkBB, 0, 0, 0, width - 1, height - 1, depth - 1, CHECK_AIR, random, &smoothStoneSelector);
	// entry door
	generateSmallDoor(level, random, chunkBB, entryDoor, 4, 3, 0);

	// exit openings
	if (leftLow) {
		generateBox(level, chunkBB, 0, 3, 1, 0, 5, 3, BlockID::AIR, BlockID::AIR, false);
	}
	if (rightLow) {
		generateBox(level, chunkBB, 9, 3, 1, 9, 5, 3, BlockID::AIR, BlockID::AIR, false);
	}
	if (leftHigh) {
		generateBox(level, chunkBB, 0, 5, 7, 0, 7, 9, BlockID::AIR, BlockID::AIR, false);
	}
	if (rightHigh) {
		generateBox(level, chunkBB, 9, 5, 7, 9, 7, 9, BlockID::AIR, BlockID::AIR, false);
	}
	generateBox(level, chunkBB, 5, 1, 10, 7, 3, 10, BlockID::AIR, BlockID::AIR, false);

	// main floor
	generateBox(level, chunkBB, 1, 2, 1, 8, 2, 6, false, random, &smoothStoneSelector);
	// side walls
	generateBox(level, chunkBB, 4, 1, 5, 4, 4, 9, false, random, &smoothStoneSelector);
	generateBox(level, chunkBB, 8, 1, 5, 8, 4, 9, false, random, &smoothStoneSelector);
	// upper floor
	generateBox(level, chunkBB, 1, 4, 7, 3, 4, 9, false, random, &smoothStoneSelector);

	// left stairs
	generateBox(level, chunkBB, 1, 3, 5, 3, 3, 6, false, random, &smoothStoneSelector);
	generateBox(level, chunkBB, 1, 3, 4, 3, 3, 4, Block::mStoneSlab->mID, enum_cast(StoneSlabBlock::StoneSlabType::StoneBrick), BlockID::AIR, 0, false);
	generateBox(level, chunkBB, 1, 4, 6, 3, 4, 6, Block::mStoneSlab->mID, enum_cast(StoneSlabBlock::StoneSlabType::StoneBrick), BlockID::AIR, 0, false);

	// lower stairs
	generateBox(level, chunkBB, 5, 1, 7, 7, 1, 8, false, random, &smoothStoneSelector);
	generateBox(level, chunkBB, 5, 1, 9, 7, 1, 9, Block::mStoneSlab->mID, enum_cast(StoneSlabBlock::StoneSlabType::StoneBrick), BlockID::AIR, 0, false);
	generateBox(level, chunkBB, 5, 2, 7, 7, 2, 7, Block::mStoneSlab->mID, enum_cast(StoneSlabBlock::StoneSlabType::StoneBrick), BlockID::AIR, 0, false);

	// bridge
	generateBox(level, chunkBB, 4, 5, 7, 4, 5, 9, Block::mStoneSlab->mID, enum_cast(StoneSlabBlock::StoneSlabType::StoneBrick), BlockID::AIR, 0, false);
	generateBox(level, chunkBB, 8, 5, 7, 8, 5, 9, Block::mStoneSlab->mID, enum_cast(StoneSlabBlock::StoneSlabType::StoneBrick), BlockID::AIR, 0, false);
	generateBox(level, chunkBB, 5, 5, 7, 7, 5, 9, Block::mDoubleStoneSlab->mID, 0, BlockID::AIR, 0, false);
	placeBlock(level, Block::mTorch->mID, getTorchData(Direction::NORTH), 6, 5, 6, chunkBB);

	return true;
}

void SHFiveCrossing::addAdditionalSaveData(CompoundTag& tag) const {
	StrongholdPiece::addAdditionalSaveData(tag);
	tag.putBoolean("leftLow", leftLow);
	tag.putBoolean("leftHigh", leftHigh);
	tag.putBoolean("rightLow", rightLow);
	tag.putBoolean("rightHigh", rightHigh);
}

void SHFiveCrossing::readAdditionalSaveData(const CompoundTag& tag) {
	StrongholdPiece::readAdditionalSaveData(tag);
	leftLow = tag.getBoolean("leftLow");
	leftHigh = tag.getBoolean("leftHigh");
	rightLow = tag.getBoolean("rightLow");
	rightHigh = tag.getBoolean("rightHigh");
}

void SHFiveCrossing::addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random){
	int zOffA = 3;
	int zOffB = 5;
	// compensate for weird negative-facing behaviour
	if (orientation == Direction::WEST || orientation == Direction::NORTH) {
		zOffA = depth - 3 - zOffA;
		zOffB = depth - 3 - zOffB;
	}

	generateSmallDoorChildForward((SHStartPiece*) startPiece, pieces, random, 5, 1);
	if (leftLow) {
		generateSmallDoorChildLeft((SHStartPiece*) startPiece, pieces, random, zOffA, 1);
	}
	if (leftHigh) {
		generateSmallDoorChildLeft((SHStartPiece*) startPiece, pieces, random, zOffB, 7);
	}
	if (rightLow) {
		generateSmallDoorChildRight((SHStartPiece*) startPiece, pieces, random, zOffA, 1);
	}
	if (rightHigh) {
		generateSmallDoorChildRight((SHStartPiece*) startPiece, pieces, random, zOffB, 7);
	}

}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

SHLeftTurn::SHLeftTurn()
	: StrongholdPiece(){

}

SHLeftTurn::SHLeftTurn(int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: StrongholdPiece(genDepth){
	orientation = direction;
	entryDoor = randomSmallDoor(random);
	boundingBox = stairsBox;
}

Unique<StrongholdPiece> SHLeftTurn::createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth){
	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, -1, -1, 0, width, height, depth, direction);

	if (!isOkBox(box) || StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<SHLeftTurn>(genDepth, random, box, direction);
}

bool SHLeftTurn::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB){
	if (CHECK_WATER_EDGE && edgesLiquid(level, chunkBB)) {
		return false;
	}

	// bounding walls
	generateBox(level, chunkBB, 0, 0, 0, width - 1, height - 1, depth - 1, CHECK_AIR, random, &smoothStoneSelector);
	// entry door
	generateSmallDoor(level, random, chunkBB, entryDoor, 1, height - SMALL_DOOR_HEIGHT - 1, 0);
	// exit opening
	if (orientation == Direction::NORTH || orientation == Direction::EAST) {
		generateBox(level, chunkBB, 0, 1, 1, 0, 3, 3, BlockID::AIR, BlockID::AIR, false);
	}
	else {
		generateBox(level, chunkBB, 4, 1, 1, 4, 3, 3, BlockID::AIR, BlockID::AIR, false);
	}

	return true;
}

void SHLeftTurn::addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random){
	if (orientation == Direction::NORTH || orientation == Direction::EAST) {
		generateSmallDoorChildLeft((SHStartPiece*) startPiece, pieces, random, 1, 1);
	}
	else {
		generateSmallDoorChildRight((SHStartPiece*) startPiece, pieces, random, 1, 1);
	}
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

SHRightTurn::SHRightTurn()
	: StrongholdPiece(){

}

SHRightTurn::SHRightTurn(int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: StrongholdPiece(genDepth){
	orientation = direction;
	entryDoor = randomSmallDoor(random);
	boundingBox = stairsBox;
}

Unique<StrongholdPiece> SHRightTurn::createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth){
	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, -1, -1, 0, width, height, depth, direction);

	if (!isOkBox(box) || StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<SHRightTurn>(genDepth, random, box, direction);
}

bool SHRightTurn::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB){
	if (CHECK_WATER_EDGE && edgesLiquid(level, chunkBB)) {
		return false;
	}

	// bounding walls
	generateBox(level, chunkBB, 0, 0, 0, width - 1, height - 1, depth - 1, CHECK_AIR, random, &smoothStoneSelector);
	// entry door
	generateSmallDoor(level, random, chunkBB, entryDoor, 1, height - SMALL_DOOR_HEIGHT - 1, 0);
	// exit opening
	if (orientation == Direction::NORTH || orientation == Direction::EAST) {
		generateBox(level, chunkBB, 4, 1, 1, 4, 3, 3, BlockID::AIR, BlockID::AIR, false);
	}
	else {
		generateBox(level, chunkBB, 0, 1, 1, 0, 3, 3, BlockID::AIR, BlockID::AIR, false);
	}

	return true;
}

void SHRightTurn::addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random){
	if (orientation == Direction::NORTH || orientation == Direction::EAST) {
		generateSmallDoorChildRight((SHStartPiece*) startPiece, pieces, random, 1, 1);
	}
	else {
		generateSmallDoorChildLeft((SHStartPiece*) startPiece, pieces, random, 1, 1);
	}
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

SHLibrary::SHLibrary()
	: StrongholdPiece(){

}

SHLibrary::SHLibrary(int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: StrongholdPiece(genDepth){
	orientation = direction;
	entryDoor = randomSmallDoor(random);
	boundingBox = stairsBox;
	isTall = stairsBox.getYSpan() > height;
}

Unique<StrongholdPiece> SHLibrary::createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth){
	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, -4, -1, 0, width, tallHeight, depth, direction);

	if (!isOkBox(box) || StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		// make a short library
		box = BoundingBox::orientBox(footX, footY, footZ, -4, -1, 0, width, height, depth, direction);

		if (!isOkBox(box) || StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
			return nullptr;
		}
	}

	return make_unique<SHLibrary>(genDepth, random, box, direction);
}

bool SHLibrary::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB){
	if (CHECK_WATER_EDGE && edgesLiquid(level, chunkBB)) {
		return false;
	}

	int currentHeight = tallHeight;
	if (!isTall) {
		currentHeight = height;
	}

	// bounding walls
	generateBox(level, chunkBB, 0, 0, 0, width - 1, currentHeight - 1, depth - 1, CHECK_AIR, random, &smoothStoneSelector);
	// entry door
	generateSmallDoor(level, random, chunkBB, entryDoor, 4, 1, 0);

	// place sparse cob webs
	generateMaybeBox(level, chunkBB, random, .07f, 2, 1, 1, width - 1 - 2, height - 2, depth - 2, Block::mWeb->mID, Block::mWeb->mID, false);

	const int bookLeft = 1;
	const int bookRight = width - 2;

	// place library walls
	for (int d = 1; d <= depth - 2; d++) {
		if (((d - 1) % 4) == 0) {
			generateBox(level, chunkBB, bookLeft, 1, d, bookLeft, 4, d, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
			generateBox(level, chunkBB, bookRight, 1, d, bookRight, 4, d, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);

			placeBlock(level, Block::mTorch->mID, getTorchData(Direction::WEST), 2, 3, d, chunkBB);
			placeBlock(level, Block::mTorch->mID, getTorchData(Direction::EAST), width - 3, 3, d, chunkBB);

			if (isTall) {
				generateBox(level, chunkBB, bookLeft, 6, d, bookLeft, 9, d, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
				generateBox(level, chunkBB, bookRight, 6, d, bookRight, 9, d, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);

			}
		}
		else {
			generateBox(level, chunkBB, bookLeft, 1, d, bookLeft, 4, d, Block::mBookshelf->mID, Block::mBookshelf->mID, false);
			generateBox(level, chunkBB, bookRight, 1, d, bookRight, 4, d, Block::mBookshelf->mID, Block::mBookshelf->mID, false);

			if (isTall) {
				generateBox(level, chunkBB, bookLeft, 6, d, bookLeft, 9, d, Block::mBookshelf->mID, Block::mBookshelf->mID, false);
				generateBox(level, chunkBB, bookRight, 6, d, bookRight, 9, d, Block::mBookshelf->mID, Block::mBookshelf->mID, false);
			}
		}
	}

	// place book shelves
	for (int d = 3; d < depth - 3; d += 2) {
		generateBox(level, chunkBB, 3, 1, d, 4, 3, d, Block::mBookshelf->mID, Block::mBookshelf->mID, false);
		generateBox(level, chunkBB, 6, 1, d, 7, 3, d, Block::mBookshelf->mID, Block::mBookshelf->mID, false);
		generateBox(level, chunkBB, 9, 1, d, 10, 3, d, Block::mBookshelf->mID, Block::mBookshelf->mID, false);
	}

	if (isTall) {
		// create balcony
		generateBox(level, chunkBB, 1, 5, 1, 3, 5, depth - 2, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
		generateBox(level, chunkBB, width - 4, 5, 1, width - 2, 5, depth - 2, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
		generateBox(level, chunkBB, 4, 5, 1, width - 5, 5, 2, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
		generateBox(level, chunkBB, 4, 5, depth - 3, width - 5, 5, depth - 2, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);

		placeBlock(level, Block::mWoodPlanks->mID, 0, width - 5, 5, depth - 4, chunkBB);
		placeBlock(level, Block::mWoodPlanks->mID, 0, width - 6, 5, depth - 4, chunkBB);
		placeBlock(level, Block::mWoodPlanks->mID, 0, width - 5, 5, depth - 5, chunkBB);

		// balcony fences
		generateBox(level, chunkBB, 3, 6, 2, 3, 6, depth - 3, Block::mFence->mID, Block::mFence->mID, false);
		generateBox(level, chunkBB, width - 4, 6, 2, width - 4, 6, depth - 5, Block::mFence->mID, Block::mFence->mID, false);
		generateBox(level, chunkBB, 4, 6, 2, width - 5, 6, 2, Block::mFence->mID, Block::mFence->mID, false);
		generateBox(level, chunkBB, 4, 6, depth - 3, 8, 6, depth - 3, Block::mFence->mID, Block::mFence->mID, false);
		placeBlock(level, Block::mFence->mID, 0, width - 5, 6, depth - 4, chunkBB);
		placeBlock(level, Block::mFence->mID, 0, width - 6, 6, depth - 4, chunkBB);
		placeBlock(level, Block::mFence->mID, 0, width - 5, 6, depth - 5, chunkBB);

		// ladder
		int orientationData = getOrientationData(Block::mLadder, 3);
		placeBlock(level, Block::mLadder->mID, orientationData, width - 4, 1, depth - 2, chunkBB);
		placeBlock(level, Block::mLadder->mID, orientationData, width - 4, 2, depth - 2, chunkBB);
		placeBlock(level, Block::mLadder->mID, orientationData, width - 4, 3, depth - 2, chunkBB);
		placeBlock(level, Block::mLadder->mID, orientationData, width - 4, 4, depth - 2, chunkBB);
		placeBlock(level, Block::mLadder->mID, orientationData, width - 4, 5, depth - 2, chunkBB);
		placeBlock(level, Block::mLadder->mID, orientationData, width - 4, 6, depth - 2, chunkBB);
		placeBlock(level, Block::mLadder->mID, orientationData, width - 4, 7, depth - 2, chunkBB);

		// chandelier
		int x = width / 2;
		int z = depth / 2;
		placeBlock(level, Block::mFence->mID, 0, x - 1, tallHeight - 2, z, chunkBB);
		placeBlock(level, Block::mFence->mID, 0, x, tallHeight - 2, z, chunkBB);
		placeBlock(level, Block::mFence->mID, 0, x - 1, tallHeight - 3, z, chunkBB);
		placeBlock(level, Block::mFence->mID, 0, x, tallHeight - 3, z, chunkBB);
		placeBlock(level, Block::mFence->mID, 0, x - 1, tallHeight - 4, z, chunkBB);
		placeBlock(level, Block::mFence->mID, 0, x, tallHeight - 4, z, chunkBB);

		placeBlock(level, Block::mFence->mID, 0, x - 2, tallHeight - 4, z, chunkBB);
		placeBlock(level, Block::mFence->mID, 0, x + 1, tallHeight - 4, z, chunkBB);
		placeBlock(level, Block::mFence->mID, 0, x - 1, tallHeight - 4, z - 1, chunkBB);
		placeBlock(level, Block::mFence->mID, 0, x - 1, tallHeight - 4, z + 1, chunkBB);
		placeBlock(level, Block::mFence->mID, 0, x, tallHeight - 4, z - 1, chunkBB);
		placeBlock(level, Block::mFence->mID, 0, x, tallHeight - 4, z + 1, chunkBB);

		placeBlock(level, Block::mTorch->mID, getTorchData(Direction::UNDEFINED), x - 2, tallHeight - 3, z, chunkBB);
		placeBlock(level, Block::mTorch->mID, getTorchData(Direction::UNDEFINED), x + 1, tallHeight - 3, z, chunkBB);
		placeBlock(level, Block::mTorch->mID, getTorchData(Direction::UNDEFINED), x - 1, tallHeight - 3, z - 1, chunkBB);
		placeBlock(level, Block::mTorch->mID, getTorchData(Direction::UNDEFINED), x - 1, tallHeight - 3, z + 1, chunkBB);
		placeBlock(level, Block::mTorch->mID, getTorchData(Direction::UNDEFINED), x, tallHeight - 3, z - 1, chunkBB);
		placeBlock(level, Block::mTorch->mID, getTorchData(Direction::UNDEFINED), x, tallHeight - 3, z + 1, chunkBB);
	}

	// place chests
	if (chunkBB.isInside(getWorldX(3, 5), getWorldY(3), getWorldZ(3, 5))) {
		createChest(level, chunkBB, random, 3, 3, 5, Direction::WEST, "loot_tables/chests/stronghold_library.json");
	}
	if (isTall && chunkBB.isInside(getWorldX(width - 2, 1), getWorldY(tallHeight - 3), getWorldZ(width - 2, 1))) {
		createChest(level, chunkBB, random, width - 2, tallHeight - 3, 1, Direction::NORTH, "loot_tables/chests/stronghold_library.json");
	}

	return true;
}

void SHLibrary::addAdditionalSaveData(CompoundTag& tag) const {
	StrongholdPiece::addAdditionalSaveData(tag);
	tag.putBoolean("Tall", isTall);
}

void SHLibrary::readAdditionalSaveData(const CompoundTag& tag) {
	StrongholdPiece::readAdditionalSaveData(tag);
	isTall = tag.getBoolean("Tall");
}

void SHLibrary::addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random){

}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

SHPortalRoom::SHPortalRoom()
	: StrongholdPiece(){

}

SHPortalRoom::SHPortalRoom(int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: StrongholdPiece(genDepth){
	orientation = direction;
	boundingBox = stairsBox;
	hasPlacedMobSpawner = false;
}

Unique<StrongholdPiece> SHPortalRoom::createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth){
	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, -4, -1, 0, width, height, depth, direction);

	if (!isOkBox(box) || StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<SHPortalRoom>(genDepth, random, box, direction);
}

bool SHPortalRoom::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB){
	// bounding walls
	generateBox(level, chunkBB, 0, 0, 0, width - 1, height - 1, depth - 1, false, random, &smoothStoneSelector);
	// entry door
	generateSmallDoor(level, random, chunkBB, GRATES, 4, 1, 0);

	// inner roof row
	int y = height - 2;
	generateBox(level, chunkBB, 1, y, 1, 1, y, depth - 2, false, random, &smoothStoneSelector);
	generateBox(level, chunkBB, width - 2, y, 1, width - 2, y, depth - 2, false, random, &smoothStoneSelector);
	generateBox(level, chunkBB, 2, y, 1, width - 3, y, 2, false, random, &smoothStoneSelector);
	generateBox(level, chunkBB, 2, y, depth - 2, width - 3, y, depth - 2, false, random, &smoothStoneSelector);

	// entrance lava pools
	generateBox(level, chunkBB, 1, 1, 1, 2, 1, 4, false, random, &smoothStoneSelector);
	generateBox(level, chunkBB, width - 3, 1, 1, width - 2, 1, 4, false, random, &smoothStoneSelector);
	generateBox(level, chunkBB, 1, 1, 1, 1, 1, 3, Block::mStillLava->mID, Block::mStillLava->mID, false);
	generateBox(level, chunkBB, width - 2, 1, 1, width - 2, 1, 3, Block::mStillLava->mID, Block::mStillLava->mID, false);

	// portal lava pool
	generateBox(level, chunkBB, 3, 1, 8, 7, 1, 12, false, random, &smoothStoneSelector);
	generateBox(level, chunkBB, 4, 1, 9, 6, 1, 11, Block::mStillLava->mID, Block::mStillLava->mID, false);

	// wall decorations
	for (int z = 3; z < depth - 2; z += 2) {
		generateBox(level, chunkBB, 0, 3, z, 0, 4, z, Block::mIronFence->mID, Block::mIronFence->mID, false);
		generateBox(level, chunkBB, width - 1, 3, z, width - 1, 4, z, Block::mIronFence->mID, Block::mIronFence->mID, false);
	}

	for (int x = 2; x < width - 2; x += 2) {
		generateBox(level, chunkBB, x, 3, depth - 1, x, 4, depth - 1, Block::mIronFence->mID, Block::mIronFence->mID, false);
	}

	// stair
	int orientationData = getOrientationData(Block::mStoneBrickStairs, 3);
	generateBox(level, chunkBB, 4, 1, 5, 6, 1, 7, false, random, &smoothStoneSelector);
	generateBox(level, chunkBB, 4, 2, 6, 6, 2, 7, false, random, &smoothStoneSelector);
	generateBox(level, chunkBB, 4, 3, 7, 6, 3, 7, false, random, &smoothStoneSelector);

	for (int x = 4; x <= 6; x++) {
		placeBlock(level, Block::mStoneBrickStairs->mID, orientationData, x, 1, 4, chunkBB);
		placeBlock(level, Block::mStoneBrickStairs->mID, orientationData, x, 2, 5, chunkBB);
		placeBlock(level, Block::mStoneBrickStairs->mID, orientationData, x, 3, 6, chunkBB);
	}

	int north = Direction::NORTH;
	int south = Direction::SOUTH;
	int east = Direction::EAST;
	int west = Direction::WEST;

	switch (orientation) {
	case Direction::SOUTH:
		north = Direction::SOUTH;
		south = Direction::NORTH;
		break;
	case Direction::EAST:
		north = Direction::EAST;
		south = Direction::WEST;
		east = Direction::SOUTH;
		west = Direction::NORTH;
		break;
	case Direction::WEST:
		north = Direction::WEST;
		south = Direction::EAST;
		east = Direction::SOUTH;
		west = Direction::NORTH;
		break;
	}

	bool allEyes = true;
	bool eyes[12];
	for (int i = 0; i < 12; ++i) {
		eyes[i] = random.nextFloat() > 0.9f;
		allEyes &= eyes[i];
	}

	placeBlock(level, Block::mEndPortalFrame->mID, north + (eyes[0] ? EndPortalFrameBlock::EYE_BIT : 0), 4, 3, 8, chunkBB);
	placeBlock(level, Block::mEndPortalFrame->mID, north + (eyes[1] ? EndPortalFrameBlock::EYE_BIT : 0), 5, 3, 8, chunkBB);
	placeBlock(level, Block::mEndPortalFrame->mID, north + (eyes[2] ? EndPortalFrameBlock::EYE_BIT : 0), 6, 3, 8, chunkBB);
	placeBlock(level, Block::mEndPortalFrame->mID, south + (eyes[3] ? EndPortalFrameBlock::EYE_BIT : 0), 4, 3, 12, chunkBB);
	placeBlock(level, Block::mEndPortalFrame->mID, south + (eyes[4] ? EndPortalFrameBlock::EYE_BIT : 0), 5, 3, 12, chunkBB);
	placeBlock(level, Block::mEndPortalFrame->mID, south + (eyes[5] ? EndPortalFrameBlock::EYE_BIT : 0), 6, 3, 12, chunkBB);
	placeBlock(level, Block::mEndPortalFrame->mID, east + (eyes[6] ? EndPortalFrameBlock::EYE_BIT : 0), 3, 3, 9, chunkBB);
	placeBlock(level, Block::mEndPortalFrame->mID, east + (eyes[7] ? EndPortalFrameBlock::EYE_BIT : 0), 3, 3, 10, chunkBB);
	placeBlock(level, Block::mEndPortalFrame->mID, east + (eyes[8] ? EndPortalFrameBlock::EYE_BIT : 0), 3, 3, 11, chunkBB);
	placeBlock(level, Block::mEndPortalFrame->mID, west + (eyes[9] ? EndPortalFrameBlock::EYE_BIT : 0), 7, 3, 9, chunkBB);
	placeBlock(level, Block::mEndPortalFrame->mID, west + (eyes[10] ? EndPortalFrameBlock::EYE_BIT : 0), 7, 3, 10, chunkBB);
	placeBlock(level, Block::mEndPortalFrame->mID, west + (eyes[11] ? EndPortalFrameBlock::EYE_BIT : 0), 7, 3, 11, chunkBB);

	if (allEyes) {
		placeBlock(level, Block::mEndPortal->mID, 4, 3, 9, chunkBB);
		placeBlock(level, Block::mEndPortal->mID, 5, 3, 9, chunkBB);
		placeBlock(level, Block::mEndPortal->mID, 6, 3, 9, chunkBB);
		placeBlock(level, Block::mEndPortal->mID, 4, 3, 10, chunkBB);
		placeBlock(level, Block::mEndPortal->mID, 5, 3, 10, chunkBB);
		placeBlock(level, Block::mEndPortal->mID, 6, 3, 10, chunkBB);
		placeBlock(level, Block::mEndPortal->mID, 4, 3, 11, chunkBB);
		placeBlock(level, Block::mEndPortal->mID, 5, 3, 11, chunkBB);
		placeBlock(level, Block::mEndPortal->mID, 6, 3, 11, chunkBB);
	}

	if (!hasPlacedMobSpawner) {
		y = getWorldY(3);
		int x = getWorldX(5, 6), z = getWorldZ(5, 6);
		if (chunkBB.isInside(x, y, z)) {
			hasPlacedMobSpawner = true;
			level->setBlockAndData(x, y, z, Block::mMobSpawner->mID, Block::UPDATE_NONE);
// 			MobSpawnerBlockEntity* entity = (MobSpawnerBlockEntity*) level->getBlockEntity(x, y, z);
// 			if (entity != nullptr) {
// 				entity->getSpawner().setEntityId(EntityType::Silverfish);
// 			}
		}
	}

	return true;
}

void SHPortalRoom::addAdditionalSaveData(CompoundTag& tag) const {
	StrongholdPiece::addAdditionalSaveData(tag);
	tag.putBoolean("Mob", hasPlacedMobSpawner);
}

void SHPortalRoom::readAdditionalSaveData(const CompoundTag& tag) {
	StrongholdPiece::readAdditionalSaveData(tag);
	hasPlacedMobSpawner = tag.getBoolean("Mob");
}

void SHPortalRoom::addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random){
	if (startPiece != nullptr) {
		((SHStartPiece*) startPiece)->portalRoom = this;
	}
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

SHPrisonHall::SHPrisonHall()
	: StrongholdPiece(){

}

SHPrisonHall::SHPrisonHall(int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: StrongholdPiece(genDepth){
	orientation = direction;
	entryDoor = randomSmallDoor(random);
	boundingBox = stairsBox;
}

Unique<StrongholdPiece> SHPrisonHall::createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth){
	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, -1, -1, 0, width, height, depth, direction);

	if (!isOkBox(box) || StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<SHPrisonHall>(genDepth, random, box, direction);
}

bool SHPrisonHall::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB){
	if (CHECK_WATER_EDGE && edgesLiquid(level, chunkBB)) {
		return false;
	}

	// bounding walls
	generateBox(level, chunkBB, 0, 0, 0, width - 1, height - 1, depth - 1, CHECK_AIR, random, &smoothStoneSelector);
	// entry door
	generateSmallDoor(level, random, chunkBB, entryDoor, 1, 1, 0);
	// exit openings
	generateBox(level, chunkBB, 1, 1, depth - 1, 3, 3, depth - 1, BlockID::AIR, BlockID::AIR, false);

	// door pillars
	generateBox(level, chunkBB, 4, 1, 1, 4, 3, 1, false, random, &smoothStoneSelector);
	generateBox(level, chunkBB, 4, 1, 3, 4, 3, 3, false, random, &smoothStoneSelector);
	generateBox(level, chunkBB, 4, 1, 7, 4, 3, 7, false, random, &smoothStoneSelector);
	generateBox(level, chunkBB, 4, 1, 9, 4, 3, 9, false, random, &smoothStoneSelector);

	// grates
	generateBox(level, chunkBB, 4, 1, 4, 4, 3, 6, Block::mIronFence->mID, Block::mIronFence->mID, false);
	generateBox(level, chunkBB, 5, 1, 5, 7, 3, 5, Block::mIronFence->mID, Block::mIronFence->mID, false);

	// doors
	placeBlock(level, Block::mIronFence->mID, 0, 4, 3, 2, chunkBB);
	placeBlock(level, Block::mIronFence->mID, 0, 4, 3, 8, chunkBB);
	//placeBlock(level, Blocks.IRON_DOOR, getOrientationData(Blocks.IRON_DOOR, 3), 4, 1, 2, chunkBB);
	//placeBlock(level, Blocks.IRON_DOOR, getOrientationData(Blocks.IRON_DOOR, 3) + DoorBlock.UPPER_BIT, 4, 2, 2,
	// chunkBB);
	//placeBlock(level, Blocks.IRON_DOOR, getOrientationData(Blocks.IRON_DOOR, 3), 4, 1, 8, chunkBB);
	//placeBlock(level, Blocks.IRON_DOOR, getOrientationData(Blocks.IRON_DOOR, 3) + DoorBlock.UPPER_BIT, 4, 2, 8,
	// chunkBB);

	return true;
}

void SHPrisonHall::addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random){
	generateSmallDoorChildForward((SHStartPiece*) startPiece, pieces, random, 1, 1);
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

SHRoomCrossing::SHRoomCrossing()
	: StrongholdPiece(){

}

SHRoomCrossing::SHRoomCrossing(int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: StrongholdPiece(genDepth){
	orientation = direction;
	entryDoor = randomSmallDoor(random);
	boundingBox = stairsBox;
	type = random.nextInt(5);
}

Unique<StrongholdPiece> SHRoomCrossing::createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth){
	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, -4, -1, 0, width, height, depth, direction);

	if (!isOkBox(box) || StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<SHRoomCrossing>(genDepth, random, box, direction);
}

bool SHRoomCrossing::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB){
	if (CHECK_WATER_EDGE && edgesLiquid(level, chunkBB)) {
		return false;
	}

	// bounding walls
	generateBox(level, chunkBB, 0, 0, 0, width - 1, height - 1, depth - 1, CHECK_AIR, random, &smoothStoneSelector);
	// entry door
	generateSmallDoor(level, random, chunkBB, entryDoor, 4, 1, 0);
	// exit openings
	generateBox(level, chunkBB, 4, 1, depth - 1, 6, 3, depth - 1, BlockID::AIR, BlockID::AIR, false);
	generateBox(level, chunkBB, 0, 1, 4, 0, 3, 6, BlockID::AIR, BlockID::AIR, false);
	generateBox(level, chunkBB, width - 1, 1, 4, width - 1, 3, 6, BlockID::AIR, BlockID::AIR, false);

	switch (type) {
	default:
		break;
	case 0:
		// middle torch pillar
		placeBlock(level, Block::mStoneBrick->mID, 0, 5, 1, 5, chunkBB);
		placeBlock(level, Block::mStoneBrick->mID, 0, 5, 2, 5, chunkBB);
		placeBlock(level, Block::mStoneBrick->mID, 0, 5, 3, 5, chunkBB);
		placeBlock(level, Block::mTorch->mID, getTorchData(Direction::EAST), 4, 3, 5, chunkBB);
		placeBlock(level, Block::mTorch->mID, getTorchData(Direction::WEST), 6, 3, 5, chunkBB);
		placeBlock(level, Block::mTorch->mID, getTorchData(Direction::NORTH), 5, 3, 4, chunkBB);
		placeBlock(level, Block::mTorch->mID, getTorchData(Direction::SOUTH), 5, 3, 6, chunkBB);
		placeBlock(level, Block::mStoneSlab->mID, 0, 4, 1, 4, chunkBB);
		placeBlock(level, Block::mStoneSlab->mID, 0, 4, 1, 5, chunkBB);
		placeBlock(level, Block::mStoneSlab->mID, 0, 4, 1, 6, chunkBB);
		placeBlock(level, Block::mStoneSlab->mID, 0, 6, 1, 4, chunkBB);
		placeBlock(level, Block::mStoneSlab->mID, 0, 6, 1, 5, chunkBB);
		placeBlock(level, Block::mStoneSlab->mID, 0, 6, 1, 6, chunkBB);
		placeBlock(level, Block::mStoneSlab->mID, 0, 5, 1, 4, chunkBB);
		placeBlock(level, Block::mStoneSlab->mID, 0, 5, 1, 6, chunkBB);
		break;
	case 1:

		for (int i = 0; i < 5; i++) {
			placeBlock(level, Block::mStoneBrick->mID, 0, 3, 1, 3 + i, chunkBB);
			placeBlock(level, Block::mStoneBrick->mID, 0, 7, 1, 3 + i, chunkBB);
			placeBlock(level, Block::mStoneBrick->mID, 0, 3 + i, 1, 3, chunkBB);
			placeBlock(level, Block::mStoneBrick->mID, 0, 3 + i, 1, 7, chunkBB);
		}
		placeBlock(level, Block::mStoneBrick->mID, 0, 5, 1, 5, chunkBB);
		placeBlock(level, Block::mStoneBrick->mID, 0, 5, 2, 5, chunkBB);
		placeBlock(level, Block::mStoneBrick->mID, 0, 5, 3, 5, chunkBB);
		placeBlock(level, Block::mFlowingWater->mID, 0, 5, 4, 5, chunkBB);
		break;
	case 2:

		for (int z = 1; z <= 9; z++) {
			placeBlock(level, Block::mCobblestone->mID, 0, 1, 3, z, chunkBB);
			placeBlock(level, Block::mCobblestone->mID, 0, 9, 3, z, chunkBB);
		}

		for (int x = 1; x <= 9; x++) {
			placeBlock(level, Block::mCobblestone->mID, 0, x, 3, 1, chunkBB);
			placeBlock(level, Block::mCobblestone->mID, 0, x, 3, 9, chunkBB);
		}
		placeBlock(level, Block::mCobblestone->mID, 0, 5, 1, 4, chunkBB);
		placeBlock(level, Block::mCobblestone->mID, 0, 5, 1, 6, chunkBB);
		placeBlock(level, Block::mCobblestone->mID, 0, 5, 3, 4, chunkBB);
		placeBlock(level, Block::mCobblestone->mID, 0, 5, 3, 6, chunkBB);
		placeBlock(level, Block::mCobblestone->mID, 0, 4, 1, 5, chunkBB);
		placeBlock(level, Block::mCobblestone->mID, 0, 6, 1, 5, chunkBB);
		placeBlock(level, Block::mCobblestone->mID, 0, 4, 3, 5, chunkBB);
		placeBlock(level, Block::mCobblestone->mID, 0, 6, 3, 5, chunkBB);

		for (int y = 1; y <= 3; y++) {
			placeBlock(level, Block::mCobblestone->mID, 0, 4, y, 4, chunkBB);
			placeBlock(level, Block::mCobblestone->mID, 0, 6, y, 4, chunkBB);
			placeBlock(level, Block::mCobblestone->mID, 0, 4, y, 6, chunkBB);
			placeBlock(level, Block::mCobblestone->mID, 0, 6, y, 6, chunkBB);
		}
		placeBlock(level, Block::mTorch->mID, 0, 5, 3, 5, chunkBB);

		for (int z = 2; z <= 8; z++) {
			placeBlock(level, Block::mWoodPlanks->mID, 0, 2, 3, z, chunkBB);
			placeBlock(level, Block::mWoodPlanks->mID, 0, 3, 3, z, chunkBB);
			if (z <= 3 || z >= 7) {
				placeBlock(level, Block::mWoodPlanks->mID, 0, 4, 3, z, chunkBB);
				placeBlock(level, Block::mWoodPlanks->mID, 0, 5, 3, z, chunkBB);
				placeBlock(level, Block::mWoodPlanks->mID, 0, 6, 3, z, chunkBB);
			}
			placeBlock(level, Block::mWoodPlanks->mID, 0, 7, 3, z, chunkBB);
			placeBlock(level, Block::mWoodPlanks->mID, 0, 8, 3, z, chunkBB);
		}
		placeBlock(level, Block::mLadder->mID, getOrientationData(Block::mLadder, Facing::WEST), 9, 1, 3, chunkBB);
		placeBlock(level, Block::mLadder->mID, getOrientationData(Block::mLadder, Facing::WEST), 9, 2, 3, chunkBB);
		placeBlock(level, Block::mLadder->mID, getOrientationData(Block::mLadder, Facing::WEST), 9, 3, 3, chunkBB);

		createChest(level, chunkBB, random, 3, 4, 8, Direction::WEST, "loot_tables/chests/stronghold_crossing.json");

		break;
	}

	return true;
}

void SHRoomCrossing::addAdditionalSaveData(CompoundTag& tag) const {
	StrongholdPiece::addAdditionalSaveData(tag);
	tag.putInt("Type", type);
}

void SHRoomCrossing::readAdditionalSaveData(const CompoundTag& tag) {
	StrongholdPiece::readAdditionalSaveData(tag);
	type = tag.getInt("Type");
}

void SHRoomCrossing::addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random){
	generateSmallDoorChildForward((SHStartPiece*) startPiece, pieces, random, 4, 1);
	generateSmallDoorChildLeft((SHStartPiece*) startPiece, pieces, random, 1, 4);
	generateSmallDoorChildRight((SHStartPiece*) startPiece, pieces, random, 1, 4);
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

SHStraight::SHStraight()
	: StrongholdPiece(){

}

SHStraight::SHStraight(int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: StrongholdPiece(genDepth){
	orientation = direction;
	entryDoor = randomSmallDoor(random);
	boundingBox = stairsBox;

	leftChild = random.nextInt(2) == 0;
	rightChild = random.nextInt(2) == 0;
}

Unique<StrongholdPiece> SHStraight::createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth){
	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, -1, -1, 0, width, height, depth, direction);

	if (!isOkBox(box) || StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<SHStraight>(genDepth, random, box, direction);
}

bool SHStraight::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB){
	if (CHECK_WATER_EDGE && edgesLiquid(level, chunkBB)) {
		return false;
	}

	// bounding walls
	generateBox(level, chunkBB, 0, 0, 0, width - 1, height - 1, depth - 1, CHECK_AIR, random, &smoothStoneSelector);
	// entry door
	generateSmallDoor(level, random, chunkBB, entryDoor, 1, height - SMALL_DOOR_HEIGHT - 1, 0);
	// exit door
	generateSmallDoor(level, random, chunkBB, OPENING, 1, 1, depth - 1);

	maybeGenerateBlock(level, chunkBB, random, .1f, 1, 2, 1, Block::mTorch->mID, getTorchData(Direction::WEST));
	maybeGenerateBlock(level, chunkBB, random, .1f, 3, 2, 1, Block::mTorch->mID, getTorchData(Direction::EAST));
	maybeGenerateBlock(level, chunkBB, random, .1f, 1, 2, 5, Block::mTorch->mID, getTorchData(Direction::WEST));
	maybeGenerateBlock(level, chunkBB, random, .1f, 3, 2, 5, Block::mTorch->mID, getTorchData(Direction::EAST));

	if (leftChild) {
		generateBox(level, chunkBB, 0, 1, 2, 0, 3, 4, BlockID::AIR, BlockID::AIR, false);
	}
	if (rightChild) {
		generateBox(level, chunkBB, 4, 1, 2, 4, 3, 4, BlockID::AIR, BlockID::AIR, false);
	}

	return true;
}

void SHStraight::addAdditionalSaveData(CompoundTag& tag) const {
	StrongholdPiece::addAdditionalSaveData(tag);
	tag.putBoolean("Left", leftChild);
	tag.putBoolean("Right", rightChild);
}

void SHStraight::readAdditionalSaveData(const CompoundTag& tag) {
	StrongholdPiece::readAdditionalSaveData(tag);
	leftChild = tag.getBoolean("Left");
	rightChild = tag.getBoolean("Right");
}

void SHStraight::addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random){
	generateSmallDoorChildForward((SHStartPiece*) startPiece, pieces, random, 1, 1);
	if (leftChild) {
		generateSmallDoorChildLeft((SHStartPiece*) startPiece, pieces, random, 1, 2);
	}
	if (rightChild) {
		generateSmallDoorChildRight((SHStartPiece*) startPiece, pieces, random, 1, 2);
	}
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

SHStraightStairsDown::SHStraightStairsDown()
	: StrongholdPiece(){

}

SHStraightStairsDown::SHStraightStairsDown(int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: StrongholdPiece(genDepth){
	orientation = direction;
	entryDoor = randomSmallDoor(random);
	boundingBox = stairsBox;
}

Unique<StrongholdPiece> SHStraightStairsDown::createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth){
	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, -1, 4 - height, 0, width, height, depth, direction);

	if (!isOkBox(box) || StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<SHStraightStairsDown>(genDepth, random, box, direction);
}

bool SHStraightStairsDown::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB){
	if (CHECK_WATER_EDGE && edgesLiquid(level, chunkBB)) {
		return false;
	}

	// bounding walls
	generateBox(level, chunkBB, 0, 0, 0, width - 1, height - 1, depth - 1, CHECK_AIR, random, &smoothStoneSelector);
	// entry door
	generateSmallDoor(level, random, chunkBB, entryDoor, 1, height - SMALL_DOOR_HEIGHT - 1, 0);
	// exit door
	generateSmallDoor(level, random, chunkBB, OPENING, 1, 1, depth - 1);

	// stairs
	int orientationData = getOrientationData(Block::mStoneStairs, 2);

	for (int i = 0; i < 6; i++) {
		placeBlock(level, Block::mStoneStairs->mID, orientationData, 1, height - 5 - i, 1 + i, chunkBB);
		placeBlock(level, Block::mStoneStairs->mID, orientationData, 2, height - 5 - i, 1 + i, chunkBB);
		placeBlock(level, Block::mStoneStairs->mID, orientationData, 3, height - 5 - i, 1 + i, chunkBB);
		if (i < 5) {
			placeBlock(level, Block::mStoneBrick->mID, 0, 1, height - 6 - i, 1 + i, chunkBB);
			placeBlock(level, Block::mStoneBrick->mID, 0, 2, height - 6 - i, 1 + i, chunkBB);
			placeBlock(level, Block::mStoneBrick->mID, 0, 3, height - 6 - i, 1 + i, chunkBB);
		}
	}

	return true;
}

void SHStraightStairsDown::addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random){
	generateSmallDoorChildForward((SHStartPiece*) startPiece, pieces, random, 1, 1);
}
