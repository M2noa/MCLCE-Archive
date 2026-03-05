#pragma once
#include "EntityTile_SPU.h"

class Random;

class MobSpawnerTile_SPU : public EntityTile_SPU
{
public:
	MobSpawnerTile_SPU(int id) : EntityTile_SPU(id) {}
    virtual bool isSolidRender(bool isServerLevel = false) { return false; }
    virtual bool blocksLight() { return false; }
};