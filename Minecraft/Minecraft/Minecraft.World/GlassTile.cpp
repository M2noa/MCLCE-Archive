#include "stdafx.h"
#include "GlassTile.h"

GlassTile::GlassTile(int id, Material *material, bool allowSame) : HalfTransparentTile(id, L"glass", material, allowSame)
{
}

int GlassTile::getResourceCount(Random *random)
{
	return 0;
}

int GlassTile::getRenderLayer()
{
	return 0;
}

bool GlassTile::isSolidRender()
{
	return false;
}

bool GlassTile::isCubeShaped()
{
	return false;
}

bool GlassTile::isSilkTouchable()
{
	return true;
}