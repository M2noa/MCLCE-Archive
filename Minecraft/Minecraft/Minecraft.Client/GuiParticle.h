#pragma once
class GuiParticles;
class Random;

class GuiParticle
{
private:
	static Random *random;

public:
	double x, y;
    double xo, yo;
    double xa, ya;
    double friction;
    bool removed;
    int life, lifeTime;

    double r, g, b, a;
    double oR, oG, oB, oA;	// MGH - remaned these, as PS3 complained about "or" var name

    GuiParticle(double x, double y, double xa, double ya);
    void tick(GuiParticles *guiParticles);
    void preTick();
    void remove();
};