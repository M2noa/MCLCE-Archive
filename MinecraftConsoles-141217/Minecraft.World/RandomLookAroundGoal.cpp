#include "stdafx.h"
#include "net.minecraft.world.entity.h"
#include "net.minecraft.world.entity.ai.control.h"
#include "RandomLookAroundGoal.h"

RandomLookAroundGoal::RandomLookAroundGoal(Mob *mob)
{
	relX = relZ = 0.0;
	lookTime = 0;

	this->mob = mob;
	setRequiredControlFlags(Control::MoveControlFlag | Control::LookControlFlag);
}

bool RandomLookAroundGoal::canUse()
{
	return mob->getRandom()->nextFloat() < 0.02f;
}

bool RandomLookAroundGoal::canContinueToUse()
{
	return lookTime >= 0;
}

void RandomLookAroundGoal::start()
{
	double rnd = 2 * PI * mob->getRandom()->nextDouble();
	relX = cos(rnd);
	relZ = sin(rnd);
	lookTime = 20 + mob->getRandom()->nextInt(20);
}

void RandomLookAroundGoal::tick()
{
	--lookTime;
	mob->getLookControl()->setLookAt(mob->x + relX, mob->y + mob->getHeadHeight(), mob->z + relZ, 10, mob->getMaxHeadXRot());
}