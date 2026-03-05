/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/

#include "Dungeons.h"

#include "world/level/levelgen/structure/StrongholdFeature.h"
#include "world/level/levelgen/structure/VillageFeature.h"
#include "world/level/levelgen/structure/StrongholdPieces.h"

#include "nbt/CompoundTag.h"
#include "world/level/Level.h"
#include "world/level/biome/Biome.h"
#include "world/level/biome/BiomeSource.h"
#include "world/level/chunk/LevelChunk.h"
#include "world/level/dimension/Dimension.h"
#include "world/entity/player/Player.h"
#include "Core/Debug/Log.h"

StrongholdFeature::StrongholdFeature(VillageFeature* villages)
	: StructureFeature()
	, isSpotSelected(false)
	, villages(villages) {
}

bool StrongholdFeature::isFeatureChunk(BiomeSource* biomeSource, Random& random, const ChunkPos& pos) {
	{
		//TODO refactor and use a RWLock
		LockGuard l(positionMutex);

		if (!isSpotSelected) {
			generatePositions(random, *biomeSource);
		}

		for (int i = 0; i < 3; i++) {
			if (selectedChunks[i] == pos) {
				return true;
			}
		}
	}

	// Check for additional strongholds (out of scope of the lock as the lock is not needed)
	return _hasAdditionalStronghold(*biomeSource, random, pos);
}

bool StrongholdFeature::getNearestGeneratedFeature(Player& player, BlockPos& pos) {
	if (!isSpotSelected) {
		auto& level = player.getLevel();
		generatePositions(level.getRandom(), level.getDimension(player.getDimensionId())->getBiomes());
		isSpotSelected = true;
	}

	return _getNearestStronghold(player, pos);
}

Unique<StructureStart> StrongholdFeature::createStructureStart(Dimension& generator, Random& random, const ChunkPos& cp){
	//LOGE("Created stronghold at %d,%d\n", x, z);
	return make_unique<StrongholdStart>(generator, random, cp.x, cp.z);
}

void StrongholdFeature::generatePositions(Random& random, BiomeSource& biomeSource) {
	// select 16x16 areas until three villages have been found
	random.setSeed(biomeSource.getSeed());

	int count = 0;
	float angle = PI * 2.0f * random.nextFloat();
	int chunkDist = 40 + random.nextInt(16);

	while (count < 3) {
		int cx = Math::floor(Math::cos(angle) * chunkDist);
		int cz = Math::floor(Math::sin(angle) * chunkDist);

		bool found = false;

		for (int chunkX = cx - 8; chunkX < cx + 8 && !found; chunkX++) {
			for (int chunkZ = cz - 8; chunkZ < cz + 8; chunkZ++) {
				ChunkPos cp(chunkX, chunkZ);
				if (villages->isFeatureChunk(&biomeSource, random, cp)) {
					selectedChunks[count] = cp;
					count++;
					found = true;
					break;
				}
			}
		}
		if (found) {
			angle += PI * .6f;	// slightly less than 1/3 of a circle
			chunkDist += 8;
		}
		else {
			angle += PI * .25f;// 1/8 of a circle
			chunkDist += 4;
		}
	}
	isSpotSelected = true;
}

std::vector<BlockPos> StrongholdFeature::getGuesstimatedFeaturePositions() {
	std::vector<BlockPos> result;
	for (auto& chunkPos : selectedChunks) {
		result.push_back(chunkPos.getMiddleBlockPosition(64));
	}
	return result;
}

bool StrongholdFeature::_hasAdditionalStronghold(BiomeSource& biomeSource, Random& random, const ChunkPos& cp) {
	// Are we far enough from the origin?
	if (!_isBeyondMinimumDistance(cp)) {
		return false;
	}

	// Attempt to "generate" a stronghold in proximity to this chunk and see if it happens here
	const StrongholdResult result = _generateStronghold(biomeSource, _getCenterOfGrid(cp));
	if (result.success) {
		if (result.location == cp) {
			return true;
		}
	}

	return false;
}

const StrongholdFeature::StrongholdResult StrongholdFeature::_generateStronghold(BiomeSource& biomeSource, const ChunkPos& cp) {
	StrongholdFeature::StrongholdResult result;
	result.success = false;

	// Are we far enough out to have one?
	if (!_isBeyondMinimumDistance(cp)) {
		return result;
	}
	
	// What gridsquare are we in?
	GridPos grid = _getGridCoordinates(cp);

	// Seed a new random influenced by the world seed
	Random strongholdRandom;
	int64_t seed = (cp.x * 784295783249l + cp.z * 827828252345l) + biomeSource.getSeed() + 97858791;
	strongholdRandom.setSeed((RandomSeed)(seed & 0xffffffff));

	// Generate potential coordinates for a stronghold
	int alpha = (grid.x * GRID_SIZE) + (GRID_SIZE - GRID_INSET);
	int beta = (grid.x * GRID_SIZE) + GRID_INSET;

	int gamma = (grid.z * GRID_SIZE) + (GRID_SIZE - GRID_INSET);
	int psi = (grid.z * GRID_SIZE) + GRID_INSET;

	int posX = strongholdRandom.nextInt(alpha, beta);
	int posZ = strongholdRandom.nextInt(gamma, psi);
	result.location = ChunkPos(posX, posZ);

	// Only if we satisfy our percent chance will there actually be a stronghold here
	if (strongholdRandom.nextFloat() < STRONGHOLD_CHANCE) {
		result.success = true;
	}

	return result;
}

bool StrongholdFeature::_getNearestStronghold(Player& player, BlockPos& pos) {
	ChunkPos cp(player.getPos());
	const GridPos myGridSquare = _getGridCoordinates(cp);
	auto& level = player.getLevel();

	// Otherwise, search in a spiral pattern outward from this gridsquare
	// Each ring or "layer" of the grid should be examined and all strongholds found,
	// Then their distances can be compared and the closest can be returned.
	// Search up to the maximum search distance (currently 100 rings of the grid)

	// Store all strongholds found for distance comparison -- start with 9 for the 3x3 grid centered on our grid's position
	std::vector<StrongholdFeature::StrongholdResult> strongholds(9);
	int resultIndex = 0;
	size_t numGridsToSearch = 1;

	for (int gridRing = 1; gridRing < MAX_GRID_SEARCH_DISTANCE; ++gridRing) {
		// Loop through all grid squares in this ring and attempt to generate a stronghold
		// Remember that rings are centered around the player and their grid square, not the world origin
		for (int x = -gridRing + myGridSquare.x; x <= gridRing + myGridSquare.x; ++x) {
			for (int z = -gridRing + myGridSquare.z; z <= gridRing + myGridSquare.z; ++z) {
				// Skip grid squares we have already searched in previous rings
				if (gridRing > 1) {
					if ((abs(myGridSquare.x - x) < gridRing) && (abs(myGridSquare.z - z) < gridRing)) {
						continue;
					}
				}

				numGridsToSearch += gridRing * 8;
				if (strongholds.size() < numGridsToSearch) {
					strongholds.resize(numGridsToSearch);
				}

				// Does this grid contain one of the pre-generated strongholds?
				ChunkPos pregen;
				GridPos here { x, z };
				// If so, store that location for distance comparison
				if (_isPregeneratedStrongholdHere(here, pregen)) {
					StrongholdResult preResult;
					preResult.success = true;
					preResult.location = pregen;          // Chunk Position (not Grid) of the stronghold
					strongholds[resultIndex] = preResult;
				}
				else {  // Otherwise, randomly generate a stronghold in this grid
					ChunkPos center((x * GRID_SIZE) + (GRID_SIZE / 2), (z * GRID_SIZE) + (GRID_SIZE / 2));
					// Get a chunk position in the center of this grid square and attempt to generate a stronghold there
					strongholds[resultIndex] = _generateStronghold(level.getDimension(player.getDimensionId())->getBiomes(), center);
				}
				
				++resultIndex;
			}
		}

#ifdef PUBLISH

		// Did we find any strongholds? If so, return the closest; otherwise, keep searching
		if (_hasStrongholds(strongholds)) {
			ChunkPos result = _closestChunkPos(cp, strongholds);
			pos = BlockPos(result, 32);
			return true;
		}

#else

		// If not in publish, search a 10 x 10 grid and output all of the stronghold locations to the log
		if (_hasStrongholds(strongholds) && gridRing > 10) {
			_logStrongholdData(strongholds);
			ChunkPos result = _closestChunkPos(cp, strongholds);
			pos = BlockPos(result, 32);
			return true;
		}
#endif
	}

	// No strongholds have been found within maximum search distance
	return false;
}

bool StrongholdFeature::_isBeyondMinimumDistance(const ChunkPos& cp) {
	int lengthSquared = cp.x * cp.x + cp.z * cp.z;
	if (lengthSquared < MIN_STRONGHOLD_DISTANCE * MIN_STRONGHOLD_DISTANCE) {
		return false;
	}

	return true;
}

GridPos StrongholdFeature::_getGridCoordinates(const ChunkPos& cp) {
	GridPos pos;
	pos.x = Math::floor((float)cp.x / GRID_SIZE);
	pos.z = Math::floor((float)cp.z / GRID_SIZE);
	return pos;
}

const ChunkPos& StrongholdFeature::_closestChunkPos(const ChunkPos& origin, std::vector<StrongholdFeature::StrongholdResult>& potentials) {
	int distance2 = INT_MAX;
	int closestIndex = 0;                                                // Index of closest chunk pos in array
	int size = potentials.size();

	for (int i = 0; i < size; ++i) {
		// Only consider successfully found strongholds
		if (!potentials[i].success) {
			continue;
		}

		int thisDistance = abs(origin.distanceToSqr(potentials[i].location));
		if (thisDistance < distance2) {
			distance2 = thisDistance;
			closestIndex = i;
		}
	}

	return potentials[closestIndex].location;
}

bool StrongholdFeature::_hasStrongholds(const std::vector<StrongholdFeature::StrongholdResult>& results) {
	int size = results.size();

	for (int i = 0; i < size; ++i) {
		if (results[i].success) {
			return true;
		}
	}

	return false;
}

bool StrongholdFeature::_sameGrid(const GridPos& lhs, const GridPos& rhs) {
	return lhs.x == rhs.x && lhs.z == rhs.z;
}

bool StrongholdFeature::_isPregeneratedStrongholdHere(const GridPos& gridPos, ChunkPos& outPos) {
	LockGuard l(positionMutex);

	for (int i = 0; i < TOTAL_VILLAGE_STRONGHOLDS; ++i) {
		// Get grid coordinates of this pregenerated stronghold
		GridPos preGrid = _getGridCoordinates(selectedChunks[i]);
		// If in the same grid square, store the Chunk Position of this pre-generated stronghold
		if (preGrid.x == gridPos.x && preGrid.z == gridPos.z) {
			outPos.x = selectedChunks[i].x;
			outPos.z = selectedChunks[i].z;
			return true;
		}
	}

	return false;
}

const ChunkPos StrongholdFeature::_getCenterOfGrid(const ChunkPos& cp) {
	GridPos grid = _getGridCoordinates(cp);
	ChunkPos center((grid.x * GRID_SIZE) + (GRID_SIZE / 2), (grid.z * GRID_SIZE) + (GRID_SIZE / 2));
	return center;
}

void StrongholdFeature::_logStrongholdData(const std::vector<StrongholdFeature::StrongholdResult>& results) {
	LOGI("####### BEGIN STRONGHOLD GENERATION DATA #######\n");
	
	// First log the original three strongholds and their positions
	LOGI("### NOTE: VALUES ARE X AND Z COORDS ###\n");
	LOGI("##### ORIGINAL STRONGHOLD GENERATION #####\n");
	for (int i = 0; i < TOTAL_VILLAGE_STRONGHOLDS; ++i) {
		BlockPos bp(selectedChunks[i]);
		LOGI("%i, %i\n", bp.x, bp.z);
	}

	// Now log all successfully generated additional strongholds
	LOGI("##### NEW STRONGHOLD GENERATION #####\n");

	size_t size = results.size();
	for (size_t j = 0; j < size; ++j) {
		if (results[j].success) {
			BlockPos bp(results[j].location, 32);
			LOGI("%i, %i\n", bp.x, bp.z);
		}
	}

	LOGI("####### END STRONGHOLD GENERATION DATA #######\n");
}

StrongholdStart::StrongholdStart(Dimension& source, Random& random, int chunkX, int chunkZ) :
	StructureStart(chunkX, chunkZ) {
	pieces.push_back(make_unique<SHStartPiece>(0, random, (chunkX << 4) + 2, (chunkZ << 4) + 2));

	auto start = (SHStartPiece*)pieces.back().get();
	
	PieceWeightList& pieceSet = start->pieceWeights;
	{
		pieceSet.push_back(PieceWeight("Straight", 40, 0));
		pieceSet.push_back(PieceWeight("PrisonHall", 5, 5));
		pieceSet.push_back(PieceWeight("LeftTurn", 20, 0));
		pieceSet.push_back(PieceWeight("RightTurn", 20, 0));
		pieceSet.push_back(PieceWeight("RoomCrossing", 10, 6));
		pieceSet.push_back(PieceWeight("StraightStairsDown", 5, 5));
		pieceSet.push_back(PieceWeight("StairsDown", 5, 5));
		pieceSet.push_back(PieceWeight("FiveCrossing", 5, 4));
		pieceSet.push_back(PieceWeight("ChestCorridor", 5, 4));
		pieceSet.push_back(PieceWeight("Library", 10, 2, 5));
		pieceSet.push_back(PieceWeight("PortalRoom", 10, 1, 6));
	}

	start->addChildren(start, pieces, random);	

	ReferencedPieceList& pendingChildren = start->pendingChildren;

	while (!pendingChildren.empty()) {
		int pos = random.nextInt(pendingChildren.size());
		StructurePiece* structurePiece = pendingChildren[pos];
		pendingChildren.erase(pendingChildren.begin() + pos);
		structurePiece->addChildren(start, pieces, random);
	}

	calculateBoundingBox();
	moveToLevel(random, source.getSeaLevel() - 5);

	valid = true;
}

void StrongholdStart::addAdditionalSaveData(CompoundTag& tag){
	StructureStart::addAdditionalSaveData(tag);

	tag.putBoolean("Valid", valid);
}

void StrongholdStart::readAdditionalSaveData(const CompoundTag& tag){
	StructureStart::readAdditionalSaveData(tag);
	valid = tag.getBoolean("Valid");
}
