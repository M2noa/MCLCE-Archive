#pragma once
#include "Particle.h"

class BubbleParticle : public Particle
{
public:
	virtual eINSTANCEOF GetType() { return eType_BUBBLEPARTICLE; }
	BubbleParticle(Level *level, double x, double y, double z, double xa, double ya, double za);
	virtual void tick();
};