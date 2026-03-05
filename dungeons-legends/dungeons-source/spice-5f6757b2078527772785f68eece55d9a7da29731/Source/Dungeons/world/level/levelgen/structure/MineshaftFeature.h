#pragma once

#include "world/level/levelgen/structure/StructureFeature.h"
#include "world/level/levelgen/structure/StructureStart.h"

class Dimension;

class MineshaftFeature : public StructureFeature {

public:

	MineshaftFeature() :
		StructureFeature() {
	}

	virtual ~MineshaftFeature() {
	}

	std::string getFeatureName() override {
		return "Mineshaft";
	}

	bool isFeatureChunk(BiomeSource* biomeSource, Random& random, const ChunkPos& lc) override;

	bool isSurfaceChunk(const ChunkPos& pos);

protected:

	Unique<StructureStart> createStructureStart(Dimension& biomeSource, Random& random, const ChunkPos& lc) override;

};

class MineshaftStart : public StructureStart {

public:

	MineshaftStart()
		: StructureStart(0, 0){
	}

	MineshaftStart(Dimension& source, Random& random, const ChunkPos& pos);

	void _moveToSurface(Height surface);

	virtual ~MineshaftStart() {
	}

	StructureFeatureType getType() const override {
		return StructureFeatureType::Mineshaft;
	}

private:

};
