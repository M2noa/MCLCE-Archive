/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/PortalBlock.h"
#include "world/level/material/Material.h"
#include "world/level/Level.h"
#include "world/level/BlockSource.h"
#include "world/level/dimension/Dimension.h"
// #include "world/level/PortalForcer.h"
// #include "world/item/MobPlacerItem.h"
#include "world/entity/Entity.h"
#include "world/Facing.h"
// #include "world/level/Spawner.h"

class PortalShape {
public:
	PortalShape(BlockSource& region, BlockPos pos, int axis);

	int getDistanceUntilEdge(const BlockPos& pos, int direction);

	int getHeight();
	int getWidth();
	int getNumberOfPortalBlocks();

	int calculatePortalHeight();
	bool isEmptyBlock(BlockID block);
	bool isValid();
	void createPortalBlocks();

private:
	BlockSource& mSource;
	int mAxis = 0;
	int mRightDir = 0;
	int mLeftDir = 0;
	int mNumPortalBlocks = 0;
	BlockPos mBottomLeft = BlockPos::ZERO;
	bool mBottomLeftValid = false;
	int mHeight = 0;
	int mWidth = 0;
};

//------------------------------------------------------------------------------

int PortalBlock::getAxis(int data) {
	return Block::mPortal->getBlockState(BlockState::Axis).get<int>(data);
}

PortalBlock::PortalBlock(const std::string& nameId, int id)
	: HalfTransparentBlock(nameId, id, Material::getMaterial(MaterialType::Portal), false) {
	setTicking(true);
	setSolid(false);
	mProperties = BlockProperty::Portal | BlockProperty::Immovable;
	mAnimatedTexture = true;
}

bool PortalBlock::mayPick(BlockSource& region, int data, bool liquid) const {
	return region.getLevel().getLevelData().getGameType() == GameType::Creative;
}

void PortalBlock::onRemove(BlockSource& region, const BlockPos& pos) const {
// 	region.getLevel().getPortalForcer().removePortalRecord(region, pos);
	Block::onRemove(region, pos);
}

void PortalBlock::tick(BlockSource& region, const BlockPos& pos, Random& random) const {
	HalfTransparentBlock::tick(region, pos, random);

// 	auto& dimension = region.getDimensionConst();
// 
// 	if (dimension.isNaturalDimension()) {
// 		int spawnChance = random.nextInt(2000);
// 
// 		switch(dimension.getLevelConst().getDifficulty()) {
// 		case Difficulty::Peaceful:
// 		case Difficulty::Easy:
// 			return;	// dont spawn in Peaceful and Easy
// 		case Difficulty::Normal:
// 			if(spawnChance < 2) {
// 				break;
// 			} else{
// 				return;
// 			}
// 		case Difficulty::Hard:
// 			if(spawnChance < 3) {
// 				break;
// 			} else{
// 				return;
// 			}
// 		default:
// 			DEBUG_FAIL("unknown difficulty");
// 			return;
// 		}
// 
// 		// locate floor
// 		int y0 = pos.y;
// 
// 		while (!region.canProvideSupport(BlockPos(pos.x, y0, pos.z), Facing::UP, BlockSupportType::Any) && y0 > 0) { // TODO: Does it really make sense to use canProvideSupport here?
// 			y0--;
// 		}
// 
// 		if (y0 > 0 && !region.isSolidBlockingBlock(pos.x, y0 + 1, pos.z)) {
// 			// spawn a pig man here
// 			Vec3 mobPos(pos.x + 0.5f, y0 + 1.1f, pos.z + 0.5f);
// // 			region.getLevel().getSpawner().spawnMob(region, EntityTypeToString(EntityType::PigZombie, EntityTypeNamespaceRules::ReturnWithNamespace), nullptr, mobPos);
// 		}
// 	}
}

void PortalBlock::animateTick(BlockSource& region, const BlockPos& pos, Random& random) const {
	auto& level = region.getLevel();

	// TODO: rherlitz
// 	if (random.nextInt(10) == 0) { // Java is 100, but calls animate tick 10 times more per frame (randomly choosing blocks to tick)
// 		level.playSound(LevelSoundEvent::Portal, Vec3(pos));
// 	}

	for (int i = 0; i < 40; i++) { // Java is 4, so increase this given that Java calls animateTick 10 times more frequently
		float x = pos.x + random.nextFloat();
		float y = pos.y + random.nextFloat();
		float z = pos.z + random.nextFloat();
		float xa = (random.nextFloat() - 0.5f) * 0.5f;
		float ya = (random.nextFloat() - 0.5f) * 0.5f;
		float za = (random.nextFloat() - 0.5f) * 0.5f;

		int flip = random.nextInt(2) * 2 - 1;
		if (region.getBlockID(pos.west()) == mID || region.getBlockID(pos.east()) == mID) {
			z = pos.z + 0.5f + 0.25f * flip;
			za = (random.nextFloat() * 2) * flip;
		} else {
			x = pos.x + 0.5f + 0.25f * flip;
			xa = (random.nextFloat() * 2) * flip;
		}

		// TODO: rherlitz
// 		level.addParticle(ParticleType::Portal, Vec3(x, y, z), Vec3(xa, ya, za));
	}
}

const AABB& PortalBlock::getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping) const {

	int dir = getAxis(region.getData(pos));
	if (dir == AXIS_UNKNOWN) {
		if (region.getBlockID( pos.west()) == mID || region.getBlockID( pos.east() ) == mID) {
			dir = AXIS_X;
		} else {
			dir = AXIS_Z;
		}
	}

	float xr = 2 / 16.0f;
	float yr = 2 / 16.0f;

	if (dir == AXIS_X) {
		xr = 8 / 16.0f;
	}

	if (dir == AXIS_Z) {
		yr = 8 / 16.0f;
	}

	bufferAABB.set(0.5f - xr, 0, 0.5f - yr, 0.5f + xr, 1, 0.5f + yr);
	return bufferAABB;
}

const AABB& PortalBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	if (isClipping) {
		return bufferValue.set(Vec3::ZERO, Vec3::ONE).move(Vec3(pos));
	}

	return AABB::EMPTY;
}

void PortalBlock::entityInside(BlockSource& region, const BlockPos& pos, Entity& entity) const {
// 	if (entity.mRiding == nullptr && !entity.isRide() && entity.canChangeDimensions()) {
// 		entity.handleInsidePortal(pos);
// 	}
}

void PortalBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	int axis = getAxis(region.getData(pos));

	if (axis == AXIS_X) {
		PortalShape xSize(region, pos, AXIS_X);
		if (!xSize.isValid() || xSize.getNumberOfPortalBlocks() < xSize.getWidth() * xSize.getHeight()) {
			region.setBlock(pos.x, pos.y, pos.z, BlockID::AIR, UPDATE_ALL);
		}
	} else if (axis == AXIS_Z) {
		PortalShape zSize(region, pos, AXIS_Z);
		if (!zSize.isValid() || zSize.getNumberOfPortalBlocks() < zSize.getWidth() * zSize.getHeight()) {
			region.setBlock(pos, BlockID::AIR, UPDATE_ALL);
		}
	}
}

int PortalBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 0;
}

bool PortalBlock::trySpawnPortal(BlockSource& region, const BlockPos& pos) const {

	// no portals or nether for legacy worlds
	if (region.getLevel().getLevelData().getGenerator() == GeneratorType::Legacy) {
		return false;
	}

	PortalShape xSize(region, pos, AXIS_X);
	if (xSize.isValid() && xSize.getNumberOfPortalBlocks() == 0) {
		xSize.createPortalBlocks();
// 		region.getLevel().getPortalForcer().addPortalRecord(region, pos.x, pos.y, pos.z, 1, 0);
		return true;
	}

	PortalShape zSize(region, pos, AXIS_Z);
	if (zSize.isValid() && zSize.getNumberOfPortalBlocks() == 0) {
		zSize.createPortalBlocks();
// 		region.getLevel().getPortalForcer().addPortalRecord(region, pos.x, pos.y, pos.z, 0, 1);
		return true;
	}

	return false;
}

 ItemInstance PortalBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
 	return ItemInstance();
 }

bool PortalBlock::isWaterBlocking() const {
	return true;
}

//------------------------------------------------------------------------------

PortalShape::PortalShape(BlockSource& region, BlockPos pos, int axis) :
	mSource(region) {
	mAxis = axis;
	if (axis == PortalBlock::AXIS_X) {
		mLeftDir = Facing::EAST;
		mRightDir = Facing::WEST;
	} else {
		mLeftDir = Facing::NORTH;
		mRightDir = Facing::SOUTH;
	}

	BlockPos originalPosition = pos;

	while (pos.y > originalPosition.y - PortalBlock::MAX_HEIGHT && pos.y > 0 && isEmptyBlock(mSource.getBlockID(pos.below()))) {
		pos = pos.below();
	}

	int edge = getDistanceUntilEdge(pos, mLeftDir) - 1;

	if (edge >= 0) {
		mBottomLeft = pos.relative(mLeftDir, edge);
		mBottomLeftValid = true;

		mWidth = getDistanceUntilEdge(mBottomLeft, mRightDir);

		if (mWidth < PortalBlock::MIN_WIDTH || mWidth > PortalBlock::MAX_WIDTH) {
			mBottomLeftValid = false;
			mWidth = 0;
		}
	}

	if (mBottomLeftValid) {
		mHeight = calculatePortalHeight();
	}
}

int PortalShape::getDistanceUntilEdge(const BlockPos& pos, int direction) {
	int width = 0;

	for (; width < PortalBlock::MAX_WIDTH + 1; width++) {
		BlockPos blockPos = pos.relative(direction, width);
		if (!isEmptyBlock(mSource.getBlockID(blockPos))) {
			break;	// Something is blocking the portal
		}

		if (mSource.getBlockID(blockPos.below()) != Block::mObsidian->mID) {
			break;	// There's no frame below this block
		}
	}

	BlockID edge = mSource.getBlockID(pos.relative(direction, width));
	if (edge == Block::mObsidian->mID) {
		return width;
	}

	return 0;
}

int PortalShape::getHeight() {
	return mHeight;
}

int PortalShape::getWidth() {
	return mWidth;
}

int PortalShape::getNumberOfPortalBlocks() {
	return mNumPortalBlocks;
}

int PortalShape::calculatePortalHeight() {
	int shapeFailed = false;

	for (mHeight = 0; mHeight < PortalBlock::MAX_HEIGHT; mHeight++) {
		for (int i = 0; i < mWidth; i++) {
			BlockPos blockPos = mBottomLeft.relative(mRightDir, i).above(mHeight);

			BlockID block = mSource.getBlockID(blockPos);
			if (!isEmptyBlock(block)) {
				shapeFailed = true;
				break;	// Something is blocking the portal
			}

			if (block == Block::mPortal->mID) {
				mNumPortalBlocks++;
			}

			if (i == 0) {
				block = mSource.getBlockID(blockPos.relative(mLeftDir));
				if (block != Block::mObsidian->mID) {
					shapeFailed = true;
					break;	// Missing left frame
				}
			} else if (i == mWidth - 1) {
				block = mSource.getBlockID(blockPos.relative(mRightDir));
				if (block != Block::mObsidian->mID) {
					shapeFailed = true;
					break;	// Missing right frame
				}
			}
		}

		if (shapeFailed) {
			break;
		}
	}

	for (int i = 0; i < mWidth; i++) {
		if (mSource.getBlockID(mBottomLeft.relative(mRightDir, i).above(mHeight)) != Block::mObsidian->mID) {
			mHeight = 0;
			break;	// We have no top edge
		}
	}

	if (mHeight > PortalBlock::MAX_HEIGHT || mHeight < PortalBlock::MIN_HEIGHT) {
		mBottomLeftValid = false;
		mWidth = 0;
		mHeight = 0;
		return 0;
	} else {
		return mHeight;
	}
}

bool PortalShape::isEmptyBlock(BlockID block) {
	return block == BlockID::AIR || block == Block::mFire->mID || block == Block::mPortal->mID;
}

bool PortalShape::isValid() {
	return mBottomLeftValid && mWidth >= PortalBlock::MIN_WIDTH && mWidth <= PortalBlock::MAX_WIDTH && mHeight >= PortalBlock::MIN_HEIGHT && mHeight <= PortalBlock::MAX_HEIGHT;
}

void PortalShape::createPortalBlocks() {
	for (int horizontal = 0; horizontal < mWidth; horizontal++) {
		BlockPos offset = mBottomLeft.relative(mRightDir, horizontal);
		for (int vertical = 0; vertical < mHeight; vertical++) {
			mSource.setBlockAndData(offset.above(vertical), Block::mPortal->mID, mAxis, PortalBlock::UPDATE_CLIENTS);
		}
	}
}
