#pragma once

#include "Particle.h"

class HugeExplosionSeedParticle : public Particle
{
private:
	int life;
	int lifeTime;

public:
	virtual eINSTANCEOF GetType() { return eType_HUGEEXPLOSIONSEEDPARTICLE; }
	HugeExplosionSeedParticle(Level *level, double x, double y, double z, double xa, double ya, double za);
	void render(Tesselator *t, float a, float xa, float ya, float za, float xa2, float za2);
	void tick();
	int getParticleTexture();
};