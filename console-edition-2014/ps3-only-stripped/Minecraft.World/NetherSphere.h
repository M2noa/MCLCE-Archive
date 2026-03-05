#pragma once

#include "Entity.h"

class NetherSphere : public Entity
{
public:
	eINSTANCEOF GetType() { return eTYPE_NETHER_SPHERE; };
public:
	NetherSphere(Level *level);

protected:
	virtual void defineSynchedData();
	virtual void readAdditionalSaveData(CompoundTag *tag);
	virtual void addAdditonalSaveData(CompoundTag *tag);
};