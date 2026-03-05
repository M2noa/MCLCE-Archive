/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/entity/Mob.h"

#include "Core/Utility/UUID.h"
#include "world/entity/player/Abilities.h"
#include "world/level/GameType.h"
//#include "world/item/ClockItem.h"
//#include "world/item/CompassItem.h"
//#include "network/NetworkIdentifier.h"
//#include "server/commands/CommandPermissionLevel.h"
//#include "world/Container.h"
//#include "Audio/SoundSystem.h"

class Attribute;
class Block;
//class BlockEntity;
class BrewingStandBlockEntity;
class ChestBlockEntity;
class ChunkSource;
class CompoundTag;
class Dimension;
class EnchantingTableEntity;
class FillingContainer;
class FoodItemComponent;
class FurnaceBlockEntity;
class IContainerManager;
class Inventory;
//class ItemEntity;
class ItemInstance;
class MinecraftEventing;
class PacketSender;
class PacketSender;
class PlayerChunkSource;
class SkinInfoData;
class EventPacket;
//class Agent;
//class Npc;
class ContainerManagerModel;
class ChalkboardBlockEntity;
class FixedInventoryContainer;
class HudContainerManagerModel;
class PlayerInventoryProxy;
class EnderChestContainer;
enum class CooldownType;
class Item;
class Certificate;

enum ContainerID : signed char;

struct Tick;

enum class BedSleepingResult {
	OK,
	NOT_POSSIBLE_HERE,
	NOT_POSSIBLE_NOW,
	TOO_FAR_AWAY,
	OTHER_PROBLEM,
	NOT_SAFE
};

class Player: public Mob {

	//typedef SynchedEntityData::TypeInt8 PlayerFlagIDType;

	static const int DATA_PLAYER_FLAGS_ID = 16;
	static const int DATA_BED_POSITION_ID = 17;
	static const int DATA_PLAYER_DEAD = 18;
	static const int DATA_PLAYER_PLAYERINDEX = 19;
	static const int PLAYER_SLEEP_FLAG = 1;
	static const int PLAYER_DEAD_FLAG = 2;

public:

	static const float DEFAULT_PLAYER_HEIGHT_OFFSET;
	static const float PLAYER_GLIDING_CAMERA_OFFSET;

	static const float DEFAULT_BB_WIDTH;
	static const float DEFAULT_BB_HEIGHT;

	static const float DISTANCE_TO_TRAVELLED_EVENT;
	static const float DISTANCE_TO_TRANSFORM_EVENT;

	enum class DimensionState {
		Ready,
		Pending,
		WaitingServerResponse,
		WaitingArea,
		WaitingRespawn
	};

	enum class PositionMode : byte {
		Normal,
		Respawn,
		Teleport,
		OnlyHeadRot
	};

	static const int MAX_NAME_LENGTH = 16;
	static const int MIN_NAME_LENGTH = 1;
	static const int MAX_HEALTH = 20;
	static const float DEFAULT_WALK_SPEED;
	static const float DEFAULT_FLY_SPEED;
	static const int SLEEP_DURATION = 100;
	static const int WAKE_UP_DURATION = 10;

	static const int SLOT_HELM = 0;
	static const int SLOT_CHEST = 1;
	static const int SLOT_LEGGINGS = 2;
	static const int SLOT_BOOTS = 3;

	//players create their own blocksources, usually
//	Player(Level& level, PacketSender& packetSender, GameType playerGameType, const NetworkIdentifier& owner, Unique<SkinInfoData> skin, mce::UUID uuid, Unique<Certificate> certificate);
//	virtual ~Player();
//
//	void initializeComponents(InitializationMethod method, const VariantParameterList &params) override;
//	void reloadHardcoded(InitializationMethod method, const VariantParameterList &params) override;
//
//	NetworkIdentifier getClientId() const;
//	const Certificate* getCertificate() const;
//
//	virtual void prepareRegion(ChunkSource& mainChunkSource);
//	virtual void destroyRegion();
//	virtual void suspendRegion();
//
//	virtual void onPrepChangeDimension();
//	virtual void onDimensionChanged();
//
//	virtual bool onLadder(bool orVine = true) override;
//
//	virtual void changeDimensionWithCredits(DimensionId dimension);
//
//	PlayerChunkSource* getChunkSource() const {
//		return mChunkSource.get();
//	}
//
//	Dimension* getDimension() const;
//
//	int getChunkRadius() const;
//	void setChunkRadius(int chunkRadius);
//
//	virtual int tickWorld(const Tick& currentTick);
//
//	virtual void normalTick() override;
//	virtual void moveView();
//	virtual void setName(const std::string& newName);
//	virtual void onSynchedDataUpdate(int dataId) override;
//	virtual void aiStep() override;
//	virtual void travel(float xa, float ya) override;
//	virtual void checkMovementStats(const Vec3& d);
//	virtual void jumpFromGround() override;
//	void handleJumpEffects();
//
//	virtual void rideTick() override;
//
//	virtual void die(const EntityDamageSource& source) override;
//	virtual void remove() override;
//	virtual void respawn();
//	//virtual void dropAllGear(int lootBonusLevel = 0) override;
//	virtual void dropEquipment(const EntityDamageSource& source, int lootBonusLevel) override;
//
//	void updateTeleportDestPos();
//
//	virtual void resetPos(bool clearMore);
//	Vec3 getStandingPositionOnBlock(const BlockPos& pos);
//	BlockPos getSpawnPosition();
//	bool isForcedRespawn() const;
//
//	void setRespawnPosition(const BlockPos& respawnPosition, bool forced);
//	void setRespawnDimensionId(DimensionId respawnDimensionId);
//
//	DimensionId getRespawnDimensionId() const {
//		return mRespawnDimensionId;
//	}
//
//	virtual bool isShootable() override;
//	bool isInCreativeMode();
//	virtual bool isCreativeModeAllowed() override;
//	virtual bool isInTrialMode();
//	bool isBouncing() const;
//
//	bool isDestroyingBlock() {
//		return mDestroyingBlock;
//	}
//
//	bool isHiddenFrom(Mob& target) const;
//
//	virtual void setSpeed(float speed) override;
//	virtual bool hasResource(int id);
//	bool hasRespawnPosition() const;
//
//	bool isUsingItem() const;
//	ItemInstance* getItemInUse();
//
//	void startUsingItem(ItemInstance instance, int duration);
//	void stopUsingItem();
//	void releaseUsingItem();
//	virtual void completeUsingItem();
//
//	virtual int getItemUseDuration() override;
//	int getTicksUsingItem();
//
//	int getScore();
//	virtual void awardKillScore(Entity& victim, int score) override;
//	virtual void handleEntityEvent(EntityEvent id, int data) override;
//
//	virtual std::vector<const ItemInstance*> getAllHand() const override;
//	virtual std::vector<ItemInstance*> getAllHand() override;
//	virtual std::vector<const ItemInstance*> getAllEquipment() const override;
//	virtual std::vector<ItemInstance*> getAllEquipment() override;
//
//	void take(Entity& e, int orgCount);
//	virtual bool add(ItemInstance& item) override;
//
//	bool canDestroy(const Block& block) const;
//	float getDestroySpeed(const Block& block) const;
//	float getDestroyProgress(const Block& block) const;
//
//	CommandPermissionLevel getPermissions();
//	virtual void setPermissions(CommandPermissionLevel permissions);
//
//	bool isHurt();
//	bool isHungry() const;
//
//	bool interact(Entity& entity);
//	void attack(Entity& entity);
//	virtual const ItemInstance* getCarriedItem() const override;
//	virtual ItemInstance* getCarriedItem()override;
//	bool canUseCarriedItemWhileMoving();
//
//	virtual void startCrafting(const BlockPos& pos, bool workbench = true);
//	virtual void startStonecutting(const BlockPos& pos);
//
//	virtual void startDestroying() {
//		mDestroyingBlock = true;
//	}
//
//	virtual void stopDestroying() {
//		mDestroyingBlock = false;
//	}
//
//	virtual void openContainer(int containerSize, const BlockPos& pos);
//	virtual void openContainer(int containerSize, const EntityUniqueID& uniqueID);
//	virtual void openFurnace(int containerSize, const BlockPos& pos);
//	virtual void openEnchanter(int containerSize, const BlockPos& pos);
//	virtual void openAnvil(int containerSize, const BlockPos& pos);
//	virtual void openBrewingStand(int containerSize, const BlockPos& pos);
//	virtual void openHopper(int containerSize, const BlockPos& pos);
//	virtual void openHopper(int containerSize, const EntityUniqueID& entityUniqueID);
//	virtual void openDispenser(int containerSize, const BlockPos& pos, bool dispenser);
//	virtual void openBeacon(int containerSize, const BlockPos& pos);
//	virtual void openPortfolio();
//	virtual void openHorseInventory(int containerSize, const EntityUniqueID& uniqueID);
//	virtual bool canOpenContainerScreen();
//	virtual void openChalkboard(ChalkboardBlockEntity& chalkboard);
//	virtual void openNpcInteractScreen(Entity& npc);
//	virtual void openInventory();
//
//	virtual void openStructureEditor(const BlockPos& pos);
//
//
//	virtual void displayChatMessage(const std::string& author, const std::string& message) {
//		UNUSED_PARAMETER(author, message);
//	}
//
//	virtual void displayClientMessage(const std::string& message) {
//		UNUSED_PARAMETER(message);
//	}
//
//	virtual void displayLocalizableMessage(const std::string& message, const std::vector<std::string>& params = {}, bool toSpeech = false) {
//		UNUSED_PARAMETER(message, params, toSpeech);
//	}
//
//	virtual void displayWhisperMessage(const std::string& author, const std::string& message) {
//		UNUSED_PARAMETER(author, message);
//	}
//
//	virtual float getHeadHeight() const override;
//	virtual float getCameraOffset() const override;
//	virtual float getRidingHeight() override;

	EntityType getEntityTypeId() const override {
		return EntityType::Player;
	}

//	virtual bool isSleeping() const override;
//
//	void recheckSpawnPosition();
//	virtual BedSleepingResult startSleepInBed(const BlockPos& pos);
//	virtual void stopSleepInBed(bool forcefulWakeUp, bool updateLevelList);
//	virtual bool canStartSleepInBed();
//	virtual int getSleepTimer() const;
//	void setAllPlayersSleeping();
//	float getSleepRotation();
//	bool isSleepingLongEnough() const;
//	virtual void sendInventory() const override;
//	void sendInventorySlot(int slot) const;
	ItemInstance* getSelectedItem() const;
//
//	int getAttackDamage();
//
//	virtual void openTextEdit(BlockEntity* blockEntity);
//
//	virtual bool isLocalPlayer() const;
//
//	virtual bool useNewAi() const override {
//		return false;
//	}
//
//	virtual void useItem(ItemInstance& instance) const override;
//
//	void setTeleportDestination(const Vec3& pos);
//	void setSkin(const std::string& skinId, const std::vector<uint8_t>& data);
//
//	SkinInfoData& getSkin() const;
//
//	bool isRespawnReady() {
//		return mRespawnReady;
//	}
//
//	virtual void stopLoading() {
//	}
//
//	virtual void registerTrackedBoss(EntityUniqueID mob);
//	virtual void unRegisterTrackedBoss(EntityUniqueID mob);
//	const std::vector<EntityUniqueID>& getTrackedBosses();
//	void updateTrackedBosses();
//	bool shouldUpdateBossGUIBinds();
//	bool shouldUpdateBosGUIControls();
//
//
	virtual bool isCreative() const override;
//	bool isSurvival() const;
//
//	bool isViewer() const;
//	virtual float getSpeed() const override;
//	Vec3 getCapePos(float a);
//
//	virtual int getPortalCooldown() const override;
//	virtual int getPortalWaitTime() const override;
//
//	virtual void setPlayerGameType(GameType gameType);
//
//	GameType getPlayerGameType() {
//		return mPlayerGameType;
//	}
//
//	// set position to go to when going back to screen mode
//	void SetSurvivalViewerPosition(const Vec3& pos) {
//		mSurvivalViewerPosition = pos;
//	}
//		
//	void eat(const ItemInstance& instance);
//	void eat(int nutrition, float saturationModifier);
//
//	virtual void _crit(Entity& entity);
//
//	void causeFoodExhaustion(float exhaustionAmount);
//	virtual bool isImmobile() const override;
//
//	virtual void sendMotionPacketIfNeeded() override;
//	
//	void handleMovePlayerPacket(PositionMode mode, const Vec3& pos, const Vec2& rot, float yHeadRot);
//
//	virtual MinecraftEventing* getEventing() const;
//	virtual void sendEventPacket(const EventPacket& packet) const;
//	virtual bool isPushable() const override;
//	virtual bool isPushableByPiston() const override;
//
//	// experience methods
//	virtual void addExperience(int xp);
//	virtual void addLevels(int levels);	// levels can be added or removed, but level will not become negative
//	int getPlayerLevel(void) const;
//	float getLevelProgress(void) const;	// progress to next level [0.0, 1.0)
//	int getXpNeededForNextLevel(void) const;
//	void resetPlayerLevel(void);
//	virtual int getExperienceReward(void) const override;
//	int getEnchantmentSeed(void) const;
//	void getNewEnchantmentSeed(void);
//	void spawnExperienceOrb(const Vec3& pos, int count);
//
//	virtual Unique<Packet> getAddPacket() override;
//
//	virtual bool isWorldBuilder() override;
//
//	virtual void lavaHurt() override;
//	static bool isValidUserName(const std::string& value);
//	static std::string filterValidUserName(const std::string& value);
//
//	//
//	// ContainerListener
//	//
//	virtual void setContainerData(IContainerManager& menu, int id, int value) = 0;
//	virtual void slotChanged(IContainerManager& menu, int slot, const ItemInstance& item, bool isResultSlot) = 0;
//	virtual void refreshContainer(IContainerManager& menu, const std::vector<ItemInstance>& items) = 0;
//
//	bool hasOpenContainer() const;
//	std::weak_ptr<IContainerManager> getContainerManager();
//	void setContainerManager(std::shared_ptr<IContainerManager> manager);
//
//	virtual void deleteContainerManager();
//
//	std::weak_ptr<HudContainerManagerModel> getHudContainerManagerModel();
//
//	virtual void setEquippedSlot(ArmorSlot slot, int item, int auxValue) override;
//	virtual void setEquippedSlot(ArmorSlot slot, const ItemInstance& item) override;
//
//	float getLuck();
//	virtual void setFieldOfViewModifier(float modifier);
//
//	// Should really be in a "client" class, not this entity
//	virtual bool isPositionRelevant(DimensionId dimension, const BlockPos& position);
//	virtual bool isEntityRelevant(const Entity& entity);
//
//	virtual float getMapDecorationRotation() override;
//	
//	virtual void actuallyHurt(int dmg, const EntityDamageSource* source = nullptr, bool bypassArmor = false) override;
//	virtual void teleportTo(const Vec3& pos, int cause = 0, int sourceEntityType = enum_cast(EntityType::Undefined)) override;
//
//	int32_t getPlayerIndex() const;
//	void setPlayerIndex(int32_t index);
//	
//	void startGliding();
//	void stopGliding();
//	
//	void setAgent(Agent* agent);
//	Agent* getAgent() const;
//
//	virtual void onSuspension();
//
//	virtual bool canBePulledIntoVehicle() const override;
//	virtual void feed(int itemId) override;
//
//	virtual void startCooldown(Item* item);
//	virtual int getItemCooldownLeft(CooldownType type) const;
//	virtual bool isItemInCooldown(CooldownType type) const;
//	void fireDimensionChangedEvent(DimensionId destinationDimension);
//
//	int getDirection() const;
//
//protected:
//
//	void updateAi() override;
//	void setDefaultHeadHeight();
//
//	virtual bool _hurt(const EntityDamageSource& source, int damage, bool knock, bool ignite) override;
//
//	virtual void readAdditionalSaveData(const CompoundTag& tag) override;
//	virtual void addAdditionalSaveData(CompoundTag& entityTag) override;
//
//	void setBedOffset(int bedDirection);
//	bool checkBed(bool useSpawnChunkSource);
//
//	bool fixSpawnPosition(BlockPos& spawnPosition, std::vector<BlockSource*> regions) const;
//	bool fixStartSpawnPosition(BlockPos& spawnPosition, std::vector<BlockSource*> regions) const;
//
//	void registerAttributes();
//
//	void moveCape();
//
//	virtual void onMovePlayerPacketNormal(const Vec3& pos, const Vec2& rot);
//
//	virtual void onBounceStarted(const BlockPos& bounceStartPos, const FullBlock& block) override;
//
//	void _tickCooldowns();
//
//private:
//
//	void _touch(Entity& entity);
//	float _getItemDestroySpeed(const Block& block) const;
//	void _addLevels(int levels);	// levels can be added or removed, but level will not become negative
//
//	void _applyExhaustion(const Vec3& d);

public:

	// Attributes
	static const Attribute HUNGER;
	static const Attribute SATURATION;
	static const Attribute EXHAUSTION;
	static const Attribute LEVEL;		// actual player level based on experience gained
	static const Attribute EXPERIENCE;	// progress to next levelup, on a [0.0, 1.0) scale

//	DimensionState mDimensionState = DimensionState::Ready;
//	bool mServerHasMovementAuthority = false;
//
//	char mUserType;
//	int mScore;
//	float mOBob, mBob;
//	bool mHandsBusy = false;	
//
//	PlayerInventoryProxy& getSupplies() const;
//
//	EnderChestContainer* getEnderChestContainer() {
//		return mEnderChestInventory.get();
//	}

	std::string mName;
	Abilities mAbilities;

//	const NetworkIdentifier mOwner;
	std::string mUniqueName;
	uint64_t mClientRandomId = 0;
//	const mce::UUID mClientUUID;
//	Unique<Certificate> mCertificate;

	int mLastThrowTick;

	int mChunksSentSinceStart = 0;

//	EntityUniqueID mPendingRideID;
//	EntityUniqueID mInteractTarget = {};

	bool mHasFakeInventory;
//	Unique<PlayerChunkSource> mChunkSource;
//	Unique<PlayerChunkSource> mSpawnChunkSource;
//
//	Unique<BlockSource> mOwnedBlockSource;

	BlockPos mBedPosition;
	Vec3 mBedOffset;

	Vec3 mTeleportDestPos;

	bool mUpdateMobs = true;

//	CompassItem::SpriteCalculator mCompassSpriteCalc;
//	ClockItem::SpriteCalculator mClockSpriteCalc;
//
//	// This is horrible, just like me; Stores the item's latest offset from the player,
//	// this is useful for getting the player's right arm position without using the matrix stack every time
//	// TODO: Refactor when full data-driven animations are added in (such as bones)
//	// -- With love, Venvious
//	Vec3 mLatestHandOffset;
//
//	Vec3 mCapePosO, mCapePos;
//
//protected:
//	float mDistanceSinceTravelledEvent;
//	float mDistanceSinceTransformEvent;
//
//	std::shared_ptr<IContainerManager> mContainerManager;
//
//	Unique<PlayerInventoryProxy> mInventoryProxy;
//	Unique<SkinInfoData> mSkin;
//	bool mRespawnReady = false;
//	bool mRespawnBlocked = false;
//	bool mFellWorldAsRespawn = false;
//	DimensionId mRespawnDimensionId = DimensionId::Undefined;
//	bool mCheckBed = false;
//	bool mSpawned = false;
//	ItemInstance mItemInUse;
//	int mItemInUseDuration;
//	short mSleepCounter;
//	PacketSender& mPacketSender;
//
//	BlockPos mBounceStartPos;
//	FullBlock mBounceBlock;
//
//	float mFOVModifier = 1.0f;
//
//	int mVoidDamageTimer = 0;
//
//	std::shared_ptr<HudContainerManagerModel> mHudContainerManagerModel;
//	Unique<EnderChestContainer> mEnderChestInventory;
//
//	std::vector<EntityUniqueID> mTrackedBossIDs;
//	EntityUniqueID mAgentID = {};
//	bool mTrackedBossUpdatebinds;
//	bool mTrackedBossUpdateControls;
//	Player::PositionMode mPositionMode = Player::PositionMode::Normal;
//
//	EntityType mLastHurtBy = EntityType::Undefined;
//
//private:
//
//	void _registerElytraLoopSound();
//
//	// experience related
//	int mLastLevelUpTime;
//
//	// reduces some redundant experience calculations
//	mutable bool mPlayerLevelChanged;
//	mutable int mPreviousLevelRequirement;
//
//	BlockPos mRespawnPosition;
//	bool mIsForcedRespawnPos;
//
//	bool mPlayerIsSleeping;
//	bool mAllPlayersSleeping;
//	Vec3 mSurvivalViewerPosition;	//used for return from Viewer modes, and rendering the "statue"
//
//	bool mDestroyingBlock;
//
//	GameType mPlayerGameType;
//
//	int mEnchantmentSeed;
//	int mChunkRadius = 4;
//	LoopingSoundHandle mElytraLoop;
//
//	std::vector<int> mCooldowns;
};

void replaceCurrentItem(Player& player, ItemInstance& newItem);
void addItem(Player& player, ItemInstance& itemToAdd);
