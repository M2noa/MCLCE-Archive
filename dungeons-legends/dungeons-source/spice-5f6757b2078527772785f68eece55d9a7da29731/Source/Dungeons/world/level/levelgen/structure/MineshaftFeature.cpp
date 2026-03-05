#include "Dungeons.h"

#include "world/level/levelgen/structure/MineshaftFeature.h"
#include "world/level/levelgen/structure/MineshaftPieces.h"
#include "world/level/LevelConstants.h"

#include "world/level/biome/Biome.h"

#include "world/level/chunk/LevelChunk.h"
#include "world/level/block/WoodBlock.h"
#include "world/level/biome/BiomeSource.h"
#include "world/level/dimension/Dimension.h"

bool MineshaftFeature::isFeatureChunk(BiomeSource* biomeSource, Random& random, const ChunkPos& pos){
	float randf = random.nextFloat();
	int randi = random.nextInt(80);
	return randf < 0.004f && randi < std::max(std::abs(pos.x), std::abs(pos.z));
}

Unique<StructureStart> MineshaftFeature::createStructureStart(Dimension& biomeSource, Random& random, const ChunkPos& lc){
	return make_unique<MineshaftStart>(biomeSource, random, lc);
}

bool _isSurfaceMineshaft(BiomeSource& source, const ChunkPos& pos) {
	return source.getBiome(pos)->mBiomeType == Biome::BiomeType::Mesa;
}

MineshaftStart::MineshaftStart(Dimension& source, Random& random, const ChunkPos& pos)
	: StructureStart(pos.x, pos.z){
	bool surfaceMineshaft = _isSurfaceMineshaft(source.getBiomes(), pos);
	MineshaftData data(surfaceMineshaft);

	auto newRoom = make_unique<MineshaftRoom>(data, 0, random, (pos.x << 4) + 2, (pos.z << 4) + 2);
	auto mineShaftRoom = newRoom.get();
	pieces.emplace_back(std::move(newRoom));
	mineShaftRoom->addChildren(mineShaftRoom, pieces, random);

	calculateBoundingBox();

	// move all bounding boxes
	if (surfaceMineshaft) {
		_moveToSurface(source.getSeaLevel());
	}
	else{
		moveToLevel(random, source.getSeaLevel() - 10);
	}
}

void MineshaftStart::_moveToSurface(Height surface) {
	const int offset = -5;
	int dy = surface - boundingBox.y1 + boundingBox.getYSpan() / 2 - offset;	//HACK
	boundingBox.move(0, dy, 0);

	for (unsigned int i = 0; i < pieces.size(); i++) {
		pieces[i]->moveBoundingBox(0, dy, 0);
	}
}

MineshaftData::MineshaftData(bool surface) :

	woodBlock(Block::mWoodPlanks->mID, (DataID)(surface ? WoodBlockType::Big_Oak : WoodBlockType::Oak))
	, fenceBlock(Block::mFence->mID, (DataID)(surface ? WoodBlockType::Big_Oak : WoodBlockType::Oak))
	, roomChance(surface ? 0.5f : 1.f) {

}
