#pragma once

#include "Biome.h"

class ExtremeHillsBiome : public Biome
{
	friend class Biome;
private:
	static const bool GENERATE_EMERALD_ORE = true;

protected:
	ExtremeHillsBiome(int id);

public:
	void decorate(Level *level, Random *random, int xo, int zo);
};