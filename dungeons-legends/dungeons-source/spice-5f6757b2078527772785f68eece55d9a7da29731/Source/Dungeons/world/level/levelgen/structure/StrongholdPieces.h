#pragma once

#include "world/level/levelgen/structure/StructurePiece.h"

class SHStartPiece;
class StrongholdPiece;

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class SmoothStoneSelector : public BlockSelector {

public:

	void next(Random& random, int worldX, int worldY, int worldZ, bool isEdge) override;

};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class StrongholdPiece : public StructurePiece {

public:

	enum SmallDoorType {
		OPENING, WOOD_DOOR, GRATES, IRON_DOOR,
	};

protected:

	SmallDoorType entryDoor;

	const static int LOWEST_Y_POSITION = 10;
	const static int MAX_DEPTH = 50;
	const static int SMALL_DOOR_WIDTH = 3;
	const static int SMALL_DOOR_HEIGHT = 3;
	const static bool CHECK_AIR = false;
	const static bool CHECK_WATER_EDGE = false;

public:

	StrongholdPiece()
		: StructurePiece(0)
		, entryDoor(OPENING){

	}

	StrongholdPiece(int genDepth)
		: StructurePiece(genDepth)
		, entryDoor(OPENING){

	}

	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;

	Unique<StructurePiece> findAndCreatePieceFactory(const std::string& pieceClass, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int depth);

	Unique<StructurePiece> generatePieceFromSmallDoor(SHStartPiece* startPiece, PieceList& pieces, const Random& randomRef, int footX, int footY, int footZ, int direction, int depth);
	StructurePiece* generateAndAddPiece(SHStartPiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int depth);

	void generateSmallDoor(BlockSource* level, Random& random, const BoundingBox& chunkBB, SmallDoorType doorType, int footX, int footY, int footZ);
	StructurePiece* generateSmallDoorChildForward(SHStartPiece* startPiece, PieceList& pieces, Random& random, int xOff, int yOff);
	StructurePiece* generateSmallDoorChildLeft(SHStartPiece* startPiece, PieceList& pieces, Random& random, int yOff, int zOff);
	StructurePiece* generateSmallDoorChildRight(SHStartPiece* startPiece, PieceList& pieces, Random& random, int yOff, int zOff);
	SmallDoorType randomSmallDoor(Random& random);

	static bool isOkBox(const BoundingBox& box) {
		return box.y0 > LOWEST_Y_POSITION;
	}

	static SmoothStoneSelector smoothStoneSelector;

};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class SHStairsDown : public StrongholdPiece {

public:

	SHStairsDown();
	SHStairsDown(int genDepth, Random& random, const BoundingBox& stairsBox, int direction);
	SHStairsDown(int genDepth, Random& random, int west, int north);

	StructurePieceType getType() const override {
		return StructurePieceType::StrongholdStairsDown;
	}

	static Unique<StrongholdPiece> createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;

	virtual void addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) override;

	const static int width = 5;
	const static int height = 11;
	const static int depth = 5;

private:

	bool isSource;

};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class SHPortalRoom;

class SHStartPiece : public SHStairsDown {

public:

	SHStartPiece();
	SHStartPiece(int genDepth, Random& random, int west, int north);

	StructurePieceType getType() const override {
		return StructurePieceType::StrongholdPortalRoom;
	}

	ReferencedPieceList pendingChildren;
	SHPortalRoom* portalRoom;

	std::string imposedPiece;
	std::string previousPiece;
	PieceWeightList pieceWeights;

};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class SHChestCorridor : public StrongholdPiece {

public:

	SHChestCorridor();
	SHChestCorridor(int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::StrongholdChestCorridor;
	}

	static Unique<StrongholdPiece> createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;

	virtual void addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) override;

	const static int width = 5;
	const static int height = 5;
	const static int depth = 7;

private:

	bool hasPlacedChest;
};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class SHFillerCorridor : public StrongholdPiece {

public:

	SHFillerCorridor();
	SHFillerCorridor(int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::StrongholdFillerCorridor;
	}

	static BoundingBox findPieceBox(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction);

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;

	virtual void addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) override;

private:

	int steps;
};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class SHFiveCrossing : public StrongholdPiece {

public:

	SHFiveCrossing();
	SHFiveCrossing(int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::StrongholdFiveCrossing;
	}

	static Unique<StrongholdPiece> createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;

	virtual void addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) override;

	const static int width = 10;
	const static int height = 9;
	const static int depth = 11;

private:

	bool leftHigh, leftLow, rightHigh, rightLow;
};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class SHLeftTurn : public StrongholdPiece {

public:

	SHLeftTurn();
	SHLeftTurn(int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::StrongholdLeftTurn;
	}

	static Unique<StrongholdPiece> createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

	virtual void addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) override;

	const static int width = 5;
	const static int height = 5;
	const static int depth = 5;

private:

};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class SHRightTurn : public StrongholdPiece {

public:

	SHRightTurn();
	SHRightTurn(int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::StrongholdRightTurn;
	}

	static Unique<StrongholdPiece> createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

	virtual void addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) override;

	const static int width = 5;
	const static int height = 5;
	const static int depth = 5;

private:

};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class SHLibrary : public StrongholdPiece {

public:

	SHLibrary();
	SHLibrary(int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::StrongholdLibrary;
	}

	static Unique<StrongholdPiece> createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;

	virtual void addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) override;

	const static int width = 14;
	const static int height = 6;
	const static int tallHeight = 11;
	const static int depth = 15;

private:

	bool isTall;
};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class SHPortalRoom : public StrongholdPiece {

public:

	SHPortalRoom();
	SHPortalRoom(int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::StrongholdPortalRoom;
	}

	static Unique<StrongholdPiece> createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;

	virtual void addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) override;

	const static int width = 11;
	const static int height = 8;
	const static int depth = 16;

private:

	bool hasPlacedMobSpawner;
};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class SHPrisonHall : public StrongholdPiece {

public:

	SHPrisonHall();
	SHPrisonHall(int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::StrongholdPrisonHall;
	}

	static Unique<StrongholdPiece> createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

	virtual void addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) override;

	const static int width = 9;
	const static int height = 5;
	const static int depth = 11;

private:

};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class SHRoomCrossing : public StrongholdPiece {

public:

	SHRoomCrossing();
	SHRoomCrossing(int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::StrongholdRoomCrossing;
	}

	static Unique<StrongholdPiece> createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;

	virtual void addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) override;

	const static int width = 11;
	const static int height = 7;
	const static int depth = 11;

private:

	int type;
};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class SHStraight : public StrongholdPiece {

public:

	SHStraight();
	SHStraight(int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::StrongholdStraight;
	}

	static Unique<StrongholdPiece> createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;

	virtual void addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) override;

	const static int width = 5;
	const static int height = 5;
	const static int depth = 7;

private:

	bool leftChild, rightChild;
};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class SHStraightStairsDown : public StrongholdPiece {

public:

	SHStraightStairsDown();
	SHStraightStairsDown(int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::StrongholdStairsDown;
	}

	static Unique<StrongholdPiece> createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

	virtual void addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) override;

	const static int width = 5;
	const static int height = 11;
	const static int depth = 8;

private:

};
