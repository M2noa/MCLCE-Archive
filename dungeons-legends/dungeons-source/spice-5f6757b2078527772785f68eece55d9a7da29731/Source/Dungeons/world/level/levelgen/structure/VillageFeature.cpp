#include "Dungeons.h"

#include "world/level/levelgen/structure/VillageFeature.h"
#include "world/level/levelgen/structure/VillagePieces.h"

#include "nbt/CompoundTag.h"
#include "world/level/Level.h"
#include "world/level/biome/Biome.h"
#include "world/level/biome/BiomeSource.h"
#include "world/level/chunk/LevelChunk.h"
#include "world/level/dimension/Dimension.h"

VillageFeature::VillageFeature()
	: StructureFeature(){
	radius = 4;
	allowedBiomes.push_back(Biome::plains->mId);
	allowedBiomes.push_back(Biome::desert->mId);
	allowedBiomes.push_back(Biome::savanna->mId);
	allowedBiomes.push_back(Biome::iceFlats->mId);
}

bool VillageFeature::isFeatureChunk(BiomeSource* biomeSource, Random& random, const ChunkPos& cp_){
	const int townSpacing = 40;
	const int minTownSeparation = 12;

	ChunkPos cp = cp_;
	ChunkPos temp = cp;
	if (cp.x < 0) {
		cp.x -= townSpacing - 1;
	}
	if (cp.z < 0) {
		cp.z -= townSpacing - 1;
	}

	int xCenterTownChunk = cp.x / townSpacing;
	int zCenterTownChunk = cp.z / townSpacing;

	int64_t seed = (cp.x * 341873128712l + cp.z * 132897987541l) + biomeSource->getSeed() + 10387312;
	random.setSeed((RandomSeed)(seed & 0xffffffff));
	xCenterTownChunk *= townSpacing;
	zCenterTownChunk *= townSpacing;
	xCenterTownChunk += random.nextInt(townSpacing - minTownSeparation);
	zCenterTownChunk += random.nextInt(townSpacing - minTownSeparation);
	cp = temp;

	if ((cp.x == xCenterTownChunk && cp.z == zCenterTownChunk)) {
		bool biomeOk = biomeSource->containsOnly(cp.x * 16 + 8, cp.z * 16 + 8, 0, allowedBiomes);
		//bool biomeOk = true;
		if (biomeOk) {
			return true;
		}
	}

	return false;
}

Unique<StructureStart> VillageFeature::createStructureStart(Dimension& generator, Random& random, const ChunkPos& cp){
	//LOGE("Created village at %d,%d\n", x, z);
	return make_unique<VillageStart>(&generator.getBiomes(), random, cp.x, cp.z, 0);
}

VillageStart::VillageStart(BiomeSource* biomeSource, Random& random, int chunkX, int chunkZ, int villageSizeModifier)
	: StructureStart(chunkX, chunkZ){

	PieceWeightList pieceSet;
	{
		int villageSize = villageSizeModifier;

		pieceSet = {
			{"SimpleHouse", 4, Math::nextInt(random, 2 + villageSize, 4 + villageSize * 2)},
			{ "SmallTemple", 20, Math::nextInt(random, 0 + villageSize, 1 + villageSize)},
			{ "BookHouse", 20, Math::nextInt(random, 0 + villageSize, 2 + villageSize)},
			{ "SmallHut", 3, Math::nextInt(random, 2 + villageSize, 5 + villageSize * 3) },
			{ "PigHouse", 15, Math::nextInt(random, 0 + villageSize, 2 + villageSize)},
			{ "DoubleFarmland", 3, Math::nextInt(random, 1 + villageSize, 4 + villageSize)},
			{ "Farmland", 3, Math::nextInt(random, 2 + villageSize, 4 + villageSize * 2)},
			{ "Smithy", 15, Math::nextInt(random, 0, 1 + villageSize)},
			{ "TwoRoomHouse", 8, Math::nextInt(random, 0 + villageSize, 3 + villageSize * 2)}
		};

		// silly way of filtering "infinite" buildings
		PieceWeightList::iterator it = pieceSet.begin();

		while (it != pieceSet.end()) {
			if ((*it).maxPlaceCount == 0) {
				it = pieceSet.erase(it);
			}
			else {
				++it;
			}
		}
	}

	Random abandonedRandom(random.getSeed());

	auto ptr = make_unique<StartPiece>(biomeSource, 0, random, (chunkX << 4) + 2, (chunkZ << 4) + 2, pieceSet, villageSizeModifier, abandonedRandom.nextInt(50) == 0);
	auto startRoom = ptr.get();
	pieces.push_back(std::move(ptr));
	startRoom->addChildren(startRoom, pieces, random);

	ReferencedPieceList& pendingRoads = startRoom->pendingRoads;
	ReferencedPieceList& pendingHouses = startRoom->pendingHouses;

	while (!pendingRoads.empty() || !pendingHouses.empty()) {

		// prioritize roads
		if (pendingRoads.empty()) {
			int pos = random.nextInt(pendingHouses.size());
			StructurePiece* structurePiece = pendingHouses[pos];
			pendingHouses.erase(pendingHouses.begin() + pos);
			structurePiece->addChildren(startRoom, pieces, random);
		}
		else {
			int pos = random.nextInt(pendingRoads.size());
			StructurePiece* structurePiece = pendingRoads[pos];
			pendingRoads.erase(pendingRoads.begin() + pos);
			structurePiece->addChildren(startRoom, pieces, random);
		}
	}

	calculateBoundingBox();

	//int count = 0;
	//for (auto&& piece : pieces) {
	//	if (!(piece instanceof VillageRoadPiece)) {
	//		count++;
	//	}
	//}
	//valid = count > 2;
	valid = true;
}

void VillageStart::addAdditionalSaveData(CompoundTag& tag){
	StructureStart::addAdditionalSaveData(tag);

	tag.putBoolean("Valid", valid);
}

void VillageStart::readAdditionalSaveData(const CompoundTag& tag){
	StructureStart::readAdditionalSaveData(tag);
	valid = tag.getBoolean("Valid");
}
