#pragma once

#include "Particle.h"

class SuspendedParticle : public Particle
{
public:
	virtual eINSTANCEOF GetType() { return eType_SUSPENDEDPARTICLE; }
	SuspendedParticle(Level *level, double x, double y, double z, double xa, double ya, double za);
	void tick();
};