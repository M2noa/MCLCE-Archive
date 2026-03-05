#pragma once

#include "Mob.h"

class Level;

class FlyingMob : public Mob
{
public:
	FlyingMob(Level *level);

protected:
	virtual void causeFallDamage(float distance);

public:
	virtual void travel(float xa, float ya);
	virtual bool onLadder();
};