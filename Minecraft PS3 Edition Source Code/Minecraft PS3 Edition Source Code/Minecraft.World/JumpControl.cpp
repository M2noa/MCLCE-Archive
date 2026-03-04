#include "stdafx.h"
#include "net.minecraft.world.entity.h"
#include "JumpControl.h"

JumpControl::JumpControl(Mob *mob)
{
	_jump = false;

	this->mob = mob;
}

void JumpControl::jump()
{
	_jump = true;
}

void JumpControl::tick()
{
	mob->setJumping(_jump);
	_jump = false;
}