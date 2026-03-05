#pragma once

#include "Particle.h"

class SpellParticle : public Particle
{
private:
	int baseTex;

public:
	virtual eINSTANCEOF GetType() { return eTYPE_SPELLPARTICLE; }

	SpellParticle(Level *level, double x, double y, double z, double xa, double ya, double za);

	virtual void render(Tesselator *t, float a, float xa, float ya, float za, float xa2, float za2);
	virtual void tick();
	virtual void setBaseTex(int baseTex);
};