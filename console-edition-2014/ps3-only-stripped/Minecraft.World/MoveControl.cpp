#include "stdafx.h"
#include "net.minecraft.world.entity.ai.control.h"
#include "net.minecraft.world.entity.h"
#include "net.minecraft.world.phys.h"
#include "MoveControl.h"

const float MoveControl::MIN_SPEED = 0.0005f;
const float MoveControl::MIN_SPEED_SQR = MIN_SPEED * MIN_SPEED;

MoveControl::MoveControl(Mob *mob)
{
	this->mob = mob;
	wantedX = mob->x;
	wantedY = mob->y;
	wantedZ = mob->z;

	speed = 0.0f;

	_hasWanted = false;
}

bool MoveControl::hasWanted()
{
	return _hasWanted;
}

float MoveControl::getSpeed()
{
	return speed;
}

void MoveControl::setWantedPosition(double x, double y, double z, float speed)
{
	wantedX = x;
	wantedY = y;
	wantedZ = z;
	this->speed = speed;
	_hasWanted = true;
}

void MoveControl::tick()
{
	mob->setYya(0);
	if (!_hasWanted) return;
	_hasWanted = false;

	int yFloor = floor(mob->bb->y0 + .5f);

	double xd = wantedX - mob->x;
	double zd = wantedZ - mob->z;
	double yd = wantedY - yFloor;
	double dd = xd * xd + yd * yd + zd * zd;
	if (dd < MIN_SPEED_SQR) return;

	float yRotD = (float) (atan2(zd, xd) * 180 / PI) - 90;

	mob->yRot = rotlerp(mob->yRot, yRotD, MAX_TURN);
	mob->setSpeed(speed * mob->getWalkingSpeedModifier());

	if (yd > 0 && xd * xd + zd * zd < 1) mob->getJumpControl()->jump();
}

float MoveControl::rotlerp(float a, float b, float max)
{
	float diff = Mth::wrapDegrees(b - a);
	if (diff > max)
	{
		diff = max;
	}
	if (diff < -max)
	{
		diff = -max;
	}
	return a + diff;
}