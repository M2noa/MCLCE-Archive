/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/EntityBlock.h"

class FlowerPotBlockEntity;
class BlockSource;

class FlowerPotBlock : public EntityBlock {
public:
	FlowerPotBlock(const std::string& nameId, int id);

	void updateDefaultShape(void);
	bool isCubeShaped(void);
	bool isSolidRender(void) const;

	bool use(Player& player, const BlockPos& pos) const override;
	void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;

	bool mayPlace(BlockSource& region, const BlockPos& pos) const override;
	bool mayPlace(BlockSource& region, const BlockPos& pos, FacingID face) const override;

	bool canSurvive(BlockSource& region, const BlockPos& pos) const override;

	FlowerPotBlockEntity* getFlowerPotEntity(BlockSource& region, const BlockPos& pos) const;

	bool playerWillDestroy(Player& player, const BlockPos& pos, int data) const override;
	void onRemove(BlockSource& region, const BlockPos& pos) const override;

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;

protected:
	virtual bool canBeSilkTouched() const override;

private:
	bool isSupportedBlock(const Block* block, int plantType) const;
};
