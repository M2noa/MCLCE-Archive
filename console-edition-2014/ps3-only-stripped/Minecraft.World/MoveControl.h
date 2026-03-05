#pragma once

#include "Control.h"

class Mob;

class MoveControl : public Control
{
public:
	static const float MIN_SPEED;
	static const float MIN_SPEED_SQR;

private:
	static const int MAX_TURN = 30;

	Mob *mob;
	double wantedX;
	double wantedY;
	double wantedZ;
	float speed;
	bool _hasWanted;

public:
	MoveControl(Mob *mob);

	bool hasWanted();
	float getSpeed();
	void setWantedPosition(double x, double y, double z, float speed);
	void setSpeed(float speed);
	virtual void tick();

private:
	float rotlerp(float a, float b, float max);
};