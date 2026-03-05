/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

//#include "world/entity/player/PermissionsHandler.h"

class CompoundTag;

class Abilities {
public:

	Abilities();

	void addSaveData(CompoundTag& parentTag) const;
	void loadSaveData(const CompoundTag& parentTag);

	bool isFlying() const;

	float getFlyingSpeed() const;
	void setFlyingSpeed(float value);

	float getWalkingSpeed() const;
	void setWalkingSpeed(float value);

	bool isWorldBuilder() const;
	void setWorldBuilder(bool isWorldBuilder);

	//CommandPermissionLevel getCommandPermissions() const;
	//void setCommandPermissions(CommandPermissionLevel permissions);

	//Stock abilities
	bool mInvulnerable;
	bool mFlying;
	bool mMayfly;
	bool mInstabuild;
	bool mNoclip;

	//Holoviewer mode abilities
	bool mLightning;
private:

	float mWalkingSpeed;
	float mFlyingSpeed;
	bool mWorldBuilder;
	//std::unique_ptr<PermissionsHandler> mPermissions;
};
