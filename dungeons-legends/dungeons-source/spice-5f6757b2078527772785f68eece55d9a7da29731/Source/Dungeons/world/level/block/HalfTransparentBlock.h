/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class Material;

class HalfTransparentBlock : public Block {
public:
	HalfTransparentBlock(const std::string& nameId, int id, const Material& material, bool allowSame);

private:
	bool mAllowSame;
};
