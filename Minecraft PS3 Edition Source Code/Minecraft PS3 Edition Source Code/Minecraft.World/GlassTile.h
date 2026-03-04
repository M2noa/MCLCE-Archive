#pragma once
#include "HalfTransparentTile.h"

class Random;

class GlassTile : public HalfTransparentTile
{
public:
	using HalfTransparentTile::isSolidRender;

	GlassTile(int id, Material *material, bool allowSame);
	virtual int getResourceCount(Random *random);
	virtual int getRenderLayer();
	virtual bool isSolidRender();
	virtual bool isCubeShaped();
	virtual bool isSilkTouchable();
};
