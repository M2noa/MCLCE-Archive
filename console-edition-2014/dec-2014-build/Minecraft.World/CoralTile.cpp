#include "stdafx.h"
#include "CoralTile.h"

CoralTile::CoralTile(int id) : HalfTransparentTile(id, L"coral", Material::coral, true)
{
    float r = 1 / 16.0f;
    setShape(0 - r, 0 - r, 0 - r, 1 + r, 1 + r, 1 + r);
}

int CoralTile::getColor(LevelSource *level, int x, int y, int z)
{
	return (x * x * 3187961 + x * 987243 + y * y * 43297126 + y * 987121 + z * z * 927469861 + z * 1861) & 0xffffff;
}

int CoralTile::getColor(LevelSource *level, int x, int y, int z, int data)
{
	return (x * x * 3187961 + x * 987243 + y * y * 43297126 + y * 987121 + z * z * 927469861 + z * 1861) & 0xffffff;
}
