#pragma once

#include "world/level/levelgen/structure/BoundingBox.h"
#include "world/level/levelgen/structure/StructurePiece.h"

class Level;
class Random;
class CompoundTag;
enum class StructureFeatureType : int8_t;

class StructureStart {
public:

	StructureStart(int x, int z)
		: chunkX(x)
		, chunkZ(z){

	}

	virtual ~StructureStart() {
	}

	virtual bool postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB);
	void postProcessMobsAt(BlockSource* blockSource, Random& random, const BoundingBox& chunkBB);

	virtual bool isValid() const {
		return true;
	}

	int getChunkX() const {
		return chunkX;
	}

	int getChunkZ() const {
		return chunkZ;
	}

	const BoundingBox& getBoundingBox() const {
		return boundingBox;
	}

	const PieceList& getPieces() const {
		return pieces;
	}

	virtual StructureFeatureType getType() const = 0;

	Unique<CompoundTag> createTag(int chunkX, int chunkZ);
	void load(const CompoundTag& tag, Level& level);
	virtual void addAdditionalSaveData(CompoundTag& tag);
	virtual void readAdditionalSaveData(const CompoundTag& tag);

protected:

	void calculateBoundingBox();
	void moveToLevel(Random& random, int maxY);
	void moveInsideHeights(Random* random, int lowestAllowed, int highestAllowed);

	BoundingBox boundingBox;
	int chunkX, chunkZ;
	PieceList pieces;

public:

	std::vector<int> generatedChunkPositions;

};
