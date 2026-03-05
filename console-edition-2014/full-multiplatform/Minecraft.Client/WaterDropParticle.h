#pragma once
#include "Particle.h"

class WaterDropParticle : public Particle
{
public:
	virtual eINSTANCEOF GetType() { return eType_WATERDROPPARTICLE; }
	WaterDropParticle(Level *level, double x, double y, double z) ;
    virtual void tick();
};