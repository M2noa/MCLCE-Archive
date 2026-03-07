#pragma once
#include "Tile.h"

class TransparentTile : public Tile
{
protected:
	bool allowSame;
protected:
	TransparentTile(int id, Material *material, bool allowSame, bool isSolidRender = false);
public:
	virtual bool isSolidRender(bool isServerLevel = false);
    virtual bool shouldRenderFace(LevelSource *level, int x, int y, int z, int face);
    virtual bool blocksLight();
};