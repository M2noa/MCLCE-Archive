/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class DiodeBlock : public Block {
public:
	static bool isDiode(const Block& block);

	DiodeBlock(const std::string& nameId, int id, bool on);

	bool mayPlace(BlockSource& region, const BlockPos& pos) const override;
	bool canSurvive(BlockSource& region, const BlockPos& pos) const override;

	void onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const override;

	int getVariant(int data) const override;
	bool shouldRenderFace(BlockSource& region, const BlockPos& pos, FacingID face, const AABB& shape) const override;

	int getDirectSignal(BlockSource& region, const BlockPos& pos, int dir) const override;
	virtual int getSignal(BlockSource& region, const BlockPos& pos, int dir) const;
	bool isSignalSource() const override;

	void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;

	virtual bool isLocked(BlockSource& region, const BlockPos& pos, int data) const;
	virtual bool isSameDiode(const Block& block) const;

	int getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const override;

	virtual bool shouldPrioritize(BlockSource& region, const BlockPos& pos, int data) const;

protected:
	virtual bool isOn(int data) const;
	virtual bool shouldTurnOn(BlockSource& region, const BlockPos& pos, int data) const;

	virtual void checkTickOnNeighbor(BlockSource& region, const BlockPos& pos, BlockID blockID) const;

	virtual int getInputSignal(BlockSource& region, const BlockPos& pos, int data) const;

	virtual bool isAlternateInput(const Block& block) const;
	virtual int getAlternateSignal(BlockSource& region, const BlockPos& pos, int data) const;
	int getAlternateSignalAt(BlockSource& region, const BlockPos& pos, int facing) const;

	virtual int getOutputSignal(BlockSource& region, const BlockPos& pos, int data) const;

	virtual int getTurnOffDelay(int data);
	virtual int getTurnOnDelay(int data) = 0;

	virtual const DiodeBlock* getOnBlock() const = 0;
	virtual const DiodeBlock* getOffBlock() const = 0;

	bool mOn;
};
