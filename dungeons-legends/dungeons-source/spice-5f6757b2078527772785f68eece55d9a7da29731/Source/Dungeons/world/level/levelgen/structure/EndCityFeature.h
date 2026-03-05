/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "world/level/levelgen/structure/StructureFeature.h"
#include "world/level/levelgen/structure/StructureStart.h"

class TheEndRandomLevelSource;

class EndCityFeature : public StructureFeature {
public:
	EndCityFeature(TheEndRandomLevelSource& levelSource);
	
	std::string getFeatureName() override {
		return "EndCity";
	}

protected:
	bool isFeatureChunk(BiomeSource* biomeSource, Random& random, const ChunkPos& pos) override;
	Unique<StructureStart> createStructureStart(Dimension& dimension, Random& random, const ChunkPos& pos) override;

private:
	TheEndRandomLevelSource& mLevelSource;
};

class EndCityStart : public StructureStart {
public:
	EndCityStart();
	EndCityStart(Dimension& dimension, TheEndRandomLevelSource& levelSource, Random& random, const ChunkPos& pos);
	virtual ~EndCityStart() {
	}

	bool isValid() const override {
		return mIsValid;
	}

	StructureFeatureType getType() const override {
		return StructureFeatureType::EndCity;
	}

private:
	void _create(Dimension& dimension, TheEndRandomLevelSource& levelSource, Random& random, const ChunkPos& pos);

	bool mIsValid;
};
