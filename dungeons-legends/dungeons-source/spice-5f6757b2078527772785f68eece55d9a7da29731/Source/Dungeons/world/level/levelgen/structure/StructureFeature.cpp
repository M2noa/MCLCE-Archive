/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/entity/player/Player.h"

#include "world/level/levelgen/structure/StructureFeature.h"
#include "world/level/levelgen/structure/StructureStart.h"
#include "world/level/levelgen/structure/StructurePiece.h"

#include "world/level/Level.h"
#include "world/level/ChunkPos.h"
#include "world/level/dimension/Dimension.h"

#include "world/level/levelgen/structure/EndCityFeature.h"
#include "world/level/levelgen/structure/MineshaftFeature.h"
#include "world/level/levelgen/structure/NetherBridgeFeature.h"
#include "world/level/levelgen/structure/OceanMonumentFeature.h"
#include "world/level/levelgen/structure/RandomScatteredLargeFeature.h"
#include "world/level/levelgen/structure/StrongholdFeature.h"
#include "world/level/levelgen/structure/VillageFeature.h"
#include "nbt/ListTag.h"


void StructureFeature::addFeature(LevelChunk& lc, Dimension& generator, Random& random, int x, int z ){
	// restoreSavedData(level);

	// this method is called for each chunk within 8 chunk's distance from
	// the chunk being generated, but not all chunks are the sources of
	// structures

	// clear random key
	random.nextInt();
	ChunkPos cp(x, z);

	if (!isFeatureChunk(&generator.getBiomes(), random, cp)) {
		return;
	}

	{
		LockGuard l(cacheMutex);
		if (cachedStructures.count(cp) == 0) {
			cachedStructures[cp] = createStructureStart(generator, random, cp);
		}
	}
}

bool StructureFeature::postProcess(BlockSource* level, Random& random, int chunkX, int chunkZ) {

	LockGuard l(cacheMutex);

	//restoreSavedData(level);

	int cx = (chunkX << 4);	// +8;
	int cz = (chunkZ << 4);	// +8;

	bool intersection = false;

	for (auto& pair : cachedStructures) {
		Unique<StructureStart>& start = pair.second;
		if (start->isValid()) {
			//if (start->getBoundingBox().intersects(cx - 16, cz - 16, cx + 32, cz + 32))
			if (start->getBoundingBox().intersects(cx, cz, cx + 16, cz + 16)) {

				//for (int cxo = chunkX - 1; cxo <= chunkX + 1; cxo++)
				{
					int cxo = chunkX;
					//for (int czo = chunkZ - 1; czo <= chunkZ + 1; czo++)
					{
						int czo = chunkZ;
						int chunkHash = ChunkPos::hashCode(cxo, czo);
						bool alreadyDone = false;

						for (unsigned int i = 0; i < start->generatedChunkPositions.size(); i++) {
							if (chunkHash == start->generatedChunkPositions[i]) {
								alreadyDone = true;
								break;
							}
						}
						if (!alreadyDone) {
							intersection |= start->postProcess(level, random, BoundingBox((cxo << 4), (czo << 4), (cxo << 4) + 15, (czo << 4) + 15));
							start->generatedChunkPositions.push_back(chunkHash);
						}
					}
				}

				// because some feature pieces are modified in the postProcess step, we need to save them again
				// if (intersection) {
				//saveFeature(structureStart.getChunkX(), structureStart.getChunkZ(), structureStart);
				// }
			}
		}
	}
	return intersection;
}

void StructureFeature::postProcessMobsAt(BlockSource* blockSource, int chunkWestBlock, int chunkNorthBlock, Random& random){
	LockGuard l(cacheMutex);

	//restoreSavedData(level);

	BoundingBox bb(chunkWestBlock, chunkNorthBlock, chunkWestBlock + 16, chunkNorthBlock + 16);

	for (auto& pair : cachedStructures) {
		auto& start = pair.second;
		if (start->isValid()) {
			if (start->getBoundingBox().intersects(bb)) {
				start->postProcessMobsAt(blockSource, random, bb);
			}
		}
	}
}

bool StructureFeature::isInsideFeature(int cellX, int cellY, int cellZ) {
	//restoreSavedData(level);
	return getStructureAt(cellX, cellY, cellZ) != nullptr;
}

StructureStart* StructureFeature::getStructureAt(int cellX, int cellY, int cellZ) {
	for (auto& pair : cachedStructures) {
		auto& start = pair.second;
		if (start->isValid()) {
			if (start->getBoundingBox().intersects(cellX, cellZ, cellX, cellZ)) {

				PieceList::const_iterator it = start->getPieces().cbegin();

				while (it != start->getPieces().cend()) {
					const Unique<StructurePiece>& next = (*it);
					if (next->getBoundingBox().isInside(cellX, cellY, cellZ)) {
						return start.get();
					}
					++it;
				}
			}
		}
	}
	return nullptr;
}

std::vector<BlockPos> StructureFeature::getGuesstimatedFeaturePositions() {
	return std::vector<BlockPos>();
}

bool StructureFeature::isInsideBoundingFeature(int cellX, int cellY, int cellZ) {

	//restoreSavedData(level);

	for (auto& pair : cachedStructures) {
		auto& start = pair.second;
		if (start->isValid()) {
			return start->getBoundingBox().intersects(cellX, cellZ, cellX, cellZ);
		}
	}
	return false;
}

bool StructureFeature::getNearestGeneratedFeature(Player& player, BlockPos& pos) {
	return false;
}

Unique<CompoundTag> StructureFeature::getStructureTag() const {
	if (cachedStructures.size() > 0) {

		Unique<CompoundTag> structures = make_unique<CompoundTag>();

		auto features = make_unique<ListTag>();
		for (auto& pair : cachedStructures) {
			features->add(pair.second->createTag(pair.first.x, pair.first.z));
		}

		structures->put("structures", std::move(features));
		return structures;
	}
	return make_unique<CompoundTag>();
}

// Yeah, I know. This sucks, but ZBR was two days ago.
static Unique<StructureStart> createStructure(StructureFeatureType type) {
	switch (type) {
	case StructureFeatureType::Village:
		return make_unique<VillageStart>();
	case StructureFeatureType::Monument:
		return make_unique<OceanMonumentStart>();
	case StructureFeatureType::Temple:
		return make_unique<ScatteredFeatureStart>();
	case StructureFeatureType::Fortress:
		return make_unique<NetherBridgeStart>();
	case StructureFeatureType::Mineshaft:
		return make_unique<MineshaftStart>();
	case StructureFeatureType::Stronghold:
		return make_unique<StrongholdStart>();
	case StructureFeatureType::EndCity:
		return make_unique<EndCityStart>();
	default:
		break;
	}
	// Can't return StructureStart as it's pure virtual :(
	DEBUG_ASSERT(false, "You forgot to add your new structure type to createStructure");
	return make_unique<VillageStart>();
}

void StructureFeature::readStructureTag(const CompoundTag& tag, Level& level) {
	cachedStructures.clear();

	const ListTag* list= tag.getList("structures");
	if (list != nullptr) {
		for (int i = 0; i < list->size(); i++) {
			const CompoundTag* compoundTag = list->getCompound(i);
			int structureType= compoundTag->getInt("ID");
			Unique<StructureStart> structureStart = createStructure(static_cast<StructureFeatureType>(structureType));
			structureStart->load(*compoundTag, level);
			ChunkPos cp(structureStart->getChunkX(), structureStart->getChunkZ());
			{
				// Shouldn't be necessary as we only need to load on load of the level
				LockGuard l(cacheMutex);
				cachedStructures[cp] = std::move(structureStart);
			}
		}
	}
}
