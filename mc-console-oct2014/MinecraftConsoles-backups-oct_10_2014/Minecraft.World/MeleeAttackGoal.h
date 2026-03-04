#pragma once

#include "Goal.h"

class Level;
class Mob;
class Path;

class MeleeAttackGoal : public Goal
{
private:
	Level *level;
	Mob *mob; // Owner of this goal
	weak_ptr<Mob> target;

	int attackTime;
	float speed;
	bool trackTarget;
	Path *path;
	eINSTANCEOF attackType;
	int timeToRecalcPath;

	void _init(Mob *mob, float speed, bool trackTarget);

public:
	MeleeAttackGoal(Mob *mob, eINSTANCEOF attackType, float speed, bool trackTarget);
	MeleeAttackGoal(Mob *mob, float speed, bool trackTarget);
	~MeleeAttackGoal();

	virtual bool canUse();
	virtual bool canContinueToUse();
	virtual void start();
	virtual void stop();
	virtual void tick();
	
	// 4J Added override to update ai elements when loading entity from schematics
	virtual void setLevel(Level *level) { this->level = level; }
};