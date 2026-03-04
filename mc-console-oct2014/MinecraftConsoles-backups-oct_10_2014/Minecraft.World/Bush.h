#pragma once
#include "Tile.h"
#include "Material.h"
#include "Definitions.h"

class Random;
class Level;

class Bush : public Tile
{
	friend class Tile;

private:
	void _init();

protected:
	Bush(int id, Material *material);
	Bush(int id);

public:
	virtual void updateDefaultShape();
	virtual bool mayPlace(Level *level, int x, int y, int z);

protected:
	virtual bool mayPlaceOn(int tile);

public:
	virtual void neighborChanged(Level *level, int x, int y, int z, int type);
    virtual void tick(Level *level, int x, int y, int z, Random *random);

protected:
	void checkAlive(Level *level, int x, int y, int z);

public:
	virtual bool canSurvive(Level *level, int x, int y, int z);
    virtual AABB *getAABB(Level *level, int x, int y, int z);
    virtual bool blocksLight();

    virtual bool isSolidRender(bool isServerLevel = false);
    virtual bool isCubeShaped();
    virtual int getRenderShape();
};
