#pragma once

#include "Particle.h"

class Level;
class Material;

class DripParticle : public Particle
{
private:
	Material *material;
	int stuckTime;

public:	
	virtual eINSTANCEOF GetType() { return eTYPE_DRIPPARTICLE; }

	DripParticle(Level *level, double x, double y, double z, Material *material);

	virtual int getLightColor(float a);
	virtual float getBrightness(float a);
	virtual void tick();
};