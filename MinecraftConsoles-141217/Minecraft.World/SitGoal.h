#pragma once

#include "Goal.h"

class SitGoal : public Goal
{
private:
	TamableAnimal *mob;
	bool _wantToSit;

public:
	SitGoal(TamableAnimal *mob);

	bool canUse();
	void start();
	void stop();
	void wantToSit(bool _wantToSit);
};