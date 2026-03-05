#pragma once
#include "Particle.h"

class FlameParticle : public Particle
{
public:
	virtual eINSTANCEOF GetType() { return eType_FLAMEPARTICLE; }
private:
	float oSize;
    
public:
	FlameParticle(Level *level, double x, double y, double z, double xd, double yd, double zd);
    virtual void render(Tesselator *t, float a, float xa, float ya, float za, float xa2, float za2);
	virtual int getLightColor(float a);	// 4J - brought forward from 1.8.2
    virtual float getBrightness(float a);
    virtual void tick();
};