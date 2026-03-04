#pragma once
#include "Tile.h"

class ChunkRebuildData;
class SmoothStoneBrickTile : public Tile
{
	friend class ChunkRebuildData;
public:
	static const int TYPE_DEFAULT = 0;
    static const int TYPE_MOSSY = 1;
    static const int TYPE_CRACKED = 2;
	static const int TYPE_DETAIL = 3;

	static const wstring TEXTURE_NAMES[];

	static const int SMOOTH_STONE_BRICK_NAMES_LENGTH = 4;

	static const unsigned int SMOOTH_STONE_BRICK_NAMES[SMOOTH_STONE_BRICK_NAMES_LENGTH];

private:
	Icon **icons;

public:

    SmoothStoneBrickTile(int id);

public:
	virtual Icon *getTexture(int face, int data);

	virtual unsigned int getDescriptionId(int iData = -1);
	virtual int getSpawnResourcesAuxValue(int data);
	void registerIcons(IconRegister *iconRegister);
};