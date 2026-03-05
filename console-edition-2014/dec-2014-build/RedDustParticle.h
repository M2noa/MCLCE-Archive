#pragma once
#include "Particle.h"

class RedDustParticle : public Particle
{
public:
	virtual eINSTANCEOF GetType() { return eType_REDDUSTPARTICLE; }
private:
	void init(Level *level, double x, double y, double z, float scale, float rCol, float gCol, float bCol);	// 4J - added
public:
	RedDustParticle(Level *level, double x, double y, double z, float rCol, float gCol, float bCol);
    float oSize;

    RedDustParticle(Level *level, double x, double y, double z, float scale, float rCol, float gCol, float bCol);
    virtual void render(Tesselator *t, float a, float xa, float ya, float za, float xa2, float za2);
    virtual void tick();
};