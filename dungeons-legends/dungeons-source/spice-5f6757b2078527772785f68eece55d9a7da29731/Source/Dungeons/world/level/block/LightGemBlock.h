/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"
//	@Todo: Change 'LightGemBlock' block to be 'GlowstoneBlock'
class LightGemBlock : public Block {
public:
	LightGemBlock(const std::string& nameId, int id, const Material& material);
	virtual int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;
	virtual int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
 };
