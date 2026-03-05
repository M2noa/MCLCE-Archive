#pragma once

#include "Particle.h"

class PlayerCloudParticle : public Particle
{
private:
	float oSize;

public:
	virtual eINSTANCEOF GetType() { return eType_PLAYERCLOUDPARTICLEPARTICLE; }
	PlayerCloudParticle(Level *level, double x, double y, double z, double xa, double ya, double za);
	void render(Tesselator *t, float a, float xa, float ya, float za, float xa2, float za2);
	void tick();
};