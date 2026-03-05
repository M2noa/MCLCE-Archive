#pragma once

class CompoundTag;

class Abilities
{
public:
	bool invulnerable;
	bool flying;
	bool mayfly;
	bool instabuild;
	bool mayBuild;

private:
	float flyingSpeed;
	float walkingSpeed;

public:
#ifdef _DEBUG_MENUS_ENABLED
	bool debugflying;
#endif

	Abilities();

	void addSaveData(CompoundTag *parentTag);
	void loadSaveData(CompoundTag *parentTag);

	float getFlyingSpeed();
	void setFlyingSpeed(float value);
	float getWalkingSpeed();
	void setWalkingSpeed(float value);
};