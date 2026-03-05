/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/

#pragma once

#include "world/level/block/HalfTransparentBlock.h"

class Material;
class ItemInstance;
class I18n;

class StainedGlassBlock : public HalfTransparentBlock {
public:
	StainedGlassBlock(const std::string& nameId, int id, const Material& material, bool allowSame);

	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;

	DataID getSpawnResourcesAuxValue(DataID data) const override;

	static int getBlockDataForItemAuxValue(int data);
	static int getItemAuxValueForBlockData(int data);

	virtual std::string buildDescriptionName(DataID data) const override;

};