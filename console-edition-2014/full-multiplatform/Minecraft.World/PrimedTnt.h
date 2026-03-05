#pragma once
#include "Entity.h"

class PrimedTnt : public Entity
{
public:
	eINSTANCEOF GetType() { return eTYPE_PRIMEDTNT; };
	static Entity *create(Level *level) { return new PrimedTnt(level); }

public:
	static const int serialVersionUID = 0;

	int life;

	void _init();

	PrimedTnt(Level *level);
	PrimedTnt(Level *level, double x, double y, double z);

protected:
	virtual void defineSynchedData();
	virtual bool makeStepSound();

public:
	virtual bool isPickable();
	virtual void tick();

private:
	void explode();

protected:
	virtual void addAdditonalSaveData(CompoundTag *entityTag);
	virtual void readAdditionalSaveData(CompoundTag *tag);

public:
	virtual float getShadowHeightOffs();
};
