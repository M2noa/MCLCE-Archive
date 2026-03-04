#include "stdafx.h"
#include "net.minecraft.world.entity.h"
#include "net.minecraft.world.entity.ai.control.h"
#include "net.minecraft.world.entity.ai.navigation.h"
#include "FloatGoal.h"

FloatGoal::FloatGoal(Mob *mob)
{
	this->mob = mob;
	setRequiredControlFlags(Control::JumpControlFlag);
	mob->getNavigation()->setCanFloat(true);
}

bool FloatGoal::canUse()
{
	return (mob->isInWater() || mob->isInLava());
}

void FloatGoal::tick()
{
	if (mob->getRandom()->nextFloat() < 0.8f) mob->getJumpControl()->jump();
}