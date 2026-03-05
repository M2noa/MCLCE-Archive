/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class DirtBlock : public Block {
public:
	enum DirtType : unsigned char {
		TYPE_NORMAL = 0,
		TYPE_NO_GRASS_SPREAD
	};

	DirtBlock(const std::string& nameId, int id);
};
