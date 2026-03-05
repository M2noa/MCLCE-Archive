/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/TorchBlock.h"
#include "world/level/material/Material.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/entity/Mob.h"
#include "world/Facing.h"
// #include "world/entity/ParticleType.h"

const DataID TorchBlock::DATA_FROM_FACING[6] = {
	TORCH_TOP, TORCH_TOP, TORCH_SOUTH, TORCH_NORTH, TORCH_EAST, TORCH_WEST
};

const DataID TorchBlock::FACING_FROM_DATA[6] = {
	Facing::UP, Facing::EAST, Facing::WEST, Facing::SOUTH, Facing::NORTH, Facing::UP
};

TorchBlock::TorchBlock(const std::string& nameId, int id) :
	Block(nameId, id, Material::getMaterial(MaterialType::Decoration)) {
	setSolid(false);
	mProperties = BlockProperty::BreakOnPush;
	float r = 0.15f;
	setVisualShape(Vec3(0, 0.2f, 0.5f - r), Vec3(r * 2, 0.8f, 0.5f + r));
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

const AABB& TorchBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	if(isClipping) {
		getVisualShape(region, pos, bufferValue, true);
		return bufferValue.move(Vec3(pos));
	}

	return AABB::EMPTY;
}

int TorchBlock::getIconYOffset() const {
	return -3;	// offset the torch texture
}

bool TorchBlock::mayPlace(BlockSource& region, const BlockPos& pos) const {
	if (canBePlacedOn(region, pos, Facing::NORTH) ||
		canBePlacedOn(region, pos, Facing::SOUTH) ||
		canBePlacedOn(region, pos, Facing::EAST) ||
		canBePlacedOn(region, pos, Facing::WEST) ||
		canBePlacedOn(region, pos, Facing::UP)) {
		return true;
	}
	return false;
}

bool TorchBlock::canBePlacedOn(BlockSource& region, const BlockPos& pos, FacingID facing) const {
	BlockPos relative = pos.neighbor(Facing::OPPOSITE_FACING[facing]);

	return region.canProvideSupport(relative, facing, BlockSupportType::Center);
}

int TorchBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	auto& region = by.getRegion();

	bool facingIsSupportingBlock = (face != Facing::DOWN);
	if (canBePlacedOn(region, pos, face) && facingIsSupportingBlock) {
		return DATA_FROM_FACING[face];
	}

	for (auto& facing : Facing::Plane::HORIZONTAL) {
		if (region.canProvideSupport(pos.relative(Facing::OPPOSITE_FACING[facing]), facing, BlockSupportType::Center)) {
			return DATA_FROM_FACING[facing];
		}
	}

	return TORCH_TOP;
}

void TorchBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	if (getDirection(region.getData(pos)) == TORCH_UNKNOWN) {
		if (region.canProvideSupport(pos.west(), Facing::EAST, BlockSupportType::Center)) {
			region.setBlockAndData(pos, mID, TORCH_WEST, Block::UPDATE_ALL);
		} else if (region.canProvideSupport(pos.east(), Facing::WEST, BlockSupportType::Center)) {
			region.setBlockAndData(pos, mID, TORCH_EAST, Block::UPDATE_ALL);
		} else if (region.canProvideSupport(pos.north(), Facing::SOUTH, BlockSupportType::Center)) {
			region.setBlockAndData(pos, mID, TORCH_NORTH, Block::UPDATE_ALL);
		} else if (region.canProvideSupport(pos.south(), Facing::NORTH, BlockSupportType::Center)) {
			region.setBlockAndData(pos, mID, TORCH_SOUTH, Block::UPDATE_ALL);
		} else if (region.canProvideSupport(pos.below(), Facing::UP, BlockSupportType::Center)) {
			region.setBlockAndData(pos, mID, TORCH_TOP, Block::UPDATE_ALL);
		}
	}

	_checkCanSurvive(region, pos);
}


void TorchBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	_checkDoPop(region, pos);
}

const AABB& TorchBlock::getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping) const {
	int dir = getDirection(region.getData(pos));

	float r = 0.15f;
	if (dir == TORCH_WEST) {
		bufferAABB.set(0, 0.2f, 0.5f - r, r * 2, 0.8f, 0.5f + r);
	} else if (dir == TORCH_EAST) {
		bufferAABB.set(1 - r * 2, 0.2f, 0.5f - r, 1, 0.8f, 0.5f + r);
	} else if (dir == TORCH_NORTH) {
		bufferAABB.set(0.5f - r, 0.2f, 0, 0.5f + r, 0.8f, r * 2);
	} else if (dir == TORCH_SOUTH) {
		bufferAABB.set(0.5f - r, 0.2f, 1 - r * 2, 0.5f + r, 0.8f, 1);
	} else {
		r = 0.1f;
		bufferAABB.set(0.5f - r, 0.0f, 0.5f - r, 0.5f + r, 0.6f, 0.5f + r);
	}

	return bufferAABB;
}

Vec3 TorchBlock::_flameParticlePos(const BlockPos& pos, DataID dir) const {
	Vec3 orig = Vec3(0.5f, 0.7f, 0.5f) + pos;
	float h = 0.22f;
	float r = 0.27f;

	switch (dir) {
	case TORCH_WEST:
		return Vec3(orig.x - r, orig.y + h, orig.z);
	case TORCH_EAST:
		return Vec3(orig.x + r, orig.y + h, orig.z);
	case TORCH_NORTH:
		return Vec3(orig.x, orig.y + h, orig.z - r);
	case TORCH_SOUTH:
		return Vec3(orig.x, orig.y + h, orig.z + r);
	default:
		return orig;
	}
}

void TorchBlock::animateTick(BlockSource& region, const BlockPos& pos, Random& random) const {
	auto& level = region.getLevel();

	Vec3 particlePos = _flameParticlePos(pos, region.getData(pos));

	// TODO: rherlitz
// 	level.addParticle(ParticleType::Smoke, particlePos, Vec3(0.0f));
// 	level.addParticle(ParticleType::Flame, particlePos, Vec3(0.0f));
}

bool TorchBlock::canBeSilkTouched() const {
	return false;
}

bool TorchBlock::_checkDoPop(BlockSource& region, const BlockPos& pos) const {
	if (_checkCanSurvive(region, pos)) {
		int dir = getDirection(region.getData(pos));
		FacingID facing = FACING_FROM_DATA[dir];
		bool replace = !canBePlacedOn(region, pos, facing);

		if (replace) {
			spawnResources(region, pos, region.getData(pos), 1.0f);
			region.setBlock(pos, BlockID::AIR, Block::UPDATE_ALL);
			return true;
		}
	}

	return true;
}

bool TorchBlock::_checkCanSurvive(BlockSource& region, const BlockPos& pos) const {
	// Don't kill the torch just because a neighbor chunk is unloaded.
	int dir = getDirection(region.getData(pos));
	FacingID facing = FACING_FROM_DATA[dir];
	BlockPos attachPos = pos.neighbor(Facing::OPPOSITE_FACING[facing]);

	if (region.getChunk(attachPos) == nullptr) {
		return true;
	}

	if (!mayPlace(region, pos)) {
		auto t = region.getBlockAndData(pos);

		if (t.id == mID) {
			spawnResources(region, pos, t.data, 1.0f);
			region.setBlock(pos, BlockID::AIR, Block::UPDATE_ALL);
		}

		return false;
	}

	return true;
}

int TorchBlock::getDirection(const DataID& data) const {
	return getBlockState(BlockState::FacingDirection).get<int>(data);
}
