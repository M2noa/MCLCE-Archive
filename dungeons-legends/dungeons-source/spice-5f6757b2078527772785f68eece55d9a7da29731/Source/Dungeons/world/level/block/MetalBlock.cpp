/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/MetalBlock.h"
#include "world/level/material/Material.h"

MetalBlock::MetalBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Metal)) {
}
