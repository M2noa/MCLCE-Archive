#pragma once
#include "Tile_SPU.h"
#include <math.h>

class Vec3_SPU
{
public:
	float x,y,z;
	Vec3_SPU(float xVal, float yVal, float zVal) :x(xVal), y(yVal), z(zVal) {} 
	Vec3_SPU normalize() 
	{
		float dist = sqrtf(x * x + y * y + z * z);
		if (dist < 0.0001) return Vec3_SPU(0, 0, 0);
		return Vec3_SPU(x / dist, y / dist, z / dist);
	}
	Vec3_SPU add(float x, float y, float z)
	{
		return Vec3_SPU(this->x+x, this->y+y, this->z+z);
	}

};


class LiquidTile_SPU : public Tile_SPU
{
public:

	LiquidTile_SPU(int id) : Tile_SPU(id) {}
public:
//	virtual int getColor() const;
    virtual int getColor(ChunkRebuildData *level, int x, int y, int z);
	virtual int getColor(ChunkRebuildData *level, int x, int y, int z, int data); // 4J added
    static float getHeight(int d);
	static double getSlopeAngle(ChunkRebuildData *level, int x, int y, int z, Material_SPU *m);
    virtual Icon_SPU *getTexture(int face, int data);
	virtual int getDepth(ChunkRebuildData *level, int x, int y, int z);
    virtual int getRenderedDepth(ChunkRebuildData *level, int x, int y, int z);
	virtual bool isSolidRender(bool isServerLevel = false);
	virtual bool isSolidFace(ChunkRebuildData *level, int x, int y, int z, int face);
	virtual bool shouldRenderFace(ChunkRebuildData *level, int x, int y, int z, int face);
	virtual int getRenderShape();
	virtual Vec3_SPU getFlow(ChunkRebuildData *level, int x, int y, int z);
	virtual int getRenderLayer();
	virtual int getLightColor(ChunkRebuildData *level, int x, int y, int z);	// 4J - brought forward from 1.8.2
	virtual float getBrightness(ChunkRebuildData *level, int x, int y, int z);
private:
};