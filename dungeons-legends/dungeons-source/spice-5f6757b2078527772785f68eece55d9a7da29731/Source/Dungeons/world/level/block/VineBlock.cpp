/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/VineBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/FoliageColor.h"
#include "world/level/Level.h"
#include "world/level/biome/Biome.h"
#include "world/Direction.h"
#include "world/item/Item.h"
#include "world/entity/player/Player.h"
#include "world/level/material/Material.h"
#include "world/Facing.h"
#include "world/level/LevelConstants.h"

int VineBlock::VINE_SOUTH = VineBlock::_getFacingData(Facing::SOUTH);
int VineBlock::VINE_NORTH = VineBlock::_getFacingData(Facing::NORTH);
int VineBlock::VINE_EAST = VineBlock::_getFacingData(Facing::EAST);
int VineBlock::VINE_WEST = VineBlock::_getFacingData(Facing::WEST);
int VineBlock::VINE_ALL = VineBlock::VINE_SOUTH | VineBlock::VINE_NORTH | VineBlock::VINE_EAST | VineBlock::VINE_WEST;

int VineBlock::_getFacingData(int facing) {
	if (facing == Facing::UP || facing == Facing::DOWN) {
		return 0;
	}

	return 1 << Direction::FACING_DIRECTION[facing];
}

VineBlock::VineBlock(const std::string& nameId, int id) :
	Block(nameId, id, Material::getMaterial(MaterialType::ReplaceablePlant)) {
	setTicking(true);
	setSolid(false);
	mProperties = BlockProperty::BreakOnPush;
	mRenderLayer = RENDERLAYER_ALPHATEST;
	mCanBuildOver = true;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

const AABB& VineBlock::getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping) const {
	const float thickness = 1.0f / 16.0f;
	int data = region.getData(pos);

	float minX = 1;
	float minY = 1;
	float minZ = 1;
	float maxX = 0;
	float maxY = 0;
	float maxZ = 0;

	int vineGrowth = getBlockState(BlockState::VineGrowth).get<int>(data);
	bool bVineEast = (vineGrowth & VINE_EAST) != 0;
	bool bVineWest = (vineGrowth & VINE_WEST) != 0;
	bool bVineSouth = (vineGrowth & VINE_SOUTH) != 0;
	bool bVineNorth = (vineGrowth & VINE_NORTH) != 0;
	bool hasWall = vineGrowth > 0;

	if(bVineWest) {
		maxX = std::max(maxX, thickness);
		minX = 0;
		minY = 0;
		maxY = 1;
		minZ = 0;
		maxZ = 1;
	}

	if(bVineEast) {
		minX = std::min(minX, 1 - thickness);
		maxX = 1;
		minY = 0;
		maxY = 1;
		minZ = 0;
		maxZ = 1;
	}

	if(bVineNorth) {
		maxZ = std::max(maxZ, thickness);
		minZ = 0;
		minX = 0;
		maxX = 1;
		minY = 0;
		maxY = 1;
	}

	if(bVineSouth) {
		minZ = std::min(minZ, 1 - thickness);
		maxZ = 1;
		minX = 0;
		maxX = 1;
		minY = 0;
		maxY = 1;
	}

	BlockPos above = pos.above();
	if(!hasWall && _isAcceptableNeighbor(*mBlocks[region.getBlockID(above.x, above.y, above.z)])) {
		minY = std::min(minY, 1 - thickness);
		maxY = 1;
		minX = 0;
		maxX = 1;
		minZ = 0;
		maxZ = 1;
	}
	bufferAABB.set(minX, minY, minZ, maxX, maxY, maxZ);
	return bufferAABB;
}

const AABB& VineBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	if(isClipping) {
		getVisualShape(region, pos, bufferValue, isClipping);
		return bufferValue.move(Vec3(pos));
	}

	return AABB::EMPTY;
}

bool VineBlock::mayPlace(BlockSource& region, const BlockPos& pos, FacingID face) const {
	switch(face) {
	case Facing::UP:
		return _isAcceptableNeighbor(region.getBlock(pos.above()));
	// Using opposite sides for the blocks below
	case Facing::NORTH:
		return _isAcceptableNeighbor(region.getBlock(pos.south()));
	case Facing::SOUTH:
		return _isAcceptableNeighbor(region.getBlock(pos.north()));
	case Facing::EAST:
		return _isAcceptableNeighbor(region.getBlock(pos.west()));
	case Facing::WEST:
		return _isAcceptableNeighbor(region.getBlock(pos.east()));
	default:
		return false;
	}
}

int VineBlock::getColor(int auxData) const {
	return FoliageColor::getDefaultColor();
}

int VineBlock::getColor(BlockSource& region, const BlockPos& pos, DataID data) const {
	float totalRed = 0;
	float totalGreen = 0;
	float totalBlue = 0;

	//TODO but this is called every frame when rendering, this is slow!
	BlockPos offset;

	for(offset.x = -1; offset.x < 2; ++offset.x) {
		for(offset.z = -1; offset.z < 2; ++offset.z) {
			if(offset.x == 0 && offset.z == 0) {
				continue;
			}

			auto foliageColor = Color::fromARGB(region.getBiome(pos + offset * 4).getFoliageColor());
			totalRed += foliageColor.r;
			totalGreen += foliageColor.g;
			totalBlue += foliageColor.b;
		}
	}

	return Color(totalRed / 8, totalGreen / 8, totalBlue / 8, 1.0f).toARGB();
}

Color VineBlock::getMapColor(BlockSource& region, const BlockPos& pos) const {
	float totalRed = 0;
	float totalGreen = 0;
	float totalBlue = 0;

	//TODO but this is called every frame when rendering, this is slow!
	BlockPos offset;

	for (offset.x = -1; offset.x < 2; ++offset.x) {
		for (offset.z = -1; offset.z < 2; ++offset.z) {
			if (offset.x == 0 && offset.z == 0) {
				continue;
			}

			auto foliageColor = Color::fromARGB(region.getBiome(pos + offset * 4).getMapFoliageColor());
			totalRed += foliageColor.r;
			totalGreen += foliageColor.g;
			totalBlue += foliageColor.b;
		}
	}

	return Color(totalRed / 8, totalGreen / 8, totalBlue / 8, 1.0f) * Color::fromARGB(0xaaaaaa);
}

void VineBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	if (!region.getLevel().isClientSide() && !updateSurvival(region, pos)) {
// 		region.getLevel().broadcastDimensionEvent(region, LevelEvent::ParticlesDestroyBlock, pos, mID);
		spawnResources(region, pos, region.getData(pos), 0);
		region.removeBlock(pos);
	}
}

void VineBlock::tick(BlockSource& region, const BlockPos& pos, Random& random) const{
	if(region.getLevel().isClientSide()) {
		return;
	}

	if(random.nextInt(4) == 0) {
		int r = 4;
		int max = 5;
		bool noSideSpread = false;

		for(int xx = pos.x - r; xx <= pos.x + r && !noSideSpread; xx++) {
			for(int zz = pos.z - r; zz <= pos.z + r && !noSideSpread; zz++) {
				for(int yy = pos.y - 1; yy <= pos.y + 1 && !noSideSpread; yy++) {
					if(region.getBlockID(xx, yy, zz) == mID && --max <= 0) {
						noSideSpread = true;
						break;
					}
				}
			}
		}

		int currentFacings = region.getData(pos);
		int testFacing = random.nextInt(6);
		int testDirection = Direction::FACING_DIRECTION[testFacing];

		if(testFacing == Facing::UP && pos.y < (region.getMaxHeight() - 1) && region.isEmptyBlock( pos.above() )) {
			if(noSideSpread) {
				return;
			}

			// grow upwards, but only if there is something to cling to
			int spawnFacings = random.nextInt(16) & currentFacings;
			if(spawnFacings > 0) {
				for(auto d : range_incl(3)) {
					if(!_isAcceptableNeighbor(region.getBlock(pos.x + Direction::STEP_X[d], pos.y + 1, pos.z + Direction::STEP_Z[d]))) {
						spawnFacings &= ~(1 << d);
					}
				}

				if(spawnFacings > 0) {
					region.setBlockAndData(pos.x, pos.y + 1, pos.z, mID, spawnFacings, Block::UPDATE_CLIENTS);
				}
			}
		} else if(testFacing >= Facing::NORTH && testFacing <= Facing::EAST && (currentFacings & (1 << testDirection)) == 0) {
			if(noSideSpread) {
				return;
			}

			BlockID edgeBlock = region.getBlockID(pos.x + Direction::STEP_X[testDirection], pos.y, pos.z + Direction::STEP_Z[testDirection]);

			if(edgeBlock == 0 || Block::mBlocks[edgeBlock] == nullptr) {
				// if the edge block is air, we could possibly cling to something
				int left = (testDirection + 1) & 3;
				int right = (testDirection + 3) & 3;

				// attempt to grow straight onto solid blocks
				if((currentFacings & (1 << left)) != 0
					&& _isAcceptableNeighbor(region.getBlock(pos.x + Direction::STEP_X[testDirection] + Direction::STEP_X[left], pos.y, pos.z + Direction::STEP_Z[testDirection] + Direction::STEP_Z[left]))) {
					region.setBlockAndData(pos.x + Direction::STEP_X[testDirection], pos.y, pos.z + Direction::STEP_Z[testDirection], mID, 1 << left, Block::UPDATE_CLIENTS);
				} else if((currentFacings & (1 << right)) != 0
					&& _isAcceptableNeighbor(region.getBlock(pos.x + Direction::STEP_X[testDirection] + Direction::STEP_X[right], pos.y, pos.z + Direction::STEP_Z[testDirection] + Direction::STEP_Z[right]))) {
					region.setBlockAndData(pos.x + Direction::STEP_X[testDirection], pos.y, pos.z + Direction::STEP_Z[testDirection], mID, 1 << right, Block::UPDATE_CLIENTS);
				}
				// attempt to grow around corners, but only if the base block is solid
				else if((currentFacings & (1 << left)) != 0
					&& region.isEmptyBlock(pos.x + Direction::STEP_X[testDirection] + Direction::STEP_X[left], pos.y, pos.z + Direction::STEP_Z[testDirection] + Direction::STEP_Z[left])
					&& _isAcceptableNeighbor(region.getBlock(pos.x + Direction::STEP_X[left], pos.y, pos.z + Direction::STEP_Z[left]))) {
					region.setBlockAndData(pos.x + Direction::STEP_X[testDirection] + Direction::STEP_X[left], pos.y, pos.z + Direction::STEP_Z[testDirection] + Direction::STEP_Z[left], mID,
						1 << ((testDirection + 2) & 3), Block::UPDATE_CLIENTS);
				}
				else if((currentFacings & (1 << right)) != 0
					&& region.isEmptyBlock(pos.x + Direction::STEP_X[testDirection] + Direction::STEP_X[right], pos.y, pos.z + Direction::STEP_Z[testDirection] + Direction::STEP_Z[right])
					&& _isAcceptableNeighbor(region.getBlock(pos.x + Direction::STEP_X[right], pos.y, pos.z + Direction::STEP_Z[right]))) {
					region.setBlockAndData(pos.x + Direction::STEP_X[testDirection] + Direction::STEP_X[right], pos.y, pos.z + Direction::STEP_Z[testDirection] + Direction::STEP_Z[right], mID,
						1 << ((testDirection + 2) & 3), Block::UPDATE_CLIENTS);
				}
				// attempt to grow onto the ceiling
				else if(_isAcceptableNeighbor(region.getBlock(pos.x + Direction::STEP_X[testDirection], pos.y + 1, pos.z + Direction::STEP_Z[testDirection]))) {
					region.setBlockAndData(pos.x + Direction::STEP_X[testDirection], pos.y, pos.z + Direction::STEP_Z[testDirection], mID, 0, Block::UPDATE_CLIENTS);
				}

			} else if(Block::mBlocks[edgeBlock]->getMaterial().isSolidBlocking() && Block::mSolid[edgeBlock]) {
				// we have a wall that we can cling to
				region.setBlockAndData(pos.x, pos.y, pos.z, mID, currentFacings | (1 << testDirection), Block::UPDATE_CLIENTS);
			}
		} else if(pos.y > 1) {
			// growing downwards happens more often than the other directions
			auto below = region.getBlockAndData( pos.below() );
			// grow downwards into air
			if(below.id == 0) {
				int spawnFacings = random.nextInt(16) & currentFacings;
				if(spawnFacings > 0) {
					region.setBlockAndData(pos.x, pos.y - 1, pos.z, mID, spawnFacings, Block::UPDATE_CLIENTS);
				}
			} else if(below.id == mID) {
				int spawnFacings = random.nextInt(16) & currentFacings;
				if(below.data != (below.data | spawnFacings)) {
					region.setBlockAndData(pos.x, pos.y - 1, pos.z, mID, below.data | spawnFacings, Block::UPDATE_CLIENTS);
				}
			}
		}
	}
}

int VineBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return 0;
}

int VineBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 0;
}

void VineBlock::playerDestroy(Player* player, const BlockPos& pos, int data) const {
	if(!player->getLevel().isClientSide() && player->getSelectedItem() != nullptr && player->getSelectedItem()->getItem()->getId() == Item::mShears->getId()) {
		//player->awardStat(Stats:::blockMined[id], 1);

		// drop leaf block instead of sapling
		popResource(player->getRegion(), pos, ItemInstance(Block::mVine, 1, 0));
	} else {
		Block::playerDestroy(player, pos, data);
	}
}

bool VineBlock::_isAcceptableNeighbor(const Block& block) const {
	if(block.isType(Block::mAir)) {
		return false;
	}

	if ((block.hasProperty(BlockProperty::CubeShaped) || block.hasProperty(BlockProperty::SolidBlocking))
		&& block.getMaterial().getBlocksMotion() && !block.isType(Block::mInvisibleBedrock)) {
		return true;
	}

	return false;
}

int VineBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	
	DataID data = 0;
	int facing = 0;

	switch(face) {
	case Facing::NORTH:
		facing = VINE_SOUTH;
		break;
	case Facing::SOUTH:
		facing = VINE_NORTH;
		break;
	case Facing::WEST:
		facing = VINE_EAST;
		break;
	case Facing::EAST:
		facing = VINE_WEST;
		break;
	}

	getBlockState(BlockState::VineGrowth).set(data, facing);

	if(data != 0) {
		return data;
	}

	return itemValue;
}

bool VineBlock::updateSurvival(BlockSource& region, const BlockPos& pos) const {
	const int facings = region.getData(pos);
	int newFacings = facings;
	const BlockPos above = pos.above();
	FullBlock aboveBlock = region.getBlockAndData(above);

	if(newFacings > 0) {
		for(auto& facing : Facing::Plane::HORIZONTAL) {
			const int facingData = _getFacingData(facing);

			if((facings & facingData) != 0) {
				if(!_isAcceptableNeighbor(*Block::mBlocks[region.getBlockID(pos.relative(facing))])) {
					// no attachment in this direction, verify that there is vines hanging above
					if(aboveBlock.id != mID || (aboveBlock.data & facingData) == 0) {
						newFacings &= ~facingData;
					}
				}
			}
		}
	}

	if(newFacings == 0) {
		// the block will die unless it has a roof
		if(!_isAcceptableNeighbor(*Block::mBlocks[aboveBlock.id])) {
			return false;
		}
	}

	if(newFacings != facings) {
		region.setBlockAndData(pos, mID, newFacings, Block::UPDATE_CLIENTS);
	}

	return true;
}
