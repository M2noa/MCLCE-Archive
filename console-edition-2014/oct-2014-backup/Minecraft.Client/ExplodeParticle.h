#pragma once
#include "Particle.h"

class ExplodeParticle : public Particle
{
public:
	virtual eINSTANCEOF GetType() { return eType_EXPLODEPARTICLE; }
	ExplodeParticle(Level *level, double x, double y, double z, double xa, double ya, double za);
	virtual void render(Tesselator *t, float a, float xa, float ya, float za, float xa2, float za2);
	virtual void tick();
};