#pragma once
#include "Tile_SPU.h"


class GrassTile_SPU : public Tile_SPU
{
	friend class Tile_SPU;
private:
// 	Icon *iconTop;
// 	Icon *iconSnowSide;
// 	Icon *iconSideOverlay;
public:
	static const int MIN_BRIGHTNESS = 4;

protected:
	GrassTile_SPU(int id) : Tile_SPU(id) {}
public:
	virtual Icon_SPU *getTexture(int face, int data);
	virtual Icon_SPU *getTexture(ChunkRebuildData *level, int x, int y, int z, int face);
    virtual int getColor(ChunkRebuildData *level, int x, int y, int z);
	virtual int getColor(ChunkRebuildData *level, int x, int y, int z, int data); // 4J added
	static Icon_SPU *getSideTextureOverlay();
};