#pragma once
#include "Feature.h"

class SpruceFeature : public Feature
{
public:
	SpruceFeature(bool doUpdate);
    virtual bool place(Level *level, Random *random, int x, int y, int z);
};
