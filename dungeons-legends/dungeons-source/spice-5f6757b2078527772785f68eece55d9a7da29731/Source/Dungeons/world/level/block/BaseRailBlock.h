/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class BlockPos;

class BaseRailBlock : public Block {
public:
	static const DataID DIR_FLAT_Z;
	static const DataID DIR_FLAT_X;
	// the data bit is used by boosters and detectors, so they can't turn
	static const int RAIL_DATA_BIT = 1 << 3;
	static const int RAIL_DIRECTION_MASK = 7;

	static bool isRail(BlockSource& region, const BlockPos& pos);
	static bool isRail(int id);

	BaseRailBlock(const std::string& nameId, int id, bool usesDataBit);
	bool isUsesDataBit() const;
	virtual const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const override;
	virtual const AABB& getVisualShape(DataID data, AABB& bufferAABB, bool isClipping = false) const override;
	virtual int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;
	virtual bool mayPlace(BlockSource& region, const BlockPos& pos) const override;

	virtual void onLoaded(BlockSource& region, const BlockPos& pos) const override;
	virtual void onPlace(BlockSource& region, const BlockPos& pos) const override;
	virtual void onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const override;
	virtual void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;

	virtual ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;
	virtual bool isRailBlock() const override;

protected:
	class Rail {
public:
		Rail(BlockSource& region, const BlockPos& pos);

		void place(bool hasSignal, bool first);

	protected:
		void updateConnections(int direction);
		void removeSoftConnections();
		std::shared_ptr<BaseRailBlock::Rail> getRail(const BlockPos& p);
		bool connectsTo(Rail& rail);
		bool hasConnection(const BlockPos& pos);
		bool canConnectTo(Rail& rail);
		void connectTo(Rail& rail);
		bool hasNeighborRail(const BlockPos& pos);

		BlockSource& mRegion;
		BlockPos mPos;
		bool mUsesDataBit;
		std::vector<BlockPos> mConnections;
	};

	void updateDir(BlockSource& region, const BlockPos& pos, bool first) const;
	virtual void updateState(BlockSource& region, const BlockPos& pos, int data, int dir, int type) const;

private:
	static void _createCircuitComponent(BlockSource& region, const BlockPos& pos, DataID data);
	void _updatePlacement(BlockSource& region, const BlockPos& pos, int type) const;
	static bool _isFacingWestEast(BlockSource& region, const BlockPos& pos);

	const bool mUsesDataBit;
};
