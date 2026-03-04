#pragma once

#include "Biome.h"

class PlainsBiome : public Biome
{
	friend class Biome;
protected:
	PlainsBiome(int id);
};