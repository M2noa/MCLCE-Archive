/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/FenceBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/block/Block.h"
#include "world/level/block/FenceGateBlock.h"
#include "world/level/material/Material.h"
#include "world/item/ItemInstance.h"
#include "world/Direction.h"
#include "world/Facing.h"
#include "locale/I18n.h"
// #include "world/item/LeadItem.h"
#include "world/entity/player/Player.h"
#include "world/level/Level.h"

FenceBlock::FenceBlock(const std::string& nameId, int id, const Material& material) 
	: Block(nameId, id, material) {
	setSolid(false);
	mProperties = BlockProperty::Connects2D | BlockProperty::Power_NO;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

const AABB& FenceBlock::getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping) const {
	bool n = connectsTo(region, pos, pos.north());
	bool s = connectsTo(region, pos, pos.south());
	bool w = connectsTo(region, pos, pos.west());
	bool e = connectsTo(region, pos, pos.east());

	float west = 6.0f / 16.0f;
	float east = 10.0f / 16.0f;
	float north = 6.0f / 16.0f;
	float south = 10.0f / 16.0f;

	if(n) {
		north = 0;
	}
	if(s) {
		south = 1;
	}
	if(w) {
		west = 0;
	}
	if(e) {
		east = 1;
	}

	bufferAABB.set(west, 0, north, east, 1.0f, south);
	return bufferAABB;
}

bool FenceBlock::connectsTo(BlockSource& region, const BlockPos& thisPos, const BlockPos& otherPos) const {
	const Block& block = region.getBlock(otherPos);
	const Material& material = block.getMaterial(); 
	if (/*jasonmaj REFACTOR TODO block.getBlockShape() == BlockShape::FENCE && */ material == getMaterial() && block.isFenceBlock()) {
		return true;
	} else if (block.hasProperty(BlockProperty::FenceGate)) {
		int dir = ((const FenceGateBlock&)block).getBlockState(BlockState::Direction).get<int>(region.getData(otherPos));
		if (thisPos.z == otherPos.z && (dir == Direction::NORTH || dir == Direction::SOUTH)) {
			return true;
		} else if (thisPos.x == otherPos.x && (dir == Direction::EAST || dir == Direction::WEST)) {
			return true;
		}
	} else if (material.isSolidBlocking() && block.hasProperty(BlockProperty::CubeShaped)) {
		return !material.isType(MaterialType::Vegetable);
	}
	return false;
}

const AABB& FenceBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	bool n = connectsTo(region, pos, pos.north());
	bool s = connectsTo(region, pos, pos.south());
	bool w = connectsTo(region, pos, pos.west());
	bool e = connectsTo(region, pos, pos.east());

	float west = 6.0f / 16.0f;
	float east = 10.0f / 16.0f;
	float north = 6.0f / 16.0f;
	float south = 10.0f / 16.0f;

	if(n) {
		north = 0;
	}
	if(s) {
		south = 1;
	}
	if(w) {
		west = 0;
	}
	if(e) {
		east = 1;
	}

	float height = isClipping ? 1.0f : 1.5f;
	bufferValue.set((float)pos.x + west, (float)pos.y, (float)pos.z + north, (float)pos.x + east, (float)pos.y + height, (float)pos.z + south);
	return bufferValue;
}

bool FenceBlock::addCollisionShapes(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes, Entity* entity) const {
	bool n = connectsTo(region, pos, pos.north());
	bool s = connectsTo(region, pos, pos.south());
	bool w = connectsTo(region, pos, pos.west());
	bool e = connectsTo(region, pos, pos.east());

	float west = 6.0f / 16.0f;
	float east = 10.0f / 16.0f;
	float north = 6.0f / 16.0f;
	float south = 10.0f / 16.0f;

	if (n) {
		north = 0;
	}
	if (s) {
		south = 1;
	}

	float height = 1.5f;
	AABB tmpAABB;
	if (n || s) {
		tmpAABB.set((float)pos.x + west, (float)pos.y, (float)pos.z + north, (float)pos.x + east, (float)pos.y + height, (float)pos.z + south);
		addAABB(tmpAABB, intersectTestBox, inoutBoxes);
	}

	if (w) {
		west = 0;
	}
	if (e) {
		east = 1;
	}
	if (w || e) {
		north = 6.0f / 16.0f;
		south = 10.0f / 16.0f;
		tmpAABB.set((float)pos.x + west, (float)pos.y, (float)pos.z + north, (float)pos.x + east, (float)pos.y + height, (float)pos.z + south);
		addAABB(tmpAABB, intersectTestBox, inoutBoxes);
	}

	if (!n && !s && !w && !e) {
		tmpAABB.set((float)pos.x + west, (float)pos.y, (float)pos.z + north, (float)pos.x + east, (float)pos.y + height, (float)pos.z + south);
		addAABB(tmpAABB, intersectTestBox, inoutBoxes);
	}

	return true;
}

bool FenceBlock::canProvideSupport(BlockSource& region, const BlockPos& pos, FacingID face, BlockSupportType type) const {
	return face == Facing::UP && (type == BlockSupportType::Center || type == BlockSupportType::Any);
}

DataID FenceBlock::getSpawnResourcesAuxValue(DataID data) const {
	return data;
}

bool FenceBlock::use(Player& player, const BlockPos& pos) const {	
// 	if (player.getLevel().isClientSide())
// 		return false;
// 	
// 	return LeadItem::bindPlayerMobs(player, pos.x, pos.y, pos.z);

	return false;
}

std::string FenceBlock::buildDescriptionName(DataID data) const {
	int mappedType = getBlockState(BlockState::MappedType).get<int>(data);
	if(mappedType < 1 || mappedType > 5) {
		return I18n::get(mDescriptionId + ".name");
	}

	static const std::array<std::string, 6> FENCE_NAMES = {
		{"fence", "spruceFence", "birchFence", "jungleFence", "acaciaFence", "darkOakFence" }
	};

	return I18n::get(Block::BLOCK_DESCRIPTION_PREFIX + FENCE_NAMES[mappedType] + ".name");
}

bool FenceBlock::canBeSilkTouched() const {
	return false;
}
