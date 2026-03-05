#pragma once

#include "Tile_SPU.h"

class WallTile_SPU : public Tile_SPU
{
public:
	static const float WALL_WIDTH = 3.0f / 16.0f;
	static const float WALL_HEIGHT = 13.0f / 16.0f;
	static const float POST_WIDTH = 4.0f / 16.0f;
	static const float POST_HEIGHT = 16.0f / 16.0f;


	static const int TYPE_NORMAL = 0;
	static const int TYPE_MOSSY = 1;

	static const unsigned int COBBLE_NAMES[2];

	WallTile_SPU(int id) : Tile_SPU(id) {}

	Icon_SPU *getTexture(int face, int data)
	{
		if (data == TYPE_MOSSY)
		{
			return TileRef_SPU(mossStone_Id)->getTexture(face);
		}
		return TileRef_SPU(stoneBrick_Id)->getTexture(face);

	}
	int getRenderShape() { return SHAPE_WALL; }
	bool isCubeShaped() { return false; }
 	bool isSolidRender(bool isServerLevel = false) { return false; }
	void updateShape(ChunkRebuildData *level, int x, int y, int z)
	{
		bool n = connectsTo(level, x, y, z - 1);
		bool s = connectsTo(level, x, y, z + 1);
		bool w = connectsTo(level, x - 1, y, z);
		bool e = connectsTo(level, x + 1, y, z);

		float west = .5f - POST_WIDTH;
		float east = .5f + POST_WIDTH;
		float north = .5f - POST_WIDTH;
		float south = .5f + POST_WIDTH;
		float up = POST_HEIGHT;

		if (n)
		{
			north = 0;
		}
		if (s)
		{
			south = 1;
		}
		if (w)
		{
			west = 0;
		}
		if (e)
		{
			east = 1;
		}

		if (n && s && !w && !e)
		{
			up = WALL_HEIGHT;
			west = .5f - WALL_WIDTH;
			east = .5f + WALL_WIDTH;
		}
		else if (!n && !s && w && e)
		{
			up = WALL_HEIGHT;
			north = .5f - WALL_WIDTH;
			south = .5f + WALL_WIDTH;
		}

		setShape(west, 0, north, east, up, south);
	}
	bool connectsTo(ChunkRebuildData *level, int x, int y, int z)
	{
		int tile = level->getTile(x, y, z);
		if (tile == id || tile == Tile_SPU::fenceGate_Id)
		{
			return true;
		}
		TileRef_SPU tileInstance(tile);
		if (tileInstance.getPtr() != NULL)
		{
			if (tileInstance->getMaterial()->isSolidBlocking() && tileInstance->isCubeShaped())
			{
				return tileInstance->getMaterial()->getID() != Material_SPU::vegetable_Id;
			}
		}
		return false;

	}
	bool shouldRenderFace(ChunkRebuildData *level, int x, int y, int z, int face)
	{
		if (face == Facing::DOWN)
		{
			return Tile_SPU::shouldRenderFace(level, x, y, z, face);
		}
		return true;

	}
};