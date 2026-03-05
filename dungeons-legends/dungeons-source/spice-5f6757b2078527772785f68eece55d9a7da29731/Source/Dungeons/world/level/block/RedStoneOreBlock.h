/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class BlockSource;
class Player;

class RedStoneOreBlock : public Block {
public:
	RedStoneOreBlock(const std::string& nameId, int id, bool lit);

	bool attack(Player* player, const BlockPos& pos) const override;

	void onStepOn(Entity& entity, const BlockPos& pos) const override;

	bool use(Player& player, const BlockPos& pos) const override;

	int getTickDelay() const;
	void tick(BlockSource& region, const BlockPos& pos, Random& random) const override;
	void animateTick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	
	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;

	int getExperienceDrop(Random& random) const override;

	ItemInstance getSilkTouchItemInstance(DataID data) const override;

private:
	//	@Todo: This function exist in Obsidian as well and is nearly functionally identical.
	//	Maybe abstract it out into the base block?
	void _poofParticles(BlockSource& region, const BlockPos& pos) const;

	void _interact(BlockSource& region, const BlockPos& pos) const;

	bool mLit;
};
