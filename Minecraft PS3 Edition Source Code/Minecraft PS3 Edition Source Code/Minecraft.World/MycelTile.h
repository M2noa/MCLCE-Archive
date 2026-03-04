#pragma once
#include "Tile.h"

class ChunkRebuildData;
class MycelTile : public Tile
{
	friend class ChunkRebuildData;
public:
	static const int MIN_BRIGHTNESS = 4;
	
private:
	Icon *iconTop;
    Icon *iconSnowSide;
public:
	MycelTile(int id);

	virtual Icon *getTexture(int face, int data);
    virtual Icon *getTexture(LevelSource *level, int x, int y, int z, int face);
	void registerIcons(IconRegister *iconRegister);
    virtual void tick(Level *level, int x, int y, int z, Random *random);
    virtual void animateTick(Level *level, int x, int y, int z, Random *random);
    virtual int getResource(int data, Random *random, int playerBonusLevel);
};
