#pragma once

#include "Control.h"

class BodyControl : public Control
{
private:
	Mob *mob;
	static const float maxClampAngle;
	int timeStill;
	float lastHeadY;

public:
	BodyControl(Mob *mob);

	void clientTick();

private:
	float clamp(float clampTo, float clampFrom, float clampAngle);
};