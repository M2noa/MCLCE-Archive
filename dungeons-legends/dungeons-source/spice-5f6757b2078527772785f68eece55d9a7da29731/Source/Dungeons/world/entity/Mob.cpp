/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/entity/Mob.h"
//#include "world/entity/item/ItemEntity.h"
//#include "world/item/ArmorItem.h"
//#include "world/entity/EntityEvent.h"
#include "world/level/Level.h"
#include "world/level/material/Material.h"
//#include "world/effect/MobEffect.h"
//#include "util/PerfTimer.h"
//#include "world/entity/ai/control/MoveControl.h"
//#include "world/entity/ai/control/JumpControl.h"
//#include "world/entity/ai/control/LookControl.h"
//#include "world/entity/ai/control/BodyControl.h"
//#include "world/entity/ai/navigation/PathNavigation.h"
//#include "world/entity/ai/Sensing.h"
//#include "world/entity/item/ExperienceOrb.h"
//#include "world/entity/EntityDefinition.h"
//#include "world/attribute/Attribute.h"
//#include "world/attribute/SharedAttributes.h"
//#include "world/attribute/HealthAttributeDelegate.h"
//#include "world/attribute/AttributeMap.h"
//#include "world/attribute/AttributeInstance.h"
//#include "network/packet/SetEntityDataPacket.h"
//#include "network/packet/EntityEventPacket.h"
//#include "network/packet/EventPacket.h"
//#include "network/PacketSender.h"
//#include "network/packet/UpdateAttributesPacket.h"
//#include "EntityClassTree.h"
//#include "world/effect/MobEffectInstance.h"
//#include "Events/MinecraftEventing.h"
//#include "world/level/Spawner.h"
//
////Components
//#include "world/entity/components/AngryComponent.h"
//#include "world/entity/components/BreedableComponent.h"
//#include "world/entity/components/BreathableComponent.h"
//#include "world/entity/components/LookAtComponent.h"
//#include "world/entity/components/TeleportComponent.h"
//#include "world/entity/components/RideableComponent.h"
//#include "world/entity/components/ShooterComponent.h"
//#include "world/entity/components/HealableComponent.h"
//#include "world/entity/components/BoostableComponent.h"
//#include "world/entity/components/LeashableComponent.h"
//#include "world/entity/components/TransformationComponent.h"
//#include "world/entity/components/DamageSensorComponent.h"
//#include "world/entity/components/InteractComponent.h"
//#include "world/entity/components/PeekComponent.h"
//#include "world/entity/components/AgentCommandComponent.h"
//
//#include "world/level/storage/loot/LootTables.h"
//#include "world/level/storage/loot/LootTableContext.h"
//#include "world/level/storage/loot/LootTable.h"
//
//#include "client/renderer/block/BlockGraphics.h"
//#include "network/packet/PlayerActionPacket.h"

static const float GLIDING_FALL_RESET_DELTA = -0.5f;
const int Mob::PLAYER_HURT_EXPERIENCE_TIME = SharedConstants::TicksPerSecond * 5;

#include "util/EntityUtil.h"

//static void _defineMobData(SynchedEntityData &data) {
//	data.define(enum_cast(EntityDataIDs::EFFECT_COLOR), (SynchedEntityData::TypeInt) 0);
//	data.define(enum_cast(EntityDataIDs::EFFECT_AMBIENCE), (SynchedEntityData::TypeInt8) 0);
//	data.define(enum_cast(EntityDataIDs::LEASH_HOLDER), (SynchedEntityData::TypeInt64) 0);
//}
//
//Mob::Mob(EntityDefinitionGroup& definitions, const EntityDefinitionIdentifier& definitionName)
//	: Entity(definitions, definitionName) {
//	mCategories |= EntityCategory::Mob;
//
//	enableAutoSendPosRot(true);
//
//	_defineMobData(mEntityData);
//	mAttributes = make_unique<BaseAttributeMap>();
//	registerAttributes();
//
//	mLookControl = make_unique<LookControl>(*this);
//	mMoveControl = make_unique<MoveControl>(*this);
//	mJumpControl = make_unique<JumpControl>(*this);
//	mBodyControl = make_unique<BodyControl>(*this);
//	mNavigation = make_unique<PathNavigation>(*this, 16.f);
//	mSensing = make_unique<Sensing>(*this);
//
//	//Setting this value to be the size of 1/2 a block + carpet, which is 1/16th of a block thick.
//	mMaxAutoStep = 0.5f + (1.0f / 16.0f);
//}

// Called only for Player
Mob::Mob(Level& level) : Entity(level) {
	//mCategories |= EntityCategory::Mob;

	//enableAutoSendPosRot(true);

	//_defineMobData(mEntityData);
	//mAttributes = make_unique<BaseAttributeMap>();
	//registerAttributes();

	//mLookControl = make_unique<LookControl>(*this);
	//mMoveControl = make_unique<MoveControl>(*this);
	//mJumpControl = make_unique<JumpControl>(*this);
	//if (!mBodyControl) {
	//	mBodyControl = make_unique<BodyControl>(*this);
	//}
	//mNavigation = make_unique<PathNavigation>(*this, 16.f);
	//mSensing = make_unique<Sensing>(*this);

	//mMaxAutoStep = 0.5f + (1.0f / 16.0f);

	//mOTilt = mTilt = 0.0f;

}

//void Mob::reloadHardcodedClient(InitializationMethod method, const VariantParameterList &params) {
//	// Used for clients to determine if their bounding box prevents construction of blocks
//	mBlocksBuilding = true;
//}
//
//void Mob::initializeComponents(InitializationMethod method, const VariantParameterList &params) {
//	Entity::initializeComponents(method, params);
//
//	// First-time initialization
//	if (!mInitialized) {
//
//		mYBodyRot = 0;
//		mYBodyRotO = 0;
//		mFallTime = 0;
//		mLastHurt = 0;
//		mDeathTime = 0;
//		mAttackTime = 0;
//		mTilt = 0;
//		mOTilt = 0;
//		mHurtDuration = 0;
//		mHurtTime = 0;
//		mHurtDir = 0;
//		mLookTime = 0;
//		mNoActionTime = 0;
//		mXxa = 0;
//		mYya = 0;
//		mYRotA = 0;
//		mFrictionModifier = 1.0f;
//		mLastHealth = 20;
//
//		mBlocksBuilding = true;
//
//		mCanPickUpLoot = false;
//	
//		for (int i = 0, count = static_cast<int>(HandSlot::_count); i < count; i++) {
//			mHandDropChance[i] = 0.085f;
//		}
//		for (int i = 0, count = static_cast<int>(ArmorSlot::_count); i < count; i++) {
//			mArmorDropChance[i] = 0.085f;
//		}
//
//	}
//
//	// Calculate the difference between this definition and our last
//	EntityDefinitionDescriptor& changedDescription = mDefinitionList.getChangedDescription();
//
//	if (method != InitializationMethod::LOADED && method != InitializationMethod::LEGACY && method != InitializationMethod::INVALID) {
//		for (auto& a : changedDescription.mAttributes) {
//			AttributeInstance& inst = mAttributes->registerAttribute(Attribute::getByName(a.name));
//
//			if (inst.getAttribute() == nullptr) {
//				continue;
//			}
//
//			float originalMin = inst.getDefaultValue(enum_cast(AttributeOperands::OPERAND_MIN));
//			float originalMax = inst.getDefaultValue(enum_cast(AttributeOperands::OPERAND_MAX));
//			float currentMax = inst.getMaxValue();
//
//			// New min and max
//			float min = (a.min != std::numeric_limits<float>::min()) ? a.min : originalMin;
//			float max = (a.max != std::numeric_limits<float>::max()) ? a.max : originalMax;
//			if (max < min) {
//				std::swap(min, max);
//			}
//
//			inst.setDefaultValue(min, (int)AttributeOperands::OPERAND_MIN);
//			inst.setDefaultValue(max, (int)AttributeOperands::OPERAND_MAX);
//
//			// Choose a new default value
//			float chosenValue = a.value.getValue(mRandom);
//			chosenValue = Math::clamp(chosenValue, min, max);
//
//			// Special case for health setting it's max value to the generated default
//			if (inst.getAttribute() == &SharedAttributes::HEALTH) {
//				max = chosenValue;
//				inst.setDefaultValue(max, (int)AttributeOperands::OPERAND_MAX);
//			}
//
//			switch (inst.getAttribute()->getRedefinitionMode()) {
//				case RedefinitionMode::KeepCurrent: {
//					float currentValue = inst.getCurrentValue();
//
//					if (currentValue == currentMax) {
//						// Update both (current and default) to the new max
//						inst.setDefaultValue(chosenValue, enum_cast(AttributeOperands::OPERAND_CURRENT));
//					}
//					else {
//						// Ensure current is between the new min and max
//						currentValue = Math::clamp(currentValue, min, max);
//						if (currentValue != inst.getCurrentValue()) {
//							inst.setDefaultValue(currentValue, enum_cast(AttributeOperands::OPERAND_CURRENT));
//						}
//
//						// Update default to new value so modifiers apply
//						// to the new default
//						inst.setDefaultValueOnly(chosenValue);
//					}
//					break;
//				}
//				case RedefinitionMode::UpdateToNewDefault: {
//					// Update both default and current values to new default
//					inst.setDefaultValue(chosenValue, enum_cast(AttributeOperands::OPERAND_CURRENT));
//
//					// Apply modifiers to new default
//					inst.recalculateModifiers();
//					break;
//				}
//			}
//		}
//		if (method != InitializationMethod::UPDATED) {
//			resetAttributes();
//		}
//	}
//
//	// Now that we have a definition, re-create AI goals based on it.
//	createAIGoals();
//
//	_tryInitializeComponent(this, mCurrentDescription->mLeashable, mLeashableComponent, changedDescription.mLeashable);
//	_tryInitializeComponent(this, mCurrentDescription->mBreathable, mBreathableComponent, changedDescription.mBreathable);
//	_tryInitializeComponent(this, mCurrentDescription->mLookAt, mLookAtComponent, changedDescription.mLookAt);
//	_tryInitializeComponent(this, mCurrentDescription->mTeleport, mTeleportComponent, changedDescription.mTeleport);
//	_tryInitializeComponent(this, mCurrentDescription->mBoostable, mBoostableComponent, changedDescription.mBoostable);
//	_tryInitializeComponent(this, mCurrentDescription->mHealable, mHealableComponent, changedDescription.mHealable);
//
//	if (changedDescription.mBreedable && !_tryInitializeComponent(this, mCurrentDescription->mBreedable, mBreedableComponent)) {
//		//if we can't breed, we can't be in love. Just in terms of Minecraft.
//		setStatusFlag(EntityFlags::INLOVE, false);
//	}
//	if (changedDescription.mAngry && !_tryInitializeComponent(this, mCurrentDescription->mAngry, mAngryComponent)) {
//		//if we can't breed, we can't be in love. Just in terms of Minecraft.
//		setStatusFlag(EntityFlags::ANGRY, false);
//	}
//	
//	//Init other attributes
//	if (mCurrentDescription->mFloatsInLiquid) {
//		mFloatsInLiquid = *mCurrentDescription->mFloatsInLiquid;
//	}
//	if (mCurrentDescription->mBurnsInDaylight) {
//		mBurnsInDaylight = *mCurrentDescription->mBurnsInDaylight;
//	}
//	if (mCurrentDescription->mFireImmune) {
//		mFireImmune = *mCurrentDescription->mFireImmune;
//		mAlwaysFireImmune = *mCurrentDescription->mFireImmune;
//	}
//	if (mCurrentDescription->mDefaultLookAngle) {
//		mDefaultLookAngle = *mCurrentDescription->mDefaultLookAngle;
//	}
//	if (mCurrentDescription->mFlyingSpeed) {
//		mFlyingSpeed = *mCurrentDescription->mFlyingSpeed;
//	}
//	if (mCurrentDescription->mWalkAnimSpeedMultiplier) {
//		mWalkAnimSpeedMultiplier = *mCurrentDescription->mWalkAnimSpeedMultiplier;
//	}
//	if (mCurrentDescription->mFrictionModifier) {
//		mFrictionModifier = *mCurrentDescription->mFrictionModifier;
//	}
//	if (mCurrentDescription->mWantsJockey) {
//		mWantsToBeJockey = *mCurrentDescription->mWantsJockey;
//	}
//	if (mCurrentDescription->mHurtWhenWet) {
//		mHurtWhenWet = *mCurrentDescription->mHurtWhenWet;
//	}
//
//	// Attack damage
//	if (mCurrentDescription->mAttack) {
//		// The attack component ensures the existence of the attack damage attribute
//		float attack = mCurrentDescription->mAttack->mDamage.rangeMin;
//		getAttributes()->registerAttribute(SharedAttributes::ATTACK_DAMAGE).setRange(attack, attack, attack);
//	}
//
//	// Equipment (only generated on non-load)
//	if (changedDescription.mEquipmentTable && mCurrentDescription->mEquipmentTable && method != InitializationMethod::LOADED && method != InitializationMethod::LEGACY) {
//		DEBUG_ASSERT(hasCategory(EntityCategory::Mob), "Entity with equipment is not a Mob!");
//		LootTable* table = LootTables::lookupByName(*mCurrentDescription->mEquipmentTable, *getLevel().getServerResourcePackManager());
//		if (table != nullptr) {
//
//			auto context = LootTableContext::Builder(&getLevel()).withThisEntity(this).create();
//			std::vector<ItemInstance> items = table->getRandomItems(mRandom, context);
//
//			for (auto& item : items) {
//				if (ItemInstance::isArmorItem(&item)) {
//					ArmorSlot slot = ArmorItem::getSlotForItem(item);
//					setArmor(slot, &item);
//				}
//				else {
//					//some other item that will be carried in their hand
//					setCarriedItem(item);
//				}
//			}
//		}
//	}
//
//	if (changedDescription.mMobEffects && mCurrentDescription->mMobEffects && method == InitializationMethod::UPDATED) {
//		for (auto& effectName : mCurrentDescription->mMobEffects->mRemoveEffects) {
//			MobEffect* effect = MobEffect::getByName(effectName);
//			if (effect != nullptr) {
//				removeEffect(effect->getId());
//			}
//		}
//		for (auto& effectInst : mCurrentDescription->mMobEffects->mAddEffects) {
//			if (mMobEffects[effectInst.getId()] != MobEffectInstance::NO_EFFECT) {
//				MobEffectInstance* inst = &mMobEffects[effectInst.getId()];
//				inst->update(effectInst);
//			}
//			else {
//				addEffect(effectInst);
//			}
//		}
//	}
//}
//
//Mob::~Mob() {
//	removeAllEffects();
//}
//
//bool Mob::hasComponent(const std::string &name) const {
//	if (Entity::hasComponent(name)) {
//		return true;
//	}
//	for (auto &effect : mMobEffects) {
//		if (effect != MobEffectInstance::NO_EFFECT && effect.getComponentName() == name) {
//			return true;
//		}
//	}
//	
//	int enchant = EnchantUtils::getEnchantmentId(name);
//	const ItemInstance* item = EnchantUtils::getRandomItemWith(enchant, *this);
//	if (item != nullptr) {
//		return true;
//	}
//	
//	return false;
//}
//
//void Mob::createAIGoals() {
//
//	if (!mCurrentDescription) {
//		DEBUG_FAIL("Cannot createAIGoals() unless there is a definition set first.");
//		return;
//	}
//	
//	mGoalSelector.clearGoals();
//	mTargetSelector.clearGoals();
//	auto && goalDefs = mCurrentDescription->mGoalDefinitions;
//	for (auto i = goalDefs.begin(), iend = goalDefs.end(); i != iend; ++i) {
//		Unique<Goal> goal = GoalDefinition::CreateGoal(*this, *i);
//		if (goal) {
//			if (goal->isTargetGoal()) {
//				mTargetSelector.addGoal(i->mPriority, std::move(goal));
//			}
//			else {
//				mGoalSelector.addGoal(i->mPriority, std::move(goal));
//			}
//		}
//	}
//}
//
//Weak<Village> const& Mob::getVillage() const {
//	return mVillage;
//}
//
//bool Mob::canSee(const Entity& target) const {
//	auto &region = this->getRegion();
//	HitResult res = region.clip(
//		Vec3(mPos.x, mPos.y + getHeadHeight(), mPos.z),
//		Vec3(target.mPos.x, target.mPos.y + target.getHeadHeight(), target.mPos.z));
//	return !res.isHit();
//}
//
//bool Mob::isPickable() {
//	return !isRemoved();
//}
//
//bool Mob::isPushable() const {
//	return !isRemoved() && !isImmobile();
//}
//
//float Mob::getHeadHeight() const {
//	return mBBDim.y * 0.85f;
//}
//
//float Mob::getVoicePitch() {
//	//TODO adors - isn't this solved in data driving? 
//	if (isBaby()) {
//		return mRandom.nextGaussianFloat() * 0.2f + 1.5f;
//	}
//
//	return mRandom.nextGaussianFloat() * 0.2f + 1.0f;
//}
//
//int Mob::getAmbientSoundPostponeTicks() {
//	return 8 * SharedConstants::TicksPerSecond;
//}
//
//bool Mob::_isHeadInWater() {
//	BlockPos t(getAttachPos(EntityLocation::Head));
//	return getRegion().getMaterial(t).isType(MaterialType::Water);
//}
//
//void Mob::playAmbientSound() {
////	const char* ambient = _getAmbientSound();
////	if (ambient) {
////		playSound(ambient, _getSoundVolume(), getVoicePitch(), EntityLocation::Head);
////	}
//
////	if (mCurrentDefinition && mCurrentDefinition->mMobSettings.mAmbientSound.mSoundName.length() > 0) {
////		const SoundDefinition& soundDef = mCurrentDefinition->mMobSettings.mAmbientSound;
////		playSound(soundDef.mSoundName.c_str(), soundDef.getVolume(mRandom), soundDef.getPitch(mRandom), EntityLocation::Head);
////	}
//
//	//todo Li : handle mobs that make multiple version of ambient sound
//	LevelSoundEvent ambientSoundEvent = LevelSoundEvent::Ambient;
//	if (_isHeadInWater()) {
//		ambientSoundEvent = LevelSoundEvent::AmbientInWater;
//	}
//	else if (isBaby()) {
//		ambientSoundEvent = LevelSoundEvent::AmbientBaby;
//	}
//
//	playSound(ambientSoundEvent, getAttachPos(EntityLocation::Head));
//}
//
//void Mob::playSpawnSound() {
//	LevelSoundEvent ambientSoundEvent = LevelSoundEvent::Ambient;
//	if (_isHeadInWater()) {
//		ambientSoundEvent = LevelSoundEvent::AmbientInWater;
//	}
//	else if (isBaby()) {
//		ambientSoundEvent = LevelSoundEvent::AmbientBaby;
//	}
//
//	playSynchronizedSound(ambientSoundEvent, getAttachPos(EntityLocation::Head));
//}
//
//void Mob::baseTick() {
//	mOAttackAnim = mAttackAnim;
//	Entity::baseTick();
//
//	ScopedProfile("mobBaseTick");
//
//	tickLeash();
//
//	if (!isImmobile() && ((mAmbientSoundTime++ & 15) == 0) && mRandom.nextInt(mAmbientPlayBackInterval) < mAmbientSoundTime) {
//		mAmbientSoundTime = -getAmbientSoundPostponeTicks();
//		playAmbientSound();
//	}
//
//	if (isAlive() && isInWall()) {
//		EntityDamageSource source = EntityDamageSource(EntityDamageCause::Suffocation);
//		hurt(source, 1);
//	}
//
//	mOTilt = mTilt;
//	if (mAttackTime > 0) {
//		mAttackTime--;
//	}
//	if (mHurtTime > 0) {
//		mHurtTime--;
//	}
//	if (mInvulnerableTime > 0) {
//		mInvulnerableTime--;
//	}
//
//	if (mLastHurtByMobId && mLastHurtByMobTime <= 0) {
//		setLastHurtByMob(nullptr);
//	}
//
//	if (mLastHurtByPlayerId && mLastHurtByPlayerTime <= 0) {
//		setLastHurtByPlayer(nullptr);
//	}
//
//	if (getHealth() <= 0) {
//		tickDeath();
//	}
//	if (mLastHurtByMobTime > 0) {
//		--mLastHurtByMobTime;
//	}
//
//	if (mLastHurtByPlayerTime > 0) {
//		--mLastHurtByPlayerTime;
//	}
//
//	if (getAttribute(SharedAttributes::HEALTH).getCurrentValue() > 0.f) {
//		tickAttributes();
//	}
//
//	_sendDirtyMobData();
//
//	// update effects
//	tickEffects();
//
//	// Sprint particles
//	_updateSprintingState();
//
//	mAnimStepO = mAnimStep;
//
//	mYBodyRotO = mYBodyRot;
//	mYHeadRotO = mYHeadRot;
//	mRotPrev.y = mRot.y;
//	mRotPrev.x = mRot.x;
//}
//
//void Mob::tickDeath() {
//	mDeathTime++;
//
//	if (!mSpawnedXP && mDeathTime > SharedConstants::TicksPerSecond) {
//		if (!mLevel->isClientSide()) {
//			ExperienceOrb::spawnOrbs(getRegion(), getPos(), getExperienceReward());
//			mSpawnedXP = true;
//		}
//
//		if (hasCategory(EntityCategory::Player)) {
//			setInvisible(true);
//		}
//		else {
//			remove();
//		}
//
//		spawnDeathParticles();
//	}
//}
//
//void Mob::spawnAnim() {
//
//}
//
//void Mob::lerpTo(const Vec3& pos, const Vec2& rot, int steps) {
//	mInterpolation.lerpTo(pos, rot, steps);
//}
//
//void Mob::normalTick() {
//	Entity::normalTick();
//
//	if (onLadder()) {
//		mFallDistance = 0;
//	}
//
//	if (mArrowCount > 0) {
//		if (mRemoveArrowTime <= 0) {
//			mRemoveArrowTime = 20 * 3;
//		}
//		mRemoveArrowTime--;
//		if (mRemoveArrowTime <= 0) {
//			mArrowCount--;
//		}
//	}
//
//	// If your target is a player and they are in creative mode, then don't target them anymore.
//	// Ideally I would like to do this when the property is changed on the player, but that
//	// would require more of a refacter than I'm willing to take on right now.
//	Entity* target = getTarget();
//	if (target != nullptr && target->getEntityTypeId() == EntityType::Player ) {
//		Player* targetPlayer = static_cast<Player*>(target);
//		if (targetPlayer->mAbilities.mInvulnerable) {
//			setTarget(nullptr);
//		}
//	}
//
//	mInterpolation.tick(*this);
//
//	aiStep();
//
//	float xd = mPos.x - mPosPrev.x;
//	float zd = mPos.z - mPosPrev.z;
//
//	float sideDist = (float)Math::sqrt(xd * xd + zd * zd);
//
//	float yBodyRotT = mYBodyRot;
//
//	float walkSpeed = 0;
//	mORun = mRun;
//	float tRun = 0;
//	if (sideDist <= 0.05f) {
//		// animStep = 0;
//	}
//	else {
//		tRun = 1;
//		walkSpeed = sideDist * 3;
//		yBodyRotT = Math::atan2(zd, xd) * Math::RADDEG - 90;
//	}
//	if (mAttackAnim > 0) {
//		yBodyRotT = mRot.y;
//	}
//	if (!mOnGround) {
//		tRun = 0;
//	}
//	mRun = mRun + (tRun - mRun) * 0.3f;
//
//	if (useNewAi()) {
//		mBodyControl->clientTick();
//	}
//	else {
//		float yBodyRotD = yBodyRotT - mYBodyRot;
//
//		while (yBodyRotD < -180) {
//			yBodyRotD += 360;
//		}
//
//		while (yBodyRotD >= 180) {
//			yBodyRotD -= 360;
//		}
//		mYBodyRot += yBodyRotD * 0.3f;
//
//		float headDiff = mRot.y - mYBodyRot;
//
//		while (headDiff < -180) {
//			headDiff += 360;
//		}
//
//		while (headDiff >= 180) {
//			headDiff -= 360;
//		}
//		bool behind = headDiff < -90 || headDiff >= 90;
//		if (headDiff < -75) {
//			headDiff = -75;
//		}
//		if (headDiff >= 75) {
//			headDiff = +75;
//		}
//
//		//don't mess with body rotation, if we're riding and flagged not to
//		if ((nullptr == mRiding) || !mRiding->mInheritRotationWhenRiding) {
//			mYBodyRot = mRot.y - headDiff;
//			if (headDiff * headDiff > 50 * 50) {
//				mYBodyRot += headDiff * 0.2f;
//			}
//		}
//
//		if (behind) {
//			walkSpeed *= -1;
//		}
//	}
//
//	while (mRot.y - mRotPrev.y < -180) {
//		mRotPrev.y -= 360;
//	}
//
//	while (mRot.y - mRotPrev.y >= 180) {
//		mRotPrev.y += 360;
//	}
//
//	while (mYBodyRot - mYBodyRotO < -180) {
//		mYBodyRotO -= 360;
//	}
//
//	while (mYBodyRot - mYBodyRotO >= 180) {
//		mYBodyRotO += 360;
//	}
//
//	while (mRot.x - mRotPrev.x < -180) {
//		mRotPrev.x -= 360;
//	}
//
//	while (mRot.x - mRotPrev.x >= 180) {
//		mRotPrev.x += 360;
//	}
//
//	while (mYHeadRot - mYHeadRotO < -180) {
//		mYHeadRotO -= 360;
//	}
//
//	while (mYHeadRot - mYHeadRotO >= 180) {
//		mYHeadRotO += 360;
//	}
//
//	updateWalkAnim();
//
//	mAnimStep += walkSpeed;
//
//	if (isGliding()) {
//		mFallFlyTicks++;
//	}
//	else {
//		mFallFlyTicks = 0;
//	}
//
//	auto& data = getEntityData();
//	if (!getLevel().isClientSide() && data.isDirty()) {
//		SetEntityDataPacket packet(getRuntimeID(), data);
//		getDimension().sendPacketForEntity(*this, packet);
//	}
//
//	
//}
//
//void Mob::heal(int heal) {
//	if (getHealth() > 0 && !isInvertedHealAndHarm()) {
//		getMutableAttribute(SharedAttributes::HEALTH)->addBuff(InstantaneousAttributeBuff((float)heal, AttributeBuffType::Heal));
//	}
//	else if (getHealth() > 0) {
//		getMutableAttribute(SharedAttributes::HEALTH)->addBuff(InstantaneousAttributeBuff((float)heal, AttributeBuffType::Harm));
//	}
//}
//
//void Mob::healEffects(int amountHealed) {
//	mInvulnerableTime = INVULNERABLE_DURATION / 2;
//}
//
//bool Mob::isInvertedHealAndHarm() const {
//	return EntityClassTree::isTypeInstanceOf(getEntityTypeId(), EntityType::UndeadMob);
//}
//
//bool Mob::doFireHurt(int amount) {
//	if (hasEffect(*MobEffect::FIRE_RESISTANCE)) {
//		return false;
//	}
//	return Entity::doFireHurt(amount);
//}
//
//int Mob::getMeleeWeaponDamageBonus(Mob* target) {
//	int bonus = 0;
//	ItemInstance* weapon = getCarriedItem();
//
//	// most mobs behave as if they have no weapon, so weapon damage is added on top of base damage
//	if (weapon) {
//		bonus += weapon->getAttackDamage();
//		bonus += EnchantUtils::getMeleeDamageBonus(*target, *this);
//	}
//
//	return bonus;
//}
//
//int Mob::getMeleeKnockbackBonus() {
//	ItemInstance* weapon = getCarriedItem();
//
//	if (weapon == nullptr) {
//		return 0;
//	}
//
//	if (weapon->getId() == Item::mEnchanted_book->getId()) {
//		return 0;
//	}
//
//	return EnchantUtils::getEnchantLevel(Enchant::Type::WEAPON_KNOCKBACK, *weapon);
//}
//
//bool Mob::_hurt(const EntityDamageSource& source, int dmg, bool knock, bool ignite) {
//	if (getLevel().isClientSide()) {
//		return false;
//	}
//
//	mNoActionTime = 0;
//
//	mWalkAnimSpeed = 1.5f;
//
//	// reduce damage by enchanted armor values
//	dmg = Math::floor(EnchantUtils::getDamageReduction(source, *this) * static_cast<float>(dmg));
//
//	//mobs that interact with the player become persistent
//	if (source.isEntitySource()) {
//		Entity* entity = ((EntityDamageByEntitySource&)source).getEntity();
//
//		if (entity && entity->hasCategory(EntityCategory::Mob)) {
//
//			// if it's not a mob, it can't be a player
//			if (entity->hasCategory(EntityCategory::Player)) {
//				setLastHurtByPlayer(static_cast<Player*>(entity));
//			}
//			else {
//				setLastHurtByMob(static_cast<Mob*>(entity));
//			}
//
//			// if enchanted armor has a reactive effect (basically just thorns) apply it here
//			EnchantUtils::doPostHurtEffects(*this, (Mob&)*entity);
//		}
//	}
//
//	if (mInvulnerableTime == INVULNERABLE_DURATION && getChainedDamageEffects()) {
//		dmg += mLastHurt;
//	}
//	else if (mInvulnerableTime > INVULNERABLE_DURATION / 2.0f) {
//		if (dmg <= mLastHurt) {
//			return false;
//		}
//	}
//	else {
//		mLastHurt = 0;
//	}
//
//
//	if (!getLevel().isClientSide()) {
//		if (getDamageSensorComponent()) {
//			if (!getDamageSensorComponent()->recordDamage(source.getEntity(), source.getCause(), dmg - mLastHurt, (getHealth() - dmg - mLastHurt) <= 0)) {
//				return false;
//			}
//		}
//		actuallyHurt(dmg - mLastHurt, &source);
//	}
//
//	return hurtEffects(source, dmg, knock, ignite);
//}
//
//bool Mob::hurtEffects(const EntityDamageSource& source, int damage, bool knock, bool ignite) {
//	if (getLevel().isClientSide()) {
//		return false;
//	}
//
//	if (hasCategory(EntityCategory::Player)) {
//		if (static_cast<Player*>(this)->mAbilities.mInvulnerable) {
//			return false;
//		}
//	}
//
//	bool hurt = false;
//	bool chainedHurt = false;
//	if (mInvulnerableTime == INVULNERABLE_DURATION && getChainedDamageEffects()) {
//		chainedHurt = true;
//		mLastHurt = damage;
//		mLastHealth = getHealth();
//	}
//	else if (mInvulnerableTime > INVULNERABLE_DURATION / 2.0f) {
//		if (damage <= mLastHurt) {
//			return false;
//		}
//
//		mLastHurt = damage;
//	}
//	else {
//		hurt = true;
//		mLastHurt = damage;
//		mLastHealth = getHealth();
//		mInvulnerableTime = INVULNERABLE_DURATION;
//		mHurtTime = 10;
//		mHurtDuration = 10;
//	}
//
//	Entity* srcEntity = source.getEntity();
//	Entity* childEntity = source.getDamagingEntity();
//	if ((srcEntity != nullptr && ignite) ||
//		(childEntity != nullptr && (childEntity->isOnFire() || (childEntity->getProjectileComponent() != nullptr && childEntity->getProjectileComponent()->getCatchFire())))) {
//		if (!(childEntity->getProjectileComponent() != nullptr && !childEntity->getProjectileComponent()->getCatchFire())) {
//			setOnFire(2 * DifficultyUtils::getMultiplier(getLevel().getDifficulty()));
//		}
//	}
//
//	mHurtDir = 0;
//
//
//	if (hurt){
//
//		getLevel().broadcastEntityEvent(this, EntityEvent::HURT);	// note: handles playSound for all clients (including
//																// this one)
//
//		markHurt();
//
//		if (source.isEntitySource()) {
//
//			Entity* entity = source.getEntity();
//			float xd = entity->mPos.x - mPos.x;
//			float zd = entity->mPos.z - mPos.z;
//
//			if (knock) {
//
//				while (xd * xd + zd * zd < 0.0001) {
//					xd = (Math::random() - Math::random()) * 0.01f;
//					zd = (Math::random() - Math::random()) * 0.01f;
//				}
//
//				mHurtDir = (float)(std::atan2(zd, xd) * Math::RADDEG) - mRot.y;
//
//
//				knockback(entity, damage, xd, zd);
//			}
//			if (entity->hasCategory(EntityCategory::Player)) {
//				setLastHurtByPlayer(static_cast<Player*>(entity));
//			}
//			else if (entity->hasCategory(EntityCategory::Mob)) {
//				setLastHurtByMob(static_cast<Mob*>(entity));
//			}
//		}
//		else {
//			mHurtDir = (int)(Math::random() * 2.0f) * 180.0f;
//		}
//
//		
//	}
//
//	if (getHealth() <= 0) {
//		die(source);
//	}
//
//	return hurt || chainedHurt;
//}
//
//void Mob::actuallyHurt(int dmg, const EntityDamageSource* source, bool bypassArmor) {
//	if (isInvulnerableTo(*source)) {
//		return;
//	}
//
//	if (!bypassArmor) {
//		dmg = getDamageAfterArmorAbsorb(*source, dmg);
//	}
//	dmg = getDamageAfterMagicAbsorb(*source, dmg);
//
//	// Check Absorption
//	auto absorption = getMutableAttribute(SharedAttributes::ABSORPTION);
//	int originalDamage = dmg;
//	dmg = (int) std::max((float) dmg - absorption->getCurrentValue(), 0.f);
//	absorption->addBuff(InstantaneousAttributeBuff((float)-(originalDamage - dmg)));
//
//	if (dmg == 0) {
//		return;
//	}
//
//	AttributeInstance *health = getMutableAttribute(SharedAttributes::HEALTH);
//
//	health->addBuff(InstantaneousAttributeBuff((float)-dmg, AttributeBuffType::None));
//	absorption->addBuff(InstantaneousAttributeBuff((float)-dmg));
//}
//
//void Mob::_playStepSound(const BlockPos& pos, int t) {
//	if (onLadder(false)) {
//		playSound(LevelSoundEvent::Step, getAttachPos(EntityLocation::Feet), Block::mLadder->getId());
//	} 
//	else {
//		Entity::_playStepSound(pos, t);
//	}	
//}
//
//void Mob::knockback(Entity* source, int dmg, float xd, float zd, float power) {
//
//	if (mRandom.nextFloat() < getAttribute(SharedAttributes::KNOCKBACK_RESISTANCE).getCurrentValue()) {
//		return;
//	}
//
//	float dd = Math::invSqrt(xd * xd + zd * zd);
//	float pow = 0.4f * power;
//
//	mPosDelta.x *= 0.5f;
//	mPosDelta.y *= 0.5f;
//	mPosDelta.z *= 0.5f;
//
//	mPosDelta.x -= (xd * dd * pow);
//	mPosDelta.y += 0.4f;
//	mPosDelta.z -= (zd * dd * pow);
//
//	if (mPosDelta.y > 0.4f) {
//		mPosDelta.y = 0.4f;
//	}
//}
//
//void Mob::die(const EntityDamageSource& source) {
//	if (mDead) { return; }
//	mDead = true;
//
//	if (getTransformationComponent()) {
//		getTransformationComponent()->transformIfAble();
//	}
//	
//	Entity* sourceEntity = source.getEntity();
//	/*
//	final LivingEntity killer = getKillCredit();
//	if (deathScore >= 0 && killer != null) {
//	killer.awardKillScore(this, deathScore);
//	}	*/
//
//	if (sourceEntity != nullptr) {
//		sourceEntity->killed(this);
//	}
//
//	if (!getLevel().isClientSide()) {
//		int lootBonusLevel = 0;
//		if (sourceEntity != nullptr && sourceEntity->hasCategory(EntityCategory::Mob)) {
//			lootBonusLevel = EnchantUtils::getBestEnchantLevel(Enchant::Type::WEAPON_LOOT, *static_cast<const Mob*>(sourceEntity), EquipmentFilter::HANDS);
//		}
//		resolveDeathLoot((sourceEntity != nullptr && sourceEntity->hasCategory(EntityCategory::Player)), lootBonusLevel, source);
//		dropLeash(true);
//		getLevel().broadcastEntityEvent(this, EntityEvent::DEATH);
//	}
//
//	// Send MobKilled event
//	Player* player = getLastHurtByPlayer();
//	if (player != nullptr && sourceEntity != nullptr) {
//		EventPacket killedMobPacket(player, sourceEntity, this, source.getCause());
//		player->sendEventPacket(killedMobPacket);
//	}
//
//	removeAllRiders(true);
//}
//
//void Mob::resolveDeathLoot(bool wasKilledByPlayer, int lootBonusLevel, const EntityDamageSource& source) {
//	auto lootTable = mLootDropped ? getDefaultLootTable() : getLootTable();
//
//	if (lootTable != nullptr) {
//		auto context = LootTableContext::Builder(&getLevel()).withThisEntity(this).withDeathSource(&source);
//		if (wasKilledByPlayer && getLastHurtByPlayer() != nullptr) 	{
//			context.withKillerPlayer(getLastHurtByPlayer()).withLuck(getLastHurtByPlayer()->getLuck());
//		}
//
//		auto random = mLootTableSeed == 0 ? mRandom : Random(mLootTableSeed);
//		auto contextInstance = context.create();
//		auto items = lootTable->getRandomItems(random, contextInstance);
//		for(auto& item : items) { 
//			spawnAtLocation(item, 0);
//		}
//		mLootDropped = true;
//	} 
//	dropEquipment(source, lootBonusLevel);
//}
//
//void Mob::playerTouch(Player& player) {
//	Entity::playerTouch(player);
//	setPersistent();
//}
//
//void Mob::_sendDirtyMobData() {
//	if (!getLevel().isClientSide()) {
//		// Attributes
//		auto dirtyAttributes = mAttributes->getDirtyAttributes();
//		if (dirtyAttributes.size()) {
//			UpdateAttributesPacket attrPacket(*this, dirtyAttributes);
//			getDimension().sendPacketForEntity(*this, attrPacket);
//			mAttributes->clearDirtyAttributes();
//		}
//
//		// SynchedEntityData
//		auto& data = getEntityData();
//		if (data.isDirty()) {
//			SetEntityDataPacket packet(getRuntimeID(), data);
//			getDimension().sendPacketForEntity(*this, packet);
//		}
//	}
//}
//
//int Mob::getEquipmentCount(void) const {
//	int count = 0;
//
//	for (auto&& equipment : mArmor) {
//		if (!equipment.isNull()) {
//			++count;
//		}
//	}
//	return count;
//}
//
//void Mob::causeFallDamage(float distance) {
//
//	auto jumpBoost = getEffect(*MobEffect::JUMP);
//	int padding = jumpBoost != nullptr ? jumpBoost->getAmplifier() + 1 : 0;
//
//	float dmg = (float)ceil((getAttribute(SharedAttributes::FALL_DAMAGE).getCurrentValue() * distance - 3 - padding));
//
//	if (dmg > 0) {
//		playSynchronizedSound(dmg > 4 ? LevelSoundEvent::FallBig : LevelSoundEvent::FallSmall, getAttachPos(EntityLocation::Feet));
//
//		EntityDamageSource source = EntityDamageSource(EntityDamageCause::Fall);
//		hurt(source, (int)dmg);
//		
//		if (isSilent() == false) {
//			BlockID t = getRegion().getBlockID(Math::floor(mPos.x), Math::floor(mPos.y - 0.2f - mHeightOffset), Math::floor(mPos.z));
//			playSynchronizedSound(LevelSoundEvent::Fall, getAttachPos(EntityLocation::Feet), t);
//		}
//	}
//}
//
//
//Mob::TravelType Mob::getTravelType() {
//
//	if (isInWater() && !(EntityClassTree::isOfType(getEntityTypeId(), EntityType::Player) && ((Player*)this)->mAbilities.isFlying())) {
//		return TravelType::Water;
//	}
//
//	if (isInLava() && !(EntityClassTree::isOfType(getEntityTypeId(), EntityType::Player) && ((Player*)this)->mAbilities.isFlying())) {
//		return TravelType::Lava;
//	}
//
//	if (mOnGround) {
//		return TravelType::Ground;
//	}
//
//	// not on the ground, so we must be flying
//	return TravelType::Air;
//}
//
//// magic numbers courtesy of java version
//const float MAGIC_FRICTION_CONSTANT = 0.91f;
//const float MAGIC_FRICTION_MODIFIER = 0.6f;
//
//float Mob::calcMoveRelativeSpeed(TravelType travelType) {
//
//	if (travelType == TravelType::Water) {
//
//		float waterSpeed = 0.02f;
//		float waterWalker = static_cast<float>(EnchantUtils::getBestEnchantLevel(Enchant::Type::WATER_SPEED, *this, EquipmentFilter::ARMOR));
//		int maxWaterWalker = Enchant::mEnchants[Enchant::Type::WATER_SPEED]->getMaxLevel();
//		waterWalker = Math::clamp(waterWalker, 0.0f, static_cast<float>(maxWaterWalker));
//
//		if (!mOnGround) {
//			waterWalker *= 0.5f;
//		}
//
//		if (waterWalker > 0.0f) {
//			waterSpeed += (getSpeed() - waterSpeed) * waterWalker / maxWaterWalker;
//		}
//
//		return waterSpeed;
//	}
//	else if (travelType == TravelType::Lava) {
//		return 0.02f;
//	}
//	else if (travelType == TravelType::Ground) {
//		
//		float friction = MAGIC_FRICTION_CONSTANT;
//
//		// Fetch Ground Block
//		BlockPos blockPos(Math::floor(mPos.x), Math::floor(mBB.min.y - 0.1f), Math::floor(mPos.z));
//		BlockSource& region = getRegion();
//		const Block* groundBlock = &region.getBlock(blockPos);
//		if (groundBlock->isType(Block::mAir)) {
//			blockPos = blockPos.below();
//			groundBlock = &region.getBlock(blockPos);
//		}
//
//		if (!groundBlock->isType(Block::mAir)) {
//			friction = groundBlock->calcGroundFriction(*this, blockPos) * MAGIC_FRICTION_CONSTANT;
//		}
//		else {
//			friction = MAGIC_FRICTION_MODIFIER * MAGIC_FRICTION_CONSTANT;
//		}
//
//		const float friction2comp = (MAGIC_FRICTION_MODIFIER * MAGIC_FRICTION_CONSTANT) / friction;
//		const float friction2 = friction2comp * friction2comp * friction2comp;
//		const float walkingSpeed = getSpeed();
//
//		return walkingSpeed * friction2;
//	}
//
//	// not on the ground, so we must be flying
//	return mFlyingSpeed;
//}
//
//float Mob::getWaterSlowDown() const {
//	return 0.8f;
//}
//
//void Mob::travel(float xa, float ya) {
//	if (isImmobile()) {
//		return;
//	}
//	
//	TravelType travelType = getTravelType();
//	float moveRelativeSpeed = calcMoveRelativeSpeed(travelType);
//	
//	if (travelType == TravelType::Water) {
//		float yo = mPos.y;
//
//		float slowdown = getWaterSlowDown();
//		float waterWalker = static_cast<float>(EnchantUtils::getBestEnchantLevel(Enchant::Type::WATER_SPEED, *this, EquipmentFilter::ARMOR));
//		int maxWaterWalker = Enchant::mEnchants[Enchant::Type::WATER_SPEED]->getMaxLevel();
//		waterWalker = Math::clamp(waterWalker, 0.0f, static_cast<float>(maxWaterWalker));
//
//		if (!mOnGround) {
//			waterWalker *= 0.5f;
//		}
//
//		if (waterWalker > 0.0f) {
//			slowdown += ((MAGIC_FRICTION_MODIFIER * MAGIC_FRICTION_CONSTANT) - slowdown) * waterWalker / maxWaterWalker;
//		}
//
//		moveRelative(xa, ya, moveRelativeSpeed);
//		move(mPosDelta);
//
//		mPosDelta.x *= slowdown;
//		mPosDelta.y *= 0.8f;
//		mPosDelta.z *= slowdown;
//		mPosDelta.y -= 0.02f;
//
//		if (mHorizontalCollision && isFree(Vec3(mPosDelta.x, mPosDelta.y + 0.6f - mPos.y + yo, mPosDelta.z))) {
//			mPosDelta.y = 0.3f;
//		}
//	}
//	else if (travelType == TravelType::Lava) {
//		float yo = mPos.y;
//		moveRelative(xa, ya, moveRelativeSpeed);
//		move(mPosDelta);
//		mPosDelta.x *= 0.50f;
//		mPosDelta.y *= 0.50f;
//		mPosDelta.z *= 0.50f;
//		mPosDelta.y -= 0.02f;
//
//		if (mHorizontalCollision && isFree(Vec3(mPosDelta.x, mPosDelta.y + 0.6f - mPos.y + yo, mPosDelta.z))) {
//			mPosDelta.y = 0.3f;
//		}
//	}
//	else if (isGliding()) {
//		// Reset fall distance if your downwards movement is slow enough to prevent fall damage
//		if (mPosDelta.y > GLIDING_FALL_RESET_DELTA) {
//			mFallDistance = 1;
//		}
//
//		const Vec3 lookAngle = getViewVector(1.0f);
//		const float leanAngle = mRot.x * Math::DEGRAD;
//		const float lookHorLength = Math::sqrt(lookAngle.x * lookAngle.x + lookAngle.z * lookAngle.z);
//		const float moveHorLength = Math::sqrt(mPosDelta.x * mPosDelta.x + mPosDelta.z * mPosDelta.z);
//		const float moveTotalLength = lookAngle.length();
//
//		float liftForce = Math::cos(leanAngle);
//		liftForce *= liftForce * Math::min(1.0f, moveTotalLength / 0.4f);
//		mPosDelta.y += -0.08f + liftForce * 0.06f;
//
//		// The lift force will convert falling speed into forward speed.
//		if (mPosDelta.y < 0 && lookHorLength > 0) {
//			const float convert = mPosDelta.y * -0.1f * liftForce;
//			mPosDelta.x += lookAngle.x * convert / lookHorLength;
//			mPosDelta.y += convert;
//			mPosDelta.z += lookAngle.z * convert / lookHorLength;
//		}
//
//		// Convert forward speed to up speed.
//		if (leanAngle < 0) {
//			const float convert = moveHorLength * -Math::sin(leanAngle) * 0.04f;
//			mPosDelta.x += -lookAngle.x * convert / lookHorLength;
//			mPosDelta.y += convert * 3.2f;  // Magic number that allows you raise, but not enough to fly forever.
//			mPosDelta.z += -lookAngle.z * convert / lookHorLength;
//		}
//
//		if (lookHorLength > 0) {
//			mPosDelta.x += ((lookAngle.x / lookHorLength * moveHorLength) - mPosDelta.x) * 0.1f;
//			mPosDelta.z += ((lookAngle.z / lookHorLength * moveHorLength) - mPosDelta.z) * 0.1f;
//		}
//
//		mPosDelta.x *= 0.99f;
//		mPosDelta.y *= 0.98f;
//		mPosDelta.z *= 0.99f;
//		move(mPosDelta);
//
//		if (mHorizontalCollision && !mLevel->isClientSide()) {
// 			const float newMoveHorLength = Math::sqrt(mPosDelta.x * mPosDelta.x + mPosDelta.z * mPosDelta.z);
//			const float diff = moveHorLength - newMoveHorLength;
//			const int dmg = (int)(diff * 10 - 3);
//
//			if (dmg > 0) {
//				playSynchronizedSound(dmg > 4 ? LevelSoundEvent::FallBig : LevelSoundEvent::FallSmall, getAttachPos(EntityLocation::Body));
//				EntityDamageSource source = EntityDamageSource(EntityDamageCause::FlyIntoWall);
//				hurt(source, dmg);
//			}
//		}
//	}
//	else if (travelType == TravelType::Ground || travelType == TravelType::Air) { // Assume Ground or Air Travel Types
//
//		moveRelative(xa, ya, moveRelativeSpeed);
//
//		float friction = MAGIC_FRICTION_CONSTANT; //non-ground, non-lava, non-water, non-ladder friction -- i.e. flying
//		if (mOnGround) {
//			friction = MAGIC_FRICTION_MODIFIER * MAGIC_FRICTION_CONSTANT;
//			BlockID t = getRegion().getBlockID(Math::floor(mPos.x), Math::floor(mBB.min.y - 0.1f), Math::floor(mPos.z));
//			if (t > 0) {
//				friction = Block::mBlocks[t]->getFriction() * MAGIC_FRICTION_CONSTANT;
//			}
//		}
//		//@todo: make it easier to climb ladders
//		if (onLadder()) {
//			mFallDistance = 0;
//			if (mPosDelta.y < -0.15) {
//				mPosDelta.y = -0.15f;
//			}
//			if (isSneaking() && mPosDelta.y < 0) {
//				mPosDelta.y = 0;
//			}
//		}
//
//		move(mPosDelta);
//
//		//@todo: make it easier to climb ladders
//		if (mHorizontalCollision && onLadder()) {
//			mPosDelta.y = 0.2f;
//		}
//
//		if (hasEffect(*MobEffect::LEVITATION)) {
//			mPosDelta.y += (0.05f * (getEffect(*MobEffect::LEVITATION)->getAmplifier() + 1) - mPosDelta.y) * 0.2f;
//		}
//		
//		// Gravity is awesome!
//		if (!mNoGravity) {
//			mPosDelta.y -= 0.08f;
//			mPosDelta.y *= 0.98f;
//		}
//		
//		//apply deceleration, via a virtual method, as some mobs (like VR players) may desire to apply it differently
//		applyFinalFriction(friction * mFrictionModifier);
//	}
//}
//
//void Mob::applyFinalFriction(float finalFriction) {
//	mPosDelta.x *= finalFriction;
//	mPosDelta.z *= finalFriction;
//}
//
//void Mob::updateWalkAnim() {
//	mWalkAnimSpeedO = mWalkAnimSpeed;
//
//	if (!isRiding()) {
//		float mult = mWalkAnimSpeedMultiplier;
//		if (isOnFire() || mHurtTime != 0) {
//			mult *= 1.5f;
//		}
//
//		//make the animation slower when jumping
//		bool isJumping = !mOnGround && mPosDelta.y > 0;
//		if (isJumping) {
//			mult *= 0.35f;
//		}
//
//		float xxd = mPos.x - mPosPrev.x;
//		float zzd = mPos.z - mPosPrev.z;
//		float rotd = mYBodyRot - mYBodyRotO;
//		float wst = Math::sqrt(xxd * xxd + zzd * zzd) * 4;
//		if (wst == 0) {	//factor rotation too
//			wst = std::min(std::abs(rotd) * 0.05f, 0.5f);
//		}
//
//		if (wst > 1) {
//			wst = 1;
//		}
//
//		mWalkAnimSpeed += (wst * mult - mWalkAnimSpeed) * 0.4f;
//		mWalkAnimPos += mWalkAnimSpeed;
//	}
//	else {
//		mWalkAnimSpeedO = 0;
//		mWalkAnimSpeed = 0;
//	}
//}
//
//bool Mob::isShootable() {
//	return true;
//}
//
//void Mob::onBorn(Mob& parentLeft, Mob& parentRight) {
//}
//
//void Mob::onLove() {
//
//}
//
//void Mob::addAdditionalSaveData(CompoundTag& entityTag) {
//	Entity::addAdditionalSaveData(entityTag);
//	entityTag.put("Armor", saveArmor());
//
//	entityTag.putShort("HurtTime", (short)mHurtTime);
//	entityTag.putShort("DeathTime", (short)mDeathTime);
//	entityTag.putShort("AttackTime", (short)mAttackTime);
//	entityTag.putBoolean("CanPickUpLoot", mCanPickUpLoot);
//	entityTag.putBoolean("Persistent", mPersistent);
//	entityTag.putInt64("TargetID", (int64_t)mTargetId);
//	entityTag.putInt64("LeasherID", (int64_t)getLeashHolder());
//	entityTag.put("Attributes", SharedAttributes::saveAttributes(mAttributes.get()));
//
//	if (hasAnyEffects()) {
//		auto listTag = make_unique<ListTag>();
//
//		for(auto&& effect : mMobEffects) {
//			if (effect != MobEffectInstance::NO_EFFECT) {
//				listTag->add(effect.save());
//			}
//		}
//		entityTag.put("ActiveEffects", std::move(listTag));
//	}
//
//	if (useNewAi()) {
//		entityTag.putFloat("BodyRot", mYBodyRot);
//	}
//
//	if (mWantsToBeJockey) {
//		entityTag.putBoolean("WantsToBeJockey", true);
//	}
//}
//
//void Mob::readAdditionalSaveData(const CompoundTag& tag) {
//	Entity::readAdditionalSaveData(tag);
//	resetAttributes();
//	// Legacy, health now saved in AttributeInstance
//	if (tag.contains("Health")) {
//		serializationSetHealth(tag.getShort("Health"));
//	}
//	if (tag.contains("Armor", Tag::Type::List)) {
//		loadArmor(tag.getList("Armor"));
//	}
//
//	mHurtTime = tag.getShort("HurtTime");
//	mDeathTime = tag.getShort("DeathTime");
//	mAttackTime = tag.getShort("AttackTime");
//	mCanPickUpLoot = tag.getBoolean("CanPickUpLoot");
//	mPersistent = tag.getBoolean("Persistent");
//
//	mTargetId = (EntityUniqueID)tag.getInt64("TargetID");
//
//	EntityUniqueID leashHolder = (EntityUniqueID)tag.getInt64("LeasherID");
//
//	if ((int64_t)leashHolder != 0) {
//		setLeashHolder(leashHolder);
//	}
//
//	if (tag.contains("Attributes", Tag::Type::List)) {
//		SharedAttributes::loadAttributes(mAttributes.get(), tag.getList("Attributes"));
//	}
//	_verifyAttributes();
//
//	if (tag.contains("ActiveEffects", Tag::Type::List)) {
//		auto effects = tag.getList("ActiveEffects");
//
//		for (int i = 0; i < effects->size(); i++) {
//			Tag* effectTag = effects->get(i);
//			addEffect(MobEffectInstance::load((CompoundTag*)effectTag));
//		}
//	}
//
//	//TODO:: READ LOOT TABLE and asign from LootTables and seed
//
//	_tryLoadComponent(this, mCurrentDescription->mBreathable, mBreathableComponent, tag);
//	_tryLoadComponent(this, mCurrentDescription->mBreedable, mBreedableComponent, tag);
//
//	//mobs with the new AI glitch their rotation if the ybodyrot is not saved
//	if (useNewAi()) {
//		mYBodyRot = mYBodyRotO = tag.getFloat("BodyRot");
//	}
//
//	if (tag.getBoolean("WantsToBeJockey")) {
//		mWantsToBeJockey = true;
//	}
//}
//
//// Ensure the loaded max is valid with respect to the current entity definition.
//void Mob::_verifyAttributes() {
//	for (auto& descAttr : mCurrentDescription->mAttributes) {
//		auto currentAttr = mAttributes->getMutableInstance(descAttr.name);
//		if (currentAttr == nullptr) {
//			LOGW("Ignoring unknown attribute '%s'", descAttr.name.c_str());
//			continue;
//		}
//
//		float currentMax = currentAttr->getMaxValue();
//		if (descAttr.max != std::numeric_limits<float>::max() && currentMax == std::numeric_limits<float>::max()) {
//			currentMax = descAttr.max;
//			currentAttr->setDefaultValue(currentMax, (int)AttributeOperands::OPERAND_MAX);
//
//			// Ensure the current value is valid
//			float currentValue = currentAttr->getCurrentValue();
//			if (currentValue > currentMax) {
//				currentAttr->setDefaultValue(currentMax, (int)AttributeOperands::OPERAND_CURRENT);
//			}
//		}
//	}
//}
//
//void Mob::animateHurt() {
//	mHurtTime = mHurtDuration = 10;
//	mHurtDir = 0;
//}
//
//bool Mob::isAlive() const {
//	return !isRemoved() && getHealth() > 0;
//}
//
//void Mob::_applyRidingRotationLimits() {
//	if ((nullptr != mRiding) && enforceRiderRotationLimit()) {
//		//the rider rot limit is set on the rider, each frame
//		//mRot.y is relative, while riding, whereas the body rot is absolute
//		const float rotDelta = Math::wrapDegrees(mRot.y + 90.0f);
//		if (rotDelta > mRiderYRotLimit) {
//			mRot.y -= (rotDelta - mRiderYRotLimit);
//		}
//		else if (rotDelta < -mRiderYRotLimit) {
//			mRot.y -= (rotDelta + mRiderYRotLimit);
//		}
//
//		//use the raw rotation here.
//		mYBodyRot = mLockedBodyYRot;
//	}
//}
//
//void Mob::aiStep() {
//	//@todo? 30 lines of code here in java version
//	
//	if (mBurnsInDaylight && !mFireImmune && getLevel().getCurrentTick().getTimeStamp() & 1)
//	{
//		Dimension& dimension = getRegion().getDimension();
//		if (dimension.isDay() && !isInWaterOrRain() && !getLevel().isClientSide()) {
//			float skyDarkenEffect = dimension.getSkyDarken() / static_cast<float>(Brightness::MAX);
//			float br = getBrightness(1) - skyDarkenEffect;
//			Vec3 pos(getPos());
//			pos.y += 0.25f;
//			if (br > 0.5f && !isOnFire() && getRegion().canSeeSky(pos) && !getArmor(ArmorSlot::Head)) {
//				setOnFire(8);
//			}
//		}
//	}
//
//	if (mHurtWhenWet) {
//		if (isInWaterOrRain()) {
//			EntityDamageSource source = EntityDamageSource(EntityDamageCause::Drowning);
//			hurt(source, 1);
//		}
//	}
//
//	{
//		ScopedProfile("looting");
//		if (!getLevel().isClientSide() && mCanPickUpLoot && isAlive()) {	//&&
//																	// getLevel().getGameRules().getBoolean(GameRules.RULE_MOBGRIEFING))
//																	// {
//
//			auto& entities = getRegion().getEntities(EntityType::ItemEntity, mBB.grow(Vec3(1, 0, 1)));
//
//			for (Entity* entity : entities) {
//
//				if (entity->isRemoved() || !entity->hasCategory(EntityCategory::Item)) {
//					continue;
//				}
//
//				ItemEntity* itemEntity = static_cast<ItemEntity*>(entity);
//				if (itemEntity->hasPickUpDelay()) {
//					continue;
//				}
//
//				_pickUpItem(*itemEntity);
//			}
//		}
//	}
//
//	{
//		ScopedProfile("ai");
//		if (isImmobile()) {
//			mJumping = false;
//			mXxa = 0;
//			mYya = 0;
//			mYRotA = 0;
//			mPosDelta.x = mPosDelta.y = mPosDelta.z = 0;
//		}
//		else {
//			if (!getLevel().isClientSide()) {
//				if (useNewAi()) {
//					ScopedProfile("newAi");
//					newServerAiStep();
//				}
//				else {
//					ScopedProfile("oldAi");
//					if (useNewAi()) {
//						// this case only occurs due to the @todo above / jeb
//						setSpeed(getAttribute(SharedAttributes::MOVEMENT_SPEED).getCurrentValue());
//					}
//					updateAi();
//
//					_applyRidingRotationLimits();
//					mYHeadRot = mRot.y;
//				}
//			}
//		}
//	}
//	{
//		ScopedProfile("move");
//		if (mNoJumpDelay > 0) {
//			mNoJumpDelay--;
//		}
//
//		bool inWater = isInWater();
//		bool inLava = isInLava();
//		if(!isRiding()){
//			if (mJumping) {
//				float jumpVel = 0.04f;
//				if (mJumpVelRedux) {
//					jumpVel = 0.028f;
//					mJumpVelRedux = false;
//				}
//				if (inWater && !isSwimmer()) {
//					mPosDelta.y += jumpVel;
//				}
//				else if (inLava) {
//					mPosDelta.y += jumpVel;
//				}
//				else if (mOnGround) {
//					if (mNoJumpDelay == 0) {
//						jumpFromGround();
//						mNoJumpDelay = 10;
//					}
//				}
//			}
//			else {
//				mNoJumpDelay = 0;
//			}
//
//			mXxa *= 0.98f;
//			mYya *= 0.98f;
//			mYRotA *= 0.9f;
//
//			updateGliding();
//			travel(mXxa, mYya);
//			pushEntities();
//		}
//	}
//}
//
//void Mob::updateAi() {
//	mNoActionTime++;
//
//	Player* player = getLevel().getNearestPlayer(*this, -1);
//	checkDespawn(player);
//
//	mXxa = 0;
//	mYya = 0;
//
//	float lookDistance = 8;
//	if (mRandom.nextFloat() < 0.02f) {
//		player = getLevel().getNearestPlayer(*this, lookDistance);
//		if (player) {
//			mLookingAtId = player->getUniqueID();
//			mLookTime = 10 + mRandom.nextInt(20);
//		}
//		else {
//			mYRotA = (mRandom.nextFloat() - 0.5f) * 20;
//		}
//	}
//
//	if (mLookingAtId) {
//		Entity* lookingAt = getLevel().fetchEntity(mLookingAtId);
//		mLookingAtId = {};
//		if (lookingAt) {
//			lookAt(lookingAt, 10, getMaxHeadXRot());
//			if (mLookTime-- <= 0 || lookingAt->isRemoved() || lookingAt->distanceToSqr(*this) > lookDistance * lookDistance) {
//				mLookingAtId = {};
//			}
//			else {
//				mLookingAtId = lookingAt->getUniqueID();
//			}
//		}
//	}
//	else {
//		if (mRandom.nextFloat() < 0.05f) {
//			mYRotA = (mRandom.nextFloat() - 0.5f) * 20;
//		}
//		mRot.y += mYRotA;
//		mRot.x = mDefaultLookAngle;
//	}
//
//	bool inWater = isInWater();
//	bool inLava = isInLava();
//	if ((inWater || inLava) && mFloatsInLiquid && !isSwimmer()) {
//		mJumping = mRandom.nextFloat() < 0.8f;
//	}
//}
//
//void Mob::newServerAiStep() {
//	mNoActionTime++;
//
//	{
//		ScopedProfile("checkDespawn");
//		checkDespawn();
//		mJumping = false;
//	}
//
//	{
//		ScopedProfile("sensing");
//		mSensing->tick();
//	}
//
//	{
//		ScopedProfile("targetSelector");
//		mTargetSelector.tick();
//	}
//
//	{
//		ScopedProfile("goalSelector");
//		mGoalSelector.tick();
//	}
//
//	{
//		ScopedProfile("navigation");
//		mNavigation->tick();
//	}
//
//	{
//		ScopedProfile("mobtick");
//		_serverAiMobStep();
//	}
//
//	{
//		ScopedProfile("controls");
//		mMoveControl->tick();
//		mLookControl->tick();
//		mJumpControl->tick();
//	}
//}
//
//void Mob::_serverAiMobStep() {
//}
//
//bool Mob::isImmobile() const {
//	return mImmobile || getHealth() <= 0 || getStatusFlag(EntityFlags::NOAI);
//}
//
//void Mob::jumpFromGround() {
//	mPosDelta.y = getJumpPower();
//	if (hasEffect(*MobEffect::JUMP)) {
//		mPosDelta.y += (getEffect(*MobEffect::JUMP)->getAmplifier() + 1) * .1f;
//	}
//
//	if (isSprinting()) {
//		float rr = mRot.y * Math::DEGRAD;
//
//		mPosDelta.x -= Math::sin(rr) * 0.2f;
//		mPosDelta.z += Math::cos(rr) * 0.2f;
//	}
//
//	//play the jump sound
//	if (!isSilent()) {
//		BlockID blockID = getRegion().getBlockID(_getBlockOnPos());
//		playSound(LevelSoundEvent::Jump, getAttachPos(EntityLocation::Feet), blockID);
//	}
//}
//
//void Mob::stopRiding(bool exitFromRider, bool entityIsBeingDestroyed) {
//	mRiderYRotLimit = 181.0f; //reset this value, for safety
//
//	Entity::stopRiding(exitFromRider, entityIsBeingDestroyed);
//}
//
//void Mob::_pickUpItem(ItemEntity& entity) {
//}
//
//bool Mob::_removeWhenFarAway() {
//	return !mPersistent;
//}
//
//bool Mob::_isPersistent() const {
//	return mPersistent;
//}
//
//float Mob::getMaxHeadXRot() {
//	return 40;
//}
//
//void Mob::lookAt(Entity* e, float yMax, float xMax) {
//	float xd = e->mPos.x - mPos.x;
//	float yd;
//	float zd = e->mPos.z - mPos.z;
//
//	if (e->hasCategory(EntityCategory::Mob)) {
//		Mob* mob = (Mob*)e;
//		yd = (mPos.y + getHeadHeight()) - (mob->mPos.y + mob->getHeadHeight());
//	}
//	else {
//		yd = (e->mBB.min.y + e->mBB.max.y) / 2 - (mPos.y + getHeadHeight());
//	}
//
//	float sd = Math::sqrt(xd * xd + zd * zd);
//
//	float yRotD = (float)(atan2(zd, xd) * 180 / PI) - 90;
//	float xRotD = (float)(atan2(yd, sd) * 180 / PI);
//	mRot.x = -Math::clampRotate(mRot.x, xRotD, xMax);
//	mRot.y = Math::clampRotate(mRot.y, yRotD, yMax);
//}
//
//bool Mob::isLookingAtAnEntity() {
//	return static_cast<bool>(mLookingAtId);
//}
//
//bool Mob::checkSpawnRules(bool fromSpawner)
//{
//	auto &region = this->getRegion();
//	return !region.containsAnySolidBlocking(mBB) && 
//		   !region.containsAnyLiquid(mBB);
//}
//
//void Mob::outOfWorld() {
//	mFallDistance = 0;
//	EntityDamageSource source = EntityDamageSource(EntityDamageCause::Void);
//	hurt(source, 4);
//}
//
//float Mob::getAttackAnim(float a) {
//	float diff = mAttackAnim - mOAttackAnim;
//	if (diff < 0) {
//		diff += 1;
//	}
//	return mOAttackAnim + diff * a;
//}
//
//void Mob::performRangedAttack(Entity& target, float power) {
//
//}
//
//void Mob::handleEntityEvent(EntityEvent id, int data) {
//	if (id == EntityEvent::HURT) {
//		mInvulnerableTime = INVULNERABLE_DURATION;
//		mHurtTime = mHurtDuration = 10;
//		mHurtDir = 0;
//
//		LevelSoundEvent hurtSoundEvent = _isHeadInWater() ? LevelSoundEvent::HurtInWater : LevelSoundEvent::Hurt;
//		playSound(hurtSoundEvent, getAttachPos(EntityLocation::Head));
//
//	}
//	else if (id == EntityEvent::DEATH) {
//		//playSound(_getDeathSound(), _getSoundVolume(), getVoicePitch(), EntityLocation::Head);
//		//if (mCurrentDefinition && mCurrentDefinition->mMobSettings.mDeathSound.mSoundName.length() > 0) {
//		//	const SoundDefinition& soundDef = mCurrentDefinition->mMobSettings.mDeathSound;
//		//	playSound(soundDef.mSoundName.c_str(), soundDef.getVolume(mRandom), soundDef.getPitch(mRandom), EntityLocation::Head);
//		//}
//
//		// todo Li: some mob makes different death sounds based on its condition. Need to find a way to handle it. json has only one version. 
//		LevelSoundEvent deathSoundEvent = _isHeadInWater() ? LevelSoundEvent::DeathInWater : LevelSoundEvent::Death;
//		playSound(deathSoundEvent, getAttachPos(EntityLocation::Head));
//		getMutableAttribute(SharedAttributes::HEALTH)->resetToMinValue();
//	}
//	else if (id == EntityEvent::SPAWN_ALIVE) {
//		if (getHealth() <= 0) {
//			getMutableAttribute(SharedAttributes::HEALTH)->resetToMaxValue();
//			mDeathTime = 0;
//		}
//	}
//	else {
//		Entity::handleEntityEvent(id, data);
//	}
//}
//
//ItemInstance* Mob::getCarriedItem() {
//	return nullptr;
//}
//
//const ItemInstance* Mob::getCarriedItem() const {
//	return nullptr;
//}
//
//void Mob::setCarriedItem(const ItemInstance& item) {
//}
//
//int Mob::getItemUseDuration() {
//	return 0;
//}
//
//void Mob::setYya(float yya) {
//	mYya = yya;
//}
//
//float Mob::getSpeed() const {
//	if (useNewAi()) {
//		return mSpeed;
//	}
//
//	return getAttribute(SharedAttributes::MOVEMENT_SPEED).getCurrentValue();
//}
//
//void Mob::setSpeed(float speed) {
//	mSpeed = speed;
//	mYya = speed;
//}
//
//float Mob::getYHeadRot() const {
//	return mYHeadRot;
//}
//
//void Mob::setYHeadRot(float yHeadRot) {
//	mYHeadRotO = mYHeadRot = yHeadRot;
//}
//
//void Mob::setRiderLockedBodyRot(float degreesAngle) {
//	mLockedBodyYRot = degreesAngle;
//}
//
//void Mob::setRiderRotLimit(float degreesLimit) {
//	mRiderYRotLimit = degreesLimit;
//}
//
//bool Mob::isGliding() const {
//	return getStatusFlag(EntityFlags::GLIDING);
//}
//
//int Mob::getGlidingTicks() const {
//	return mFallFlyTicks;
//}
//
//void Mob::setWantsToBeJockey(bool jockey) {
//	mWantsToBeJockey = jockey;
//}
//
//bool Mob::wantsToBeJockey() const {
//	return mWantsToBeJockey;
//}
//
//void Mob::setJumping(bool jump) {
//	mJumping = jump;
//}
//
//void Mob::setJumpVelRedux(bool val) {
//	mJumpVelRedux = val;
//}
//
//
//float Mob::getJumpPower() const {
//	return 0.42f;
//}
//
//void Mob::setSprinting(bool shouldSprint) {
//	setStatusFlag(EntityFlags::SPRINTING, shouldSprint);
//
//	if (auto speed = getMutableAttribute(SharedAttributes::MOVEMENT_SPEED)) {
//		if (shouldSprint && !speed->hasModifier(SharedModifiers::SPRINTING_BOOST)) {
//			speed->addModifier(SharedModifiers::SPRINTING_BOOST);
//		} else if (!shouldSprint && speed->hasModifier(SharedModifiers::SPRINTING_BOOST)) {
//			speed->removeModifier(SharedModifiers::SPRINTING_BOOST);
//		}
//	}
//}
//
//bool Mob::isSprinting() const {
//	return getStatusFlag(EntityFlags::SPRINTING);
//}
//
//void Mob::_updateSprintingState() {
//	if (isSprinting() && !isInWater()) {
//		_doSprintParticleEffect();
//	}
//}
//
//void Mob::_doSprintParticleEffect() {
//	int xt = Math::floor(mPos.x);
//	int yt = Math::floor(mPos.y - 0.2f - mHeightOffset);
//	int zt = Math::floor(mPos.z);
//	BlockPos pos = BlockPos(xt, yt, zt);
//
//	auto &region = this->getRegion();
//	auto fullBlock = region.getBlockAndData(pos);
//	const Block* baseBlock = Block::mBlocks[fullBlock.id];
//
//	if (baseBlock) {
//		const BlockGraphics* blockGraphics = BlockGraphics::mBlocks[baseBlock->getId()];
//		if (blockGraphics && blockGraphics->getBlockShape() != BlockShape::INVISIBLE) {
//			float posx = mPos.x + (getLevel().getRandom().nextFloat() - 0.5f) * mBBDim.x;
//			float posy = mBB.min.y + 0.1f;
//			float posz = mPos.z + (getLevel().getRandom().nextFloat() - 0.5f) * mBBDim.x;
//			getLevel().addParticle(ParticleType::Terrain, Vec3(posx, posy, posz), Vec3(-mPosDelta.x * 4, 0.15f, -mPosDelta.z * 4), fullBlock.toInt());
//		}
//	}
//}
//
//bool Mob::useNewAi() const {
//	return true;
//}
//
//void Mob::checkDespawn() {
//	checkDespawn(getLevel().getNearestPlayer(*this, -1));
//}
//
//void Mob::checkDespawn(Mob* nearestBlocking) {
//	if (nearestBlocking != nullptr) {
//		const bool removeIfFar = _removeWhenFarAway();
//		Vec3 delta(nearestBlocking->getPos() - mPos);
//		float sd = delta.lengthSquared();
//
//		// We only tick on average up to 4.5 chunks away from a player 4.5 * 16 = 80 blocks
//		if (removeIfFar && sd > 70 * 70) {
//			remove();
//		}
//
//		if (removeIfFar && sd > 32 * 32) {
//			// Don't want to reset the time just because the random check failed.
//			if (mNoActionTime > 30 * SharedConstants::TicksPerSecond && mRandom.nextInt(800) == 0) {
//				remove();
//			}
//		}
//		else {
//			mNoActionTime = 0;
//		}
//	}
//}
//
//int Mob::getCurrentSwingDuration() {
//	if (hasEffect(*MobEffect::DIG_SPEED)) {
//		return SWING_DURATION - (1 + getEffect(*MobEffect::DIG_SPEED)->getAmplifier()) * 1;
//	}
//	if (hasEffect(*MobEffect::DIG_SLOWDOWN)) {
//		return SWING_DURATION + (1 + getEffect(*MobEffect::DIG_SLOWDOWN)->getAmplifier()) * 2;
//	}
//	return SWING_DURATION;
//}
//
//void Mob::updateAttackAnim() {
//	int swingDuration = getCurrentSwingDuration();
//	if (mSwinging) {
//		if (++mSwingTime >= swingDuration) {
//			mSwingTime = 0;
//			mSwinging = false;
//		}
//	}
//	else {
//		mSwingTime = 0;
//	}
//
//	mAttackAnim = mSwingTime / (float)swingDuration;
//}
//
//void Mob::swing() {
//	if (!mSwinging || mSwingTime >= getCurrentSwingDuration() / 2 || mSwingTime < 0) {
//		mSwingTime = -1;
//		mSwinging = true;
//	}
//}
//
//void Mob::ate() {
//}
//
//bool Mob::isSneaking() const {
//	return getStatusFlag(EntityFlags::SNEAKING);
//}
//
//bool Mob::isSleeping() const {
//	return false;
//}
//
//void Mob::setSneaking(bool value) {
//	setStatusFlag(EntityFlags::SNEAKING, value);
//}
//
//int Mob::getDamageAfterArmorAbsorb(const EntityDamageSource& source, int damage) {
//
//	if (source.getCause() == EntityDamageCause::Override ||
//		source.getCause() == EntityDamageCause::Magic ||
//		source.getCause() == EntityDamageCause::Suicide ||
//		source.getCause() == EntityDamageCause::Void ||
//		source.getCause() == EntityDamageCause::Drowning ||
//		source.getCause() == EntityDamageCause::FireTick ||
//		source.getCause() == EntityDamageCause::Fall ||
//		source.getCause() == EntityDamageCause::Suffocation) {
//		return damage;
//	}
//
//	int absorb = 25 - getArmorValue();
//	int v = (damage) * absorb + mDmgSpill;
//	hurtArmor(damage);
//	damage = v / 25;
//	mDmgSpill = v % 25;
//	return damage;
//}
//
//int Mob::getDamageAfterMagicAbsorb(const EntityDamageSource& source, int damage) {
//	if ((hasEffect(*MobEffect::DAMAGE_RESISTANCE)) || 
//		(hasEffect(*MobEffect::DAMAGE_RESISTANCE) && 
//			source.getCause() != EntityDamageCause::Override && 
//			source.getCause() != EntityDamageCause::Void && 
//			source.getCause() != EntityDamageCause::Suicide)) {
//
//		int absorbValue = (getEffect(*MobEffect::DAMAGE_RESISTANCE)->getAmplifier() + 1) * 5;
//		int absorb = 25 - absorbValue;
//		int v = (damage) * absorb + mDmgSpill;
//		damage = v / 25;
//		mDmgSpill = v % 25;
//	}
//	return damage;
//}
//
//ItemInstance* Mob::getItemSlot(EquipmentSlot slot) {
//	ItemInstance* item = nullptr;
//	if (isHandSlot(slot)) {
//		item = &mHand[toSlot(slot)];
//	} else if (isArmorSlot(slot)) {
//		item = &mArmor[toSlot(slot)];
//	}
//	return item;
//}
//
//void Mob::setItemSlot(EquipmentSlot slot, ItemInstance& item) {
//	if (isHandSlot(slot)) {
//		mHand[toSlot(slot)] = std::move(item);
//	}
//	else if (isArmorSlot(slot)) {
//		mArmor[toSlot(slot)] = std::move(item);;
//	}
//}
//
//int Mob::getArmorValue() {
//	int val = 0;
//
//	for(auto&& item : mArmor) {
//		if (!ItemInstance::isArmorItem(&item)) {
//			continue;
//		}
//
//		int baseProtection = ((ArmorItem*)item.getItem())->mDefense;
//		val += baseProtection;
//	}
//	return val;
//}
//
//float Mob::getArmorCoverPercentage() const{
//	int count = 0;
//
//	for (auto&& item : mArmor) {
//		if (!item.isNull()) {
//			count++;
//		}
//	}
//
//	return float(count) / float(ArmorSlot::_count);
//}
//
//const ItemInstance* Mob::getArmor(ArmorSlot slot) const {
//	if (mArmor[enum_cast(slot)].isNull()) {
//		return nullptr;
//	}
//
//	return &mArmor[enum_cast(slot)];
//}
//
//std::vector<const ItemInstance*> Mob::getAllArmor() const {
//	std::vector<const ItemInstance*> allArmor;
//	allArmor.reserve(enum_cast(ArmorSlot::_count));
//
//	for (auto&& item : mArmor) {
//		if (!item.isNull()) {
//			allArmor.push_back(&item);
//		}
//	}
//
//	return allArmor;
//}
//
//std::vector<ItemInstance*> Mob::getAllArmor() {
//	std::vector<ItemInstance*> allArmor;
//	allArmor.reserve(enum_cast(ArmorSlot::_count));
//
//	for (auto&& item : mArmor) {
//		if (!item.isNull()) {
//			allArmor.push_back(&item);
//		}
//	}
//
//	return allArmor;
//}
//
//std::vector<const ItemInstance*> Mob::getAllHand() const {
//	std::vector<const ItemInstance*> allWeapon;
//	allWeapon.reserve(enum_cast(HandSlot::_count));
//
//	for (auto&& item : mHand) {
//		if (!item.isNull()) {
//			allWeapon.push_back(&item);
//		}
//	}
//
//	return allWeapon;
//}
//
//std::vector<ItemInstance*> Mob::getAllHand() {
//	std::vector<ItemInstance*> allWeapon;
//	allWeapon.reserve(enum_cast(HandSlot::_count));
//
//	for (auto&& item : mHand) {
//		if (!item.isNull()) {
//			allWeapon.push_back(&item);
//		}
//	}
//
//	return allWeapon;
//}
//
//std::vector<const ItemInstance*> Mob::getAllEquipment() const {
//	std::vector<const ItemInstance*> allEquipment;
//	allEquipment.reserve(enum_cast(EquipmentSlot::_count));
//
//	for (auto&& item : mHand) {
//		if (!item.isNull()) {
//			allEquipment.push_back(&item);
//		}
//	}
//	for (auto&& item : mArmor) {
//		if (!item.isNull()) {
//			allEquipment.push_back(&item);
//		}
//	}
//
//	return allEquipment;
//}
//
//std::vector<ItemInstance*> Mob::getAllEquipment() {
//	std::vector<ItemInstance*> allEquipment;
//	allEquipment.reserve(enum_cast(EquipmentSlot::_count));
//
//	for (auto&& item : mHand) {
//		if (!item.isNull()) {
//			allEquipment.push_back(&item);
//		}
//	}
//	for (auto&& item : mArmor) {
//		if (!item.isNull()) {
//			allEquipment.push_back(&item);
//		}
//	}
//
//	return allEquipment;
//}
//
//void Mob::setArmor(ArmorSlot slot, const ItemInstance* item) {
//	if (item == nullptr) {
//		mArmor[enum_cast(slot)].setNull();
//	}
//	else {
//		mArmor[enum_cast(slot)] = *item;
//	}
//}
//
//void Mob::hurtArmor(int dmg) {
//	dmg = std::max(1, dmg / 4);
//
//	for (auto&& item : mArmor) {
//		if (!ItemInstance::isArmorItem(&item) || !ArmorItem::isDamageable(item)) {
//			continue;
//		}
//
//		item.hurtAndBreak(dmg, this);
//	}
//}
//
//int Mob::getArmorTypeHash() {
//	/* all items have an offset of +256 added. By removing this, item IDs will fit into a smaller number of bits.
//	   By additionally reducing the ID down by the smallest ID value for armor (a cloth helm), even fewer bits are
//	      required */
//	// range of IDs after removal of offset is 0-19, which fits into 5 bits
//	const int offset = 256 + Item::mHelmet_cloth->getId();
//	int armorHash = 0;
//
//	int i = 0;
//	for (auto&& item : mArmor) {
//		armorHash += ((item.getId() - offset) << (i * 5))
//			+ ((item.isEnchanted() ? 0 : 1) << (enum_cast(ArmorSlot::_count) * 5 + i + 1));
//		++i;
//	}
//
//	return armorHash;
//}
//
////void Mob::dropHeldItem() {
////	ItemInstance* item = getCarriedItem();
////	if (item) {
////		drop(item, true);
////	}
////}
////
////void Mob::dropAllGear(int lootBonusLevel) {
////	float chance = 0.085f + 0.01f * lootBonusLevel;
////
////	if (mLastHurtByPlayerTime > 0) {
////		if (getLevel().getRandom().nextFloat() < chance) {
////			dropHeldItem();
////		}
////
////		for (auto&& item : armor) {
////			if (ItemInstance::isArmorItem(&item) && getLevel().getRandom().nextFloat() < chance) {
////				drop(&item, false);
////				item.setNull();
////			}
////		}
////	}
////}
//
//void Mob::setOnFire(int seconds) {
//	// total duration is reduced by 15% per level of fire protection across all equipped armor
//	float fireProtection = 1.0f - (EnchantUtils::getTotalProtectionLevels(Enchant::Type::ARMOR_FIRE, *this) * 0.15f);
//	int totalDuration = seconds * SharedConstants::TicksPerSecond;
//
//	totalDuration = static_cast<int>(fireProtection * totalDuration);
//
//	mOnFire = std::max(mOnFire, totalDuration);
//}
//
//void Mob::sendInventory() const {
//	//TODO - packet handlers are all hard coded to always assume Player for anything inventory related, needs to be
//	// changed before this can be implemented
//	/*std::vector<ItemInstance> armorItems(4);
//	   for (int i = 0; i < ArmorSlot::_count; ++i) {
//	    armorItems[i] = armor[i].isValid() ? &armor[i] : ItemInstance();
//	   }
//
//	   ContainerSetContentPacket setContainerPacket(ContainerID::ARMOR, armorItems, 0);
//	   getLevel().packetSender->send(owner, setContainerPacket.setReliableOrdered());*/
//}
//
//Unique<ListTag> Mob::saveArmor() {
//	auto listTag = make_unique<ListTag>();
//
//	for(auto&& item : mArmor) {
//		listTag->add(item.save());
//	}
//	return listTag;
//}
//
//void Mob::loadArmor(const ListTag* listTag) {
//	if (!listTag) {
//		return;
//	}
//
//	const int count = std::min(enum_cast(ArmorSlot::_count), listTag->size());
//
//	for (int i = 0; i < count; ++i) {
//		Tag* tag = listTag->get(i);
//		if (tag->getId() != Tag::Type::Compound) {
//			continue;
//		}
//
//		mArmor[i].load(*(CompoundTag*)tag);
//	}
//}
//
//int Mob::getExperienceReward(void) const {
//	return 0;
//}
//
//void Mob::dropEquipment(const EntityDamageSource& source, int lootBonusLevel) {
//	int equipmentSlots = static_cast<int>(EquipmentSlot::_count);
//	bool wasKilledByPlayer = source.getDamagingEntity() != nullptr && EntityClassTree::isInstanceOf(*source.getDamagingEntity(), EntityType::Player);
//	for (int iterator = static_cast<int>(EquipmentSlot::_begin); iterator < equipmentSlots; ++iterator) {
//		auto equipmentSlot = EquipmentSlot(iterator);
//		ItemInstance* item = getItemSlot(equipmentSlot);
//		float dropChance = 0;
//		if (isHandSlot(equipmentSlot)) {
//			dropChance = mHandDropChance[toSlot(equipmentSlot)];
//		} else if (isArmorSlot(equipmentSlot)) {
//			dropChance = mArmorDropChance[toSlot(equipmentSlot)];
//		}
//		
//		if (source.getDamagingEntity() != nullptr && EntityClassTree::isMob(source.getDamagingEntity()->getEntityTypeId())) {
//			Mob* damagingMob = static_cast<Mob*>(source.getDamagingEntity());
//			
//			if (damagingMob->getCarriedItem() != nullptr && EnchantUtils::hasEnchant(Enchant::WEAPON_LOOT, *damagingMob->getCarriedItem())) {
//				dropChance += .01f * EnchantUtils::getEnchantLevel(Enchant::WEAPON_LOOT, *damagingMob->getCarriedItem());
//			}
//		}
//
//		bool preserve = dropChance > 1;
//		if(item != nullptr && !item->isNull() && (wasKilledByPlayer || preserve) && mRandom.nextFloat() - lootBonusLevel * 0.01f < dropChance) {
//			if (!preserve && item->isDamageableItem()) {
//				int max = Math::max(item->getMaxDamage() - 25, 1);
//				int randModif = mRandom.nextInt(max) + 1;
//				int damage = item->getMaxDamage() - mRandom.nextInt(randModif);
//				damage = Math::clamp(damage, 1, max);
//				
//				item->setAuxValue(damage);
//			}
//			spawnAtLocation(*item, 0);
//			item->setNull();
//		}
//	}
//}
//
//
//const TextureUVCoordinateSet& Mob::getItemInHandIcon(const ItemInstance* ii, int layer) {
//	return ii->getIcon(ii->getItem()->getAnimationFrameFor(*this));
//}
//
//bool Mob::canBeControlledByRider() {
//	//if (getRideableComponent()) {
//		//return getRideableComponent()->canBeControlled();
//	//}
//	return false;
//}
//
//void Mob::rideTick() {
//	Entity::rideTick();
//	mORun = mRun;
//	mRun = 0;
//	mFallDistance = 0;
//}
//
//BaseAttributeMap* Mob::getAttributes() {
//	return mAttributes.get();
//}
//
//PathNavigation& Mob::getNavigation() {
//	return *mNavigation;
//}
//
//LookControl& Mob::getLookControl() {
//	return *mLookControl;
//}
//
//MoveControl& Mob::getMoveControl() {
//	return *mMoveControl;
//}
//
//JumpControl& Mob::getJumpControl() {
//	return *mJumpControl;
//}
//
//Sensing& Mob::getSensing() {
//	return *mSensing;
//}
//
//Mob* Mob::getLastHurtByMob() {
//	return updateMobId(mLastHurtByMobId);
//}
//
//void Mob::setLastHurtByMob(Mob* mob) {
//	if (mob) {
//		if (mob->isTame() && mob->getOwner() != nullptr) {
//			mLastHurtByPlayerId = {};
//			mLastHurtByPlayerTime = PLAYER_HURT_EXPERIENCE_TIME;
//			setPersistent();
//		}
//
//		mLastHurtByMobId = mob->getUniqueID();
//		mLastHurtByMobTime = (SharedConstants::TicksPerSecond * 3);
//	}
//	else {
//		mLastHurtByMobId = {};
//		mLastHurtByMobTime = 0;
//	}
//}
//
//Player* Mob::getLastHurtByPlayer() {
//	Player* player = nullptr;
//	if (mLastHurtByPlayerId) {
//		player = getLevel().getPlayer(mLastHurtByPlayerId);
//	}
//	if (!player) {
//		mLastHurtByPlayerId = {};
//	}
//	return player;
//}
//
//void Mob::setLastHurtByPlayer(Player* player) {
//	if (player) {
//		mLastHurtByPlayerId = player->getUniqueID();
//		mLastHurtByPlayerTime = PLAYER_HURT_EXPERIENCE_TIME;
//		setPersistent();
//
//		// because a player is also a mob
//		setLastHurtByMob(player);
//
//		if (mCurrentDescription && mCurrentDescription->mOnHurtByPlayer) {
//			VariantParameterList params;
//			initParams(params);
//			mCurrentDescription->executeTrigger(*this, *mCurrentDescription->mOnHurtByPlayer, params);
//		}
//	}
//	else {
//		if (mLastHurtByMobId == mLastHurtByPlayerId) {
//			setLastHurtByMob(nullptr);
//		}
//		mLastHurtByPlayerId = {};
//		mLastHurtByPlayerTime = 0;
//	}
//}
//
//void Mob::setLastHurtMob(Entity* target) {
//	if(target->hasCategory(EntityCategory::Mob)) {
//		mLastHurtMobId = target->getUniqueID();
//	}
//	mLastHurtMobTimestamp = mTickCount;
//}
//
//int Mob::getLastHurtMobTimestamp() {
//	return mLastHurtMobTimestamp;
//}
//
//int Mob::getLastHurtByMobTimestamp() {
//	return mLastHurtByMobTime;
//}
//
//Mob* Mob::getLastHurtMob() {
//	return updateMobId(mLastHurtMobId);
//}
//
//bool Mob::canAttack(Entity* entity, bool allowInvulnerable) {
//	if (entity == nullptr) {
//		return false;
//	}
//
//	// Can't attack Ghast
//	if (!EntityClassTree::isInstanceOf(*entity, EntityType::Ghast)) {		
//		return true;
//	}
//
//	return false;
//}
//
//bool Mob::isAlliedTo(Mob* other) {
//	return false;
//}
//
//bool Mob::doHurtTarget(Entity* target) {
//	swing();
//
//	getLevel().broadcastEntityEvent(this, EntityEvent::START_ATTACKING);
//
//	int dmg = (int)getAttribute(SharedAttributes::ATTACK_DAMAGE).getCurrentValue();
//
//	if (target->hasCategory(EntityCategory::Mob)) {
//		Mob* mobTarget = static_cast<Mob*>(target);
//		setLastHurtMob(target);
//		dmg += getMeleeWeaponDamageBonus(mobTarget);
//
//		int knockbackBonus = getMeleeKnockbackBonus();
//		if (knockbackBonus > 0) {
//			if (getAttribute(SharedAttributes::KNOCKBACK_RESISTANCE).getCurrentValue() < 1.0f) {
//				target->push(Vec3(-Math::sin(mRot.y * PI / 180) * knockbackBonus * 0.5f, 0.1f, Math::cos(mRot.y * PI / 180) * knockbackBonus * 0.5f));
//			}
//			mPosDelta.x *= 0.6f;
//			mPosDelta.z *= 0.6f;
//		}
//
//		EnchantUtils::doPostDamageEffects(*mobTarget, *this);
//	}
//
//	EntityDamageByEntitySource source = EntityDamageByEntitySource(*this, EntityDamageCause::EntityAttack);
//
//	if (target->hurt(source, dmg)) {
//		return true;
//	}
//	return false;
//}
//
//Mob* Mob::updateMobId(EntityUniqueID& entityId) {
//	Mob* mob = nullptr;
//	if (entityId) {
//		mob = getLevel().getMob(entityId);
//	}
//	if (!mob) {
//		entityId = {};
//	}
//	return mob;
//}
//
//void Mob::resetAttributes() {
//	for (auto& i : *mAttributes) {
//		i.second.resetToDefaultValue();
//	}
//}
//
//void Mob::pushEntities() {
//	ScopedProfile("push");
//	Player* player = nullptr;
//	if (hasCategory(EntityCategory::Player)) {
//		player = (Player*)this;
//	}
//
//	if (!player || !player->isSleeping()) {
//		auto& entities = getRegion().getEntities(this, mBB.grow(Vec3(0.2f, 0, 0.2f)));
//
//		// Do not push player when not 'trying' to move (through move input).
//		// This push behavior matches the Java version etc.
//		bool thisIsPlayer = (player != nullptr);
//		bool playerShouldNotBePushed = thisIsPlayer && !player->mHasMoveInput;
//
//		bool previousRideFound = false;
//		Entity* ridingPrev = getLevel().fetchEntity(mRidingPrevID);
//
//		for (unsigned int i = 0; i < entities.size(); i++) {
//			Entity* e = entities[i];
//
//			// A Player cannot be pushed by other entities
//			if (e->hasCategory(EntityCategory::Player)) {
//				continue;
//			}
//
//			// We do not want mobs to push block entities but they should be pushable by other things :)
//			if ( e->isPushable() && !e->hasCategory(EntityCategory::Item) && ridingPrev != this && ridingPrev != e) {
//				// Don't push ourself if we are the player or if we can't be pushed for another reason (horse being ridden)
//				bool pushSelfOnly = playerShouldNotBePushed || (thisIsPlayer && e->hasCategory(EntityCategory::Animal)) || !isPushable();
//				e->push(*this, pushSelfOnly);
//			}
//
//			previousRideFound |= (e == ridingPrev);
//		}
//
//		// Ensure the last riding entity is included for sure in the check if close enough to the player.
//		// This will prevent it from being pushed until the player is a little farther from it.
//		if (player && ridingPrev && !previousRideFound && mBB.grow(Vec3(0.5f, 1.f, 0.5f)).intersects(ridingPrev->mBB)) {
//			previousRideFound = true;
//		}
//
//		if (ridingPrev && !previousRideFound) {
//			mRidingPrevID = {};
//		}
//	}
//}
//
//bool Mob::shouldDespawn() const {
//	return !mPersistent;
//}
//
//void Mob::setInvisible(bool value) {
//	setStatusFlag(EntityFlags::CAN_SHOW_NAME, !value);
//	setStatusFlag(EntityFlags::INVISIBLE, value);
//}
//
//void Mob::addEffect(const MobEffectInstance& effect) {
//	if (!canBeAffected(effect)) {
//		return;
//	}
//
//	mMobEffects.resize(std::max(mMobEffects.size(), (size_t)effect.getId() + 1), MobEffectInstance::NO_EFFECT);
//
//	if (mMobEffects[effect.getId()] != MobEffectInstance::NO_EFFECT) {
//		// replace effect and update
//		mMobEffects[effect.getId()].update(effect);
//		onEffectUpdated(mMobEffects[effect.getId()]);
//	}
//	else {
//		//TODO this new seems entirely useless
//		mMobEffects[effect.getId()] = effect;
//		onEffectAdded(mMobEffects[effect.getId()]);
//	}
//}
//
//void Mob::removeEffectNoUpdate(int effectId) {
//	if (effectId >= (int)mMobEffects.size()) {
//		return;
//	}
//
//	mMobEffects[effectId] = MobEffectInstance::NO_EFFECT;
//}
//
//int Mob::getActiveEffectCount() const {
//	int effectCount = 0;
//
//	for (unsigned i = 0; i < mMobEffects.size(); i++) {
//		if (mMobEffects[i] != MobEffectInstance::NO_EFFECT) {
//			++effectCount;
//		}
//	}
//	return effectCount;
//}
//
//void Mob::removeEffect(int effectId) {
//	if (effectId >= (int)mMobEffects.size()) {
//		return;
//	}
//
//	if (mMobEffects[effectId] != MobEffectInstance::NO_EFFECT) {
//		onEffectRemoved(mMobEffects[effectId]);
//
//		mMobEffects[effectId] = MobEffectInstance::NO_EFFECT;
//	}
//}
//
//void Mob::removeAllEffects() {
//	removeEffectParticles();
//
//	for (int i = 0; i < MobEffect::NUM_EFFECTS; i++) {
//		removeEffect(i);
//	}
//
//	removeEffectParticles();
//}
//
//bool Mob::canBeAffected(const MobEffectInstance& effect) {
//	// For when posion damage is added to an entity basicly the same frame as it died,
//	// for more info, see how the spiders adds poison:
//	// playerHurt -> playerDie -> addPosition
//	if(!isAlive()) {
//		return false;
//	}
//			
//	if (hasFamily("undead")) {
//		int id = effect.getId();
//		if (id == MobEffect::REGENERATION->getId() || id == MobEffect::POISON->getId()) {
//			return false;
//		}
//	}
//	return true;
//}
//
//void Mob::tickEffects() {
//
//	for(auto i : range(mMobEffects.size())) {
//		if (!mMobEffects[i].tick(this)) {
//			removeEffect(i);
//		}
//	}
//
//	if (mEffectsDirty) {
//		if (!getLevel().isClientSide()) {
//			updateInvisibilityStatus();
//		}
//
//		mEffectsDirty = false;
//	}
//
//	int colorValue = mEntityData.getInt(enum_cast(EntityDataIDs::EFFECT_COLOR));
//	bool ambient = mEntityData.getInt8(enum_cast(EntityDataIDs::EFFECT_AMBIENCE)) > 0;
//	
//	// colorValue is only non-zero when there are effects on the mob, and the blended color is always correct given a collection of effects
//	if (hasAnyEffects() || colorValue != 0) {
//		bool doParticle = false;
//
//		if (!isInvisible()) {
//			doParticle = mRandom.nextBoolean();
//		}
//		else {
//			doParticle = mRandom.nextInt(15) == 0;	// much fewer particles when invisible
//		}
//
//		if (ambient) {
//			doParticle &= mRandom.nextInt(5) == 0;
//		}
//
//		if (doParticle) {
//			float posx = mPos.x + (mRandom.nextFloat() - 0.5f) * mBBDim.x;
//			float posy = mPos.y + mRandom.nextFloat() * mBBDim.y - mHeightOffset;
//			float posz = mPos.z + (mRandom.nextFloat() - 0.5f) * mBBDim.x;
//
//			getLevel().addParticle(ambient ? ParticleType::MobSpellAmbient
//				: ParticleType::MobSpell,
//				Vec3(posx, posy, posz),
//				Vec3::ZERO,
//				colorValue);
//		}
//	}
//}
//
//void Mob::onEffectAdded(MobEffectInstance& effect) {
//	mEffectsDirty = true;
//	// Apply the effect if it will actually do something
//	if (mLevel && !getLevel().isClientSide() && effect.getId() != MobEffectInstance::NO_EFFECT.getId()) {
//		effect.applyEffects(this);
//	}
//
//	// TODO: Move this, feels dirty.
//	//   The reason this is here is because Entity has a burn() function, but
//	//   it can't check MobEffects there because those are on Mob.
//	//   Java gets away with this because burn() is on LivingEntity, as are MobEffects
//	if (effect.getId() == MobEffect::FIRE_RESISTANCE->getId() && !mAlwaysFireImmune) {
//		mFireImmune = true;
//	}
//}
//
//void Mob::onEffectUpdated(const MobEffectInstance& effect) {
//	mEffectsDirty = true;
//	if (/*doRefreshAttributes &&*/ !getLevel().isClientSide()) {
//		// TODO: Implement timer override so we don't just restart mob effects here
//		effect.removeEffects(this);
//		effect.applyEffects(this);
//	}
//}
//
//void Mob::onEffectRemoved(MobEffectInstance& effect) {
//	mEffectsDirty = true;
//	if (!getLevel().isClientSide()) {
//		effect.removeEffects(this);
//	}
//
//	if (effect.getId() == MobEffect::FIRE_RESISTANCE->getId() && !mAlwaysFireImmune) {
//		mFireImmune = false;
//	}
//}
//
//void Mob::updateInvisibilityStatus() {
//	if (!hasAnyEffects() || mMobEffects.empty()) {
//		removeEffectParticles();
//		setInvisible(false);
//	}
//	else {
//		auto colorValue = MobEffectInstance::getColorValue(mMobEffects);
//		auto effect = (SynchedEntityData::TypeInt8)(MobEffectInstance::areAllEffectsAmbient(mMobEffects) ? 1 : 0);
//		mEntityData.set(enum_cast(EntityDataIDs::EFFECT_AMBIENCE), effect);
//		mEntityData.set(enum_cast(EntityDataIDs::EFFECT_COLOR), colorValue.toARGB());
//		setInvisible(hasEffect(*MobEffect::INVISIBILITY));
//	}
//
//	mEntityData.markDirty(enum_cast(EntityDataIDs::EFFECT_AMBIENCE));
//	mEntityData.markDirty(enum_cast(EntityDataIDs::EFFECT_COLOR));
//}
//
//void Mob::removeEffectParticles() {
//	mEntityData.set(enum_cast(EntityDataIDs::EFFECT_AMBIENCE), (SynchedEntityData::TypeInt8)0);
//	mEntityData.set(enum_cast(EntityDataIDs::EFFECT_COLOR), (SynchedEntityData::TypeInt)0);
//}
//
//bool Mob::hasEffect(const MobEffect& effect) const {
//	return getEffect(effect) != nullptr;
//}
//
//const MobEffectInstance* Mob::getEffect(const MobEffect& effect) const {
//	if (mMobEffects.size() > effect.getId()) {
//		if (mMobEffects[effect.getId()] != MobEffectInstance::NO_EFFECT) {
//			return &mMobEffects[effect.getId()];
//		}
//	}
//
//	return nullptr;
//}
//
//bool Mob::hasAnyEffects() {
//	for (auto&& effect : mMobEffects) {
//		if (effect != MobEffectInstance::NO_EFFECT) {
//			return true;
//		}
//	}
//	return false;
//}
//
//const MobEffectInstanceList& Mob::getAllEffects() {
//	return mMobEffects;
//}
//
//void Mob::registerAttributes() {
//	/*
//	adors
//	This is going to need to be moved into some sort of base mob.shared definition
//	before we remove it here.
//	*/
//	auto& health = mAttributes->registerAttribute(SharedAttributes::HEALTH);
//	health.setDelegate(make_unique<HealthAttributeDelegate>(health, this));
//	health.setDefaultValue(20.0f, enum_cast(AttributeOperands::OPERAND_CURRENT));
//
//	mAttributes->registerAttribute(SharedAttributes::ABSORPTION).setMaxValue(16.0f);
//	mAttributes->registerAttribute(SharedAttributes::KNOCKBACK_RESISTANCE).setDefaultValue(1.0f, enum_cast(AttributeOperands::OPERAND_MAX));
//	mAttributes->registerAttribute(SharedAttributes::MOVEMENT_SPEED).setDefaultValue(0.7f, enum_cast(AttributeOperands::OPERAND_CURRENT));
//	mAttributes->registerAttribute(SharedAttributes::LUCK).setRange(-1024.0f, 0.0f, 1024.0f);
//	mAttributes->registerAttribute(SharedAttributes::FALL_DAMAGE).setDefaultValue(1.0f, enum_cast(AttributeOperands::OPERAND_CURRENT));
//	mAttributes->registerAttribute(SharedAttributes::FOLLOW_RANGE).setRange(0.0f, 16.0f, 2048.0f);
//}
//
//void Mob::teleportTo(const Vec3& pos, int cause /*= 0*/, int sourceEntityType /*= enum_cast(EntityType::Undefined)*/) {
//	Entity::teleportTo(pos);
//	mInterpolation.stop();
//}
//
//AttributeInstance* Mob::getMutableAttribute(const Attribute& attribute) {
//	return mAttributes->getMutableInstance((attribute));
//}
//
//const AttributeInstance& Mob::getAttribute(const Attribute& attribute) const {
//	return mAttributes->getInstance(attribute);
//}
//
//bool Mob::hasAttributeBuff(AttributeBuffType type) const {
//	for (auto& instance : * mAttributes) {
//		for (auto& buff : instance.second.getBuffs()) {
//			if (buff.getType() == type) {
//				return true;
//			}
//		}
//	}
//
//	return false;
//}
//
//int Mob::getHealth() const {
//	return (int)getAttribute(SharedAttributes::HEALTH).getCurrentValue();
//}
//
//int Mob::getMaxHealth() const {
//	return (int)getAttribute(SharedAttributes::HEALTH).getMaxValue();
//}
//
//void Mob::serializationSetHealth(int newHealth) {
//	getMutableAttribute(SharedAttributes::HEALTH)->serializationSetValue((float)newHealth, enum_cast(AttributeOperands::OPERAND_CURRENT));
//}
//
//void Mob::setCanPickUpLoot(bool canPickUp) {
//	mCanPickUpLoot = canPickUp;
//}
//
//bool Mob::canPickickUpLoot() const {
//	return mCanPickUpLoot;
//}
//
//void Mob::tickAttributes() {
//	for (auto& attribute : *mAttributes) {
//		attribute.second.tick();
//	}
//}
//
//void Mob::buildDebugInfo(std::string& out) const {
//	if (mCurrentDescription) {
//		out += mCurrentDescription->mIdentifier;
//		out += "\n";
//	}
//	float speed = getAttribute(SharedAttributes::MOVEMENT_SPEED).getCurrentValue();
//	out += Util::format("Health: %d/%d\n", getHealth(), getMaxHealth());
//	out += Util::format("Speed: %f\n", speed);
//	mGoalSelector.buildDebugInfo(out);
//}
//
//float Mob::getXxa() const {
//	return mXxa;
//}
//float Mob::getYya() const {
//	return mYya;
//}
//
//float Mob::getYRotA() const {
//	return mYRotA;
//}
//void Mob::setYRotA(float y) {
//	mYRotA = y;
//}
//
//void Mob::incrementArrowCount(const int count) {
//	mArrowCount += count;
//}
//
//void Mob::setSurfaceMob(bool isSurfaceMob) {
//	mSurfaceMob = isSurfaceMob;
//}
//
//bool Mob::canAddRider(Entity& mob) const {
//	//return false;
//	if (getRideableComponent()) {
//		return getRideableComponent()->canAddRider(mob);
//	}
//	return getLevel().isClientSide();
//}
//
//void Mob::_addSpeedBonus(const mce::UUID & attributeID, const std::string& attributeName, float speedModifier) {
//
//	// Make sure we don't apply the same bonus twice
//	_removeSpeedBonus(attributeID);
//
//	AttributeInstance* speedAttribute = getMutableAttribute(SharedAttributes::MOVEMENT_SPEED);
//	speedAttribute->addModifier(std::make_shared<AttributeModifier>(attributeID, attributeName, speedModifier,
//		enum_cast(AttributeModifierOperation::OPERATION_MULTIPLY_TOTAL),
//		enum_cast(AttributeOperands::OPERAND_CURRENT)));
//}
//
//void Mob::_removeSpeedBonus(const mce::UUID & attributeID) {
//	AttributeInstance* speedAttribute = getMutableAttribute(SharedAttributes::MOVEMENT_SPEED);
//	speedAttribute->removeModifier(attributeID);
//}
//
//float Mob::getFlightSpeed() {
//	return mFlyingSpeed;
//}
//
//void Mob::setFlightSpeed(float flightSpeed) {
//	mFlyingSpeed = flightSpeed;
//}
//
//void Mob::onPlayerDimensionChanged(Player* player, DimensionId destinationDimension) {
//	mGoalSelector.onPlayerDimensionChanged(player, destinationDimension);
//}
//
//void Mob::updateGliding() {
//	if (getLevel().isClientSide()) {
//		return;
//	}
//
//	bool gliding = isGliding();
//
//	if (gliding && !mOnGround && !isRiding() && !isInWater()) {
//		ItemInstance &itemSlot = mArmor[enum_cast(ArmorSlot::Torso)];
//
//		if (!itemSlot.isNull() && ArmorItem::isFlyEnabled(itemSlot)) {
//			// Damage the elytra for 1 damage every second active.
//			if (((mFallFlyTicks + 1) % SharedConstants::TicksPerSecond == 0)) {
//				itemSlot.hurtAndBreak(1, this);
//			}
//		}
//		else {
//			gliding = false;
//		}
//	}
//	else {
//		gliding = false;
//	}
//
//	setStatusFlag(EntityFlags::GLIDING, gliding);
//}
//
//bool Mob::canBePulledIntoVehicle() const {
//	return true;
//}