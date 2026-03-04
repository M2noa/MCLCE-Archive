#include "stdafx.h"
#include "HeartParticle.h"

// 4J - added
void HeartParticle::init(Level *level, double x, double y, double z, double xa, double ya, double za, float scale)
{
    xd *= 0.01f;
    yd *= 0.01f;
    zd *= 0.01f;
    yd += 0.1;

    size *= 0.75f;
    size *= scale;
    oSize = size;

    lifetime = 16;
    noPhysics = false;


    setMiscTex(16 * 5);
}

HeartParticle::HeartParticle(Level *level, double x, double y, double z, double xa, double ya, double za) : Particle(level, x, y, z, 0, 0, 0)
{
	init(level, x, y, z, xa, ya, za, 2);
}

HeartParticle::HeartParticle(Level *level, double x, double y, double z, double xa, double ya, double za, float scale) : Particle(level, x, y, z, 0, 0, 0)
{
	init(level,x,y,z,xa,ya,za,scale);
}

void HeartParticle::render(Tesselator *t, float a, float xa, float ya, float za, float xa2, float za2)
{
    float l = ((age + a) / lifetime) * 32;
    if (l < 0) l = 0;
    if (l > 1) l = 1;

    size = oSize * l;
    Particle::render(t, a, xa, ya, za, xa2, za2);
}

void HeartParticle::tick()
{
    xo = x;
    yo = y;
    zo = z;

    if (age++ >= lifetime) remove();

    move(xd, yd, zd);
    if (y == yo)
	{
        xd *= 1.1;
        zd *= 1.1;
    }
    xd *= 0.86f;
    yd *= 0.86f;
    zd *= 0.86f;

    if (onGround)
	{
        xd *= 0.7f;
        zd *= 0.7f;
    }
}