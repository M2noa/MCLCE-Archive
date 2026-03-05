#pragma once

#include "world/level/levelgen/structure/StructurePiece.h"

class NBStartPiece;

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class NetherBridgePiece : public StructurePiece {

public:

	NetherBridgePiece();
	NetherBridgePiece(int genDepth);

	int updatePieceWeight(PieceWeightList& currentPieces);

	Unique<NetherBridgePiece> findAndCreateBridgePieceFactory(const std::string& pieceClass, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int depth);

	Unique<NetherBridgePiece> generatePiece(NBStartPiece* startPiece, PieceWeightList& currentPieces, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int depth);

	StructurePiece* generateAndAddPiece(NBStartPiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int depth, bool isCastle);
	StructurePiece* generateChildForward(NBStartPiece* startPiece, PieceList& pieces, Random& random, int xOff, int yOff, bool isCastle);
	StructurePiece* generateChildLeft(NBStartPiece* startPiece, PieceList& pieces, Random& random, int yOff, int zOff, bool isCastle);
	StructurePiece* generateChildRight(NBStartPiece* startPiece, PieceList& pieces, Random& random, int yOff, int zOff, bool isCastle);

	void generateLightPost(BlockSource* level, Random& random, const BoundingBox& chunkBB, int x, int y, int z, int xOff, int zOff);
	void generateLightPostFacingRight(BlockSource*  level, Random& random, const BoundingBox& chunkBB, int x, int y, int z);
	void generateLightPostFacingLeft(BlockSource*  level, Random& random, const BoundingBox& chunkBB, int x, int y, int z);
	void generateLightPostFacingUp(BlockSource*  level, Random& random, const BoundingBox& chunkBB, int x, int y, int z);
	void generateLightPostFacingDown(BlockSource*  level, Random& random, const BoundingBox& chunkBB, int x, int y, int z);

	static bool isOkBox(BoundingBox box);

	virtual bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

protected:

	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;

};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class NBBridgeCrossing : public NetherBridgePiece {

	static const int width = 19;
	static const int height = 10;
	static const int depth = 19;

public:

	NBBridgeCrossing();
	NBBridgeCrossing(int genDepth, Random& random, const BoundingBox& stairsBox, int direction);
	NBBridgeCrossing(Random& random, int west, int north);

	StructurePieceType getType() const override {
		return StructurePieceType::NetherBridgeCrossing;
	}


	static Unique<NetherBridgePiece> createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	virtual void addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) override;
	virtual bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class NBStartPiece : public NBBridgeCrossing {

public:

	NBStartPiece();
	NBStartPiece(Random& random, int west, int north);

	StructurePieceType getType() const override {
		return StructurePieceType::NetherBridgeStartPiece;
	}

	std::string previousPiece;

	PieceWeightList availableBridgePieces;
	PieceWeightList availableCastlePieces;

	ReferencedPieceList pendingChildren;

protected:

	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;

};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class NBBridgeStraight : public NetherBridgePiece {

	static const int width = 5;
	static const int height = 10;
	static const int depth = 19;

public:

	NBBridgeStraight();
	NBBridgeStraight(int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::NetherBridgeStraight;
	}

	static Unique<NetherBridgePiece> createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	virtual void addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) override;
	virtual bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class NBBridgeEndFiller : public NetherBridgePiece {
	static const int width = 5;
	static const int height = 10;
	static const int depth = 8;

	int selfSeed;

public:

	NBBridgeEndFiller();
	NBBridgeEndFiller(int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::NetherBridgeEndFiller;
	}

	static Unique<NetherBridgePiece> createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	virtual bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

protected:

	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;

};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class NBRoomCrossing : public NetherBridgePiece {

	static const int width = 7;
	static const int height = 9;
	static const int depth = 7;

public:

	NBRoomCrossing();
	NBRoomCrossing(int genDepth, Random& random, const BoundingBox& box, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::NetherBridgeRoomCrossing;
	}

	static Unique<NetherBridgePiece> createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	virtual void addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) override;
	virtual bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class NBStairsRoom : public NetherBridgePiece {

	static const int width = 7;
	static const int height = 11;
	static const int depth = 7;

public:

	NBStairsRoom();
	NBStairsRoom(int genDepth, Random& random, const BoundingBox& box, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::NetherBridgeStairsRoom;
	}

	static Unique<NetherBridgePiece> createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	virtual void addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) override;
	virtual bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class NBMonsterThrone : public NetherBridgePiece {

	static const int width = 7;
	static const int height = 8;
	static const int depth = 9;

	bool hasPlacedMobSpawner = false;

public:

	NBMonsterThrone();
	NBMonsterThrone(int genDepth, Random& random, const BoundingBox& box, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::NetherBridgeMonsterThrone;
	}

	static Unique<NetherBridgePiece> createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	virtual bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

protected:

	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;

};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class NBCastleEntrance : public NetherBridgePiece {

	static const int width = 13;
	static const int height = 14;
	static const int depth = 13;

public:

	NBCastleEntrance();
	NBCastleEntrance(int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::NetherCastleEntrance;
	}

	static Unique<NetherBridgePiece> createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	virtual void addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) override;
	virtual bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class NBCastleStalkRoom : public NetherBridgePiece {

	static const int width = 13;
	static const int height = 14;
	static const int depth = 13;

public:

	NBCastleStalkRoom();
	NBCastleStalkRoom(int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::NetherCastleStalkRoom;
	}

	static Unique<NetherBridgePiece> createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	virtual void addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) override;
	virtual bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class NBCastleSmallCorridorPiece : public NetherBridgePiece {

	static const int width = 5;
	static const int height = 7;
	static const int depth = 5;

public:

	NBCastleSmallCorridorPiece();
	NBCastleSmallCorridorPiece(int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::NetherCastleSmallCorridor;
	}

	static Unique<NetherBridgePiece> createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	virtual void addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) override;
	virtual bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class NBCastleSmallCorridorCrossingPiece : public NetherBridgePiece {

	static const int width = 5;
	static const int height = 7;
	static const int depth = 5;

public:

	NBCastleSmallCorridorCrossingPiece();
	NBCastleSmallCorridorCrossingPiece(int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::NetherCastleSmallCorridorCrossing;
	}

	static Unique<NetherBridgePiece> createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	virtual void addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) override;
	virtual bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class NBCastleSmallCorridorRightTurnPiece : public NetherBridgePiece {

	static const int width = 5;
	static const int height = 7;
	static const int depth = 5;

	bool isNeedingChest;

public:

	NBCastleSmallCorridorRightTurnPiece();
	NBCastleSmallCorridorRightTurnPiece(int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::NetherCastleSmallCorridorRightTurn;
	}

	static Unique<NetherBridgePiece> createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	virtual void addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) override;
	virtual bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

protected:

	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;

};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class NBCastleSmallCorridorLeftTurnPiece : public NetherBridgePiece {

	static const int width = 5;
	static const int height = 7;
	static const int depth = 5;

	bool isNeedingChest;

public:

	NBCastleSmallCorridorLeftTurnPiece();
	NBCastleSmallCorridorLeftTurnPiece(int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::NetherCastleSmallCorridorLeftTurn;
	}

	static Unique<NetherBridgePiece> createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	virtual void addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) override;
	virtual bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

protected:

	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;

};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class NBCastleCorridorStairsPiece : public NetherBridgePiece {

	static const int width = 5;
	static const int height = 14;
	static const int depth = 10;

public:

	NBCastleCorridorStairsPiece();
	NBCastleCorridorStairsPiece(int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::NetherCastleCorridorStairs;
	}

	static Unique<NetherBridgePiece> createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	virtual void addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) override;
	virtual bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class NBCastleCorridorTBalconyPiece : public NetherBridgePiece {

	static const int width = 9;
	static const int height = 7;
	static const int depth = 9;

public:

	NBCastleCorridorTBalconyPiece();
	NBCastleCorridorTBalconyPiece(int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::NetherCastleCorridorTBalcony;
	}

	static Unique<NetherBridgePiece> createPiece(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	virtual void addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) override;
	virtual bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

};
