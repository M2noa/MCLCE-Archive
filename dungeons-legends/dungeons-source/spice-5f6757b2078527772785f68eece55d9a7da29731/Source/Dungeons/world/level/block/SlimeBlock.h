/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/HalfTransparentBlock.h"

class SlimeBlock : public HalfTransparentBlock {
public:
	SlimeBlock(const std::string& nameId, int id, const Material& material);

	void onFallOn(BlockSource& region, const BlockPos& pos, Entity* entity, float fallDistance) const override;
	void onStepOn(Entity& entity, const BlockPos& pos) const override;

	void updateEntityAfterFallOn(Entity& entity) const override;

	int getExtraRenderLayers() const override;
};
