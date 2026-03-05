#pragma once
#include "Biome.h"
class LevelSource;

class SwampBiome : public Biome
{
	// 4J Stu - No idea why this is protected in Java
//protected:
public:
	SwampBiome(int id);

public:
	virtual Feature *getTreeFeature(Random *random);

	// 4J Stu - Not using these any more
    //virtual int getGrassColor();
    //virtual int getFolageColor();
};