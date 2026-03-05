/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/entity/player/Player.h"
#include "world/level/block/EndPortalFrameBlock.h"
#include "world/level/material/Material.h"
#include "world/Facing.h"
#include "world/level/BlockSource.h"
#include "world/entity/Mob.h"
// #include "world/redstone/circuit/CircuitSystem.h"
// #include "world/redstone/circuit/components/PoweredBlockComponent.h"
#include "world/level/Level.h"
#include "world/level/dimension/Dimension.h"
#include "world/redstone/Redstone.h"

class EndPortalShape {
public:
	EndPortalShape(BlockSource& region, BlockPos pos);

	int getDistanceUntilEdge(const BlockPos& pos, int direction, int facing);

	bool isEmptyBlock(BlockID block);
	bool isValid(BlockSource& region);
	BlockPos getOrigin() const;

	static std::vector<std::vector<BlockID>> mPortalPattern;
	
private:
	BlockSource& mSource;
	int mRightDir = 0;
	int mLeftDir = 0;
	int mDepthDir = 0;
	BlockPos mBottomLeft = BlockPos::ZERO;
	BlockPos mOrigin = BlockPos::ZERO;
	int mBlockDirection;

#if 0 //[IMPLEMENTATION] Unreferenced field 
	int mNumPortalBlocks = 0;
	bool mBottomLeftValid = false;
#endif
};

std::vector<std::vector<BlockID>> EndPortalShape::mPortalPattern = std::vector<std::vector<BlockID>>();

EndPortalFrameBlock::EndPortalFrameBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Stone)) {

	setSolid(false);
	setPushesOutItems(true);
	mProperties = BlockProperty::Power_NO | BlockProperty::Immovable;
	setVisualShape(Vec3::ZERO, Vec3(1, 13.0f / 16.0f, 1));
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}


void EndPortalFrameBlock::addAABBs(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes) const{
	AABB shape(0, 0, 0, 1, 13.0f / 16.0f, 1);
	addAABB(shape.move( Vec3(pos) ), intersectTestBox, inoutBoxes);

	int data = region.getData(pos);
	if (hasEye(data)) {
		shape.set(5.0f / 16.0f, 13.0f / 16.0f, 5.0f / 16.0f, 11.0f / 16.0f, 1, 11.0f / 16.0f);
		addAABB(shape, intersectTestBox, inoutBoxes);
	}
}

bool EndPortalFrameBlock::addCollisionShapes( BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes, Entity* entity ) const {
	addAABBs(region, pos, intersectTestBox, inoutBoxes);
	return true;
}

int EndPortalFrameBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	return Math::floor(by.getRotation().y * 4 / 360 + 0.5f) & 3;
}

bool EndPortalFrameBlock::hasEye(int data) {
	return Block::mEndPortalFrame->getBlockState(BlockState::EndPortalEyeBit).getBool(data);
}

int EndPortalFrameBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return 0;
}

int EndPortalFrameBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 0;
}
 
void EndPortalFrameBlock::onPlace(BlockSource& region, const BlockPos& pos) const {

	//All worlds ever generated in pocket have the frame blocks flipped in the strongholds,
	//so they are useless. Flipping them when placing them makes it so we can accept the
	//inverted portals as valid instead :)

	FullBlock fullBlock = region.getBlockAndData(pos);
	auto dirState = Block::mEndPortalFrame->getBlockState(BlockState::Direction);
	int dir = dirState.get<int>(fullBlock.data);

	dirState.set(fullBlock.data, Direction::DIRECTION_OPPOSITE[dir]);
	region.setBlockAndData(pos, fullBlock, Block::UPDATE_ALL);
}

bool EndPortalFrameBlock::use(Player& player, const BlockPos& pos) const {
	auto& region = player.getRegion();
	DataID eyeData = region.getData(pos);
	if (hasEye(eyeData)) {
		return false;
	}

	ItemInstance* instance = player.getSelectedItem();
	if (!instance || instance->getId() != Item::mEnderEye->getId()) {
		return false;
	}

	// place the ender eye
	eyeData |= EndPortalFrameBlock::EYE_BIT;
	region.setBlockAndData(pos, FullBlock(mID, eyeData), Block::UPDATE_CLIENTS);
// 	player.useItem(*instance);

	tryCreatePortal(region, pos);

	return true;
}

bool EndPortalFrameBlock::isInteractiveBlock() const {
	return true;
}

int EndPortalFrameBlock::getComparatorSignal(BlockSource& region, const BlockPos& pos, FacingID dir, int data) const {
	return hasEye(data) ? Redstone::SIGNAL_MAX : Redstone::SIGNAL_NONE;
}

void EndPortalFrameBlock::tryCreatePortal(BlockSource& region, const BlockPos& pos) {
	EndPortalShape shape(region, pos);
	if (shape.isValid(region)) {
		createPortal(region, shape.getOrigin());
	}
}

ItemInstance EndPortalFrameBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(Block::mEndPortalFrame);
}

ItemInstance EndPortalFrameBlock::getSilkTouchItemInstance(DataID data) const {
	return ItemInstance(Block::mEndPortalFrame);
}

void EndPortalFrameBlock::createPortal(BlockSource& region, const BlockPos& origin) {
	// no portals for legacy worlds
	if (region.getLevel().getLevelData().getGenerator() == GeneratorType::Legacy) {
		return;
	}

	for (int i = -1; i <= 1; ++i) {
		for (int j = -1; j <= 1; ++j) {
			BlockPos pos = origin + BlockPos(i, 0, j);
			region.setBlock(pos, Block::mEndPortal->mID, Block::UPDATE_CLIENTS);
		}
	}
}

EndPortalShape::EndPortalShape(BlockSource& region, BlockPos pos) :
	mSource(region) {

	FullBlock fullBlock = region.getBlockAndData(pos);
	mBlockDirection = Block::mEndPortalFrame->getBlockState(BlockState::Direction).get<int>(fullBlock.data);
	BlockPos newPos;
	int facing = 0;
	//Move inside the portal
	switch (mBlockDirection) {
	case Direction::SOUTH:
		newPos = pos.south();
		mDepthDir = Facing::SOUTH;
		facing = Facing::NORTH;
		mLeftDir = Facing::EAST;
		mRightDir = Facing::WEST;
		break;
	case Direction::WEST:
		newPos = pos.west();
		mDepthDir = Facing::WEST;
		facing = Facing::EAST;
		mLeftDir = Facing::SOUTH;
		mRightDir = Facing::NORTH;
		break;
	case Direction::NORTH:
		newPos = pos.north();
		mDepthDir = Facing::NORTH;
		facing = Facing::SOUTH;
		mLeftDir = Facing::WEST;
		mRightDir = Facing::EAST;
		break;
	case Direction::EAST:
		newPos = pos.east();
		mDepthDir = Facing::EAST;
		facing = Facing::WEST;
		mLeftDir = Facing::NORTH;
		mRightDir = Facing::SOUTH;
		break;
	default:
		LOGI("Direction doesn't exist!\n");
		break;
	}

	int edge = getDistanceUntilEdge(newPos, mLeftDir, facing);
	mBottomLeft = pos.relative(mLeftDir, edge);

	if (mPortalPattern.empty()) {
		mPortalPattern = { { Block::mAir->mID, Block::mEndPortalFrame->mID, Block::mEndPortalFrame->mID, Block::mEndPortalFrame->mID, Block::mAir->mID },
					{ Block::mEndPortalFrame->mID, Block::mAir->mID, Block::mAir->mID, Block::mAir->mID, Block::mEndPortalFrame->mID },
					{ Block::mEndPortalFrame->mID, Block::mAir->mID, Block::mAir->mID, Block::mAir->mID, Block::mEndPortalFrame->mID },
					{ Block::mEndPortalFrame->mID, Block::mAir->mID, Block::mAir->mID, Block::mAir->mID, Block::mEndPortalFrame->mID },
					{ Block::mAir->mID, Block::mEndPortalFrame->mID, Block::mEndPortalFrame->mID, Block::mEndPortalFrame->mID, Block::mAir->mID } };
	}
}

int EndPortalShape::getDistanceUntilEdge(const BlockPos& pos, int direction, int facing) {
	int width = 0;

	for (; width < EndPortalFrameBlock::PORTAL_SIZE + 1; width++) {
		BlockPos blockPos = pos.relative(direction, width);
		if (!isEmptyBlock(mSource.getBlockID(blockPos))) {
			break;	// Something is blocking the portal
		}

		if (mSource.getBlockID(blockPos.relative(facing)) != Block::mEndPortalFrame->mID) {
			break;	// There's no frame below this block
		}
	}

	BlockID edge = mSource.getBlockID(pos.relative(direction, width));
	if (edge == Block::mEndPortalFrame->mID) {
		return width;
	}

	return 0;
}

bool EndPortalShape::isEmptyBlock(BlockID block) {
	return block == BlockID::AIR;
}

bool EndPortalShape::isValid(BlockSource& region) {
	for (int i = 0; i < 5; ++i) {
		for (int j = 0; j < 5; ++j) {
			//skip corners. they can be any block
			if ((i == 0 && j == 0) || (i == 0 && j == 4) || (i == 4 && j == 0) || (i == 4 && j == 4)) {
				continue;
			}
			BlockPos pos = mBottomLeft.relative(mRightDir, i).relative(mDepthDir, j);
			BlockID id = region.getBlockID(pos);
			if (id != mPortalPattern[i][j]) {
				return false;
			}
			if (id == Block::mEndPortalFrame->mID) {
				FullBlock fullBlock = region.getBlockAndData(pos);
				if (!EndPortalFrameBlock::hasEye(fullBlock.data)) {
					return false;
				}

				int dir = Block::mEndPortalFrame->getBlockState(BlockState::Direction).get<int>(fullBlock.data);
				if (i == 0 && dir != (mBlockDirection + 1) % 4) {
					return false;
				}
				else if (i == 4 && dir != (mBlockDirection + 3) % 4) {
					return false;
				}

				if (j == 0 && dir != (mBlockDirection) % 4) {
					return false;
				}
				else if (j == 4 && dir != (mBlockDirection + 2) % 4) {
					return false;
				}
			}
			if (i == 2 && j == 2) {
				mOrigin = pos;
			}
		}
	}

	return true;
}

BlockPos EndPortalShape::getOrigin() const {
	return mOrigin;
}

