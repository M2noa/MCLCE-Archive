#include "stdafx.h"

#include "Golem.h"



Golem::Golem(Level *level) : PathfinderMob(level)
{
}


void Golem::causeFallDamage(float distance)
{
}

int Golem::getAmbientSound()
{
	return -1;
}

int Golem::getHurtSound()
{
	return -1;
}

int Golem::getDeathSound()
{
	return -1;
}

int Golem::getAmbientSoundInterval()
{
	return 20 * 6;
}

bool Golem::removeWhenFarAway()
{
	return false;
}