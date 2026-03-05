#include "stdafx.h"
#include "net.minecraft.world.entity.ai.navigation.h"
#include "net.minecraft.world.entity.h"
#include "net.minecraft.world.level.h"
#include "RestrictSunGoal.h"

RestrictSunGoal::RestrictSunGoal(PathfinderMob *mob)
{
	this->mob = mob;
}

bool RestrictSunGoal::canUse()
{
	return mob->level->isDay();
}

void RestrictSunGoal::start()
{
	mob->getNavigation()->setAvoidSun(true);
}

void RestrictSunGoal::stop()
{
	mob->getNavigation()->setAvoidSun(false);
}