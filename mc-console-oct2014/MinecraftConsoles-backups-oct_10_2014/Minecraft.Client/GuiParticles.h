#pragma once
#include "GuiComponent.h"

class GuiParticle;
class Minecraft;
using namespace std;

class GuiParticles : public GuiComponent
{
private:
	vector<GuiParticle *> particles;
    Minecraft *mc;

public:
	GuiParticles(Minecraft *mc);
    void tick();
    void add(GuiParticle *guiParticle);
    void render(float a);
};
