#pragma once

#include "Entity.h"

class EyeOfEnderSignal : public Entity
{
public:
	eINSTANCEOF GetType() {	return eTYPE_EYEOFENDERSIGNAL; }
	static Entity *create(Level *level) { return new EyeOfEnderSignal(level); }

public:
	int shakeTime;

private:
	double tx, ty, tz;
	int life;
	bool surviveAfterDeath;

	void _init();

public:
	EyeOfEnderSignal(Level *level);

protected:
	virtual void defineSynchedData();

public:
	virtual bool shouldRenderAtSqrDistance(double distance);

	EyeOfEnderSignal(Level *level, double x, double y, double z);

	void signalTo(double tx, int ty, double tz);
	virtual void lerpMotion(double xd, double yd, double zd);
	virtual void tick();
	virtual void addAdditonalSaveData(CompoundTag *tag);
	virtual void readAdditionalSaveData(CompoundTag *tag);
	virtual float getShadowHeightOffs();
	virtual float getBrightness(float a);
	virtual int getLightColor(float a);
	virtual bool isAttackable();
};