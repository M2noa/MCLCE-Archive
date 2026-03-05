#pragma once

#include "world/level/levelgen/structure/StructurePiece.h"

struct MineshaftData {

	const FullBlock woodBlock, fenceBlock;

	float roomChance;

	MineshaftData(bool surface);
};

class MineshaftPiece : public StructurePiece {

protected:

	const static int DEFAULT_SHAFT_WIDTH = 3;
	const static int DEFAULT_SHAFT_HEIGHT = 3;
	const static int DEFAULT_SHAFT_LENGTH = 5;

	const static int MAX_DEPTH = 8;

	const static bool CHECK_WATER_EDGE = true;

public:

	MineshaftData metadata;

	MineshaftPiece(int depth, MineshaftData& metadata)
		: StructurePiece(depth)
		, metadata(metadata){

	}

	Unique<StructurePiece> createRandomShaftPiece(MineshaftData& metadata, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);
	StructurePiece* generateAndAddPiece(StructurePiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int depth);

	bool createChest(BlockSource* level, const BoundingBox& chunkBB, Random& random, int x, int y, int z, int direction, const std::string& lootTable) override;

	bool _isSupportingBox(const int x0, const int x1, BlockSource* level, const int y1, int z);

};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class MineshaftRoom : public MineshaftPiece {

public:

	MineshaftRoom(MineshaftData& metadata);
	MineshaftRoom(MineshaftData& metadata, int genDepth, Random& random, int west, int north);

	StructurePieceType getType() const override { 
		return StructurePieceType::MineshaftRoom;
	}

	virtual void addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) override;

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;

	void moveBoundingBox(int dx, int dy, int dz) override;

private:

	std::vector<BoundingBox> childEntranceBoxes;
};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class MineshaftCorridor : public MineshaftPiece {

public:

	static BoundingBox findCorridorSize(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction);

	MineshaftCorridor(MineshaftData& metadata);
	MineshaftCorridor(MineshaftData& metadata, int genDepth, Random& random, const BoundingBox& corridorBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::MineshaftCorridor;
	}

	virtual void addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) override;

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

	void _placeCobWeb(BlockSource* level, const BoundingBox& chunkBB, Random& random, float p, const int x0, const int y1, int z);

	void _placeSupport(BlockSource* level, const BoundingBox& chunkBB, const int x0, const int y0, int z, const int y1, const int x1, Random& random);

	void postProcessMobsAt(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;

private:

	bool hasRails, spiderCorridor, hasPlacedSpider;
	int numSections;
};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class MineshaftCrossing : public MineshaftPiece {

public:

	static BoundingBox findCrossing(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction);

	MineshaftCrossing(MineshaftData& metadata);
	MineshaftCrossing(MineshaftData& metadata, int genDepth, Random& random, const BoundingBox& crossingBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::MineshaftCrossing;
	}


	virtual void addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) override;

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;
	void _placeSupportPillar(BlockSource* level, const BoundingBox& chunkBB, int x, int y0, int z, int y1);

private:

	int direction;
	bool isTwoFloored;
};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class MineshaftStairs : public MineshaftPiece {

public:

	static BoundingBox findStairs(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction);

	MineshaftStairs(MineshaftData& metadata);
	MineshaftStairs(MineshaftData& metadata, int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::MineshaftStairs;
	}


	virtual void addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) override;

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

	void addAdditionalSaveData(CompoundTag& tag) const override {
	}

	void readAdditionalSaveData(const CompoundTag& tag) override {
	}

private:

};
