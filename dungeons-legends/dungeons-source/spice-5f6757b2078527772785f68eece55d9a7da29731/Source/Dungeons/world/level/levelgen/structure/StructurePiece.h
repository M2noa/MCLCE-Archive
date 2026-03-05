#pragma once

#include "CommonTypes.h"
#include "world/level/levelgen/structure/BoundingBox.h"
#include "world/level/levelgen/structure/StructurePieceType.h"
#include "world/Direction.h"
#include "world/level/BlockPos.h"
#include "util/WeighedRandom.h"
#include "world/item/ItemInstance.h"

class Level;
class BlockSource;
class Random;
class Block;

class CompoundTag;

class ChunkPos;
class StructurePiece;
typedef std::vector<Unique<StructurePiece> > PieceList;
typedef std::vector<StructurePiece*> ReferencedPieceList;

class BlockSelector {
protected:

	FullBlock nextBlock;

public:

	virtual ~BlockSelector() {
	}

	virtual void next(Random& random, int worldX, int worldY, int worldZ, bool isEdge) = 0;

	FullBlock getNext() const {
		return nextBlock;
	}

};

class PieceWeight {
public:

	std::string pieceClass;
	int weight;
	int placeCount;
	int maxPlaceCount;
	int minDepth;
	bool allowInRow;

	PieceWeight(const std::string& pieceClass, int weight, int maxPlaceCount, int minDepth = 0, bool allowInRow = false) {
		this->pieceClass = pieceClass;
		this->weight = weight;
		this->maxPlaceCount = maxPlaceCount;
		this->placeCount = 0;
		this->minDepth = minDepth;
		this->allowInRow = allowInRow;
	}

	bool doPlace(int depth) {
		return (maxPlaceCount == 0 || placeCount < maxPlaceCount) && depth >= minDepth;
	}

	bool isValid() {
		return maxPlaceCount == 0 || placeCount < maxPlaceCount;
	}

};

typedef std::vector<PieceWeight> PieceWeightList;

class StructurePiece {
public:

	StructurePiece(int genDepth)
		: orientation(Direction::UNDEFINED)
		, genDepth(genDepth){

	}

	virtual ~StructurePiece() {
	}

	const BoundingBox& getBoundingBox() const {
		return boundingBox;
	}

	virtual void moveBoundingBox(int dx, int dy, int dz) {
		boundingBox.move(dx, dy, dz);
	}

	int getGenDepth() const {
		return genDepth;
	}

	void setGenDepth(int newDepth) {
		genDepth = newDepth;
	}

	virtual StructurePieceType getType() const { return StructurePieceType::Unknown; }

	virtual void addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random);

	virtual bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) = 0;
	virtual void postProcessMobsAt(BlockSource* level, Random& random, const BoundingBox& chunkBB);

	Unique<CompoundTag> createTag() const;
	void loadFromTag(const CompoundTag& tag);

	bool isInChunk(const ChunkPos& pos) const;
	BlockPos getLocatorPosition() const;

	static StructurePiece* findCollisionPiece(const PieceList& pieces, const BoundingBox& box);
	bool edgesLiquid(BlockSource* level, const BoundingBox& chunkBB);

	virtual int getWorldX(int x, int z);
	int getWorldY(int y);
	virtual int getWorldZ(int x, int z);

	Brightness getBrightness(int x0, int y1, int z, BlockSource* level);

	DataID getOrientationData(const Block* block, DataID data);
	DataID getTorchData(DataID torchOrientation);	// extra method since it's used so frequently

	virtual void placeBlock(BlockSource* level, FullBlock block, int x, int y, int z, const BoundingBox& chunkBB);

	void placeBlock(BlockSource* level, BlockID block, DataID data, int x, int y, int z, const BoundingBox& chunkBB) {
		placeBlock(level, FullBlock(block, data), x, y, z, chunkBB);
	}

	BlockPos _getWorldPos(int x, int y, int z);

	BlockID getBlock(BlockSource* level, int x, int y, int z, const BoundingBox& chunkBB);
	bool isAir(BlockSource* level, int x, int y, int z, const BoundingBox& chunkBB);

	void generateAirBox(BlockSource* level, const BoundingBox& chunkBB, int x0, int y0, int z0, int x1, int y1, int z1);
	virtual void generateBox(BlockSource* level, const BoundingBox& chunkBB, int x0, int y0, int z0, int x1, int y1, int z1, FullBlock edgeBlock, FullBlock fillBlock, bool skipAir);
	void generateBox(BlockSource* level, const BoundingBox& chunkBB, int x0, int y0, int z0, int x1, int y1, int z1, BlockID edgeBlock, DataID edgeData, BlockID fillBlock, DataID fillData, bool skipAir);
	void generateBox(BlockSource* level, const BoundingBox& chunkBB, const BoundingBox& boxBB, BlockID edgeBlock, BlockID fillBlock, bool skipAir);
	void generateBox(BlockSource* level, const BoundingBox& chunkBB, int x0, int y0, int z0, int x1, int y1, int z1, bool skipAir, Random& random, BlockSelector* selector);
	void generateBox(BlockSource* level, const BoundingBox& chunkBB, const BoundingBox& boxBB, bool skipAir, Random& random, BlockSelector* selector);
	void generateMaybeBox(BlockSource* level, const BoundingBox& chunkBB, Random& random, float probability, int x0, int y0, int z0, int x1, int y1, int z1, BlockID edgeBlock, BlockID fillBlock, bool skipAir, Brightness maxBrightness = Brightness::MIN);
	void maybeGenerateBlock(BlockSource* level, const BoundingBox& chunkBB, Random& random, float probability, int x, int y, int z, BlockID block, DataID data);
	void generateUpperHalfSphere(BlockSource* level, const BoundingBox& chunkBB, int x0, int y0, int z0, int x1, int y1, int z1, BlockID fillBlock, bool skipAir);
	void generateAirColumnUp(BlockSource* level, int x, int startY, int z, const BoundingBox& chunkBB);
	virtual void fillColumnDown(BlockSource* level, FullBlock block, int x, int startY, int z, const BoundingBox& chunkBB);

	virtual bool createChest(BlockSource* level, const BoundingBox& chunkBB, Random& random, int x, int y, int z, int direction, const std::string& lootTable);
	virtual bool createDispenser(BlockSource* level, const BoundingBox& chunkBB, Random& random, int x, int y, int z, int facing, const std::string& lootTable);
	virtual void createDoor(BlockSource* level, const BoundingBox& chunkBB, Random& random, int x, int y, int z, int orientation);

	static int getTotalWeight(const PieceWeightList& pieceWeights);

protected:

	virtual void addAdditionalSaveData(CompoundTag& tag) const = 0;
	virtual void readAdditionalSaveData(const CompoundTag& tag) = 0;

	BoundingBox boundingBox;
	int orientation;
	int genDepth;

};
