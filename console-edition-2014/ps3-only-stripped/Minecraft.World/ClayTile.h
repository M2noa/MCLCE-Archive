#pragma once
#include "Tile.h"
#include "Material.h"

class Random;

class ClayTile : public Tile
{
public:
    ClayTile(int id);
    virtual int getResource(int data, Random *random, int playerBonusLevel);
    virtual int getResourceCount(Random *random);
};