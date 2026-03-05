#include "stdafx.h"
#include "Goal.h"

Goal::Goal()
{
	_requiredControlFlags = 0;
}

bool Goal::canContinueToUse()
{
	return canUse();
}

bool Goal::canInterrupt()
{
	return true;
}

void Goal::start()
{
}

void Goal::stop()
{
}

void Goal::tick()
{
}

void Goal::setRequiredControlFlags(int requiredControlFlags)
{
	_requiredControlFlags = requiredControlFlags;
}

int Goal::getRequiredControlFlags()
{
	return _requiredControlFlags;
}