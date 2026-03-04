#pragma once

#include "Goal.h"

class PathNavigation;
class PathfinderMob;
class Path;

class AvoidPlayerGoal : public Goal
{
private:
	PathfinderMob *mob; // Owner of this goal
	float walkSpeed, sprintSpeed;
	weak_ptr<Entity> toAvoid;
	float maxDist;
	Path *path;
	PathNavigation *pathNav;
	const type_info& avoidType;

public:
	AvoidPlayerGoal(PathfinderMob *mob, const type_info& avoidType, float maxDist, float walkSpeed, float sprintSpeed);
	~AvoidPlayerGoal();

	virtual bool canUse();
	virtual bool canContinueToUse();
	virtual void start();
	virtual void stop();
	virtual void tick();
};