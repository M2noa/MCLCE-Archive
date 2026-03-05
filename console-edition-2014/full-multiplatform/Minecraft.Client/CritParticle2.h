#pragma once

#include "Particle.h"

class CritParticle2 : public Particle
{
public:	
	float oSize;
	bool m_bAgeUniformly; // 4J added for Halo texture pack

	virtual eINSTANCEOF GetType() { return eType_CRITPARTICLE2; }
	void _init(double xa, double ya, double za, float scale);
	CritParticle2(Level *level, double x, double y, double z, double xa, double ya, double za);
	CritParticle2(Level *level, double x, double y, double z, double xa, double ya, double za, float scale);
	void CritParticle2PostConstructor(void);
	void render(Tesselator *t, float a, float xa, float ya, float za, float xa2, float za2);
	void tick();
	void SetAgeUniformly();
};
