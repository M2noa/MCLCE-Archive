#pragma once
#include "ChunkSource.h"

class Level;

class LargeFeature
{
public:
	static const wstring STRONGHOLD;
protected:
    int radius;
    Random *random;
	Level *level;

public:
	LargeFeature();
	~LargeFeature();

    virtual void apply(ChunkSource *ChunkSource, Level *level, int xOffs, int zOffs, byteArray blocks);

protected:
	virtual void addFeature(Level *level, int x, int z, int xOffs, int zOffs, byteArray blocks) {}
};