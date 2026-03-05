/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

//#include "EntityLink.h"
//#include "Core/Math/Color.h"
//#include "world/Direction.h"
//#include "world/entity/components/ExplodeComponent.h"
//#include "world/entity/EntityRendererId.h"
#include "world/entity/EntityTypes.h"
//#include "world/entity/EntityDefinitionDiffList.h"
#include "world/phys/Vec2.h"
#include "world/phys/Vec3.h"
//#include "world/entity/SynchedEntityData.h"
//#include "world/level/block/PortalBlock.h"
#include "world/level/ChunkPos.h"
//#include "world/level/material/MaterialType.h"
#include "world/phys/AABB.h"
//#include "world/entity/EntityCategory.h"
//#include "world/item/EquipmentSlot.h"
#include "world/Direction.h" 
//#include "EntityLink.h"
//#include "world/level/material/MaterialType.h"
#include "util/Random.h"
#include "EntityUniqueID.h"

//#include "util/VariantParameterList.h"

class Level;
//class Block;
//class Player;
//class EntityPos;
//class Material;
//class ItemEntity;
//class ItemInstance;
//class CompoundTag;
//class BlockSource;
//class FishingHook;
//class Explosion;
//class Block;
//class MoveEntityPacketData;
//class SynchedEntityData;
//class ListTag;
//class EntityDamageSource;
//class Packet;
//class ChangeDimensionPacket;
//class Dimension;
//class Mob;
//class Container;
//class EntityDefinitionGroup; 
//class EntityDefinitionDescriptor;
//class EntityInteraction;
//
////Components
//class AddRiderComponent;
//class AgeableComponent;
//class AngryComponent;
//class BoostableComponent;
//class BreathableComponent;
//class BreedableComponent;
//class DamageSensorComponent;
//class EnvironmentSensorComponent;
//class EquippableComponent;
//class ExplodeComponent;
//class HealableComponent;
//class InteractComponent;
//class LeashableComponent;
//class LookAtComponent;
//class MountTamingComponent;
//class NameableComponent;
//class PeekComponent;
//class ProjectileComponent;
//class RailMovementComponent;
//class RideableComponent;
//class ScaleByAgeComponent;
//class SitComponent;
//class TameableComponent;
//class TargetNearbyComponent;
//class TeleportComponent;
//class TimerComponent;
//class TransformationComponent;
//class ContainerComponent;
//class HopperComponent;
//class ShooterComponent;
//class InteractComponent;
//class EquippableComponent;
//class RailActivatorComponent;
//class NpcComponent;
//class BossComponent;
//class TripodCameraComponent;
//class AgentCommandComponent;
//struct SeatDescription;
//
//class LootTable;
//
//enum class EntityEvent : byte;
//enum class UseAnimation : byte;
//class NpcComponent;
//class SetEntityDataPacket;
//enum class PaletteColor : unsigned char;
//enum class LevelSoundEvent : unsigned char;

// where to play the sound for an entity sound
enum class EntityLocation {
	Feet,
	Body,
	WeaponAttachPoint,
	Head,
	DropAttachPoint,
};

enum class EntityFlags : int {
	ONFIRE,
	SNEAKING,
	RIDING,
	SPRINTING,
	USINGITEM,
	INVISIBLE,
	TEMPTED,
	INLOVE,
	SADDLED,
	POWERED,
	IGNITED,
	BABY,
	CONVERTING,
	CRITICAL,
	CAN_SHOW_NAME,
	ALWAYS_SHOW_NAME,
	NOAI,
	SILENT,
	WALLCLIMBING,
	RESTING,
	SITTING,
	ANGRY,
	INTERESTED,
	CHARGED,
	TAMED,
	LEASHED,
	SHEARED,
	GLIDING,
	ELDER,
	MOVING,
	BREATHING,
	CHESTED,
	STACKABLE,
	SHOW_BOTTOM,
	STANDING,
	SHAKING,
	IDLING,
	COUNT
};

enum class EntityDataIDs : byte {
	FLAGS = 0,			// DATA_STATUS_BITMASK,
	HEALTH,				// DATA_ID_DAMAGE
	VARIANT,			// DATA_ID_TYPE + DATA_ID_WOODID + DATA_PROFESSION_ID
	COLOR_INDEX,		// DATA_ID_COLAR_COLOR + DATA_ID_WOOL
	NAME,
	OWNER,
	TARGET,
	AIR_SUPPLY,
	EFFECT_COLOR,
	EFFECT_AMBIENCE,
	JUMP_TYPE,
	HURT,
	HURT_DIR,
	ROW_TIME_LEFT,		
	ROW_TIME_RIGHT,
	VALUE,
	DISPLAY_TILE,
	DISPLAY_OFFSET,
	CUSTOM_DISPLAY,
	SWELL,
	OLD_SWELL,
	SWELL_DIR,
	CHARGE_AMOUNT,
	CARRY_BLOCK,
	CARRY_DATA,
	CLIENT_EVENT,
	USING_ITEM,
	PLAYER_FLAGS,
	PLAYER_INDEX,
	BED_POSITION,
	X_POWER,
	Y_POWER,
	Z_POWER,
	AUX_POWER,
	FISHX,
	FISHZ,
	FISHANGLE,
	AUX_VALUE_DATA,
	LEASH_HOLDER,
	SCALE,
	INTERACT_TEXT,
	INTERACT_ENTITY,
	SKIN_ID,
	URL_TEXT,
	AIR_SUPPLY_MAX,
	MARK_VARIANT,
	CONTAINER_TYPE,
	CONTAINER_SIZE,
	BLOCK_TARGET,
	INV,
	TARGET_A,
	TARGET_B,
	TARGET_C,
	AERIAL_ATTACK,
	WIDTH,
	HEIGHT,
	FUSE_TIME,
	SEAT_OFFSET,
	SEAT_LOCK_RIDER_ROTATION,
	SEAT_LOCK_RIDER_ROTATION_DEGREES,
	SEAT_ROTATION_OFFSET,
	DATA_RADIUS,
	DATA_WAITING,
	DATA_PARTICLE,
	PEEK_ID,
	ATTACH_FACE,
	ATTACH_POS
};

struct EntityDefinitionIdentifier;

class Entity {
public:
	enum class InitializationMethod : byte {
		INVALID = 0,
		LOADED,
		SPAWNED,
		BORN,
		TRANSFORMED,
		UPDATED,
		LEGACY
	};

	// Constructor for Player classes
	Entity(Level& level);
	// Constructor for everything else
//	Entity(EntityDefinitionGroup& definitions, const EntityDefinitionIdentifier& definitionName);
//	// DEPRECATED - Used by some Entities still being adapted to data driving:
//	//	- Painting
//	Entity(BlockSource& region, const std::string& defaultDefName);
//
//	template <typename T>
//	inline void pushInitialParameter(const std::string &name, T *data) {
//		mInitParams.pushParameter<T>(name, data);
//	}
//
//	// Called after any entity was created and added
//	void reload();
//	inline bool isInitialized() { return mInitialized; }
//
//protected:
//	// Leaf-only override-able method called at the end of reload()
//	virtual void reloadHardcoded(InitializationMethod method, const VariantParameterList &params) {
//		UNUSED_PARAMETER(method, params);
//	}
//	virtual void reloadHardcodedClient(InitializationMethod method, const VariantParameterList &params) {
//		UNUSED_PARAMETER(method,params);
//	}
//	// Initializes all components up the class hierarchy
//	virtual void initializeComponents(InitializationMethod method, const VariantParameterList &params);
//	
//	virtual void reloadComponents(InitializationMethod method, const VariantParameterList& params);
//
//private:
//	// If EntityFactory doesn't set this (nothing else does), we know the entity was improperly constructed
//	InitializationMethod mInitMethod = InitializationMethod::INVALID;
//	VariantParameterList mInitParams;
//	// These are the classes allowed to set mInitMethod
//	friend class EntityFactory;
//	friend class MultiPlayerLevel;
//	friend class ServerNetworkHandler;	// Player
//	friend class Level;	// Player
//
public:
//
//	// Level
	DimensionId mDimensionId = DimensionId::Undefined;
	bool mAdded = false;
	Level *mLevel = nullptr;
//
//	// Definition
//	EntityDefinitionGroup &mDefinitions;
//	//EntityDefinitionPtr mCurrentDefinition;
//	//std::string mNextDefinition;
//	/*
//	We need a diff list here, and then we can keep the current 
//	built out descriptor.
//
//	*/
//	Unique<EntityDefinitionDescriptor> mCurrentDescription;
//
//	void initParams(VariantParameterList& params);
//
//	//	@note: Find a better way to handle these public variables as they're touched and looked at by other classes
//	//	but having them with both a getter and setter seems a waste since it just results in the same as them being
//	//	public but with an addition function call to handle. Also we don't want public member variables. :(
//	typedef std::vector<Entity*> RiderList;
//	typedef std::vector<EntityUniqueID> RiderIDList;

	static const std::string RIDING_TAG;
	static const int TOTAL_AIR_SUPPLY;

	mutable EntityUniqueID mUniqueID;

	// Placement
	Vec3 mPos = Vec3::ZERO;
	Vec3 mPosPrev = Vec3::ZERO;
	Vec3 mPosPrev2 = Vec3::ZERO;
	Vec3 mPosDelta = Vec3::ZERO;
	Vec2 mRot = Vec2::ZERO;
	Vec2 mRotPrev = Vec2::ZERO;
	Vec2 mRotRide = Vec2::ZERO;
	ChunkPos mChunkPos;
	BlockSource* mRegion = nullptr;
//
//	// Type
//	EntityCategory mCategories = EntityCategory::None; // *** Def ***
//
//	// Sync
//	SynchedEntityData mEntityData;
//	Unique<MoveEntityPacketData> mSentPositionData;
	Vec3 mSentDelta = Vec3::ZERO;
	//TODO move this to the client side
	float mScale = 1.f;
	float mScalePrev = 0.f;

	// Physics
	bool mNoPhysics = false;
	bool mNoGravity = false;
	bool mOnGround = false;
	bool mWasOnGround = false;
	bool mHorizontalCollision = false;
	bool mVerticalCollision = false;
	bool mCollision = false;
	BlockID mInsideBlockId = BlockID::AIR;
	float mFallDistance = 0;
	
	// Visual
	bool mIgnoreLighting = false;
//	Color mTintColor;
//	EntityRendererId mEntityRendererId = ER_DEFAULT_RENDERER; // *** Def ***
//
	//TODO move to EntityGraphics
	float mStepSoundVolume = 0.25f;
	float mStepSoundPitch = 1.f;
	AABB* mLastHitBB;
	AABB mBB;
	std::vector<AABB> mSubBBs;
	Vec2 mBBDim = { 0.6f, 1.8f }; // *** Def ***

	float mTerrainSurfaceOffset = 0; // *** Def ***
	float mHeightOffset = 0; // *** Def ***
	float mShadowOffset = 0; // to account for mismatches with the visual shape and the aabb (e.g. ghast tendrils)
	float mMaxAutoStep = 0.0f; // *** Def ***
	float mPushthrough = 0; // *** Def ***
	float mWalkDistPrev = 0;
	float mWalkDist = 0;
	float mMoveDist = 0;
	int mNextStep = 1;
	bool mMakeStepSound = true; // *** Def ***
	bool mIsStuckInWeb = false;
	bool mImmobile = false;
	bool mWasInWater = false;
	Vec2 mSlideOffset = Vec2::ZERO;

	// Time
	bool mFirstTick = true;
	int mTickCount = 0;
	int mInvulnerableTime = 0;

	// Health State
	bool mFallDamageImmune = true;
	bool mHurtMarked = false;
	bool mInvulnerable = false;

	// Air State
	//int mAirSupply = TOTAL_AIR_SUPPLY;

	// Fire State
	bool mFireImmune = false;
	bool mAlwaysFireImmune = false;

	int mOnFire = 0;
	bool mHurtWhenWet = false;

//	// Portal
//	int mPortalCooldown = 0;
//	BlockPos mPortalBlockPos = BlockPos::MAX;
//	int mPortalEntranceAxis = PortalBlock::AXIS_UNKNOWN;
//	int mInsidePortalTime = 0;
//
//	// Rider
//	std::vector<Entity*> mRiders;
//	std::vector<Entity*> mRidersToRemove;
//	//Rider IDs list is being added as a hack to easily avoid a crash caused by player being destructed before its mount saves links
//	std::vector<EntityUniqueID> mRiderIDs;
//	Entity* mRiding = nullptr;
//	EntityUniqueID mRidingPrevID = {};
//	bool mInheritRotationWhenRiding = false; // *** Def ***
//	bool mRidersChanged = false;
//
//	// Fishing
//	FishingHook* mFishing = nullptr;	// the actual entity is owned by the level.
	bool mBlocksBuilding = false;
	bool mUsesOneWayCollision = false;
	bool mForcedLoading = false;
	bool mPrevPosRotSetThisTick = false;
	float mSoundVolume = 1.0f;

	//Util
	Random mRandom;
	
//	mutable EntityUniqueID mLegacyUniqueID;
//	bool mHighlightedThisFrame = false;
//	
//	// Check if the block position is already on the oneway list, otherwise add it (positions can't have duplicates)
//	// Turns the physics to only prevent following collisions, not resolve current ones while within the BlockPos, or AABB
//	void onOnewayCollision(BlockPos pos);
//	void onOnewayCollision(const AABB& oneWayVolume);
//
//	// Helper function for going through intersecting entities and adding them to the oneway collision list
//	static void checkEntityOnewayCollision(BlockSource& region, const BlockPos& position);
//
//	virtual bool hasComponent(const std::string &name) const;
//	virtual bool hasInventory() const;
//	virtual Container& getInventory();
//
//	bool mInitialized = false;
//protected:
	bool mChanged = false;
	bool mRemoved = false;
	bool mGlobal = false;
	bool mAutonomous = false;
//
//	friend class EntityLegacySaveConverter;
//
//	// Components
//	Unique<AddRiderComponent> mAddRiderComponent;
//	Unique<EnvironmentSensorComponent> mEnvironmentSensorComponent;
//	Unique<AgeableComponent> mAgeableComponent;
//	Unique<AngryComponent> mAngryComponent;
//	Unique<BreathableComponent> mBreathableComponent;
//	Unique<BreedableComponent> mBreedableComponent;
//	Unique<ContainerComponent> mContainerComponent;
//	Unique<ExplodeComponent> mExplodeComponent;
//	Unique<TameableComponent> mTameableComponent;
//	Unique<SitComponent> mSitComponent;
//	Unique<LookAtComponent> mLookAtComponent;
//	Unique<ProjectileComponent> mProjectileComponent;
//	Unique<TeleportComponent> mTeleportComponent;
//	Unique<MountTamingComponent> mMountTamingComponent;
//	Unique<TimerComponent> mTimerComponent;
//	Unique<RideableComponent> mRideableComponent;
//	Unique<ShooterComponent> mShooterComponent;
//	Unique<InteractComponent> mInteractComponent;
//	Unique<EquippableComponent> mEquippableComponent;
//	Unique<HealableComponent> mHealableComponent;
//	Unique<ScaleByAgeComponent> mScaleByAgeComponent;
//	Unique<TargetNearbyComponent> mTargetNearbyComponent;
//	Unique<TransformationComponent> mTransformationComponent;
//	Unique<BoostableComponent> mBoostableComponent;
//	Unique<RailMovementComponent> mRailMovementComponent;
//	Unique<DamageSensorComponent> mDamageSensorComponent;
//	Unique<LeashableComponent> mLeashableComponent;
//	Unique<HopperComponent> mHopperComponent;
//	Unique<NameableComponent> mNameableComponent;
//	Unique<RailActivatorComponent> mRailActivatorComponent;
//	Unique<NpcComponent> mNpcComponent;
//	Unique<TripodCameraComponent> mTripodCameraComponent;
//	Unique<BossComponent> mBossComponent;
//	Unique<PeekComponent> mPeekComponent;
//	Unique<AgentCommandComponent> mAgentCommandComponent;
//
//	//Other
//	EntityUniqueID mTargetId = {};
//	bool mPersistent = false;
//	
//	// To drop lootTable only once
//	bool mLootDropped = false;
//
//	// Restrict
//	float mRestrictRadius = -1.0f;
//	BlockPos mRestrictCenter = BlockPos::ZERO;
//
//	// Love
//	EntityUniqueID mInLovePartner;
//
//public:
//	static bool isSpawnableInCreative(EntityType type);
	virtual ~Entity();
//
//	//EntityDefinitionPtr getDefinition() const;
//	//bool setDefinition(const std::string& defName, bool forceDefinition = false, bool callChangeAction = true);
//	void addDefinitionGroup(const std::string& name);
//	void removeDefinitionGroup(const std::string& name);
//	bool hasDefinitionGroup(const std::string& name) const;
//	void setBaseDefinition(const std::string& name, bool clearDefs = true, bool update = false);
//
//	void updateDescription();
//
//	virtual void reset();
//
//	void setPersistent();
//
//	bool hasCategory(EntityCategory categories) const;
//	bool hasType(EntityType types) const;
//	bool hasFamily(const std::string& family) const;
	BlockSource& getRegion() const;
//	Dimension& getDimension() const;
//	bool isRegionValid() const;
//	void resetRegion();
//
	const EntityUniqueID& getUniqueID() const;
//	EntityRuntimeID getRuntimeID() const;
	bool hasUniqueID() const;
//	bool hasRuntimeID() const;
//	void setRuntimeID(EntityRuntimeID ID);
	void setUniqueID(EntityUniqueID ID);
//	virtual EntityType getOwnerEntityType();
	void setRegion(BlockSource& region);

	bool operator==(Entity& rhs);

	virtual void remove();

	virtual void setPos(const Vec3& pos);
	virtual const Vec3& getPos() const;
	virtual const Vec3& getPosOld() const;
	virtual const Vec3 getPosExtrapolated(float factorAlpha) const;
	virtual const Vec3& getVelocity() const;
//	void setPreviousPosRot(const Vec3& pos, const Vec2& rot);
//	void setColor(PaletteColor color);
//	PaletteColor getColor() const;
//	void moveBBs(const Vec3& pos);
//	void setLastHitBB(const Vec3& pos, const Vec3& radius);
//
//	Vec3 getAttachPos(EntityLocation location) const;
//	
//	float getRadius() const;
//
	virtual void setRot(const Vec2& rot);
//	virtual void move(const Vec3& pos);
//	void testForEntityStacking(BlockSource& region, const AABB& intersectTestBox, std::vector<AABB>& collisionAABBs);
//	void testForCollidableMobs(BlockSource& region, const AABB& intersectTestBox, std::vector<AABB>& collisionAABBs);
	Vec2 getRotation() const;
//
//	Vec3 buildForward() const;

	Vec3 getInterpolatedPosition(float a) const;
	Vec3 getInterpolatedPosition2(float a) const;
	Vec2 getInterpolatedRotation(float a) const;

	Vec3 getViewVector(float a) const;
	Vec2 getViewVector2(float a) const;

//	bool enforceRiderRotationLimit();
//
//	virtual void checkBlockCollisions(const AABB& aabb);
//	virtual void checkBlockCollisions();
//
//	bool isInsideBorderBlock(float grow = 1.0f) const;
//
    virtual bool canFly() { return false; }
//	void setCollidableMobNear(bool mobNear);
//	bool getCollidableMobNear();
//
//	void setCollidableMob(bool mobNear);
//	bool getCollidableMob();
//
	void moveTo(const Vec3& pos, const Vec2& rot);
//	virtual void moveRelative(float xa, float za, float speed);
//	virtual void teleportTo(const Vec3& pos, int cause = 0, int entityType = enum_cast(EntityType::Undefined));
//	virtual bool tryTeleportTo(const Vec3& pos, bool landOnBlock = true, bool avoidLiquid = true, int cause = 0, int entityType = enum_cast(EntityType::Undefined));
//
//	virtual void lerpTo(const Vec3& pos, const Vec2& rot, int steps);
//	virtual void lerpMotion(const Vec3& delta);
//
//	virtual void turn(const Vec2& rot, bool useScaling = true);
//	virtual void interpolateTurn(const Vec2& rot);
//
//	virtual Unique<Packet> getAddPacket();
//
//	bool tick(BlockSource& tickingArea);
//
//	virtual void normalTick();
//	virtual void baseTick();
//
//	virtual void rideTick();
//	virtual void positionRider(Entity& rider) const;
//	Entity* getRide() const;
//	virtual float getRidingHeight();
//	virtual bool startRiding(Entity& ride);
//	virtual void addRider(Entity& newrider);
//	virtual void flagRiderToRemove(Entity& r);
//	void removeAllRiders(bool beingDestroyed = false, bool playAnimateHurt = false);
//	void setSeatDescription(const Vec3& offset, const SeatDescription& seat);
//
//	virtual bool intersects(const Vec3 &pos1, const Vec3 &pos);
//	virtual bool isFree(const Vec3 &pos, float grow);
//	virtual bool isFree(const Vec3 &pos);
//	virtual bool isInWall() const;
//	virtual bool isInvisible() const;
//	virtual bool canShowNameTag();
//	virtual bool canExistInPeaceful() const;
//	virtual void setNameTagVisible(bool visible);
//	virtual const std::string& getNameTag() const;
//	virtual std::string getFormattedNameTag() const;
//	virtual void setNameTag(const std::string& name);
	virtual bool isInWater() const;
//	virtual bool isInWaterOrRain();
//	virtual bool isInLava();
//	bool isInClouds() const;
//	virtual bool isUnderLiquid(MaterialType type) const;
//
//	virtual bool onLadder(bool orVine = true);
//
//	virtual void makeStuckInWeb();
//
//	virtual float getHeadHeight() const;
//	virtual float getCameraOffset() const;
//	virtual float getShadowHeightOffs();
//	virtual float getShadowRadius() const;
//	
//	float getScaleFactor(float a) const {
//		return Math::lerp(mScalePrev, mScale, a);
//	}

	virtual bool isSwimmer() { return false; }

//	virtual bool canSeeInvisible() { return false; }
//
//	Vec3 getRandomPointInAABB(Random& r);
//
//	virtual bool isSkyLit(float a);
//	virtual float getBrightness(float a) const;

	Vec3 getCenter(float a) const;

	float distanceTo(const Entity& e) const;
	float distanceTo(const Vec3& v) const;
	float distanceToSqr(const Vec3& v) const;
	float distanceToSqr(const Entity& e) const;
	float distanceSqrToBlockPosCenter(const BlockPos& incomingPosition) const;
//
//	virtual bool interactPreventDefault();
	virtual void playerTouch(Player& player);
//
//	virtual void push(Entity& e, bool pushEntityOnly = false);
//	virtual void push(const Vec3 &vec);

	virtual bool isImmobile() const;
	virtual bool isSilent();
	virtual bool isPickable();
	virtual bool isFishable() const;
	virtual bool isPushable() const;
	virtual bool isPushableByPiston() const;
	virtual bool isShootable();

	virtual bool isSneaking() const;
	virtual bool isAlive() const;
	virtual bool isOnFire() const;
	virtual bool isCreativeModeAllowed();
	virtual bool isSurfaceMob() const;

//	int getVariant() const;
//	void setVariant(int value);
//
//	int getMarkVariant() const;
//	void setMarkVariant(int value);
//
//	Entity* getTarget() const;
//	EntityUniqueID getTargetId();
//	virtual void setTarget(Entity*);
//
//	void setBlockTarget(const BlockPos& target);
//	BlockPos getBlockTarget(void);
//
//	//Ageable Component
//	bool isBaby() const;
//	int getAge() const;
//
//	// Stackable Component
//	bool isStackable() const;
//
//	//Tameable Component
//	bool isTame() const;
//	Player* getOwner() const;
//	virtual void setOwner(const EntityUniqueID ownerId);
//	const EntityUniqueID getOwnerId() const;
//	bool isSitting() const;
//	virtual void setSitting(bool value);
//	void spawnTamingParticles(bool wasSuccess);
//	virtual void onTame();
//	virtual void onFailedTame();
//
//	//Breedable Component
//	bool isInLove() const;
//	bool canMate(const Entity& entity) const;
//	virtual void onMate(Mob& partner);
//
//	//Angry Component
//	bool isAngry() const;
//
//	//Sheared Component
//	bool isSheared() const;
//
//	//Chested Component
//	bool isChested() const;
//
//	//Exploadable Component
//	bool isIgnited() const;
//
//	//Rideable
//	bool hasSaddle() const;
//	void setSaddle(bool saddled);
//
//	//end components
//	bool isCharged() const;
//	void setCharged(bool value);
//
//	bool isPowered() const;
//	void setPowered(bool value);
//
//	virtual bool shouldRender() const;
//
//
//	bool isUpsideDown() const;
//
//	bool hurt(const EntityDamageSource& source, int damage, bool knock = true, bool ignite = false);
//	virtual bool isInvulnerableTo(const EntityDamageSource& source) const;
//
//	virtual void animateHurt();
//	virtual bool doFireHurt(int amount);
//	virtual void onLightningHit();
//	virtual void onBounceStarted(const BlockPos& bounceStartPos, const FullBlock& block) {
//		UNUSED_PARAMETER(bounceStartPos, block);
//	};
//
//	virtual void feed(int itemId);
//	void spawnEatParticles(const ItemInstance& itemInUse, int count);
//	void spawnDeathParticles();
//	void spawnDustParticles();
//
//	virtual void handleEntityEvent(EntityEvent eventId, int data);
//
//	virtual float getPickRadius();
//
//	virtual ItemEntity* spawnAtLocation(int resource, int count);
//	virtual ItemEntity* spawnAtLocation(int resource, int count, float yOffs);
//	virtual ItemEntity* spawnAtLocation(FullBlock block, int count);
//	virtual ItemEntity* spawnAtLocation(FullBlock block, int count, float yOffs);
//
//	// @attn: right now this means a pointer that spawnAtLocation takes ownership of
//	
//	virtual ItemEntity* spawnAtLocation(const ItemInstance& itemInstance, float yOffs);
//
//	virtual void killed(Entity* entity) {
//		UNUSED_PARAMETER(entity);
//	};
//	virtual void awardKillScore(Entity& victim, int score);
//
//	virtual void setEquippedSlot(ArmorSlot slot, int item, int auxValue);
//	virtual void setEquippedSlot(ArmorSlot slot, const ItemInstance& item);	// use this to set enchanted armor on a mob
//
//	virtual bool save(CompoundTag& entityTag);
//
//	Unique<ListTag> saveLinks() const;
//	EntityLink::List getLinks() const;
//	virtual void saveWithoutId(CompoundTag& entityTag);
//	virtual bool load(const CompoundTag& entityTag);
//	virtual void loadLinks(const CompoundTag& entityTag, EntityLink::List& links);
//	void saveEntityFlags(CompoundTag& entityTag);
//	void loadEntityFlags(const CompoundTag& entityTag);
//
//	void setChanged();
//
//	const SynchedEntityData& getEntityData() const;
//	SynchedEntityData& getEntityData();

virtual EntityType getEntityTypeId() const = 0;

//	virtual bool acceptClientsideEntityData(Player& sourcePlayer, const SetEntityDataPacket& packet);
//
//	virtual EntityRendererId queryEntityRenderer();
//
//	virtual EntityUniqueID getSourceUniqueID() const;
//
//	virtual void setOnFire(int seconds);
//
//	virtual AABB getHandleWaterAABB() const;
//
//	virtual void handleInsidePortal(const BlockPos& portalPos);
//	virtual int getPortalCooldown() const;
//	virtual int getPortalWaitTime() const;
//
//	int getPortalEntranceAxis() const;
	virtual DimensionId getDimensionId() const;
//	virtual bool canChangeDimensions() const;
//	virtual void changeDimension(DimensionId toId);
//	virtual void changeDimension(const ChangeDimensionPacket& packet);
//
//	virtual EntityUniqueID getControllingPlayer() const;
//	bool isControlledByLocalInstance() const;
//
//	virtual void checkFallDamage(float ya, bool onGround);
//	virtual void causeFallDamage(float fallDistance);
//	virtual void handleFallDistanceOnServer(float fallDistance);
//
//	bool getStatusFlag(EntityFlags flag) const;
//	void setStatusFlag(EntityFlags flag, bool value);
//
//	bool isDebugging() const;
//
//	void playSound(LevelSoundEvent type, Vec3 const& pos, int blockID = -1);
//	virtual void playSynchronizedSound(LevelSoundEvent type, Vec3 const& pos, int blockID = -1, bool isGlobal = false);
//
//	virtual void onSynchedDataUpdate(int dataId);
//
//	bool isRiding() const;
//	bool isRide() const;
//	virtual bool canAddRider(Entity& rider) const;
//	bool isRider(Entity& rider) const;
//
//	virtual bool canBePulledIntoVehicle() const;
//
//	bool isLeashed();
//	EntityUniqueID getLeashHolder();
//	void setLeashHolder(EntityUniqueID leashHolder);
//	void dropLeash(bool createItemDrop);
//	virtual bool canBeLeashed();
//	virtual bool isLeashableType() { return false; }
//	virtual void tickLeash();
//
//	int getAirSupply() const;
//	int getTotalAirSupply() const;
//
//	virtual float getEyeHeight() const;
//	Vec3 getEyePos();
//
//	virtual void sendMotionPacketIfNeeded();
//	void sendMotionToServer(bool doBroadcast = true);

	bool isRemoved() const;
//	virtual bool canSynchronizeNewEntity() const;
//
//	bool isGlobal() const;
//	void setGlobal(bool g);
//
//	bool isAutonomous() const;
//	void setAutonomous(bool g);
//
//	virtual void stopRiding(bool exitFromRider = true, bool entityIsBeingDestroyed = false);
//
//	virtual void buildDebugInfo(std::string& out) const;
//	void buildDebugGroupInfo(std::string& out) const;
//	bool _tryPlaceAt(const Vec3& pos);

	Level& getLevel();
	const Level& getLevel() const;

//	const Color & getHurtColor() const;
//
//	AgeableComponent* getAgeableComponent() const;
//	AngryComponent* getAngryComponent() const;
//	BreathableComponent* getBreathableComponent() const;
//	BreedableComponent * getBreedableComponent() const;
//	ExplodeComponent* getExplodeComponent() const;
//	TameableComponent* getTameableComponent() const;
//	ProjectileComponent* getProjectileComponent() const;
//	LookAtComponent* getLookAtComponent() const;
//	TeleportComponent * getTeleportComponent() const;
//	MountTamingComponent* getMountTamingComponent() const; 
//	TimerComponent* getTimerComponent() const;
//	RideableComponent* getRideableComponent() const;
//	ShooterComponent* getShooterComponent() const;
//	InteractComponent* getInteractComponent() const;
//	EquippableComponent* getEquippableComponent() const;
//	HealableComponent* getHealableComponent() const;
//	ScaleByAgeComponent* getScaleByAgeComponent() const;
//	BoostableComponent* getBoostableComponent() const;
//	RailMovementComponent* getRailMovementComponent() const;
//	DamageSensorComponent* getDamageSensorComponent() const;
//	TransformationComponent* getTransformationComponent() const;
//	NameableComponent* getNameableComponent() const;
//	LeashableComponent* getLeashableComponent() const;
//	ContainerComponent* getContainerComponent() const;
//	NpcComponent* getNpcComponent() const;
//	TripodCameraComponent* getTripodCameraComponent() const;
//	HopperComponent* getHopperComponent() const;
//	RailActivatorComponent* getRailActivatorComponent() const;
//	BossComponent* getBossComponent() const;
//	AgentCommandComponent* getAgentCommandComponent() const;
//
//	virtual void openContainerComponent(Player& player);
//	
//	virtual void useItem(ItemInstance& instance) const;
//
//	virtual bool hasOutputSignal(FacingID dir) const;
//	virtual int getOutputSignal() const;
//
//	virtual void getDebugText(std::vector<std::string>& outputInfo);
//
//	virtual void startSeenByPlayer(Player& player);
//	virtual void stopSeenByPlayer(Player& player);
//
//	void doEnchantDamageEffects(Mob& source, Mob& target);
//
//	virtual float getMapDecorationRotation();
//	virtual float getRiderDecorationRotation(Player& player);

	virtual float getYHeadRot() const;

//	virtual bool isWorldBuilder();
	virtual bool isCreative() const;
//
//	void dropTowards(const ItemInstance& item, Vec3 towards);
//	virtual bool add(ItemInstance& item);
//	virtual void drop(const ItemInstance& item, bool randomly = false);
//
//	// Food
//	bool hasEnoughFoodToBreed();
//	bool hasExcessFood();
//	bool wantsMoreFood();
//
//	// Restriction
//	bool isWithinRestriction() const;
//	bool isWithinRestriction(const BlockPos& pos) const;
//	void restrictTo(const BlockPos& newCenter, float radius);
//	BlockPos getRestrictCenter() const;
//	float getRestrictRadius() const;
//	bool hasRestriction() const;
//	void clearRestriction();
//
//	// Love
//	EntityUniqueID const& lovePartnerId() const;
//	void setInLove(Entity* inLovePartner);
//	Vec3 calcCenterPos() const;
//	Vec3 _randomHeartPos();
//
//	void burn(int dmg, bool bInFire = false);
//
//	bool getInteraction(Player& player, EntityInteraction& interaction);
//
//	virtual bool canDestroyBlock(const Block& block) const;
//
//	virtual void setAuxValue(int val){
//		UNUSED_PARAMETER(val);
//	}
//
//	void setChainedDamageEffects(bool active);
//	bool getChainedDamageEffects() const;
//	
//	virtual void setSize(float w, float h);
//	void setEnforceRiderRotationLimit(bool value);
//	void setInheritRotationWhenRiding(bool value);
//
//	float getSpeedInMetersPerSecond() const;
//
//protected:
//	void _playMovementSound(bool tryStepSound);
//
//	virtual void setPos(const EntityPos& pos);
//	virtual void outOfWorld();
//
//	virtual bool _hurt(const EntityDamageSource& source, int damage, bool knock, bool ignite);
//	virtual void markHurt();
//	virtual void lavaHurt();
//
//	virtual void readAdditionalSaveData(const CompoundTag& tag);
//	virtual void addAdditionalSaveData(CompoundTag& tag);
//
//	virtual void _playStepSound(const BlockPos& pos, int t);
//
//	virtual void checkInsideBlocks(float grow = 0.001f);
//
//	virtual void pushOutOfBlocks(const Vec3& vec);
//
//	virtual bool updateWaterState();
//	virtual void doWaterSplashEffect();
//
//	virtual void updateInsideBlock();
//
//	bool isInWorld() const;
//
//	virtual void onBlockCollision(int axis);
//
//	void enableAutoSendPosRot(bool enable);
//	bool isAutoSendEnabled() const;
//
//	virtual LootTable* getLootTable() const;
//	virtual LootTable* getDefaultLootTable() const { return nullptr; }
//	
//	BlockPos _getBlockOnPos();
//
//	EntityRuntimeID mRuntimeID = EntityRuntimeID(0);
//	Color mHurtColor = Color(1.f, 0, 0, 0.6f);
//
//	bool mEnforceRiderRotationLimit = false;
//
//	EntityDefinitionDiffList mDefinitionList;
//private:
//	void _convertOldSave(const CompoundTag& tag);
//	bool _countFood(int baseMultiplier);
//	void _updateOwnerChunk();
//	virtual void _removeRider(Entity& r, bool entityIsBeingDestroyed);
//	void _exitRide(const Entity& riding, float exitDist = 1.2f);
//	void _manageRiders(BlockSource& tickingArea);
//	RiderList::iterator _findRider(Entity& rider) const;
//	//Rider IDs list is being added as a hack to easily avoid a crash caused by player being destructed before its mount saves links
//	RiderIDList::iterator _findRiderID(Entity& rider) const;
//	void _sendLinkPacket(const EntityLink& link) const;
//
//	static int mEntityCounter;
//
//	// Check to see if the object position should be removed from this entities' oneway collision list
//	void _updateOnewayCollisions(BlockSource& region);
//
//	// Turns the physics to only prevent following collisions, not resolve current ones
//	std::vector<AABB> mOnewayPhysicsBlocks;
//	// Used to avoid turning off one way physics if entity is still stuck in collider
//	bool mStuckInCollider;
//	// Used to detect if an entity got stuck in colliders. If this is nonzero for more than a frame, it's assumed stuck
//	float mLastPenetration;
//
//	bool mCollidableMobNear = false;
//	bool mCollidableMob = false;
//
//	bool mChainedDamageEffects = false;
};