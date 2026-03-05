#include "Dungeons.h"

#include "Abilities.h"
#include "nbt/CompoundTag.h"

Abilities::Abilities()
	: mInvulnerable(false)
	, mFlying(false)
	, mMayfly(false)
	, mInstabuild(false)
	, mNoclip(false)
	, mLightning(false)
	, mWalkingSpeed(0.1f)
	, mFlyingSpeed(0.05f)
	, mWorldBuilder(false)
	//, mPermissions(std::make_unique<PermissionsHandler>())
{
}

void Abilities::addSaveData(CompoundTag& parentTag) const {
	auto tag = make_unique<CompoundTag>();

	tag->putBoolean("invulnerable", mInvulnerable);
	tag->putBoolean("flying", mFlying);
	tag->putBoolean("mayfly", mMayfly);
	tag->putBoolean("instabuild", mInstabuild);
	tag->putBoolean("lightning", mLightning);
	tag->putFloat("flySpeed", mFlyingSpeed);
	tag->putFloat("walkSpeed", mWalkingSpeed);
	//mPermissions->addSaveData(*tag);
	tag->putBoolean("worldBuilder", mWorldBuilder);

	parentTag.put("abilities", std::move(tag));
}

void Abilities::loadSaveData(const CompoundTag& parentTag){
	if (parentTag.contains("abilities", Tag::Type::Compound)) {
		auto& tag = *parentTag.getCompound("abilities");

		mInvulnerable = tag.getBoolean("invulnerable");
		mFlying = tag.getBoolean("flying");
		mMayfly = tag.getBoolean("mayfly");
		mInstabuild = tag.getBoolean("instabuild");
		mLightning = tag.getBoolean("lightning");

		if (tag.contains("flySpeed", Tag::Type::Float)) {
			mFlyingSpeed = tag.getFloat("flySpeed");
			mWalkingSpeed = tag.getFloat("walkSpeed");
		}

		if (tag.contains("worldBuilder")) {
			mWorldBuilder = tag.getBoolean("worldBuilder");
		}

		//mPermissions->loadSaveData(tag);
	}
}

bool Abilities::isFlying() const {
	return mFlying || mNoclip;
}

float Abilities::getFlyingSpeed() const {
	return mFlyingSpeed;
}

void Abilities::setFlyingSpeed(float value){
	mFlyingSpeed = value;
}

float Abilities::getWalkingSpeed() const {
	return mWalkingSpeed;
}

void Abilities::setWalkingSpeed(float value){
	mWalkingSpeed = value;
}


bool Abilities::isWorldBuilder() const {
	return mWorldBuilder;
}

void Abilities::setWorldBuilder(bool isWorldBuilder) {
	mWorldBuilder = isWorldBuilder;
}

//CommandPermissionLevel Abilities::getCommandPermissions() const {
//	return mPermissions->getCommandPermissions();
//}
//
//void Abilities::setCommandPermissions(CommandPermissionLevel permissions) {
//	mPermissions->setCommandPermissions(permissions);
//}
