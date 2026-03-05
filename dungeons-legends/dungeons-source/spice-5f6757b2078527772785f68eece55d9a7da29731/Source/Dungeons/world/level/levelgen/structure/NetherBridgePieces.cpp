#include "Dungeons.h"

#include "world/level/levelgen/structure/NetherBridgePieces.h"

#include "nbt/CompoundTag.h"
#include "world/entity/Entity.h"
#include "world/level/chunk/LevelChunk.h"
#include "world/level/Level.h"
#include "world/level/BlockSource.h"
#include "world/item/ItemInstance.h"
#include "world/level/block/Block.h"
// #include "world/level/block/entity/MobSpawnerBlockEntity.h"
#include "world/item/Item.h"

static const int MAX_DEPTH = 30;
// the dungeon starts at 64 and traverses downwards to this point
static const int LOWEST_Y_POSITION = 10;

NetherBridgePiece::NetherBridgePiece() :
	StructurePiece(0){
}

NetherBridgePiece::NetherBridgePiece(int genDepth) :
	StructurePiece(genDepth){
}

int NetherBridgePiece::updatePieceWeight(PieceWeightList& currentPieces) {
	bool hasAnyPieces = false;
	int totalWeight = 0;

	for (auto&& piece : currentPieces) {
		if (piece.maxPlaceCount > 0 && piece.placeCount < piece.maxPlaceCount) {
			hasAnyPieces = true;
		}
		totalWeight += piece.weight;
	}
	return hasAnyPieces ? totalWeight : -1;
}

Unique<NetherBridgePiece> NetherBridgePiece::findAndCreateBridgePieceFactory(const std::string& pieceClass, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int depth) {

	if (pieceClass == "BridgeStraight") {
		return NBBridgeStraight::createPiece(pieces, random, footX, footY, footZ, direction, depth);
	}
	else if (pieceClass == "BridgeCrossing") {
		return NBBridgeCrossing::createPiece(pieces, random, footX, footY, footZ, direction, depth);
	}
	else if (pieceClass == "RoomCrossing") {
		return NBRoomCrossing::createPiece(pieces, random, footX, footY, footZ, direction, depth);
	}
	else if (pieceClass == "StairsRoom") {
		return NBStairsRoom::createPiece(pieces, random, footX, footY, footZ, direction, depth);
	}
	else if (pieceClass == "MonsterThrone") {
		return NBMonsterThrone::createPiece(pieces, random, footX, footY, footZ, direction, depth);
	}
	else if (pieceClass == "CastleEntrance") {
		return NBCastleEntrance::createPiece(pieces, random, footX, footY, footZ, direction, depth);
	}
	else if (pieceClass == "CastleSmallCorridorPiece") {
		return NBCastleSmallCorridorPiece::createPiece(pieces, random, footX, footY, footZ, direction, depth);
	}
	else if (pieceClass == "CastleSmallCorridorRightTurnPiece") {
		return NBCastleSmallCorridorRightTurnPiece::createPiece(pieces, random, footX, footY, footZ, direction, depth);
	}
	else if (pieceClass == "CastleSmallCorridorLeftTurnPiece") {
		return NBCastleSmallCorridorLeftTurnPiece::createPiece(pieces, random, footX, footY, footZ, direction, depth);
	}
	else if (pieceClass == "CastleCorridorStairsPiece") {
		return NBCastleCorridorStairsPiece::createPiece(pieces, random, footX, footY, footZ, direction, depth);
	}
	else if (pieceClass == "CastleCorridorTBalconyPiece") {
		return NBCastleCorridorTBalconyPiece::createPiece(pieces, random, footX, footY, footZ, direction, depth);
	}
	else if (pieceClass == "CastleSmallCorridorCrossingPiece") {
		return NBCastleSmallCorridorCrossingPiece::createPiece(pieces, random, footX, footY, footZ, direction, depth);
	}
	else if (pieceClass == "CastleStalkRoom") {
		return NBCastleStalkRoom::createPiece(pieces, random, footX, footY, footZ, direction, depth);
	}
	return nullptr;
}

Unique<NetherBridgePiece> NetherBridgePiece::generatePiece(NBStartPiece* startPiece, PieceWeightList& currentPieces, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int depth) {

	int totalWeight = updatePieceWeight(currentPieces);
	bool doStuff = totalWeight > 0 && depth <= MAX_DEPTH;

	int numAttempts = 0;

	while (numAttempts < 5 && doStuff) {
		numAttempts++;

		int weightSelection = random.nextInt(totalWeight);
		PieceWeightList::iterator pieceIt = currentPieces.begin();

		while (pieceIt != currentPieces.end()) {
			weightSelection -= pieceIt->weight;
			if (weightSelection < 0) {

				if (!pieceIt->doPlace(depth) || (pieceIt->pieceClass == startPiece->previousPiece && !pieceIt->allowInRow)) {
					break;
				}

				auto structurePiece = findAndCreateBridgePieceFactory(pieceIt->pieceClass, pieces, random, footX, footY, footZ, direction, depth);
				if (structurePiece != nullptr) {
					pieceIt->placeCount++;
					startPiece->previousPiece = pieceIt->pieceClass;

					if (!pieceIt->isValid()) {
						currentPieces.erase(pieceIt);
					}
					return structurePiece;
				}
			}
			++pieceIt;
		}
	}
	return NBBridgeEndFiller::createPiece(pieces, random, footX, footY, footZ, direction, depth);
}

StructurePiece* NetherBridgePiece::generateAndAddPiece(NBStartPiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int depth, bool isCastle) {
	if (std::abs(footX - startPiece->getBoundingBox().x0) > 7 * 16 || std::abs(footZ - startPiece->getBoundingBox().z0) > 7 * 16) {
		auto newPiece = NBBridgeEndFiller::createPiece(pieces, random, footX, footY, footZ, direction, depth);
		if (newPiece != nullptr) {
			auto ptr = newPiece.get();
			pieces.emplace_back(std::move(newPiece));
			startPiece->pendingChildren.push_back(ptr);
			return ptr;
		}
		return nullptr;
	}
	PieceWeightList* availablePieces = &startPiece->availableBridgePieces;
	if (isCastle) {
		availablePieces = &startPiece->availableCastlePieces;
	}
	auto newPiece = generatePiece(startPiece, *availablePieces, pieces, random, footX, footY, footZ, direction, depth + 1);
	if (newPiece != nullptr) {
		auto ptr = newPiece.get();		
		pieces.emplace_back(std::move(newPiece));
		startPiece->pendingChildren.push_back(ptr);
		return ptr;
	}
	return nullptr;
}

StructurePiece* NetherBridgePiece::generateChildForward(NBStartPiece* startPiece, PieceList& pieces, Random& random, int xOff, int yOff, bool isCastle) {
	switch (orientation) {
	case Direction::NORTH:
		return generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 + xOff, boundingBox.y0 + yOff, boundingBox.z0 - 1, orientation, getGenDepth(), isCastle);
	case Direction::SOUTH:
		return generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 + xOff, boundingBox.y0 + yOff, boundingBox.z1 + 1, orientation, getGenDepth(), isCastle);
	case Direction::WEST:
		return generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 - 1, boundingBox.y0 + yOff, boundingBox.z0 + xOff, orientation, getGenDepth(), isCastle);
	case Direction::EAST:
		return generateAndAddPiece(startPiece, pieces, random, boundingBox.x1 + 1, boundingBox.y0 + yOff, boundingBox.z0 + xOff, orientation, getGenDepth(), isCastle);
	}
	return nullptr;
}

StructurePiece* NetherBridgePiece::generateChildLeft(NBStartPiece* startPiece, PieceList& pieces, Random& random, int yOff, int zOff, bool isCastle) {
	switch (orientation) {
	case Direction::NORTH:
		return generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 - 1, boundingBox.y0 + yOff, boundingBox.z0 + zOff, Direction::WEST, getGenDepth(), isCastle);
	case Direction::SOUTH:
		return generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 - 1, boundingBox.y0 + yOff, boundingBox.z0 + zOff, Direction::WEST, getGenDepth(), isCastle);
	case Direction::WEST:
		return generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 + zOff, boundingBox.y0 + yOff, boundingBox.z0 - 1, Direction::NORTH, getGenDepth(), isCastle);
	case Direction::EAST:
		return generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 + zOff, boundingBox.y0 + yOff, boundingBox.z0 - 1, Direction::NORTH, getGenDepth(), isCastle);
	}
	return nullptr;
}

StructurePiece* NetherBridgePiece::generateChildRight(NBStartPiece* startPiece, PieceList& pieces, Random& random, int yOff, int zOff, bool isCastle) {
	switch (orientation) {
	case Direction::NORTH:
		return generateAndAddPiece(startPiece, pieces, random, boundingBox.x1 + 1, boundingBox.y0 + yOff, boundingBox.z0 + zOff, Direction::EAST, getGenDepth(), isCastle);
	case Direction::SOUTH:
		return generateAndAddPiece(startPiece, pieces, random, boundingBox.x1 + 1, boundingBox.y0 + yOff, boundingBox.z0 + zOff, Direction::EAST, getGenDepth(), isCastle);
	case Direction::WEST:
		return generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 + zOff, boundingBox.y0 + yOff, boundingBox.z1 + 1, Direction::SOUTH, getGenDepth(), isCastle);
	case Direction::EAST:
		return generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 + zOff, boundingBox.y0 + yOff, boundingBox.z1 + 1, Direction::SOUTH, getGenDepth(), isCastle);
	}
	return nullptr;
}

void NetherBridgePiece::generateLightPost(BlockSource* level, Random& random, const BoundingBox& chunkBB, int x, int y, int z, int xOff, int zOff) {

	BlockPos pos(getWorldX(x, z), getWorldY(y), getWorldZ(x, z));
	if (!chunkBB.isInside(pos)) {
		return;
	}

	BlockPos above = pos.above();
	BlockPos above2 = pos.above(2);
	BlockPos above3 = pos.above(3);

	if (level->isEmptyBlock(pos) && level->isEmptyBlock(above) && level->isEmptyBlock(above2) && level->isEmptyBlock(above3)) {
		level->setBlockAndData(pos, Block::mNetherFence->mID, 0, Block::UPDATE_CLIENTS);
		level->setBlockAndData(above, Block::mNetherFence->mID, 0, Block::UPDATE_CLIENTS);
		level->setBlockAndData(above2, Block::mNetherFence->mID, 0, Block::UPDATE_CLIENTS);
		level->setBlockAndData(above3, Block::mNetherFence->mID, 0, Block::UPDATE_CLIENTS);
		placeBlock(level, Block::mNetherFence->mID, 0, x + xOff, y + 3, z + zOff, chunkBB);
		placeBlock(level, Block::mGlowStone->mID, 0, x + xOff, y + 2, z + zOff, chunkBB);
	}
}

void NetherBridgePiece::generateLightPostFacingRight(BlockSource* level, Random& random, const BoundingBox& chunkBB, int x, int y, int z) {
	generateLightPost(level, random, chunkBB, x, y, z, 1, 0);
}

void NetherBridgePiece::generateLightPostFacingLeft(BlockSource* level, Random& random, const BoundingBox& chunkBB, int x, int y, int z) {
	generateLightPost(level, random, chunkBB, x, y, z, -1, 0);
}

void NetherBridgePiece::generateLightPostFacingUp(BlockSource* level, Random& random, const BoundingBox& chunkBB, int x, int y, int z) {
	generateLightPost(level, random, chunkBB, x, y, z, 0, 1);
}

void NetherBridgePiece::generateLightPostFacingDown(BlockSource* level, Random& random, const BoundingBox& chunkBB, int x, int y, int z) {
	generateLightPost(level, random, chunkBB, x, y, z, 0, -1);
}

bool NetherBridgePiece::isOkBox(BoundingBox box) {
	return box.y0 > LOWEST_Y_POSITION;
}

bool NetherBridgePiece::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	if (!level) {
		return false;
	}

	for (int i = chunkBB.x0; i <= chunkBB.x1; ++i) {
		for (int j = chunkBB.y0; j <= chunkBB.y1; ++j) {
			for (int k = chunkBB.z0; k <= chunkBB.z1; ++k) {
				const BlockPos pos(i, j, k);
				LevelChunk* chunk = level->getChunkAt(pos);
				if (chunk) {
					const BlockID id = chunk->getBlock(ChunkBlockPos(pos));
					if (id == Block::mStillLava->getId() || id == Block::mFlowingLava->getId()) {
						const Block* lavaBlock = Block::mBlocks[id];
						lavaBlock->neighborChanged(*level, pos, pos.above());
						lavaBlock->neighborChanged(*level, pos, pos.below());
						lavaBlock->neighborChanged(*level, pos, pos.east());
						lavaBlock->neighborChanged(*level, pos, pos.west());
						lavaBlock->neighborChanged(*level, pos, pos.north());
						lavaBlock->neighborChanged(*level, pos, pos.south());
					}
				}
			}
		}
	}

	return true;
}

void NetherBridgePiece::addAdditionalSaveData(CompoundTag& tag) const {
}

void NetherBridgePiece::readAdditionalSaveData(const CompoundTag& tag) {
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

NBBridgeCrossing::NBBridgeCrossing() :
	NetherBridgePiece(0){
}

NBBridgeCrossing::NBBridgeCrossing(int genDepth, Random& random, const BoundingBox& stairsBox, int direction) :
	NetherBridgePiece(genDepth){
	orientation = direction;
	boundingBox = stairsBox;
}

NBBridgeCrossing::NBBridgeCrossing(Random& random, int west, int north) :
	NetherBridgePiece(0){
	orientation = random.nextInt(4);

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

Unique<NetherBridgePiece> NBBridgeCrossing::createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth) {

	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, -8, -3, 0, width, height, depth, direction);

	if (!isOkBox(box) || StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<NBBridgeCrossing>(genDepth, random, box, direction);
}

void NBBridgeCrossing::addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) {

	generateChildForward((NBStartPiece*)startPiece, pieces, random, 8, 3, false);
	generateChildLeft((NBStartPiece*)startPiece, pieces, random, 3, 8, false);
	generateChildRight((NBStartPiece*)startPiece, pieces, random, 3, 8, false);
}

bool NBBridgeCrossing::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {

	// floor
	generateBox(level, chunkBB, 7, 3, 0, 11, 4, 18, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 0, 3, 7, 18, 4, 11, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	// room air
	generateBox(level, chunkBB, 8, 5, 0, 10, 7, 18, BlockID::AIR, BlockID::AIR, false);
	generateBox(level, chunkBB, 0, 5, 8, 18, 7, 10, BlockID::AIR, BlockID::AIR, false);
	// hand rails
	generateBox(level, chunkBB, 7, 5, 0, 7, 5, 7, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 7, 5, 11, 7, 5, 18, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 11, 5, 0, 11, 5, 7, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 11, 5, 11, 11, 5, 18, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 0, 5, 7, 7, 5, 7, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 11, 5, 7, 18, 5, 7, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 0, 5, 11, 7, 5, 11, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 11, 5, 11, 18, 5, 11, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);

	// supports
	generateBox(level, chunkBB, 7, 2, 0, 11, 2, 5, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 7, 2, 13, 11, 2, 18, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 7, 0, 0, 11, 1, 3, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 7, 0, 15, 11, 1, 18, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);

	for (int x = 7; x <= 11; x++) {
		for (int z = 0; z <= 2; z++) {
			fillColumnDown(level, FullBlock(Block::mNetherBrick->mID, 0), x, -1, z, chunkBB);
			fillColumnDown(level, FullBlock(Block::mNetherBrick->mID, 0), x, -1, 18 - z, chunkBB);
		}
	}

	generateBox(level, chunkBB, 0, 2, 7, 5, 2, 11, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 13, 2, 7, 18, 2, 11, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 0, 0, 7, 3, 1, 11, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 15, 0, 7, 18, 1, 11, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);

	for (int x = 0; x <= 2; x++) {
		for (int z = 7; z <= 11; z++) {
			fillColumnDown(level, FullBlock(Block::mNetherBrick->mID, 0), x, -1, z, chunkBB);
			fillColumnDown(level, FullBlock(Block::mNetherBrick->mID, 0), 18 - x, -1, z, chunkBB);
		}
	}

	return NetherBridgePiece::postProcess(level, random, chunkBB);
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

NBStartPiece::NBStartPiece() :
	NBBridgeCrossing(){
}

NBStartPiece::NBStartPiece(Random& random, int west, int north)
	: NBBridgeCrossing(random, west, north){
	availableBridgePieces.clear();
	{
		availableBridgePieces.push_back(PieceWeight("BridgeStraight", 30, 0, 0, true));
		availableBridgePieces.push_back(PieceWeight("BridgeCrossing", 10, 4));
		availableBridgePieces.push_back(PieceWeight("RoomCrossing", 10, 4));
		availableBridgePieces.push_back(PieceWeight("StairsRoom", 10, 3));
		availableBridgePieces.push_back(PieceWeight("MonsterThrone", 5, 2));
		availableBridgePieces.push_back(PieceWeight("CastleEntrance", 5, 1));
	}

	availableCastlePieces.clear();
	{
		availableCastlePieces.push_back(PieceWeight("CastleSmallCorridorPiece", 25, 0, 0, true));
		availableCastlePieces.push_back(PieceWeight("CastleSmallCorridorCrossingPiece", 15, 5));
		availableCastlePieces.push_back(PieceWeight("CastleSmallCorridorRightTurnPiece", 5, 10));
		availableCastlePieces.push_back(PieceWeight("CastleSmallCorridorLeftTurnPiece", 5, 10));
		availableCastlePieces.push_back(PieceWeight("CastleCorridorStairsPiece", 10, 3, 0, true));
		availableCastlePieces.push_back(PieceWeight("CastleCorridorTBalconyPiece", 7, 2));
		availableCastlePieces.push_back(PieceWeight("CastleStalkRoom", 5, 2));
	}
}

void NBStartPiece::addAdditionalSaveData(CompoundTag& tag) const {
	NBBridgeCrossing::addAdditionalSaveData(tag);
}

void NBStartPiece::readAdditionalSaveData(const CompoundTag& tag) {
	NBBridgeCrossing::readAdditionalSaveData(tag);
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

NBBridgeStraight::NBBridgeStraight()
	: NetherBridgePiece(){
}

NBBridgeStraight::NBBridgeStraight(int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: NetherBridgePiece(genDepth){
	orientation = direction;
	boundingBox = stairsBox;
}

Unique<NetherBridgePiece> NBBridgeStraight::createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth) {

	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, -1, -3, 0, width, height, depth, direction);

	if (!isOkBox(box) || StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<NBBridgeStraight>(genDepth, random, box, direction);
}

void NBBridgeStraight::addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) {

	generateChildForward((NBStartPiece*)startPiece, pieces, random, 1, 3, false);
}

bool NBBridgeStraight::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {

	// floor
	generateBox(level, chunkBB, 0, 3, 0, width - 1, 4, depth - 1, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	// room air
	generateBox(level, chunkBB, 1, 5, 0, 3, 7, depth - 1, BlockID::AIR, BlockID::AIR, false);

	// hand rails
	generateBox(level, chunkBB, 0, 5, 0, 0, 5, depth - 1, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 4, 5, 0, 4, 5, depth - 1, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);

	// supports
	generateBox(level, chunkBB, 0, 2, 0, 4, 2, 5, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 0, 2, 13, 4, 2, 18, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 0, 0, 0, 4, 1, 3, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 0, 0, 15, 4, 1, 18, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);

	for (int x = 0; x <= 4; x++) {
		for (int z = 0; z <= 2; z++) {
			fillColumnDown(level, FullBlock(Block::mNetherBrick->mID, 0), x, -1, z, chunkBB);
			fillColumnDown(level, FullBlock(Block::mNetherBrick->mID, 0), x, -1, 18 - z, chunkBB);
		}
	}

	generateBox(level, chunkBB, 0, 1, 1, 0, 4, 1, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
	generateBox(level, chunkBB, 0, 3, 4, 0, 4, 4, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
	generateBox(level, chunkBB, 0, 3, 14, 0, 4, 14, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
	generateBox(level, chunkBB, 0, 1, 17, 0, 4, 17, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
	generateBox(level, chunkBB, 4, 1, 1, 4, 4, 1, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
	generateBox(level, chunkBB, 4, 3, 4, 4, 4, 4, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
	generateBox(level, chunkBB, 4, 3, 14, 4, 4, 14, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
	generateBox(level, chunkBB, 4, 1, 17, 4, 4, 17, Block::mNetherFence->mID, Block::mNetherFence->mID, false);

	return NetherBridgePiece::postProcess(level, random, chunkBB);
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

NBBridgeEndFiller::NBBridgeEndFiller()
	: NetherBridgePiece(){
}

NBBridgeEndFiller::NBBridgeEndFiller(int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: NetherBridgePiece(genDepth){

	orientation = direction;
	boundingBox = stairsBox;
	selfSeed = random.nextInt();
}

Unique<NetherBridgePiece> NBBridgeEndFiller::createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth) {

	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, -1, -3, 0, width, height, depth, direction);

	if (!isOkBox(box) || StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<NBBridgeEndFiller>(genDepth, random, box, direction);
}

bool NBBridgeEndFiller::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {

	Random selfRandom(selfSeed);

	// floor
	for (int x = 0; x <= 4; x++) {
		for (int y = 3; y <= 4; y++) {
			int z = selfRandom.nextInt(8);
			generateBox(level, chunkBB, x, y, 0, x, y, z, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
		}
	}

	// hand rails
	{
		int z = selfRandom.nextInt(8);
		generateBox(level, chunkBB, 0, 5, 0, 0, 5, z, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	}
	{
		int z = selfRandom.nextInt(8);
		generateBox(level, chunkBB, 4, 5, 0, 4, 5, z, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	}

	// supports
	for (int x = 0; x <= 4; x++) {
		int z = selfRandom.nextInt(5);
		generateBox(level, chunkBB, x, 2, 0, x, 2, z, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	}

	for (int x = 0; x <= 4; x++) {
		for (int y = 0; y <= 1; y++) {
			int z = selfRandom.nextInt(3);
			generateBox(level, chunkBB, x, y, 0, x, y, z, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
		}
	}

	return NetherBridgePiece::postProcess(level, random, chunkBB);
}

void NBBridgeEndFiller::addAdditionalSaveData(CompoundTag& tag) const {
	NetherBridgePiece::addAdditionalSaveData(tag);

	tag.putInt("Seed", selfSeed);
}

void NBBridgeEndFiller::readAdditionalSaveData(const CompoundTag& tag) {
	NetherBridgePiece::readAdditionalSaveData(tag);

	selfSeed = tag.getInt("Seed");
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

NBRoomCrossing::NBRoomCrossing()
	: NetherBridgePiece(){
}

NBRoomCrossing::NBRoomCrossing(int genDepth, Random& random, const BoundingBox& box, int direction)
	: NetherBridgePiece(genDepth){
	orientation = direction;
	boundingBox = box;
}

Unique<NetherBridgePiece> NBRoomCrossing::createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth) {

	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, -2, 0, 0, width, height, depth, direction);

	if (!isOkBox(box) || StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<NBRoomCrossing>(genDepth, random, box, direction);
}

void NBRoomCrossing::addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) {

	generateChildForward((NBStartPiece*)startPiece, pieces, random, 2, 0, false);
	generateChildLeft((NBStartPiece*)startPiece, pieces, random, 0, 2, false);
	generateChildRight((NBStartPiece*)startPiece, pieces, random, 0, 2, false);
}

bool NBRoomCrossing::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {

	// floor
	generateBox(level, chunkBB, 0, 0, 0, width - 1, 1, depth - 1, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	// room air
	generateBox(level, chunkBB, 0, 2, 0, 6, 7, 6, BlockID::AIR, BlockID::AIR, false);

	// walls
	generateBox(level, chunkBB, 0, 2, 0, 1, 6, 0, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 0, 2, 6, 1, 6, 6, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 5, 2, 0, 6, 6, 0, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 5, 2, 6, 6, 6, 6, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 0, 2, 0, 0, 6, 1, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 0, 2, 5, 0, 6, 6, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 6, 2, 0, 6, 6, 1, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 6, 2, 5, 6, 6, 6, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);

	// entries
	generateBox(level, chunkBB, 2, 6, 0, 4, 6, 0, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 2, 5, 0, 4, 5, 0, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
	generateBox(level, chunkBB, 2, 6, 6, 4, 6, 6, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 2, 5, 6, 4, 5, 6, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
	generateBox(level, chunkBB, 0, 6, 2, 0, 6, 4, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 0, 5, 2, 0, 5, 4, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
	generateBox(level, chunkBB, 6, 6, 2, 6, 6, 4, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 6, 5, 2, 6, 5, 4, Block::mNetherFence->mID, Block::mNetherFence->mID, false);

	// supports
	for (int x = 0; x <= 6; x++) {
		for (int z = 0; z <= 6; z++) {
			fillColumnDown(level, FullBlock(Block::mNetherBrick->mID, 0), x, -1, z, chunkBB);
		}
	}

	return NetherBridgePiece::postProcess(level, random, chunkBB);
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

NBStairsRoom::NBStairsRoom()
	: NetherBridgePiece(){
}

NBStairsRoom::NBStairsRoom(int genDepth, Random& random, const BoundingBox& box, int direction)
	: NetherBridgePiece(genDepth){
	orientation = direction;
	boundingBox = box;
}

Unique<NetherBridgePiece> NBStairsRoom::createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth) {

	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, -2, 0, 0, width, height, depth, direction);

	if (!isOkBox(box) || StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<NBStairsRoom>(genDepth, random, box, direction);
}

void NBStairsRoom::addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) {

	generateChildRight((NBStartPiece*)startPiece, pieces, random, 6, 2, false);
}

bool NBStairsRoom::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {

	// floor
	generateBox(level, chunkBB, 0, 0, 0, width - 1, 1, depth - 1, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	// room air
	generateBox(level, chunkBB, 0, 2, 0, 6, 10, 6, BlockID::AIR, BlockID::AIR, false);

	// walls
	generateBox(level, chunkBB, 0, 2, 0, 1, 8, 0, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 5, 2, 0, 6, 8, 0, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 0, 2, 1, 0, 8, 6, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 6, 2, 1, 6, 8, 6, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 1, 2, 6, 5, 8, 6, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);

	// wall decorations
	generateBox(level, chunkBB, 0, 3, 2, 0, 5, 4, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
	generateBox(level, chunkBB, 6, 3, 2, 6, 5, 2, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
	generateBox(level, chunkBB, 6, 3, 4, 6, 5, 4, Block::mNetherFence->mID, Block::mNetherFence->mID, false);

	// stair
	placeBlock(level, Block::mNetherBrick->mID, 0, 5, 2, 5, chunkBB);
	generateBox(level, chunkBB, 4, 2, 5, 4, 3, 5, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 3, 2, 5, 3, 4, 5, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 2, 2, 5, 2, 5, 5, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 1, 2, 5, 1, 6, 5, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);

	// top floor
	generateBox(level, chunkBB, 1, 7, 1, 5, 7, 4, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 6, 8, 2, 6, 8, 4, BlockID::AIR, BlockID::AIR, false);

	// entries
	generateBox(level, chunkBB, 2, 6, 0, 4, 8, 0, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 2, 5, 0, 4, 5, 0, Block::mNetherFence->mID, Block::mNetherFence->mID, false);

	for (int x = 0; x <= 6; x++) {
		for (int z = 0; z <= 6; z++) {
			fillColumnDown(level, FullBlock(Block::mNetherBrick->mID, 0), x, -1, z, chunkBB);
		}
	}

	return NetherBridgePiece::postProcess(level, random, chunkBB);
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

NBMonsterThrone::NBMonsterThrone()
	: NetherBridgePiece(){
}

NBMonsterThrone::NBMonsterThrone(int genDepth, Random& random, const BoundingBox& box, int direction)
	: NetherBridgePiece(genDepth){
	orientation = direction;
	boundingBox = box;
}

Unique<NetherBridgePiece> NBMonsterThrone::createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth) {

	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, -2, 0, 0, width, height, depth, direction);

	if (!isOkBox(box) || StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<NBMonsterThrone>(genDepth, random, box, direction);
}

bool NBMonsterThrone::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {

	// room air
	generateBox(level, chunkBB, 0, 2, 0, 6, 7, 7, BlockID::AIR, BlockID::AIR, false);

	// floors
	generateBox(level, chunkBB, 1, 0, 0, 5, 1, 7, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 1, 2, 1, 5, 2, 7, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 1, 3, 2, 5, 3, 7, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 1, 4, 3, 5, 4, 7, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);

	// rails
	generateBox(level, chunkBB, 1, 2, 0, 1, 4, 2, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 5, 2, 0, 5, 4, 2, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 1, 5, 2, 1, 5, 3, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 5, 5, 2, 5, 5, 3, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 0, 5, 3, 0, 5, 8, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 6, 5, 3, 6, 5, 8, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 1, 5, 8, 5, 5, 8, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);

	placeBlock(level, Block::mNetherFence->mID, 0, 1, 6, 3, chunkBB);
	placeBlock(level, Block::mNetherFence->mID, 0, 5, 6, 3, chunkBB);
	generateBox(level, chunkBB, 0, 6, 3, 0, 6, 8, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
	generateBox(level, chunkBB, 6, 6, 3, 6, 6, 8, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
	generateBox(level, chunkBB, 1, 6, 8, 5, 7, 8, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
	generateBox(level, chunkBB, 2, 8, 8, 4, 8, 8, Block::mNetherFence->mID, Block::mNetherFence->mID, false);

// 	if (!hasPlacedMobSpawner) {
// 		BlockPos pos(getWorldX(3, 5), getWorldY(5), getWorldZ(3, 5));
// 		if (chunkBB.isInside(pos)) {
// 			hasPlacedMobSpawner = true;
// 			level->setBlockAndData(pos, Block::mMobSpawner->mID, 0, Block::UPDATE_CLIENTS);
// 			MobSpawnerBlockEntity* entity = (MobSpawnerBlockEntity*)level->getBlockEntity(pos);
// 			if (entity != nullptr) {
// 				entity->getSpawner().setEntityId(EntityType::Blaze);
// 			}
// 		}
// 	}

	// supports
	for (int x = 0; x <= 6; x++) {
		for (int z = 0; z <= 6; z++) {
			fillColumnDown(level, FullBlock(Block::mNetherBrick->mID, 0), x, -1, z, chunkBB);
		}
	}

	return NetherBridgePiece::postProcess(level, random, chunkBB);
}

void NBMonsterThrone::addAdditionalSaveData(CompoundTag& tag) const {
	NetherBridgePiece::addAdditionalSaveData(tag);

	tag.putBoolean("Mob", hasPlacedMobSpawner);
}

void NBMonsterThrone::readAdditionalSaveData(const CompoundTag& tag) {
	NetherBridgePiece::readAdditionalSaveData(tag);

	hasPlacedMobSpawner = tag.getBoolean("Mob");
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

NBCastleEntrance::NBCastleEntrance()
	: NetherBridgePiece(){
}

NBCastleEntrance::NBCastleEntrance(int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: NetherBridgePiece(genDepth){
	orientation = direction;
	boundingBox = stairsBox;
}

Unique<NetherBridgePiece> NBCastleEntrance::createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth) {

	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, -5, -3, 0, width, height, depth, direction);

	if (!isOkBox(box) || StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<NBCastleEntrance>(genDepth, random, box, direction);
}

void NBCastleEntrance::addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) {

	generateChildForward((NBStartPiece*)startPiece, pieces, random, 5, 3, true);
}

bool NBCastleEntrance::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {

	// floor
	generateBox(level, chunkBB, 0, 3, 0, 12, 4, 12, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	// room air
	generateBox(level, chunkBB, 0, 5, 0, 12, 13, 12, BlockID::AIR, BlockID::AIR, false);

	// walls
	generateBox(level, chunkBB, 0, 5, 0, 1, 12, 12, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 11, 5, 0, 12, 12, 12, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 2, 5, 11, 4, 12, 12, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 8, 5, 11, 10, 12, 12, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 5, 9, 11, 7, 12, 12, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 2, 5, 0, 4, 12, 1, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 8, 5, 0, 10, 12, 1, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 5, 9, 0, 7, 12, 1, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);

	// roof
	generateBox(level, chunkBB, 2, 11, 2, 10, 12, 10, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);

	// entrance decoration
	generateBox(level, chunkBB, 5, 8, 0, 7, 8, 0, Block::mNetherFence->mID, Block::mNetherFence->mID, false);

	// wall decorations
	for (int i = 1; i <= 11; i += 2) {
		generateBox(level, chunkBB, i, 10, 0, i, 11, 0, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
		generateBox(level, chunkBB, i, 10, 12, i, 11, 12, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
		generateBox(level, chunkBB, 0, 10, i, 0, 11, i, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
		generateBox(level, chunkBB, 12, 10, i, 12, 11, i, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
		placeBlock(level, Block::mNetherBrick->mID, 0, i, 13, 0, chunkBB);
		placeBlock(level, Block::mNetherBrick->mID, 0, i, 13, 12, chunkBB);
		placeBlock(level, Block::mNetherBrick->mID, 0, 0, 13, i, chunkBB);
		placeBlock(level, Block::mNetherBrick->mID, 0, 12, 13, i, chunkBB);
		placeBlock(level, Block::mNetherFence->mID, 0, i + 1, 13, 0, chunkBB);
		placeBlock(level, Block::mNetherFence->mID, 0, i + 1, 13, 12, chunkBB);
		placeBlock(level, Block::mNetherFence->mID, 0, 0, 13, i + 1, chunkBB);
		placeBlock(level, Block::mNetherFence->mID, 0, 12, 13, i + 1, chunkBB);
	}
	placeBlock(level, Block::mNetherFence->mID, 0, 0, 13, 0, chunkBB);
	placeBlock(level, Block::mNetherFence->mID, 0, 0, 13, 12, chunkBB);
	placeBlock(level, Block::mNetherFence->mID, 0, 0, 13, 0, chunkBB);
	placeBlock(level, Block::mNetherFence->mID, 0, 12, 13, 0, chunkBB);

	// inside decorations
	for (int z = 3; z <= 9; z += 2) {
		generateBox(level, chunkBB, 1, 7, z, 1, 8, z, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
		generateBox(level, chunkBB, 11, 7, z, 11, 8, z, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
	}

	// supports
	generateBox(level, chunkBB, 4, 2, 0, 8, 2, 12, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 0, 2, 4, 12, 2, 8, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);

	generateBox(level, chunkBB, 4, 0, 0, 8, 1, 3, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 4, 0, 9, 8, 1, 12, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 0, 0, 4, 3, 1, 8, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 9, 0, 4, 12, 1, 8, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);

	for (int x = 4; x <= 8; x++) {
		for (int z = 0; z <= 2; z++) {
			fillColumnDown(level, FullBlock(Block::mNetherBrick->mID, 0), x, -1, z, chunkBB);
			fillColumnDown(level, FullBlock(Block::mNetherBrick->mID, 0), x, -1, 12 - z, chunkBB);
		}
	}

	for (int x = 0; x <= 2; x++) {
		for (int z = 4; z <= 8; z++) {
			fillColumnDown(level, FullBlock(Block::mNetherBrick->mID, 0), x, -1, z, chunkBB);
			fillColumnDown(level, FullBlock(Block::mNetherBrick->mID, 0), 12 - x, -1, z, chunkBB);
		}
	}

	// lava well
	generateBox(level, chunkBB, 5, 5, 5, 7, 5, 7, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 6, 1, 6, 6, 4, 6, BlockID::AIR, BlockID::AIR, false);
	placeBlock(level, Block::mNetherBrick->mID, 0, 6, 0, 6, chunkBB);
	placeBlock(level, Block::mFlowingLava->mID, 0, 6, 5, 6, chunkBB);
	// tick lava well
	BlockPos pos(getWorldX(6, 6), getWorldY(5), getWorldZ(6, 6));
	if (chunkBB.isInside(pos)) {
		Block::mBlocks[Block::mFlowingLava->mID]->tick(*level, pos, random);
	}

	return NetherBridgePiece::postProcess(level, random, chunkBB);
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

NBCastleStalkRoom::NBCastleStalkRoom()
	: NetherBridgePiece(){
}

NBCastleStalkRoom::NBCastleStalkRoom(int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: NetherBridgePiece(genDepth){
	orientation = direction;
	boundingBox = stairsBox;
}

Unique<NetherBridgePiece> NBCastleStalkRoom::createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth) {

	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, -5, -3, 0, width, height, depth, direction);

	if (!isOkBox(box) || StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<NBCastleStalkRoom>(genDepth, random, box, direction);
}

void NBCastleStalkRoom::addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) {

	generateChildForward((NBStartPiece*)startPiece, pieces, random, 5, 3, true);
	generateChildForward((NBStartPiece*)startPiece, pieces, random, 5, 11, true);
}

bool NBCastleStalkRoom::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {

	// floor
	generateBox(level, chunkBB, 0, 3, 0, 12, 4, 12, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	// room air
	generateBox(level, chunkBB, 0, 5, 0, 12, 13, 12, BlockID::AIR, BlockID::AIR, false);

	// walls
	generateBox(level, chunkBB, 0, 5, 0, 1, 12, 12, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 11, 5, 0, 12, 12, 12, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 2, 5, 11, 4, 12, 12, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 8, 5, 11, 10, 12, 12, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 5, 9, 11, 7, 12, 12, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 2, 5, 0, 4, 12, 1, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 8, 5, 0, 10, 12, 1, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 5, 9, 0, 7, 12, 1, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);

	// roof
	generateBox(level, chunkBB, 2, 11, 2, 10, 12, 10, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);

	// wall decorations
	for (int i = 1; i <= 11; i += 2) {
		generateBox(level, chunkBB, i, 10, 0, i, 11, 0, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
		generateBox(level, chunkBB, i, 10, 12, i, 11, 12, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
		generateBox(level, chunkBB, 0, 10, i, 0, 11, i, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
		generateBox(level, chunkBB, 12, 10, i, 12, 11, i, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
		placeBlock(level, Block::mNetherBrick->mID, 0, i, 13, 0, chunkBB);
		placeBlock(level, Block::mNetherBrick->mID, 0, i, 13, 12, chunkBB);
		placeBlock(level, Block::mNetherBrick->mID, 0, 0, 13, i, chunkBB);
		placeBlock(level, Block::mNetherBrick->mID, 0, 12, 13, i, chunkBB);
		placeBlock(level, Block::mNetherFence->mID, 0, i + 1, 13, 0, chunkBB);
		placeBlock(level, Block::mNetherFence->mID, 0, i + 1, 13, 12, chunkBB);
		placeBlock(level, Block::mNetherFence->mID, 0, 0, 13, i + 1, chunkBB);
		placeBlock(level, Block::mNetherFence->mID, 0, 12, 13, i + 1, chunkBB);
	}
	placeBlock(level, Block::mNetherFence->mID, 0, 0, 13, 0, chunkBB);
	placeBlock(level, Block::mNetherFence->mID, 0, 0, 13, 12, chunkBB);
	placeBlock(level, Block::mNetherFence->mID, 0, 0, 13, 0, chunkBB);
	placeBlock(level, Block::mNetherFence->mID, 0, 12, 13, 0, chunkBB);

	// inside decorations
	for (int z = 3; z <= 9; z += 2) {
		generateBox(level, chunkBB, 1, 7, z, 1, 8, z, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
		generateBox(level, chunkBB, 11, 7, z, 11, 8, z, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
	}

	// inside stair
	int stairOrientation = getOrientationData(Block::mNetherBrickStairs, 3);

	for (int i = 0; i <= 6; i++) {
		int z = i + 4;

		for (int x = 5; x <= 7; x++) {
			placeBlock(level, Block::mNetherBrickStairs->mID, stairOrientation, x, 5 + i, z, chunkBB);
		}
		if (z >= 5 && z <= 8) {
			generateBox(level, chunkBB, 5, 5, z, 7, i + 4, z, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
		}
		else if (z >= 9 && z <= 10) {
			generateBox(level, chunkBB, 5, 8, z, 7, i + 4, z, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
		}
		if (i >= 1) {
			generateBox(level, chunkBB, 5, 6 + i, z, 7, 9 + i, z, BlockID::AIR, BlockID::AIR, false);
		}
	}

	for (int x = 5; x <= 7; x++) {
		placeBlock(level, Block::mNetherBrickStairs->mID, stairOrientation, x, 12, 11, chunkBB);
	}
	generateBox(level, chunkBB, 5, 6, 7, 5, 7, 7, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
	generateBox(level, chunkBB, 7, 6, 7, 7, 7, 7, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
	generateBox(level, chunkBB, 5, 13, 12, 7, 13, 12, BlockID::AIR, BlockID::AIR, false);

	// farmland catwalks
	generateBox(level, chunkBB, 2, 5, 2, 3, 5, 3, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 2, 5, 9, 3, 5, 10, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 2, 5, 4, 2, 5, 8, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 9, 5, 2, 10, 5, 3, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 9, 5, 9, 10, 5, 10, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 10, 5, 4, 10, 5, 8, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	int eastOrientation = getOrientationData(Block::mNetherBrickStairs, 0);
	int westOrientation = getOrientationData(Block::mNetherBrickStairs, 1);
	placeBlock(level, Block::mNetherBrickStairs->mID, westOrientation, 4, 5, 2, chunkBB);
	placeBlock(level, Block::mNetherBrickStairs->mID, westOrientation, 4, 5, 3, chunkBB);
	placeBlock(level, Block::mNetherBrickStairs->mID, westOrientation, 4, 5, 9, chunkBB);
	placeBlock(level, Block::mNetherBrickStairs->mID, westOrientation, 4, 5, 10, chunkBB);
	placeBlock(level, Block::mNetherBrickStairs->mID, eastOrientation, 8, 5, 2, chunkBB);
	placeBlock(level, Block::mNetherBrickStairs->mID, eastOrientation, 8, 5, 3, chunkBB);
	placeBlock(level, Block::mNetherBrickStairs->mID, eastOrientation, 8, 5, 9, chunkBB);
	placeBlock(level, Block::mNetherBrickStairs->mID, eastOrientation, 8, 5, 10, chunkBB);

	// farmlands
	generateBox(level, chunkBB, 3, 4, 4, 4, 4, 8, Block::mSoulSand->mID, Block::mSoulSand->mID, false);
	generateBox(level, chunkBB, 8, 4, 4, 9, 4, 8, Block::mSoulSand->mID, Block::mSoulSand->mID, false);
	generateBox(level, chunkBB, 3, 5, 4, 4, 5, 8, Block::mNetherWart->mID, Block::mNetherWart->mID, false);
	generateBox(level, chunkBB, 8, 5, 4, 9, 5, 8, Block::mNetherWart->mID, Block::mNetherWart->mID, false);

	// supports
	generateBox(level, chunkBB, 4, 2, 0, 8, 2, 12, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 0, 2, 4, 12, 2, 8, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);

	generateBox(level, chunkBB, 4, 0, 0, 8, 1, 3, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 4, 0, 9, 8, 1, 12, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 0, 0, 4, 3, 1, 8, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 9, 0, 4, 12, 1, 8, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);

	for (int x = 4; x <= 8; x++) {
		for (int z = 0; z <= 2; z++) {
			fillColumnDown(level, FullBlock(Block::mNetherBrick->mID, 0), x, -1, z, chunkBB);
			fillColumnDown(level, FullBlock(Block::mNetherBrick->mID, 0), x, -1, 12 - z, chunkBB);
		}
	}

	for (int x = 0; x <= 2; x++) {
		for (int z = 4; z <= 8; z++) {
			fillColumnDown(level, FullBlock(Block::mNetherBrick->mID, 0), x, -1, z, chunkBB);
			fillColumnDown(level, FullBlock(Block::mNetherBrick->mID, 0), 12 - x, -1, z, chunkBB);
		}
	}

	return NetherBridgePiece::postProcess(level, random, chunkBB);
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

NBCastleSmallCorridorPiece::NBCastleSmallCorridorPiece()
	: NetherBridgePiece(){
}

NBCastleSmallCorridorPiece::NBCastleSmallCorridorPiece(int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: NetherBridgePiece(genDepth){
	orientation = direction;
	boundingBox = stairsBox;
}

Unique<NetherBridgePiece> NBCastleSmallCorridorPiece::createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth) {

	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, -1, 0, 0, width, height, depth, direction);

	if (!isOkBox(box) || StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<NBCastleSmallCorridorPiece>(genDepth, random, box, direction);
}

void NBCastleSmallCorridorPiece::addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) {

	generateChildForward((NBStartPiece*)startPiece, pieces, random, 1, 0, true);
}

bool NBCastleSmallCorridorPiece::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {

	// floor
	generateBox(level, chunkBB, 0, 0, 0, 4, 1, 4, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	// room air
	generateBox(level, chunkBB, 0, 2, 0, 4, 5, 4, BlockID::AIR, BlockID::AIR, false);

	// walls
	generateBox(level, chunkBB, 0, 2, 0, 0, 5, 4, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 4, 2, 0, 4, 5, 4, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 0, 3, 1, 0, 4, 1, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
	generateBox(level, chunkBB, 0, 3, 3, 0, 4, 3, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
	generateBox(level, chunkBB, 4, 3, 1, 4, 4, 1, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
	generateBox(level, chunkBB, 4, 3, 3, 4, 4, 3, Block::mNetherFence->mID, Block::mNetherFence->mID, false);

	// roof
	generateBox(level, chunkBB, 0, 6, 0, 4, 6, 4, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);

	// supports
	for (int x = 0; x <= 4; x++) {
		for (int z = 0; z <= 4; z++) {
			fillColumnDown(level, FullBlock(Block::mNetherBrick->mID, 0), x, -1, z, chunkBB);
		}
	}

	return NetherBridgePiece::postProcess(level, random, chunkBB);
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

NBCastleSmallCorridorCrossingPiece::NBCastleSmallCorridorCrossingPiece()
	: NetherBridgePiece(){
}

NBCastleSmallCorridorCrossingPiece::NBCastleSmallCorridorCrossingPiece(int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: NetherBridgePiece(genDepth){
	orientation = direction;
	boundingBox = stairsBox;
}

Unique<NetherBridgePiece> NBCastleSmallCorridorCrossingPiece::createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth) {

	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, -1, 0, 0, width, height, depth, direction);

	if (!isOkBox(box) || StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<NBCastleSmallCorridorCrossingPiece>(genDepth, random, box, direction);
}

void NBCastleSmallCorridorCrossingPiece::addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) {

	generateChildForward((NBStartPiece*)startPiece, pieces, random, 1, 0, true);
	generateChildLeft((NBStartPiece*)startPiece, pieces, random, 0, 1, true);
	generateChildRight((NBStartPiece*)startPiece, pieces, random, 0, 1, true);
}

bool NBCastleSmallCorridorCrossingPiece::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {

	// floor
	generateBox(level, chunkBB, 0, 0, 0, 4, 1, 4, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	// room air
	generateBox(level, chunkBB, 0, 2, 0, 4, 5, 4, BlockID::AIR, BlockID::AIR, false);

	// walls
	generateBox(level, chunkBB, 0, 2, 0, 0, 5, 0, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 4, 2, 0, 4, 5, 0, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 0, 2, 4, 0, 5, 4, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 4, 2, 4, 4, 5, 4, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);

	// roof
	generateBox(level, chunkBB, 0, 6, 0, 4, 6, 4, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);

	// supports
	for (int x = 0; x <= 4; x++) {
		for (int z = 0; z <= 4; z++) {
			fillColumnDown(level, FullBlock(Block::mNetherBrick->mID, 0), x, -1, z, chunkBB);
		}
	}

	return NetherBridgePiece::postProcess(level, random, chunkBB);
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

NBCastleSmallCorridorRightTurnPiece::NBCastleSmallCorridorRightTurnPiece()
	: NetherBridgePiece(){
}

NBCastleSmallCorridorRightTurnPiece::NBCastleSmallCorridorRightTurnPiece(int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: NetherBridgePiece(genDepth){
	orientation = direction;
	boundingBox = stairsBox;

	isNeedingChest = random.nextInt(3) == 0;
}

Unique<NetherBridgePiece> NBCastleSmallCorridorRightTurnPiece::createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth) {

	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, -1, 0, 0, width, height, depth, direction);

	if (!isOkBox(box) || StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<NBCastleSmallCorridorRightTurnPiece>(genDepth, random, box, direction);
}

void NBCastleSmallCorridorRightTurnPiece::addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) {

	generateChildRight((NBStartPiece*)startPiece, pieces, random, 0, 1, true);
}

bool NBCastleSmallCorridorRightTurnPiece::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {

	// floor
	generateBox(level, chunkBB, 0, 0, 0, 4, 1, 4, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	// room air
	generateBox(level, chunkBB, 0, 2, 0, 4, 5, 4, BlockID::AIR, BlockID::AIR, false);

	// walls
	generateBox(level, chunkBB, 0, 2, 0, 0, 5, 4, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 0, 3, 1, 0, 4, 1, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
	generateBox(level, chunkBB, 0, 3, 3, 0, 4, 3, Block::mNetherFence->mID, Block::mNetherFence->mID, false);

	generateBox(level, chunkBB, 4, 2, 0, 4, 5, 0, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);

	generateBox(level, chunkBB, 1, 2, 4, 4, 5, 4, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 1, 3, 4, 1, 4, 4, Block::mNetherFence->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 3, 3, 4, 3, 4, 4, Block::mNetherFence->mID, Block::mNetherBrick->mID, false);

	if (isNeedingChest) {
		if (chunkBB.isInside(BlockPos(getWorldX(1, 3), getWorldY(2), getWorldZ(1, 3)))) {
			isNeedingChest = false;
			createChest(level, chunkBB, random, 1, 2, 3, Direction::WEST, "loot_tables/chests/nether_bridge.json");
		}
	}

	// roof
	generateBox(level, chunkBB, 0, 6, 0, 4, 6, 4, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);

	// supports
	for (int x = 0; x <= 4; x++) {
		for (int z = 0; z <= 4; z++) {
			fillColumnDown(level, FullBlock(Block::mNetherBrick->mID, 0), x, -1, z, chunkBB);
		}
	}

	return NetherBridgePiece::postProcess(level, random, chunkBB);
}

void NBCastleSmallCorridorRightTurnPiece::addAdditionalSaveData(CompoundTag& tag) const {
	NetherBridgePiece::addAdditionalSaveData(tag);

	tag.putBoolean("Chest", isNeedingChest);
}

void NBCastleSmallCorridorRightTurnPiece::readAdditionalSaveData(const CompoundTag& tag) {
	NetherBridgePiece::readAdditionalSaveData(tag);

	isNeedingChest = tag.getBoolean("Chest");
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

NBCastleSmallCorridorLeftTurnPiece::NBCastleSmallCorridorLeftTurnPiece()
	: NetherBridgePiece(){
}

NBCastleSmallCorridorLeftTurnPiece::NBCastleSmallCorridorLeftTurnPiece(int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: NetherBridgePiece(genDepth){
	orientation = direction;
	boundingBox = stairsBox;

	isNeedingChest = random.nextInt(3) == 0;
}

Unique<NetherBridgePiece> NBCastleSmallCorridorLeftTurnPiece::createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth) {

	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, -1, 0, 0, width, height, depth, direction);

	if (!isOkBox(box) || StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<NBCastleSmallCorridorLeftTurnPiece>(genDepth, random, box, direction);
}

void NBCastleSmallCorridorLeftTurnPiece::addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) {

	generateChildLeft((NBStartPiece*)startPiece, pieces, random, 0, 1, true);
}

bool NBCastleSmallCorridorLeftTurnPiece::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {

	// floor
	generateBox(level, chunkBB, 0, 0, 0, 4, 1, 4, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	// room air
	generateBox(level, chunkBB, 0, 2, 0, 4, 5, 4, BlockID::AIR, BlockID::AIR, false);

	// walls
	generateBox(level, chunkBB, 4, 2, 0, 4, 5, 4, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 4, 3, 1, 4, 4, 1, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
	generateBox(level, chunkBB, 4, 3, 3, 4, 4, 3, Block::mNetherFence->mID, Block::mNetherFence->mID, false);

	generateBox(level, chunkBB, 0, 2, 0, 0, 5, 0, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);

	generateBox(level, chunkBB, 0, 2, 4, 3, 5, 4, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 1, 3, 4, 1, 4, 4, Block::mNetherFence->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 3, 3, 4, 3, 4, 4, Block::mNetherFence->mID, Block::mNetherBrick->mID, false);

	if (isNeedingChest) {
		if (chunkBB.isInside(BlockPos(getWorldX(3, 3), getWorldY(2), getWorldZ(3, 3)))) {
			isNeedingChest = false;
			createChest(level, chunkBB, random, 3, 2, 3, Direction::WEST, "loot_tables/chests/nether_bridge.json");
		}
	}

	// roof
	generateBox(level, chunkBB, 0, 6, 0, 4, 6, 4, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);

	// supports
	for (int x = 0; x <= 4; x++) {
		for (int z = 0; z <= 4; z++) {
			fillColumnDown(level, FullBlock(Block::mNetherBrick->mID, 0), x, -1, z, chunkBB);
		}
	}

	return NetherBridgePiece::postProcess(level, random, chunkBB);
}

void NBCastleSmallCorridorLeftTurnPiece::addAdditionalSaveData(CompoundTag& tag) const {
	NetherBridgePiece::addAdditionalSaveData(tag);

	tag.putBoolean("Chest", isNeedingChest);
}

void NBCastleSmallCorridorLeftTurnPiece::readAdditionalSaveData(const CompoundTag& tag) {
	NetherBridgePiece::readAdditionalSaveData(tag);

	isNeedingChest = tag.getBoolean("Chest");
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

NBCastleCorridorStairsPiece::NBCastleCorridorStairsPiece()
	: NetherBridgePiece(){
}

NBCastleCorridorStairsPiece::NBCastleCorridorStairsPiece(int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: NetherBridgePiece(genDepth){
	orientation = direction;
	boundingBox = stairsBox;
}

Unique<NetherBridgePiece> NBCastleCorridorStairsPiece::createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth) {

	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, -1, -7, 0, width, height, depth, direction);

	if (!isOkBox(box) || StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<NBCastleCorridorStairsPiece>(genDepth, random, box, direction);
}

void NBCastleCorridorStairsPiece::addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) {

	generateChildForward((NBStartPiece*)startPiece, pieces, random, 1, 0, true);
}

bool NBCastleCorridorStairsPiece::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {

	// generate stairs
	int stairsData = getOrientationData(Block::mNetherBrickStairs, 2);

	for (int step = 0; step <= 9; step++) {
		int floor = std::max(1, 7 - step);
		int roof = std::min(std::max(floor + 5, 14 - step), 13);
		int z = step;

		// floor
		generateBox(level, chunkBB, 0, 0, z, 4, floor, z, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
		// room air
		generateBox(level, chunkBB, 1, floor + 1, z, 3, roof - 1, z, BlockID::AIR, BlockID::AIR, false);
		if (step <= 6) {
			placeBlock(level, Block::mNetherBrickStairs->mID, stairsData, 1, floor + 1, z, chunkBB);
			placeBlock(level, Block::mNetherBrickStairs->mID, stairsData, 2, floor + 1, z, chunkBB);
			placeBlock(level, Block::mNetherBrickStairs->mID, stairsData, 3, floor + 1, z, chunkBB);
		}
		// roof
		generateBox(level, chunkBB, 0, roof, z, 4, roof, z, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
		// walls
		generateBox(level, chunkBB, 0, floor + 1, z, 0, roof - 1, z, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
		generateBox(level, chunkBB, 4, floor + 1, z, 4, roof - 1, z, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
		if ((step & 1) == 0) {
			generateBox(level, chunkBB, 0, floor + 2, z, 0, floor + 3, z, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
			generateBox(level, chunkBB, 4, floor + 2, z, 4, floor + 3, z, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
		}

		// supports
		for (int x = 0; x <= 4; x++) {
			fillColumnDown(level, FullBlock(Block::mNetherBrick->mID, 0), x, -1, z, chunkBB);
		}
	}

	return NetherBridgePiece::postProcess(level, random, chunkBB);
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

NBCastleCorridorTBalconyPiece::NBCastleCorridorTBalconyPiece()
	: NetherBridgePiece(){
}

NBCastleCorridorTBalconyPiece::NBCastleCorridorTBalconyPiece(int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: NetherBridgePiece(genDepth){
	orientation = direction;
	boundingBox = stairsBox;
}

Unique<NetherBridgePiece> NBCastleCorridorTBalconyPiece::createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth) {

	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, -3, 0, 0, width, height, depth, direction);

	if (!isOkBox(box) || StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<NBCastleCorridorTBalconyPiece>(genDepth, random, box, direction);
}

void NBCastleCorridorTBalconyPiece::addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) {

	int zOff = 1;
	// compensate for weird negative-facing behaviour
	if (orientation == Direction::WEST || orientation == Direction::NORTH) {
		zOff = 5;
	}

	generateChildLeft((NBStartPiece*)startPiece, pieces, random, 0, zOff, random.nextInt(8) > 0);
	generateChildRight((NBStartPiece*)startPiece, pieces, random, 0, zOff, random.nextInt(8) > 0);
}

bool NBCastleCorridorTBalconyPiece::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {

	// floor
	generateBox(level, chunkBB, 0, 0, 0, 8, 1, 8, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	// room air
	generateBox(level, chunkBB, 0, 2, 0, 8, 5, 8, BlockID::AIR, BlockID::AIR, false);
	// corridor roof
	generateBox(level, chunkBB, 0, 6, 0, 8, 6, 5, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);

	// inside walls
	generateBox(level, chunkBB, 0, 2, 0, 2, 5, 0, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 6, 2, 0, 8, 5, 0, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 1, 3, 0, 1, 4, 0, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
	generateBox(level, chunkBB, 7, 3, 0, 7, 4, 0, Block::mNetherFence->mID, Block::mNetherFence->mID, false);

	// balcony floor
	generateBox(level, chunkBB, 0, 2, 4, 8, 2, 8, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 1, 1, 4, 2, 2, 4, BlockID::AIR, BlockID::AIR, false);
	generateBox(level, chunkBB, 6, 1, 4, 7, 2, 4, BlockID::AIR, BlockID::AIR, false);

	// hand rails
	generateBox(level, chunkBB, 0, 3, 8, 8, 3, 8, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
	generateBox(level, chunkBB, 0, 3, 6, 0, 3, 7, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
	generateBox(level, chunkBB, 8, 3, 6, 8, 3, 7, Block::mNetherFence->mID, Block::mNetherFence->mID, false);

	// balcony walls
	generateBox(level, chunkBB, 0, 3, 4, 0, 5, 5, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 8, 3, 4, 8, 5, 5, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 1, 3, 5, 2, 5, 5, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 6, 3, 5, 7, 5, 5, Block::mNetherBrick->mID, Block::mNetherBrick->mID, false);
	generateBox(level, chunkBB, 1, 4, 5, 1, 5, 5, Block::mNetherFence->mID, Block::mNetherFence->mID, false);
	generateBox(level, chunkBB, 7, 4, 5, 7, 5, 5, Block::mNetherFence->mID, Block::mNetherFence->mID, false);

	// supports
	for (int z = 0; z <= 5; z++) {
		for (int x = 0; x <= 8; x++) {
			fillColumnDown(level, FullBlock(Block::mNetherBrick->mID, 0), x, -1, z, chunkBB);
		}
	}

	return NetherBridgePiece::postProcess(level, random, chunkBB);
}
