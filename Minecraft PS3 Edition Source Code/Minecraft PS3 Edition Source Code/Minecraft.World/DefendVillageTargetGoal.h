#pragma once

#include "TargetGoal.h"

class VillagerGolem;

class DefendVillageTargetGoal : public TargetGoal
{
private:
	VillagerGolem *golem; // Owner of this goal
	weak_ptr<Mob> potentialTarget;

public:
	DefendVillageTargetGoal(VillagerGolem *golem);

	bool canUse();
	void start();
};