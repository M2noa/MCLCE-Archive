/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/DiodeBlock.h"

class RepeaterBlock : public DiodeBlock {
public:
	static const int DELAY_SHIFT = 2;
	static const float DELAY_RENDER_OFFSETS[4];

	RepeaterBlock(const std::string& nameId, int id, bool on);

	void onPlace(BlockSource& region, const BlockPos& pos) const override;
	void onLoaded(BlockSource& region, const BlockPos& pos) const override;

	void updateDelay(BlockSource &region, const BlockPos& pos, bool doIncrement) const;
	bool use(Player& player, const BlockPos& pos) const override;

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;
	
	bool canBeSilkTouched() const override;

	bool isLocked(BlockSource& region, const BlockPos& pos, int data) const override;
	virtual bool isInteractiveBlock() const override {
		return true;
	}

	void animateTick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	bool mayPlace(BlockSource& region, const BlockPos& pos) const override;
	bool canSurvive(BlockSource& region, const BlockPos& pos) const override;

	void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;

	void onRemove(BlockSource& region, const BlockPos& pos) const override;
	bool shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const override;

protected:

	int getTurnOnDelay(int data) override;

	const DiodeBlock* getOnBlock() const override;
	const DiodeBlock* getOffBlock() const override;

	bool isAlternateInput(const Block& block) const override;

private:
	static const int DELAYS[4];
};
