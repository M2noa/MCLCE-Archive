/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/EntityBlock.h"
#include "world/Facing.h"

class BlockSource;

class PistonBlock : public EntityBlock {
	static const BlockPos ARM_DIRECTION_OFFSETS[6];

public:
	enum class Type {
		Normal,
		Sticky
	};
	
	static const BlockPos& getFacingDir(int inData) {
		FacingID pistonFacing = PistonBlock::getFacing(inData);
		return getFacingDir(enum_cast(pistonFacing));
	}

	static const BlockPos& getFacingDir(FacingID inFacing) {
		return ARM_DIRECTION_OFFSETS[enum_cast(inFacing)];
	}

	static FacingID getFacing(int inData) {
		return Block::mPiston->getBlockState(BlockState::FacingDirection).get<FacingID>(inData);
	}

	int getVariant(int data) const override;
	FacingID getMappedFace(FacingID face, int data) const override;

public:
	PistonBlock(const std::string& nameId, int id, Type isSticky = Type::Normal);

	int getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const override;
	void onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const override;
	void onLoaded(BlockSource& region, const BlockPos& pos) const override;
	void onPlace(BlockSource& region, const BlockPos& pos) const override;
	
	//int getResource(Random& random, int data, int bonusLootLevel = 0) override;
	DataID getSpawnResourcesAuxValue(DataID data) const override;
	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;

	virtual bool canProvideSupport(BlockSource& region, const BlockPos& pos, FacingID face, BlockSupportType type) const override;

	void spawnResources(BlockSource& region, const BlockPos& pos, int data, float odds = 1, int bonusLootLevel = 0) const override;
	void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;
	bool canBeSilkTouched() const override;
	bool getSecondPart(BlockSource& region, const BlockPos& pos, BlockPos& out) const override;
	bool shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const override;

	bool isSticky() const;
private:
	friend class PistonBlockEntity;

	Type mType = Type::Normal;
};
