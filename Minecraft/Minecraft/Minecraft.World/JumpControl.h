#pragma once

#include "Control.h"

class Mob;

class JumpControl : public Control
{
private:
	Mob *mob;
	bool _jump;

public:
	JumpControl(Mob *mob);

	void jump();
	virtual void tick();
};