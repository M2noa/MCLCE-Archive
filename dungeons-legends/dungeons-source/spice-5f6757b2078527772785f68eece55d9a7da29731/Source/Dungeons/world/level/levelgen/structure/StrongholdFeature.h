/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/levelgen/structure/StructureFeature.h"
#include "world/level/levelgen/structure/StructureStart.h"

#include "world/level/ChunkPos.h"

class VillageFeature;

// Represents one square in a virtual grid overlaid by the world, scale dictated by GRID_SIZE
struct GridPos {
	int x;
	int z;
};

// The strongholds are different to the PC version because they use the village feature to determine
// placement. Strongholds are always generated below a village.
class StrongholdFeature : public StructureFeature {

public:

	StrongholdFeature(VillageFeature* villages);

	std::string getFeatureName() override {
		return "Stronghold";
	}

	bool isFeatureChunk(BiomeSource* biomeSource, Random& random, const ChunkPos& lc) override;
	virtual bool getNearestGeneratedFeature(Player& player, BlockPos& pos) override;

protected:

	Unique<StructureStart> createStructureStart(Dimension& generator, Random& random, const ChunkPos& lc) override;

	void generatePositions(Random& random, BiomeSource& biomeSource);
	virtual std::vector<BlockPos> getGuesstimatedFeaturePositions() override;

	bool isSpotSelected;
	ChunkPos selectedChunks[3];
	VillageFeature* villages;

	std::mutex positionMutex;

private:

	StrongholdFeature(const StrongholdFeature& copy) {
		UNUSED_PARAMETER(copy);
	}

	const int TOTAL_VILLAGE_STRONGHOLDS = 3;     // Number of strongholds pre-generated under villages using the previous method
	const int GRID_SIZE = 200;                   // Size of each grid square, start at 100 chunks
	const int GRID_INSET = 150;                  // Size of each inner grid square where strongholds can spawn, start at 75 chunks
	const int MIN_STRONGHOLD_DISTANCE = 10;      // Minimum distance from world origin to start spawning strongholds
	const float STRONGHOLD_CHANCE = 0.25f;       // Percent chance that a stronghold will spawn in each grid square inset
	const int MAX_GRID_SEARCH_DISTANCE = 100;    // Maximum radius of the grid to search for strongholds before giving up

	struct StrongholdResult {
		bool success;
		ChunkPos location;
	};

	// Does this chunk contain an additional stronghold?
	bool _hasAdditionalStronghold(BiomeSource& biomeSource, Random& random, const ChunkPos& cp);

	// Generate a stronghold based on constants
	const StrongholdResult _generateStronghold(BiomeSource& biomeSource, const ChunkPos& cp);

	// Find the nearest stronghold position to the player
	bool _getNearestStronghold(Player& player, BlockPos& pos);

	// Helper function to determine if this position is far enough from world origin to generate a stronghold
	bool _isBeyondMinimumDistance(const ChunkPos& cp);

	// Convert chunk pos to grid square -- find which grid square this chunk is in
	GridPos _getGridCoordinates(const ChunkPos& cp);

	// Find closest chunk pos in a vector to another chunk pos
	const ChunkPos& _closestChunkPos(const ChunkPos& origin, std::vector<StrongholdResult>& potentials);

	// Determine if any results in a vector have strongholds
	bool _hasStrongholds(const std::vector<StrongholdResult>& results);

	// Quick helper for GridPos equivalence
	bool _sameGrid(const GridPos& lhs, const GridPos& rhs);

	// Returns true if there exists a pre-generated stronghold at specified coordinates
	bool _isPregeneratedStrongholdHere(const GridPos& gridPos, ChunkPos& outPos);

	// Provide ChunkPos at the center of the grid square the given ChunkPos is in
	const ChunkPos _getCenterOfGrid(const ChunkPos& cp);

	// Log positional data for all strongholds found and generated, old and new, for debugging purposes
	void _logStrongholdData(const std::vector<StrongholdResult>& results);
};

class StrongholdStart : public StructureStart {

public:

	StrongholdStart()
		: StructureStart(0, 0)
		, valid(false){
	}

	StrongholdStart(Dimension& biomeSource, Random& random, int chunkX, int chunkZ);

	virtual ~StrongholdStart() {
	}

	bool isValid() const override {
		return valid;
	}

	void addAdditionalSaveData(CompoundTag& tag) override;
	void readAdditionalSaveData(const CompoundTag& tag) override;

	StructureFeatureType getType() const override {
		return StructureFeatureType::Stronghold;
	}

private:

	bool valid;

};
