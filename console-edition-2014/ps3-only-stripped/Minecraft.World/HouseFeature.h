#pragma once
#include "Feature.h"
#include "Material.h"

class HouseFeature : public Feature
{
public:
    virtual bool place(Level *level, Random *random, int x, int y, int z);
};