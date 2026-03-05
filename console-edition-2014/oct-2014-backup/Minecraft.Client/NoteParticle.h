#pragma once
#include "Particle.h"

class NoteParticle : public Particle
{
public:
	virtual eINSTANCEOF GetType() { return eType_NOTEPARTICLE; }
private:
	void init(Level *level, double x, double y, double z, double xa, double ya, double za, float scale);	// 4J - added
public:
    NoteParticle(Level *level, double x, double y, double z, double xa, double ya, double za);
    float oSize;
    NoteParticle(Level *level, double x, double y, double z, double xa, double ya, double za, float scale);
    virtual void render(Tesselator *t, float a, float xa, float ya, float za, float xa2, float za2);
    virtual void tick();
};
