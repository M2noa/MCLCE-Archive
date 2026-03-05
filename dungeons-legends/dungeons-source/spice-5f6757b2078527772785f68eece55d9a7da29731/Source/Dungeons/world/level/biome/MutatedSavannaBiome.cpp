#include "Dungeons.h"
#include "world/level/biome/MutatedSavannaBiome.h"
#include "world/level/biome/BiomeDecorator.h"
#include "world/level/block/DirtBlock.h"
#include "world/level/block/BlockVolume.h"

MutatedSavannaBiome::MutatedSavannaBiome(int id, Biome* contained) :
	MutatedBiome(id, contained) {
	decorator->treeCount = 2;
	decorator->flowerCount = 2;
	decorator->grassCount = 5;
}

void MutatedSavannaBiome::buildSurfaceAt(Random& random, BlockVolume& blocks, const BlockPos& pos, float depthValue, Height seaLevel) {
	mTopMaterial = Block::mGrass->mID;
	mTopMaterialData = 0;
	mMaterial = Block::mDirt->mID;
	if(depthValue > 1.75) {
		mTopMaterial = Block::mStone->mID;
		mMaterial = Block::mStone->mID;
	}
	else if(depthValue > -.5) {
		mTopMaterial = Block::mDirt->mID;
		Block::mDirt->getBlockState(BlockState::MappedType).set(mTopMaterialData, DirtBlock::TYPE_NO_GRASS_SPREAD);
	}
	buildSurfaceAtDefault(random, blocks, pos, depthValue, seaLevel);	// skip super
}
