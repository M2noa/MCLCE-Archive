/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/SlabBlock.h"

class StoneSlabBlock : public SlabBlock {
public:
	enum class StoneSlabType {
		Stone = 0,
		Sandstone = 1,
		Wood = 2,	//	@Note:	Must not use, this is here to maintain functionality. Double (Stone) Wooden Slab
		Cobblestone = 3,
		Brick = 4,
		StoneBrick = 5,
		Quartz = 6,			//	@Note: Should the Quartz and Netherbrick Damage Values be inversed compared to the Java version?
		Netherbrick = 7,

		_count
	};

	static const std::string SLAB_NAMES[];
	static const int SLAB_NAMES_COUNT;

	StoneSlabBlock(const std::string& nameId, int id, bool fullSize);
	
	int getVariant(int data) const override;

	virtual std::string buildDescriptionName(DataID data) const override;
	
	bool isValidAuxValue(int auxValue) const override;

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;

protected:
	DataID getSpawnResourcesAuxValue(DataID data) const override;
};

class StoneSlabBlock2 : public SlabBlock {
public:
	enum class StoneSlabType {
		RedSandstone = 0,
		Purpur = 1,
	};

	static const std::string SLAB_NAMES[];
	static const int SLAB_NAMES_COUNT;

	StoneSlabBlock2(const std::string& nameId, int id, bool fullSize);

	int getVariant(int data) const override;

	virtual std::string buildDescriptionName(DataID data) const override;

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;

protected:
	DataID getSpawnResourcesAuxValue(DataID data) const override;

};
