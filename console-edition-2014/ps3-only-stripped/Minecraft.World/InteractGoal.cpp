#include "stdafx.h"
#include "net.minecraft.world.entity.ai.control.h"
#include "InteractGoal.h"

InteractGoal::InteractGoal(Mob *mob, const type_info& lookAtType, float lookDistance) : LookAtPlayerGoal(mob, lookAtType, lookDistance)
{
	setRequiredControlFlags(Control::LookControlFlag | Control::MoveControlFlag);
}

InteractGoal::InteractGoal(Mob *mob, const type_info& lookAtType, float lookDistance, float probability) : LookAtPlayerGoal(mob, lookAtType, lookDistance, probability)
{
	setRequiredControlFlags(Control::LookControlFlag | Control::MoveControlFlag);
}