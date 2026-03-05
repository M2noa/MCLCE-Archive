#pragma once
#include "Particle.h"

// 4J Stu - This class was originally "PortalParticle" but I have split the two uses of the particle
// End creatures/items (e.g. EnderMan, EyeOfEnder, etc) use this particle

class EnderParticle : public Particle
{
public:
	virtual eINSTANCEOF GetType() { return eType_ENDERPARTICLE; }
private:
	float oSize;
     double xStart, yStart, zStart;

public:
    EnderParticle(Level *level, double x, double y, double z, double xd, double yd, double zd);
    virtual void render(Tesselator *t, float a, float xa, float ya, float za, float xa2, float za2);
	virtual int getLightColor(float a);	// 4J - brought forward from 1.8.2
    virtual float getBrightness(float a);
    virtual void tick();
};