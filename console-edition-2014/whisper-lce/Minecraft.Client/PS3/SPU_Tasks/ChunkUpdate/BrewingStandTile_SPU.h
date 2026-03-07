#pragma once
#include "EntityTile_SPU.h"


class BrewingStandTile_SPU : public EntityTile_SPU
{
public:
	BrewingStandTile_SPU(int id) : EntityTile_SPU(id) {}
    virtual bool isSolidRender(bool isServerLevel = false) { return false; }
    virtual int getRenderShape() { return SHAPE_BREWING_STAND; }
    virtual bool isCubeShaped() { return false; }
	virtual void updateDefaultShape() { setShape(0, 0, 0, 1, 2.0f / 16.0f, 1); }
	Icon_SPU *getBaseTexture() { return &ms_pTileData->brewingStand_iconBase; }
};