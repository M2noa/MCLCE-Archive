#pragma once
#include "Particle.h"

class DragonBreathParticle : public Particle
{
public:
	virtual eINSTANCEOF GetType() { return eTYPE_DRAGONBREATHPARTICLE; }
private:
	bool m_bHasHitGround;
	void init(Level *level, double x, double y, double z, double xa, double ya, double za, float scale);	// 4J - added
public:
	DragonBreathParticle(Level *level, double x, double y, double z, double xa, double ya, double za);
    float oSize;

    DragonBreathParticle(Level *level, double x, double y, double z, double xa, double ya, double za, float scale);
    virtual void render(Tesselator *t, float a, float xa, float ya, float za, float xa2, float za2);
    virtual void tick();
	virtual int getParticleTexture();
	virtual float getBrightness(float a);
};