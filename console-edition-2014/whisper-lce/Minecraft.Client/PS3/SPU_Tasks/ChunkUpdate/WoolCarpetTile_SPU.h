#pragma once

#include "Tile_SPU.h"

class WoolCarpetTile_SPU : public Tile_SPU
{
public:
	WoolCarpetTile_SPU(int id) : Tile_SPU(id) {}

	Icon_SPU *getTexture(int face, int data) { return TileRef_SPU(cloth_Id)->getTexture(face, data); }
	bool isSolidRender(bool isServerLevel = false) { return false; }
	void updateDefaultShape() { updateShape(0); }
	void updateShape(ChunkRebuildData *level, int x, int y, int z) { updateShape(level->getData(x, y, z)); }

protected:
	void updateShape(int data)
	{
		int height = 0;
		float o = 1 * (1 + height) / 16.0f;
		setShape(0, 0, 0, 1, o, 1);
	}

public:
	bool shouldRenderFace(ChunkRebuildData *level, int x, int y, int z, int face)
	{
		if (face == 1) return true;
		return Tile_SPU::shouldRenderFace(level, x, y, z, face);

	}
};