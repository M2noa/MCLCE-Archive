#pragma once

#include "world/level/levelgen/structure/StructurePiece.h"

class BiomeSource;

class StartPiece;
class VillagePiece;

class MossyStoneSelector : public BlockSelector {

public:

	void next(Random& random, int worldX, int worldY, int worldZ, bool isEdge) override;

};

class VillagePiece : public StructurePiece {

protected:

	const static int LOWEST_Y_POSITION = 10;
	const static int MAX_DEPTH = 50;
	const static int BASE_ROAD_DEPTH = 3;

	int heightPosition;
	int spawnedVillagerCount;
	bool isDesertVillage;
	bool isSavannahVillage;
	bool isTaigaVillage;
	bool isAbandoned;

public:

	VillagePiece();

	VillagePiece(StartPiece* startPiece, int genDepth);

	static MossyStoneSelector mossyStoneSelector;
	BlockSelector* mCobbleSelector = nullptr;

protected:

	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;

	Unique<VillagePiece> findAndCreatePieceFactory(StartPiece* startPiece, PieceWeight& piece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int depth);
	VillagePiece* generateAndAddPiece(StartPiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int depth);

	Unique<VillagePiece> generatePieceFromSmallDoor(StartPiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int depth);

	VillagePiece* generateHouseNorthernLeft(StartPiece* startPiece, PieceList& pieces, Random& random, int yOff, int zOff);
	VillagePiece* generateHouseNorthernRight(StartPiece* startPiece, PieceList& pieces, Random& random, int yOff, int zOff);

	VillagePiece* generateAndAddRoadPiece(StartPiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int depth);

	int getAverageGroundHeight(BlockSource* level, const BoundingBox& chunkBB);

	static bool isOkBox(const BoundingBox& box);

	void spawnVillagers(BlockSource* level, const BoundingBox& chunkBB, int x, int y, int z, int count);
	virtual int getVillagerProfession(int villagerNumber);

	FullBlock biomeBlock(FullBlock block);

	void placeBlock(BlockSource* level, FullBlock block, int x, int y, int z, const BoundingBox& chunkBB) override;
	void placeBlock(BlockSource* level, FullBlock defaultBlock, Random random, BlockSelector* selector, int x, int y, int z, const BoundingBox& chunkBB);
	void generateBoxPieces(BlockSource* level, const BoundingBox& chunkBB, int x0, int y0, int z0, int x1, int y1, int z1, FullBlock edgeBlock, FullBlock fillBlock, bool skipAir, float randomChance = 0.0f, FullBlock randomBlock = FullBlock());
	void generateBoxPieces(BlockSource* level, const BoundingBox& chunkBB, int x0, int y0, int z0, int x1, int y1, int z1, bool skipAir, FullBlock defaultBlock, Random& random, BlockSelector* selector);
	void fillColumnDown(BlockSource* level, FullBlock block, int x, int startY, int z, const BoundingBox& chunkBB) override;
	void createDoor(BlockSource* level, const BoundingBox& chunkBB, Random& random, int x, int y, int z, int orientation) override;

};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class Well : public VillagePiece {

public:

	Well();
	Well(StartPiece* startPiece, int genDepth, Random& random, const BoundingBox& stairsBox, int direction);
	Well(StartPiece* startPiece, int genDepth, Random& random, int west, int north);

	StructurePieceType getType() const override {
		return StructurePieceType::VillageWell;
	}

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

	const static int width = 6;
	const static int height = 15;
	const static int depth = 6;

};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class StartPiece : public Well {

public:
	StartPiece()
		:biomeSource(nullptr) {
	}

	StartPiece(BiomeSource* biomeSource, int genDepth, Random& random, int west, int north, PieceWeightList& pieceSet, int villageSize, bool abandoned = false);

	StructurePieceType getType() const override {
		return StructurePieceType::VillageStartPiece;
	}

	BiomeSource* getBiomeSource();

	PieceWeightList& getPieceWeights();

	virtual void addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) override;

public:

	std::string previousPiece;
	ReferencedPieceList pendingHouses;
	ReferencedPieceList pendingRoads;
	int villageSize;

protected:

	PieceWeightList pieceWeights;
	BiomeSource* biomeSource;
};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class SimpleHouse : public VillagePiece {

public:

	static Unique<VillagePiece> createPiece(StartPiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	SimpleHouse();
	SimpleHouse(StartPiece* startPiece, int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::VillageSimpleHouse;
	}

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;
	void postProcessMobsAt(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;

	const static int width = 5;
	const static int height = 6;
	const static int depth = 5;

private:

	bool hasTerrace;
};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class SmallTemple : public VillagePiece {

public:

	static Unique<VillagePiece> createPiece(StartPiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	SmallTemple();
	SmallTemple(StartPiece* startPiece, int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::VillageSmallTemple;
	}

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;
	void postProcessMobsAt(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;

	int getVillagerProfession(int villagerNumber) override;

	const static int width = 5;
	const static int height = 12;
	const static int depth = 9;
};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class BookHouse : public VillagePiece {

public:

	static Unique<VillagePiece> createPiece(StartPiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	BookHouse();
	BookHouse(StartPiece* startPiece, int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::VillageBookHouse;
	}

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;
	void postProcessMobsAt(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;

	int getVillagerProfession(int villagerNumber) override;

	const static int width = 9;
	const static int height = 9;
	const static int depth = 6;
};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class SmallHut : public VillagePiece {

public:

	static Unique<VillagePiece> createPiece(StartPiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	SmallHut();
	SmallHut(StartPiece* startPiece, int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::VillageSmallHut;
	}

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;
	void postProcessMobsAt(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;

	const static int width = 4;
	const static int height = 6;
	const static int depth = 5;

private:

	bool lowCeiling;
	int tablePlacement;
};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class PigHouse : public VillagePiece {

public:

	static Unique<VillagePiece> createPiece(StartPiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	PigHouse();
	PigHouse(StartPiece* startPiece, int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::VillagePigHouse;
	}

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;
	void postProcessMobsAt(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;

	int getVillagerProfession(int villagerNumber) override;

	const static int width = 9;
	const static int height = 7;
	const static int depth = 11;
};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class DoubleFarmland : public VillagePiece {

public:

	static Unique<VillagePiece> createPiece(StartPiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	DoubleFarmland();
	DoubleFarmland(StartPiece* startPiece, int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::VillageDoubleFarmland;
	}

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;

	const static int width = 13;
	const static int height = 4;
	const static int depth = 9;

private:

	BlockID cropsA, cropsB, cropsC, cropsD;
};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class Farmland : public VillagePiece {

public:

	static Unique<VillagePiece> createPiece(StartPiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	Farmland();
	Farmland(StartPiece* startPiece, int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::VillageFarmland;
	}

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;

	static BlockID selectCrops(Random& random, StartPiece& start);

	const static int width = 7;
	const static int height = 4;
	const static int depth = 9;

private:

	BlockID cropsA, cropsB;
};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class Smithy : public VillagePiece {

public:

	static Unique<VillagePiece> createPiece(StartPiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	Smithy();
	Smithy(StartPiece* startPiece, int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::VillageSmithy;
	}

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;
	void postProcessMobsAt(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;

	int getVillagerProfession(int villagerNumber) override;

	const static int width = 10;
	const static int height = 6;
	const static int depth = 7;

private:

	bool hasPlacedChest;
};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class TwoRoomHouse : public VillagePiece {

public:

	static Unique<VillagePiece> createPiece(StartPiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth);

	TwoRoomHouse();
	TwoRoomHouse(StartPiece* startPiece, int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::VillageTwoRoomHouse;
	}

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;
	void postProcessMobsAt(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;

	const static int width = 9;
	const static int height = 7;
	const static int depth = 12;

	bool hasPlacedChest = false;
};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class LightPost : public VillagePiece {

public:

	static BoundingBox findPieceBox(StartPiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction);

	LightPost();
	LightPost(StartPiece* startPiece, int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::VillageLightPost;
	}

	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

	const static int width = 3;
	const static int height = 4;
	const static int depth = 2;
};

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

class StraightRoad : public VillagePiece {

public:

	StraightRoad();
	StraightRoad(StartPiece* startPiece, int genDepth, Random& random, const BoundingBox& stairsBox, int direction);

	StructurePieceType getType() const override {
		return StructurePieceType::VillageStraightRoad;
	}

	virtual void addAdditionalSaveData(CompoundTag& tag) const override;
	virtual void readAdditionalSaveData(const CompoundTag& tag) override;

	void addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) override;
	bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) override;

	static BoundingBox findPieceBox(StartPiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction);

	const static int width = 3;

private:

	int length;
};
