#include "Dungeons.h"
#include "world/level/biome/MutatedBirchForest.h"
#include "world/level/biome/BiomeDecorator.h"

MutatedBirchForest::MutatedBirchForest(int id, Biome* containedBiome)
	: MutatedBiome(id, containedBiome) {

}

const FeaturePtr& MutatedBirchForest::getTreeFeature(Random* random){
	if(random->nextBoolean()) {
		return decorator->superBirchFeature;
	}
	return decorator->birchFeature;
}
