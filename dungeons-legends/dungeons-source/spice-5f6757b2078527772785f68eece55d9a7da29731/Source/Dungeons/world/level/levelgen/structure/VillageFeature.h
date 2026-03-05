#pragma once

#include "world/level/levelgen/structure/StructureFeature.h"
#include "world/level/levelgen/structure/StructureStart.h"

class VillageFeature : public StructureFeature {

public:

	VillageFeature();

	std::string getFeatureName() override {
		return "Village";
	}

	bool isFeatureChunk(BiomeSource* biomeSource, Random& random, const ChunkPos& cp) override;

protected:

	Unique<StructureStart> createStructureStart(Dimension& generator, Random& random, const ChunkPos& cp) override;

public:

	std::vector<int> allowedBiomes;

};

class VillageStart : public StructureStart {

public:

	VillageStart()
		: StructureStart(0, 0)
		, valid(false){
	}

	VillageStart(BiomeSource* biomeSource, Random& random, int chunkX, int chunkZ, int villageSizeModifier);

	virtual ~VillageStart() {
	}

	bool isValid() const override {
		return valid;
	}

	void addAdditionalSaveData(CompoundTag& tag) override;
	void readAdditionalSaveData(const CompoundTag& tag) override;

	StructureFeatureType getType() const override {
		return StructureFeatureType::Village;
	}

private:

	bool valid;

};
