/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

enum class WoodBlockType {
	Oak = 0,
	Spruce = 1,
	Birch = 2,
	Jungle = 3,
	Acacia = 4,
	Big_Oak = 5,
	_count
};

class WoodBlock : public Block {
public:
	WoodBlock(const std::string& nameId, int id);

	virtual std::string buildDescriptionName(DataID data) const override;

	static const std::array<std::string, 6> WOOD_NAMES;

	static const Color getWoodMaterialColor(WoodBlockType type);

	static bool isValidWoodMaterial(int woodType);

protected:
	DataID getSpawnResourcesAuxValue(DataID data) const override;

};
