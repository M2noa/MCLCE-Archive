/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/EndRodBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
// #include "world/entity/ParticleType.h"
#include "LevelEvent.h"
#include "world/Facing.h"
#include "world/entity/Mob.h"

EndRodBlock::EndRodBlock(const std::string& nameId, int id) :
	Block(nameId, id, Material::getMaterial(MaterialType::Decoration)) {

	float ss = 0.2f;
	setVisualShape( Vec3(0.5f - ss, 0, 0.5f - ss), Vec3(0.5f + ss, ss * 3, 0.5f + ss));
	setSolid(false);
	mProperties = BlockProperty::Piston;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

bool EndRodBlock::mayPlace(BlockSource& region, const BlockPos& pos) const {
	return true;
}

bool EndRodBlock::mayPlace(BlockSource& region, const BlockPos& pos, FacingID facing) const {
	return true;
}

int EndRodBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	int dir = ROD_TOP;
	BlockSource& region = by.getRegion();

	auto downBlock = region.getBlockAndData(pos.below());
	auto upBlock = region.getBlockAndData(pos.above());
	auto northBlock = region.getBlockAndData(pos.north());
	auto southBlock = region.getBlockAndData(pos.south());
	auto westBlock = region.getBlockAndData(pos.west());
	auto eastBlock = region.getBlockAndData(pos.east());

	if (face == Facing::UP) {
		if (_shouldMirror(downBlock.getBlock(), downBlock.data, face)) {
			dir = _mirror(downBlock.data);
		}
	}

	if (face == Facing::DOWN) {
		if (_shouldMirror(upBlock.getBlock(), upBlock.data, face)) {
			dir = _mirror(upBlock.data);
		}
		else {
			dir = ROD_DOWN;
		}
	}

	if (face == Facing::NORTH) {
		if (_shouldMirror(southBlock.getBlock(), southBlock.data, face)) {
			dir = _mirror(southBlock.data);
		}
		else {
			dir = ROD_SOUTH;
		}
	}

	if (face == Facing::SOUTH ) {
		if (_shouldMirror(northBlock.getBlock(), northBlock.data, face)) {
			dir = _mirror(northBlock.data);
		}
		else {
			dir = ROD_NORTH;
		}
	}

	if (face == Facing::WEST) {
		if (_shouldMirror(eastBlock.getBlock(), eastBlock.data, face)) {
			dir = _mirror(eastBlock.data);
		}
		else {
			dir = ROD_EAST;
		}
	}

	if (face == Facing::EAST) {
		if (_shouldMirror(westBlock.getBlock(), westBlock.data, face)) {
			dir = _mirror(westBlock.data);
		}
		else {
			dir = ROD_WEST;
		}
	}

	return dir;
}

void EndRodBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	return;
}

void EndRodBlock::animateTick(BlockSource& region, const BlockPos& pos, Random& random) const {
	auto& level = region.getLevel();
	int facing = region.getData(pos) & 7;

	const float x = pos.x + 0.55f - random.nextFloat() * 0.1f;
	const float y = pos.y + 0.55f - random.nextFloat() * 0.1f;
	const float z = pos.z + 0.55f - random.nextFloat() * 0.1f;
	const float r = 0.4f - random.nextGaussianFloat() * 0.4f;

	Vec3 direction(x + Facing::getStepX(facing) * r, y + Facing::getStepY(facing) * r, z + Facing::getStepZ(facing) * r);
	float posx = random.nextGaussian() * 0.005f;
	float posy = random.nextGaussian() * 0.005f;
	float posz = random.nextGaussian() * 0.005f;

// 	if (random.nextInt(5) == 0) {
// 		level.addParticle(ParticleType::EndRod, direction, Vec3(posx, posy, posz));
// 	}
}

bool EndRodBlock::breaksFallingBlocks(int data) const {
	int facing = data & 7;

	switch (facing) {
	case Facing::UP:
	case Facing::DOWN:
		return false;
	default: 
		break;
	}

	return true;
}

bool EndRodBlock::canSurvive(BlockSource& region, const BlockPos& pos) const {
	return mayPlaceOn(region.getBlock(pos.below()));
}

const AABB& EndRodBlock::getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping) const {
	int dir = region.getData(pos) & 7;

	Vec3 first;
	Vec3 second;

	// AABB should have same shape for opposite directions
	_shapeFromDirection(first, second, dir);

	bufferAABB.set(first, second);

	return bufferAABB;
}

bool EndRodBlock::canStandOn(BlockSource& region, const BlockPos& pos) const {
	return true;
}

void EndRodBlock::addAABBs(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes) const {
	int dir = region.getData(pos) & 7;
	AABB tmpAABB;

	Vec3 first;
	Vec3 second;

	// AABB should have same shape for opposite directions
	_shapeFromDirection(first, second, dir);

	tmpAABB.set(first, second);
	addAABB(tmpAABB.move(static_cast<float>(pos.x), static_cast<float>(pos.y), static_cast<float>(pos.z)), intersectTestBox, inoutBoxes);
}

DataID EndRodBlock::_mirror(const DataID theirs) const {

	// Mirrored facings
	static const DataID mirrors[] = {
		ROD_TOP,    // index 0 [DOWN]  = TOP
		ROD_DOWN,   // index 1 [TOP]   = DOWN
		ROD_SOUTH,  // index 2 [NORTH] = SOUTH
		ROD_NORTH,  // index 3 [SOUTH] = NORTH
		ROD_EAST,   // index 4 [WEST]  = EAST
		ROD_WEST    // index 5 [EAST]  = WEST
	};

	return mirrors[theirs];
}

bool EndRodBlock::_shouldMirror(const Block& b, const DataID adjacentFace, const DataID ourFace) const {
	if (b.mID == Block::mEndRod->mID) {
		// Only mirror if we're looking at the top or bottom of another end rod
		if (ourFace == ROD_TOP || ourFace == ROD_DOWN) {
			if (adjacentFace == ROD_TOP || adjacentFace == ROD_DOWN) {
				return true;
			}
		}
		else if (ourFace == ROD_NORTH || ourFace == ROD_SOUTH) {
			if (adjacentFace == ROD_NORTH || adjacentFace == ROD_SOUTH) {
				return true;
			}
		}
		else if (ourFace == ROD_WEST || ourFace == ROD_EAST) {
			if (adjacentFace == ROD_WEST || adjacentFace == ROD_EAST) {
				return true;
			}
		}
		return false;
	}

	return false;
}

void EndRodBlock::_shapeFromDirection(Vec3& first, Vec3& second, const int facing) const {
	static const float zero = 0.0f;
	static const float six = 6.0f / 16.0f;
	static const float ten = 10.0f / 16.0f;
	static const float sixteen = 1.0f;

	switch (facing) {
		case Facing::UP:
		case Facing::DOWN:
			first.set(six, zero, six);
			second.set(ten, sixteen, ten);
			break;

		case Facing::NORTH:
		case Facing::SOUTH:
			first.set(six, six, zero);
			second.set(ten, ten, sixteen);
			break;
		case Facing::WEST:
		case Facing::EAST:
			first.set(zero, six, six);
			second.set(sixteen, ten, ten);
			break;

		default: break;
	}
}
