#include "stdafx.h"
#include "NetherSphere.h"



NetherSphere::NetherSphere(Level *level) : Entity(level)
{
	// 4J Stu - This function call had to be moved here from the Entity ctor to ensure that
	// the derived version of the function is called
	this->defineSynchedData();

	setSize(4, 4);
}


void NetherSphere::defineSynchedData()
{
}

void NetherSphere::readAdditionalSaveData(CompoundTag *tag)
{
}

void NetherSphere::addAdditonalSaveData(CompoundTag *tag)
{
}