/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class StoneBlock : public Block {
public:
	enum class StoneType : DataID {
		Stone = 0,
		Granite = 1,
		GraniteSmooth = 2,
		Diorite = 3,
		DioriteSmooth = 4,
		Andesite = 5,
		AndesiteSmooth = 6
	};
	static const int NUM_TYPES = 7;

	StoneBlock(const std::string& nameId, int id);

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	DataID getSpawnResourcesAuxValue(DataID data) const override;

	virtual std::string buildDescriptionName(DataID data) const override;
};
