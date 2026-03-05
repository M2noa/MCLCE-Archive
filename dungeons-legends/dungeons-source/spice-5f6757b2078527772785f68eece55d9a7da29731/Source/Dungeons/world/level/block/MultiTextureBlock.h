/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"
#include "Block.h"
#include "world/item/ItemInstance.h"

class Material;

class MultiTextureBlock : public Block {
public:
	enum class Type : int {
		Default = 0,
		Mossy = 1,
		Cracked = 2,
		Chiseled = 3,
		Smooth = 4
	};

	MultiTextureBlock(const std::string& nameId, int id, const Material& material);

	virtual std::string buildDescriptionName(DataID data) const override;

protected:
	DataID getSpawnResourcesAuxValue(DataID data) const override;
};
