#pragma  once

#include "world/level/levelgen/structure/StructureFeature.h"
#include "world/level/levelgen/structure/StructureStart.h"
#include "world/level/biome/MobSpawnerData.h"

class NetherBridgeFeature : public StructureFeature {

	typedef std::vector<MobSpawnerData> MobList;

public:

	NetherBridgeFeature();

	virtual std::string getFeatureName() override;

	MobList& getBridgeEnemies();

protected:

	virtual bool isFeatureChunk(BiomeSource* biomeSource, Random& random, const ChunkPos& chunkPos) override;

	virtual Unique<StructureStart> createStructureStart(Dimension& generator, Random& random, const ChunkPos& cp) override;

	void clearCachedBuildings();

	std::mutex positionMutex;

private:

	NetherBridgeFeature(const NetherBridgeFeature& copy) {
		UNUSED_PARAMETER(copy);
	}

	MobList bridgeEnemies;
};

class NetherBridgeStart : public StructureStart {

public:

	NetherBridgeStart();
	NetherBridgeStart(Dimension& biomeSource, Random& random, int chunkX, int chunkZ);

	StructureFeatureType getType() const override {
		return StructureFeatureType::Fortress;
	}
};
