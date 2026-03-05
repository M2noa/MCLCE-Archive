/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/levelgen/structure/OceanMonumentPieces.h"
#include "world/Facing.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/BlockSource.h"
//#include "world/level/Spawner.h"
#include "world/level/block/PrismarineBlock.h"
#include "world/level/block/SpongeBlock.h"
#include "world/level/Level.h"
// #include "world/entity/monster/Guardian.h"
#include <algorithm>

//define static non-const members
int OceanMonumentPiece::mGridroomSourceIndex = getRoomIndex(2, 0, 0); // entry room
int OceanMonumentPiece::mGridroomTopConnectIndex = getRoomIndex(2, 2, 0); // room with connection to roof piece
int OceanMonumentPiece::mGridroomLeftWingConnectIndex = getRoomIndex(0, 1, 0); // room with connection to left wing piece
int OceanMonumentPiece::mGridroomRightWingConnectIndex = getRoomIndex(4, 1, 0); // room with connection to right wing piece

const int OceanMonumentPiece::mLeftWingIndex = 1001;
const int OceanMonumentPiece::mRightWingIndex = 1002;
const int OceanMonumentPiece::mPenthouseIndex = 1003;

static FullBlock BASE_GRAY;
static FullBlock BASE_LIGHT;
static FullBlock BASE_BLACK;
static FullBlock DOT_DECO_DATA;
static FullBlock LAMP_BLOCK;
static FullBlock FILL_BLOCK;

static void initBuildingBlocks() {
	BASE_GRAY = FullBlock(Block::mPrismarine->mID, enum_cast(PrismarineBlock::PrismarineType::Prismarine));
	BASE_LIGHT = FullBlock(Block::mPrismarine->mID, enum_cast(PrismarineBlock::PrismarineType::PrismarineBricks));
	BASE_BLACK = FullBlock(Block::mPrismarine->mID, enum_cast(PrismarineBlock::PrismarineType::PrismarineDark));
	DOT_DECO_DATA = BASE_LIGHT;
	LAMP_BLOCK = FullBlock(Block::mSeaLantern->mID);
	FILL_BLOCK = FullBlock(Block::mStillWater->mID);
}
///////////////////////////////////////////////////////////////
//ROOM DEFINITION
RoomDefinition::RoomDefinition() {
	RoomDefinition(-1);
}

RoomDefinition::RoomDefinition(int roomIndex) : mConnections(6), mHasOpening(6) {
	mIndex = roomIndex;
	mClaimed = false;
	mIsSource = false;
	mScanIndex = 0;
}

void RoomDefinition::setConnection(const FacingID& direction, Shared<RoomDefinition> definition) {
	mConnections[direction] = definition;
	const FacingID& oppDir = Facing::OPPOSITE_FACING[direction];
	(*definition).mConnections[oppDir] = shared_from_this(); //setting "this" ptr as a shared ptr
}

void RoomDefinition::updateOpenings() {
	for (int i = 0; i < 6; i++) {
		mHasOpening[i] = (mConnections[i] != nullptr);
	}
}

bool RoomDefinition::findSource(int scanIndex) {
	if (mIsSource) {
		return true;
	}
	mScanIndex = scanIndex;
	for (int i = 0; i < 6; i++) {
		if (mConnections[i] != nullptr && mHasOpening[i]) {
			if ((*mConnections[i]).mScanIndex != scanIndex && (*mConnections[i]).findSource(scanIndex)) {
				return true;
			}
		}
	}
	return false;
}

bool RoomDefinition::isSpecial() {
	return mIndex >= OceanMonumentPiece::getGridSize();
}

int RoomDefinition::countOpenings() {
	int c = 0;
	for (int i = 0; i < 6; i++) {
		if (mHasOpening[i]) {
			c++;
		}
	}
	return c;
}

///////////////////////////////////////////////////////////////
//OCEAN MONUMENT PIECE
OceanMonumentPiece::OceanMonumentPiece()
	: StructurePiece(0) {
	initBuildingBlocks();
}

OceanMonumentPiece::OceanMonumentPiece(int genDepth)
	: StructurePiece(genDepth) {
	initBuildingBlocks();
}

OceanMonumentPiece::OceanMonumentPiece(int& orientation, BoundingBox& boundingBox)
	: StructurePiece(1) {
	this->orientation = orientation;
	this->boundingBox = boundingBox;
	initBuildingBlocks();
}

OceanMonumentPiece::OceanMonumentPiece(int genDepth, int& orientation, Shared<RoomDefinition>& roomDefinition, int roomWidth, int roomHeight, int roomDepth)
	: StructurePiece(genDepth) {
	this->orientation = orientation;
	mRoomDefinition = roomDefinition;
	int roomIndex = (*roomDefinition).mIndex;
	int roomX = roomIndex % mGridWidth;
	int roomZ = (roomIndex / mGridWidth) % mGridDepth;
	int roomY = roomIndex / mGridFloorCount;

	// calculate bounding box from room mIndex according to the room grid
	// (note: relative coordinates, needs to be pushed by the parent to world coordinates)
	if (orientation == Facing::NORTH || orientation == Facing::SOUTH) {
		boundingBox = BoundingBox(0, 0, 0, (roomWidth * mGridroomWidth) - 1, (roomHeight * mGridroomHeight) - 1, (roomDepth * mGridroomDepth) - 1);
	}
	else {
		// flip width and depth
		boundingBox = BoundingBox(0, 0, 0, (roomDepth * mGridroomDepth) - 1, (roomHeight * mGridroomHeight) - 1, (roomWidth * mGridroomWidth) - 1);
	}

	switch (orientation) {
	case Facing::NORTH:
		boundingBox.move(roomX * mGridroomWidth, roomY * mGridroomHeight, -(roomZ + roomDepth) * mGridroomDepth + 1);
		break;
	case Facing::SOUTH:
		// flipped Z
		boundingBox.move(roomX * mGridroomWidth, roomY * mGridroomHeight, roomZ * mGridroomDepth);
		break;
	case Facing::WEST:
		boundingBox.move(-(roomZ + roomDepth) * mGridroomWidth + 1, roomY * mGridroomHeight, roomX * mGridroomWidth);
		break;
	default:
		boundingBox.move(roomZ * mGridroomDepth, roomY * mGridroomHeight, roomX * mGridroomWidth);
		break;
	}

	initBuildingBlocks();
}

void OceanMonumentPiece::addAdditionalSaveData(CompoundTag& tag) const {
}

void OceanMonumentPiece::readAdditionalSaveData(const CompoundTag& tag) {
}

int OceanMonumentPiece::getWorldX(int x, int z) {
	switch (orientation) {
	case Facing::NORTH:
	case Facing::SOUTH:
		return boundingBox.x0 + x;
	case Facing::WEST:
		return boundingBox.x1 - z;
	case Facing::EAST:
		return boundingBox.x0 + z;
	default:
		return x;
	}
}

int OceanMonumentPiece::getWorldZ(int x, int z) {
	switch (orientation) {
	case Facing::NORTH:
		return boundingBox.z1 - z;
	case Facing::SOUTH:
		return boundingBox.z0 + z;
	case Facing::WEST:
	case Facing::EAST:
		return boundingBox.z0 + x;
	default:
		return z;
	}
}

void OceanMonumentPiece::generateWaterBox(BlockSource* level, const BoundingBox& chunkBB, int x0, int y0, int z0, int x1, int y1, int z1, bool skipAir) {
	for (int y = y0; y <= y1; y++) {
		for (int x = x0; x <= x1; x++) {
			for (int z = z0; z <= z1; z++) {
				if (skipAir && getBlock(level, x, y, z, chunkBB) == BlockID::AIR) {
					continue;
				}
				if (getWorldY(y) >= (*level).getDimension().getSeaLevel()) {
					placeBlock(level, Block::mAir->mID, 0, x, y, z, chunkBB);
				}
				else {
					placeBlock(level, FILL_BLOCK.id, 0, x, y, z, chunkBB);
				}
			}
		}
	}
}

void OceanMonumentPiece::generateDefaultFloor(BlockSource* level, const BoundingBox& chunkBB, int xOff, int zOff, bool downOpening) {
	if (downOpening) {
		generateBox(level, chunkBB, xOff + 0, 0, zOff + 0, xOff + 2, 0, zOff + mGridroomDepth - 1, BASE_GRAY, BASE_GRAY, false);
		generateBox(level, chunkBB, xOff + 5, 0, zOff + 0, xOff + mGridroomWidth - 1, 0, zOff + mGridroomDepth - 1, BASE_GRAY, BASE_GRAY, false);
		generateBox(level, chunkBB, xOff + 3, 0, zOff + 0, xOff + 4, 0, zOff + 2, BASE_GRAY, BASE_GRAY, false);
		generateBox(level, chunkBB, xOff + 3, 0, zOff + 5, xOff + 4, 0, zOff + mGridroomDepth - 1, BASE_GRAY, BASE_GRAY, false);
		generateBox(level, chunkBB, xOff + 3, 0, zOff + 2, xOff + 4, 0, zOff + 2, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, xOff + 3, 0, zOff + 5, xOff + 4, 0, zOff + 5, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, xOff + 2, 0, zOff + 3, xOff + 2, 0, zOff + 4, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, xOff + 5, 0, zOff + 3, xOff + 5, 0, zOff + 4, BASE_LIGHT, BASE_LIGHT, false);
	}
	else {
		generateBox(level, chunkBB, xOff + 0, 0, zOff + 0, xOff + mGridroomWidth - 1, 0, zOff + mGridroomDepth - 1, BASE_GRAY, BASE_GRAY, false);
	}
}

void OceanMonumentPiece::generateBoxOnFillOnly(BlockSource* level, const BoundingBox& chunkBB, int x0, int y0, int z0, int x1, int y1, int z1, FullBlock targetBlock) {
	for (int y = y0; y <= y1; y++) {
		for (int x = x0; x <= x1; x++) {
			for (int z = z0; z <= z1; z++) {
				if (getBlock(level, x, y, z, chunkBB) != FILL_BLOCK.id) {
					continue;
				}
				placeBlock(level, targetBlock, x, y, z, chunkBB);
			}
		}
	}
}

void OceanMonumentPiece::spawnElder(BlockSource* level, const BoundingBox& chunkBB, int x, int y, int z) {
	float worldX = (float)getWorldX(x, z);
	float worldY = (float)getWorldY(y);
	float worldZ = (float)getWorldZ(x, z);

// 	Guardian* elder = static_cast<Guardian*>(level->getLevel().getSpawner().spawnMob(*level, EntityTypeToString(EntityType::ElderGuardian, EntityTypeNamespaceRules::ReturnWithNamespace), nullptr, Vec3(worldX + 0.5f, worldY, worldZ + 0.5f)));
// 	if (elder) {
// 		elder->setElder(true);
// 		elder->heal((*elder).getMaxHealth());
// 	}
}

bool OceanMonumentPiece::chunkIntersects(const BoundingBox& chunkBB, int x0, int z0, int x1, int z1) {
	int wx0 = getWorldX(x0, z0);
	int wz0 = getWorldZ(x0, z0);
	int wx1 = getWorldX(x1, z1);
	int wz1 = getWorldZ(x1, z1);
	return chunkBB.intersects(Math::min(wx0, wx1), Math::min(wz0, wz1), Math::max(wx0, wx1), Math::max(wz0, wz1));
}

void OceanMonumentPiece::postProcessMobsAt(BlockSource* region, Random& random, const BoundingBox& chunkBB) {
}

///////////////////////////////////////////////////////////////
//MONUMENT BUILDING
MonumentBuilding::MonumentBuilding(Random& random, int west, int north, int& orientation)
	: OceanMonumentPiece(0) {
	this->orientation = orientation;
	if (orientation == Direction::NORTH || orientation == Direction::SOUTH) {
		boundingBox = BoundingBox(west, mTopPosition - mHeight, north, west + mWidth - 1, mTopPosition, north + mDepth - 1);
	}
	else {
		boundingBox = BoundingBox(west, mTopPosition - mHeight, north, west + mDepth - 1, mTopPosition, north + mWidth - 1);
	}

	std::vector<Shared<RoomDefinition>> roomDefinitions = generateRoomGraph(random);

	mSourceRoom->mClaimed = true;
	mChildPieces.push_back(make_unique<OceanMonumentEntryRoom>(orientation, mSourceRoom));
	mChildPieces.push_back(make_unique<OceanMonumentCoreRoom>(orientation, mCoreRoom, random));

	std::vector<Unique<MonumentRoomFitter>> fitters;
	fitters.push_back(make_unique<FitDoubleXYRoom>());
	fitters.push_back(make_unique<FitDoubleYZRoom>());
	fitters.push_back(make_unique<FitDoubleZRoom>());
	fitters.push_back(make_unique<FitDoubleXRoom>());
	fitters.push_back(make_unique<FitDoubleYRoom>());
	fitters.push_back(make_unique<FitSimpleTopRoom>());
	fitters.push_back(make_unique<FitSimpleRoom>());

	for (Shared<RoomDefinition> definition : roomDefinitions) {
		if (!definition->mClaimed && !definition->isSpecial()) {
			for (unsigned i = 0; i < fitters.size(); ++i) {
				if (fitters[i]->fits(definition)) {
					mChildPieces.push_back(fitters[i]->create(orientation, definition, random));
					break;
				}
			}
		}
	}

	// move the room grid relative to this and current orientation
	int dy = boundingBox.y0;
	int dx = getWorldX(25 - 16, 22);
	int dz = getWorldZ(25 - 16, 22);
	for (unsigned i = 0; i < mChildPieces.size(); ++i) {
		mChildPieces[i]->moveBoundingBox(dx, dy, dz);
	}

	// create the special rooms
	BoundingBox leftWing = BoundingBox::createProper(getWorldX(1, 1), getWorldY(1), getWorldZ(1, 1), getWorldX(23, 21), getWorldY(8), getWorldZ(23, 21));
	BoundingBox rightWing = BoundingBox::createProper(getWorldX(34, 1), getWorldY(1), getWorldZ(34, 1), getWorldX(56, 21), getWorldY(8), getWorldZ(56, 21));
	BoundingBox penthouse = BoundingBox::createProper(getWorldX(22, 22), getWorldY(13), getWorldZ(22, 22), getWorldX(35, 35), getWorldY(17), getWorldZ(35, 35));

	// randomize the designs' positions, but always have one of each
	int wingRandom = random.nextInt();

	mChildPieces.push_back(make_unique<OceanMonumentWingRoom>(orientation, leftWing, wingRandom++, false));
	mChildPieces.push_back(make_unique<OceanMonumentWingRoom>(orientation, rightWing, wingRandom++, true));
	mChildPieces.push_back(make_unique<OceanMonumentPenthouse>(orientation, penthouse));
}

MonumentBuilding::MonumentBuilding() {
}

bool MonumentBuilding::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	int waterHeight = Math::max(level->getDimension().getSeaLevel(), 64) - boundingBox.y0;

	generateWaterBox(level, chunkBB, 0, 0, 0, 58, waterHeight, 58, false);

	// left wing
	generateWing(false, 0, level, random, chunkBB);

	// right wing
	generateWing(true, 33, level, random, chunkBB);

	// main entrance (not including entrance wall)
	generateEntranceArchs(level, random, chunkBB);

	generateEntranceWall(level, random, chunkBB);
	generateRoofPiece(level, random, chunkBB);

	generateLowerWall(level, random, chunkBB);
	generateMiddleWall(level, random, chunkBB);
	generateUpperWall(level, random, chunkBB);

	// generate support pillars
	for (int pillarX = 0; pillarX < 7; pillarX++) {
		for (int pillarZ = 0; pillarZ < 7; ) {
			if (pillarZ == 0 && pillarX == 3) {
				// don't place a pillar at the entrance
				pillarZ = 6;
			}

			int bx = pillarX * 9;
			int bz = pillarZ * 9;
			for (int w = 0; w < 4; w++) {
				for (int d = 0; d < 4; d++) {
					placeBlock(level, BASE_LIGHT.id, bx + w, 0, bz + d, chunkBB);
					fillColumnDown(level, BASE_LIGHT, bx + w, -1, bz + d, chunkBB);
				}
			}

			if (pillarX == 0 || pillarX == 6) {
				pillarZ++;
			}
			else {
				pillarZ += 6;
			}
		}
	}

	// cut water slopes on the sides of the monument
	for (int i = 0; i < 5; i++) {
		generateWaterBox(level, chunkBB, -1 - i, 0 + i * 2, -1 - i, -1 - i, 23, 58 + i, false);
		generateWaterBox(level, chunkBB, 58 + i, 0 + i * 2, -1 - i, 58 + i, 23, 58 + i, false);
		generateWaterBox(level, chunkBB, 0 - i, 0 + i * 2, -1 - i, 57 + i, 23, -1 - i, false);
		generateWaterBox(level, chunkBB, 0 - i, 0 + i * 2, 58 + i, 57 + i, 23, 58 + i, false);
	}

	for (unsigned i = 0; i < mChildPieces.size(); ++i) {
		if (mChildPieces[i]->getBoundingBox().intersects(chunkBB)) {
			mChildPieces[i]->postProcess(level, random, chunkBB);
		}
	}
	return true;
}

void MonumentBuilding::postProcessMobsAt(BlockSource* region, Random& random, const BoundingBox& chunkBB) {
	EntityList guardiansInChunk = region->getEntities(EntityType::Guardian, AABB((float)chunkBB.x0, (float)chunkBB.y0, (float)chunkBB.z0, (float)chunkBB.x1, (float)chunkBB.y1, (float)chunkBB.z1));
	int guardianCount = guardiansInChunk.size();
	while (guardianCount <= 3) {
		Vec3 spawnPos;
		spawnPos.x = random.nextFloat((float)boundingBox.x0, (float)boundingBox.x1);
		spawnPos.y = random.nextFloat((float)boundingBox.y0, (float)boundingBox.y1);
		spawnPos.z = random.nextFloat((float)boundingBox.z0, (float)boundingBox.z1);
	
		// TODO: rherlitz
// 		if (chunkBB.isInside(spawnPos)) {
// 			region->getLevel().getSpawner().spawnMob(*region, EntityTypeToString(EntityType::Guardian, EntityTypeNamespaceRules::ReturnWithNamespace), nullptr, spawnPos);
// 			++guardianCount;
// 		}
	}

	std::vector<Unique<OceanMonumentPiece>>::iterator it = mChildPieces.begin();

	while (it != mChildPieces.end()) {
		Unique<OceanMonumentPiece>& next = (*it);
		next->postProcessMobsAt(region, random, chunkBB);
		++it;
	}
}

std::vector<Shared<RoomDefinition>> MonumentBuilding::generateRoomGraph(Random& random) {
	mRoomGrid.reserve(OceanMonumentPiece::mGridSize);

	for (unsigned i = 0; i < OceanMonumentPiece::mGridSize; ++i) {
		mRoomGrid.push_back(nullptr);
	}

	for (int x = 0; x < mGridWidth; x++) {
		for (int z = 0; z < 4; z++) {
			int y = 0;
			int pos = getRoomIndex(x, y, z);
			mRoomGrid[pos] = make_shared<RoomDefinition>(pos);
		}
	}
	for (int x = 0; x < mGridWidth; x++) {
		for (int z = 0; z < 4; z++) {
			int y = 1;
			int pos = getRoomIndex(x, y, z);
			mRoomGrid[pos] = make_shared<RoomDefinition>(pos);
		}
	}
	for (int x = 1; x < (mGridWidth - 1); x++) {
		for (int z = 0; z < 2; z++) {
			int y = 2;
			int pos = getRoomIndex(x, y, z);
			mRoomGrid[pos] = make_shared<RoomDefinition>(pos);
		}
	}

	mSourceRoom = mRoomGrid[mGridroomSourceIndex];

	for (int x = 0; x < 5; x++) {
		for (int z = 0; z < 5; z++) {
			for (int y = 0; y < 3; y++) {
				int pos = getRoomIndex(x, y, z);
				if (mRoomGrid[pos] == nullptr) {
					continue;
				}
				for (const FacingID& direction : Facing::ALL_FACES) {
					int neighX = x + Facing::getStepX(direction);
					int neighY = y + Facing::getStepY(direction);
					int neighZ = z + Facing::getStepZ(direction);
					if (neighX >= 0 && neighX < mGridWidth && neighZ >= 0 && neighZ < mGridDepth && neighY >= 0 && neighY < mGridHeight) {
						int neighPos = getRoomIndex(neighX, neighY, neighZ);
						if (mRoomGrid[neighPos] == nullptr) {
							continue;
						}
						if (neighZ == z) {
							mRoomGrid[pos]->setConnection(direction, mRoomGrid[neighPos]);
						}
						else {
							FacingID oppDir = Facing::OPPOSITE_FACING[direction];
							mRoomGrid[pos]->setConnection(oppDir, mRoomGrid[neighPos]);
						}
					}
				}
			}
		}
	}
	Shared<RoomDefinition> roofRoom = make_shared<RoomDefinition>(OceanMonumentPiece::mPenthouseIndex);
	Shared<RoomDefinition> leftWing = make_shared<RoomDefinition>(OceanMonumentPiece::mLeftWingIndex);
	Shared<RoomDefinition> rightWing = make_shared<RoomDefinition>(OceanMonumentPiece::mRightWingIndex);
	FacingID up = Facing::UP;
	FacingID south = Facing::SOUTH;
	mRoomGrid[mGridroomTopConnectIndex]->setConnection(up, roofRoom);
	mRoomGrid[mGridroomLeftWingConnectIndex]->setConnection(south, leftWing);
	mRoomGrid[mGridroomRightWingConnectIndex]->setConnection(south, rightWing);
	roofRoom->mClaimed = true;
	leftWing->mClaimed = true;
	rightWing->mClaimed = true;
	mSourceRoom->mIsSource = true;

	// claim one room for the core
	int roomIndex = getRoomIndex(random.nextInt(mGridWidth - 1), 0, 2);
	mCoreRoom = mRoomGrid[roomIndex];
	mCoreRoom->mClaimed = true;
	mCoreRoom->mConnections[Facing::EAST]->mClaimed = true;
	mCoreRoom->mConnections[Facing::NORTH]->mClaimed = true;
	mCoreRoom->mConnections[Facing::EAST]->mConnections[Facing::NORTH]->mClaimed = true;
	mCoreRoom->mConnections[Facing::UP]->mClaimed = true;
	mCoreRoom->mConnections[Facing::EAST]->mConnections[Facing::UP]->mClaimed = true;
	mCoreRoom->mConnections[Facing::NORTH]->mConnections[Facing::UP]->mClaimed = true;
	mCoreRoom->mConnections[Facing::EAST]->mConnections[Facing::NORTH]->mConnections[Facing::UP]->mClaimed = true;

	std::vector<Shared<RoomDefinition>> roomDefs;
	for (unsigned i = 0; i < mRoomGrid.size(); ++i) {
		if (mRoomGrid[i] != nullptr) {
			mRoomGrid[i]->updateOpenings();
			roomDefs.push_back(mRoomGrid[i]);
		}
	}
	roofRoom->updateOpenings();

	std::random_shuffle(roomDefs.begin(), roomDefs.end());
	int scanIndex = 1;
	for (unsigned i = 0; i < roomDefs.size(); ++i) {
		// attempt to randomly close two openings
		int closeCount = 0;
		int attemptCount = 0;
		while (closeCount < 2 && attemptCount < 5) {
			attemptCount++;

			int randDir = random.nextInt(6);
			if (roomDefs[i]->mHasOpening[randDir]) {
				int oppRandDir = Facing::OPPOSITE_FACING[randDir];

				// temporarily close this one
				roomDefs[i]->mHasOpening[randDir] = false;
				roomDefs[i]->mConnections[randDir]->mHasOpening[oppRandDir] = false;

				if (roomDefs[i]->findSource(scanIndex++) && roomDefs[i]->mConnections[randDir]->findSource(scanIndex++)) {
					closeCount++;
				}
				else {
					// failed, re-open
					roomDefs[i]->mHasOpening[randDir] = true;
					roomDefs[i]->mConnections[randDir]->mHasOpening[oppRandDir] = true;
				}
			}
		}
	}
	roomDefs.push_back(roofRoom);
	roomDefs.push_back(leftWing);
	roomDefs.push_back(rightWing);

	return roomDefs;
}

void MonumentBuilding::generateWing(bool isFlipped, int xoff, BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	int sectionWidth = 24;
	if (chunkIntersects(chunkBB, xoff, 0, xoff + 23, 20)) {
		generateBox(level, chunkBB, xoff + 0, 0, 0, xoff + 24, 0, 20, BASE_GRAY, BASE_GRAY, false);
		if (mDoFill) {
			generateWaterBox(level, chunkBB, xoff + 0, 1, 0, xoff + 24, 10, 20, false);
		}
		for (int i = 0; i < 4; i++) {
			generateBox(level, chunkBB, xoff + i, i + 1, i, xoff + i, i + 1, 20, BASE_LIGHT, BASE_LIGHT, false);
			generateBox(level, chunkBB, xoff + i + 7, i + 5, i + 7, xoff + i + 7, i + 5, 20, BASE_LIGHT, BASE_LIGHT, false);
			generateBox(level, chunkBB, xoff + 17 - i, i + 5, i + 7, xoff + 17 - i, i + 5, 20, BASE_LIGHT, BASE_LIGHT, false);
			generateBox(level, chunkBB, xoff + 24 - i, i + 1, i, xoff + 24 - i, i + 1, 20, BASE_LIGHT, BASE_LIGHT, false);
			generateBox(level, chunkBB, xoff + i + 1, i + 1, i, xoff + 23 - i, i + 1, i, BASE_LIGHT, BASE_LIGHT, false);
			generateBox(level, chunkBB, xoff + i + 8, i + 5, i + 7, xoff + 16 - i, i + 5, i + 7, BASE_LIGHT, BASE_LIGHT, false);
		}
		generateBox(level, chunkBB, xoff + 4, 4, 4, xoff + 6, 4, 20, BASE_GRAY, BASE_GRAY, false);
		generateBox(level, chunkBB, xoff + 7, 4, 4, xoff + 17, 4, 6, BASE_GRAY, BASE_GRAY, false);
		generateBox(level, chunkBB, xoff + 18, 4, 4, xoff + 20, 4, 20, BASE_GRAY, BASE_GRAY, false);
		generateBox(level, chunkBB, xoff + 11, 8, 11, xoff + 13, 8, 20, BASE_GRAY, BASE_GRAY, false);
		placeBlock(level, DOT_DECO_DATA, xoff + 12, 9, 12, chunkBB);
		placeBlock(level, DOT_DECO_DATA, xoff + 12, 9, 15, chunkBB);
		placeBlock(level, DOT_DECO_DATA, xoff + 12, 9, 18, chunkBB);

		int leftPos = xoff + (isFlipped ? sectionWidth - 5 : 5);
		int rightPos = xoff + (isFlipped ? sectionWidth - 19 : 19);
		for (int z = 20; z >= 5; z -= 3) {
			placeBlock(level, DOT_DECO_DATA, leftPos, 5, z, chunkBB);
		}
		for (int z = 19; z >= 7; z -= 3) {
			placeBlock(level, DOT_DECO_DATA, rightPos, 5, z, chunkBB);
		}
		for (int i = 0; i < 4; i++) {
			int pos = isFlipped ? xoff + (sectionWidth - (17 - i * 3)) : xoff + 17 - i * 3;
			placeBlock(level, DOT_DECO_DATA, pos, 5, 5, chunkBB);
		}
		placeBlock(level, DOT_DECO_DATA, rightPos, 5, 5, chunkBB);

		// inside pillars
		generateBox(level, chunkBB, xoff + 11, 1, 12, xoff + 13, 7, 12, BASE_GRAY, BASE_GRAY, false);
		generateBox(level, chunkBB, xoff + 12, 1, 11, xoff + 12, 7, 13, BASE_GRAY, BASE_GRAY, false);
	}
}

void MonumentBuilding::generateEntranceArchs(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	// size: (22, 2, 5) - (35, 6, 17)
	if (chunkIntersects(chunkBB, 22, 5, 35, 17)) {
		if (mDoFill) {
			generateWaterBox(level, chunkBB, 25, 0, 0, 32, 8, 20, false);
		}

		for (int i = 0; i < 4; i++) {
			generateBox(level, chunkBB, 24, 2, 5 + i * 4, 24, 4, 5 + i * 4, BASE_LIGHT, BASE_LIGHT, false);
			generateBox(level, chunkBB, 22, 4, 5 + i * 4, 23, 4, 5 + i * 4, BASE_LIGHT, BASE_LIGHT, false);
			placeBlock(level, BASE_LIGHT, 25, 5, 5 + i * 4, chunkBB);
			placeBlock(level, BASE_LIGHT, 26, 6, 5 + i * 4, chunkBB);
			placeBlock(level, LAMP_BLOCK, 26, 5, 5 + i * 4, chunkBB);

			generateBox(level, chunkBB, 33, 2, 5 + i * 4, 33, 4, 5 + i * 4, BASE_LIGHT, BASE_LIGHT, false);
			generateBox(level, chunkBB, 34, 4, 5 + i * 4, 35, 4, 5 + i * 4, BASE_LIGHT, BASE_LIGHT, false);
			placeBlock(level, BASE_LIGHT, 32, 5, 5 + i * 4, chunkBB);
			placeBlock(level, BASE_LIGHT, 31, 6, 5 + i * 4, chunkBB);
			placeBlock(level, LAMP_BLOCK, 31, 5, 5 + i * 4, chunkBB);

			generateBox(level, chunkBB, 27, 6, 5 + i * 4, 30, 6, 5 + i * 4, BASE_GRAY, BASE_GRAY, false);
		}
	}
}

void MonumentBuilding::generateEntranceWall(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	// entrance wall
	// size: (15, 0, 20) - (42, 9, 21)
	if (chunkIntersects(chunkBB, 15, 20, 42, 21)) {
		generateBox(level, chunkBB, 15, 0, 21, 42, 0, 21, BASE_GRAY, BASE_GRAY, false);
		if (mDoFill) {
			generateWaterBox(level, chunkBB, 26, 1, 21, 31, 3, 21, false);
		}

		// dark wall
		generateBox(level, chunkBB, 21, 12, 21, 36, 12, 21, BASE_GRAY, BASE_GRAY, false);
		generateBox(level, chunkBB, 17, 11, 21, 40, 11, 21, BASE_GRAY, BASE_GRAY, false);
		generateBox(level, chunkBB, 16, 10, 21, 41, 10, 21, BASE_GRAY, BASE_GRAY, false);
		generateBox(level, chunkBB, 15, 7, 21, 42, 9, 21, BASE_GRAY, BASE_GRAY, false);
		generateBox(level, chunkBB, 16, 6, 21, 41, 6, 21, BASE_GRAY, BASE_GRAY, false);
		generateBox(level, chunkBB, 17, 5, 21, 40, 5, 21, BASE_GRAY, BASE_GRAY, false);
		generateBox(level, chunkBB, 21, 4, 21, 36, 4, 21, BASE_GRAY, BASE_GRAY, false);
		generateBox(level, chunkBB, 22, 3, 21, 26, 3, 21, BASE_GRAY, BASE_GRAY, false);
		generateBox(level, chunkBB, 31, 3, 21, 35, 3, 21, BASE_GRAY, BASE_GRAY, false);
		generateBox(level, chunkBB, 23, 2, 21, 25, 2, 21, BASE_GRAY, BASE_GRAY, false);
		generateBox(level, chunkBB, 32, 2, 21, 34, 2, 21, BASE_GRAY, BASE_GRAY, false);

		// decorations
		generateBox(level, chunkBB, 28, 4, 20, 29, 4, 21, BASE_LIGHT, BASE_LIGHT, false);
		placeBlock(level, BASE_LIGHT, 27, 3, 21, chunkBB);
		placeBlock(level, BASE_LIGHT, 30, 3, 21, chunkBB);
		placeBlock(level, BASE_LIGHT, 26, 2, 21, chunkBB);
		placeBlock(level, BASE_LIGHT, 31, 2, 21, chunkBB);
		placeBlock(level, BASE_LIGHT, 25, 1, 21, chunkBB);
		placeBlock(level, BASE_LIGHT, 32, 1, 21, chunkBB);
		for (int i = 0; i < 7; i++) {
			placeBlock(level, BASE_BLACK, 28 - i, 6 + i, 21, chunkBB);
			placeBlock(level, BASE_BLACK, 29 + i, 6 + i, 21, chunkBB);
		}
		for (int i = 0; i < 4; i++) {
			placeBlock(level, BASE_BLACK, 28 - i, 9 + i, 21, chunkBB);
			placeBlock(level, BASE_BLACK, 29 + i, 9 + i, 21, chunkBB);
		}
		placeBlock(level, BASE_BLACK, 28, 12, 21, chunkBB);
		placeBlock(level, BASE_BLACK, 29, 12, 21, chunkBB);
		for (int i = 0; i < 3; i++) {
			placeBlock(level, BASE_BLACK, 22 - i * 2, 8, 21, chunkBB);
			placeBlock(level, BASE_BLACK, 22 - i * 2, 9, 21, chunkBB);

			placeBlock(level, BASE_BLACK, 35 + i * 2, 8, 21, chunkBB);
			placeBlock(level, BASE_BLACK, 35 + i * 2, 9, 21, chunkBB);
		}

		if (mDoFill) {
			// fill insides
			generateWaterBox(level, chunkBB, 15, 13, 21, 42, 15, 21, false);
			generateWaterBox(level, chunkBB, 15, 1, 21, 15, 6, 21, false);
			generateWaterBox(level, chunkBB, 16, 1, 21, 16, 5, 21, false);
			generateWaterBox(level, chunkBB, 17, 1, 21, 20, 4, 21, false);
			generateWaterBox(level, chunkBB, 21, 1, 21, 21, 3, 21, false);
			generateWaterBox(level, chunkBB, 22, 1, 21, 22, 2, 21, false);
			generateWaterBox(level, chunkBB, 23, 1, 21, 24, 1, 21, false);
			generateWaterBox(level, chunkBB, 42, 1, 21, 42, 6, 21, false);
			generateWaterBox(level, chunkBB, 41, 1, 21, 41, 5, 21, false);
			generateWaterBox(level, chunkBB, 37, 1, 21, 40, 4, 21, false);
			generateWaterBox(level, chunkBB, 36, 1, 21, 36, 3, 21, false);
			generateWaterBox(level, chunkBB, 33, 1, 21, 34, 1, 21, false);
			generateWaterBox(level, chunkBB, 35, 1, 21, 35, 2, 21, false);
		}
	}
}

void MonumentBuilding::generateRoofPiece(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	// roof piece
	// size: (21, 13, 21) - (36, 21, 36)
	if (chunkIntersects(chunkBB, 21, 21, 36, 36)) {
		generateBox(level, chunkBB, 21, 0, 22, 36, 0, 36, BASE_GRAY, BASE_GRAY, false);

		// water
		if (mDoFill) {
			generateWaterBox(level, chunkBB, 21, 1, 22, 36, 23, 36, false);
		}

		for (int i = 0; i < 4; i++) {
			generateBox(level, chunkBB, 21 + i, 13 + i, 21 + i, 36 - i, 13 + i, 21 + i, BASE_LIGHT, BASE_LIGHT, false);
			generateBox(level, chunkBB, 21 + i, 13 + i, 36 - i, 36 - i, 13 + i, 36 - i, BASE_LIGHT, BASE_LIGHT, false);
			generateBox(level, chunkBB, 21 + i, 13 + i, 22 + i, 21 + i, 13 + i, 35 - i, BASE_LIGHT, BASE_LIGHT, false);
			generateBox(level, chunkBB, 36 - i, 13 + i, 22 + i, 36 - i, 13 + i, 35 - i, BASE_LIGHT, BASE_LIGHT, false);
		}
		generateBox(level, chunkBB, 25, 16, 25, 32, 16, 32, BASE_GRAY, BASE_GRAY, false);
		generateBox(level, chunkBB, 25, 17, 25, 25, 19, 25, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 32, 17, 25, 32, 19, 25, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 25, 17, 32, 25, 19, 32, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 32, 17, 32, 32, 19, 32, BASE_LIGHT, BASE_LIGHT, false);

		placeBlock(level, BASE_LIGHT, 26, 20, 26, chunkBB);
		placeBlock(level, BASE_LIGHT, 27, 21, 27, chunkBB);
		placeBlock(level, LAMP_BLOCK, 27, 20, 27, chunkBB);
		placeBlock(level, BASE_LIGHT, 26, 20, 31, chunkBB);
		placeBlock(level, BASE_LIGHT, 27, 21, 30, chunkBB);
		placeBlock(level, LAMP_BLOCK, 27, 20, 30, chunkBB);
		placeBlock(level, BASE_LIGHT, 31, 20, 31, chunkBB);
		placeBlock(level, BASE_LIGHT, 30, 21, 30, chunkBB);
		placeBlock(level, LAMP_BLOCK, 30, 20, 30, chunkBB);
		placeBlock(level, BASE_LIGHT, 31, 20, 26, chunkBB);
		placeBlock(level, BASE_LIGHT, 30, 21, 27, chunkBB);
		placeBlock(level, LAMP_BLOCK, 30, 20, 27, chunkBB);

		generateBox(level, chunkBB, 28, 21, 27, 29, 21, 27, BASE_GRAY, BASE_GRAY, false);
		generateBox(level, chunkBB, 27, 21, 28, 27, 21, 29, BASE_GRAY, BASE_GRAY, false);
		generateBox(level, chunkBB, 28, 21, 30, 29, 21, 30, BASE_GRAY, BASE_GRAY, false);
		generateBox(level, chunkBB, 30, 21, 28, 30, 21, 29, BASE_GRAY, BASE_GRAY, false);
	}
}

void MonumentBuilding::generateLowerWall(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	// left lower wall
	// size: (0, 1, 21) - (6, 5, 57)
	if (chunkIntersects(chunkBB, 0, 21, 6, 58)) {
		generateBox(level, chunkBB, 0, 0, 21, 6, 0, 57, BASE_GRAY, BASE_GRAY, false);
		if (mDoFill) {
			generateWaterBox(level, chunkBB, 0, 1, 21, 6, 7, 57, false);
		}

		generateBox(level, chunkBB, 4, 4, 21, 6, 4, 53, BASE_GRAY, BASE_GRAY, false);
		for (int i = 0; i < 4; i++) {
			generateBox(level, chunkBB, i, i + 1, 21, i, i + 1, 57 - i, BASE_LIGHT, BASE_LIGHT, false);
		}
		for (int z = 23; z < 53; z += 3) {
			placeBlock(level, DOT_DECO_DATA, 5, 5, z, chunkBB);
		}
		placeBlock(level, DOT_DECO_DATA, 5, 5, 52, chunkBB);

		for (int i = 0; i < 4; i++) {
			generateBox(level, chunkBB, i, i + 1, 21, i, i + 1, 57 - i, BASE_LIGHT, BASE_LIGHT, false);
		}
		// inside pillars
		generateBox(level, chunkBB, 4, 1, 52, 6, 3, 52, BASE_GRAY, BASE_GRAY, false);
		generateBox(level, chunkBB, 5, 1, 51, 5, 3, 53, BASE_GRAY, BASE_GRAY, false);
	}

	// right lower wall
	// size: (51, 1, 21) - (57, 5, 57)
	if (chunkIntersects(chunkBB, 51, 21, 58, 58)) {
		generateBox(level, chunkBB, 51, 0, 21, 57, 0, 57, BASE_GRAY, BASE_GRAY, false);
		if (mDoFill) {
			generateWaterBox(level, chunkBB, 51, 1, 21, 57, 7, 57, false);
		}

		generateBox(level, chunkBB, 51, 4, 21, 53, 4, 53, BASE_GRAY, BASE_GRAY, false);
		for (int i = 0; i < 4; i++) {
			generateBox(level, chunkBB, 57 - i, i + 1, 21, 57 - i, i + 1, 57 - i, BASE_LIGHT, BASE_LIGHT, false);
		}
		for (int z = 23; z < 53; z += 3) {
			placeBlock(level, DOT_DECO_DATA, 52, 5, z, chunkBB);
		}
		placeBlock(level, DOT_DECO_DATA, 52, 5, 52, chunkBB);

		// inside pillars
		generateBox(level, chunkBB, 51, 1, 52, 53, 3, 52, BASE_GRAY, BASE_GRAY, false);
		generateBox(level, chunkBB, 52, 1, 51, 52, 3, 53, BASE_GRAY, BASE_GRAY, false);
	}

	// far lower wall
	// size: (0, 1, 51) - (57, 5, 57)
	if (chunkIntersects(chunkBB, 0, 51, 57, 57)) {
		generateBox(level, chunkBB, 7, 0, 51, 50, 0, 57, BASE_GRAY, BASE_GRAY, false);
		if (mDoFill) {
			generateWaterBox(level, chunkBB, 7, 1, 51, 50, 10, 57, false);
		}

		for (int i = 0; i < 4; i++) {
			generateBox(level, chunkBB, i + 1, i + 1, 57 - i, 56 - i, i + 1, 57 - i, BASE_LIGHT, BASE_LIGHT, false);
		}
	}
}

void MonumentBuilding::generateMiddleWall(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	// left middle wall
	// size: (7, 6, 21) - (13, 9, 50)
	if (chunkIntersects(chunkBB, 7, 21, 13, 54)) {
		generateBox(level, chunkBB, 7, 0, 21, 13, 0, 50, BASE_GRAY, BASE_GRAY, false);
		if (mDoFill) {
			generateWaterBox(level, chunkBB, 7, 1, 21, 13, 10, 50, false);
		}

		generateBox(level, chunkBB, 11, 8, 21, 13, 8, 53, BASE_GRAY, BASE_GRAY, false);
		for (int i = 0; i < 4; i++) {
			generateBox(level, chunkBB, i + 7, i + 5, 21, i + 7, i + 5, 54, BASE_LIGHT, BASE_LIGHT, false);
		}
		for (int z = 21; z <= 45; z += 3) {
			placeBlock(level, DOT_DECO_DATA, 12, 9, z, chunkBB);
		}
	}

	// right middle wall
	// size: (44, 6, 21) - (50, 9, 50)
	if (chunkIntersects(chunkBB, 44, 21, 50, 54)) {
		generateBox(level, chunkBB, 44, 0, 21, 50, 0, 50, BASE_GRAY, BASE_GRAY, false);
		if (mDoFill) {
			generateWaterBox(level, chunkBB, 44, 1, 21, 50, 10, 50, false);
		}

		generateBox(level, chunkBB, 44, 8, 21, 46, 8, 53, BASE_GRAY, BASE_GRAY, false);
		for (int i = 0; i < 4; i++) {
			generateBox(level, chunkBB, 50 - i, i + 5, 21, 50 - i, i + 5, 54, BASE_LIGHT, BASE_LIGHT, false);
		}
		for (int z = 21; z <= 45; z += 3) {
			placeBlock(level, DOT_DECO_DATA, 45, 9, z, chunkBB);
		}
	}

	// far middle wall
	// size: (8, 6, 47) - (49, 9, 50)
	if (chunkIntersects(chunkBB, 8, 44, 49, 54)) {
		generateBox(level, chunkBB, 14, 0, 44, 43, 0, 50, BASE_GRAY, BASE_GRAY, false);
		if (mDoFill) {
			generateWaterBox(level, chunkBB, 14, 1, 44, 43, 10, 50, false);
		}

		for (int x = 12; x <= 45; x += 3) {
			placeBlock(level, DOT_DECO_DATA, x, 9, 45, chunkBB);
			placeBlock(level, DOT_DECO_DATA, x, 9, 52, chunkBB);
			if (x == 12 || x == 18 || x == 24 || x == 33 || x == 39 || x == 45) {
				placeBlock(level, DOT_DECO_DATA, x, 9, 47, chunkBB);
				placeBlock(level, DOT_DECO_DATA, x, 9, 50, chunkBB);
				placeBlock(level, DOT_DECO_DATA, x, 10, 45, chunkBB);
				placeBlock(level, DOT_DECO_DATA, x, 10, 46, chunkBB);
				placeBlock(level, DOT_DECO_DATA, x, 10, 51, chunkBB);
				placeBlock(level, DOT_DECO_DATA, x, 10, 52, chunkBB);
				placeBlock(level, DOT_DECO_DATA, x, 11, 47, chunkBB);
				placeBlock(level, DOT_DECO_DATA, x, 11, 50, chunkBB);
				placeBlock(level, DOT_DECO_DATA, x, 12, 48, chunkBB);
				placeBlock(level, DOT_DECO_DATA, x, 12, 49, chunkBB);
			}
		}

		for (int i = 0; i < 3; i++) {
			generateBox(level, chunkBB, 8 + i, 5 + i, 54, 49 - i, 5 + i, 54, BASE_GRAY, BASE_GRAY, false);
		}
		generateBox(level, chunkBB, 11, 8, 54, 46, 8, 54, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 14, 8, 44, 43, 8, 53, BASE_GRAY, BASE_GRAY, false);
	}
}

void MonumentBuilding::generateUpperWall(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	// left upper wall
	// size: (14, 9, 21) - (20, 13, 43)
	if (chunkIntersects(chunkBB, 14, 21, 20, 43)) {
		generateBox(level, chunkBB, 14, 0, 21, 20, 0, 43, BASE_GRAY, BASE_GRAY, false);
		if (mDoFill) {
			generateWaterBox(level, chunkBB, 14, 1, 22, 20, 14, 43, false);
		}

		generateBox(level, chunkBB, 18, 12, 22, 20, 12, 39, BASE_GRAY, BASE_GRAY, false);
		generateBox(level, chunkBB, 18, 12, 21, 20, 12, 21, BASE_LIGHT, BASE_LIGHT, false);
		for (int i = 0; i < 4; i++) {
			generateBox(level, chunkBB, i + 14, i + 9, 21, i + 14, i + 9, 43 - i, BASE_LIGHT, BASE_LIGHT, false);
		}
		for (int z = 23; z <= 39; z += 3) {
			placeBlock(level, DOT_DECO_DATA, 19, 13, z, chunkBB);
		}
	}

	// right upper wall
	// size: (37, 9, 21) - (43, 13, 43)
	if (chunkIntersects(chunkBB, 37, 21, 43, 43)) {
		generateBox(level, chunkBB, 37, 0, 21, 43, 0, 43, BASE_GRAY, BASE_GRAY, false);
		if (mDoFill) {
			generateWaterBox(level, chunkBB, 37, 1, 22, 43, 14, 43, false);
		}

		generateBox(level, chunkBB, 37, 12, 22, 39, 12, 39, BASE_GRAY, BASE_GRAY, false);
		generateBox(level, chunkBB, 37, 12, 21, 39, 12, 21, BASE_LIGHT, BASE_LIGHT, false);
		for (int i = 0; i < 4; i++) {
			generateBox(level, chunkBB, 43 - i, i + 9, 21, 43 - i, i + 9, 43 - i, BASE_LIGHT, BASE_LIGHT, false);
		}
		for (int z = 23; z <= 39; z += 3) {
			placeBlock(level, DOT_DECO_DATA, 38, 13, z, chunkBB);
		}
	}

	// far upper wall
	// size: (15, 9, 37) - (42, 13, 43)
	if (chunkIntersects(chunkBB, 15, 37, 42, 43)) {
		generateBox(level, chunkBB, 21, 0, 37, 36, 0, 43, BASE_GRAY, BASE_GRAY, false);
		if (mDoFill) {
			generateWaterBox(level, chunkBB, 21, 1, 37, 36, 14, 43, false);
		}

		generateBox(level, chunkBB, 21, 12, 37, 36, 12, 39, BASE_GRAY, BASE_GRAY, false);
		for (int i = 0; i < 4; i++) {
			generateBox(level, chunkBB, 15 + i, i + 9, 43 - i, 42 - i, i + 9, 43 - i, BASE_LIGHT, BASE_LIGHT, false);
		}
		for (int x = 21; x <= 36; x += 3) {
			placeBlock(level, DOT_DECO_DATA, x, 13, 38, chunkBB);
		}
	}
}

///////////////////////////////////////////////////////////////
//ENTRY ROOM
OceanMonumentEntryRoom::OceanMonumentEntryRoom() {
}

OceanMonumentEntryRoom::OceanMonumentEntryRoom(int& orientation, Shared<RoomDefinition>& definition)
	: OceanMonumentPiece(1, orientation, definition, 1, 1, 1) {
}

bool OceanMonumentEntryRoom::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	// slope roof
	generateBox(level, chunkBB, 0, 3, 0, 2, 3, 7, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 5, 3, 0, 7, 3, 7, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 0, 2, 0, 1, 2, 7, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 6, 2, 0, 7, 2, 7, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 0, 1, 0, 0, 1, 7, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 7, 1, 0, 7, 1, 7, BASE_LIGHT, BASE_LIGHT, false);

	// far wall																 
	generateBox(level, chunkBB, 0, 1, 7, 7, 3, 7, BASE_LIGHT, BASE_LIGHT, false);

	// near wall															 
	generateBox(level, chunkBB, 1, 1, 0, 2, 3, 0, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 5, 1, 0, 6, 3, 0, BASE_LIGHT, BASE_LIGHT, false);

	if ((*mRoomDefinition).mHasOpening[Facing::NORTH]) {
		generateWaterBox(level, chunkBB, 3, 1, 7, 4, 2, 7, false);
	}
	if ((*mRoomDefinition).mHasOpening[Facing::WEST]) {
		generateWaterBox(level, chunkBB, 0, 1, 3, 1, 2, 4, false);
	}
	if ((*mRoomDefinition).mHasOpening[Facing::EAST]) {
		generateWaterBox(level, chunkBB, 6, 1, 3, 7, 2, 4, false);
	}

	return true;
}
///////////////////////////////////////////////////////////////
//SIMPLE ROOM
OceanMonumentSimpleRoom::OceanMonumentSimpleRoom() {
}

OceanMonumentSimpleRoom::OceanMonumentSimpleRoom(int& orientation, Shared<RoomDefinition>& definition, Random& random)
	: OceanMonumentPiece(1, orientation, definition, 1, 1, 1) {
	mMainDesign = random.nextInt(3);
}

bool OceanMonumentSimpleRoom::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	if (((*mRoomDefinition).mIndex / mGridFloorCount) > 0) {
		generateDefaultFloor(level, chunkBB, 0, 0, (*mRoomDefinition).mHasOpening[Facing::DOWN]);
	}
	if ((*mRoomDefinition).mConnections[Facing::UP] == nullptr) {
		generateBoxOnFillOnly(level, chunkBB, 1, 4, 1, 6, 4, 6, BASE_GRAY);
	}

	bool centerPillar = mMainDesign != 0 && random.nextBoolean() && !(*mRoomDefinition).mHasOpening[Facing::DOWN] && !(*mRoomDefinition).mHasOpening[Facing::UP] && (*mRoomDefinition).countOpenings() > 1;

	if (mMainDesign == 0) {
		// back left
		generateBox(level, chunkBB, 0, 1, 0, 2, 1, 2, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 0, 3, 0, 2, 3, 2, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 0, 2, 0, 0, 2, 2, BASE_GRAY, BASE_GRAY, false);
		generateBox(level, chunkBB, 1, 2, 0, 2, 2, 0, BASE_GRAY, BASE_GRAY, false);
		placeBlock(level, LAMP_BLOCK, 1, 2, 1, chunkBB);

		// back right
		generateBox(level, chunkBB, 5, 1, 0, 7, 1, 2, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 5, 3, 0, 7, 3, 2, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 7, 2, 0, 7, 2, 2, BASE_GRAY, BASE_GRAY, false);
		generateBox(level, chunkBB, 5, 2, 0, 6, 2, 0, BASE_GRAY, BASE_GRAY, false);
		placeBlock(level, LAMP_BLOCK, 6, 2, 1, chunkBB);

		// front left
		generateBox(level, chunkBB, 0, 1, 5, 2, 1, 7, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 0, 3, 5, 2, 3, 7, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 0, 2, 5, 0, 2, 7, BASE_GRAY, BASE_GRAY, false);
		generateBox(level, chunkBB, 1, 2, 7, 2, 2, 7, BASE_GRAY, BASE_GRAY, false);
		placeBlock(level, LAMP_BLOCK, 1, 2, 6, chunkBB);

		// front right
		generateBox(level, chunkBB, 5, 1, 5, 7, 1, 7, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 5, 3, 5, 7, 3, 7, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 7, 2, 5, 7, 2, 7, BASE_GRAY, BASE_GRAY, false);
		generateBox(level, chunkBB, 5, 2, 7, 6, 2, 7, BASE_GRAY, BASE_GRAY, false);
		placeBlock(level, LAMP_BLOCK, 6, 2, 6, chunkBB);

		if ((*mRoomDefinition).mHasOpening[Facing::SOUTH]) {
			generateBox(level, chunkBB, 3, 3, 0, 4, 3, 0, BASE_LIGHT, BASE_LIGHT, false);
		}
		else {
			generateBox(level, chunkBB, 3, 3, 0, 4, 3, 1, BASE_LIGHT, BASE_LIGHT, false);
			generateBox(level, chunkBB, 3, 2, 0, 4, 2, 0, BASE_GRAY, BASE_GRAY, false);
			generateBox(level, chunkBB, 3, 1, 0, 4, 1, 1, BASE_LIGHT, BASE_LIGHT, false);
		}
		if ((*mRoomDefinition).mHasOpening[Facing::NORTH]) {
			generateBox(level, chunkBB, 3, 3, 7, 4, 3, 7, BASE_LIGHT, BASE_LIGHT, false);
		}
		else {
			generateBox(level, chunkBB, 3, 3, 6, 4, 3, 7, BASE_LIGHT, BASE_LIGHT, false);
			generateBox(level, chunkBB, 3, 2, 7, 4, 2, 7, BASE_GRAY, BASE_GRAY, false);
			generateBox(level, chunkBB, 3, 1, 6, 4, 1, 7, BASE_LIGHT, BASE_LIGHT, false);
		}
		if ((*mRoomDefinition).mHasOpening[Facing::WEST]) {
			generateBox(level, chunkBB, 0, 3, 3, 0, 3, 4, BASE_LIGHT, BASE_LIGHT, false);
		}
		else {
			generateBox(level, chunkBB, 0, 3, 3, 1, 3, 4, BASE_LIGHT, BASE_LIGHT, false);
			generateBox(level, chunkBB, 0, 2, 3, 0, 2, 4, BASE_GRAY, BASE_GRAY, false);
			generateBox(level, chunkBB, 0, 1, 3, 1, 1, 4, BASE_LIGHT, BASE_LIGHT, false);
		}
		if ((*mRoomDefinition).mHasOpening[Facing::EAST]) {
			generateBox(level, chunkBB, 7, 3, 3, 7, 3, 4, BASE_LIGHT, BASE_LIGHT, false);
		}
		else {
			generateBox(level, chunkBB, 6, 3, 3, 7, 3, 4, BASE_LIGHT, BASE_LIGHT, false);
			generateBox(level, chunkBB, 7, 2, 3, 7, 2, 4, BASE_GRAY, BASE_GRAY, false);
			generateBox(level, chunkBB, 6, 1, 3, 7, 1, 4, BASE_LIGHT, BASE_LIGHT, false);
		}
	}
	else if (mMainDesign == 1) {
		// light pillars
		generateBox(level, chunkBB, 2, 1, 2, 2, 3, 2, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 2, 1, 5, 2, 3, 5, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 5, 1, 5, 5, 3, 5, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 5, 1, 2, 5, 3, 2, BASE_LIGHT, BASE_LIGHT, false);
		placeBlock(level, LAMP_BLOCK, 2, 2, 2, chunkBB);
		placeBlock(level, LAMP_BLOCK, 2, 2, 5, chunkBB);
		placeBlock(level, LAMP_BLOCK, 5, 2, 5, chunkBB);
		placeBlock(level, LAMP_BLOCK, 5, 2, 2, chunkBB);

		// corners
		generateBox(level, chunkBB, 0, 1, 0, 1, 3, 0, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 0, 1, 1, 0, 3, 1, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 0, 1, 7, 1, 3, 7, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 0, 1, 6, 0, 3, 6, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 6, 1, 7, 7, 3, 7, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 7, 1, 6, 7, 3, 6, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 6, 1, 0, 7, 3, 0, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 7, 1, 1, 7, 3, 1, BASE_LIGHT, BASE_LIGHT, false);
		placeBlock(level, BASE_GRAY, 1, 2, 0, chunkBB);
		placeBlock(level, BASE_GRAY, 0, 2, 1, chunkBB);
		placeBlock(level, BASE_GRAY, 1, 2, 7, chunkBB);
		placeBlock(level, BASE_GRAY, 0, 2, 6, chunkBB);
		placeBlock(level, BASE_GRAY, 6, 2, 7, chunkBB);
		placeBlock(level, BASE_GRAY, 7, 2, 6, chunkBB);
		placeBlock(level, BASE_GRAY, 6, 2, 0, chunkBB);
		placeBlock(level, BASE_GRAY, 7, 2, 1, chunkBB);
		if (!(*mRoomDefinition).mHasOpening[Facing::SOUTH]) {
			generateBox(level, chunkBB, 1, 3, 0, 6, 3, 0, BASE_LIGHT, BASE_LIGHT, false);
			generateBox(level, chunkBB, 1, 2, 0, 6, 2, 0, BASE_GRAY, BASE_GRAY, false);
			generateBox(level, chunkBB, 1, 1, 0, 6, 1, 0, BASE_LIGHT, BASE_LIGHT, false);
		}
		if (!(*mRoomDefinition).mHasOpening[Facing::NORTH]) {
			generateBox(level, chunkBB, 1, 3, 7, 6, 3, 7, BASE_LIGHT, BASE_LIGHT, false);
			generateBox(level, chunkBB, 1, 2, 7, 6, 2, 7, BASE_GRAY, BASE_GRAY, false);
			generateBox(level, chunkBB, 1, 1, 7, 6, 1, 7, BASE_LIGHT, BASE_LIGHT, false);
		}
		if (!(*mRoomDefinition).mHasOpening[Facing::WEST]) {
			generateBox(level, chunkBB, 0, 3, 1, 0, 3, 6, BASE_LIGHT, BASE_LIGHT, false);
			generateBox(level, chunkBB, 0, 2, 1, 0, 2, 6, BASE_GRAY, BASE_GRAY, false);
			generateBox(level, chunkBB, 0, 1, 1, 0, 1, 6, BASE_LIGHT, BASE_LIGHT, false);
		}
		if (!(*mRoomDefinition).mHasOpening[Facing::EAST]) {
			generateBox(level, chunkBB, 7, 3, 1, 7, 3, 6, BASE_LIGHT, BASE_LIGHT, false);
			generateBox(level, chunkBB, 7, 2, 1, 7, 2, 6, BASE_GRAY, BASE_GRAY, false);
			generateBox(level, chunkBB, 7, 1, 1, 7, 1, 6, BASE_LIGHT, BASE_LIGHT, false);
		}
	}
	else if (mMainDesign == 2) {
		generateBox(level, chunkBB, 0, 1, 0, 0, 1, 7, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 7, 1, 0, 7, 1, 7, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 1, 1, 0, 6, 1, 0, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 1, 1, 7, 6, 1, 7, BASE_LIGHT, BASE_LIGHT, false);

		generateBox(level, chunkBB, 0, 2, 0, 0, 2, 7, BASE_BLACK, BASE_BLACK, false);
		generateBox(level, chunkBB, 7, 2, 0, 7, 2, 7, BASE_BLACK, BASE_BLACK, false);
		generateBox(level, chunkBB, 1, 2, 0, 6, 2, 0, BASE_BLACK, BASE_BLACK, false);
		generateBox(level, chunkBB, 1, 2, 7, 6, 2, 7, BASE_BLACK, BASE_BLACK, false);

		generateBox(level, chunkBB, 0, 3, 0, 0, 3, 7, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 7, 3, 0, 7, 3, 7, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 1, 3, 0, 6, 3, 0, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 1, 3, 7, 6, 3, 7, BASE_LIGHT, BASE_LIGHT, false);

		generateBox(level, chunkBB, 0, 1, 3, 0, 2, 4, BASE_BLACK, BASE_BLACK, false);
		generateBox(level, chunkBB, 7, 1, 3, 7, 2, 4, BASE_BLACK, BASE_BLACK, false);
		generateBox(level, chunkBB, 3, 1, 0, 4, 2, 0, BASE_BLACK, BASE_BLACK, false);
		generateBox(level, chunkBB, 3, 1, 7, 4, 2, 7, BASE_BLACK, BASE_BLACK, false);

		if ((*mRoomDefinition).mHasOpening[Facing::SOUTH]) {
			generateWaterBox(level, chunkBB, 3, 1, 0, 4, 2, 0, false);
		}
		if ((*mRoomDefinition).mHasOpening[Facing::NORTH]) {
			generateWaterBox(level, chunkBB, 3, 1, 7, 4, 2, 7, false);
		}
		if ((*mRoomDefinition).mHasOpening[Facing::WEST]) {
			generateWaterBox(level, chunkBB, 0, 1, 3, 0, 2, 4, false);
		}
		if ((*mRoomDefinition).mHasOpening[Facing::EAST]) {
			generateWaterBox(level, chunkBB, 7, 1, 3, 7, 2, 4, false);
		}
	}
	if (centerPillar) {
		generateBox(level, chunkBB, 3, 1, 3, 4, 1, 4, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 3, 2, 3, 4, 2, 4, BASE_GRAY, BASE_GRAY, false);
		generateBox(level, chunkBB, 3, 3, 3, 4, 3, 4, BASE_LIGHT, BASE_LIGHT, false);
	}

	return true;
}
///////////////////////////////////////////////////////////////
//SIMPLE TOP ROOM
OceanMonumentSimpleTopRoom::OceanMonumentSimpleTopRoom() {
}

OceanMonumentSimpleTopRoom::OceanMonumentSimpleTopRoom(int& orientation, Shared<RoomDefinition>& definition, Random& random)
	: OceanMonumentPiece(1, orientation, definition, 1, 1, 1) {
}

bool OceanMonumentSimpleTopRoom::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	if (((*mRoomDefinition).mIndex / mGridFloorCount) > 0) {
		generateDefaultFloor(level, chunkBB, 0, 0, (*mRoomDefinition).mHasOpening[Facing::DOWN]);
	}
	if ((*mRoomDefinition).mConnections[Facing::UP] == nullptr) {
		generateBoxOnFillOnly(level, chunkBB, 1, 4, 1, 6, 4, 6, BASE_GRAY);
	}
	// place random sponge blocks
	for (int x = 1; x <= 6; x++) {
		for (int z = 1; z <= 6; z++) {
			if (random.nextInt(3) != 0) {
				 int y0 = 2 + ((random.nextInt(4) == 0) ? 0 : 1);
				 FullBlock wetSponge = FullBlock(Block::mSponge->mID, enum_cast(SpongeBlock::SpongeType::Wet));
				generateBox(level, chunkBB, x, y0, z, x, 3, z, wetSponge, wetSponge, false);
			}
		}
	}

	generateBox(level, chunkBB, 0, 1, 0, 0, 1, 7, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 7, 1, 0, 7, 1, 7, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 1, 1, 0, 6, 1, 0, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 1, 1, 7, 6, 1, 7, BASE_LIGHT, BASE_LIGHT, false);

	generateBox(level, chunkBB, 0, 2, 0, 0, 2, 7, BASE_BLACK, BASE_BLACK, false);
	generateBox(level, chunkBB, 7, 2, 0, 7, 2, 7, BASE_BLACK, BASE_BLACK, false);
	generateBox(level, chunkBB, 1, 2, 0, 6, 2, 0, BASE_BLACK, BASE_BLACK, false);
	generateBox(level, chunkBB, 1, 2, 7, 6, 2, 7, BASE_BLACK, BASE_BLACK, false);

	generateBox(level, chunkBB, 0, 3, 0, 0, 3, 7, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 7, 3, 0, 7, 3, 7, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 1, 3, 0, 6, 3, 0, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 1, 3, 7, 6, 3, 7, BASE_LIGHT, BASE_LIGHT, false);

	generateBox(level, chunkBB, 0, 1, 3, 0, 2, 4, BASE_BLACK, BASE_BLACK, false);
	generateBox(level, chunkBB, 7, 1, 3, 7, 2, 4, BASE_BLACK, BASE_BLACK, false);
	generateBox(level, chunkBB, 3, 1, 0, 4, 2, 0, BASE_BLACK, BASE_BLACK, false);
	generateBox(level, chunkBB, 3, 1, 7, 4, 2, 7, BASE_BLACK, BASE_BLACK, false);

	if ((*mRoomDefinition).mHasOpening[Facing::SOUTH]) {
		generateWaterBox(level, chunkBB, 3, 1, 0, 4, 2, 0, false);
	}

	return true;
}

///////////////////////////////////////////////////////////////
//DOUBLE Y ROOM
OceanMonumentDoubleYRoom::OceanMonumentDoubleYRoom() {
}

OceanMonumentDoubleYRoom::OceanMonumentDoubleYRoom(int& orientation, Shared<RoomDefinition>& definition, Random& random)
	: OceanMonumentPiece(1, orientation, definition, 1, 2, 1) {
}

bool OceanMonumentDoubleYRoom::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	if (((*mRoomDefinition).mIndex / mGridFloorCount) > 0) {
		generateDefaultFloor(level, chunkBB, 0, 0, (*mRoomDefinition).mHasOpening[Facing::SOUTH]);
	}
	Shared<RoomDefinition> above = (*mRoomDefinition).mConnections[Facing::UP];
	if (above != nullptr && above->mConnections[Facing::UP] == nullptr) {
		generateBoxOnFillOnly(level, chunkBB, 1, 8, 1, 6, 8, 6, BASE_GRAY);
	}

	{
		// middle
		generateBox(level, chunkBB, 0, 4, 0, 0, 4, 7, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 7, 4, 0, 7, 4, 7, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 1, 4, 0, 6, 4, 0, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 1, 4, 7, 6, 4, 7, BASE_LIGHT, BASE_LIGHT, false);

		generateBox(level, chunkBB, 2, 4, 1, 2, 4, 2, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 1, 4, 2, 1, 4, 2, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 5, 4, 1, 5, 4, 2, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 6, 4, 2, 6, 4, 2, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 2, 4, 5, 2, 4, 6, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 1, 4, 5, 1, 4, 5, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 5, 4, 5, 5, 4, 6, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 6, 4, 5, 6, 4, 5, BASE_LIGHT, BASE_LIGHT, false);

		Shared<RoomDefinition> definition = mRoomDefinition;
		for (int y = 1; y <= 5; y += 4) {
			int z = 0;
			if (definition != nullptr) {
				if (definition->mHasOpening[Facing::SOUTH]) {
					generateBox(level, chunkBB, 2, y, z, 2, y + 2, z, BASE_LIGHT, BASE_LIGHT, false);
					generateBox(level, chunkBB, 5, y, z, 5, y + 2, z, BASE_LIGHT, BASE_LIGHT, false);
					generateBox(level, chunkBB, 3, y + 2, z, 4, y + 2, z, BASE_LIGHT, BASE_LIGHT, false);
				}
				else {
					generateBox(level, chunkBB, 0, y, z, 7, y + 2, z, BASE_LIGHT, BASE_LIGHT, false);
					generateBox(level, chunkBB, 0, y + 1, z, 7, y + 1, z, BASE_GRAY, BASE_GRAY, false);
				}
				z = 7;
				if (definition->mHasOpening[Facing::NORTH]) {
					generateBox(level, chunkBB, 2, y, z, 2, y + 2, z, BASE_LIGHT, BASE_LIGHT, false);
					generateBox(level, chunkBB, 5, y, z, 5, y + 2, z, BASE_LIGHT, BASE_LIGHT, false);
					generateBox(level, chunkBB, 3, y + 2, z, 4, y + 2, z, BASE_LIGHT, BASE_LIGHT, false);
				}
				else {
					generateBox(level, chunkBB, 0, y, z, 7, y + 2, z, BASE_LIGHT, BASE_LIGHT, false);
					generateBox(level, chunkBB, 0, y + 1, z, 7, y + 1, z, BASE_GRAY, BASE_GRAY, false);
				}
				int x = 0;
				if (definition->mHasOpening[Facing::WEST]) {
					generateBox(level, chunkBB, x, y, 2, x, y + 2, 2, BASE_LIGHT, BASE_LIGHT, false);
					generateBox(level, chunkBB, x, y, 5, x, y + 2, 5, BASE_LIGHT, BASE_LIGHT, false);
					generateBox(level, chunkBB, x, y + 2, 3, x, y + 2, 4, BASE_LIGHT, BASE_LIGHT, false);
				}
				else {
					generateBox(level, chunkBB, x, y, 0, x, y + 2, 7, BASE_LIGHT, BASE_LIGHT, false);
					generateBox(level, chunkBB, x, y + 1, 0, x, y + 1, 7, BASE_GRAY, BASE_GRAY, false);
				}
				x = 7;
				if (definition->mHasOpening[Facing::EAST]) {
					generateBox(level, chunkBB, x, y, 2, x, y + 2, 2, BASE_LIGHT, BASE_LIGHT, false);
					generateBox(level, chunkBB, x, y, 5, x, y + 2, 5, BASE_LIGHT, BASE_LIGHT, false);
					generateBox(level, chunkBB, x, y + 2, 3, x, y + 2, 4, BASE_LIGHT, BASE_LIGHT, false);
				}
				else {
					generateBox(level, chunkBB, x, y, 0, x, y + 2, 7, BASE_LIGHT, BASE_LIGHT, false);
					generateBox(level, chunkBB, x, y + 1, 0, x, y + 1, 7, BASE_GRAY, BASE_GRAY, false);
				}
			}
			definition = above;
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////
//DOUBLE X ROOM
OceanMonumentDoubleXRoom::OceanMonumentDoubleXRoom() {
}

OceanMonumentDoubleXRoom::OceanMonumentDoubleXRoom(int& orientation, Shared<RoomDefinition>& definition, Random& random)
	: OceanMonumentPiece(1, orientation, definition, 2, 1, 1) {
}

bool OceanMonumentDoubleXRoom::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	RoomDefinition east = *(*mRoomDefinition).mConnections[Facing::EAST];
	RoomDefinition west = *mRoomDefinition;
	if (((*mRoomDefinition).mIndex / mGridFloorCount) > 0) {
		generateDefaultFloor(level, chunkBB, mGridroomWidth, 0, east.mHasOpening[Facing::DOWN]);
		generateDefaultFloor(level, chunkBB, 0, 0, west.mHasOpening[Facing::DOWN]);
	}
	if (west.mConnections[Facing::UP] == nullptr) {
		generateBoxOnFillOnly(level, chunkBB, 1, 4, 1, 7, 4, 6, BASE_GRAY);
	}
	if (east.mConnections[Facing::UP] == nullptr) {
		generateBoxOnFillOnly(level, chunkBB, 8, 4, 1, 14, 4, 6, BASE_GRAY);
	}

	// wall frame
	generateBox(level, chunkBB, 0, 3, 0, 0, 3, 7, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 15, 3, 0, 15, 3, 7, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 1, 3, 0, 15, 3, 0, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 1, 3, 7, 14, 3, 7, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 0, 2, 0, 0, 2, 7, BASE_GRAY, BASE_GRAY, false);
	generateBox(level, chunkBB, 15, 2, 0, 15, 2, 7, BASE_GRAY, BASE_GRAY, false);
	generateBox(level, chunkBB, 1, 2, 0, 15, 2, 0, BASE_GRAY, BASE_GRAY, false);
	generateBox(level, chunkBB, 1, 2, 7, 14, 2, 7, BASE_GRAY, BASE_GRAY, false);
	generateBox(level, chunkBB, 0, 1, 0, 0, 1, 7, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 15, 1, 0, 15, 1, 7, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 1, 1, 0, 15, 1, 0, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 1, 1, 7, 14, 1, 7, BASE_LIGHT, BASE_LIGHT, false);

	// center deco
	generateBox(level, chunkBB, 5, 1, 0, 10, 1, 4, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 6, 2, 0, 9, 2, 3, BASE_GRAY, BASE_GRAY, false);
	generateBox(level, chunkBB, 5, 3, 0, 10, 3, 4, BASE_LIGHT, BASE_LIGHT, false);

	placeBlock(level, LAMP_BLOCK, 6, 2, 3, chunkBB);
	placeBlock(level, LAMP_BLOCK, 9, 2, 3, chunkBB);

	// openings
	if (west.mHasOpening[Facing::SOUTH]) {
		generateWaterBox(level, chunkBB, 3, 1, 0, 4, 2, 0, false);
	}
	if (west.mHasOpening[Facing::NORTH]) {
		generateWaterBox(level, chunkBB, 3, 1, 7, 4, 2, 7, false);
	}
	if (west.mHasOpening[Facing::WEST]) {
		generateWaterBox(level, chunkBB, 0, 1, 3, 0, 2, 4, false);
	}
	if (east.mHasOpening[Facing::SOUTH]) {
		generateWaterBox(level, chunkBB, 11, 1, 0, 12, 2, 0, false);
	}
	if (east.mHasOpening[Facing::NORTH]) {
		generateWaterBox(level, chunkBB, 11, 1, 7, 12, 2, 7, false);
	}
	if (east.mHasOpening[Facing::EAST]) {
		generateWaterBox(level, chunkBB, 15, 1, 3, 15, 2, 4, false);
	}

	return true;
}

///////////////////////////////////////////////////////////////
//DOUBLE Z ROOM
OceanMonumentDoubleZRoom::OceanMonumentDoubleZRoom() {
}

OceanMonumentDoubleZRoom::OceanMonumentDoubleZRoom(int& orientation, Shared<RoomDefinition>& definition, Random& random)
	: OceanMonumentPiece(1, orientation, definition, 1, 1, 2) {
}

bool OceanMonumentDoubleZRoom::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	RoomDefinition north = *(*mRoomDefinition).mConnections[Facing::NORTH];
	RoomDefinition south = *mRoomDefinition;
	if (((*mRoomDefinition).mIndex / mGridFloorCount) > 0) {
		generateDefaultFloor(level, chunkBB, 0, mGridroomDepth, north.mHasOpening[Facing::DOWN]);
		generateDefaultFloor(level, chunkBB, 0, 0, south.mHasOpening[Facing::DOWN]);
	}
	if (south.mConnections[Facing::UP] == nullptr) {
		generateBoxOnFillOnly(level, chunkBB, 1, 4, 1, 6, 4, 7, BASE_GRAY);
	}
	if (north.mConnections[Facing::UP] == nullptr) {
		generateBoxOnFillOnly(level, chunkBB, 1, 4, 8, 6, 4, 14, BASE_GRAY);
	}

	// wall frame
	generateBox(level, chunkBB, 0, 3, 0, 0, 3, 15, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 7, 3, 0, 7, 3, 15, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 1, 3, 0, 7, 3, 0, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 1, 3, 15, 6, 3, 15, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 0, 2, 0, 0, 2, 15, BASE_GRAY, BASE_GRAY, false);
	generateBox(level, chunkBB, 7, 2, 0, 7, 2, 15, BASE_GRAY, BASE_GRAY, false);
	generateBox(level, chunkBB, 1, 2, 0, 7, 2, 0, BASE_GRAY, BASE_GRAY, false);
	generateBox(level, chunkBB, 1, 2, 15, 6, 2, 15, BASE_GRAY, BASE_GRAY, false);
	generateBox(level, chunkBB, 0, 1, 0, 0, 1, 15, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 7, 1, 0, 7, 1, 15, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 1, 1, 0, 7, 1, 0, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 1, 1, 15, 6, 1, 15, BASE_LIGHT, BASE_LIGHT, false);

	// corners
	generateBox(level, chunkBB, 1, 1, 1, 1, 1, 2, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 6, 1, 1, 6, 1, 2, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 1, 3, 1, 1, 3, 2, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 6, 3, 1, 6, 3, 2, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 1, 1, 13, 1, 1, 14, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 6, 1, 13, 6, 1, 14, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 1, 3, 13, 1, 3, 14, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 6, 3, 13, 6, 3, 14, BASE_LIGHT, BASE_LIGHT, false);

	// center deco
	generateBox(level, chunkBB, 2, 1, 6, 2, 3, 6, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 5, 1, 6, 5, 3, 6, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 2, 1, 9, 2, 3, 9, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 5, 1, 9, 5, 3, 9, BASE_LIGHT, BASE_LIGHT, false);

	generateBox(level, chunkBB, 3, 2, 6, 4, 2, 6, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 3, 2, 9, 4, 2, 9, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 2, 2, 7, 2, 2, 8, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 5, 2, 7, 5, 2, 8, BASE_LIGHT, BASE_LIGHT, false);

	placeBlock(level, LAMP_BLOCK, 2, 2, 5, chunkBB);
	placeBlock(level, LAMP_BLOCK, 5, 2, 5, chunkBB);
	placeBlock(level, LAMP_BLOCK, 2, 2, 10, chunkBB);
	placeBlock(level, LAMP_BLOCK, 5, 2, 10, chunkBB);
	placeBlock(level, BASE_LIGHT, 2, 3, 5, chunkBB);
	placeBlock(level, BASE_LIGHT, 5, 3, 5, chunkBB);
	placeBlock(level, BASE_LIGHT, 2, 3, 10, chunkBB);
	placeBlock(level, BASE_LIGHT, 5, 3, 10, chunkBB);

	// openings
	if (south.mHasOpening[Facing::SOUTH]) {
		generateWaterBox(level, chunkBB, 3, 1, 0, 4, 2, 0, false);
	}
	if (south.mHasOpening[Facing::EAST]) {
		generateWaterBox(level, chunkBB, 7, 1, 3, 7, 2, 4, false);
	}
	if (south.mHasOpening[Facing::WEST]) {
		generateWaterBox(level, chunkBB, 0, 1, 3, 0, 2, 4, false);
	}
	if (north.mHasOpening[Facing::NORTH]) {
		generateWaterBox(level, chunkBB, 3, 1, 15, 4, 2, 15, false);
	}
	if (north.mHasOpening[Facing::WEST]) {
		generateWaterBox(level, chunkBB, 0, 1, 11, 0, 2, 12, false);
	}
	if (north.mHasOpening[Facing::EAST]) {
		generateWaterBox(level, chunkBB, 7, 1, 11, 7, 2, 12, false);
	}

	return true;
}

///////////////////////////////////////////////////////////////
//DOUBLE XY ROOM
OceanMonumentDoubleXYRoom::OceanMonumentDoubleXYRoom() {
}

OceanMonumentDoubleXYRoom::OceanMonumentDoubleXYRoom(int& orientation, Shared<RoomDefinition>& definition, Random& random)
	: OceanMonumentPiece(1, orientation, definition, 2, 2, 1) {
}

bool OceanMonumentDoubleXYRoom::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	RoomDefinition east = *(*mRoomDefinition).mConnections[Facing::EAST];
	RoomDefinition west = *mRoomDefinition;
	RoomDefinition westUp = *west.mConnections[Facing::UP];
	RoomDefinition eastUp = *east.mConnections[Facing::UP];

	if (((*mRoomDefinition).mIndex / mGridFloorCount) > 0) {
		generateDefaultFloor(level, chunkBB, mGridroomWidth, 0, east.mHasOpening[Facing::DOWN]);
		generateDefaultFloor(level, chunkBB, 0, 0, west.mHasOpening[Facing::DOWN]);
	}
	if (westUp.mConnections[Facing::UP] == nullptr) {
		generateBoxOnFillOnly(level, chunkBB, 1, 8, 1, 7, 8, 6, BASE_GRAY);
	}
	if (eastUp.mConnections[Facing::UP] == nullptr) {
		generateBoxOnFillOnly(level, chunkBB, 8, 8, 1, 14, 8, 6, BASE_GRAY);
	}

	// wall frame
	FullBlock block;
	for (int y = 1; y <= 7; y++) {
		block = BASE_LIGHT;
		if (y == 2 || y == 6) {
			block = BASE_GRAY;
		}
		generateBox(level, chunkBB, 0, y, 0, 0, y, 7, block, block, false);
		generateBox(level, chunkBB, 15, y, 0, 15, y, 7, block, block, false);
		generateBox(level, chunkBB, 1, y, 0, 15, y, 0, block, block, false);
		generateBox(level, chunkBB, 1, y, 7, 14, y, 7, block, block, false);
	}

	// pipes
	generateBox(level, chunkBB, 2, 1, 3, 2, 7, 4, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 3, 1, 2, 4, 7, 2, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 3, 1, 5, 4, 7, 5, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 13, 1, 3, 13, 7, 4, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 11, 1, 2, 12, 7, 2, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 11, 1, 5, 12, 7, 5, BASE_LIGHT, BASE_LIGHT, false);

	generateBox(level, chunkBB, 5, 1, 3, 5, 3, 4, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 10, 1, 3, 10, 3, 4, BASE_LIGHT, BASE_LIGHT, false);

	generateBox(level, chunkBB, 5, 7, 2, 10, 7, 5, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 5, 5, 2, 5, 7, 2, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 10, 5, 2, 10, 7, 2, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 5, 5, 5, 5, 7, 5, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 10, 5, 5, 10, 7, 5, BASE_LIGHT, BASE_LIGHT, false);
	placeBlock(level, BASE_LIGHT, 6, 6, 2, chunkBB);
	placeBlock(level, BASE_LIGHT, 9, 6, 2, chunkBB);
	placeBlock(level, BASE_LIGHT, 6, 6, 5, chunkBB);
	placeBlock(level, BASE_LIGHT, 9, 6, 5, chunkBB);

	generateBox(level, chunkBB, 5, 4, 3, 6, 4, 4, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 9, 4, 3, 10, 4, 4, BASE_LIGHT, BASE_LIGHT, false);
	placeBlock(level, LAMP_BLOCK, 5, 4, 2, chunkBB);
	placeBlock(level, LAMP_BLOCK, 5, 4, 5, chunkBB);
	placeBlock(level, LAMP_BLOCK, 10, 4, 2, chunkBB);
	placeBlock(level, LAMP_BLOCK, 10, 4, 5, chunkBB);

	// openings
	if (west.mHasOpening[Facing::SOUTH]) {
		generateWaterBox(level, chunkBB, 3, 1, 0, 4, 2, 0, false);
	}
	if (west.mHasOpening[Facing::NORTH]) {
		generateWaterBox(level, chunkBB, 3, 1, 7, 4, 2, 7, false);
	}
	if (west.mHasOpening[Facing::WEST]) {
		generateWaterBox(level, chunkBB, 0, 1, 3, 0, 2, 4, false);
	}
	if (east.mHasOpening[Facing::SOUTH]) {
		generateWaterBox(level, chunkBB, 11, 1, 0, 12, 2, 0, false);
	}
	if (east.mHasOpening[Facing::NORTH]) {
		generateWaterBox(level, chunkBB, 11, 1, 7, 12, 2, 7, false);
	}
	if (east.mHasOpening[Facing::EAST]) {
		generateWaterBox(level, chunkBB, 15, 1, 3, 15, 2, 4, false);
	}
	if (westUp.mHasOpening[Facing::SOUTH]) {
		generateWaterBox(level, chunkBB, 3, 5, 0, 4, 6, 0, false);
	}
	if (westUp.mHasOpening[Facing::NORTH]) {
		generateWaterBox(level, chunkBB, 3, 5, 7, 4, 6, 7, false);
	}
	if (westUp.mHasOpening[Facing::WEST]) {
		generateWaterBox(level, chunkBB, 0, 5, 3, 0, 6, 4, false);
	}
	if (eastUp.mHasOpening[Facing::SOUTH]) {
		generateWaterBox(level, chunkBB, 11, 5, 0, 12, 6, 0, false);
	}
	if (eastUp.mHasOpening[Facing::NORTH]) {
		generateWaterBox(level, chunkBB, 11, 5, 7, 12, 6, 7, false);
	}
	if (eastUp.mHasOpening[Facing::EAST]) {
		generateWaterBox(level, chunkBB, 15, 5, 3, 15, 6, 4, false);
	}

	return true;
}

///////////////////////////////////////////////////////////////
//DOUBLE YZ ROOM
OceanMonumentDoubleYZRoom::OceanMonumentDoubleYZRoom() {
}

OceanMonumentDoubleYZRoom::OceanMonumentDoubleYZRoom(int& orientation, Shared<RoomDefinition>& definition, Random& random)
	: OceanMonumentPiece(1, orientation, definition, 1, 2, 2) {
}

bool OceanMonumentDoubleYZRoom::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	RoomDefinition north = *(*mRoomDefinition).mConnections[Facing::NORTH];
	RoomDefinition south = *mRoomDefinition;
	RoomDefinition northUp = *north.mConnections[Facing::UP];
	RoomDefinition southUp = *south.mConnections[Facing::UP];
	if (((*mRoomDefinition).mIndex / mGridFloorCount) > 0) {
		generateDefaultFloor(level, chunkBB, 0, mGridroomDepth, north.mHasOpening[Facing::DOWN]);
		generateDefaultFloor(level, chunkBB, 0, 0, south.mHasOpening[Facing::DOWN]);
	}
	if (southUp.mConnections[Facing::UP] == nullptr) {
		generateBoxOnFillOnly(level, chunkBB, 1, 8, 1, 6, 8, 7, BASE_GRAY);
	}
	if (northUp.mConnections[Facing::UP] == nullptr) {
		generateBoxOnFillOnly(level, chunkBB, 1, 8, 8, 6, 8, 14, BASE_GRAY);
	}

	// wall frame
	FullBlock block;
	for (int y = 1; y <= 7; y++) {
		block = BASE_LIGHT;
		if (y == 2 || y == 6) {
			block = BASE_GRAY;
		}
		generateBox(level, chunkBB, 0, y, 0, 0, y, 15, block, block, false);
		generateBox(level, chunkBB, 7, y, 0, 7, y, 15, block, block, false);
		generateBox(level, chunkBB, 1, y, 0, 6, y, 0, block, block, false);
		generateBox(level, chunkBB, 1, y, 15, 6, y, 15, block, block, false);
	}

	// center pillar
	//int block;
	for (int y = 1; y <= 7; y++) {
		block = BASE_BLACK;
		if (y == 2 || y == 6) {
			block = LAMP_BLOCK;
		}
		generateBox(level, chunkBB, 3, y, 7, 4, y, 8, block, block, false);
	}

	// openings
	if (south.mHasOpening[Facing::SOUTH]) {
		generateWaterBox(level, chunkBB, 3, 1, 0, 4, 2, 0, false);
	}
	if (south.mHasOpening[Facing::EAST]) {
		generateWaterBox(level, chunkBB, 7, 1, 3, 7, 2, 4, false);
	}
	if (south.mHasOpening[Facing::WEST]) {
		generateWaterBox(level, chunkBB, 0, 1, 3, 0, 2, 4, false);
	}
	if (north.mHasOpening[Facing::NORTH]) {
		generateWaterBox(level, chunkBB, 3, 1, 15, 4, 2, 15, false);
	}
	if (north.mHasOpening[Facing::WEST]) {
		generateWaterBox(level, chunkBB, 0, 1, 11, 0, 2, 12, false);
	}
	if (north.mHasOpening[Facing::EAST]) {
		generateWaterBox(level, chunkBB, 7, 1, 11, 7, 2, 12, false);
	}

	if (southUp.mHasOpening[Facing::SOUTH]) {
		generateWaterBox(level, chunkBB, 3, 5, 0, 4, 6, 0, false);
	}
	if (southUp.mHasOpening[Facing::EAST]) {
		generateWaterBox(level, chunkBB, 7, 5, 3, 7, 6, 4, false);
		generateBox(level, chunkBB, 5, 4, 2, 6, 4, 5, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 6, 1, 2, 6, 3, 2, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 6, 1, 5, 6, 3, 5, BASE_LIGHT, BASE_LIGHT, false);
	}
	if (southUp.mHasOpening[Facing::WEST]) {
		generateWaterBox(level, chunkBB, 0, 5, 3, 0, 6, 4, false);
		generateBox(level, chunkBB, 1, 4, 2, 2, 4, 5, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 1, 1, 2, 1, 3, 2, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 1, 1, 5, 1, 3, 5, BASE_LIGHT, BASE_LIGHT, false);
	}
	if (northUp.mHasOpening[Facing::NORTH]) {
		generateWaterBox(level, chunkBB, 3, 5, 15, 4, 6, 15, false);
	}
	if (northUp.mHasOpening[Facing::WEST]) {
		generateWaterBox(level, chunkBB, 0, 5, 11, 0, 6, 12, false);
		generateBox(level, chunkBB, 1, 4, 10, 2, 4, 13, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 1, 1, 10, 1, 3, 10, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 1, 1, 13, 1, 3, 13, BASE_LIGHT, BASE_LIGHT, false);
	}
	if (northUp.mHasOpening[Facing::EAST]) {
		generateWaterBox(level, chunkBB, 7, 5, 11, 7, 6, 12, false);
		generateBox(level, chunkBB, 5, 4, 10, 6, 4, 13, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 6, 1, 10, 6, 3, 10, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 6, 1, 13, 6, 3, 13, BASE_LIGHT, BASE_LIGHT, false);
	}

	return true;
}

///////////////////////////////////////////////////////////////
//CORE ROOM
OceanMonumentCoreRoom::OceanMonumentCoreRoom() {
}

OceanMonumentCoreRoom::OceanMonumentCoreRoom(int& orientation, Shared<RoomDefinition>& definition, Random& random)
	: OceanMonumentPiece(1, orientation, definition, 2, 2, 2) {
}

bool OceanMonumentCoreRoom::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	generateBoxOnFillOnly(level, chunkBB, 1, 8, 0, 14, 8, 14, BASE_GRAY);

	// walls
	{
		int y = 7;
		generateBox(level, chunkBB, 0, y, 0, 0, y, 15, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 15, y, 0, 15, y, 15, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 1, y, 0, 15, y, 0, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 1, y, 15, 14, y, 15, BASE_LIGHT, BASE_LIGHT, false);
	}
	FullBlock block;
	for (int y = 1; y <= 6; y++) {
		block = BASE_LIGHT;
		if (y == 2 || y == 6) {
			block = BASE_GRAY;
		}

		for (int x = 0; x <= 15; x += 15) {
			generateBox(level, chunkBB, x, y, 0, x, y, 1, block, block, false);
			generateBox(level, chunkBB, x, y, 6, x, y, 9, block, block, false);
			generateBox(level, chunkBB, x, y, 14, x, y, 15, block, block, false);
		}
		generateBox(level, chunkBB, 1, y, 0, 1, y, 0, block, block, false);
		generateBox(level, chunkBB, 6, y, 0, 9, y, 0, block, block, false);
		generateBox(level, chunkBB, 14, y, 0, 14, y, 0, block, block, false);

		generateBox(level, chunkBB, 1, y, 15, 14, y, 15, block, block, false);
	}

	// core
	generateBox(level, chunkBB, 6, 3, 6, 9, 6, 9, BASE_BLACK, BASE_BLACK, false);
	generateBox(level, chunkBB, 7, 4, 7, 8, 5, 8, Block::mGoldBlock->mID, Block::mGoldBlock->mID, false);
	for (int y = 3; y <= 6; y += 3) {
		for (int x = 6; x <= 9; x += 3) {
			placeBlock(level, LAMP_BLOCK, x, y, 6, chunkBB);
			placeBlock(level, LAMP_BLOCK, x, y, 9, chunkBB);
		}
	}

	generateBox(level, chunkBB, 5, 1, 6, 5, 2, 6, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 5, 1, 9, 5, 2, 9, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 10, 1, 6, 10, 2, 6, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 10, 1, 9, 10, 2, 9, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 6, 1, 5, 6, 2, 5, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 9, 1, 5, 9, 2, 5, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 6, 1, 10, 6, 2, 10, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 9, 1, 10, 9, 2, 10, BASE_LIGHT, BASE_LIGHT, false);

	generateBox(level, chunkBB, 5, 2, 5, 5, 6, 5, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 5, 2, 10, 5, 6, 10, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 10, 2, 5, 10, 6, 5, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 10, 2, 10, 10, 6, 10, BASE_LIGHT, BASE_LIGHT, false);

	generateBox(level, chunkBB, 5, 7, 1, 5, 7, 6, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 10, 7, 1, 10, 7, 6, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 5, 7, 9, 5, 7, 14, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 10, 7, 9, 10, 7, 14, BASE_LIGHT, BASE_LIGHT, false);

	generateBox(level, chunkBB, 1, 7, 5, 6, 7, 5, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 1, 7, 10, 6, 7, 10, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 9, 7, 5, 14, 7, 5, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 9, 7, 10, 14, 7, 10, BASE_LIGHT, BASE_LIGHT, false);

	// floor decorations
	generateBox(level, chunkBB, 2, 1, 2, 2, 1, 3, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 3, 1, 2, 3, 1, 2, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 13, 1, 2, 13, 1, 3, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 12, 1, 2, 12, 1, 2, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 2, 1, 12, 2, 1, 13, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 3, 1, 13, 3, 1, 13, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 13, 1, 12, 13, 1, 13, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 12, 1, 13, 12, 1, 13, BASE_LIGHT, BASE_LIGHT, false);

	return true;
}

///////////////////////////////////////////////////////////////
//WING ROOM
OceanMonumentWingRoom::OceanMonumentWingRoom() {
}

OceanMonumentWingRoom::OceanMonumentWingRoom(int& orientation, BoundingBox& boundingBox, int random, bool isRightWing)
	: OceanMonumentPiece(orientation, boundingBox) {
	mMainDesign = random & 1;
	mIsRightWing = isRightWing;
}

bool OceanMonumentWingRoom::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	if (mMainDesign == 0) {
		for (int i = 0; i < 4; i++) {
			generateBox(level, chunkBB, 10 - i, 3 - i, 20 - i, 12 + i, 3 - i, 20, BASE_LIGHT, BASE_LIGHT, false);
		}
		generateBox(level, chunkBB, 7, 0, 6, 15, 0, 16, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 6, 0, 6, 6, 3, 20, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 16, 0, 6, 16, 3, 20, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 7, 1, 7, 7, 1, 20, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 15, 1, 7, 15, 1, 20, BASE_LIGHT, BASE_LIGHT, false);

		generateBox(level, chunkBB, 7, 1, 6, 9, 3, 6, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 13, 1, 6, 15, 3, 6, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 8, 1, 7, 9, 1, 7, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 13, 1, 7, 14, 1, 7, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 9, 0, 5, 13, 0, 5, BASE_LIGHT, BASE_LIGHT, false);

		generateBox(level, chunkBB, 10, 0, 7, 12, 0, 7, BASE_BLACK, BASE_BLACK, false);
		generateBox(level, chunkBB, 8, 0, 10, 8, 0, 12, BASE_BLACK, BASE_BLACK, false);
		generateBox(level, chunkBB, 14, 0, 10, 14, 0, 12, BASE_BLACK, BASE_BLACK, false);

		for (int z = 18; z >= 7; z -= 3) {
			placeBlock(level, LAMP_BLOCK, 6, 3, z, chunkBB);
			placeBlock(level, LAMP_BLOCK, 16, 3, z, chunkBB);
		}
		placeBlock(level, LAMP_BLOCK, 10, 0, 10, chunkBB);
		placeBlock(level, LAMP_BLOCK, 12, 0, 10, chunkBB);
		placeBlock(level, LAMP_BLOCK, 10, 0, 12, chunkBB);
		placeBlock(level, LAMP_BLOCK, 12, 0, 12, chunkBB);

		placeBlock(level, LAMP_BLOCK, 8, 3, 6, chunkBB);
		placeBlock(level, LAMP_BLOCK, 14, 3, 6, chunkBB);

		// corridor light poles
		placeBlock(level, BASE_LIGHT, 4, 2, 4, chunkBB);
		placeBlock(level, LAMP_BLOCK, 4, 1, 4, chunkBB);
		placeBlock(level, BASE_LIGHT, 4, 0, 4, chunkBB);

		placeBlock(level, BASE_LIGHT, 18, 2, 4, chunkBB);
		placeBlock(level, LAMP_BLOCK, 18, 1, 4, chunkBB);
		placeBlock(level, BASE_LIGHT, 18, 0, 4, chunkBB);

		placeBlock(level, BASE_LIGHT, 4, 2, 18, chunkBB);
		placeBlock(level, LAMP_BLOCK, 4, 1, 18, chunkBB);
		placeBlock(level, BASE_LIGHT, 4, 0, 18, chunkBB);

		placeBlock(level, BASE_LIGHT, 18, 2, 18, chunkBB);
		placeBlock(level, LAMP_BLOCK, 18, 1, 18, chunkBB);
		placeBlock(level, BASE_LIGHT, 18, 0, 18, chunkBB);

		// fill holes, technically outside the bb
		placeBlock(level, BASE_LIGHT, 9, 7, 20, chunkBB);
		placeBlock(level, BASE_LIGHT, 13, 7, 20, chunkBB);
		generateBox(level, chunkBB, 6, 0, 21, 7, 4, 21, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 15, 0, 21, 16, 4, 21, BASE_LIGHT, BASE_LIGHT, false);

	}
	else if (mMainDesign == 1) {
		generateBox(level, chunkBB, 9, 3, 18, 13, 3, 20, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 9, 0, 18, 9, 2, 18, BASE_LIGHT, BASE_LIGHT, false);
		generateBox(level, chunkBB, 13, 0, 18, 13, 2, 18, BASE_LIGHT, BASE_LIGHT, false);
		int x = 9;
		int z = 20;
		int y = 5;
		for (int i = 0; i < 2; i++) {
			placeBlock(level, BASE_LIGHT, x, y + 1, z, chunkBB);
			placeBlock(level, LAMP_BLOCK, x, y, z, chunkBB);
			placeBlock(level, BASE_LIGHT, x, y - 1, z, chunkBB);
			x = 13;
		}

		generateBox(level, chunkBB, 7, 3, 7, 15, 3, 14, BASE_LIGHT, BASE_LIGHT, false);
		x = 10;
		for (int i = 0; i < 2; i++) {
			generateBox(level, chunkBB, x, 0, 10, x, 6, 10, BASE_LIGHT, BASE_LIGHT, false);
			generateBox(level, chunkBB, x, 0, 12, x, 6, 12, BASE_LIGHT, BASE_LIGHT, false);
			placeBlock(level, LAMP_BLOCK, x, 0, 10, chunkBB);
			placeBlock(level, LAMP_BLOCK, x, 0, 12, chunkBB);
			placeBlock(level, LAMP_BLOCK, x, 4, 10, chunkBB);
			placeBlock(level, LAMP_BLOCK, x, 4, 12, chunkBB);
			x = 12;
		}
		x = 8;
		for (int i = 0; i < 2; i++) {
			generateBox(level, chunkBB, x, 0, 7, x, 2, 7, BASE_LIGHT, BASE_LIGHT, false);
			generateBox(level, chunkBB, x, 0, 14, x, 2, 14, BASE_LIGHT, BASE_LIGHT, false);
			x = 14;
		}
		generateBox(level, chunkBB, 8, 3, 8, 8, 3, 13, BASE_BLACK, BASE_BLACK, false);
		generateBox(level, chunkBB, 14, 3, 8, 14, 3, 13, BASE_BLACK, BASE_BLACK, false);
	}

	return true;
}
void OceanMonumentWingRoom::postProcessMobsAt(BlockSource* region, Random& random, const BoundingBox& chunkBB) {
	
	if (!mSpawnedElders && mIsRightWing) {
		float worldX = (float)getWorldX(16, 6);
		float worldY = (float)getWorldY(1);
		float worldZ = (float)getWorldZ(16, 6);
		if (region->getChunkAt(Vec3(worldX, worldY, worldZ))) {
			spawnElder(region, chunkBB, 16, 1, 6); //rightWing
			mSpawnedElders = true;
			return;
		}
	}
	else if (!mSpawnedElders) {
		float worldX = (float)getWorldX(6, 6);
		float worldY = (float)getWorldY(1);
		float worldZ = (float)getWorldZ(6, 6);
		if (region->getChunkAt(Vec3(worldX, worldY, worldZ))) {
			spawnElder(region, chunkBB, 6, 1, 6); //leftWing
			mSpawnedElders = true;
			return;
		}
	}
}

void OceanMonumentWingRoom::addAdditionalSaveData(CompoundTag& tag) const {
	OceanMonumentPiece::addAdditionalSaveData(tag);
	tag.putBoolean("Elders", mSpawnedElders);
}

void OceanMonumentWingRoom::readAdditionalSaveData(const CompoundTag& tag) {
	OceanMonumentPiece::readAdditionalSaveData(tag);
	mSpawnedElders = tag.getBoolean("Elders");
}

///////////////////////////////////////////////////////////////
//PENTHOUSE
OceanMonumentPenthouse::OceanMonumentPenthouse() {
}

OceanMonumentPenthouse::OceanMonumentPenthouse(int& orientation, BoundingBox& boundingBox)
	: OceanMonumentPiece(orientation, boundingBox) {
}

bool OceanMonumentPenthouse::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	generateBox(level, chunkBB, 2, -1, 2, 11, -1, 11, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 0, -1, 0, 1, -1, 11, BASE_GRAY, BASE_GRAY, false);
	generateBox(level, chunkBB, 12, -1, 0, 13, -1, 11, BASE_GRAY, BASE_GRAY, false);
	generateBox(level, chunkBB, 2, -1, 0, 11, -1, 1, BASE_GRAY, BASE_GRAY, false);
	generateBox(level, chunkBB, 2, -1, 12, 11, -1, 13, BASE_GRAY, BASE_GRAY, false);

	generateBox(level, chunkBB, 0, 0, 0, 0, 0, 13, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 13, 0, 0, 13, 0, 13, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 1, 0, 0, 12, 0, 0, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 1, 0, 13, 12, 0, 13, BASE_LIGHT, BASE_LIGHT, false);

	for (int i = 2; i <= 11; i += 3) {
		placeBlock(level, LAMP_BLOCK, 0, 0, i, chunkBB);
		placeBlock(level, LAMP_BLOCK, 13, 0, i, chunkBB);
		placeBlock(level, LAMP_BLOCK, i, 0, 0, chunkBB);
	}

	generateBox(level, chunkBB, 2, 0, 3, 4, 0, 9, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 9, 0, 3, 11, 0, 9, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 4, 0, 9, 9, 0, 11, BASE_LIGHT, BASE_LIGHT, false);
	placeBlock(level, BASE_LIGHT, 5, 0, 8, chunkBB);
	placeBlock(level, BASE_LIGHT, 8, 0, 8, chunkBB);
	placeBlock(level, BASE_LIGHT, 10, 0, 10, chunkBB);
	placeBlock(level, BASE_LIGHT, 3, 0, 10, chunkBB);
	generateBox(level, chunkBB, 3, 0, 3, 3, 0, 7, BASE_BLACK, BASE_BLACK, false);
	generateBox(level, chunkBB, 10, 0, 3, 10, 0, 7, BASE_BLACK, BASE_BLACK, false);
	generateBox(level, chunkBB, 6, 0, 10, 7, 0, 10, BASE_BLACK, BASE_BLACK, false);

	int x = 3;
	for (int i = 0; i < 2; i++) {
		for (int z = 2; z <= 8; z += 3) {
			generateBox(level, chunkBB, x, 0, z, x, 2, z, BASE_LIGHT, BASE_LIGHT, false);
		}
		x = 10;
	}
	generateBox(level, chunkBB, 5, 0, 10, 5, 2, 10, BASE_LIGHT, BASE_LIGHT, false);
	generateBox(level, chunkBB, 8, 0, 10, 8, 2, 10, BASE_LIGHT, BASE_LIGHT, false);

	generateBox(level, chunkBB, 6, -1, 7, 7, -1, 8, BASE_BLACK, BASE_BLACK, false);

	// open to downwards room
	generateWaterBox(level, chunkBB, 6, -1, 3, 7, -1, 4, false);

	return true;
}

void OceanMonumentPenthouse::postProcessMobsAt(BlockSource* region, Random& random, const BoundingBox& chunkBB) {
	if (!mSpawnedElders) {
		float worldX = (float)getWorldX(6, 3);
		float worldY = (float)getWorldY(1);
		float worldZ = (float)getWorldZ(6, 3);
		if (region->getChunkAt(Vec3(worldX, worldY, worldZ))) {
			spawnElder(region, chunkBB, 6, 1, 3); //penthouse
			mSpawnedElders = true;
		}
	}
}

void OceanMonumentPenthouse::addAdditionalSaveData(CompoundTag& tag) const {
	OceanMonumentPiece::addAdditionalSaveData(tag);
	tag.putBoolean("Elders", mSpawnedElders);
}

void OceanMonumentPenthouse::readAdditionalSaveData(const CompoundTag& tag) {
	OceanMonumentPiece::readAdditionalSaveData(tag);
	mSpawnedElders = tag.getBoolean("Elders");
}


///////////////////////////////////////////////////////////////
//FIT SIMPLE ROOM
bool FitSimpleRoom::fits(Shared<RoomDefinition>& definition) {
	return true;
}
Unique<OceanMonumentPiece> FitSimpleRoom::create(int& orientation, Shared<RoomDefinition>& definition, Random& random) {
	(*definition).mClaimed = true;
	return make_unique<OceanMonumentSimpleRoom>(orientation, definition, random);
}
///////////////////////////////////////////////////////////////
//FIT SIMPLE TOP ROOM
bool FitSimpleTopRoom::fits(Shared<RoomDefinition>& definition) {
	return !(*definition).mHasOpening[Facing::WEST] && !(*definition).mHasOpening[Facing::EAST] && !(*definition).mHasOpening[Facing::NORTH] && !(*definition).mHasOpening[Facing::SOUTH] && !(*definition).mHasOpening[Facing::UP];
}
Unique<OceanMonumentPiece> FitSimpleTopRoom::create(int& orientation, Shared<RoomDefinition>& definition, Random& random) {
	(*definition).mClaimed = true;
	return make_unique<OceanMonumentSimpleTopRoom>(orientation, definition, random);
}
///////////////////////////////////////////////////////////////
//FIT DOUBLE Y ROOM
bool FitDoubleYRoom::fits(Shared<RoomDefinition>& definition) {
	if ((*definition).mHasOpening[Facing::UP] && !(*definition).mConnections[Facing::UP]->mClaimed) {
		return true;
	}
	return false;
}
Unique<OceanMonumentPiece> FitDoubleYRoom::create(int& orientation, Shared<RoomDefinition>& definition, Random& random) {
	(*definition).mClaimed = true;
	(*definition).mConnections[Facing::UP]->mClaimed = true;
	return make_unique<OceanMonumentDoubleYRoom>(orientation, definition, random);
}
///////////////////////////////////////////////////////////////
//FIT DOUBLE X ROOM
bool FitDoubleXRoom::fits(Shared<RoomDefinition>& definition) {
	if ((*definition).mHasOpening[Facing::EAST] && !(*definition).mConnections[Facing::EAST]->mClaimed) {
		return true;
	}
	return false;
}
Unique<OceanMonumentPiece> FitDoubleXRoom::create(int& orientation, Shared<RoomDefinition>& definition, Random& random) {
	(*definition).mClaimed = true;
	(*definition).mConnections[Facing::EAST]->mClaimed = true;
	return make_unique<OceanMonumentDoubleXRoom>(orientation, definition, random);
}
///////////////////////////////////////////////////////////////
//FIT DOUBLE Z ROOM
bool FitDoubleZRoom::fits(Shared<RoomDefinition>& definition) {
	if ((*definition).mHasOpening[Facing::NORTH] && !(*definition).mConnections[Facing::NORTH]->mClaimed) {
		return true;
	}
	return false;
}
Unique<OceanMonumentPiece> FitDoubleZRoom::create(int& orientation, Shared<RoomDefinition>& definition, Random& random) {
	Shared<RoomDefinition> source = definition;
	if (!((*definition).mHasOpening[Facing::NORTH] && !(*definition).mConnections[Facing::NORTH]->mClaimed)) {
		source = (*definition).mConnections[Facing::SOUTH];
	}
	source->mClaimed = true;
	source->mConnections[Facing::NORTH]->mClaimed = true;
	return make_unique<OceanMonumentDoubleZRoom>(orientation, definition, random);
}
///////////////////////////////////////////////////////////////
//FIT DOUBLE XY ROOM
bool FitDoubleXYRoom::fits(Shared<RoomDefinition>& definition) {
	if ((*definition).mHasOpening[Facing::EAST] && !(*definition).mConnections[Facing::EAST]->mClaimed) {
		if ((*definition).mHasOpening[Facing::UP] && !(*definition).mConnections[Facing::UP]->mClaimed) {
			RoomDefinition east = *(*definition).mConnections[Facing::EAST];
			//note: does not care if the two upper rooms are connected, since it's allowed to open new mConnections
			return east.mHasOpening[Facing::UP] && !east.mConnections[Facing::UP]->mClaimed;
		}
	}
	return false;
}
Unique<OceanMonumentPiece> FitDoubleXYRoom::create(int& orientation, Shared<RoomDefinition>& definition, Random& random) {
	(*definition).mClaimed = true;
	(*definition).mConnections[Facing::EAST]->mClaimed = true;
	(*definition).mConnections[Facing::UP]->mClaimed = true;
	(*definition).mConnections[Facing::EAST]->mConnections[Facing::UP]->mClaimed = true;
	return make_unique<OceanMonumentDoubleXYRoom>(orientation, definition, random);
}
///////////////////////////////////////////////////////////////
//FIT DOUBLE YZ ROOM
bool FitDoubleYZRoom::fits(Shared<RoomDefinition>& definition) {
	if ((*definition).mHasOpening[Facing::NORTH] && !(*definition).mConnections[Facing::NORTH]->mClaimed) {
		if ((*definition).mHasOpening[Facing::UP] && !(*definition).mConnections[Facing::UP]->mClaimed) {
			RoomDefinition north = *(*definition).mConnections[Facing::NORTH];
			// note: does not care if the two upper rooms are connected, since it's allowed to open new mConnections
			return north.mHasOpening[Facing::UP] && !north.mConnections[Facing::UP]->mClaimed;
		}
	}
	return false;
}
Unique<OceanMonumentPiece> FitDoubleYZRoom::create(int& orientation, Shared<RoomDefinition>& definition, Random& random) {
	(*definition).mClaimed = true;
	(*definition).mConnections[Facing::NORTH]->mClaimed = true;
	(*definition).mConnections[Facing::UP]->mClaimed = true;
	(*definition).mConnections[Facing::NORTH]->mConnections[Facing::UP]->mClaimed = true;
	return make_unique<OceanMonumentDoubleYZRoom>(orientation, definition, random);
}
