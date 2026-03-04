#pragma once
#include "Particle.h"

class BreakingItemParticle : public Particle
{
	// virtual eINSTANCEOF GetType(); // 4J-IB/JEV TODO needs implementation

public:
	virtual eINSTANCEOF GetType() { return eType_BREAKINGITEMPARTICLE; }
	void _init(Item *item, Textures *textures, int data);
	BreakingItemParticle(Level *level, double x, double y, double z, Item *item, Textures *textures, int data = 0);
	BreakingItemParticle(Level *level, double x, double y, double z, double xa, double ya, double za, Item *item, Textures *textures, int data = 0);
    virtual int getParticleTexture();
    virtual void render(Tesselator *t, float a, float xa, float ya, float za, float xa2, float za2);
};
