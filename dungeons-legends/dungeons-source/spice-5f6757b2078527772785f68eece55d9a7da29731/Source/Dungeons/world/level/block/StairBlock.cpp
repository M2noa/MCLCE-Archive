/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/dimension/Dimension.h"
#include "world/phys/HitResult.h"
#include "world/Facing.h"
#include "world/level/block/StairBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/entity/Mob.h"
//#include "world/redstone/circuit/CircuitSystem.h"
//#include "world/redstone/circuit/components/PoweredBlockComponent.h"
#include "world/level/material/Material.h"

const int StairBlock::DEAD_SPACES[8][2] = {
	{2, 6}, {3, 7}, {2, 3}, {6, 7},
	{0, 4}, {1, 5}, {0, 1}, {4, 5}
};

int StairBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	bool bUpsideDown = isUpsideDown((DataID)itemValue);

	DataID data = 0;

	if (face == Facing::DOWN || (face != Facing::UP && clickPos.y > 0.5f)) {
		bUpsideDown = true;
	}
	getBlockState(BlockState::UpsideDownBit).set(data, bUpsideDown);

	DataID dir = (Math::floor(by.getRotation().y * 4 / (360) + 0.5f)) & 3;
	switch (dir) {
	case 0:
		getBlockState(BlockState::Direction).set(data, 2);
		break;
	case 1:
		getBlockState(BlockState::Direction).set(data, 1);
		break;
	case 2:
		getBlockState(BlockState::Direction).set(data, 3);
		break;
	case 3:
		getBlockState(BlockState::Direction).set(data, 0);
		break;
	default:
		getBlockState(BlockState::Direction).set(data, dir);
	}

	return data;
}

StairBlock::StairBlock(const std::string& nameId, int id, const Block& base, int baseData)
	: Block(nameId, id, base.getMaterial())
	, mBase(base)
#if 0 //[IMPLEMENTATION]: Unused field, only initialized
	, mBaseData(baseData)
#endif
{	// when needed in the future, set this to necessary texture data
	setDestroyTime(base.getDestroySpeed());
	setExplodeable(base.getExplosionResistance() / 3);
	setLightBlock(Brightness::MAX);

	setSolid(false);
	setPushesOutItems(true);

	mRenderLayer = base.getRenderLayer();
	mProperties = BlockProperty::Stair;

	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

HitResult StairBlock::clip(BlockSource& region, const BlockPos& pos, const Vec3& A, const Vec3& B, bool isClipping, int clipData) const {
	HitResult results[8];

	int data = region.getData(pos);
	int dir = getDirection(data);
	bool upsideDown = isUpsideDown(data);
	const int* deadSpaces = (const int*)&DEAD_SPACES[dir + (upsideDown ? 4 : 0)];

	for (int i = 0; i < 8; i++) {
		clipData = i;
		results[i] = Block::clip(region, pos, A, B, true, clipData);
	}

	results[deadSpaces[0]] = HitResult(B);
	results[deadSpaces[1]] = HitResult(B);

	HitResult* closest = nullptr;
	float closestDist = 0;

	for (int r = 0; r < 8; r++) {
		HitResult& result = results[r];
		if (result.isHit()) {
			const float dist = result.getPos().distanceToSqr(B);

			if (dist > closestDist) {
				closest = &result;
				closestDist = dist;
			}
		}
	}

	if (closest == nullptr) {
		return HitResult(B);
	}

	return *closest;
}

const AABB& StairBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	if(isClipping) {
		bufferValue.set(0.5f * (clipData % 2), 0.5f * (clipData / 2 % 2), 0.5f * (clipData / 4 % 2), 0.5f + 0.5f * (clipData % 2), 0.5f + 0.5f * (clipData / 2 % 2), 0.5f + 0.5f * (clipData / 4 % 2));
	} else {
		bufferValue.set(0, 0, 0, 1, 1, 1);
	}

	return bufferValue.move(Vec3(pos));
}

void StairBlock::setBaseShape(BlockSource& region, const BlockPos& pos, AABB& shape, bool shrink) const {
	int data = region.getData(pos);

	if (isUpsideDown(data)) {
		shape.set(0, .5f, 0, 1, 1, 1);
	} else {
		shape.set(0, 0, 0, 1, .5f, 1);
	}

	if (shrink) {
		shapeZFightShrink(shape);
	}
}

bool StairBlock::isStairs(int id) {
	return id > 0 && Block::mBlocks[id]->isStairBlock();
}

bool StairBlock::isStairBlock() const {
	return true;
}

bool StairBlock::setStepShape(BlockSource& region, const BlockPos& pos, AABB& shape, bool shrink) const {
	DataID data = region.getData(pos);
	int dir = getDirection(data);

	float bottom = 0.5f;
	float top = 1.0f;

	bool bUpsideDown = isUpsideDown(data);

	if (bUpsideDown) {
		bottom = 0;
		top = .5f;
	}

	float west = 0;
	float east = 1;
	float north = 0;
	float south = .5f;

	bool checkInnerPiece = true;
	FullBlock back;

	if (dir == DIR_EAST) {
		west = .5f;
		south = 1;

		back = region.getBlockAndData(pos.east());

		if (isStairs(back.id) && (bUpsideDown == isUpsideDown(back.data))) {
			int backDir = getDirection(back.data);
			if (backDir == DIR_NORTH && !isLockAttached(region, pos.south(), data)) {
				south = .5f;
				checkInnerPiece = false;
			}
			else if (backDir == DIR_SOUTH && !isLockAttached(region, pos.north(), data)) {
				north = .5f;
				checkInnerPiece = false;
			}
		}
	} else if (dir == DIR_WEST) {
		east = .5f;
		south = 1;

		back = region.getBlockAndData(pos.west());
		if (isStairs(back.id) && (bUpsideDown == isUpsideDown(back.data))) {
			int backDir = getDirection(back.data);
			if (backDir == DIR_NORTH && !isLockAttached(region, pos.south(), data)) {
				south = .5f;
				checkInnerPiece = false;
			}
			else if (backDir == DIR_SOUTH && !isLockAttached(region, pos.north(), data)) {
				north = .5f;
				checkInnerPiece = false;
			}
		}
	} else if (dir == DIR_SOUTH) {
		north = .5f;
		south = 1;

		back = region.getBlockAndData(pos.south());
		if (isStairs(back.id) && (bUpsideDown == isUpsideDown(back.data))) {
			int backDir = getDirection(back.data);
			if (backDir == DIR_WEST && !isLockAttached(region, pos.east(), data)) {
				east = .5f;
				checkInnerPiece = false;
			}
			else if (backDir == DIR_EAST && !isLockAttached(region, pos.west(), data)) {
				west = .5f;
				checkInnerPiece = false;
			}
		}
	} else {

		back = region.getBlockAndData(pos.north());
		if (isStairs(back.id) && (bUpsideDown == isUpsideDown(back.data))) {
			int backDir = getDirection(back.data);
			if (backDir == DIR_WEST && !isLockAttached(region, pos.east(), data)) {
				east = .5f;
				checkInnerPiece = false;
			}
			else if (backDir == DIR_EAST && !isLockAttached(region, pos.west(), data)) {
				west = .5f;
				checkInnerPiece = false;
			}
		}
	}

	shape.set(west, bottom, north, east, top, south);

	if (shrink) {
		shapeZFightShrink(shape);
	}

	return checkInnerPiece;
}

/*
 * This method adds an extra 1/8 block if the stairs can attach as an
 * "inner corner."
 */
bool StairBlock::setInnerPieceShape(BlockSource& region, const BlockPos& pos, AABB& shape, bool shrink) const {
	DataID data = region.getData(pos);
	int dir = getDirection(data);

	float bottom = 0.5f;
	float top = 1.0f;

	bool bUpsideDown = isUpsideDown(data);

	if (bUpsideDown) {
		bottom = 0;
		top = .5f;
	}

	float west = 0;
	float east = .5f;
	float north = .5f;
	float south = 1.0f;
	FullBlock front;
	bool hasInnerPiece = false;

	if (dir == DIR_EAST) {
		front = region.getBlockAndData(pos.west());
		if (isStairs(front.id) && (bUpsideDown == isUpsideDown(front.data))) {
			int frontDir = getDirection(front.data);
			if (frontDir == DIR_NORTH && !isLockAttached(region, pos.north(), data)) {
				north = 0;
				south = .5f;
				hasInnerPiece = true;
			}
			else if (frontDir == DIR_SOUTH && !isLockAttached(region, pos.south(), data)) {
				north = .5f;
				south = 1;
				hasInnerPiece = true;
			}
		}
	} else if (dir == DIR_WEST) {
		front = region.getBlockAndData(pos.east());
		if (isStairs(front.id) && (bUpsideDown == isUpsideDown(front.data))) {
			west = .5f;
			east = 1.0f;
			int frontDir = getDirection(front.data);
			if (frontDir == DIR_NORTH && !isLockAttached(region, pos.north(), data)) {
				north = 0;
				south = .5f;
				hasInnerPiece = true;
			}
			else if (frontDir == DIR_SOUTH && !isLockAttached(region, pos.south(), data)) {
				north = .5f;
				south = 1;
				hasInnerPiece = true;
			}
		}
	} else if (dir == DIR_SOUTH) {
		front = region.getBlockAndData(pos.north());
		if (isStairs(front.id) && (bUpsideDown == isUpsideDown(front.data))) {
			north = 0;
			south = .5f;

			int frontDir = getDirection(front.data);
			if (frontDir == DIR_WEST && !isLockAttached(region, pos.west(), data)) {
				hasInnerPiece = true;
			}
			else if (frontDir == DIR_EAST && !isLockAttached(region, pos.east(), data)) {
				west = .5f;
				east = 1.0f;
				hasInnerPiece = true;
			}
		}
	} else if (dir == DIR_NORTH) {
		front = region.getBlockAndData(pos.south());
		if (isStairs(front.id) && (bUpsideDown == isUpsideDown(front.data))) {
			int frontDir = getDirection(front.data);
			if (frontDir == DIR_WEST && !isLockAttached(region, pos.west(), data)) {
				hasInnerPiece = true;
			}
			else if (frontDir == DIR_EAST && !isLockAttached(region, pos.east(), data)) {
				west = .5f;
				east = 1.0f;
				hasInnerPiece = true;
			}
		}
	}

	if (hasInnerPiece) {
		shape.set(west, bottom, north, east, top, south);
		if (shrink) {
			shapeZFightShrink(shape);
		}
	}

	return hasInnerPiece;
}

bool StairBlock::isLockAttached(BlockSource& region, const BlockPos& pos, int data) const {
	auto lock = region.getBlockAndData(pos);
	return lock.data == data && isStairs(lock.id);
}

void StairBlock::addAABBs(BlockSource& region, const BlockPos& blockPos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes) const {
	Vec3 pos(blockPos);
	AABB shape;
	setBaseShape(region, pos, shape);
	addAABB(shape.move(pos.x, pos.y, pos.z), intersectTestBox, inoutBoxes);

	bool checkInnerPiece = setStepShape(region, pos, shape);
	addAABB(shape.move(pos.x, pos.y, pos.z), intersectTestBox, inoutBoxes);

	if(checkInnerPiece) {
		if(setInnerPieceShape(region, pos, shape)) {
			addAABB(shape.move(pos.x, pos.y, pos.z), intersectTestBox, inoutBoxes);
		}
	}
}

bool StairBlock::addCollisionShapes( BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes, Entity* entity ) const {
	addAABBs(region, pos, intersectTestBox, inoutBoxes);
	return true;
}

void StairBlock::tick(BlockSource& region, const BlockPos& pos, Random& random) const {
	mBase.tick(region, pos, random);

}
void StairBlock::animateTick(BlockSource& region, const BlockPos& pos, Random& random) const {
	mBase.animateTick(region, pos, random);
}

bool StairBlock::attack(Player* player, const BlockPos& pos) const {
	return mBase.attack(player, pos);
}

void StairBlock::destroy(BlockSource& region, const BlockPos& pos, int data, Entity* entitySource) const {
	mBase.destroy(region, pos, data, entitySource);
}

int StairBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return mBase.getResourceCount(random, data, bonusLootLevel);
}

void StairBlock::handleEntityInside(BlockSource& region, const BlockPos& pos, Entity* entity, Vec3& current) const {
	mBase.handleEntityInside(region, pos, entity, current);
}

bool StairBlock::mayPick() const {
	return mBase.mayPick();
}

bool StairBlock::mayPick(BlockSource& region, int data, bool liquid) const {
	return mBase.mayPick(region, data, liquid);
}

bool StairBlock::mayPlace(BlockSource& region, const BlockPos& pos, FacingID face) const {
	return mBase.mayPlace(region, pos);
}

void StairBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	neighborChanged(region, pos, pos);
	mBase.onPlace(region, pos);
	 
}

BlockProperty StairBlock::getRedstoneProperty(BlockSource& region, const BlockPos& pos) const {
	if (region.getData(pos) & UPSIDEDOWN_BIT) {
		return BlockProperty::Power_BlockDown;
	}
	return BlockProperty::Unspecified;
}

 

void StairBlock::onRemove(BlockSource& region, const BlockPos& pos) const {
	mBase.onRemove(region, pos);
}

void StairBlock::onStepOn(Entity& entity, const BlockPos& pos) const {
	mBase.onStepOn(entity, pos);
}

bool StairBlock::use(Player& player, const BlockPos& pos) const {
	return mBase.use(player, pos);
}

void StairBlock::onExploded(BlockSource& region, const BlockPos& pos, Entity* entitySource) const {
	mBase.onExploded(region, pos, entitySource);
}

bool StairBlock::canBeSilkTouched() const {
	return false;
}

void StairBlock::shapeZFightShrink(AABB& shape) const {
	shape = shape.shrink(Vec3(SIZE_OFFSET));
	shape.expand(Vec3(0.5f));
}

int StairBlock::getVariant(int data) const {
	// No variants at this time, so ignore data
	return 0;
}

int StairBlock::getDirection(const DataID& data) const {
	return getBlockState(BlockState::Direction).get<int>(data);
}

bool StairBlock::isUpsideDown(const DataID& data) const {
	return getBlockState(BlockState::UpsideDownBit).getBool(data);
}
