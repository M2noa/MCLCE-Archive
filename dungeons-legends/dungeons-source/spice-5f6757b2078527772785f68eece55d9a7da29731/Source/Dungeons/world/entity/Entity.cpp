/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "world/entity/Entity.h"

//#include "world/entity/EntityPos.h"
#include "world/level/Level.h"
//#include "world/level/block/LiquidBlock.h"
//#include "world/level/block/PortalBlock.h"
//#include "world/entity/item/ItemEntity.h"
//#include "world/entity/EntityDefinition.h"
//#include "world/entity/EntityDefinitionGroup.h"
//#include "network/packet/AddEntityPacket.h"
//#include "world/item/ItemInstance.h"
//#include "util/PerfTimer.h"
//#include "util/Token.h"
#include "world/level/BlockSource.h"
//#include "world/level/material/Material.h"
//#include "world/level/chunk/LevelChunk.h"
//#include "world/entity/player/Player.h"
//#include "world/level/Weather.h"
//#include "world/entity/LeashFenceKnotEntity.h"
//#include "network/packet/SetEntityLinkPacket.h"
//#include "network/PacketSender.h"
//#include "network/packet/MoveEntityPacketData.h"
//#include "network/packet/InteractPacket.h"
//#include "network/packet/MoveEntityPacket.h"
//#include "network/packet/SetEntityMotionPacket.h"
//#include "client/renderer/debug/DebugRenderer.h"
//#include "network/packet/LevelSoundEventPacket.h"
//#include "world/level/dimension/Dimension.h"
//#include "world/entity/EntityDamageSource.h"
//#include "world/level/block/Block.h"
//#include "client/particle/BlockForceFieldParticle.h"
//#include "world/entity/EntityInteraction.h"
//#include "world/level/block/ClothBlock.h"
//#include "world/level/Spawner.h"
//#include "world/Container.h"
//#include "Core/Utility/optional_ref.h"
//#include "platform/AppPlatform.h"
//#include "client/renderer/entity/Palette.h"
//#include "client/renderer/entity/PaletteColor.h"
//#include "world/entity/components/ExplodeComponent.h"
//#include "world/entity/components/BossComponent.h"
//#include "world/level/block/CauldronBlock.h"
//#include "world/level/block/entity/CauldronBlockEntity.h"
//#include "world/effect/MobEffect.h"
//
////Components
//#include "world/entity/components/AddRiderComponent.h"
//#include "world/entity/components/AgeableComponent.h"
//#include "world/entity/components/AngryComponent.h"
//#include "world/entity/components/BreathableComponent.h"
//#include "world/entity/components/BreedableComponent.h"
//#include "world/entity/components/EnvironmentSensorComponent.h"
//#include "world/entity/components/ExplodeComponent.h"
//#include "world/entity/components/ProjectileComponent.h"
//#include "world/entity/components/TameableComponent.h"
//#include "world/entity/components/SitComponent.h"
//#include "world/entity/components/LookAtComponent.h"
//#include "world/entity/components/TeleportComponent.h"
//#include "world/entity/components/MountTamingComponent.h"
//#include "world/entity/components/TimerComponent.h"
//#include "world/entity/components/RideableComponent.h"
//#include "world/entity/components/InteractComponent.h"
//#include "world/entity/components/EquippableComponent.h"
//#include "world/entity/components/HealableComponent.h"
//#include "world/entity/components/PeekComponent.h"
//#include "world/entity/components/ScaleByAgeComponent.h"
//#include "world/entity/components/TargetNearbyComponent.h"
//#include "world/entity/components/TransformationComponent.h"
//#include "world/entity/components/BoostableComponent.h"
//#include "world/entity/components/RailMovementComponent.h"
//#include "world/entity/components/DamageSensorComponent.h"
//#include "world/entity/components/LeashableComponent.h"
//#include "world/entity/components/NameableComponent.h"
//#include "world/entity/components/LeashableComponent.h"
//#include "world/entity/components/ContainerComponent.h"
//#include "world/entity/components/HopperComponent.h"
//#include "world/entity/components/ShooterComponent.h"
//#include "world/entity/components/RailActivatorComponent.h"
//#include "world/entity/components/NpcComponent.h"
//#include "world/entity/components/TripodCameraComponent.h"
//#include "world/entity/components/AgentCommandComponent.h"
//
////Loot tables
//#include "world/level/storage/loot/LootTables.h"
//#include "world/level/storage/loot/LootTable.h"
//#include "world/level/storage/loot/LootTableContext.h"
//
//#include "nbt/StringTag.h"
//
//#include "world/attribute/SharedAttributes.h"
//
//#include "world/item/ArmorItem.h"
//
//#include "util/StringUtils.h"
//#include "util/EntityUtil.h"
//#include "world/entity/EntityLegacySaveConverter.h"
//
//int Entity::mEntityCounter = 0;

const int Entity::TOTAL_AIR_SUPPLY = 15 * SharedConstants::TicksPerSecond;

const std::string Entity::RIDING_TAG = "Riding";

//static void _defineEntityData(SynchedEntityData &data) {
//	data.define(enum_cast(EntityDataIDs::VARIANT), (SynchedEntityData::TypeInt)0);
//	data.define(enum_cast(EntityDataIDs::MARK_VARIANT), (SynchedEntityData::TypeInt)0);
//	data.define(enum_cast(EntityDataIDs::FLAGS), (SynchedEntityData::TypeInt64) 0);
//	data.define(enum_cast(EntityDataIDs::AIR_SUPPLY), (SynchedEntityData::TypeShort) Entity::TOTAL_AIR_SUPPLY);
//	data.define(enum_cast(EntityDataIDs::AIR_SUPPLY_MAX), (SynchedEntityData::TypeShort) Entity::TOTAL_AIR_SUPPLY);
//	data.define(enum_cast(EntityDataIDs::NAME), (SynchedEntityData::TypeString) "");
//	data.define(enum_cast(EntityDataIDs::OWNER), (SynchedEntityData::TypeInt64)EntityUniqueID());
//	data.define(enum_cast(EntityDataIDs::HEALTH), (SynchedEntityData::TypeInt)1);
//	data.define(enum_cast(EntityDataIDs::SCALE), (SynchedEntityData::TypeFloat)1.0f);
//	data.define(enum_cast(EntityDataIDs::COLOR_INDEX), (SynchedEntityData::TypeInt8)0);
//	data.define(enum_cast(EntityDataIDs::CONTAINER_TYPE), (SynchedEntityData::TypeInt8)0);
//	data.define(enum_cast(EntityDataIDs::CONTAINER_SIZE), (SynchedEntityData::TypeInt)0);
//	data.define(enum_cast(EntityDataIDs::WIDTH), (SynchedEntityData::TypeFloat)0.8f);
//	data.define(enum_cast(EntityDataIDs::HEIGHT), (SynchedEntityData::TypeFloat)1.8f);
//
//	data.define(enum_cast(EntityDataIDs::SEAT_OFFSET), (SynchedEntityData::TypeVec3)Vec3::ZERO);
//	data.define(enum_cast(EntityDataIDs::SEAT_LOCK_RIDER_ROTATION), (SynchedEntityData::TypeInt8)0);
//	data.define(enum_cast(EntityDataIDs::SEAT_LOCK_RIDER_ROTATION_DEGREES), (SynchedEntityData::TypeFloat)0.0f);
//	data.define(enum_cast(EntityDataIDs::SEAT_ROTATION_OFFSET), (SynchedEntityData::TypeFloat)0.0f);
//}
//
//Entity::Entity(EntityDefinitionGroup& definitions, const EntityDefinitionIdentifier& definitionName)
//	: mDefinitions(definitions)
//	, mDefinitionList(mDefinitions)
//	, mSubBBs(std::vector<AABB>(0)) {
//
//	DEBUG_ASSERT_MAIN_THREAD;
//
//	_defineEntityData(mEntityData);
//
//	setBaseDefinition(definitionName.getCanonicalName(), true, true);
//}

Entity::Entity(Level& level)
	: mRegion(nullptr)
	, mLevel(&level)
	//, mSubBBs(std::vector<AABB>(0))
	//, mDefinitions(level.getEntityDefinitions())
	//, mDefinitionList(mDefinitions) 
	{

	DEBUG_ASSERT_MAIN_THREAD;

	//_defineEntityData(mEntityData);

	//setBaseDefinition(EntityTypeToString(EntityType::Player, EntityTypeNamespaceRules::ReturnWithNamespace), true, true);
	mPosDelta = Vec3::ZERO;
}

 // DEPRECATED: Some non-Mob entities still use this to make sure they don't break
//Entity::Entity(BlockSource& region, const std::string& defaultDefName)
//	: mLevel(&region.getLevel())
//	, mDefinitions(region.getLevel().getEntityDefinitions())
//	, mDefinitionList(mDefinitions)
//	, mSubBBs(std::vector<AABB>(0))
//	, mInLovePartner() {
//	setRegion(region);
//
//	DEBUG_ASSERT_MAIN_THREAD;
//
//	_defineEntityData(mEntityData);
//
//	setBaseDefinition(defaultDefName, true, true);
//}

Entity::~Entity() {
// #ifdef ENABLE_DEBUG_RENDERING
// 	if (this == DebugRenderer::getDebugEntity()) {
// 		DebugRenderer::clearDebugEntity();
// 	}
// #endif
}

//void Entity::reload() {
//	// Code called under this shouldn't have to worry about client-side operation
//	// Unfortunately, movement packets are screwing up when we don't initialize the client-side entities
//	// Something needs to be moved into the constructor
//	//if (getLevel().isClientSide() && !hasCategory(EntityCategory::Player)) {
//	//	return;
//	//}
//	if (isRegionValid()) {
//		mCurrentDescription = mDefinitionList.getDescription();
//
//		// Initialization happens every time the definition changes, but the Entity is only truly initialized when it is first added
//		if (!mInitialized) {
//			std::string eventName;
//			switch (mInitMethod) {
//			case InitializationMethod::SPAWNED:
//				eventName = "minecraft:entity_spawned";
//				break;
//			case InitializationMethod::BORN:
//				eventName = "minecraft:entity_born";
//				break;
//			case InitializationMethod::TRANSFORMED:
//				eventName = "minecraft:entity_transformed";
//				break;
//			case InitializationMethod::LOADED:
//			case InitializationMethod::LEGACY:
//			default:
//				break;
//			}
//
//			if (!eventName.empty()) {
//				mCurrentDescription->executeEvent(*this, eventName, mInitParams);
//				updateDescription();
//			}
//		}
//
//		if (!getLevel().isClientSide()) {
//			// Set up components
//			initializeComponents(mInitMethod, mInitParams);
//			reloadComponents(mInitMethod, mInitParams);
//
//			mDefinitionList.clearChangedDescription();
//			// Run any hardcoded initialization (special mobs / bosses)
//			reloadHardcoded(mInitMethod, mInitParams);
//		}
//		else {
//			reloadHardcodedClient(mInitMethod, mInitParams);
//		}
//
//		mInitialized = true;
//
//		// Until we've checked over the positioning code, make sure the bounding box and position are in the same place
//		move(Vec3::ZERO);
//
//		// Ignore how we were created
//		mInitMethod = InitializationMethod::UPDATED;
//		mInitParams.clear();
//	}
//	else {
//		DEBUG_FAIL("Entity was not added to the Level properly.");
//	}
//
//}
//
//void Entity::initializeComponents(InitializationMethod method, const VariantParameterList &params) {
//	if (!mInitialized) {
//
//		mPosPrev = mPosPrev2 = mPos;
//		mRotPrev = mRot;
//		mRemoved = false;
//		mHurtColor = Color(1.f, 0, 0, 0.6f);
//		mTintColor = Color::NIL;
//	}
//
//	EntityDefinitionDescriptor& changedDescription = mDefinitionList.getChangedDescription();
//	
//	// Standard components
//
//	_tryInitializeComponent(this, mCurrentDescription->mAgeable, mAgeableComponent, changedDescription.mAgeable);
//	_tryInitializeComponent(this, mCurrentDescription->mTameable, mTameableComponent, changedDescription.mTameable);
//	_tryInitializeComponent(this, mCurrentDescription->mEnvironmentSensor, mEnvironmentSensorComponent, changedDescription.mEnvironmentSensor);
//	_tryInitializeComponent(this, mCurrentDescription->mDamageSensor, mDamageSensorComponent, changedDescription.mDamageSensor);
//	_tryInitializeComponent(this, mCurrentDescription->mProjectile, mProjectileComponent, changedDescription.mProjectile);
//	_tryInitializeComponent(this, mCurrentDescription->mMountTameable, mMountTamingComponent, changedDescription.mMountTameable);
//	_tryInitializeComponent(this, mCurrentDescription->mTimer, mTimerComponent, changedDescription.mTimer);
//	_tryInitializeComponent(this, mCurrentDescription->mScaleByAge, mScaleByAgeComponent, changedDescription.mScaleByAge);
//	_tryInitializeComponent(this, mCurrentDescription->mTransformation, mTransformationComponent, changedDescription.mTransformation);
//	_tryInitializeComponent(this, mCurrentDescription->mRailMovement, mRailMovementComponent, changedDescription.mRailMovement);
//	_tryInitializeComponent(this, mCurrentDescription->mTargetNearby, mTargetNearbyComponent, changedDescription.mTargetNearby);
//	_tryInitializeComponent(this, mCurrentDescription->mRideable, mRideableComponent, changedDescription.mRideable);
//	_tryInitializeComponent(this, mCurrentDescription->mNameable, mNameableComponent, changedDescription.mNameable);
//	_tryInitializeComponent(this, mCurrentDescription->mShooter, mShooterComponent, changedDescription.mShooter);
//	_tryInitializeComponent(this, mCurrentDescription->mInteract, mInteractComponent, changedDescription.mInteract);
//	_tryInitializeComponent(this, mCurrentDescription->mEquippable, mEquippableComponent, changedDescription.mEquippable);
//	_tryInitializeComponent(this, mCurrentDescription->mHopper, mHopperComponent, changedDescription.mHopper);
//	_tryInitializeComponent(this, mCurrentDescription->mRailActivator, mRailActivatorComponent, changedDescription.mRailActivator);
//	_tryInitializeComponent(this, mCurrentDescription->mPeek, mPeekComponent, changedDescription.mPeek);
//	_tryInitializeComponent(this, mCurrentDescription->mAgentCommand, mAgentCommandComponent, changedDescription.mAgentCommand);
//	_tryInitializeComponent(this, mCurrentDescription->mAddRider, mAddRiderComponent, changedDescription.mAddRider);
//
//	// Run-time component adapters, only updating their values when the change in definition gives them new data
//	if (method != InitializationMethod::LOADED || method == InitializationMethod::LEGACY) {
//		if (changedDescription.mVariant && mCurrentDescription->mVariant) {
//			setVariant(mCurrentDescription->mVariant->mVariantChoice);
//		}
//		if (changedDescription.mMarkVariant && mCurrentDescription->mMarkVariant) {
//			setMarkVariant(mCurrentDescription->mMarkVariant->mMarkVariantChoice);
//		}
//		if (changedDescription.mColor && mCurrentDescription->mColor) {
//			setColor(mCurrentDescription->mColor->mColorChoice);
//		}
//	}
//
//	if (changedDescription.mExplodable && !_tryInitializeComponent(this, mCurrentDescription->mExplodable, mExplodeComponent)) {
//		setStatusFlag(EntityFlags::IGNITED, false);
//	}
//	if (changedDescription.mSittable && !_tryInitializeComponent(this, mCurrentDescription->mSittable, mSitComponent)) {
//		setStatusFlag(EntityFlags::SITTING, false);
//	}
//
//	if(changedDescription.mContainer && !_tryInitializeComponent(this, mCurrentDescription->mContainer, mContainerComponent)) {
//		mEntityData.set(enum_cast(EntityDataIDs::CONTAINER_TYPE), (SynchedEntityData::TypeInt8)0);
//		mEntityData.set(enum_cast(EntityDataIDs::CONTAINER_SIZE), (SynchedEntityData::TypeInt)0);
//	}
//
//  // Logical components without a physical class
//
//	if (mCurrentDescription->mTerrainSurfaceOffset) {
//		mTerrainSurfaceOffset = *mCurrentDescription->mTerrainSurfaceOffset;
//	}
//
//	//if (mCurrentDescription->mFootSize) {
//	//	mFootSize = *mCurrentDescription->mFootSize;
//	//}
//	if (mCurrentDescription->mPushthrough) {
//		mPushthrough = *mCurrentDescription->mPushthrough;
//	}
//	if (mCurrentDescription->mMakeStepSound) {
//		mMakeStepSound = *mCurrentDescription->mMakeStepSound;
//	}
//	if (mCurrentDescription->mSoundVolume) {
//		mSoundVolume = *mCurrentDescription->mSoundVolume;
//	}
//
//	// Flag components
//
//	setStatusFlag(EntityFlags::ALWAYS_SHOW_NAME, mCurrentDescription->mNameable ? mCurrentDescription->mNameable->mAlwaysShow : false);
//
//	if (mCurrentDescription->mIsBaby) {
//		setStatusFlag(EntityFlags::BABY, *mCurrentDescription->mIsBaby);
//	}
//	else {
//		setStatusFlag(EntityFlags::BABY, false);
//	}
//
//	if (mCurrentDescription->mIsStackable) {
//		setStatusFlag(EntityFlags::STACKABLE, *mCurrentDescription->mIsStackable);
//	}
//	else {
//		setStatusFlag(EntityFlags::STACKABLE, false);
//	}
//
//	if (mCurrentDescription->mIsCharged) {
//		setStatusFlag(EntityFlags::POWERED, *mCurrentDescription->mIsCharged);
//	}
//	else {
//		setStatusFlag(EntityFlags::POWERED, false);
//	}
//
//	if (mCurrentDescription->mIsSaddled) {
//		setStatusFlag(EntityFlags::SADDLED, *mCurrentDescription->mIsSaddled);
//	}
//	else {
//		setStatusFlag(EntityFlags::SADDLED, false);
//	}
//
//	if (mCurrentDescription->mIsChested) {
//		setStatusFlag(EntityFlags::CHESTED, *mCurrentDescription->mIsChested);
//	}
//	else {
//		setStatusFlag(EntityFlags::CHESTED, false);
//	}
//
//	if (mCurrentDescription->mIsIgnited) {
//		setStatusFlag(EntityFlags::IGNITED, *mCurrentDescription->mIsIgnited);
//	}
//	else {
//		setStatusFlag(EntityFlags::IGNITED, false);
//	}
//
//	if (mCurrentDescription->mIsShaking) {
//		setStatusFlag(EntityFlags::SHAKING, *mCurrentDescription->mIsShaking);
//	}
//	else {
//		setStatusFlag(EntityFlags::SHAKING, false);
//	}
//
//	setStatusFlag(EntityFlags::SHEARED, mCurrentDescription->mIsSheared ? *mCurrentDescription->mIsSheared : false);
//
//	// TODO trae this should probably not exist, using for skeleton and zombie horses since they cannot be mount tamed by design
//	if (mCurrentDescription->mIsTamed) {
//		setStatusFlag(EntityFlags::TAMED, *mCurrentDescription->mIsTamed);
//	}
//
//	// Calculate entity attributes
//	if (mCurrentDescription->mScale) {
//		mEntityData.set(enum_cast(EntityDataIDs::SCALE), *mCurrentDescription->mScale);			
//	}
//	else {
//		mEntityData.set(enum_cast(EntityDataIDs::SCALE), (SynchedEntityData::TypeFloat)1.0f);
//	}
//	// Size
//	if (mCurrentDescription->mBBDim) {
//		setSize(mCurrentDescription->mBBDim->x, mCurrentDescription->mBBDim->y);
//	}
//}
//
//void Entity::reloadComponents(InitializationMethod method, const VariantParameterList& params) {
//	EntityDefinitionDescriptor& changedDescription = mDefinitionList.getChangedDescription();
//
//	_tryReloadComponent(this, mCurrentDescription->mAddRider, mAddRiderComponent, changedDescription.mAddRider);
//	_tryReloadComponent(this, mCurrentDescription->mTransformation, mTransformationComponent, changedDescription.mTransformation);
//}
//
//void Entity::updateDescription() {
//	mCurrentDescription = mDefinitionList.getDescription(true);
//}
//
//void Entity::addDefinitionGroup(const std::string& name) {
//	mDefinitionList.addDefinition(name);
//}
//
//void Entity::removeDefinitionGroup(const std::string& name) {
//	mDefinitionList.removeDefinition(name);
//}
//
//bool Entity::hasDefinitionGroup(const std::string& name) const {
//	return mDefinitionList.hasDefinition(name);
//}
//
//void Entity::setBaseDefinition(const std::string& name, bool clearDefs, bool update) {
//	EntityDefinitionPtr ptr = mDefinitions.tryGetDefinition(name);
//	if (!ptr) {
//		return;
//	}
//
//	std::vector<std::pair<bool, EntityDefinitionPtr>> defs = mDefinitionList.getDefinitionStack();
//	mDefinitionList.clearDefinitions();
//	if (clearDefs) {
//		defs.clear();
//	}
//
//	auto pair = std::make_pair(true, ptr);
//	if (!defs.empty()) {
//		defs[0] = pair;
//	}
//	else {
//		defs.push_back(pair);
//	}
//
//	mDefinitionList.setDefinitionStack(defs);
//	if (update) {
//		updateDescription();
//	}
//
//}
//
//bool Entity::hasComponent(const std::string &name) const {
//	return mCurrentDescription->hasComponent(name);
//}
//
//bool Entity::hasInventory() const {
//	return getContainerComponent() != nullptr;
//}
//
//Container& Entity::getInventory() {
//	return *getContainerComponent()->mContainer.get();
//}
//
//bool Entity::hasCategory(EntityCategory categories) const {
//	return (mCategories & categories) == categories;
//}
//
//bool Entity::hasType(EntityType types) const {
//	EntityType myType = getEntityTypeId();
//	return ((int)myType & (int)types) == (int)types;
//}
//
//bool Entity::hasFamily(const std::string& family) const {
//	if (mCurrentDescription) {
//		EntityDefinitionDescriptor& def = *mCurrentDescription;
//		if (def.mFamilyTypes) {
//			auto && familyTypes = *def.mFamilyTypes;
//			auto iter = std::find(familyTypes.begin(), familyTypes.end(), family);
//			if (iter != familyTypes.end()) {
//				return true;
//			}
//		}
//	}
//	return false;
//}
//
//bool Entity::isSpawnableInCreative(EntityType type) {
//	const static std::set<EntityType> validEntities =
//	{
//		EntityType::Cow,
//		EntityType::Pig,
//		EntityType::Sheep,
//		EntityType::Chicken,
//		EntityType::Wolf,
//		EntityType::PolarBear,
//		EntityType::MushroomCow,
//		EntityType::Rabbit,
//		EntityType::Creeper,
//		EntityType::EnderMan,
//		EntityType::Silverfish,
//		EntityType::Skeleton,
//		EntityType::WitherSkeleton,
//		EntityType::Stray,
//		EntityType::Slime,
//		EntityType::Spider,
//		EntityType::CaveSpider,
//		EntityType::Zombie,
//		EntityType::Husk,
//		EntityType::PigZombie,
//		EntityType::Villager,
//		EntityType::Squid,
//		EntityType::Ocelot,
//		EntityType::Bat,
//		EntityType::Ghast,
//		EntityType::LavaSlime,
//		EntityType::Blaze,
//		EntityType::Horse,
//		EntityType::Npc,
//		EntityType::Donkey,
//		EntityType::Mule,
//		EntityType::ZombieHorse,
//		EntityType::SkeletonHorse,
//		EntityType::Guardian,
//		EntityType::Endermite,
//	};
//
//	return validEntities.count(type) > 0;
//}

bool Entity::hasUniqueID() const {
	return (bool)mUniqueID;
}

//bool Entity::hasRuntimeID() const {
//	return (bool)mRuntimeID;
//}
//
//EntityRuntimeID Entity::getRuntimeID() const {
//	DEBUG_ASSERT(mRuntimeID, "Runtime ID has not been set!");
//
//	return mRuntimeID;
//}

const EntityUniqueID& Entity::getUniqueID() const {
	//HACK the unique ID should be assigned non-lazily!
	if (!mUniqueID) {
		mUniqueID = mLevel->getNewUniqueID();
	}

	return mUniqueID;
}

//void Entity::setRuntimeID(EntityRuntimeID ID) {
//	DEBUG_ASSERT(!mRuntimeID, "This entity already has a unique ID you villain");
//
//	mRuntimeID = ID;
//}

void Entity::setUniqueID(EntityUniqueID ID) {
	DEBUG_ASSERT(ID, "Invalid ID");
	DEBUG_ASSERT(!mUniqueID, "This entity already has a unique ID you villain");

	//TODO HACK put a breakpoint here on a client to trigger spawn in the air!!!
	mUniqueID = ID;
}

//void Entity::readAdditionalSaveData(const CompoundTag& tag) {
//
//}
//void Entity::addAdditionalSaveData(CompoundTag& tag) {
//}
//
//bool Entity::isInClouds() const {
//	return mPos.y > Level::CLOUD_HEIGHT && mPos.y < (Level::CLOUD_HEIGHT + 4);
//}
//
//int Entity::getVariant() const {
//	return mEntityData.getInt(enum_cast(EntityDataIDs::VARIANT));
//}
//
//void Entity::setVariant(int value) {
//	mEntityData.set(enum_cast(EntityDataIDs::VARIANT), SynchedEntityData::TypeInt(value));
//}
//
//int Entity::getMarkVariant() const {
//	return mEntityData.getInt(enum_cast(EntityDataIDs::MARK_VARIANT));
//}
//
//void Entity::setMarkVariant(int value) {
//	mEntityData.set(enum_cast(EntityDataIDs::MARK_VARIANT), SynchedEntityData::TypeInt(value));
//}
//
//Entity* Entity::getTarget() const{
//	return mTargetId ? getLevel().fetchEntity(mTargetId) : nullptr;
//}
//
//EntityUniqueID Entity::getTargetId() {
//	return mTargetId;
//}
//
//void Entity::setTarget(Entity* entity) {
//	if (entity != nullptr) {
//		if (mTargetId == entity->getUniqueID()) {
//			return;
//		}
//		mTargetId = entity->getUniqueID();
//		if (mCurrentDescription->mOnTargetAcquired) {
//			VariantParameterList params;
//			initParams(params);
//			mCurrentDescription->executeTrigger(*this, *mCurrentDescription->mOnTargetAcquired, params);
//		}
//		if (EntityClassTree::isMob(getEntityTypeId())) {
//			playSynchronizedSound(isBaby()? LevelSoundEvent::MobWarningBaby : LevelSoundEvent::MobWarning, getAttachPos(EntityLocation::Head));
//		}
//	}
//	else {
//		if (mTargetId == EntityUniqueID()) {
//			return;
//		}
//		if (mCurrentDescription->mOnTargetEscape) {
//			VariantParameterList params;
//			initParams(params);
//			mCurrentDescription->executeTrigger(*this, *mCurrentDescription->mOnTargetEscape, params);
//		}
//		mTargetId = EntityUniqueID();
//	}
//}
//
//void Entity::setBlockTarget(const BlockPos& target) {
//	mEntityData.set(enum_cast(EntityDataIDs::BLOCK_TARGET), target);
//}
//BlockPos Entity::getBlockTarget(void) {
//	return mEntityData.getPos(enum_cast(EntityDataIDs::BLOCK_TARGET));
//}
//
//bool Entity::isBaby() const {
//	return getStatusFlag(EntityFlags::BABY);
//}
//
//bool Entity::isStackable() const {
//	return getStatusFlag(EntityFlags::STACKABLE);
//}
//
//int Entity::getAge() const {
//	if (mAgeableComponent) {
//		return mAgeableComponent->getAge();
//	}
//	return 0;
//}
//
//bool Entity::isTame() const {
//	return getStatusFlag(EntityFlags::TAMED);
//}
//
//Player * Entity::getOwner() const {
//	return getLevel().getPlayer(getOwnerId());
//}
//
//void Entity::setOwner(const EntityUniqueID ownerId) {
//	mEntityData.set(enum_cast(EntityDataIDs::OWNER), static_cast<int64_t>(ownerId));
//	//should we also update the tamed flag here? To preserve old saves?
//}
//
//const EntityUniqueID Entity::getOwnerId() const {
//	return (EntityUniqueID)mEntityData.getInt64(enum_cast(EntityDataIDs::OWNER));
//}
//
//bool Entity::isSitting() const {
//	return getStatusFlag(EntityFlags::SITTING);
//}
//
//void Entity::setSitting(bool value) {
//	setStatusFlag(EntityFlags::SITTING, value);
//}
//
//bool Entity::isInWall() const {
//	return mRegion->isInWall(mPos + Vec3(0.0f, getHeadHeight(), 0.0f));
//}

bool Entity::isInWater() const {
	return mWasInWater;
}

//bool Entity::isInWaterOrRain() {
//	Weather& weather = mRegion->getDimension().getWeather();
//	Vec3 pos = getPos().add(0.0f, mBBDim.y, 0.0f);
//
//	return mWasInWater || weather.isRainingAt(getRegion(), pos);
//}
//
//void Entity::setPersistent() {
//	mPersistent = true;
//}
//
//bool Entity::isInLava() {
//	// Ok, lets use the same logic as Java, where they swap y to ensure min/max y is correct after growing with a
//	// negative value.
//	// The AABB box code below replaces:
//	// bb.grow(Vec3(-0.1f, -0.4f, -0.1f))
//	Vec3 v0(mBB.min.x + 0.1f, mBB.min.y + 0.4f, mBB.min.z + 0.1f);
//	Vec3 v1(mBB.max.x - 0.1f, mBB.max.y - 0.4f, mBB.max.z - 0.1f);
//
//	AABB box(std::min(v0.x, v1.x), std::min(v0.y, v1.y), std::min(v0.z, v1.z),
//		std::max(v0.x, v1.x), std::max(v0.y, v1.y), std::max(v0.z, v1.z));
//
//	return mRegion->containsMaterial(box, MaterialType::Lava);
//}
//
//bool Entity::isFree(const Vec3& vec, float grow) {
//	AABB box = mBB.grow(Vec3(grow)).cloneMove(vec);
//	auto& aABBs = mRegion->fetchAABBs(box);
//	if (aABBs.size() > 0) {
//		return false;
//	}
//
//	if (mRegion->containsAnyLiquid(box)) {
//		return false;
//	}
//
//	return true;
//}
//
//bool Entity::isFree(const Vec3& vec) {
//	AABB box = mBB.cloneMove(vec);
//	auto& aABBs = mRegion->fetchAABBs(box);
//	if (aABBs.size() > 0) {
//		return false;
//	}
//
//	if (mRegion->containsAnyLiquid(box)) {
//		return false;
//	}
//
//	return true;
//}
//
///*public*/
//void Entity::move(const Vec3& posIn) {
//	Vec3 speed(posIn);
//	// stupid check
//	if (isImmobile() || std::abs(speed.x) > 500 || std::abs(speed.y) > 500 || std::abs(speed.z) > 500) {
//		ALOGE(LOG_AREA_ENTITY, "Entity moved WAAAY too fast!\n");
//		speed.x = speed.y = speed.z = 0;
//	}
//
//	if (mNoPhysics) {
//		moveBBs(speed);
//		mPos.x = (mBB.min.x + mBB.max.x) / 2.0f;
//		mPos.y = mBB.min.y + mHeightOffset - mSlideOffset.y;
//		mPos.z = (mBB.min.z + mBB.max.z) / 2.0f;
//		return;
//	}
//
//	ScopedProfile("move");
//
//	// add all intersecting aabb's to one box
//	std::vector<AABB>& preIntersectingAabbs = mRegion->fetchCollisionShapes(getRegion(), mBB, &mTerrainSurfaceOffset, false, this);
//	AABB allIntersects = AABB::EMPTY;
//	for(auto& it : preIntersectingAabbs) {
//		if(allIntersects.isEmpty()) {
//			allIntersects = it;
//		}
//		else {
//			allIntersects.merge(it);
//		}
//	}
//
//	if (mIsStuckInWeb) {
//		mIsStuckInWeb = false;
//
//		speed.x *= .25f;
//		speed.y *= .05f;
//		speed.z *= .25f;
//		mPosDelta.x = .0f;
//		mPosDelta.y = .0f;
//		mPosDelta.z = .0f;
//	}
//
//	float xaOrg = speed.x;
//	float yaOrg = speed.y;
//	float zaOrg = speed.z;
//
//	AABB bbOrg = mBB;
//
//	bool sneaking = mOnGround && isSneaking();
//
//	if (sneaking) {
//		float d = 0.05f;
//		
//		//bias the step size a bit to make sure we reach the bottom of the step
//		float maxUpStep = mMaxAutoStep * 1.01f;
//
//		while (speed.x != 0 && mRegion->fetchCollisionShapes(getRegion(), mBB.cloneMove(Vec3(speed.x, -maxUpStep, 0)), nullptr, false, this).empty()) {
//			if (speed.x < d && speed.x >= -d) {
//				speed.x = 0;
//			}
//			else if (speed.x > 0) {
//				speed.x -= d;
//			}
//			else {
//				speed.x += d;
//			}
//
//			xaOrg = speed.x;
//		}
//
//		while (speed.z != 0 && mRegion->fetchCollisionShapes(getRegion(), mBB.cloneMove(Vec3(0, -maxUpStep, speed.z)), nullptr, false, this).empty()) {
//			if (speed.z < d && speed.z >= -d) {
//				speed.z = 0;
//			}
//			else if (speed.z > 0) {
//				speed.z -= d;
//			}
//			else {
//				speed.z += d;
//			}
//
//			zaOrg = speed.z;
//		}
//
//		while (speed.x != 0 && speed.z != 0 && mRegion->fetchCollisionShapes(getRegion(), mBB.cloneMove(Vec3(speed.x, -maxUpStep, speed.z)), nullptr, false, this).empty()) {
//			if (speed.x < d && speed.x >= -d) {
//				speed.x = 0;
//			}
//			else if (speed.x > 0) {
//				speed.x -= d;
//			}
//			else {
//				speed.x += d;
//			}
//
//			if (speed.z < d && speed.z >= -d) {
//				speed.z = 0;
//			}
//			else if (speed.z > 0) {
//				speed.z -= d;
//			}
//			else {
//				speed.z += d;
//			}
//
//			xaOrg = speed.x;
//			zaOrg = speed.z;
//		}
//
//	}
//	Vec3 orgSpeed = speed;
//
//	AABB intersectTestBox = mBB.expanded(speed);
//	std::vector<AABB>& aABBs = mRegion->fetchCollisionShapes(getRegion(), intersectTestBox, &mTerrainSurfaceOffset, false, this);
//
//	bool collidableWasNear = mCollidableMobNear;
//
//	//	Because some entities can collide and interact differently with other entities (I.E. minecarts stacking) we want to go ahead and
//	//	have the entity do it's own test for collision so we're not unnecessarily testing for collisions.
//	if (isStackable()) {
//		testForEntityStacking(*mRegion, intersectTestBox, aABBs);
//	}
//
//	else if (mCollidableMobNear) {
//		testForCollidableMobs(*mRegion, intersectTestBox, aABBs);
//		mCollidableMobNear = false;
//	}
//
//	// Use oneway collision for all blocks if the entity is within any oneway blocks, or they take advantage of one way physics intentionally,
//	// or if they're stuck, to prevent thrashing around trying to resolve
//	bool onewayCollide = !mOnewayPhysicsBlocks.empty() || mUsesOneWayCollision || mStuckInCollider;
//
//	// This is broken into 3 passes for the sake of smooth movement. Neighboring blocks on a smooth surface align the movement vector with the surface if possible
//	// If all done at once, entities can catch on corners vertical edges while trying to move horizontally along a surface, or similarly when brushing up against walls
//	Vec3 steps[3] = { Vec3(0.0f, speed.y, 0.0f), Vec3(speed.x, 0.0f, 0.0f), Vec3(0.0f, 0.0f, speed.z) };
//	Vec3 penetration(0.0f);
//
//	for (int j = 0; j < 3; ++j) {
//		Vec3& step = steps[j];
//		float& pen = penetration[j];
//		for (auto& bb : aABBs) {
//			float tempPenetration;
//			step = bb.clipCollide(mBB, step, onewayCollide, &tempPenetration);
//			pen = std::max(pen, tempPenetration);
//		}
//		moveBBs(step);
//	}
//
//	float totalPenetration = penetration.x + penetration.y + penetration.z;
//	float epsilon = FLT_EPSILON;
//	// If there was no penetration, then we're not stuck anymore
//	if(mStuckInCollider && totalPenetration <= epsilon) {
//		mStuckInCollider = false;
//	}
//	// if there was penetration last frame we might have fixed an error, but if it's also this frame, then we're probably stuck between two objects
//	else if(!mStuckInCollider && mLastPenetration > epsilon && totalPenetration > epsilon) {
//		mStuckInCollider = true;
//	}
//	mLastPenetration = totalPenetration;
//
//	// Combine the collective move we just did back into here for later use
//	speed = steps[0] + steps[1] + steps[2];
//	bool og = mOnGround || (yaOrg != speed.y && yaOrg < 0);
//
//	// If we ran into a vertical wall and we're on the ground
//	if (mMaxAutoStep > 0 && og && ((xaOrg != speed.x) || (zaOrg != speed.z))) {
//		// Try stepping up onto what's in front of us using our foot size, storing old velocity in case we can't do it
//		// This is done by first trying to move up vertically, then over horizontally, in the hopes that the first move cleared the obstacle
//		float xaN = speed.x;
//		float yaN = speed.y;
//		float zaN = speed.z;
//
//		// Use the unadjusted speed before collision as we're trying again
//		Vec3 speedX(xaOrg, 0.0f, 0.0f);
//		Vec3 speedY(0.0f, mMaxAutoStep, 0.0f);
//		Vec3 speedZ(0.0f, 0.0f, zaOrg);
//
//		// Store original bounding box in case we want to revert
//		AABB normal = mBB;
//		std::vector<AABB> norm;
//		for (auto& bb : mSubBBs) {
//			norm.push_back(bb);
//		}
//		mBB.set(bbOrg);
//		// Fetch collision shapes using original speed, this will exclude colliders above head, we'll check those at the end
//		aABBs = mRegion->fetchCollisionShapes(getRegion(), mBB.expanded(orgSpeed), nullptr, true, this);
//
//		if (collidableWasNear) {
//			testForCollidableMobs(*mRegion, intersectTestBox, aABBs);
//		}
//
//		// Try to step up over the obstacle
//		// Even though the motion is only on y, we don't want to use clipYCollide because that will force penetration resolution along the y axis, which may not be the smallest penetrating axis
//		for (auto& bb : aABBs) {
//			speedY = bb.clipCollide(mBB, speedY, onewayCollide);
//		}
//		moveBBs(speedY);
//
//		// Now that we've attempted the vertical step, do the horizontal ones, which will hopefully not be impeded by the same obstacle as before
//		for (auto& bb : aABBs) {
//			speedX = bb.clipCollide(mBB, speedX, onewayCollide);
//		}
//		moveBBs(speedX);
//
//		for (auto& bb : aABBs) {
//			speedZ = bb.clipCollide(mBB, speedZ, onewayCollide);
//		}
//		moveBBs(speedZ);
//
//		// Undo as much y axis movement as possible, we're climping up, not jumping up
//		Vec3 reverseSpeedY = -speedY;
//		for (auto& bb : aABBs) {
//			reverseSpeedY = bb.clipCollide(mBB, reverseSpeedY, onewayCollide);
//		}
//		moveBBs(reverseSpeedY);
//		speedY += reverseSpeedY;
//
//		// Store the movement we just did in here for later use if we decide to keep it
//		speed = speedX + speedY + speedZ;
//		// See if we hit our head on the ceiling, meaning we shouldn't do step up.
//		// This makes getting head stuck in ceiling possible if moving into a block lower than footsize at a speed larger than you bounding box width. This shouldn't be achievable by standard means
//		// Not expanding bb by anything because we just want to see if we ended up in a good spot
//		bool hitCeiling = !mRegion->fetchCollisionShapes(getRegion(), mBB, nullptr, true, this).empty();
//
//		// If our original horizontal square velocity was higher without trying to step over the obstacle, or our head ended up in the ceiling, revert back to that for the sake of smoother movement
//		if (hitCeiling || xaN * xaN + zaN * zaN >= speed.x * speed.x + speed.z * speed.z) {
//			speed.x = xaN;
//			speed.y = yaN;
//			speed.z = zaN;
//			mBB.set(normal);
//			for (size_t i = 0; i < mSubBBs.size(); ++i) {
//				mSubBBs[i] = norm[i];
//			}
//		}
//		// Else our vertical step cleared the obstacle and have more horizontal velocity than before, so keep the changes
//		// Don't do mSlideOffset on client side except for players because it can cause tunneling for entities
//		else if(!getLevel().isClientSide() || (getLevel().getLocalPlayer()->getUniqueID() == getUniqueID())) {
//			// Add an artificial downward offset that will ease up over time to smooth out the sudden change in height caused by this step
//			mSlideOffset.y += speedY.y;
//		}
//	}
//
//	{
//		ScopedProfile("rest");
//
//		mPos.x = (mBB.min.x + mBB.max.x) / 2.0f;
//		mPos.y = mBB.min.y + mHeightOffset - mSlideOffset.y;
//		mPos.z = (mBB.min.z + mBB.max.z) / 2.0f;
//
//		mHorizontalCollision = (xaOrg != speed.x) || (zaOrg != speed.z);
//		mVerticalCollision = (yaOrg != speed.y);
//
//		mOnGround = (yaOrg != speed.y && yaOrg < 0) || (mOnGround && yaOrg == speed.y && yaOrg == 0);
//		mCollision = mHorizontalCollision || mVerticalCollision;
//
//		checkFallDamage(speed.y, mOnGround);
//
//		if (xaOrg != speed.x) {
//			mPosDelta.x = 0;
//			onBlockCollision(0);
//		}
//
//		if (yaOrg != speed.y) {
//			auto stepPos = _getBlockOnPos();
//			auto& onBlock = mRegion->getBlock(stepPos);
//
//			onBlock.updateEntityAfterFallOn(*this);
//			onBlockCollision(1);
//
//			if (!onBlock.isType(Block::mAir) && mOnGround) {
//				onBlock.onStepOn(*this, stepPos);
//			}
//
//			if (!sneaking) {
//				Vec3 delta = mPos - mPosPrev;
//				mWalkDist += Math::sqrt(delta.x * delta.x + delta.z * delta.z) * 0.6f;
//			}
//		}
//
//		if (zaOrg != speed.z) {
//			mPosDelta.z = 0;
//			onBlockCollision(2);
//		}
//
//		_playMovementSound(mVerticalCollision);
//
//		mSlideOffset.y *= 0.4f;
//
//		checkBlockCollisions();
//	}
//}
//
//// for localPlayer and mobs on server side, only play step sound when they make a step
//// for client side, relying only on the mMoveDist to decide when to play step sound, for the lack of 
//// collision information
//void Entity::_playMovementSound(bool tryStepSound) {
//	
//	Vec3 posDelta = mPos - mPosPrev;
//	if (mMakeStepSound && !isSilent()) {
//		if (onLadder(false)) {
//			mMoveDist += posDelta.length() * 0.6f; 
//		}
//		else {
//			mMoveDist += Math::sqrt(posDelta.x*posDelta.x + posDelta.z*posDelta.z) * 0.6f;
//		}
//
//		if (mMoveDist > mNextStep) {
//			mNextStep = ((int)mMoveDist) + 1;
//			if (onLadder(false)) {
//				playSound(LevelSoundEvent::Step, getAttachPos(EntityLocation::Feet), Block::mLadder->getId());
//			}
//			else if (isInWater()) {
//				float swimSpeed = sqrt(posDelta.x * posDelta.x * 0.2f + posDelta.y*posDelta.y + posDelta.z*posDelta.z * 0.2f) * 0.35f;
//				if (swimSpeed > 1) {
//					swimSpeed = 1;
//				}
//				playSound(LevelSoundEvent::Swim, getAttachPos(EntityLocation::Body), static_cast<int>(swimSpeed * std::numeric_limits<int>::max()));
//			}
//			else if (tryStepSound && !isSneaking()) {
//				auto stepPos = _getBlockOnPos();
//				int onBlockID = getRegion().getBlockID(stepPos);
//				if (onBlockID != BlockID::AIR) {
//					_playStepSound(mPos, onBlockID);
//				}
//			}
//		}
//	}
//}
//
//
//
//// Turns the physics to only prevent following collisions, not resolve current ones
//void Entity::onOnewayCollision(BlockPos incomingPosition) {
//	// Unit block at this position
//	AABB newBB = AABB(Vec3::ZERO, Vec3::ONE).move(incomingPosition);
//	onOnewayCollision(newBB);
//}
//
//void Entity::onOnewayCollision(const AABB& oneWayVolume) {
//	auto found = std::find(mOnewayPhysicsBlocks.begin(), mOnewayPhysicsBlocks.end(), oneWayVolume);
//	if (found == mOnewayPhysicsBlocks.end()) {
//		mOnewayPhysicsBlocks.push_back(oneWayVolume);
//	}
//}
//
//// Check to see if the object position should be removed from this entities' oneway collision list
//void Entity::_updateOnewayCollisions(BlockSource& region) {
//	for (auto it = mOnewayPhysicsBlocks.begin(); it != mOnewayPhysicsBlocks.end(); ) {
//		// If the AABB no longer intersects, remove it form the list
//		if (!it->intersects(mBB)) {
//			it = mOnewayPhysicsBlocks.erase(it);
//		}
//		else {
//			++it;
//		}
//	}
//}
//
//bool Entity::canDestroyBlock(const Block& block) const {
//	return true;
//}
//
//// Helper function for going through intersecting entities and adding them to the oneway collision list
//void Entity::checkEntityOnewayCollision(BlockSource& region, const BlockPos& position) {
//	// Go through overlapping entities
//	AABB insideBlockAABB = AABB(Vec3::ZERO, Vec3::ONE).move(position);
//	const EntityList& broadSwipeEntities = region.getEntities(nullptr, insideBlockAABB);
//
//	// If they are inside, make them use oneway physics
//	for (auto &entity : broadSwipeEntities) {
//		entity->onOnewayCollision(position);
//	}
//}
//
//void Entity::testForEntityStacking(BlockSource& region, const AABB& intersectTestBox, std::vector<AABB>& collisionAABBs) {
//	float r = 0.25f;
//	for (auto&& entity : region.getEntities(this, intersectTestBox.grow(Vec3(r, r, r)))) {
//		if (entity->isStackable() && entity->mBB.intersects(intersectTestBox)) {
//			collisionAABBs.push_back(entity->mBB);
//		}
//	}
//}
//
//void Entity::testForCollidableMobs(BlockSource& region, const AABB& intersectTestBox, std::vector<AABB>& collisionAABBs) {
//	float r = 0.25f;
//	for (auto&& entity : region.getEntities(this, intersectTestBox.grow(Vec3(r, r, r)))) {
//		if (entity->getCollidableMob() && entity->mBB.intersects(intersectTestBox)) {
//			collisionAABBs.push_back(entity->mBB);
//		}
//	}
//}
//
//void Entity::makeStuckInWeb() {
//	mIsStuckInWeb = true;
//	mFallDistance = 0;
//}
//
///*public virtual*/
//bool Entity::isUnderLiquid(MaterialType type) const {
//
//	Vec3 headPos = mPos + Vec3(0, getHeadHeight(), 0);
//
//	if (isRiding() && mRiding->mBB.contains(headPos)) {
//		return false;
//	}
//
//	float yp = mPos.y + getHeadHeight();
//	int xt = Math::floor(mPos.x);
//	int yt = Math::floor((float)Math::floor(yp));
//	int zt = Math::floor(mPos.z);
//	auto& mat = mRegion->getMaterial(xt, yt, zt);
//	if (mat.isLiquid() && mat.isType(type)) {
//		float hh = LiquidBlock::getHeightFromData(mRegion->getData(xt, yt, zt)) - 1 / 9.0f;
//		float h = yt + 1 - hh;
//		return yp < h;
//	}
//
//	return false;
//}
//
//void Entity::_updateOwnerChunk() {
//	BlockPos blockPos(mPos);
//	ChunkPos cp(blockPos);
//	if (isInWorld() && mChunkPos != cp) {
//		if (!hasCategory(EntityCategory::Player)) {//yeah this is quite horrible
//			//get old chunk
//			auto oldChunk = getRegion().getChunk(mChunkPos);
//			auto newChunk = getRegion().getChunk(cp);
//
//			if (!oldChunk || !newChunk) {
//				return;
//			}
//
//			DEBUG_ASSERT(oldChunk && newChunk, "Both chunks should always be loaded here");
//
//			//handoff the entity
//			newChunk->addEntity(oldChunk->removeEntity(*this));
//		}
//	}
//}
//
/*protected virtual*/
// void Entity::setPos(const EntityPos& pos) {
// 	if (pos.mMoved) {
// 		setPos(pos);
// 	}
// 	else {
// 		setPos(mPos);
// 	}
// 
// 	if (pos.mRotated) {
// 		setRot(pos.mRot);
// 	}
// 	else {
// 		setRot(mRot);
// 	}
// }

void Entity::setPos(const Vec3 &pos) {
	mPos = pos;
	float w = mBBDim.x / 2;
	float h = mBBDim.y;
	mBB.set(pos.x - w, pos.y - mHeightOffset + mSlideOffset.y, pos.z - w, pos.x + w, pos.y - mHeightOffset + mSlideOffset.y + h, pos.z + w);

// 	if (mAdded && !getLevel().isClientSide()) {
// 		_updateOwnerChunk();
// 	}
}

const Vec3& Entity::getPos() const {
	return mPos;
}

const Vec3& Entity::getPosOld() const {
	return mPosPrev;
}

const Vec3 Entity::getPosExtrapolated(float factorAlpha) const {
	//TODO this duplicates getInterpolatedPosition
	const Vec3 ret = getPosOld() + (getPos() - getPosOld()) * factorAlpha;
	return ret;
}

const Vec3& Entity::getVelocity() const {
	return mPosDelta;
}

//void Entity::setPreviousPosRot(const Vec3& pos, const Vec2& rot) {
//	if (!mPrevPosRotSetThisTick) {
//		mPosPrev2 = mPosPrev = pos;
//		mRotPrev = rot;
//
//		mPrevPosRotSetThisTick = true;
//	}
//}
//
//Vec3 Entity::getAttachPos(EntityLocation location) const {
//	switch (location)
//	{
//	case EntityLocation::Feet:
//		return getPos() + Vec3{0, -mHeightOffset, 0};
//
//	case EntityLocation::Body:
//	case EntityLocation::WeaponAttachPoint:
//		return mBB.getCenter();
//
//	case EntityLocation::Head:
//		return getPos() + Vec3{ 0, getHeadHeight() - mHeightOffset, 0 };
//
//	case EntityLocation::DropAttachPoint: {
//		auto offset = Vec3(0.f, getHeadHeight() - 0.2f, 0.15f);
//		offset.yRot(-mRot.y * Math::DEGRAD);
//
//		return getPos() + offset;
//	}
//	default:
//		DEBUG_FAIL("Not handled");
//		return getPos();
//	}
//}
//
//float Entity::getRadius() const {
//	return Math::max(mBBDim.x, mBBDim.y) * 0.5f;
//}
//
//bool Entity::isSkyLit(float a) {
//	Vec3 pos = mPos;
//	pos.y -= mHeightOffset + (mBB.max.y - mBB.min.y) * 0.66f;
//	BlockPos block(pos);
//	return mRegion->canSeeSky(block);
//}

Vec3 Entity::getCenter(float a) const {
	Vec3 curPos = getInterpolatedPosition(a);

	curPos.y += mBB.getBounds().y * 0.66f - mHeightOffset;
	return curPos;
}

//float Entity::getBrightness(float a) const {
//	return mRegion->getBrightness(getCenter(a));
//}

Vec3 Entity::getViewVector(float a) const {
	Vec2 rot = getInterpolatedRotation(a);

	float yCos = Math::cos(-rot.y * Math::DEGRAD - PI);
	float ySin = Math::sin(-rot.y * Math::DEGRAD - PI);
	float xCos = -Math::cos(-rot.x * Math::DEGRAD);
	float xSin = Math::sin(-rot.x * Math::DEGRAD);

	return Vec3(ySin * xCos, xSin, yCos * xCos);
}

Vec2 Entity::getViewVector2(float a) const {
	Vec2 rot = getInterpolatedRotation(a);
	return Vec2(-Math::cos(rot.y * Math::DEGRAD - PI / 2), -Math::sin(rot.y * Math::DEGRAD - PI / 2));
}

bool Entity::operator==(Entity& rhs) {
	return mUniqueID == rhs.mUniqueID;
}

void Entity::remove() {
// 	if (mBossComponent != nullptr) {
// 		auto& playerList = mLevel->getPlayerList();
// 		for (auto& player : playerList) {
// 			Player* playerPtr = mLevel->getPlayer(player.first);
// 			if (playerPtr != nullptr) {
// 				mBossComponent->unRegisterPlayer(playerPtr);
// 			}
// 		}
// 	}

	mRemoved = true;
}

//void Entity::setSize(float w, float h) {
//	float scale = mEntityData.getFloat(enum_cast(EntityDataIDs::SCALE));
//
//	if (w*scale != mBBDim.x || h*scale != mBBDim.y) {
//
//		mBBDim.x = w*scale;
//		mBBDim.y = h*scale;
//
//		float halfWidth = mBBDim.x * 0.5f;
//		const Vec3& pos = getPos();
//
//		mBB.set(pos.x - halfWidth, mBB.min.y, pos.z - halfWidth, pos.x + halfWidth, mBB.min.y + mBBDim.y, pos.z + halfWidth);
//
//		mEntityData.set<SynchedEntityData::TypeFloat>(enum_cast(EntityDataIDs::WIDTH), w);
//		mEntityData.set<SynchedEntityData::TypeFloat>(enum_cast(EntityDataIDs::HEIGHT), h);
//	}
//}

void _rotationWrapWithInterpolation(float& rot, float& oRot) {
	while (rot > 360.f) {
		rot -= 360.f;
		oRot -= 360.f;
	}

	while (rot < -360.f) {
		rot += 360.f;
		oRot += 360.f;
	}
}

void Entity::setRot(const Vec2& rot) {
	mRot.y = rot.y;
	mRot.x = rot.x;

	_rotationWrapWithInterpolation(mRot.y, mRotPrev.y);
	_rotationWrapWithInterpolation(mRot.x, mRotPrev.x);
}


//void Entity::turn(const Vec2& rot, bool useScaling) {
//	float xRotOld = mRot.x;
//	float yRotOld = mRot.y;
//
//	const float smoothval = useScaling ? 0.15f : 1.0f;
//	mRot.y += rot.y * smoothval;
//	mRot.x -= rot.x * smoothval;
//	if (mRot.x < -90) {
//		mRot.x = -90;
//	}
//
//	if (mRot.x > 90) {
//		mRot.x = 90;
//	}
//
//	mRotPrev.x += mRot.x - xRotOld;
//	mRotPrev.y += mRot.y - yRotOld;
//}
//
//void Entity::setEnforceRiderRotationLimit(bool value) {
//	mEnforceRiderRotationLimit = value;
//}
//
//void Entity::setInheritRotationWhenRiding(bool value) {
//	mInheritRotationWhenRiding = value;
//}
//
//float Entity::getSpeedInMetersPerSecond() const {
//	return (mPos - mPosPrev).length() * SharedConstants::TicksPerSecond;
//}
//
//bool Entity::enforceRiderRotationLimit() {
//	return mEnforceRiderRotationLimit;
//}
//
//
//Unique<Packet> Entity::getAddPacket() {
//	if (getEntityTypeId() != EntityType::Undefined) {
//		return make_unique<AddEntityPacket>(*this);
//	}
//
//	return nullptr;
//}
//
//void Entity::interpolateTurn(const Vec2& rot) {
//	mRot.y += rot.x * 0.15f;
//	mRot.x -= rot.y * 0.15f;
//	if (mRot.x < -90) {
//		mRot.x = -90;
//	}
//
//	if (mRot.x > 90) {
//		mRot.x = 90;
//	}
//}
//
//void Entity::normalTick() {
//	if (!getLevel().isClientSide()) {
//		setStatusFlag(EntityFlags::ONFIRE, mOnFire > 0);
//	}
//
//	baseTick();
//
//	if (mRailMovementComponent) {
//		mRailMovementComponent->tick();
//	}
//
//	if (mAgeableComponent) {
//		mAgeableComponent->tick();
//	}
//
//	if (mBreedableComponent) {
//		mBreedableComponent->tick();
//	}
//
//	if (mEnvironmentSensorComponent) {
//		mEnvironmentSensorComponent->tick();
//	}
//
//	if (mExplodeComponent) {
//		mExplodeComponent->tick();
//	}
//
//	if (mBossComponent != nullptr) {
//		mBossComponent->tick();
//	}
//
//	if (mAngryComponent) {
//		mAngryComponent->tick();
//	}
//
//	if (mBreathableComponent) {
//		mBreathableComponent->tick();
//	}
//
//	if (mProjectileComponent) {
//		mProjectileComponent->tick();
//	}
//
//	if (mLookAtComponent) {
//		mLookAtComponent->tick();
//	}
//
//	if (mTeleportComponent) {
//		mTeleportComponent->tick();
//	}
//
//	if (mMountTamingComponent) {
//		mMountTamingComponent->tick();
//	}
//
//	if (mTimerComponent) {
//		mTimerComponent->tick();
//	}
//	
//	if (mPeekComponent) {
//		mPeekComponent->tick();
//	}
//
//	if (mBoostableComponent) {
//		mBoostableComponent->tick();
//	}
//
//	if (mInteractComponent) {
//		mInteractComponent->tick();
//	}
//
//	if (mScaleByAgeComponent) {
//		mScaleByAgeComponent->tick();
//	}
//
//	if (mTransformationComponent) {
//		mTransformationComponent->tick();
//	}
//
//	if (mTargetNearbyComponent) {
//		mTargetNearbyComponent->tick();
//	}
//
//	if (mLeashableComponent) {
//		mLeashableComponent->tick();
//	}
//
//	if (mHopperComponent) {
//		mHopperComponent->tick();
//	}
//
//	if (mRailActivatorComponent) {
//		mRailActivatorComponent->tick();
//	}
//
//	if (mAgentCommandComponent) {
//		mAgentCommandComponent->tick();
//	}
//}
//
//const float ENTITY_SCALE_INCREASE_PER_TICK = 0.2f;
//void Entity::baseTick() {
//	ScopedProfile("entityBaseTick");
//
//	mTickCount++;
//	mWalkDistPrev = mWalkDist;
//
//	//set the prevpos and prevrot if it hasn't been set already
//	setPreviousPosRot(mPos, mRot);
//	mPrevPosRotSetThisTick = false;
//	mScalePrev = mScale;
//
//	if (nullptr != mRiding) {
//		mRiding->positionRider(*this);
//	}
//
//	if (mScale < 1) {
//		//inflate entities that started with a tiny visible scale using their speed, or a minimum value
//		mScale = std::min(mScale + (std::max(mPosDelta.xz().length(), ENTITY_SCALE_INCREASE_PER_TICK)), 1.f);
//	}
//
//	updateInsideBlock();
//	updateWaterState();
//
//	// in fire
//	if (!mFireImmune) {
//		bool water = this->isInWaterOrRain();
//		if (getRegion().containsFireBlock(mBB)) {
//			burn(1, true);
//			if (!water) {
//				setOnFire(8);
//			}
//		}
//
//		// particle effects
//		if (water && isOnFire() && !isInLava()) {
//			playSound(LevelSoundEvent::Fizz, getAttachPos(EntityLocation::Body));
//			mOnFire = 0;
//
//			//make a nice white smoke
//			for (int i = 0; i < 5; ++i) {
//				Vec3 rand = getRandomPointInAABB(getLevel().getRandom());
//				getLevel().addParticle(ParticleType::Evaporation, rand, Vec3::ZERO);
//			}
//		}
//	}
//
//	// on fire
//	if (getLevel().isClientSide()) {
//		mOnFire = getStatusFlag(EntityFlags::ONFIRE) ? 1 : 0;
//	}
//	else {
//		if (mOnFire > 0) {
//			// remove fire faster if immune
//			if (mFireImmune) {
//				mOnFire = std::max(mOnFire - 4, 0);
//			}
//			else {
//				// do damage once per second
//				if (mOnFire % SharedConstants::TicksPerSecond == 0) {
//					doFireHurt(1);
//				}
//				mOnFire--;
//			}
//		}
//	}
//
//	if (isInLava()) {
//		lavaHurt();
//		mFallDistance *= 0.5f;
//	}
//
//	if (mPos.y < 0 && !getLevel().isClientSide()) {
//		mFallDistance = 0;
//		outOfWorld();
//	}
//
//	if (!getLevel().isClientSide()) {
//		setStatusFlag(EntityFlags::ONFIRE, mOnFire > 0);
//	}
//
//	mFirstTick = false;
//
//	if (mChanged) {
//		mRegion->fireEntityChanged(*this);
//		mChanged = false;
//	}
//}
//
//void Entity::rideTick() {
//	if (mRiding->mRemoved) {
//		stopRiding();
//		return;
//	}
//
//	mPosDelta = Vec3(0.0f, 0.0f, 0.0f);
//	normalTick();
//	if (mRiding) {
//		//we have to let the ride position us, if it's local
//		const bool isUncontrolled = (mRiding->getControllingPlayer() == EntityUniqueID());
//		const bool isControlledRemotely = (nullptr != getLevel().getLocalPlayer()) && (mRiding->getControllingPlayer() != getLevel().getLocalPlayer()->getUniqueID());
//		if ((isControlledRemotely || isUncontrolled) && !hasCategory(EntityCategory::Player)) {
//			mRiding->positionRider(*this);
//		}
//
//		mRotRide.y += (mRiding->mRot.y - mRiding->mRotPrev.y);
//		mRotRide.x += (mRiding->mRot.x - mRiding->mRotPrev.x);
//
//		while (mRotRide.y >= 180) {
//			mRotRide.y -= 360;
//		}
//
//		while (mRotRide.y < -180) {
//			mRotRide.y += 360;
//		}
//
//		while (mRotRide.x >= 180) {
//			mRotRide.x -= 360;
//		}
//
//		while (mRotRide.x < -180) {
//			mRotRide.x += 360;
//		}
//
//		float yra = mRotRide.y * 0.5f;
//		float xra = mRotRide.x * 0.5f;
//
//		float max = 10;
//		if (yra > max) {
//			yra = max;
//		}
//
//		if (yra < -max) {
//			yra = -max;
//		}
//
//		if (xra > max) {
//			xra = max;
//		}
//
//		if (xra < -max) {
//			xra = -max;
//		}
//
//		mRotRide.y -= yra;
//		mRotRide.x -= xra;
//	}
//}
//
//bool Entity::tick(BlockSource& tickingArea) {
//	if (!tickingArea.hasChunksAt(BlockPos(mBB.min), BlockPos(mBB.max)) && !isGlobal()) {
//		return false;
//	}
//
//	//always reassign the entity to the latest player that ticks it!
//	if (!hasCategory(EntityCategory::Player)) {
//		setRegion(tickingArea);
//	}
//
//	//must happen after setRegion, as reload uses it
//	if (mDefinitionList.hasChanged()) {
//		updateDescription();
//		reload();
//	}
//
//	if (mAdded && !getLevel().isClientSide()) {
//		_updateOwnerChunk();
//	}
//
//	if (!(mRiding && mRiding->isControlledByLocalInstance())) {
//		mPosPrev = mPos;
//		mPosPrev2 = mPos;
//		mRotPrev = mRot;
//	}
//
//	//manage the riders
//	_manageRiders(tickingArea);
//
//	mRiding ? rideTick() : normalTick();
//
//	_updateOnewayCollisions(tickingArea);
//
//	return true;
//}
//
//EntityLink::List Entity::getLinks() const {
//	EntityLink::List links;
//	//Rider IDs list is being added as a hack to easily avoid a crash caused by player being destructed before its mount saves links
//	for (auto&& i : range(mRiderIDs.size())) {
//		links.push_back({
//			i == 0 ? EntityLinkType::Riding : EntityLinkType::Passenger,
//			getUniqueID(),
//			mRiderIDs[i],
//			false
//		});
//	}
//
//	return links;
//}
//
//void Entity::_sendLinkPacket(const EntityLink& link) const {
//	if (!getLevel().isClientSide()) {
//		getLevel().getPacketSender()->send(SetEntityLinkPacket(link));
//	}
//}
//
//BlockPos Entity::_getBlockOnPos() {
//	auto box = mBB;
//	box.max.y = mBB.min.y - 0.2f;
//	box.min.y = box.max.y - 1;
//
//	//search the layer just below the feet
//	auto y = (int)Math::floor(box.max.y);
//	for (auto x : range_incl(Math::floor(box.min.x), Math::floor(box.max.x + 1))) {
//		for (auto z : range_incl(Math::floor(box.min.z), Math::floor(box.max.z + 1))) {
//			auto onBlock = getRegion().getBlockID({ x,y,z });
//			if (onBlock != BlockID::AIR) {
//				return{ x,y,z };
//			}
//		}
//	}
//
//	//search the one below too to see if there's fences
//	y = (int)Math::floor(box.min.y);
//	for (auto x : range_incl(Math::floor(box.min.x), Math::floor(box.max.x + 1))) {
//		for (auto z : range_incl(Math::floor(box.min.z), Math::floor(box.max.z + 1))) {
//			auto under = getRegion().getBlockID({ x,y,z });
//			if (Block::mFence->mID == under || under.hasProperty(BlockProperty::FenceGate) || Block::mCobblestoneWall->mID == under) {
//				return{ x,y,z };
//			}
//		}
//	}
//
//	return mPos;
//}
//
//void Entity::_manageRiders(BlockSource& tickingArea) {
//	while (mRidersToRemove.size()) {
//		Entity* rider = mRidersToRemove.back();
//		mRidersToRemove.pop_back();
//		_removeRider(*rider, false);
//	}
//
//	if (!getLevel().isClientSide()) {
//		if (!mRideableComponent) {
//			removeAllRiders();
//		}
//		else {
//			size_t size = (size_t)mCurrentDescription->mRideable->mSeatCount;
//			if (size < mRiders.size()) {
//				size_t diff = mRiders.size() - size;
//				for (size_t i = 0; i < diff; ++i) {
//					Entity* ent = mRiders[mRiders.size() - 1 - i];
//					if (ent != nullptr) {
//						_removeRider(*ent, false);
//					}
//				}
//			}
//		}
//
//		if (mRidersChanged) {
//			for (auto&& link : getLinks()) {
//				_sendLinkPacket(link);
//			}
//		}
//
//		mRidersChanged = false;
//
//		for (auto&& rider : mRiders) {
//			if (!rider->hasCategory(EntityCategory::Player) && !rider->isRemoved()) {
//				rider->tick(tickingArea);
//			}
//
//			if (mRidersChanged) {//skip the tick to avoid madness when reordering
//				break;
//			}
//		}
//	}
//}
//
//void Entity::positionRider(Entity& rider) const {
//	RideableComponent* rideable = getRideableComponent();
//	if (rideable != nullptr) {
//		rideable->positionRider(rider);
//	}
//	else if (getLevel().isClientSide()) {
//		//position riders locally
//		rider.setPreviousPosRot(rider.mPos, rider.mRot);
//
//		// Slightly ugly, but since we can't rely on components for the client
//		// and we need to move the entity locally for riding, simulate it using synched data
//		Vec3 offset = rider.getEntityData().getVec3(enum_cast(EntityDataIDs::SEAT_OFFSET));
//		SynchedEntityData::TypeInt8 lockRotation = rider.getEntityData().getInt8(enum_cast(EntityDataIDs::SEAT_LOCK_RIDER_ROTATION));
//		SynchedEntityData::TypeFloat lockRotationDegrees = rider.getEntityData().getFloat(enum_cast(EntityDataIDs::SEAT_LOCK_RIDER_ROTATION_DEGREES));
//		SynchedEntityData::TypeFloat seatRotationOffset = rider.getEntityData().getFloat(enum_cast(EntityDataIDs::SEAT_ROTATION_OFFSET));
//
//		Vec3 pos(mPos.x, mBB.min.y, mPos.z);
//		offset.yRot(-mRot.y * Math::DEGRAD);
//		rider.setPos(pos + offset);
//
//		// Do the inherit orientation stuff here.
//		if (rider.hasCategory(EntityCategory::Mob)) {
//			Mob& mobRider = static_cast<Mob&>(rider);
//
//			mobRider.setEnforceRiderRotationLimit(lockRotation != 0);
//			if (lockRotation != 0) {
//				mobRider.mYBodyRot = mRot.y + seatRotationOffset;
//				mobRider.setRiderLockedBodyRot(mobRider.mYBodyRot);
//				mobRider.setRiderRotLimit(lockRotationDegrees);
//			}
//			else {
//				mobRider.setRiderLockedBodyRot(0.0f);
//				mobRider.setRiderRotLimit(181.0f);
//			}
//		}
//	}
//}
//
//Entity* Entity::getRide() const {
//	return mRiding;
//}
//
//bool Entity::_tryPlaceAt(const Vec3& pos) {
//	Vec3 oldPos = mPos;
//
//	setPos(pos);
//	if (getRegion().fetchAABBs(mBB).empty()) {
//		return true;
//	}
//	else {
//		setPos(oldPos);
//		return false;
//	}
//}
//
//void Entity::_exitRide(const Entity& riding, float exitDist) {
//	DEBUG_ASSERT(exitDist >= 0, "Invalid exit distance");
//
//	Vec3 pos = riding.mPos;
//
//	if (exitDist > 0) {
//		//find the horizontal view dir
//		Vec3 off = getViewVector(0);
//		off.y = getHeadHeight() + 1.f;
//		off = off.normalized() * 1.6f;
//
//		Vec3 groundPos = pos + off;
//
//		//set the local rotation to the world rotation
//		setRot(getRotation());
//
//		for (auto i : range(3)) {
//			UNUSED_VARIABLE(i);
//			if (_tryPlaceAt(groundPos)) {
//				return;
//			}
//			else {
//				groundPos.y += 1;
//			}
//		}
//	}
//
//	//couldn't place, just place inside the thing
//	//TODO try other places around?
//	moveTo(pos + Vec3(0, getHeadHeight(), 0), getRotation());
//}
//
//float Entity::getRidingHeight() {
//	return mHeightOffset;
//}
//
//void Entity::addRider(Entity& r) {
//	//if the first rider is not a player push all players back and insert at the top
//	if (r.hasCategory(EntityCategory::Player) && mRiders.size() > 0 && !mRiders[0]->hasCategory(EntityCategory::Player)) {
//		mRiders.insert(mRiders.begin(), &r);
//		mRiderIDs.insert(mRiderIDs.begin(), r.getUniqueID());
//
//	}
//	else {
//		mRiders.emplace_back(&r);
//		mRiderIDs.emplace_back(r.getUniqueID());
//	}
//
//	for (auto&& rider : mRiders) {
//		positionRider(*rider);
//	}
//
//	mRidersChanged = true;
//}
//
//Entity::RiderList::iterator Entity::_findRider(Entity& rider) const {
//	//HACK dropping the const qualifier because GCC and Clang cannot
//	//erase a const_iterator and so it's useless to return one.
//	RiderList& riders = (RiderList&)(*this).mRiders;
//
//	auto itr = riders.begin();
//	auto end = riders.end();
//
//	for (; itr != end; ++itr) {
//		if (*itr == &rider) {
//			return itr;
//		}
//	}
//
//	return riders.end();
//}
//
////Rider IDs list is being added as a hack to easily avoid a crash caused by player being destructed before its mount saves links
//Entity::RiderIDList::iterator Entity::_findRiderID(Entity& rider) const {
//	//HACK dropping the const qualifier because GCC and Clang cannot
//	//erase a const_iterator and so it's useless to return one.
//	RiderIDList& riderIDs = (RiderIDList&)(*this).mRiderIDs;
//
//	auto itr = riderIDs.begin();
//	auto end = riderIDs.end();
//
//	EntityUniqueID riderID = rider.getUniqueID();
//
//	for (; itr != end; ++itr) {
//		if (*itr == riderID) {
//			return itr;
//		}
//	}
//
//	return riderIDs.end();
//}
//
//void Entity::flagRiderToRemove(Entity& r) {
//	mRidersToRemove.push_back(&r);
//}
//
//void Entity::_removeRider(Entity& r, bool entityIsBeingDestroyed) {
//	DEBUG_ASSERT(isRider(r), "Cannot remove this rider");
//
//	mRiders.erase(_findRider(r));
//	//Rider IDs list is being added as a hack to easily avoid a crash caused by player being destructed before its mount saves links
//	mRiderIDs.erase(_findRiderID(r));
//	mRidersChanged = true;
//
//	_sendLinkPacket({ EntityLinkType::None, getUniqueID(), r.getUniqueID(), entityIsBeingDestroyed });
//
//	if (getBoostableComponent() && r.hasCategory(EntityCategory::Player)) {
//		getBoostableComponent()->removeRider((Player&)r);
//	}
//}
//
//void Entity::removeAllRiders(bool beingDestroyed, bool playAnimateHurt) {
//	while (mRidersToRemove.size()) {
//		Entity* rider = mRidersToRemove.back();
//		mRidersToRemove.pop_back();
//		_removeRider(*rider, false);
//	}
//
//	auto copy = mRiders;
//
//	for (auto&& rider : copy) {
//		rider->stopRiding(false, beingDestroyed);
//	}
//
//	if (playAnimateHurt) {
//		animateHurt();
//	}
//}
//
//bool Entity::startRiding(Entity& ride) {
//	DEBUG_ASSERT(&ride != this, "Really dude?");
//
//	if (!ride.canAddRider(*this)) {
//		return false;
//	}
//
//	if (isSneaking()) {
//		return false;
//	}
//
//	mRotRide.x = 0;
//	mRotRide.y = 0;
//
//	if (isRiding()) {
//		stopRiding();
//	}
//
//	mRiding = &ride;
//	mRiding->addRider(*this);
//
//	checkBlockCollisions();
//
//	return true;
//}
//
//void Entity::stopRiding(bool exitFromRider, bool entityIsBeingDestroyed) {
//	//since this method is sometimes used as a network callback, there's 
//	//currently no guarantee that the mRiding pointer is still valid.
//	if (nullptr != mRiding) {
//		_exitRide(*mRiding, exitFromRider ? 1.6f : 0.f);
//
//		if (!entityIsBeingDestroyed) {
//			mRiding->flagRiderToRemove(*this);
//			mRidingPrevID = mRiding->getUniqueID();
//		}
//		else {
//			mRiding->_removeRider(*this, entityIsBeingDestroyed);
//			mRidingPrevID = {};
//		}
//
//		//tell the server we want to board //TODO should this be here?
//		if (!mRemoved && getLevel().isClientSide() && hasCategory(EntityCategory::Player)) {
//			//Make sure it's not another guy who is leaving the boat
//			if (((Player*)this)->isLocalPlayer()) {
//				getLevel().getPacketSender()->send(InteractPacket(InteractPacket::Action::StopRiding, mRiding->getRuntimeID()));
//			}
//		}
//	}
//	else {
//		mRidingPrevID = {};
//	}
//
//
//	mRiding = nullptr;
//}
//
//void Entity::setSeatDescription(const Vec3& offset, const SeatDescription& seat) {
//	Vec3 oldOffset = mEntityData.getVec3(enum_cast(EntityDataIDs::SEAT_OFFSET));
//
//	Vec3 delta(oldOffset - offset);
//	if (delta.lengthSquared() > FLT_EPSILON*FLT_EPSILON ) {
//		mEntityData.set<SynchedEntityData::TypeVec3>(enum_cast(EntityDataIDs::SEAT_OFFSET), offset);
//	}
//
//	mEntityData.set<SynchedEntityData::TypeInt8>(enum_cast(EntityDataIDs::SEAT_LOCK_RIDER_ROTATION), static_cast<SynchedEntityData::TypeInt8>(seat.mLockRiderRotation));
//	mEntityData.set<SynchedEntityData::TypeFloat>(enum_cast(EntityDataIDs::SEAT_LOCK_RIDER_ROTATION_DEGREES), static_cast<SynchedEntityData::TypeFloat>(seat.mLockRiderRotationDegrees));
//	mEntityData.set<SynchedEntityData::TypeFloat>(enum_cast(EntityDataIDs::SEAT_ROTATION_OFFSET), static_cast<SynchedEntityData::TypeFloat>(seat.mSeatRotation));
//}
//
//void Entity::buildDebugInfo(std::string& out) const {
//}
//
//void Entity::buildDebugGroupInfo(std::string& out) const {
//	out += mDefinitionList.definitionListToString();
//}
//
//void Entity::outOfWorld() {
//	remove();
//}
//
//void Entity::checkFallDamage(float ya, bool onGround) {
//	// double-check if we've reached water in this move tick
//	if (!isInWater()) {
//		updateWaterState();
//	}
//
//	if (onGround) {
//		if (mFallDistance > 0) {
//			BlockPos pos = _getBlockOnPos();
//			const Block& onBlock = mRegion->getBlock(pos);
//
//			// onFallOn may change mFallDistance depending on the block
//			onBlock.onFallOn(*mRegion, pos, this, mFallDistance);
//
//			if (this->hasCategory(EntityCategory::Mob)) {
//				if (!onBlock.isType(Block::mAir)) {
//					static const float minFallDistance = 3.f;
//					if (mFallDistance > minFallDistance) {
//						if (getLevel().isClientSide()) {
//							spawnDustParticles();
//						}
//						else {
//							getLevel().broadcastEntityEvent(this, EntityEvent::GROUND_DUST);
//						}
//					}
//				}
//			}
//			mFallDistance = 0;
//		}
//	}
//	else {
//		if (ya < 0) {
//			mFallDistance -= ya;
//		}
//		else {
//			mFallDistance = 0;
//		}
//	}
//}
//
//void Entity::causeFallDamage(float fallDistance) {
//	// NOTE: make sure you don't actually do anything in the causeFallDamage callstack that ends up modifying the mRiders array!
//	if (getLevel().getGameRules().getBool(GameRules::FALL_DAMAGE)) {
//		for (auto&& rider : mRiders) {
//			rider->causeFallDamage(fallDistance);
//		}
//	}
//}
//
//void Entity::handleFallDistanceOnServer(float fallDistance) {
//
//}
//
//float Entity::getHeadHeight() const {
//	return 0;
//}
//
//float Entity::getCameraOffset() const {
//	return 0;
//}
//
//void Entity::moveRelative(float xa, float za, float speed) {
//	float dist = sqrt(xa * xa + za * za);
//	if (dist < 0.01f) {
//		return;
//	}
//
//	if (dist < 1) {
//		dist = 1;
//	}
//
//	dist = speed / dist;
//	xa *= dist;
//	za *= dist;
//
//	float sin_ = (float)sin(mRot.y * Math::DEGRAD);
//	float cos_ = (float)cos(mRot.y * Math::DEGRAD);
//
//	mPosDelta.x += xa * cos_ - za * sin_;
//	mPosDelta.z += za * cos_ + xa * sin_;
//}
//
//
//void Entity::teleportTo(const Vec3& pos, int cause, int entityType) {
//	// Stop riding and teleport entity by itself.
//	// Use entityIsBeingDestroyed = true to trigger immediate dismount, otherwise riders'
//	// position snapping will still happen due to the delay caused by just queuing it.
//	if (isRiding()) {
//		stopRiding(true, true);
//	}
//
//	mFallDistance = 0.0f;  //Reset fall distance since we were just teleported.
//	mPosPrev = mPosPrev2 = pos;
//	mPosDelta.x = mPosDelta.y = mPosDelta.z = 0.0f;
//	moveTo(pos, mRot);
//}
//
//bool Entity::tryTeleportTo(const Vec3& pos, bool landOnBlock, bool avoidLiquid, int cause, int entityType) {
//	BlockPos blockPos(pos);
//	BlockSource& region = getRegion();
//
//	if (region.hasChunksAt(blockPos, 1)) {
//		// If we don't need to land on a block, don't try to move the y position down
//		// to find a block.
//		bool foundValidHeight = !landOnBlock;
//
//		// Set the mob down on a block so they aren't falling after the teleport.
//		while (!foundValidHeight && blockPos.y > 0) {
//			BlockID block = region.getBlockID(blockPos.below());
//			if (Block::mSolid[block]) {
//				foundValidHeight = true;
//			}
//			else {
//				blockPos.y--;
//			}
//		}
//
//		if (foundValidHeight) {
//			float w = mBBDim.x / 2;
//			float h = mBBDim.y;
//			AABB aabb = mBB;
//			Vec3 newPos(Vec3(pos.x, (float)blockPos.y, pos.z));
//			aabb.set(newPos.x - w, newPos.y + mSlideOffset.y, newPos.z - w, newPos.x + w, newPos.y + mSlideOffset.y + h, newPos.z + w);
//
//			// Make sure we aren't in another block somewhere, and if needed avoid liquids.
//			if (region.fetchAABBs(aabb).empty() && (!avoidLiquid || !region.containsAnyLiquid(aabb))) {
//				teleportTo(newPos, cause, entityType);
//				return true;
//			}
//		}
//	}
//
//	return false;
//}

void Entity::moveTo(const Vec3& pos, const Vec2& rot) {
	setRot(rot);
	setPos(pos + Vec3(0, mHeightOffset, 0));
}

float Entity::distanceTo(const Entity& e) const {
	float xd = (mPos.x - e.mPos.x);
	float yd = (mPos.y - e.mPos.y);
	float zd = (mPos.z - e.mPos.z);
	return sqrt(xd * xd + yd * yd + zd * zd);
}

float Entity::distanceTo(const Vec3& pos) const {
	float xd = (mPos.x - pos.x);
	float yd = (mPos.y - pos.y);
	float zd = (mPos.z - pos.z);
	return sqrt(xd * xd + yd * yd + zd * zd);
}

float Entity::distanceToSqr(const Vec3& pos) const {
	float xd = (mPos.x - pos.x);
	float yd = (mPos.y - pos.y);
	float zd = (mPos.z - pos.z);
	return xd * xd + yd * yd + zd * zd;
}

float Entity::distanceToSqr(const Entity& e) const {
	float xd = mPos.x - e.mPos.x;
	float yd = mPos.y - e.mPos.y;
	float zd = mPos.z - e.mPos.z;
	return xd * xd + yd * yd + zd * zd;
}

float Entity::distanceSqrToBlockPosCenter(const BlockPos& pos) const {
	return pos.distSqrToCenter(mPos.x, mPos.y, mPos.z);
}

void Entity::playerTouch(Player& player) {
}

//void Entity::push(Entity& e, bool pushSelfOnly) {
//	Entity* ridingPrev = getLevel().fetchEntity(mRidingPrevID);
//	Entity* eRidingPrev = e.getLevel().fetchEntity(e.mRidingPrevID);
//	if (mRiding || e.mRiding || ridingPrev == &e || eRidingPrev == this) {
//		return;
//	}
//
//	float xa = e.mPos.x - mPos.x;
//	float za = e.mPos.z - mPos.z;
//
//	float dd = Math::absMax(xa, za);
//
//	if (dd >= 0.01f) {
//		dd = sqrt(dd);
//		xa /= dd;
//		za /= dd;
//
//		float pow = 1 / dd;
//		if (pow > 1) {
//			pow = 1;
//		}
//
//		xa *= pow;
//		za *= pow;
//
//		xa *= 0.05f;
//		za *= 0.05f;
//
//		xa *= 1 - mPushthrough;
//		za *= 1 - mPushthrough;
//
//		this->push(Vec3(-xa, 0, -za));
//
//		if (!pushSelfOnly) {
//			e.push(Vec3(xa, 0, za));
//		}
//	}
//}
//
//void Entity::push(const Vec3 &vec) {
//	mPosDelta += vec;
//}
//
//void Entity::markHurt() {
//	mHurtMarked = true;
//}
//
//bool Entity::hurt(const EntityDamageSource& source, int damage, bool knock /*= true*/, bool ignite /*= false*/) {
//	if (isInvulnerableTo(source)) {
//		return false;
//	}
//
//	return _hurt(source, damage, knock, ignite);
//}
//
//bool Entity::_hurt(const EntityDamageSource& source, int damage, bool knock, bool ignite) {
//	markHurt();
//	if (mCurrentDescription && mCurrentDescription->mOnHurt) {
//		VariantParameterList params;
//		initParams(params);
//		mCurrentDescription->executeTrigger(*this, *mCurrentDescription->mOnHurt, params);
//	}
//	if (getProjectileComponent() != nullptr) {
//		getProjectileComponent()->hurt(source);
//	}
//	return false;
//}
//
//bool Entity::isInvulnerableTo(const EntityDamageSource& source) const {
//	Player* player = nullptr;
//	if (source.isEntitySource() && source.getEntity()->hasCategory(EntityCategory::Player)) {
//		player = static_cast<Player*>(source.getEntity());
//	}
//
//	return mInvulnerable && source.getCause() != EntityDamageCause::Void && (player == nullptr || !player->isCreative());
//}
//
//bool Entity::doFireHurt(int amount) {
//	if (mCurrentDescription && mCurrentDescription->mOnIgnite) {
//		VariantParameterList params;
//		initParams(params);
//		mCurrentDescription->executeTrigger(*this, *mCurrentDescription->mOnIgnite, params);
//	}
//
//	// when entity is "on fire", but not necessarily IN fire
//	EntityDamageSource source = EntityDamageSource(EntityDamageCause::FireTick);
//	return hurt(source, amount);
//}
//
//void Entity::onLightningHit() {
//	burn(5, true);
//	if (mOnFire == 0) {
//		setOnFire(8);
//	}
//}
//
//void Entity::spawnTamingParticles(bool wasSuccess) {
//	ParticleType particle = ParticleType::Heart;
//	if (!wasSuccess) {
//		particle = ParticleType::Smoke;
//	}
//
//	for (int i = 0; i < 7; i++) {
//		Vec3 dir = mRandom.nextGaussianVec3() * 0.02f;
//		getLevel().addParticle(particle, _randomHeartPos(), dir);
//	}
//}
//
//void Entity::onTame() {
//}
//
//void Entity::onFailedTame() {
//}
//
//bool Entity::isInLove() const {
//	return getStatusFlag(EntityFlags::INLOVE);
//}
//
//bool Entity::canMate(const Entity& entity) const {
//	if (mBreedableComponent) {
//		return mBreedableComponent->canMate(entity);
//	}
//	return false;
//}
//
//void Entity::onMate(Mob& partner) {
//}
//
//bool Entity::isAngry() const {
//	return getStatusFlag(EntityFlags::ANGRY);
//}
//
//bool Entity::isSheared() const {
//	return getStatusFlag(EntityFlags::SHEARED);
//}
//
//bool Entity::isChested() const {
//	return getStatusFlag(EntityFlags::CHESTED);
//}
//
//bool Entity::isIgnited() const {
//	return getStatusFlag(EntityFlags::IGNITED);
//}
//
//bool Entity::hasSaddle() const {
//	return getStatusFlag(EntityFlags::SADDLED);
//}
//
//bool Entity::isCharged() const {
//	return getStatusFlag(EntityFlags::CHARGED);
//}
//
//void Entity::setCharged(bool value) {
//	setStatusFlag(EntityFlags::CHARGED, value);
//}
//
//bool Entity::isPowered() const {
//	return getStatusFlag(EntityFlags::POWERED);
//}
//
//void Entity::setPowered(bool value) {
//	setStatusFlag(EntityFlags::POWERED, value);
//}
//
//void Entity::setSaddle(bool saddled) {
//	setStatusFlag(EntityFlags::SADDLED, saddled);
//}
//
//Vec3 Entity::_randomHeartPos() {
//	float posx = mPos.x + mRandom.nextFloat() * mBBDim.x * 2 - mBBDim.x;
//	float posy = mPos.y + .5f + mRandom.nextFloat() * mBBDim.y;
//	float posz = mPos.z + mRandom.nextFloat() * mBBDim.x * 2 - mBBDim.x;
//	return Vec3(posx, posy, posz);
//}
//
//void Entity::handleEntityEvent(EntityEvent eventId, int data) {
//	switch(eventId) {
//		case EntityEvent::TAMING_SUCCEEDED:
//		case EntityEvent::TAMING_FAILED:
//			spawnTamingParticles(eventId == EntityEvent::TAMING_SUCCEEDED);
//			break;
//		case EntityEvent::LOVE_HEARTS:
//			for (int i = 0; i < 3; ++i) {
//				Vec3 dir = mRandom.nextGaussianVec3() * 0.02f;
//				getLevel().addParticle(ParticleType::Heart, _randomHeartPos(), dir);
//			}
//			break;
//		case EntityEvent::BABY_EAT:
//			for (int i = 0; i < 7; ++i) {
//				Vec3 dir = mRandom.nextGaussianVec3() * 0.02f;
//				getLevel().addParticle(ParticleType::VillagerHappy, _randomHeartPos(), dir);
//			}
//			break;
//		case EntityEvent::INSTANT_DEATH:
//			spawnDeathParticles();
//			playSound(LevelSoundEvent::Death, getAttachPos(EntityLocation::Head));
//			break;
//		case EntityEvent::FEED:
//			spawnEatParticles(ItemInstance(data, 1, 0), 10);
//			break;
//		case EntityEvent::GROUND_DUST:
//			spawnDustParticles();
//			break;
//
//		default:
//			break;
//	}
//}
//
//void Entity::feed(int itemId) {
//}
//
//void Entity::spawnEatParticles(const ItemInstance& itemInUse, int count) {
//	if (itemInUse.getUseAnimation() == UseAnimation::Drink) {
//		playSound(LevelSoundEvent::Drink, getAttachPos(EntityLocation::Head));
//	}
//	else {
//		for (int i = 0; i < count; i++) {
//			const float xx = -mRot.x * Math::DEGRAD;
//			const float yy = -mRot.y * Math::DEGRAD;
//			Vec3 d((mRandom.nextFloat() - 0.5f) * 0.1f, Math::random() * 0.1f + 0.1f, 0);
//			d.xRot(xx);
//			d.yRot(yy);
//
//			const float posx = (mRandom.nextFloat() - 0.5f) * 0.3f;
//			const float posy = -mRandom.nextFloat() * 0.6f - 0.3f;
//			Vec3 pos(posx, posy, 0.6f);
//			pos.xRot(xx);
//			pos.yRot(yy);
//			pos = pos.add(mPos.x, mPos.y + getHeadHeight(), mPos.z);
//
//			Vec3 dir(d.x, d.y + 0.05f, d.z);
//
//			getLevel().addParticle(ParticleType::IconCrack, pos, dir, itemInUse.getIdAux());
//		}
//		playSound(LevelSoundEvent::Eat, getAttachPos(EntityLocation::Head));
//	}
//}
//
//void Entity::spawnDeathParticles() {
//	for (auto i : range(20)) {
//		UNUSED_VARIABLE(i);
//		float xa = mRandom.nextFloat() * 0.02f;
//		float ya = mRandom.nextFloat() * 0.02f;
//		float za = mRandom.nextFloat() * 0.02f;
//
//		float posx = mPos.x + mRandom.nextFloat() * mBBDim.x * 2 - mBBDim.x;
//		float posy = mPos.y + mRandom.nextFloat() * mBBDim.y - mHeightOffset;
//		float posz = mPos.z + mRandom.nextFloat() * mBBDim.x * 2 - mBBDim.x;
//
//		getLevel().addParticle(ParticleType::Explode, Vec3(posx, posy, posz), Vec3(xa, ya, za));
//	}
//}
//
//void Entity::spawnDustParticles() {
//	static const float minFallDistance = 3.0f;
//	int power = Math::ceil(mFallDistance - minFallDistance);
//	float scale = std::min(0.2f + power / 15.f, 2.5f);
//
//	static const unsigned particleCount = static_cast<unsigned>(150.0 * scale);
//	static const float horizontalParticleRange = 0.1f;
//	static const float verticalParticleRange = 0.1f;
//
//	FullBlock fullBlock = mRegion->getBlockAndData(_getBlockOnPos());
//	for (unsigned i = 0; i < particleCount; ++i) {
//		float dirx = getLevel().getRandom().nextFloat(-horizontalParticleRange, horizontalParticleRange);
//		float diry = getLevel().getRandom().nextFloat(0.f, verticalParticleRange);
//		float dirz = getLevel().getRandom().nextFloat(-horizontalParticleRange, horizontalParticleRange);
//
//		getLevel().addParticle(ParticleType::Terrain, Vec3(mPos.x, mBB.min.y, mPos.z), Vec3(dirx, diry, dirz), fullBlock.toInt());
//	}
//}
//
//void Entity::reset() {
//	mInitialized = false;
//
//	if (!getLevel().isClientSide()) {
//		Entity::initializeComponents(mInitMethod, mInitParams);
//	}
//
//	mInitialized = true;
//}

BlockSource& Entity::getRegion() const {
	return *mRegion;
}

//Dimension& Entity::getDimension() const {
//	return mRegion->getDimension(); //TODO we should not rely on the region to get the dimension, region is just a reader!!
//}
//
//bool Entity::isRegionValid() const {
//	return (mRegion != nullptr);
//}
//
//void Entity::resetRegion() {
//	mRegion = nullptr;
//}
//
//bool Entity::intersects(const Vec3 &pos1, const Vec3 &pos2) {
//	return mBB.intersects(AABB(pos1.x, pos1.y, pos1.z, pos2.x, pos2.y, pos2.z));
//}

bool Entity::isPickable() {
	return false;
}


bool Entity::isFishable() const {
	return true;
}

bool Entity::isPushable() const {
	return false;
}

bool Entity::isPushableByPiston() const {
	return isPushable();
}

bool Entity::isShootable() {
	return false;
}

//void Entity::awardKillScore(Entity& victim, int score) {
//}
//
//bool Entity::shouldRender() const {
//	return !isInvisible();
//}

bool Entity::isCreativeModeAllowed() {
	return false;
}

bool Entity::isSurfaceMob() const {
	return true;
}

//float Entity::getShadowHeightOffs() {
//	return mBBDim.y / 2.f;
//}
//
//float Entity::getShadowRadius() const {
//	return mBBDim.x;
//}

bool Entity::isAlive() const {
	return !mRemoved;
}

//void Entity::lerpTo(const Vec3& pos, const Vec2& rot, int steps) {
//	setPos(pos);
//	setRot(rot);
//}
//
//float Entity::getPickRadius() {
//	return 0.1f;
//}
//
//void Entity::lerpMotion(const Vec3& delta) {
//	mPosDelta = delta;
//}
//
//void Entity::animateHurt() {
//}
//
//void Entity::setEquippedSlot(ArmorSlot slot, int item, int auxValue) {
//}
//
//void Entity::setEquippedSlot(ArmorSlot slot, const ItemInstance& item) {
//}

bool Entity::isSneaking() const {
	return false;
}

//void Entity::lavaHurt() {
//	if (!mFireImmune) {
//		EntityDamageSource source = EntityDamageSource(EntityDamageCause::Lava);
//		hurt(source, 4);
//	}
//}
//
//void Entity::burn(int dmg, bool bInFire) {
//	if (!mFireImmune) {
//		// on fire damage by default
//		EntityDamageSource source(EntityDamageCause::FireTick);
//		if (bInFire) {
//			source = EntityDamageSource(EntityDamageCause::Fire);
//		}
//
//		hurt(source, dmg);
//	}
//}
//
//bool Entity::save(CompoundTag& entityTag) {
//	EntityType id = getEntityTypeId();
//	if (isRemoved() || id == EntityType::Undefined) {
//		return false;
//	}
//
//	entityTag.putInt("id", (unsigned int)id);
//
//	auto listTag = make_unique<ListTag>();
//	auto defStack = mDefinitionList.getDefinitionStack();
//	for (auto& pair : defStack) {
//		listTag->add(make_unique<StringTag>("", (pair.first ? "+" : "-") + pair.second->mIdentifier));
//	}
//	entityTag.put("definitions", std::move(listTag));
//
//	saveWithoutId(entityTag);
//
//	if (mContainerComponent != nullptr) {
//		mContainerComponent->addAdditionalSaveData(entityTag);
//	}
//
//	if (mExplodeComponent != nullptr) {
//		mExplodeComponent->addAdditionalSaveData(entityTag);
//	}
//
//	if (mAgeableComponent) {
//		mAgeableComponent->addAdditionalSaveData(entityTag);
//	}
//	
//	if (mBreedableComponent) {
//		mBreedableComponent->addAdditionalSaveData(entityTag);
//	}
//
//	if (mBreathableComponent) {
//		mBreathableComponent->addAdditionalSaveData(entityTag);
//	}
//
//	if (mProjectileComponent) {
//		mProjectileComponent->addAdditionalSaveData(entityTag);
//	}
//
//	if (mMountTamingComponent) {
//		mMountTamingComponent->addAdditionalSaveData(entityTag);
//	}
//
//	if (mTimerComponent) {
//		mTimerComponent->addAdditionalSaveData(entityTag);
//	}
//
//	if (mNpcComponent != nullptr) {
//		mNpcComponent->addAdditionalSaveData(entityTag);
//	}
//	return true;
//}
//
//Unique<ListTag> Entity::saveLinks() const {
//	auto linksTag = make_unique<ListTag>();
//
//	for (auto i : range(mRiderIDs.size())) {
//		auto link = make_unique<CompoundTag>();
//		link->putInt64("entityID", (int64_t)mRiderIDs[i]);
//		link->putInt("linkID", i);
//		//TODO link kind?
//
//		linksTag->add(std::move(link));
//	}
//
//	return linksTag;
//}
//
//void Entity::saveWithoutId(CompoundTag& entityTag) {
//	entityTag.putInt64("UniqueID", (int64_t)mUniqueID);
//
//	entityTag.put("Pos", ListTagFloatAdder(mPos.x)(mPos.y)(mPos.z).done());
//	entityTag.put("Motion", ListTagFloatAdder(mPosDelta.x)(mPosDelta.y)(mPosDelta.z).done());
//	entityTag.put("Rotation", ListTagFloatAdder(mRot.y)(mRot.x).done());
//
//	if (!hasCategory(EntityCategory::Player)) {
//		if (getNameTag() != "") {
//			entityTag.putString("CustomName", getNameTag());
//			entityTag.putBoolean("CustomNameVisible", canShowNameTag());
//		}
//
//		entityTag.putInt("LastDimensionId", (int)mDimensionId);
//	}
//
//	entityTag.putFloat("FallDistance", mFallDistance);
//	entityTag.putShort("Fire", (short)mOnFire);
//	entityTag.putBoolean("OnGround", mOnGround);
//	entityTag.putBoolean("Invulnerable", mInvulnerable);
//	entityTag.putInt("PortalCooldown", mPortalCooldown);
//	entityTag.putBoolean("IsGlobal", mGlobal);
//	entityTag.putBoolean("IsAutonomous", mAutonomous);
//
//	//write down the links that this entity has towards others (riding)
//	if (isRide()) {
//		entityTag.put("LinksTag", saveLinks());
//	}
//
//	saveEntityFlags(entityTag);
//	addAdditionalSaveData(entityTag);
//}
//
//void Entity::saveEntityFlags(CompoundTag& entityTag) {
//	entityTag.putBoolean("LootDropped", mLootDropped);
//
//	entityTag.putByte("Color", (char)getColor());
//
//	entityTag.putBoolean("Sheared", getStatusFlag(EntityFlags::SHEARED));
//
//	entityTag.putInt64("OwnerNew", static_cast<int64_t>(getOwnerId()));
//
//	// Couldn't all of these be if (boolean_check) { entityTag.putBoolean(FlagName), true }  That would reduce save size
//	entityTag.putBoolean("Sitting", isSitting());
//
//	entityTag.putBoolean("IsBaby", isBaby());
//
//	entityTag.putBoolean("IsTamed", isTame());
//
//	entityTag.putBoolean("IsAngry", getStatusFlag(EntityFlags::ANGRY));
//
//	entityTag.putInt("Variant", getVariant());
//
//	entityTag.putInt("MarkVariant", getMarkVariant());
//
//	entityTag.putBoolean("Saddled", getStatusFlag(EntityFlags::SADDLED));
//		
//	entityTag.putBoolean("Chested", getStatusFlag(EntityFlags::CHESTED));
//
//	entityTag.putBoolean("ShowBottom", getStatusFlag(EntityFlags::SHOW_BOTTOM));
//
//	entityTag.putBoolean("IsGliding", getStatusFlag(EntityFlags::GLIDING));
//}
//
//void Entity::loadEntityFlags(const CompoundTag& tag) {
//	mLootDropped = tag.getBoolean("LootDropped");
//
//	setColor(Palette::fromByte(tag.getByte("Color")));
//
//	setStatusFlag(EntityFlags::SHEARED, tag.getBoolean("Sheared"));
//	
//	if (tag.contains("Sitting")) {
//		setSitting(tag.getBoolean("Sitting"));
//	}
//
//	setStatusFlag(EntityFlags::BABY, tag.getBoolean("IsBaby"));
//
//	setStatusFlag(EntityFlags::TAMED, tag.getBoolean("IsTamed"));
//
//	setStatusFlag(EntityFlags::ANGRY, tag.getBoolean("IsAngry"));
//
//	setStatusFlag(EntityFlags::SADDLED, tag.getBoolean("Saddled"));
//
//	setStatusFlag(EntityFlags::CHESTED, tag.getBoolean("Chested"));
//
//	setStatusFlag(EntityFlags::SHOW_BOTTOM, tag.getBoolean("ShowBottom"));
//
//	setStatusFlag(EntityFlags::GLIDING, tag.getBoolean("IsGliding"));
//
//	if (tag.contains("Owner")) {
//		setOwner(EntityUniqueID::fromClientId((uint32_t)tag.getInt("Owner")));
//	}
//	else {
//		setOwner((EntityUniqueID)tag.getInt64("OwnerNew"));
//	}
//
//	setVariant(tag.getInt("Variant"));
//
//	setMarkVariant(tag.getInt("MarkVariant"));
//
//}
//
//void Entity::_convertOldSave(const CompoundTag& tag) {
//	EntityLegacySaveConverter::convertSave(*this, tag);
//	updateDescription();
//}
//
//bool Entity::load(const CompoundTag& tag) {
//	if (tag.contains("UniqueID") && !hasUniqueID()) {
//		mUniqueID = (EntityUniqueID)tag.getInt64("UniqueID");
//	}
//
//	loadEntityFlags(tag);
//
//	auto listTag = tag.getList("definitions");
//	if (!listTag && !tag.isEmpty()) {
//		_convertOldSave(tag);
//	}
//	else if (listTag) {
//		int size = listTag->size();
//		if (size > 0) {
//			Tag *ltag;
//			ltag = listTag->get(0);
//			if (ltag->getId() == Tag::Type::String) {
//				//should be a string, but who knows 
//				//how players will mess with world files
//				setBaseDefinition(((StringTag*)ltag)->data.substr(1));
//			}
//			for (int i = 1; i < size; ++i) {
//				ltag = listTag->get(i);
//				if (ltag->getId() != Tag::Type::String) {
//					continue;
//				}
//				StringTag *stag = (StringTag*)ltag;
//
//				if (stag->data.empty()) {
//					continue;
//				}
//				if (stag->data[0] == '+') {
//					mDefinitionList.addDefinition(stag->data.substr(1));
//				}
//				else {
//					mDefinitionList.removeDefinition(stag->data.substr(1));
//				}
//			}
//			updateDescription();
//			initializeComponents(mInitMethod, mInitParams);
//			mDefinitionList.clearChangedDescription();
//		}
//	}
//	
//	auto pos = tag.getList("Pos");
//	auto motion = tag.getList("Motion");
//	auto rotation = tag.getList("Rotation");
//
//	static const ListTag emptyDummy;
//	if (!pos) {
//		pos = &emptyDummy;
//	}
//	if (!motion) {
//		motion = &emptyDummy;
//	}
//	if (!rotation) {
//		rotation = &emptyDummy;
//	}
//
//	if (!hasCategory(EntityCategory::Player)) {
//		if (tag.contains("CustomName")) {
//			setNameTag(tag.getString("CustomName"));
//			setNameTagVisible(tag.contains("CustomNameVisible") ? tag.getBoolean("CustomNameVisible") : true);
//		}
//
//		if (tag.contains("LastDimensionId")) {
//			int val = tag.getInt("LastDimensionId");
//			if (val >= 0 && val < DimensionId::Count) {
//				mDimensionId = static_cast<DimensionId>(val);
//			}
//		}
//	}
//
//	mPosDelta.x = motion->getFloat(0);
//	mPosDelta.y = motion->getFloat(1);
//	mPosDelta.z = motion->getFloat(2);
//
//	if (std::abs(mPosDelta.x) > 10.0) {
//		mPosDelta.x = 0;
//	}
//
//	if (std::abs(mPosDelta.y) > 10.0) {
//		mPosDelta.y = 0;
//	}
//
//	if (std::abs(mPosDelta.z) > 10.0) {
//		mPosDelta.z = 0;
//	}
//
//	mPos.x = pos->getFloat(0);
//	mPos.y = Math::clamp(pos->getFloat(1), -5.f, LEVEL_HEIGHT_DEPRECATED + 10.f);	//NOTE 0.9.4- had a bug where the player's position could be saved with a huge y
//	mPos.z = pos->getFloat(2);
//	mPosPrev = mPosPrev2 = mPos;
//
//	mRotPrev.y = mRot.y = fmod(rotation->getFloat(0), 360.0f);
//	mRotPrev.x = mRot.x = fmod(rotation->getFloat(1), 360.0f);
//
//	mFallDistance = tag.getFloat("FallDistance");
//	mOnFire = tag.getShort("Fire");
//	mOnGround = tag.getBoolean("OnGround");
//	mInvulnerable = tag.getBoolean("Invulnerable");
//	mPortalCooldown = tag.getInt("PortalCooldown");
//	mGlobal = tag.getBoolean("IsGlobal");
//	mAutonomous = tag.getBoolean("IsAutonomous");
//
//	setPos(mPos);
//
//	readAdditionalSaveData(tag);
//
//	_tryLoadComponent(this, mCurrentDescription->mContainer, mContainerComponent, tag);
//	_tryLoadComponent(this, mCurrentDescription->mExplodable, mExplodeComponent, tag);
//	_tryLoadComponent(this, mCurrentDescription->mAgeable, mAgeableComponent, tag);
//	_tryLoadComponent(this, mCurrentDescription->mProjectile, mProjectileComponent, tag);
//	_tryLoadComponent(this, mCurrentDescription->mMountTameable, mMountTamingComponent, tag);
//	_tryLoadComponent(this, mCurrentDescription->mTimer, mTimerComponent, tag);
//	_tryLoadComponent(this, mCurrentDescription->mNpc, mNpcComponent, tag);
//
//	return true;
//}
//
//void Entity::loadLinks(const CompoundTag& entityTag, EntityLink::List& links) {
//	if (auto linksTag = (ListTag*)entityTag.get("LinksTag")) {
//		for (auto i : range(linksTag->size())) {
//			auto linkTag = (CompoundTag*)linksTag->get(i);
//
//			links.emplace_back(EntityLink{
//				linkTag->getInt("linkID") == 0 ? EntityLinkType::Riding : EntityLinkType::Passenger,
//				getUniqueID(),
//				(EntityUniqueID)linkTag->getInt64("entityID"),
//				false
//			});
//		}
//	}
//}
//
//
//void Entity::setChanged() {
//	mChanged = true;
//}
//
//const SynchedEntityData& Entity::getEntityData() const {
//	return mEntityData;
//}
//
//SynchedEntityData& Entity::getEntityData() {
//	return mEntityData;
//}
//
//bool Entity::acceptClientsideEntityData(Player&, const SetEntityDataPacket&) {
//	return false;
//}
//
//EntityRendererId Entity::queryEntityRenderer() {
//	return ER_DEFAULT_RENDERER;
//}
//
//ItemEntity* Entity::spawnAtLocation(int resource, int count) {
//	return spawnAtLocation(resource, count, 0);
//}
//
//ItemEntity* Entity::spawnAtLocation(int resource, int count, float yOffs) {
//	return spawnAtLocation(ItemInstance(resource, count, 0), yOffs);
//}
//
//ItemEntity* Entity::spawnAtLocation(FullBlock block, int count) {
//	return spawnAtLocation(block, count, 0);
//}
//
//ItemEntity* Entity::spawnAtLocation(FullBlock block, int count, float yOffs) {
//	return spawnAtLocation(ItemInstance(block.id, count, block.data), yOffs);
//}
//
//ItemEntity* Entity::spawnAtLocation(const ItemInstance& itemInstance, float yOffs) {
//
//	ItemEntity *ie = getLevel().getSpawner().spawnItem(getRegion(), itemInstance, this, mPos + Vec3(0, yOffs, 0), 10);
//	ie->setDefaultPickUpDelay();
//	return ie;
//}
//
//bool Entity::getStatusFlag(EntityFlags flag) const {
//	return mEntityData.getFlag<SynchedEntityData::TypeInt64>(enum_cast(EntityDataIDs::FLAGS), enum_cast(flag));
//}
//
//void Entity::setStatusFlag(EntityFlags flag, bool value) {
//	if (value) {
//		mEntityData.setFlag<SynchedEntityData::TypeInt64>(enum_cast(EntityDataIDs::FLAGS), enum_cast(flag));
//	}
//	else {
//		mEntityData.clearFlag<SynchedEntityData::TypeInt64>(enum_cast(EntityDataIDs::FLAGS), enum_cast(flag));
//	}
//}
//
//bool Entity::isDebugging() const {
//#ifdef ENABLE_DEBUG_RENDERING
//	if (DebugRenderer::getDebugEntity() == this)
//		return true; // <- can set a breakpoint here.
//	return false;
//#else
//	return false;
//#endif
//}

bool Entity::isOnFire() const {
// 	return mOnFire > 0 || getStatusFlag(EntityFlags::ONFIRE);
	return mOnFire > 0;
}

//bool Entity::interactPreventDefault() {
//	return false;
//}
//
//bool Entity::canExistInPeaceful() const {
//	return true;
//}
//
//bool Entity::isInvisible() const {
//	return getStatusFlag(EntityFlags::INVISIBLE);
//}
//
//bool Entity::canShowNameTag() {
//	return getStatusFlag(EntityFlags::CAN_SHOW_NAME);
//}
//
//void Entity::setNameTagVisible(bool visible) {
//	return setStatusFlag(EntityFlags::CAN_SHOW_NAME, visible);
//}
//
//const std::string& Entity::getNameTag() const {
//	return mEntityData.getString(enum_cast(EntityDataIDs::NAME));
//}
//
//std::string Entity::getFormattedNameTag() const {
//	return getNameTag();
//}
//
//void Entity::setNameTag(const std::string& name) {
//	mEntityData.set(enum_cast(EntityDataIDs::NAME), name);
//	if (mContainerComponent != nullptr) {
//		mContainerComponent->mContainer->setCustomName(name);
//	}
//}

bool Entity::isImmobile() const {
	//return mImmobile || getStatusFlag(EntityFlags::NOAI);
	return mImmobile;
}

bool Entity::isSilent() {
	//return getStatusFlag(EntityFlags::SILENT);
	return true;
}

//EntityUniqueID Entity::getSourceUniqueID() const {
//	return{};
//}
//
//EntityType Entity::getOwnerEntityType() {
//	Entity* entity = getLevel().fetchEntity(getSourceUniqueID());
//	if (entity) {
//		return entity->getEntityTypeId();
//	}
//
//	// no owner entity
//	return EntityType::Undefined;
//}
//
//void Entity::_playStepSound(const BlockPos& pos, int onBlockId) {
//	if (Block::mBlocks[onBlockId]->getMaterial().isLiquid()) {
//		return;
//	}
//
//	if (mRegion->getBlockID(pos.above()) == Block::mTopSnow->mID) {
//		onBlockId = Block::mTopSnow->getId();
//	}
//	playSound(LevelSoundEvent::Step, getAttachPos(EntityLocation::Feet), onBlockId);
//}
//
//void Entity::playSound(LevelSoundEvent type, Vec3 const& pos, int blockID) {
//	if (!isSilent()) {
//		getLevel().playSound(getRegion(), type, pos, blockID, getEntityTypeId(), isBaby());
//	}
//}
//
//void Entity::playSynchronizedSound(LevelSoundEvent type, Vec3 const& pos, int blockID, bool isGlobal){
//	if (!isSilent()) {
//		getLevel().broadcastSoundEvent(getRegion(), type, pos, blockID, getEntityTypeId(), isBaby(), isGlobal);
//	}
//}
//
//void Entity::onSynchedDataUpdate(int dataId) {	
//	if (dataId == enum_cast(EntityDataIDs::CONTAINER_TYPE) || dataId == enum_cast(EntityDataIDs::CONTAINER_SIZE)) {
//		ContainerType type = (ContainerType)mEntityData.getInt8(enum_cast(EntityDataIDs::CONTAINER_TYPE));
//		int size = mEntityData.getInt(enum_cast(EntityDataIDs::CONTAINER_SIZE));		
//		if (type == ContainerType::NONE || size <= 0) {
//			mContainerComponent.reset();
//		}
//		else {
//			//build the container component
//			mContainerComponent = make_unique<ContainerComponent>(*this);
//			//TODO adors - testing for now, but add linked slots?
//			mContainerComponent->rebuildContainer(type, size, 0, false);
//
//		}
//	}
//	else if (dataId == enum_cast(EntityDataIDs::WIDTH) || dataId == enum_cast(EntityDataIDs::HEIGHT) || dataId == enum_cast(EntityDataIDs::SCALE)) {
//		float width = mEntityData.getFloat(enum_cast(EntityDataIDs::WIDTH));
//		float height = mEntityData.getFloat(enum_cast(EntityDataIDs::HEIGHT));
//		setSize(width, height);
//	}
//}
//
//Vec3 Entity::getRandomPointInAABB(Random& random) {
//	//find a random point in the aabb
//	return Vec3::lerpComponent(Vec3(mBB.min.x, mBB.min.y, mBB.min.z), Vec3(mBB.max.x, mBB.max.y, mBB.max.z), random.nextVec3());
//}
//
//void Entity::checkBlockCollisions() {
//	checkBlockCollisions(mBB);
//
//	for (auto& rider : mRiders) {
//		checkBlockCollisions(rider->mBB);
//	}
//}
//
//bool Entity::isInsideBorderBlock(float grow) const {
//
//	AABB floored = mBB.grow(Vec3(grow));
//
//	int x0 = Math::floor(floored.min.x + 0.001f);
//	int y0 = Math::floor(floored.min.y + 0.001f);
//	int z0 = Math::floor(floored.min.z + 0.001f);
//	int x1 = Math::floor(floored.max.x - 0.001f);
//	int y1 = Math::floor(floored.max.y - 0.001f);
//	int z1 = Math::floor(floored.max.z - 0.001f);
//
//	if (mRegion->hasChunksAt(AABB(BlockPos(x0, y0, z0), BlockPos(x1, y1, z1)))) {
//		for (int x = x0; x <= x1; x++) {
//			for (int z = z0; z <= z1; z++) {
//				if (mRegion->hasBorderBlock(BlockPos(x, 0, z))) {
//					return true;
//				}
//			}
//		}
//	}
//	return false;
//}
//
//void Entity::checkBlockCollisions(const AABB& aabb) {
//
//	mInsideBlockId = BlockID::AIR;
//
//	int x0 = Math::floor(aabb.min.x + 0.001f);
//	int y0 = Math::floor(aabb.min.y + 0.001f);
//	int z0 = Math::floor(aabb.min.z + 0.001f);
//	int x1 = Math::floor(aabb.max.x - 0.001f);
//	int y1 = Math::floor(aabb.max.y - 0.001f);
//	int z1 = Math::floor(aabb.max.z - 0.001f);
//
//	if (mRegion->hasChunksAt(AABB(BlockPos(x0, y0, z0), BlockPos(x1, y1, z1)))) {
//		for (int x = x0; x <= x1; x++) {
//			for (int y = y0; y <= y1; y++) {
//				for (int z = z0; z <= z1; z++) {
//					auto& b = mRegion->getBlock(BlockPos(x, y, z));
//					b.entityInside(*mRegion, BlockPos(x, y, z), *this);
//				}
//			}
//		}
//	}
//}
//
//void Entity::setOnFire(int seconds) {
//	if (mLevel == nullptr || !isInWaterOrRain()) {
//		mOnFire = std::max(mOnFire, seconds * SharedConstants::TicksPerSecond);
//	}
//}
//
//bool Entity::isRide() const {
//	return mRiders.size() > 0;
//}
//
//bool Entity::canAddRider(Entity& rider) const {
//	return !isRide() && !isRider(rider);//if empty and the entity doesn't already ride this
//}
//
//bool Entity::isRider(Entity& rider) const {
//	return _findRider(rider) != mRiders.end();
//}
//
//bool Entity::canBePulledIntoVehicle() const {
//	return false;
//}
//
//bool Entity::isRiding() const {
//	return mRiding != nullptr;
//}
//
//bool Entity::isLeashed() {
//	return getStatusFlag(EntityFlags::LEASHED);
//}
//
//EntityUniqueID Entity::getLeashHolder() {
//	return EntityUniqueID(mEntityData.getInt64(enum_cast(EntityDataIDs::LEASH_HOLDER)));
//}
//
//void Entity::setLeashHolder(EntityUniqueID leashHolder) {
//	setStatusFlag(EntityFlags::LEASHED, true);
//	mEntityData.set<SynchedEntityData::TypeInt64>(enum_cast(EntityDataIDs::LEASH_HOLDER), (SynchedEntityData::TypeInt64)leashHolder);
//
//	mEntityData.markDirty(enum_cast(EntityDataIDs::LEASH_HOLDER));
//}
//
//void Entity::dropLeash(bool createItemDrop) {
//	if (isLeashed()) {
//		Entity* e = getLevel().fetchEntity(getLeashHolder());
//		if (e && e->getEntityTypeId() == EntityType::LeashKnot) {
//			LeashFenceKnotEntity* le = static_cast<LeashFenceKnotEntity*>(e);
//			if (le->numberofAnimalsAttached() <= 1) {
//				le->remove();
//				return;
//			}
//		}
//		setStatusFlag(EntityFlags::LEASHED, false);
//		mEntityData.set<SynchedEntityData::TypeInt64>(enum_cast(EntityDataIDs::LEASH_HOLDER), 0);
//
//		mEntityData.markDirty(enum_cast(EntityDataIDs::LEASH_HOLDER));
//		
//		if (!getLevel().isClientSide() && createItemDrop) {
//			spawnAtLocation(Item::mLead->getId(), 1);
//		}
//	}
//}
//
//bool Entity::canBeLeashed() {
//	return !isLeashed() && isLeashableType();
//}
//
//void Entity::tickLeash() {
//}
//
//int Entity::getAirSupply() const {
//	return (int)mEntityData.getShort(enum_cast(EntityDataIDs::AIR_SUPPLY));
//}
//
//int Entity::getTotalAirSupply() const {
//	return (int)mEntityData.getShort(enum_cast(EntityDataIDs::AIR_SUPPLY_MAX));
//}
//
//float Entity::getEyeHeight() const {
//	return mBBDim.y * 0.85f;
//}
//
//Vec3 Entity::getEyePos() {
//	return mPos + Vec3(0.0f, getEyeHeight(), 0.0f);
//}
//
//void Entity::sendMotionPacketIfNeeded() {
//	DEBUG_ASSERT(!getLevel().isClientSide(), "Cannot send this info from the client");
//
//	ScopedProfile("sendUpdatedState");
//
//	//send packets if requested and if the current instance is in control of the entity
//	if (isAutoSendEnabled() && isControlledByLocalInstance()) {
//		MoveEntityPacketData newData(*this);
//
//		if (newData.distantEnough(*mSentPositionData, getVelocity())) {
//			getDimension().sendPacketForEntity(*this, MoveEntityPacket(newData));
//			*mSentPositionData = newData;
//		}
//
//		const float vDiff = getVelocity().sub(mSentDelta.x, mSentDelta.y, mSentDelta.z).lengthSquared();
//
//		//Velocity
//		if (vDiff > 0.04f || (vDiff > 0 && mPosDelta.x == 0 && mPosDelta.y == 0 && mPosDelta.z == 0)) {
//			mSentDelta.x = mPosDelta.x;
//			mSentDelta.y = mPosDelta.y;
//			mSentDelta.z = mPosDelta.z;
//
//			getDimension().sendPacketForEntity(*this, SetEntityMotionPacket(*this));
//		}
//	}
//}
//
//void Entity::sendMotionToServer(bool doBroadcast) {
//	if (getLevel().isClientSide()) {
//		//this method requests the server to accept our position and speed as-is
//		//TODO should be unified with MovePlayerPacket. MPP should be removed
//		//and any authoritative entity should just send a MoveEntityPacket to the server.
//
//		MoveEntityPacketData newData(*this);
//
//		if (newData.distantEnough(*mSentPositionData, getVelocity())) {
//			if (doBroadcast) {
//				getRegion().getDimension().sendBroadcast(MoveEntityPacket(newData));
//			}
//			*mSentPositionData = newData;
//
//			MoveEntityPacket p(newData);
//
//			getLevel().getPacketSender()->send(p);
//		}
//		const float vDiff = getVelocity().sub(mSentDelta.x, mSentDelta.y, mSentDelta.z).lengthSquared();
//
//		//Velocity
//		if (vDiff > 0.04f || (vDiff > 0 && mPosDelta.x == 0 && mPosDelta.y == 0 && mPosDelta.z == 0)) {
//			SetEntityMotionPacket p;
//			getLevel().getPacketSender()->send(p);
//
//			mSentDelta.x = mPosDelta.x;
//			mSentDelta.y = mPosDelta.y;
//			mSentDelta.z = mPosDelta.z;
//		}
//	}
//}

bool Entity::isRemoved() const {
	return mRemoved;
}

//bool Entity::canSynchronizeNewEntity() const {
//	return true;
//}
//
//bool Entity::isGlobal() const {
//	return mGlobal;
//}
//
//void Entity::setGlobal(bool g) {
//	mGlobal = g;
//}
//
//bool Entity::isAutonomous() const {
//	return mAutonomous;
//}
//
//void Entity::setAutonomous(bool g) {
//	mAutonomous = g;
//}
//
//void Entity::checkInsideBlocks(float grow) {
//	AABB floored = mBB.grow(Vec3(grow)).flooredCopy(0.001f, -0.001f);
//
//	if (getRegion().hasChunksAt(mBB)) {
//		for (auto x : rangef_incl(floored.min.x, floored.max.x)) {
//			for (auto y : rangef_incl(floored.min.y, floored.max.y)) {
//				for (auto z : rangef_incl(floored.min.z, floored.max.z)) {
//					auto& b = getRegion().getBlock(BlockPos(x, y, z));
//					b.entityInside(getRegion(), BlockPos(x, y, z), *this);
//				}
//			}
//		}
//	}
//}
//
//void Entity::pushOutOfBlocks(const Vec3& vec) {
//	BlockPos pos(
//		Math::floor(vec.x),
//		Math::floor(vec.y),
//		Math::floor(vec.z));
//
//	Vec3 delta = vec - pos;
//	BlockID withinId = mRegion->getBlockID(pos);
//
//	if (Block::mPushesOutItems[withinId]) {
//		bool west = !Block::mPushesOutItems[mRegion->getBlockID(pos.west())];	//	xBlock - 1, yBlock, zBlock
//		bool east = !Block::mPushesOutItems[mRegion->getBlockID(pos.east())];	//	xBlock + 1, yBlock, zBlock
//		bool down = !Block::mPushesOutItems[mRegion->getBlockID(pos.below())];	//	xBlock, yBlock - 1, zBlock
//		bool up =	!Block::mPushesOutItems[mRegion->getBlockID(pos.above())];	//	xBlock, yBlock + 1, zBlock
//		bool north = !Block::mPushesOutItems[mRegion->getBlockID(pos.north())];	//	xBlock, yBlock, zBlock - 1
//		bool south = !Block::mPushesOutItems[mRegion->getBlockID(pos.south())];	//	xBlock, yBlock, zBlock + 1
//
//		int dir = -1;
//		float closest = 9999;
//		if (west && delta.x < closest) {
//			closest = delta.x;
//			dir = 0;
//		}
//
//		if (east && 1 - delta.x < closest) {
//			closest = 1 - delta.x;
//			dir = 1;
//		}
//
//		if (down && delta.y < closest) {
//			closest = delta.y;
//			dir = 2;
//		}
//
//		if (up && 1 - delta.y < closest) {
//			closest = 1 - delta.y;
//			dir = 3;
//		}
//
//		if (north && delta.z < closest) {
//			closest = delta.z;
//			dir = 4;
//		}
//
//		if (south && 1 - delta.z < closest) {
//			dir = 5;
//		}
//
//		float speed = getLevel().getRandom().nextFloat() * 0.2f + 0.1f;
//
//		// when being pushed upwards use a spring constraint, this prevents crazy oscillation
//		if (dir == 3) {
//			AABB box;
//			if (Block::mBlocks[withinId]->getCollisionShape(box, *mRegion, pos, nullptr)) {
//				Vec3 bounds = box.getBounds();
//				float dy = bounds.y - (vec.y - Math::floor(vec.y));
//				if (bounds.y > 0.0f) {
//					speed *= dy;
//				}
//			}
//		}
//
//		switch (dir) {
//		case 0:
//			mPosDelta.x = -speed;
//			break;
//		case 1:
//			mPosDelta.x = +speed;
//			break;
//		case 2:
//			mPosDelta.y = -speed;
//			break;
//		case 3:
//			mPosDelta.y = +speed;
//			break;
//		case 4:
//			mPosDelta.z = -speed;
//			break;
//		default:
//			mPosDelta.z = +speed;
//			break;
//		}
//	}
//}
//
//bool Entity::updateWaterState() {
//	if (getLevel().checkAndHandleMaterial(getHandleWaterAABB(), MaterialType::Water, this)) {
//		if (!isRiding() && !mWasInWater && !mFirstTick) {
//			doWaterSplashEffect();
//		}
//		mFallDistance = 0;
//		mWasInWater = true;
//		mOnFire = 0;
//	}
//	else {
//		mWasInWater = false;
//	}
//
//	// If on fire, check to see if this entity has ended up inside a cauldron
//	if (mOnFire > 0) {
//		BlockPos feet = BlockPos(mPos.x, mBB.min.y, mPos.z);
//		auto maybeCauldron = mRegion->getBlockAndData(feet);
//		if (maybeCauldron.id == Block::mCauldron->mID) {
//			// If there is water in the cauldron, put out our fire
//			if (CauldronBlock::getWaterLevel(maybeCauldron.data) > CauldronBlock::MIN_FILL_LEVEL) {
//				if (!mRegion->getLevel().isClientSide()) {
//					// ONLY decrease the water level of the cauldron to account for evaporation if cauldron contains plain water
//					CauldronBlockEntity* ce = static_cast<CauldronBlockEntity*>(getRegion().getBlockEntity(feet));
//					const CauldronBlock* cauldron = static_cast<const CauldronBlock*>(ce->getBlock());
//					int potionID = ce->getPotionId();
//					MobEffect* effect = MobEffect::getById(potionID);
//					if (effect == nullptr) {
//						cauldron->setWaterLevel(*mRegion, feet, maybeCauldron.data, maybeCauldron.data - 1, this);
//
//						// Put out the fire
//						mFallDistance = 0;
//						mWasInWater = true;
//						mOnFire = 0;
//					}
//				}
//			}
//		}
//	}
//
//	return mWasInWater;
//}
//
//void Entity::doWaterSplashEffect() {
//	float speed = sqrt(mPosDelta.x * mPosDelta.x * 0.2f + mPosDelta.y * mPosDelta.y + mPosDelta.z * mPosDelta.z * 0.2f) * 0.2f;
//	if (speed > 1) {
//		speed = 1;
//	}
//
//	playSound(LevelSoundEvent::Splash, getAttachPos(EntityLocation::Body), static_cast<int>(speed * std::numeric_limits<int>::max()));
//
//	// Spawn Particles
//	float yt = floorf(mBB.min.y);
//
//	for (int i = 0; i < 1 + mBBDim.x * 20; i++) {
//		float xo = (getLevel().getRandom().nextFloat() * 2 - 1) * mBBDim.x;
//		float zo = (getLevel().getRandom().nextFloat() * 2 - 1) * mBBDim.x;
//		getLevel().addParticle(ParticleType::Bubble, Vec3(mPos.x + xo, yt + 1, mPos.z + zo), Vec3(mPosDelta.x, mPosDelta.y, mPosDelta.z));
//	}
//
//	for (int i = 0; i < 1 + mBBDim.x * 20; i++) {
//		float xo = (getLevel().getRandom().nextFloat() * 2 - 1) * mBBDim.x;
//		float zo = (getLevel().getRandom().nextFloat() * 2 - 1) * mBBDim.x;
//		getLevel().addParticle(ParticleType::WaterSplash, Vec3(mPos.x + xo, yt + 1, mPos.z + zo), Vec3(mPosDelta.x, mPosDelta.y - getLevel().getRandom().nextFloat() * 0.2f, mPosDelta.z));
//	}
//}
//
//void Entity::updateInsideBlock() {
//	// Handle per frame inside portals
//	if (mInsideBlockId == Block::mPortal->mID && mInsideBlockId != Block::mEndPortal->mID) {
//		if (mPortalCooldown > 0) {
//			mPortalCooldown = getPortalCooldown();
//		}
//		else if (!getLevel().isClientSide() && isAlive()) {
//			int waitTime = getPortalWaitTime();
//			if (mInsidePortalTime++ >= waitTime) {
//				mInsidePortalTime = waitTime;
//				DimensionId toDim = (mRegion->getDimensionId() == DimensionId::Overworld) ? DimensionId::Nether : DimensionId::Overworld;
//				mPortalCooldown = getPortalCooldown();
//
//				// Now change dimension
//				changeDimension(toDim);
//			}
//		}
//	}
//	else {
//		if (mInsidePortalTime > 0) {
//			mInsidePortalTime -= 4;
//			if (mInsidePortalTime < 0) {
//				mInsidePortalTime = 0;
//			}
//		}
//	}
//
//	if (mPortalCooldown > 0) {
//		mPortalCooldown--;
//	}
//
//	// Handle per frame inside cactus
//	if (mInsideBlockId == Block::mCactus->mID) {
//		EntityDamageByBlockSource source = EntityDamageByBlockSource(FullBlock(mInsideBlockId), EntityDamageCause::Contact);
//		hurt(source, 1);
//	}
//}
//
//bool Entity::isInWorld() const {
//	return mAdded && mRegion && !mRemoved;
//}
//
//void Entity::onBlockCollision(int axis) {
//}
//
//void Entity::enableAutoSendPosRot(bool enable) {
//	mSentPositionData = enable ? make_unique<MoveEntityPacketData>() : nullptr;
//}
//
//bool Entity::isAutoSendEnabled() const {
//	return mSentPositionData != nullptr;
//}
//
//const Color & Entity::getHurtColor() const {
//	return mHurtColor;
//}
//
//bool Entity::isControlledByLocalInstance() const {
//	if (auto player = getLevel().getPlayer(getControllingPlayer())) {
//		return player->isLocalPlayer();
//	}
//	else {
//		return !getLevel().isClientSide();
//	}
//}
//
////bool Entity::canInteractWith(Player& player) {
////	if (mContainerComponent != nullptr) {
////		return true;
////	}
////	ItemInstance* item = player.getSelectedItem();
////
////	if (item) {
////		if (mExplodeComponent != nullptr && !mExplodeComponent->isPrimed()) {
////			return item->isInstance(Item::mFlintAndSteel);
////		}
////	}
////
////	return false;
////}
////
////std::string Entity::getInteractText(Player& player) {
////	return Util::EMPTY_STRING;
////}

void Entity::setRegion(BlockSource& source) {
	mRegion = &source;
	mDimensionId = source.getDimensionId();
}

Vec3 Entity::getInterpolatedPosition(float a) const {
	return Vec3(
		mPosPrev2.x + (mPos.x - mPosPrev2.x) * a,
		mPosPrev2.y + (mPos.y - mPosPrev2.y) * a,
		mPosPrev2.z + (mPos.z - mPosPrev2.z) * a);
}

Vec3 Entity::getInterpolatedPosition2(float a) const {
	return Vec3(
		mPosPrev.x + (mPos.x - mPosPrev.x) * a,
		mPosPrev.y + (mPos.y - mPosPrev.y) * a,
		mPosPrev.z + (mPos.z - mPosPrev.z) * a);
}

Vec2 Entity::getRotation() const {
	Vec2 rot(mRot.x, mRot.y);
// 	if (mRiding && mRiding->mInheritRotationWhenRiding) {
// 		rot += mRiding->getRotation();
// 	}

	return rot;
}

//Vec3 Entity::buildForward() const {
//	return Vec3::directionFromRotation(getRotation());
//}

Vec2 Entity::getInterpolatedRotation(float a) const {
	auto rot = Vec2(
		mRotPrev.x + (mRot.x - mRotPrev.x) * a,
		mRotPrev.y + (mRot.y - mRotPrev.y) * a);

// 	if (mRiding && mRiding->mInheritRotationWhenRiding) {
// 		rot += mRiding->getInterpolatedRotation(a);
// 	}

	return rot;
}

//AABB Entity::getHandleWaterAABB() const {
//	// @TODO: Figure out why it's -0.4f, and what it actually means
//	//	The -0.4 is there to correct for standing on a slab. The flooredCopy of our bb that is calculated in
//	// checkAndHandleWater
//	//	will return a 3 tall box instead of a 2 tall and will successfully test that we're in a water block above us even
//	// when we're not.
//
//	//
//	// Ok, lets use the same logic as Java, where they swap y to ensure min/max y is correct after growing with a
//	// negative value:
//	// this y0,y1 code replaces 'bb.grow(0,-0.4f,0)'
//	float y0 = mBB.min.y + 0.4f;
//	float y1 = mBB.max.y - 0.4f;
//	AABB box(mBB.min.x, std::min(y0, y1), mBB.min.z,
//		mBB.max.x, std::max(y0, y1), mBB.max.z);
//
//	return box.shrink(Vec3(0.001f, 0.001f, 0.001f));
//}
//
//void Entity::handleInsidePortal(const BlockPos& portalPos) {
//	if (!getLevel().isClientSide() && mInsideBlockId != Block::mPortal->mID) {
//		mPortalBlockPos = portalPos;
//	}
//
//	mInsideBlockId = Block::mPortal->mID;
//}
//
//int Entity::getPortalCooldown() const {
//	return SharedConstants::TicksPerSecond * 15;
//}
//
//int Entity::getPortalWaitTime() const {
//	return 0;
//}
//
//int Entity::getPortalEntranceAxis() const {
//	return mPortalEntranceAxis;
//}

DimensionId Entity::getDimensionId() const {
	return mDimensionId;
}

//bool Entity::canChangeDimensions() const {
//	return true;
//}
//
//void Entity::changeDimension(DimensionId toId) {
//	getLevel().entityChangeDimension(*this, toId);
//}
//
//void Entity::changeDimension(const ChangeDimensionPacket& packet) {
//}
//
//EntityUniqueID Entity::getControllingPlayer() const {
//	return{};
//}

Level& Entity::getLevel()
{
	DEBUG_ASSERT(mLevel, "Entity was not placed in a level");
	return *mLevel;
}

const Level& Entity::getLevel() const {
	DEBUG_ASSERT(mLevel, "Entity was not placed in a level");
	return *mLevel;
}

//AgeableComponent* Entity::getAgeableComponent() const {
//	return mAgeableComponent.get();
//}
//
//AngryComponent* Entity::getAngryComponent() const {
//	return mAngryComponent.get();
//}
//
//BreathableComponent* Entity::getBreathableComponent() const {
//	return mBreathableComponent.get();
//}
//
//BreedableComponent * Entity::getBreedableComponent() const {
//	return mBreedableComponent.get();
//}
//
//ExplodeComponent* Entity::getExplodeComponent() const {
//	return mExplodeComponent.get();
//}
//
//TameableComponent* Entity::getTameableComponent() const {
//	return mTameableComponent.get();
//}
//
//ProjectileComponent* Entity::getProjectileComponent() const {
//	return mProjectileComponent.get();
//}
//
//LookAtComponent* Entity::getLookAtComponent() const {
//	return mLookAtComponent.get();
//}
//
//TeleportComponent* Entity::getTeleportComponent() const {
//	return mTeleportComponent.get();
//}
//
//MountTamingComponent* Entity::getMountTamingComponent() const {
//	return mMountTamingComponent.get(); 
//}
//
//TimerComponent* Entity::getTimerComponent() const {
//	return mTimerComponent.get();
//}
//
//RideableComponent* Entity::getRideableComponent() const {
//	return mRideableComponent.get();
//}
//
//InteractComponent* Entity::getInteractComponent() const {
//	return mInteractComponent.get();
//}
//
//EquippableComponent* Entity::getEquippableComponent() const {
//	return mEquippableComponent.get();
//}
//
//HealableComponent* Entity::getHealableComponent() const {
//	return mHealableComponent.get();
//}
//
//ScaleByAgeComponent* Entity::getScaleByAgeComponent() const {
//	return mScaleByAgeComponent.get();
//}
//
//BoostableComponent* Entity::getBoostableComponent() const {
//	return mBoostableComponent.get();
//}
//
//RailMovementComponent* Entity::getRailMovementComponent() const {
//	return mRailMovementComponent.get();
//}
//
//DamageSensorComponent* Entity::getDamageSensorComponent() const {
//	return mDamageSensorComponent.get();
//}
//
//ContainerComponent* Entity::getContainerComponent() const {
//	return mContainerComponent.get();
//}
//
//LeashableComponent* Entity::getLeashableComponent() const {
//	return mLeashableComponent.get();
//}
//
//TransformationComponent* Entity::getTransformationComponent() const {
//	return mTransformationComponent.get();
//}
//
//NameableComponent* Entity::getNameableComponent() const {
//	return mNameableComponent.get();
//}
//
//HopperComponent* Entity::getHopperComponent() const {
//	return mHopperComponent.get();
//}
//
//RailActivatorComponent* Entity::getRailActivatorComponent() const {
//	return mRailActivatorComponent.get();
//}
//
//NpcComponent* Entity::getNpcComponent() const {
//	return mNpcComponent.get();
//}
//
//TripodCameraComponent* Entity::getTripodCameraComponent() const {
//	return mTripodCameraComponent.get();
//}
//
//BossComponent* Entity::getBossComponent() const {
//	return mBossComponent.get();
//}
//
//AgentCommandComponent* Entity::getAgentCommandComponent() const {
//	return mAgentCommandComponent.get();
//}
//
//bool Entity::hasOutputSignal(FacingID dir) const {
//	return false;
//}
//
//int Entity::getOutputSignal() const {
//	return 0;
//}
//
//ShooterComponent* Entity::getShooterComponent() const {
//	return mShooterComponent.get();
//}
//
//void Entity::openContainerComponent(Player& player) {
//	if (mContainerComponent) {
//		if (mContainerComponent->openContainer(player)) {
//			return;
//		}
//	}
//}
//
//void Entity::useItem(ItemInstance& instance) const {
//	instance.remove(1);
//}
//
//void Entity::getDebugText(std::vector<std::string>& outputInfo) {
//	outputInfo.push_back("Entity type: " + EntityTypeToString(getEntityTypeId()));
//	outputInfo.push_back("Position: " + _getBlockOnPos().toString());
//
//	outputInfo.push_back("Synchronized Entity Data:");
//
//	float syncScale = mEntityData.getFloat(enum_cast(EntityDataIDs::SCALE));
//	outputInfo.push_back("Scale: " + Util::toString(syncScale));
//	outputInfo.push_back("Variant: " + Util::toString(getVariant()));
//
//	for (int flag = 0; flag < (int)EntityFlags::COUNT; ++flag) {
//		if (getStatusFlag(EntityFlags(flag))) {
//			switch (flag) {
//				case (int)EntityFlags::ONFIRE:
//					outputInfo.push_back("+ONFIRE");
//					break;
//				case (int)EntityFlags::SNEAKING:
//					outputInfo.push_back("+SNEAKING");
//					break;
//				case (int)EntityFlags::RIDING:
//					outputInfo.push_back("+RIDING");
//					break;
//				case (int)EntityFlags::SPRINTING:
//					outputInfo.push_back("+SPRINTING");
//					break;
//				case (int)EntityFlags::USINGITEM:
//					outputInfo.push_back("+USINGITEM");
//					break;
//				case (int)EntityFlags::INVISIBLE:
//					outputInfo.push_back("+INVISIBLE");
//					break;
//				case (int)EntityFlags::TEMPTED:
//					outputInfo.push_back("+TEMPTED");
//					break;
//				case (int)EntityFlags::INLOVE:
//					outputInfo.push_back("+INLOVE");
//					break;
//				case (int)EntityFlags::SADDLED:
//					outputInfo.push_back("+SADDLED");
//					break;
//				case (int)EntityFlags::POWERED:
//					outputInfo.push_back("+POWERED");
//					break;
//				case (int)EntityFlags::IGNITED:
//					outputInfo.push_back("+IGNITED");
//					break;
//				case (int)EntityFlags::BABY:
//					outputInfo.push_back("+BABY");
//					break;
//				case (int)EntityFlags::CONVERTING:
//					outputInfo.push_back("+CONVERTING");
//					break;
//				case (int)EntityFlags::CRITICAL:
//					outputInfo.push_back("+CRITICAL");
//					break;
//				case (int)EntityFlags::CAN_SHOW_NAME:
//					outputInfo.push_back("+SHOWNAME");
//					break;
//				case (int)EntityFlags::NOAI:
//					outputInfo.push_back("+NOAI");
//					break;
//				case (int)EntityFlags::SILENT:
//					outputInfo.push_back("+SILENT");
//					break;
//				case (int)EntityFlags::WALLCLIMBING:
//					outputInfo.push_back("+WALLCLIMBING");
//					break;
//				case (int)EntityFlags::RESTING:
//					outputInfo.push_back("+RESTING");
//					break;
//				case (int)EntityFlags::SITTING:
//					outputInfo.push_back("+SITTING");
//					break;
//				case (int)EntityFlags::ANGRY:
//					outputInfo.push_back("+ANGRY");
//					break;
//				case (int)EntityFlags::INTERESTED:
//					outputInfo.push_back("+INTERESTED");
//					break;
//				case (int)EntityFlags::CHARGED:
//					outputInfo.push_back("+CHARGED");
//					break;
//				case (int)EntityFlags::TAMED:
//					outputInfo.push_back("+TAMED");
//					break;
//				case (int)EntityFlags::LEASHED:
//					outputInfo.push_back("+LEASHED");
//					break;
//				case (int)EntityFlags::SHEARED:
//					outputInfo.push_back("+SHEARED");
//					break;
//				case (int)EntityFlags::GLIDING:
//					outputInfo.push_back("+GLIDING");
//					break;
//				case (int)EntityFlags::ELDER:
//					outputInfo.push_back("+ELDER");
//					break;
//				case (int)EntityFlags::MOVING:
//					outputInfo.push_back("+MOVING");
//					break;
//				case (int)EntityFlags::BREATHING:
//					outputInfo.push_back("+BREATHING");
//					break;
//				case (int)EntityFlags::CHESTED:
//					outputInfo.push_back("+CHESTED");
//					break;
//				case (int)EntityFlags::STACKABLE:
//					outputInfo.push_back("+STACKABLE");
//					break;
//				case (int)EntityFlags::IDLING:
//					outputInfo.push_back("+IDLING");
//					break;
//			}
//		}
//	}
//
//}
//void Entity::setColor(PaletteColor color) {
//	mEntityData.set(enum_cast(EntityDataIDs::COLOR_INDEX), (SynchedEntityData::TypeInt8)color);
//}
//PaletteColor Entity::getColor() const {
//	return Palette::fromByte(mEntityData.getInt8(enum_cast(EntityDataIDs::COLOR_INDEX)) & 0xF);
//}
//
//void Entity::moveBBs(const Vec3& pos) {
//	mBB.move(pos);
//	for (auto& bb : mSubBBs) {
//		bb.move(pos);
//	}
//}
//
//void Entity::setLastHitBB(const Vec3& pos, const Vec3& radius) {
//	for (auto& bb : mSubBBs) {
//		auto b = bb.grow(radius);
//		if (b.contains(pos)) {
//			mLastHitBB = &bb;
//		}
//	}
//}
//
//LootTable* Entity::getLootTable() const {
//	if (mCurrentDescription && mCurrentDescription->mLootTable) {
//		return LootTables::lookupByName(*mCurrentDescription->mLootTable, *(getLevel().getServerResourcePackManager()));
//	}
//	return nullptr;
//}
//
//
//// Restriction
//bool Entity::isWithinRestriction() const {
//	return isWithinRestriction(BlockPos(this->getPos()));
//}
//
//bool Entity::isWithinRestriction(const BlockPos& pos) const {
//	if (!hasRestriction()) {
//		return true;
//	}
//
//	return mRestrictCenter.distSqr(pos) < (mRestrictRadius * mRestrictRadius);
//}
//
//void Entity::restrictTo(const BlockPos& newCenter, float radius) {
//	mRestrictCenter = newCenter;
//	mRestrictRadius = radius;
//}
//
//BlockPos Entity::getRestrictCenter() const {
//	return mRestrictCenter;
//
//}
//
//float Entity::getRestrictRadius() const {
//	return mRestrictRadius;
//}
//
//bool Entity::hasRestriction() const {
//	return mRestrictRadius != -1.0f;
//}
//
//void Entity::clearRestriction() {
//	mRestrictRadius = -1.0f;
//}
//
//float Entity::getMapDecorationRotation() {
//	return getRotation().y;
//}
//
//float Entity::getRiderDecorationRotation(Player &player) {
//	return player.getRotation().y;
//}

float Entity::getYHeadRot() const {
	return 0.f;
}

//bool Entity::isWorldBuilder() {
//	return false;
//}

bool Entity::isCreative() const {
	return false;
}

//bool Entity::add(ItemInstance& item) {
//	if (auto container = getContainerComponent()) {
//		return container->addItem(getRegion(), item, 0);
//	}
//	return false;
//}
//
//void Entity::drop(const ItemInstance& item, bool randomly /*= false*/) {
//	if (item.isNull()) {
//		return;
//	}
//
//	ItemEntity *thrownItem = getLevel().getSpawner().spawnItem(getRegion(), item, this, getAttachPos(EntityLocation::DropAttachPoint) + mPosDelta, SharedConstants::TicksPerSecond * 2);
//	thrownItem->setSourceEntity(this);
//	thrownItem->mRot = mRot;
//
//	float pow = 0.1f;
//	if (randomly) {
//		float _pow = mRandom.nextFloat() * 0.5f;
//		float dir = mRandom.nextFloat() * PI * 2;
//		thrownItem->mPosDelta.x = -Math::sin(dir) * _pow;
//		thrownItem->mPosDelta.z = Math::cos(dir) * _pow;
//		thrownItem->mPosDelta.y = 0.2f;
//
//	}
//	else {
//		pow = 0.3f;
//		thrownItem->mPosDelta.x = -Math::sin(mRot.y* Math::DEGRAD) * Math::cos(mRot.x* Math::DEGRAD) * pow;
//		thrownItem->mPosDelta.z = Math::cos(mRot.y* Math::DEGRAD) * Math::cos(mRot.x* Math::DEGRAD) * pow;
//		thrownItem->mPosDelta.y = -Math::sin(mRot.x* Math::DEGRAD) * pow;
//		pow = 0.02f;
//
//		float dir = mRandom.nextFloat() * PI * 2;
//		pow *= mRandom.nextFloat();
//		thrownItem->mPosDelta.x += Math::cos(dir) * pow;
//		thrownItem->mPosDelta.z += Math::sin(dir) * pow;
//
//		thrownItem->mPosDelta += mPosDelta; //the item starts as fast as us
//	}
//}
//
//void Entity::dropTowards(const ItemInstance& item, Vec3 towards) {
//	Vec3 pos = getAttachPos(EntityLocation::DropAttachPoint);
//	ItemEntity *itemEntity = getLevel().getSpawner().spawnItem(getRegion(), item, nullptr, pos + mPosDelta, SharedConstants::TicksPerSecond * 2);
//
//	float pow = 0.1f;
//	itemEntity->mPosDelta = (towards - pos) * pow;
//	itemEntity->mPosDelta.y += 0.2f;
//}
//
//// Love Partner
//EntityUniqueID const& Entity::lovePartnerId() const {
//	return mInLovePartner;
//}
//
//void Entity::setInLove(Entity* lovePartner) {
//	if (lovePartner) {
//		mInLovePartner = lovePartner->getUniqueID();
//		setStatusFlag(EntityFlags::INLOVE, true);
//		return;
//	}
//	
//	mInLovePartner = EntityUniqueID();
//	setStatusFlag(EntityFlags::INLOVE, false);
//}
//
//Vec3 Entity::calcCenterPos() const {
//	return Vec3(mPos.x, mBB.min.y + (mBBDim.y * 0.5f), mPos.z);
//}
//
//void Entity::startSeenByPlayer(Player& player) {
//}
//
//void Entity::stopSeenByPlayer(Player& player) {
//}
//
//bool Entity::hasEnoughFoodToBreed() {
//	return _countFood(1);
//}
//
//bool Entity::hasExcessFood() {
//	return _countFood(2);
//}
//
//bool Entity::wantsMoreFood() {
//	bool isFarmer = getVariant() == 0 /*Profession::Farmer*/;
//	if (isFarmer) {
//		return !_countFood(5);
//	}
//	return !_countFood(1);
//}
//
//bool Entity::_countFood(int baseMultiplier) {
//
//	bool isFarmer = getVariant() == 0;
//
//	// TODO:: Replace with EntityDefinition Const data
//	const int FOOD_BREAD_COUNT = 3;
//	const int FOOD_OTHER_COUNT = 12;
//
//	if (mContainerComponent) {
//		for (int i = 0; i < mContainerComponent->mContainer->getContainerSize(); i++) {
//			ItemInstance* item = mContainerComponent->mContainer->getItem(i);
//			if (item != nullptr) {
//				if ((item->getItem() == Item::mBread && item->getStackSize() >= FOOD_BREAD_COUNT * baseMultiplier)
//					|| (item->getItem() == Item::mPotato && item->getStackSize() >= FOOD_OTHER_COUNT * baseMultiplier)
//					|| (item->getItem() == Item::mCarrot && item->getStackSize() >= FOOD_OTHER_COUNT * baseMultiplier)) {
//					return true;
//				}
//				if (isFarmer) {
//					if ((item->getItem() == Item::mWheat && item->getStackSize() >= FOOD_BREAD_COUNT * 3 * baseMultiplier)) {
//						return true;
//					}
//				}
//			}
//		}
//	}
//
//	return false;
//}
//
//void Entity::doEnchantDamageEffects(Mob& source, Mob& target) {
//	if (target.hasCategory(EntityCategory::Mob)) {
//		EnchantUtils::doPostHurtEffects(target, source);
//	}
//}
//
//bool Entity::getInteraction(Player& player, EntityInteraction& interaction) {
//
//	ItemInstance* selectedItem = player.getSelectedItem();
//	if (selectedItem != nullptr) {
//		if (selectedItem->getItem() == Item::mDye_powder) {
//			DataID colorData = ClothBlock::getBlockDataForItemAuxValue(selectedItem->getAuxValue());
//			auto newColor = static_cast<PaletteColor>(colorData);
//
//			if (mCurrentDescription->mDyeable && getColor() != newColor) {
//				interaction.setInteractText(mCurrentDescription->mDyeable->mInteractText);
//				if (interaction.shouldCapture()) {
//					Player* pPlayer = &player;
//					interaction.capture(
//						[this, pPlayer, newColor]() {
//						ItemInstance* usedItem = pPlayer->getSelectedItem();
//						if (usedItem) {
//							pPlayer->useItem(*usedItem);
//							replaceCurrentItem(*pPlayer, *usedItem);
//						}
//						setColor(newColor);
//					}
//					);
//				}
//
//				return true;
//			}
//		}
//	}
//
//	// special entity functionality
//	if (mInteractComponent != nullptr && mInteractComponent->getInteraction(player, interaction)) {
//		return true;
//	}
//
//	// component functionality
//	if (mTameableComponent != nullptr && mTameableComponent->getInteraction(player, interaction)) {
//		return true;
//	}
//	if (mAgeableComponent != nullptr && mAgeableComponent->getInteraction(player, interaction)) {
//		return true;
//	}
//	if (mHealableComponent != nullptr && mHealableComponent->getInteraction(player, interaction)) {
//		return true;
//	}
//	if (mBreedableComponent != nullptr && mBreedableComponent->getInteraction(player, interaction)) {
//		return true;
//	}
//	if (mLeashableComponent != nullptr && mLeashableComponent->getInteraction(player, interaction)) {
//		return true;
//	}
//	if (mNameableComponent != nullptr && mNameableComponent->getInteraction(player, interaction)) {
//		return true;
//	}
//	if (mMountTamingComponent != nullptr && mMountTamingComponent->getInteraction(player, interaction)) {
//		return true;
//	}
//	if (mRideableComponent != nullptr && mRideableComponent->getInteraction(player, interaction)) {
//		return true;
//	}
//	if (mSitComponent != nullptr && mSitComponent->getInteraction(player, interaction)) {
//		return true;
//	}
//	if (mNpcComponent != nullptr && mNpcComponent->getInteraction(player, interaction)) {
//		return true;
//	}
//
//	if (mTripodCameraComponent != nullptr && mTripodCameraComponent->getInteraction(player, interaction)) {
//		return true;
//	}
//
//	// inventory container
//	//TODO adors - container component and riding will conflict
//	if (mContainerComponent != nullptr && mCurrentDescription->mContainer) { // && player.mRiding == this) {
//		if (!mCurrentDescription->mContainer->mPrivate) {
//			if (interaction.shouldCapture()) {
//				Player* pPlayer = &player;
//				interaction.capture([=]() {
//					openContainerComponent(*pPlayer);
//				});
//			}
//			// horses are dumb
//			if (!EntityClassTree::isInstanceOf(*this, EntityType::EquineAnimal) || player.isSneaking()) {
//				interaction.setInteractText("action.interact.opencontainer");
//			}
//
//			return true;
//		}
//	}
//
//	return false;
//}
//
//void Entity::setChainedDamageEffects(bool active) {
//	mChainedDamageEffects = active;
//}
//
//bool Entity::getChainedDamageEffects() const {
//	return mChainedDamageEffects;
//}
//
//void Entity::setCollidableMobNear(bool mobNear) {
//	mCollidableMobNear = mobNear;
//}
//
//bool Entity::getCollidableMobNear() {
//	return mCollidableMobNear;
//}
//
//void Entity::setCollidableMob(bool collidable) {
//	mCollidableMob = collidable;
//}
//
//bool Entity::getCollidableMob() {
//	return mCollidableMob;
//}
//
//void Entity::initParams(VariantParameterList& params) {
//	params.pushParameter("self", this);
//	Entity* target = getTarget();
//	if (target != nullptr) {
//		params.pushParameter("target", target);
//	}
//}
//
//bool Entity::onLadder(bool orVine) {
//	return false;
//}
//
