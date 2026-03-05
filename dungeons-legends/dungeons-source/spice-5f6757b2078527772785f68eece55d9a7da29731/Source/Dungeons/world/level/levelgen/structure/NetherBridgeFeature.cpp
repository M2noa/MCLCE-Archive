#include "Dungeons.h"

#include "world/level/levelgen/structure/NetherBridgeFeature.h"
#include "world/level/levelgen/structure/NetherBridgePieces.h"

#include "world/entity/Entity.h"
#include "world/level/biome/BiomeSource.h"

NetherBridgeFeature::NetherBridgeFeature() :
	StructureFeature(){
	bridgeEnemies.insert(bridgeEnemies.end(), MobSpawnerData(EntityType::Blaze, 10, 2, 3, nullptr, nullptr, true));
	bridgeEnemies.insert(bridgeEnemies.end(), MobSpawnerData(EntityType::PigZombie, 5, 4, 4, nullptr, nullptr, true));
	bridgeEnemies.insert(bridgeEnemies.end(), MobSpawnerData(EntityType::LavaSlime, 3, 4, 4, nullptr, nullptr, true));
	bridgeEnemies.insert(bridgeEnemies.end(), MobSpawnerData(EntityType::Skeleton, 10, 4, 4, nullptr, nullptr, true));
}

std::string NetherBridgeFeature::getFeatureName() {
	return "Fortress";
}

NetherBridgeFeature::MobList& NetherBridgeFeature::getBridgeEnemies() {
	Random rand;
	int spawnWither = rand.nextInt(0, 10);
	EntityType skelType;

	//80% chance nether fortress skeletons will be wither skeletons
	if (spawnWither < 2) {
		skelType = EntityType::Skeleton;
	}
	else {
		skelType = EntityType::WitherSkeleton;
	}
	bridgeEnemies[bridgeEnemies.size() - 1].mobClassId = skelType;
	return bridgeEnemies;
}

bool NetherBridgeFeature::isFeatureChunk(BiomeSource* biomeSource, Random& random, const ChunkPos& chunkPos) {

	//TODO refactor and use a RWLock
	LockGuard l(positionMutex);

	int cx = chunkPos.x >> 4;
	int cz = chunkPos.z >> 4;

	random.setSeed(cx ^ (cz << 4) ^ biomeSource->getSeed());
	random.nextInt();

	if (random.nextInt(3) != 0) {
		return false;
	}
	if (chunkPos.x != ((cx << 4) + 4 + random.nextInt(8))) {
		return false;
	}
	if (chunkPos.z != ((cz << 4) + 4 + random.nextInt(8))) {
		return false;
	}
	return true;
}

void NetherBridgeFeature::clearCachedBuildings() {
	cachedStructures.clear();
}

Unique<StructureStart> NetherBridgeFeature::createStructureStart(Dimension& generator, Random& random, const ChunkPos& cp) {
	return make_unique<NetherBridgeStart>(generator, random, cp.x, cp.z);
}

NetherBridgeStart::NetherBridgeStart() :
	StructureStart(0, 0){
}

NetherBridgeStart::NetherBridgeStart(Dimension& biomeSource, Random& random, int chunkX, int chunkZ) :
	StructureStart(chunkX, chunkZ){
	pieces.push_back(make_unique<NBStartPiece>(random, (chunkX << 4) + 2, (chunkZ << 4) + 2));

	auto start = (NBStartPiece*)pieces.back().get();
	start->addChildren(start, pieces, random);

	ReferencedPieceList& pendingChildren = start->pendingChildren;

	while (!pendingChildren.empty()) {
		int pos = random.nextInt(pendingChildren.size());
		StructurePiece* structurePiece = pendingChildren[pos];
		pendingChildren.erase(pendingChildren.begin() + pos);
		structurePiece->addChildren(start, pieces, random);
	}

	calculateBoundingBox();
	moveInsideHeights(&random, 48, 70);
}
