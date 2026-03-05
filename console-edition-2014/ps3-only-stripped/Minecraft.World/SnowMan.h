#pragma once

#include "Golem.h"

class SnowMan : public Golem
{
public:
	eINSTANCEOF GetType() { return eTYPE_SNOWMAN; }
	static Entity *create(Level *level) { return new SnowMan(level); }

public:
	SnowMan(Level *level);
	virtual bool useNewAi();

	virtual int getMaxHealth();
	virtual void aiStep();

protected:
	virtual int getDeathLoot();
	virtual void dropDeathLoot(bool wasKilledByPlayer, int playerBonusLevel);
};