#pragma once
#include "Particle.h"

class LavaParticle : public Particle
{
public:
	virtual eINSTANCEOF GetType() { return eType_LAVAPARTICLE; }
private:
	float oSize;
public:
	LavaParticle(Level *level, double x, double y, double z);
	virtual int getLightColor(float a);	 // 4J - brought forward from 1.8.2
    virtual float getBrightness(float a);
    virtual void render(Tesselator *t, float a, float xa, float ya, float za, float xa2, float za2);
    virtual void tick();
};
