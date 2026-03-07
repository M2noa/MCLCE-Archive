#pragma once
#include "Tile.h"

class Level;
class ChunkRebuildData;
class GrassTile : public Tile
{
	friend class Tile;
	friend class ChunkRebuildData;
private:
	Icon *iconTop;
	Icon *iconSnowSide;
	Icon *iconSideOverlay;
public:
	static const int MIN_BRIGHTNESS = 4;

protected:
	GrassTile(int id);
public:
	virtual Icon *getTexture(int face, int data);
	virtual Icon *getTexture(LevelSource *level, int x, int y, int z, int face);
	void registerIcons(IconRegister *iconRegister);
    virtual int getColor() const;
	virtual int getColor(int auxData);
    virtual int getColor(LevelSource *level, int x, int y, int z);
	virtual int getColor(LevelSource *level, int x, int y, int z, int data); // 4J added
    virtual void tick(Level *level, int x, int y, int z, Random *random);
    virtual int getResource(int data, Random *random, int playerBonusLevel);
	static Icon *getSideTextureOverlay();
	
	// 4J Added so we can check before we try to add a tile to the tick list if it's actually going to do seomthing
	virtual bool shouldTileTick(Level *level, int x,int y,int z);
};