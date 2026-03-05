/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/SlabBlock.h"
//#include "world/item/AuxDataBlockItem.h"

class WoodSlabBlock : public SlabBlock {
public:
// 	class Item : public AuxDataBlockItem {
// 	public:
// 		Item(const std::string & name, int id);
// 		bool useOn(ItemInstance& instance, Entity& entity, int x, int y, int z, FacingID face, float clickX, float clickY, float clickZ) override;
// 	};

	WoodSlabBlock(const std::string& nameId, int id, bool fullSize);

	int getVariant(int data) const override;
	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;

	bool isValidAuxValue(int auxValue) const override;

	std::string buildDescriptionName(DataID data) const override;

protected:
	DataID getSpawnResourcesAuxValue(DataID data) const override;

};
