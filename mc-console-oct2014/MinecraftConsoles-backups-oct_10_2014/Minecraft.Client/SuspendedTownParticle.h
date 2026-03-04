#pragma once

#include "Particle.h"

class SuspendedTownParticle : public Particle
{
public:
	virtual eINSTANCEOF GetType() { return eType_SUSPENDEDTOWNPARTICLE; }
	SuspendedTownParticle(Level *level, double x, double y, double z, double xa, double ya, double za);
	void tick();
};