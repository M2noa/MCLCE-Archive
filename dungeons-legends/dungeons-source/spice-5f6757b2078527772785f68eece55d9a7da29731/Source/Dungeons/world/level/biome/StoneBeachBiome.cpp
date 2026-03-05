#include "Dungeons.h"
#include "world/level/biome/StoneBeachBiome.h"
#include "world/level/block/Block.h"
#include "world/level/biome/BiomeDecorator.h"

StoneBeachBiome::StoneBeachBiome(int id) :
	Biome(id, Biome::BiomeType::StoneBeach) {
	// remove default mob spawn settings
	_friendlies.clear();
	mTopMaterial = Block::mStone->mID;
	mMaterial = Block::mStone->mID;

	decorator->treeCount = -999;
	decorator->deadBushCount = 0;
	decorator->reedsCount = 0;
	decorator->cactusCount = 0;
}
