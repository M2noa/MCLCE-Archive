/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/CameraBlock.h"

#include "world/Facing.h"
#include "world/level/material/Material.h"
#include "client/renderer/texture/TextureAtlasItem.h"


CameraBlock::CameraBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Wood)) {

	mProperties = BlockProperty::Unspecified;
	mRenderLayer = RENDERLAYER_OPAQUE;
}
