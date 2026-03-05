/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/

#include "Dungeons.h"

#include "world/level/block/ChorusFlowerBlock.h"
#include "world/level/Level.h"
#include "world/level/chunk/LevelChunk.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/level/block/LevelEvent.h"
#include "world/Facing.h"                        // Directions for checking neighbors
#include "world/level/material/Material.h"
#include "world/entity/player/Player.h"

// Current age for the chorus flower is stored in the aux data
const DataID ChorusFlowerBlock::DEAD_AGE = DataID(5);
const int ChorusFlowerBlock::GROW_RATE = 1;         // Smaller value is faster (min 1)
const int ChorusFlowerBlock::BRANCH_DIRECTIONS = 4; // All 4 horizontal directions can be branched to (max 4, min 1)

ChorusFlowerBlock::ChorusFlowerBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Wood)) { // This flower is WOOD m8

	setTicking(true);
	setPushesOutItems(true);

	mProperties = mProperties | BlockProperty::BreakOnPush;
}

int ChorusFlowerBlock::getVariant(int data) const {
	// The flower looks the same for all stages except for the dead stage
	return (data == DEAD_AGE) ? 1 : 0;
}

void ChorusFlowerBlock::tick(BlockSource& region, const BlockPos& pos, Random& random) const {
	// U ded 
	if (!canSurvive(region, pos)) {
		//region.getLevel().destroyBlock(region, pos, false);
		return;
	}

	// Is this a lovely growing plant?
	BlockPos above = pos.above();
	if (!region.isEmptyBlock(above) || above.y >= region.getMaxHeight()) {
		// No more growing, no room left
		return;
	}

	int age = region.getData(pos);
	if (age >= DEAD_AGE || random.nextInt(GROW_RATE) != 0) {
		return;
	}

	bool replaceAndGrow = false;
	bool pillarOnEndStone = false;

	Block belowBlock = region.getBlock(pos.below());
	if (belowBlock.canGrowChorus() || region.isEmptyBlock(pos.below())) {
		// I'm so sorry for these type checks, they are everywhere
		if (belowBlock.getId() == Block::mChorusPlantBlock->getId()) {
			int height = 1;
			// Test for endstone below
			for (int i = 0; i < 4; ++i) {
				Block testBlock = region.getBlock(pos.below(height + 1));
				if (testBlock.canGrowChorus()) {
					if (testBlock.getId() == Block::mChorusPlantBlock->getId()) {
						++height;
					}
					// The only other option is endstone
					else {
						pillarOnEndStone = true;
						break;
					}
				}
			}

			int heightTestValue = 4;
			if (pillarOnEndStone) {
				++heightTestValue;
			}
			if (height < 2 || random.nextInt(heightTestValue) >= height) {
				replaceAndGrow = true;
			}

		}
		// The only other options are endstone or air
		else {
			replaceAndGrow = true;
		}
	}

	// Check for flower placement
	if (replaceAndGrow && _allNeighborsEmpty(region, above, Facing::Name::NOT_DEFINED)) {
		region.setBlock(pos, Block::mChorusPlantBlock->getId(), UPDATE_ALL);
		_placeGrownFlower(region, above, age);
	}
	else if (age <= DEAD_AGE) {
		int numBranchAttempts = random.nextInt(4);
		bool createdBranch = false;
		if (pillarOnEndStone) {
			++numBranchAttempts;
		}
		for (int i = 0; i < numBranchAttempts; ++i) {
			FacingID direction = Facing::Plane::HORIZONTAL[random.nextInt(BRANCH_DIRECTIONS)];
			BlockPos target = pos.relative(direction);
			if (region.isEmptyBlock(target) && region.isEmptyBlock(target.below()) && _allNeighborsEmpty(region, target, Facing::OPPOSITE_FACING[direction])) {
				_placeGrownFlower(region, target, age + 1);
				createdBranch = true;
			}
		}
		if (createdBranch) {
			// Place down a plant branch
			region.setBlock(pos, Block::mChorusPlantBlock->getId(), UPDATE_ALL);
		}
		else {
			_placeDeadFlower(region, pos);
		}
	}
	else if (age == DEAD_AGE - 1) {
		_placeDeadFlower(region, pos);
	}
}

void ChorusFlowerBlock::_placeGrownFlower(BlockSource& region, const BlockPos& pos, DataID newAge) const {
	//DataID age = region.getData(pos);
	region.setBlockAndData(pos, {getId(), newAge}, UPDATE_ALL);
	//region.getLevel().broadcastLevelEvent(LevelEvent::SoundGhastFireball, pos, 0); // TODO [kanuge/Venvious]: Get Chorus SFX
}

void ChorusFlowerBlock::_placeDeadFlower(BlockSource& region, const BlockPos& pos) const {
	region.setBlockAndData(pos, {getId(), DEAD_AGE}, UPDATE_ALL); // RIP
	//region.getLevel().broadcastLevelEvent(LevelEvent::SoundGhastFireball, pos, 0); // TODO [kanuge/Venvious]: Get Chorus SFX
}

bool ChorusFlowerBlock::_allNeighborsEmpty(BlockSource& region, const BlockPos& pos, FacingID ignore) {
	for (FacingID& direction : Facing::Plane::HORIZONTAL) {
		if (direction != ignore && !region.isEmptyBlock(pos.relative(direction))) {
			return false;
		}
	}

	return true;
}

bool ChorusFlowerBlock::mayPlace(BlockSource& region, const BlockPos& pos) const {
	return Block::mayPlace(region, pos) && canSurvive(region, pos);
}

bool ChorusFlowerBlock::isWaterBlocking() const {
	return false;
}

int ChorusFlowerBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 0;
}

void ChorusFlowerBlock::playerDestroy(Player* player, const BlockPos& pos, int data) const {
	//auto& region = player->getRegion();
	//if (!region.getLevel().isClientSide()) {
	//	// drop chorus flower block
	//	popResource(region, pos, ItemInstance(mID, 1, 0));
	//}
}

void ChorusFlowerBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	// Only tick the after a neighbor has changed and cannot survive
	if (!canSurvive(region, pos)) {
		auto chunk = region.getChunk(pos);
		if (chunk) {
			//chunk->getTickQueue().add(region, pos, getId(), 1);
		}
	}
}

bool ChorusFlowerBlock::canSurvive(BlockSource& region, const BlockPos& pos) const {
	Block below = region.getBlock(pos.below());
	if (below.canGrowChorus()) {
		return true;
	}
	if (region.isEmptyBlock(pos.below())) {
		int neighbors = 0;
		for (auto& direction : Facing::Plane::HORIZONTAL) {
			Block neighbor = region.getBlock(pos.relative(direction));
			if (neighbor.getId() == Block::mChorusPlantBlock->getId()) {
				++neighbors;
			}
			// If the neighbor is not air or a chorus plant, destroy the flower
			else if (!region.isEmptyBlock(pos.relative(direction))) {
				return false;
			}
		}
		return neighbors == 1;
	}

	return false;
}

bool ChorusFlowerBlock::canProvideSupport(BlockSource& region, const BlockPos& pos, FacingID face, BlockSupportType type) const {
	return false;
}

//////////////////////////////////////////////////////////////////////////
// Functionality for instantly generating a random Chorus Plant
//////////////////////////////////////////////////////////////////////////

void ChorusFlowerBlock::generatePlant(BlockSource& region, const BlockPos& target, Random& random, int maxHorizontalSpread) {
	region.setBlock(target, Block::mChorusPlantBlock->getId(), UPDATE_ALL);
	_growTreeRecursive(region, target, target, random, maxHorizontalSpread, 0);
}

void ChorusFlowerBlock::_growTreeRecursive(BlockSource& region, const BlockPos& current, const BlockPos& startPos, Random& random, int maxHorizontalSpread, int depth) {
	int height = random.nextInt(4) + 1;
	if (depth == 0) {
		++height;
	}

	// Grow the plant straight up to a random height
	for (int i = 0; i < height; ++i) {
		BlockPos target = current.above(i + 1);
		if (!_allNeighborsEmpty(region, target, Facing::Name::NOT_DEFINED)) {
			return;
		}

		region.setBlock(target, Block::mChorusPlantBlock->getId(), UPDATE_ALL);
	}

	// Decide to branch or not to branch
	bool placedStem = false;
	if (depth < 4) {
		int stems = random.nextInt(4);
		if (depth == 0) {
			++stems;
		}

		for (int i = 0; i < stems; ++i) {
			FacingID direction = Facing::Plane::HORIZONTAL[random.nextInt(BRANCH_DIRECTIONS)];
			BlockPos target = current.above(height).relative(direction);
			if (abs(target.x - startPos.x) >= maxHorizontalSpread || abs(target.z - startPos.z) >= maxHorizontalSpread) {
				continue;
			}
			// Recurse and grow more of the plant if possible
			if (region.isEmptyBlock(target) && region.isEmptyBlock(target.below()) && _allNeighborsEmpty(region, target, Facing::OPPOSITE_FACING[direction])) {
				placedStem = true;
				region.setBlock(target, Block::mChorusPlantBlock->getId(), UPDATE_ALL);
				_growTreeRecursive(region, target, startPos, random, maxHorizontalSpread, depth + 1);
			}
		}
	}

	// End of generated plant reached, place a grown flower
	if (!placedStem) {
		region.setBlockAndData(current.above(height), {Block::mChorusFlowerBlock->getId(), DEAD_AGE}, UPDATE_ALL);
	}
}
