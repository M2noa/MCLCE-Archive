/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/MultiTextureBlock.h"
#include "world/item/ItemInstance.h"

class SandStoneBlock : public MultiTextureBlock {
public:
	enum class SandstoneType {
		Default = 0,
		Heiroglyphs = 1,
		Smoothside = 2
	};

	SandStoneBlock(const std::string& nameId, int id);

	virtual std::string buildDescriptionName(DataID data) const override;
};
