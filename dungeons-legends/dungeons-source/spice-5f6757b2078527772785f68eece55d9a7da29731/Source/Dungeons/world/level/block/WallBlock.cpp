/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/WallBlock.h"
#include "world/level/Level.h"
#include "world/level/material/Material.h"
#include "world/level/block/FenceGateBlock.h"
#include "world/item/ItemInstance.h"
#include "world/Facing.h"
#include "world/level/BlockSource.h"
#include "world/Direction.h"
#include "locale/I18n.h"

const float WallBlock::WALL_WIDTH = 3.0f / 16.0f;
const float WallBlock::WALL_HEIGHT = 13.0f / 16.0f;
const float WallBlock::POST_WIDTH = 4.0f / 16.0f;
const float WallBlock::POST_HEIGHT = (16.0f / 16.0f) - SIZE_OFFSET;

const std::string WallBlock::COBBLE_NAMES[] = {
	"normal", "mossy"
};

WallBlock::WallBlock(const std::string& nameId, int id, const Material& material)
	: Block(nameId, id, material) {
	setSolid(false);
	mProperties = BlockProperty::Connects2D;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

WallBlock::WallBlock(const std::string& nameId, int id, const Block& baseBlock)
	: Block(nameId, id, baseBlock.getMaterial()) {
	setDestroyTime(baseBlock.getExplosionResistance() / 3);
	setSolid(false);
	mProperties = BlockProperty::Connects2D;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

int WallBlock::getVariant(int data) const {
	return (data == TYPE_MOSSY) ? 1 : 0;
}

std::string WallBlock::buildDescriptionName(DataID data) const {
	int type = getBlockState(BlockState::MappedType).get<int>(data);

	if (type < 0 || type > 1) {
		type = 0;
	}

	return I18n::get(mDescriptionId + "." + COBBLE_NAMES[type] + ".name");
}

const AABB& WallBlock::getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping) const {
	bool n = connectsTo(region, pos, pos.north() );
	bool s = connectsTo(region, pos, pos.south() );
	bool w = connectsTo(region, pos, pos.west() );
	bool e = connectsTo(region, pos, pos.east() );

	float west = .5f - POST_WIDTH;
	float east = .5f + POST_WIDTH;
	float north = .5f - POST_WIDTH;
	float south = .5f + POST_WIDTH;
	float up = POST_HEIGHT;

	if (n) {
		north = 0;
	}
	if (s) {
		south = 1;
	}
	if (w) {
		west = 0;
	}
	if (e) {
		east = 1;
	}

	if (n && s && !w && !e) {
		up = WALL_HEIGHT;
		west = .5f - WALL_WIDTH;
		east = .5f + WALL_WIDTH;
	} else if (!n && !s && w && e) {
		up = WALL_HEIGHT;
		north = .5f - WALL_WIDTH;
		south = .5f + WALL_WIDTH;
	}

	bufferAABB.set(west, 0, north, east, up, south);
	return bufferAABB;
}

bool WallBlock::canProvideSupport(BlockSource& region, const BlockPos& pos, FacingID face, BlockSupportType type) const {
	return face == Facing::UP && (type == BlockSupportType::Center || type == BlockSupportType::Any);
}

const AABB& WallBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	bufferValue = getVisualShape(region, pos, bufferValue);
	bufferValue.max.y = isClipping ? 1 : 1.5f;
	return bufferValue.move(Vec3(pos));
}

bool WallBlock::connectsTo(BlockSource& region, const BlockPos& thisPos, const BlockPos& otherPos) const {
	const Block& block = region.getBlock(otherPos);
	if(block.mID == mID) {
		return true;
	}
	
	if (block.hasProperty(BlockProperty::FenceGate)) {
		int dir = ((FenceGateBlock&)block).getBlockState(BlockState::Direction).get<int>(region.getData(otherPos));
		if (thisPos.z == otherPos.z && (dir == Direction::NORTH || dir == Direction::SOUTH)) {
			return true;
		} else if (thisPos.x == otherPos.x && (dir == Direction::EAST || dir == Direction::WEST)) {
			return true;
		}
	}

	const Material& material = block.getMaterial();
	if (material.isSolidBlocking() && block.hasProperty(BlockProperty::CubeShaped)) {
		return !material.isType(MaterialType::Vegetable);
	}

	return false;
}

DataID WallBlock::getSpawnResourcesAuxValue(DataID data) const {
	return data;
}

bool WallBlock::shouldRenderFace(BlockSource& region, const BlockPos& pos, FacingID face, const AABB& shape) const {
	if(face == Facing::DOWN) {
		return Block::shouldRenderFace(region, pos, face, shape);
	}
	
	return true;
}

bool WallBlock::canBeSilkTouched() const {
	return false;
}
