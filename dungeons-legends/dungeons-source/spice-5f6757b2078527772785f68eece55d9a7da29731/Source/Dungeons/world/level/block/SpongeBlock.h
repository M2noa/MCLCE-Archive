/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"
#include "world/item/ItemInstance.h"

class SpongeBlock : public Block {
public:
	enum class SpongeType : DataID {
		Dry = 0,
		Wet = 1
	};

	SpongeBlock(const std::string& nameId, int id, const Material& material);

	virtual std::string buildDescriptionName(DataID data) const override;
	virtual DataID getSpawnResourcesAuxValue(DataID data) const override;

	virtual bool isValidAuxValue(int value) const override;

	virtual void animateTick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	virtual void onPlace(BlockSource& region, const BlockPos& pos) const override;
	virtual void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;

private:
	void _attemptAbsorbWater(BlockSource& region, const BlockPos& pos) const;
	bool _performAbsorbWater(BlockSource& region, const BlockPos& startPos) const;
};
