#pragma once
#include "Tile.h"
#include "Material.h"

class ChunkRebuildData;

class ClothTile : public Tile
{
	friend class ChunkRebuildData;
private:
	Icon **icons;
public:
	ClothTile();
    virtual Icon *getTexture(int face, int data);
protected:
	virtual int getSpawnResourcesAuxValue(int data);
public:
    static int getTileDataForItemAuxValue(int auxValue);
    static int getItemAuxValueForTileData(int data);
	//@Override
	void registerIcons(IconRegister *iconRegister);
};