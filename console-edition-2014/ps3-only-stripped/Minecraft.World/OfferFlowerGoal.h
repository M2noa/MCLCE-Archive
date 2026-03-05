#pragma once

#include "Goal.h"

class VillagerGolem;

class OfferFlowerGoal : public Goal
{
public:
	static const int OFFER_TICKS = 400;

private:
	VillagerGolem *golem;
	weak_ptr<Villager> villager;
	int _tick;

public:
	OfferFlowerGoal(VillagerGolem *golem);

	virtual bool canUse();
	virtual bool canContinueToUse();
	virtual void start();
	virtual void stop();
	virtual void tick();
};