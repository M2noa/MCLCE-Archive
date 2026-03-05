#include "Dungeons.h"
#include "world/level/biome/BeachBiome.h"
#include "world/level/block/Block.h"
#include "world/level/biome/BiomeDecorator.h"

BeachBiome::BeachBiome(int id) :
	Biome(id, Biome::BiomeType::Beach) {
	// remove default mob spawn settings
	_friendlies.clear();
	mTopMaterial = Block::mSand->mID;
	mMaterial = Block::mSand->mID;

	decorator->treeCount = -999;
	decorator->deadBushCount = 0;
	decorator->reedsCount = 0;
	decorator->cactusCount = 0;
}
