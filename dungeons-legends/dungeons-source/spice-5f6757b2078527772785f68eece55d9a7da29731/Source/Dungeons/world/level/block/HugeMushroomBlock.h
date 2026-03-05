/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class Material;

class HugeMushroomBlock : public Block {
public:
	enum class Type {
		Brown = 0,
		Red = 1
	};

	HugeMushroomBlock(const std::string& nameId, int id, const Material& material, Type type);

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;

	DataID getSpawnResourcesAuxValue(DataID data) const override;

private:
	Type mType;
};
