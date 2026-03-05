/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/entity/Entity.h"
#include "world/level/Level.h"
//#include "world/entity/ai/goal/GoalSelector.h"
//#include "MovementInterpolator.h"
#include "world/item/ItemInstance.h"


class LookControl;
class MoveControl;
class JumpControl;
class BodyControl;
class PathNavigation;
class Sensing;
class BaseAttributeMap;
class AttributeInstance;
class Attribute;
class MobEffectInstance;
class MobEffect;
class MovementInterpolator;
class LootTable;
class Village;

enum class AttributeBuffType;

class Mob : public Entity {
public:
	// Constructor for Player classes
	Mob(Level& level);

//	// Constructor for everything else
//	Mob(EntityDefinitionGroup& definitions, const EntityDefinitionIdentifier& definitionName);
//	virtual void initializeComponents(InitializationMethod method, const VariantParameterList &params) override;
//	virtual void reloadHardcodedClient(InitializationMethod method, const VariantParameterList &params) override;
//
//	static const int INVULNERABLE_DURATION = 20;
//
//	//	@note: Find a better way to handle these public variables as they're touched and looked at by other classes
//	//	but having them with both a getter and setter seems a waste since it just results in the same as them being
//	//	public but with an addition function call to handle. Also we don't want public member variables. :(
//	float mYBodyRot = 0, mYBodyRotO = 0;
//	float mYHeadRot = 0, mYHeadRotO = 0;
//
//	int mLastHealth = 0;
//
//	int mHurtTime = 0;
//	int mHurtDuration = 0;
//	float mHurtDir = 0.f;
//	int mLastHurtByPlayerTime = 0;
//
//	int mDeathTime = 0;
//	int mAttackTime = 0;
//	float mOTilt, mTilt;
//
//	int mLookTime = 0;
//	float mFallTime = 0.f;
//
//	//Replace this by using Goals when we switch completely to new AI
//	bool mFloatsInLiquid = true;	 // *** Def ***
//	bool mBurnsInDaylight = false;	 // *** Def ***
//
//
//	float mWalkAnimSpeedO = 0.f;	 // ??? Def ???
//	float mWalkAnimSpeed = 0.f;		 // *** Def ***
//	float mWalkAnimPos = 0.f;
//
//	//LootTable* mLootTable;
//	RandomSeed mLootTableSeed;
//	Vec3 mElytraRot;
//
//
//
//	virtual ~Mob();
//
//	void resetAttributes();
//	virtual void knockback(Entity* source, int dmg, float xd, float zd, float power = 1.0f);
//	virtual void die(const EntityDamageSource& source);
//	virtual void resolveDeathLoot(bool wasKilledByPlayer, int lootBonusLevel, const EntityDamageSource& source);
//	virtual void playerTouch(Player& player) override;
//
//	virtual bool canSee(const Entity& target) const;
//
//	virtual void spawnAnim();
//
//	bool isAlive() const override;
//	bool isPickable() override;
//	bool isPushable() const override;
//	bool isShootable() override;
//	virtual bool isSleeping() const;
//	bool isSneaking() const override;
//	virtual void setSneaking(bool value);
//	virtual bool isSprinting() const;
//	virtual void setSprinting(bool value);
//	bool canBePulledIntoVehicle() const override;
//
//	float getHeadHeight() const override;
//	virtual float getVoicePitch();
//
//	void playSpawnSound();
//	virtual void playAmbientSound();
//	virtual int getAmbientSoundPostponeTicks(); // *** Def ***
//	virtual const TextureUVCoordinateSet& getItemInHandIcon(const ItemInstance* item, int layer);
//
//	void lerpTo(const Vec3& pos, const Vec2& rot, int steps) override;
//
//	void setYya(float yya);
//	virtual float getSpeed() const;
//	virtual void setSpeed(float speed);
//	void setJumping(bool jump);
//	void setJumpVelRedux(bool val);
//	virtual float getJumpPower() const;
//
//	void normalTick() override;
//	void baseTick() override;
//
//	virtual void heal(int heal);
//	void healEffects(int amountHealed);
//
//	virtual bool hurtEffects(const EntityDamageSource& source, int damage, bool knock = true, bool ignite = false);
//
//	virtual int getMeleeWeaponDamageBonus(Mob* target);
//	virtual int getMeleeKnockbackBonus();
//
//	virtual void actuallyHurt(int dmg, const EntityDamageSource* source = nullptr, bool bypassArmor = false);
//	void animateHurt() override;
//	virtual bool isInvertedHealAndHarm() const;
//
//	virtual bool doFireHurt(int amount) override;
//
//	virtual void travel(float xa, float ya);
//	virtual void applyFinalFriction(float finalFriction);
//	virtual void updateWalkAnim();
//
//	virtual void aiStep();
//	
//	virtual void pushEntities();
//
//	virtual void lookAt(Entity* e, float yMax, float xMax);
//	virtual bool isLookingAtAnEntity();
//	virtual bool checkSpawnRules(bool fromSpawner);
//
//	virtual bool shouldDespawn() const;
//
//	virtual float getAttackAnim(float a);
//	virtual void performRangedAttack(Entity& target, float power);
//
//	virtual void stopRiding(bool exitFromRider = true, bool entityIsBeingDestroyed = false) override;
//
//	virtual float getYHeadRot() const override;
//	void setYHeadRot(float yHeadRot);
//
//	void handleEntityEvent(EntityEvent id, int data) override;
//
//	virtual ItemInstance* getCarriedItem();
//	virtual const ItemInstance* getCarriedItem() const;
//	virtual void setCarriedItem(const ItemInstance& item);
//	virtual int getItemUseDuration();
//
//	virtual void swing();
//
//	virtual void ate();
//
//	PathNavigation& getNavigation();
//	LookControl& getLookControl();
//	MoveControl& getMoveControl();
//	JumpControl& getJumpControl();
//	Sensing& getSensing();
//
//	virtual float getMaxHeadXRot();
//
//	bool isImmobile() const override;
//
//	virtual Mob* getLastHurtByMob();
//	virtual void setLastHurtByMob(Mob* mob);
//
//	virtual Player* getLastHurtByPlayer();
//	virtual void setLastHurtByPlayer(Player* player);
//
//	virtual Mob* getLastHurtMob();
//	virtual void setLastHurtMob(Entity* target);
//
//	int getLastHurtMobTimestamp();
//	int getLastHurtByMobTimestamp();
//
//	virtual bool canAttack(Entity* entity, bool allowInvulnerable);
//
//	virtual bool isAlliedTo(Mob* other);
//
//	virtual bool doHurtTarget(Entity* target);
//
//	virtual bool canBeControlledByRider(); // *** Def ***
//
//	virtual AttributeInstance* getMutableAttribute(const Attribute& attribute);
//	virtual const AttributeInstance& getAttribute(const Attribute& attribute) const;
//
//	bool hasAttributeBuff(AttributeBuffType) const;
//	BaseAttributeMap* getAttributes();
//	void _sendDirtyMobData();
//
//	virtual int getEquipmentCount(void) const;
//
//	virtual int getArmorValue();
//	virtual float getArmorCoverPercentage() const;
//	virtual void hurtArmor(int dmg);
//	virtual void setArmor(ArmorSlot slot, const ItemInstance* item);
//	virtual const ItemInstance* getArmor(ArmorSlot slot) const;
//
//	virtual std::vector<const ItemInstance*> getAllArmor() const;
//	virtual std::vector<ItemInstance*> getAllArmor();
//	virtual std::vector<const ItemInstance*> getAllHand() const;
//	virtual std::vector<ItemInstance*> getAllHand();
//	virtual std::vector<const ItemInstance*> getAllEquipment() const;
//	virtual std::vector<ItemInstance*> getAllEquipment();
//
//	virtual int getArmorTypeHash();
//	//virtual void dropHeldItem();
//	void setOnFire(int seconds) override;
//
//	virtual void sendInventory() const;
//
//	void setInvisible(bool value);
//
//	void addEffect(const MobEffectInstance& effect);
//	void removeEffect(int effectId);
//	void removeAllEffects();
//	void removeEffectNoUpdate(int effectId);
//	int getActiveEffectCount() const;
//	virtual bool canBeAffected(const MobEffectInstance& effect);
//	bool hasEffect(const MobEffect& effect) const;
//	const MobEffectInstance* getEffect(const MobEffect& effect) const;
//	bool hasAnyEffects();
//	const MobEffectInstanceList& getAllEffects();
//
//	int getHealth() const;
//	int getMaxHealth() const;
//	void serializationSetHealth(int newHealth);
//
//	void setCanPickUpLoot(bool canPickUp);
//	bool canPickickUpLoot() const;
//
//	void buildDebugInfo(std::string& out) const override;
//	
//
//	float getXxa() const;
//	float getYya() const;
//	float getYRotA() const;
//
//	void setYRotA(float y);
//
//	void incrementArrowCount(const int count);
//
//	void setSurfaceMob(bool isSurfaceMob);
//
//	virtual bool canAddRider(Entity& mob) const override;
//	virtual int getDamageAfterMagicAbsorb(const EntityDamageSource& source, int damage);
//
//	ItemInstance* getItemSlot(EquipmentSlot slot);
//	void createAIGoals();
//
//	virtual void onBorn(Mob& parentLeft, Mob& parentRight);
//	virtual void onLove();
//	void setItemSlot(EquipmentSlot slot, ItemInstance& item);
//	Weak<Village> const& getVillage() const;
//	
//	//virtual bool canInteractWith(Player& player) override;
//	//virtual std::string getInteractText(Player& player) override;
//
//	enum class TravelType { Water, Lava, Ground, Air };
//	TravelType getTravelType();
//	float calcMoveRelativeSpeed(TravelType travelType);
//	virtual float getWaterSlowDown() const;
//
//	void setRiderLockedBodyRot(float degreesAngle);
//	void setRiderRotLimit(float degreesLimit);
//
//	bool isGliding() const;
//	int getGlidingTicks() const;
//
//	void setWantsToBeJockey(bool jockey);
//	bool wantsToBeJockey() const;
//
//	virtual bool hasComponent(const std::string &name) const override;
//
//	float getFlightSpeed();
//	void setFlightSpeed(float flightSpeed);
//
//	void onPlayerDimensionChanged(Player* player, DimensionId destinationDimension);
//
//	virtual void teleportTo(const Vec3& pos, int cause = 0, int sourceEntityType = enum_cast(EntityType::Undefined)) override;
//
//protected:
//	void addAdditionalSaveData(CompoundTag& entityTag) override;
//	void readAdditionalSaveData(const CompoundTag& tag) override;
//
//	virtual void causeFallDamage(float distance) override;
//
//	virtual void outOfWorld() override;
//	virtual bool _removeWhenFarAway();
//	bool _isPersistent() const;
//	bool _hurt(const EntityDamageSource& source, int damage, bool knock, bool ignite) override;
//
//	virtual void jumpFromGround();
//	virtual void _pickUpItem(ItemEntity& entity);
//
//	virtual void updateAi();
//	virtual void newServerAiStep();
//	virtual void _serverAiMobStep();
//
//	virtual void _playStepSound(const BlockPos& pos, int t) override;
//
//	virtual int getDamageAfterArmorAbsorb(const EntityDamageSource& source, int damage);
//	
//	virtual int getExperienceReward(void) const; // *** Def ***
//	
//	virtual void dropEquipment(const EntityDamageSource& source, int lootBonusLevel);
//
//
//	virtual bool useNewAi() const; // *** Def ***
//	void checkDespawn(Mob* nearestBlocking);
//	void checkDespawn();
//	void updateAttackAnim();
//	int getCurrentSwingDuration();
//	virtual void rideTick() override;
//
//	bool shouldDropExperience() const { return !isBaby(); }
//
//	ItemInstance mHand[enum_cast(HandSlot::_count)];
//	float mHandDropChance[enum_cast(HandSlot::_count)];
//	ItemInstance mArmor[enum_cast(ArmorSlot::_count)];
//	float mArmorDropChance[enum_cast(ArmorSlot::_count)];
//
//	virtual void tickDeath();
//	void tickAttributes();
//	void tickEffects();
//	virtual void onEffectAdded(MobEffectInstance& effect);
//	virtual void onEffectUpdated(const MobEffectInstance& effect);
//	virtual void onEffectRemoved(MobEffectInstance& effect);
//	void updateInvisibilityStatus();
//	void removeEffectParticles();
//	void registerAttributes();
//
//	// 
//	void _addSpeedBonus(const mce::UUID & attributeID, const std::string& attributeName, float speedModifier);
//	void _removeSpeedBonus(const mce::UUID & attributeID);
//
//	void _applyRidingRotationLimits();
//
//	void _doSprintParticleEffect();
//
//	std::unique_ptr<BaseAttributeMap> mAttributes;
//	MobEffectInstanceList mMobEffects;
//
//	float mWalkAnimSpeedMultiplier = 1.f; // *** Def ***
//	bool mSwinging = false;
//	int mSwingTime = 0;
//	int mNoActionTime = 0;
//
//	float mDefaultLookAngle = 0;  // *** Def ***
//	float mFrictionModifier = 1.f;  // *** Def ***
//	
//	float mFlyingSpeed = 0.02f;
//
//	int mDeathScore = 0;
//	float mORun = 0, mRun = 0;
//	float mAnimStep = 0, mAnimStepO = 0;
//	
//	float mLockedBodyYRot = 0.0f;
//	float mRiderYRotLimit = 181.0f;
//
//	MovementInterpolator mInterpolation;
//
//	int mLastHurt = 0;
//	int mDmgSpill = 0;
//
//	//bool hasHair;
//	//bool mAllowAlpha = true;
//	bool mJumping = false; 
//	bool mJumpVelRedux = false;
//	int mNoJumpDelay = 0;
//	float mSpeed = 0;
//
//	Unique<LookControl> mLookControl;
//	Unique<MoveControl> mMoveControl;
//	Unique<JumpControl> mJumpControl;
//	Unique<BodyControl> mBodyControl;
//	Unique<PathNavigation> mNavigation;
//	Unique<Sensing> mSensing;
//
//	GoalSelector mGoalSelector;
//	GoalSelector mTargetSelector;
//
//	//@note: This is a temporary fix for DamageSource that bypasses armor
//	//       or creating two functions, that does virtually the same
//	//       except one bypasses armor. It's enough virtual calls and chains
//	//       of super-calls for me to think this is safer.
//	//bool mBypassArmor = false;
//	float mXxa = 0.f, mYya = 0, mYRotA = 0;
//	bool mHasMoveInput = false;
//
//	bool mSurfaceMob = false;  // *** Def ***
//
//	bool mDead = false;
//
//	Weak<Village> mVillage;
//	
//	bool mWantsToBeJockey = false;
//
//	int mAmbientPlayBackInterval = 2000;
//
//	bool mSpawnedXP = false;
//
//private:
//	Mob* updateMobId(EntityUniqueID& entityId);
//
//	void _updateSprintingState();
//	void _verifyAttributes();
//
//	Unique<ListTag> saveArmor();
//	void loadArmor(const ListTag* listTag);
//
//	void updateGliding();
//	bool _isHeadInWater();
//
//	static const int SWING_DURATION = 6;
	static const int PLAYER_HURT_EXPERIENCE_TIME;
//
//	bool mEffectsDirty = false;
//
//	EntityUniqueID mLookingAtId = {};
//	EntityUniqueID mLastHurtMobId = {};
//	EntityUniqueID mLastHurtByMobId = {};
//	EntityUniqueID mLastHurtByPlayerId = {};
//
//	int mLastHurtMobTimestamp = 0;
//	int mLastHurtByMobTime = 0;
//	int mAmbientSoundTime = 0;
//
//	bool mCanPickUpLoot = false;	// *** Def ***
//
//	float mOAttackAnim = 0.f;
//	float mAttackAnim = 0.f;
//
//	int mArrowCount = 0;
//	int mRemoveArrowTime = 0;
//	
//	int mFallFlyTicks = 0;
};
