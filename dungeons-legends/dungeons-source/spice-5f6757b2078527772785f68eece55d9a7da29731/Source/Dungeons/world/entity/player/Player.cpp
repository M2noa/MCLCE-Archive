/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#ifdef DEBUG_STATS
#include "util/DebugStats.h"
#endif

#include "world/entity/player/Player.h"
//#include "world/entity/player/PlayerInventoryProxy.h"
//#include "nbt/CompoundTag.h"
//#include "network/PacketSender.h"
//#include "network/packet/AddPlayerPacket.h"
//#include "network/packet/AddItemPacket.h"
//#include "network/packet/AnimatePacket.h"
//#include "network/packet/BossEventPacket.h"
//#include "network/packet/ContainerSetContentPacket.h"
//#include "network/packet/ContainerSetSlotPacket.h"
//#include "network/packet/PlayerActionPacket.h"
//#include "network/packet/ReplaceItemInSlotPacket.h"
//#include "network/packet/RespawnPacket.h"
//#include "network/packet/SetHealthPacket.h"
//#include "network/packet/SpawnExperienceOrbPacket.h"
//#include "network/packet/TakeItemEntityPacket.h"
//#include "network/packet/EventPacket.h"
//#include "network/PacketSender.h"
//#include "Events/MinecraftEventing.h"
#include "world/attribute/Attribute.h"
//#include "world/attribute/AttributeInstance.h"
//#include "world/attribute/AttributeMap.h"
//#include "world/attribute/ExhaustionAttributeDelegate.h"
//#include "world/attribute/HungerAttributeDelegate.h"
//#include "world/attribute/SharedAttributes.h"
//#include "world/containers/managers/models/HudContainerManagerModel.h"
//#include "world/effect/MobEffect.h"
//#include "world/effect/MobEffectInstance.h"
//#include "world/entity/animal/Pig.h"
//#include "world/entity/animal/Horse.h"
//#include "world/entity/components/BossComponent.h"
//#include "world/entity/EntityClassTree.h"
//#include "world/entity/EntityEvent.h"
//#include "world/entity/EntityDefinition.h"
//#include "world/entity/EntityInteraction.h"
//#include "world/entity/item/ItemEntity.h"
//#include "world/entity/agent/Agent.h"
//#include "world/entity/monster/Skeleton.h"
//#include "world/entity/player/PlayerInventoryProxy.h"
//#include "world/entity/player/SkinInfoData.h"
//#include "world/entity/player/Inventory.h"
//#include "world/level/block/BedBlock.h"
//#include "world/level/BlockSource.h"
//#include "world/level/block/entity/SkullBlockEntity.h"
//#include "world/level/chunk/LevelChunk.h"
//#include "world/level/chunk/PlayerChunkSource.h"
//#include "world/level/Level.h"
//#include "world/level/Spawner.h"
//#include "world/level/material/Material.h"
//#include "world/inventory/EnderChestContainer.h"
//#include "network/packet/EntityEventPacket.h"
//#include "client/sound/SoundPlayer.h"
//#include "world/entity/EntityDamageSource.h"
//#include "world/entity/item/ExperienceOrb.h"
//#include "certificates/ExtendedCertificate.h"

#define PLAYER_MAX_LEVEL 24791

const float Player::DEFAULT_PLAYER_HEIGHT_OFFSET = 1.62f;
const float Player::PLAYER_GLIDING_CAMERA_OFFSET = -1.1f;
const float Player::DEFAULT_WALK_SPEED = 0.1f;
const float Player::DEFAULT_FLY_SPEED = 0.02f;
const float Player::DEFAULT_BB_WIDTH = 0.6f;
const float Player::DEFAULT_BB_HEIGHT = 1.8f;
// Distance in blocks before the corresponding events are fired
// They should be separate variables because the events have entirely different purposes, so one is subject to change without changing the other
const float Player::DISTANCE_TO_TRAVELLED_EVENT = 1.0f;
const float Player::DISTANCE_TO_TRANSFORM_EVENT = 1.0f;

// Attribute Definitions
const Attribute Player::HUNGER = Attribute("minecraft:player.hunger", RedefinitionMode::KeepCurrent, true);
const Attribute Player::SATURATION = Attribute("minecraft:player.saturation", RedefinitionMode::KeepCurrent, true);
// Max is MAX_EXHAUSTION + 1.f to allow for overflow of exhaustion to carry over
const Attribute Player::EXHAUSTION = Attribute("minecraft:player.exhaustion", RedefinitionMode::KeepCurrent, true);

// Experience attributes
const Attribute Player::LEVEL = Attribute("minecraft:player.level", RedefinitionMode::KeepCurrent, true);
const Attribute Player::EXPERIENCE = Attribute("minecraft:player.experience", RedefinitionMode::KeepCurrent, true);

// Player has reference to the level before he is initialized, to prevent massive explosions
//Player::Player(Level& level, PacketSender& packetSender, GameType playerGameType, const NetworkIdentifier& owner, Unique<SkinInfoData> skin, mce::UUID uuid, Unique<Certificate> certificate)
//	: Mob(level)
//	, mPlayerGameType(playerGameType)
//	, mUserType(0)
//	, mHasFakeInventory(false)
//	, mContainerManager(nullptr)
//	, mItemInUseDuration(0)
//	, mPlayerIsSleeping(false)
//	, mSleepCounter(0)
//	, mRespawnPosition(0, -1, 0)
//	, mIsForcedRespawnPos(false)
//	, mAllPlayersSleeping(false)
//	, mDestroyingBlock(false)
//	, mLastThrowTick(-9999)
//	, mPacketSender(packetSender)
//	, mOwner(owner)
//	, mSkin(std::move(skin))
//	, mClientUUID(uuid)
//	, mCertificate(std::move(certificate))
//	, mOBob(0.0f)
//	, mBob(0.0f)
//	, mCooldowns(enum_cast(CooldownType::Count))
//	, mDistanceSinceTravelledEvent(0.0f)
//	, mDistanceSinceTransformEvent(0.0f) {
//
//	mCategories |= EntityCategory::Player;
//	mEntityRendererId = ER_PLAYER_RENDERER;
//
//	registerAttributes();
//
//	mEntityData.define(enum_cast(EntityDataIDs::PLAYER_FLAGS), (PlayerFlagIDType)0);
//	mEntityData.define(enum_cast(EntityDataIDs::PLAYER_INDEX), (SynchedEntityData::TypeInt)0);
//	mEntityData.define(enum_cast(EntityDataIDs::BED_POSITION), BlockPos());
//	mEntityData.define(enum_cast(EntityDataIDs::INTERACT_ENTITY), (SynchedEntityData::TypeInt64)0);
//	mEntityData.define(enum_cast(EntityDataIDs::INTERACT_TEXT), (SynchedEntityData::TypeString)"");
//
//	mHeightOffset = DEFAULT_PLAYER_HEIGHT_OFFSET;
//	setPlayerGameType(mPlayerGameType);
//
//	BlockPos spawnPos = mLevel->getSharedSpawnPos();
//	this->moveTo(Vec3((float)spawnPos.x + 0.5f, (float)(spawnPos.y + 1), (float)spawnPos.z + 0.5f), Vec2(0, 0));
//
//	mInventoryProxy = make_unique<PlayerInventoryProxy>(make_unique<Inventory>(this));
//	//order dependency. we need this to happen after the inventory's construction
//	mHudContainerManagerModel = std::make_shared<HudContainerManagerModel>(CONTAINER_ID_FIXEDINVENTORY, *this);
//	mInventoryProxy->init(mHudContainerManagerModel);
//	getLevel().getFixedInventory().addSizeChangeListener(mInventoryProxy.get());
//
//	mEnderChestInventory = make_unique<EnderChestContainer>();
//	_registerElytraLoopSound();
//}
//
//void Player::initializeComponents(InitializationMethod method, const VariantParameterList &params) {
//	Mob::initializeComponents(method, params);
//
//	if (!mInitialized) {
//		resetAttributes();
//		mOBob = mBob = 0;
//		mSwinging = 0;
//		mSwingTime = 0;
//		mScore = 0;
//		mLastLevelUpTime = 0;
//
//		// Always drop loot when the player dies, max out chances
//		for (int i = 0, count = enum_cast(HandSlot::_count); i < count; i++) {
//			mHandDropChance[i] = std::numeric_limits<float>::max();
//		}
//		for (int i = 0, count = enum_cast(ArmorSlot::_count); i < count; i++) {
//			mArmorDropChance[i] = std::numeric_limits<float>::max();
//		}
//		
//		resetPlayerLevel();
//		getNewEnchantmentSeed();
//	}
//}
//
//void Player::reloadHardcoded(InitializationMethod method, const VariantParameterList &params) {
//	mChunkPos = mPos;
//}
//
//Player::~Player() {
//	SoundPlayer& soundPlayer = getLevel().getSoundPlayer();
//	soundPlayer.unregisterLoop(mElytraLoop);
//
//	deleteContainerManager();
//	destroyRegion();
//	mHudContainerManagerModel.reset();
//	getLevel().getFixedInventory().removeSizeChangeListener(mInventoryProxy.get());
//}
//
//NetworkIdentifier Player::getClientId() const {
//	return mOwner;
//}
//
//const Certificate* Player::getCertificate() const {
//	return mCertificate.get();
//}
//
//void Player::setFieldOfViewModifier(float modifier) {
//	mFOVModifier = modifier;
//}
//
//bool Player::isPositionRelevant(DimensionId dimension, const BlockPos& position) {
//	return false;
//}
//
//bool Player::isEntityRelevant(const Entity& entity) {
//	return false;
//}
//
//float Player::getMapDecorationRotation() {
//	if (isRiding()) {
//		return getRide()->getRiderDecorationRotation(*this);
//	}
//	else {
//		return Mob::getYHeadRot();
//	}
//}
//
//void Player::actuallyHurt(int dmg, const EntityDamageSource* source /* = nullptr */, bool bypassArmor /* = false */) {
//	if (isInvulnerableTo(*source)) {
//		return;
//	}
//
//	// Apply difficulty modifier if damage comes from a mob
//	if (source->isEntitySource() && source->getDamagingEntity()->hasCategory(EntityCategory::Mob)) {
//		Difficulty diff = getLevel().getDifficulty();
//		if (diff == Difficulty::Easy) {
//			// Even damage values lose less on easy
//			dmg -= int((float)dmg / 2 - 0.5f);
//		}
//		else if (diff == Difficulty::Hard) {
//			dmg += int((float)dmg / 2);
//		}
//	}
//
//	Mob::actuallyHurt(dmg, source, bypassArmor);
//}
//
//void Player::teleportTo(const Vec3& pos, int cause, int sourceEntityType) {
//	float d = (mPos - pos).length();
//	Mob::teleportTo(pos);
//	mPositionMode = Player::PositionMode::Teleport;
//	MinecraftEventing::fireEventPlayerTeleported(this, d, static_cast<MinecraftEventing::TeleportationCause>(cause), sourceEntityType);
//}
//
//void Player::registerTrackedBoss(EntityUniqueID mob) {
//	mTrackedBossIDs.emplace_back(mob);
//}
//
//void Player::unRegisterTrackedBoss(EntityUniqueID mob) {
//	mTrackedBossIDs.erase(std::remove(mTrackedBossIDs.begin(), mTrackedBossIDs.end(), mob), mTrackedBossIDs.end());
//}
//
//const std::vector<EntityUniqueID>& Player::getTrackedBosses() {
//	return mTrackedBossIDs;
//}

bool Player::isCreative() const {
// 	return (mPlayerGameType == GameType::Creative) || (mPlayerGameType == GameType::CreativeViewer);
	return false;
}

//bool Player::isSurvival() const {
//	return (mPlayerGameType == GameType::Survival) || (mPlayerGameType == GameType::SurvivalViewer);
//}
//
//bool Player::isViewer() const {
//	return (mPlayerGameType == GameType::SurvivalViewer) || (mPlayerGameType == GameType::CreativeViewer);
//}
//
//float Player::getSpeed() const {
//	return getAttribute(SharedAttributes::MOVEMENT_SPEED).getCurrentValue();
//}
//
//Vec3 Player::getCapePos(float a) {
//	return Vec3::lerp(mCapePosO, mCapePos, a);
//}
//
//void Player::prepareRegion(ChunkSource& mainChunkSource) {
//
//	DEBUG_ASSERT(mChunkRadius, "viewRadius not defined for this player type");
//
//	//create a private chunk view to create the blocksource with
//	if (!mChunkSource) {
//		//expand the region by 2 chunks as they can't be rendered
//		mChunkSource = make_unique<PlayerChunkSource>(*this, mainChunkSource, (mChunkRadius + 2) * CHUNK_WIDTH);
//		mOwnedBlockSource = make_unique<BlockSource>(getLevel(), mainChunkSource.getDimension(), *mChunkSource);
//		setRegion(*mOwnedBlockSource);
//
//		mChunkSource->center();
//		mChunkPos = getPos();
//
//		mSpawnChunkSource = make_unique<PlayerChunkSource>(*this, mainChunkSource, CHUNK_WIDTH * 2);
//	}
//	else {
//		mChunkSource->setRadius((mChunkRadius + 2) * CHUNK_WIDTH);
//	}
//
//	mRespawnDimensionId = getDimension()->getId();
//}
//
//void Player::destroyRegion() {
//	if (mChunkSource) {	//this is a legit data-owning player
//		mChunkSource.reset();
//		mOwnedBlockSource.reset();
//		this->resetRegion();
// 
//	}
//}
//
//void Player::suspendRegion() {
//	if (mChunkSource) {
//		mChunkSource->suspend();
//	}
//}
//
//void Player::onPrepChangeDimension() {
//}
//
//void Player::onDimensionChanged() {
//}
//
//bool Player::onLadder(bool orVine /*= true*/) {
//	int xt = Math::floor(mPos.x);
//	int yt = Math::floor(mBB.min.y);
//	int zt = Math::floor(mPos.z);
//
//	BlockID block = getRegion().getBlockID(xt, yt, zt);
//	return block == Block::mLadder->mID || (orVine && block == Block::mVine->mID);
//}
//
//void Player::changeDimensionWithCredits(DimensionId dimension) {
//}
//
//bool Player::isSleeping() const {
//	return mPlayerIsSleeping;
//}
//
//BedSleepingResult Player::startSleepInBed(const BlockPos& pos) {
//
//	//skip validity checks if there is no region defined (player is being loaded in a bed)
//	if(mOwnedBlockSource && !getLevel().isClientSide()) {
//		if(isSleeping() || !isAlive()) {
//			return BedSleepingResult::OTHER_PROBLEM;
//		}
//		if(std::abs(mPos.x - pos.x) > 3 || std::abs(mPos.y - pos.y) > 4 || std::abs(mPos.z - pos.z) > 3) {
//			return BedSleepingResult::TOO_FAR_AWAY;
//		}
//		if (!getDimension()->isNaturalDimension()) {
//			return BedSleepingResult::NOT_POSSIBLE_HERE;
//		}
//		if (getDimension()->isDay()) {
//			return BedSleepingResult::NOT_POSSIBLE_NOW;
//		}
//
//		const float hRange = 8;
//		const float vRange = 5;
//		auto& monsters = mOwnedBlockSource->getEntities(EntityType::Monster, AABB(pos.x - hRange, pos.y - vRange, pos.z - hRange, pos.x + hRange, pos.y + vRange, pos.z + hRange));
//		if(!monsters.empty()) {
//			return BedSleepingResult::NOT_SAFE;
//		}
//	}
//
//	// Player cannot ride and sleep at the same time hehe
//	if (isRiding()) {
//		stopRiding(true, false);
//	}
//
//	setSize(0.2f, 0.2f);
//	mHeightOffset = 0.2f;
//	if (mOwnedBlockSource && mOwnedBlockSource->hasBlock(pos)) {
//		DataID data = mOwnedBlockSource->getData(pos);
//		int direction = Block::mBed->getBlockState(BlockState::Direction).get<int>(data);
//		float xo = 0.5f, zo = 0.5f;
//
//		switch(direction) {
//		case Direction::SOUTH:
//			zo = 0.9f;
//			break;
//		case Direction::NORTH:
//			zo = 0.1f;
//			break;
//		case Direction::WEST:
//			xo = 0.1f;
//			break;
//		case Direction::EAST:
//			xo = 0.9f;
//			break;
//		}
//		setBedOffset(direction);
//		setPos(Vec3(pos.x + xo, pos.y + 15.0f / 16.0f, pos.z + zo));
//	}
//	else {
//		setPos(Vec3(pos.x + 0.5f, pos.y + 1.0f / 16.0f, pos.z + 0.5f));
//	}
//	mPlayerIsSleeping = true;
//	mSleepCounter = 0;
//	mBedPosition = pos;
//	mPosDelta.x = mPosDelta.z = mPosDelta.y = 0;
//	if(!getLevel().isClientSide()) {
//		getLevel().updateSleepingPlayerList();
//	}
//	mEntityData.set<BlockPos>(enum_cast(EntityDataIDs::BED_POSITION), mBedPosition);
//	mEntityData.setFlag<PlayerFlagIDType>(enum_cast(EntityDataIDs::PLAYER_FLAGS), PLAYER_SLEEP_FLAG);
//	return BedSleepingResult::OK;
//}
//
//void Player::stopSleepInBed( bool forcefulWakeUp, bool updateLevelList ) {
//	if(!isSleeping()) {
//		return;
//	}
//
//	setSize(0.6f, 1.8f);
//	setDefaultHeadHeight();
//	BlockPos standUp = mBedPosition;
//	 
//	if(mOwnedBlockSource->getBlockID(mBedPosition) == Block::mBed->mID) {
//		BedBlock::setOccupied(getRegion(), mBedPosition, false);
//		bool foundStandUpPosition = BedBlock::findStandUpPosition(getRegion(), mBedPosition, 0, standUp);
//		if(!foundStandUpPosition) {
//			standUp = mBedPosition.above();
//		}
//		setPos(Vec3(standUp.x + 0.5f, standUp.y + mHeightOffset + 0.1f, standUp.z + 0.5f));
//	}
//	mPlayerIsSleeping = false;
//	if(!getLevel().isClientSide() && updateLevelList) {
//		getLevel().updateSleepingPlayerList();
//	}
//	if(forcefulWakeUp) {
//		mSleepCounter = 0;
//	}
//	else {
//		mSleepCounter = SLEEP_DURATION;
//	}
//
//	// We should always reset the spawning position when leaving bed
//	setRespawnPosition(standUp, false);
//
//	mEntityData.clearFlag<PlayerFlagIDType>(enum_cast(EntityDataIDs::PLAYER_FLAGS), PLAYER_SLEEP_FLAG);
//	mAllPlayersSleeping = false;
//}
//
//bool Player::canStartSleepInBed() {
//	return mEntityData.getFlag<PlayerFlagIDType>(enum_cast(EntityDataIDs::PLAYER_FLAGS), PLAYER_SLEEP_FLAG);
//}
//
//int Player::getSleepTimer() const
//{
//	return mSleepCounter;
//}
//
//void Player::setAllPlayersSleeping() {
//	mAllPlayersSleeping = true;
//}
//
//void Player::setBedOffset( int bedDirection ) {
//	mBedOffset.x = 0;
//	mBedOffset.z = 0;
//
//	switch(bedDirection) {
//	case Direction::SOUTH:
//		mBedOffset.z = -1.8f;
//		break;
//	case Direction::NORTH:
//		mBedOffset.z = 1.8f;
//		break;
//	case Direction::WEST:
//		mBedOffset.x = 1.8f;
//		break;
//	case Direction::EAST:
//		mBedOffset.x = -1.8f;
//		break;
//	}
//}
//
//bool Player::isSleepingLongEnough() const {
//	return isSleeping() && mSleepCounter >= SLEEP_DURATION;
//}
//
//float Player::getSleepRotation() {
//	if(isSleeping()) {
//		auto data = mOwnedBlockSource->getData(mBedPosition.x, mBedPosition.y, mBedPosition.z);
//		auto direction = Block::mBed->getBlockState(BlockState::Direction).get<int>(data);
//
//		switch(direction) {
//		case Direction::SOUTH:
//			return 90;
//		case Direction::WEST:
//			return 0;
//		case Direction::NORTH:
//			return 270;
//		case Direction::EAST:
//			return 180;
//		}
//	}
//	return 0;
//}
//
//bool Player::checkBed(bool useSpawnChunkSource) {
//	if (useSpawnChunkSource) {
//		BlockSource localBlockSource(getLevel(), *getDimension(), *mSpawnChunkSource);
//		return localBlockSource.getBlock(mBedPosition).isType(Block::mBed);
//	}
//
//	return mOwnedBlockSource->getBlock(mBedPosition).isType(Block::mBed);
//}
//
//void Player::setName(const std::string& newName){
//	mName = newName;
//	mEntityData.set(enum_cast(EntityDataIDs::NAME), (SynchedEntityData::TypeString) newName);
//}
//
//void Player::onSynchedDataUpdate(int dataId) {
//	// Update player list entry in EDU because the server can be authoritative over your name
//#ifdef MCPE_EDU
//	if (dataId == enum_cast(EntityDataIDs::NAME)) {
//		for (auto& playerEntry : mLevel->getPlayerList()) {
//			if (playerEntry.second.mId == getUniqueID()) {
//				playerEntry.second.mPlayerName = getNameTag();
//			}
//		}
//	}
//#endif
//
//	Mob::onSynchedDataUpdate(dataId);
//}
//
//void Player::moveView() {
//	//move the owned area, if any
//	if(mChunkSource && mSpawned) {
//		mChunkSource->center();
//	}
//}
//
//void Player::normalTick() {
//	_tickCooldowns();
//	bool shouldSleep = mEntityData.getFlag<PlayerFlagIDType>(enum_cast(EntityDataIDs::PLAYER_FLAGS), PLAYER_SLEEP_FLAG);
//	if (shouldSleep != isSleeping()) {
//		if (isSleeping()) {
//			stopSleepInBed(true, true);
//		}
//		else if (canStartSleepInBed()) {
//			mBedPosition = mEntityData.getPos(enum_cast(EntityDataIDs::BED_POSITION));
//			startSleepInBed(mBedPosition);
//		}
//	}
//	if (isSleeping()) {
//		mSleepCounter++;
//		if (mSleepCounter > SLEEP_DURATION) {
//			mSleepCounter = SLEEP_DURATION;
//		}
//		if (!getLevel().isClientSide()) {
//			if (!checkBed(false)) {
//				stopSleepInBed(true, true);
//			}
//			else if (getDimension()->isDay()) {
//				stopSleepInBed(false, true);
//			}
//		}
//	}
//	else if (mSleepCounter > 0) {
//		mSleepCounter++;
//		if (mSleepCounter >= (SLEEP_DURATION + WAKE_UP_DURATION)) {
//			mSleepCounter = 0;
//		}
//	}
//	else if (isDestroyingBlock() && getLevel().getHitResult().isTile()) {
//		swing();
//	}
//
//	mNoPhysics = mAbilities.mNoclip;
//
//	const Vec3 exhaustionBasePos = getPos();
//
//	Mob::normalTick();
//
//	_applyExhaustion(getPos() - exhaustionBasePos);
//	
//	moveCape();
//
//	if (!getLevel().isClientSide()) {
//		// suppress fire in creative, unless player is currently touching lava
//		if (isOnFire() && mAbilities.mInvulnerable && !isInLava()) {
//			mOnFire = 0;
//		}
//	}
//
//	if (!isLocalPlayer()) {
//		std::string label;
//		//update the interact text
//		ItemInstance* interactingItem = getCarriedItem();
//
//		Entity* interactingEntity = getLevel().fetchEntity(mInteractTarget);//getLevel().fetchEntity((EntityUniqueID)mEntityData.getInt64(enum_cast(EntityDataIDs::INTERACT_ENTITY)));
//		if (interactingEntity) {
//			EntityInteraction interaction(true);
//			if (interactingEntity->getInteraction(*this, interaction)) {
//				label = interaction.getInteractText();
//			}
//		}
//		else if (interactingItem) {
//			label = interactingItem->getItem()->getInteractText(*this);
//		}
//		mEntityData.set(enum_cast(EntityDataIDs::INTERACT_TEXT), label);
//	}
//
//	mCompassSpriteCalc.update(*this);
//	mClockSpriteCalc.update(*this);
//
//	// if we're moving down but still have the mBounce flag set, 
//	//	  we must have just started down (reached the peak or hit something) so send the Bounce event & clear the flag
//	if ((mPosDelta.y < 0) && isBouncing() && mPos.y - mBounceStartPos.y >= 1) {
//		int bounceHeight = (int)trunc(mPos.y - mBounceStartPos.y);
//		getEventing()->fireEventPlayerBounced(this, mBounceBlock, bounceHeight);
//		mBounceBlock = {};
//	}
//
//	mPositionMode = PositionMode::Normal;
//}
//
//void Player::moveCape() {
//	mCapePosO = mCapePos;
//
//	Vec3 capeOffset = mPos - mCapePos;
//
//	float m = 10.0f;
//	if (capeOffset.x > m || capeOffset.x < -m) {
//		mCapePos.x = mPos.x;
//		mCapePosO.x = mCapePos.x;
//	}
//	if (capeOffset.z > m || capeOffset.x < -m) {
//		mCapePos.z = mPos.z;
//		mCapePosO.z = mCapePos.z;
//	}
//	if (capeOffset.y > m || capeOffset.x < -m) {
//		mCapePos.y = mPos.y;
//		mCapePosO.y = mCapePos.y;
//	}
//
//	mCapePos += capeOffset * 0.25f;
//}
//
//void Player::onBounceStarted(const BlockPos& bounceStartPos, const FullBlock& block) {
//	mBounceStartPos = bounceStartPos;
//	mBounceBlock = block;
//
//	Mob::onBounceStarted(bounceStartPos, block);
//}
//
//void Player::_tickCooldowns() {
//	for (auto& cooldown : mCooldowns) {
//		if (cooldown > 0) {
//			--cooldown;
//		}
//	}
//}
//
//int32_t Player::getPlayerIndex() const {
//	return mEntityData.getInt(enum_cast(EntityDataIDs::PLAYER_INDEX));
//}
//
//void Player::setPlayerIndex(int32_t index) {
//	mEntityData.set(enum_cast(EntityDataIDs::PLAYER_INDEX), (SynchedEntityData::TypeInt)index);
//}
//
//bool Player::isBouncing() const {
//	return mBounceBlock != FullBlock::AIR;
//}
//
////
//// Use items
////
//bool Player::isUsingItem() const {
//	return !mItemInUse.isNull();
//}
//
//ItemInstance* Player::getItemInUse() {
//	return &mItemInUse;
//}
//
//void Player::startUsingItem(ItemInstance instance, int duration) {
//	if(instance == mItemInUse) {
//		return;
//	}
//	mItemInUse = instance;
//	mItemInUseDuration = duration;
//	if(!getLevel().isClientSide()) {
//		setStatusFlag(EntityFlags::USINGITEM, true);
//	}
//}
//
//void Player::stopUsingItem() {
//	//If we're still carrying the same item and the aux value has changed from using it, update the inventory item.
//	if(getCarriedItem() != nullptr && mItemInUse.sameItem( getCarriedItem()) && mItemInUse.getAuxValue() != getCarriedItem()->getAuxValue() && !mItemInUse.getItem()->uniqueAuxValues()) {
//		getCarriedItem()->setAuxValue(mItemInUse.getAuxValue());
//	}
//	mItemInUse.setNull();
//	mItemInUseDuration = 0;
//	if(!getLevel().isClientSide()) {
//		setStatusFlag(EntityFlags::USINGITEM, false);
//	}
//}
//
//void Player::releaseUsingItem() {
//	if(!mItemInUse.isNull()) {
//		mItemInUse.releaseUsing(this, mItemInUseDuration);
//	}
//	stopUsingItem();
//}
//
//void Player::feed(int itemId) {
//	EntityEventPacket p(getRuntimeID(), EntityEvent::FEED, itemId);
//	getLevel().getPacketSender()->send(p);
//}
//
//void Player::completeUsingItem() {
//	if(!mItemInUse.isNull()) {
//		feed(mItemInUse.getItem()->getId());
//
//		// Check if the item is valid, and if we should overwrite the
//		// inventory item afterwards.
//		ItemInstance* selected = getSelectedItem();
//		bool doOverwrite = selected && ItemInstance::matches(&mItemInUse, selected);
//
//		mItemInUse.useTimeDepleted(&getLevel(), this);
//		if (selected) {
//			selected->startCoolDown(this);
//		}
//
//		//we shouldn't attempt to overwrite a fixed inventory item
//		if (doOverwrite && getSupplies().getSelectedContainerId() != CONTAINER_ID_FIXEDINVENTORY) {
//			*selected = mItemInUse;
//			if (selected->isEmptyStack()) {
//				auto selectedSlot = getSupplies().getSelectedSlot();
//				mInventoryProxy->clearSlot(selectedSlot.mSlot, selectedSlot.mContainerId);
//			}
//		}
//
//		stopUsingItem();
//	}
//}
//
//void Player::useItem(ItemInstance& instance) const {
//	if (!mAbilities.mInstabuild) {
//		instance.remove(1);
//	}
//}
//
//int Player::getItemUseDuration() {
//	return mItemInUseDuration;
//}
//
//int Player::getTicksUsingItem() {
//	if(isUsingItem()) {
//		return mItemInUse.getMaxUseDuration() - mItemInUseDuration;
//	}
//	return 0;
//}
//
////TODO: Someday, gameplay constants should probably come from a user/designer tunable json
//const float SPRINT_FLYING_MODIFIER = 2.0f;
//
//void Player::travel(float xa, float ya) {
//	float preX = mPos.x, preY = mPos.y, preZ = mPos.z;
//
//	if (mAbilities.isFlying()) {
//		float ydo = mPosDelta.y;
//		float ofs = mFlyingSpeed;
//#ifdef DEBUG_STATS
//		mFlyingSpeed = DebugStats::instance.mFlyFast ? 6.0f * mAbilities.getFlyingSpeed() : mAbilities.getFlyingSpeed() * (isSprinting() ? SPRINT_FLYING_MODIFIER : 1.0f);
//#else
//		mFlyingSpeed = mAbilities.getFlyingSpeed() * (isSprinting() ? SPRINT_FLYING_MODIFIER : 1.0f);
//#endif
//		Mob::travel(xa, ya);
//		mPosDelta.y = ydo * 0.6f;
//		mFlyingSpeed = ofs;
//		mFallDistance = 0;
//
//		// In order to give control back to the player, we need to stop fall flying if they go into
//		// creative.
//		if (isGliding()) {
//			stopGliding();
//			if (mLevel->isClientSide()) {
//				getLevel().getPacketSender()->send(PlayerActionPacket(PlayerActionType::STOP_GLIDING, getRuntimeID()));
//			}
//		}
//	}
//	else if (isGliding()) {
//		Mob::travel(xa, ya);
//		if (mOnGround) {
//			stopGliding();
//			if (mLevel->isClientSide()) {
//				getLevel().getPacketSender()->send(PlayerActionPacket(PlayerActionType::STOP_GLIDING, getRuntimeID()));
//			}
//		}
//	}
//	else {
//		Mob::travel(xa, ya);
//	}
//
//	//call the local player movement stats check, if this is the local player
//	checkMovementStats(Vec3(mPos.x - preX, mPos.y - preY, mPos.z - preZ));
//}
//
//
//void Player::checkMovementStats(const Vec3& d) { 
//	//currently unused, but we may eventually want to collect stats for all player types, on both client and server
//	UNUSED_PARAMETER(d); 
//}
//
//
//void Player::_applyExhaustion(const Vec3& d) {
//	if (getLevel().isClientSide() || mRiding) {
//		return;
//	}
//	
//	const int totalDistance = Math::round(d.length() * 100.0f);
//	const int horizontalDistance = Math::round(Math::sqrt(d.x * d.x + d.z * d.z) * 100.0f);
//
//	if (isUnderLiquid(MaterialType::Water)) {
//		if (totalDistance > 0) {
//			causeFoodExhaustion(FoodConstants::EXHAUSTION_SWIM * totalDistance * .01f);
//		}
//	}
//	else if (isInWater()) {
//		if (horizontalDistance > 0) {
//			causeFoodExhaustion(FoodConstants::EXHAUSTION_SWIM * horizontalDistance * .01f);
//		}
//	}
//	else if (mOnGround) {
//		if (horizontalDistance > 0) {
//			if (isSprinting()) {
//				causeFoodExhaustion(FoodConstants::EXHAUSTION_SPRINT * horizontalDistance * .01f);
//			}
//			else {
//				causeFoodExhaustion(FoodConstants::EXHAUSTION_WALK * horizontalDistance * .01f);
//			}
//		}
//	}
//}
//
//void Player::handleJumpEffects() {
//	if (getLevel().isClientSide()) {
//		return;
//	}
//
//	//awardStat(Stats.jump);
//	if (isSprinting()) {
//		causeFoodExhaustion(FoodConstants::EXHAUSTION_SPRINT_JUMP);
//	}
//	else {
//		causeFoodExhaustion(FoodConstants::EXHAUSTION_JUMP);
//	}
//}
//
//void Player::jumpFromGround(){
//	Mob::jumpFromGround();
//	if (getLevel().isClientSide()) {
//		PlayerActionPacket p(PlayerActionType::START_JUMP, getRuntimeID());
//		mPacketSender.send(p);
//	}
//	else{
//		handleJumpEffects();
//	}
//}
//
//bool Player::isImmobile() const {
//	return Mob::isImmobile() || isSleeping();
//}
//
//void Player::deleteContainerManager() {
//	setContainerManager(nullptr);
//}
//
//void Player::resetPos(bool clearMore) {
//	mPosDelta.x = mPosDelta.y = mPosDelta.z = 0;
//	mRot.x = 0;
//	mInterpolation.stop();
//
//	if(!isSleeping()) {
//		mHeightOffset = DEFAULT_PLAYER_HEIGHT_OFFSET;
//		setSize(0.6f, 1.8f);
//		setInvisible(false);
//	}
//
//	mImmobile = false;
//
//	if (clearMore) {
//		removeAllEffects();
//
//		getMutableAttribute(SharedAttributes::HEALTH)->resetToMaxValue();
//		getMutableAttribute(Player::HUNGER)->resetToDefaultValue();
//		getMutableAttribute(Player::EXHAUSTION)->resetToDefaultValue();
//		getMutableAttribute(Player::SATURATION)->resetToDefaultValue();
//
//		setStatusFlag(EntityFlags::GLIDING, false);
//
//		resetPlayerLevel();
//		mScore = 0;
//
//		mOnFire = 0;
//		mDeathTime = 0;
//		mPlayerIsSleeping = false;
//		mFallDistance = 0.f;
//
//		//adors - solved in breathable component
//		//mAirSupply = TOTAL_AIR_SUPPLY;
//
//		if (!getLevel().isClientSide()) {
//			getLevel().broadcastEntityEvent(this, EntityEvent::SPAWN_ALIVE);
//		}
//	}
//}
//
//void Player::updateAi() {
//	updateAttackAnim();
//}
//
//void Player::aiStep() {
//	//tick the selected inventory for updating contained complex items
//	mInventoryProxy->tick();
//
//	mOBob = mBob;
//	// moved the super::aiStep() part to the local player
//
//	auto speed = getMutableAttribute(SharedAttributes::MOVEMENT_SPEED);
//	if (!getLevel().isClientSide()) {
//		speed->setDefaultValue(mAbilities.getWalkingSpeed(), enum_cast(AttributeOperands::OPERAND_CURRENT));
//	}
//	mFlyingSpeed = DEFAULT_FLY_SPEED;
//	if (isSprinting()) {
//		mFlyingSpeed += DEFAULT_FLY_SPEED * 0.3f;
//	}
//
//	setSpeed(speed->getCurrentValue());
//
//	float tBob = (float) Math::sqrt(mPosDelta.x * mPosDelta.x + mPosDelta.z * mPosDelta.z);
//
//	float tTilt = (float) Math::atan(-mPosDelta.y * 0.2f) * 15.f;
//	if (tBob > 0.1f) {
//		tBob = 0.1f;
//	}
//	if (!mOnGround || getHealth() <= 0) {
//		tBob = 0;
//	}
//	if (mOnGround || getHealth() <= 0) {
//		tTilt = 0;
//	}
//	mBob += (tBob - mBob) * 0.4f;
//
//	mTilt += (tTilt - mTilt) * 0.8f;
//
//	if (!getLevel().isClientSide() && getHealth() > 0) {
//		AABB pickupArea;
//		if (isRiding() && !getRide()->isRemoved()) {
//			// if the player is riding, also touch entities under the pig/horse
//			pickupArea = mBB.merge(getRide()->mBB).grow(Vec3(1, 0, 1));
//		} else {
//			pickupArea = mBB.grow(Vec3(1, 0.5f, 1));
//		}
//
//		auto& entities = mOwnedBlockSource->getEntities(this, pickupArea);
//
//		for (unsigned int i = 0; i < entities.size(); i++) {
//			Entity* e = entities[i];
//			if (!e->isRemoved()) {
//				_touch(*e);
//			}
//		}
//	}
//	updateTrackedBosses();
//}
//
//void Player::_touch(Entity& entity) {
//	if (isEntityRelevant(entity)) {
//		entity.playerTouch(*this);
//	}
//}
//
//int Player::getScore() {
//	return mScore;
//}
//
//void Player::die(const EntityDamageSource& source) {
//	Mob::die(source);
//
//	if(isRiding()) {
//		stopRiding(true, false);
//	}
//
//	getLevel().onPlayerDeath(*this, source);
//
//	this->setSize(0.2f, 0.2f);
//	setPos(mPos);
//	mPosDelta.y = 0.1f;
//
//	mInventoryProxy->dropAll(getLevel().isClientSide());
//	mInventoryProxy->clearInventory();
//	sendInventory();
//
//	if (source.isEntitySource()) {
//		mPosDelta.x = -(float) Math::cos((mHurtDir + mRot.y) * Math::DEGRAD) * 0.1f;
//		mPosDelta.z = -(float) Math::sin((mHurtDir + mRot.y) * Math::DEGRAD) * 0.1f;
//	}
//	else {
//		mPosDelta.x = mPosDelta.z = 0;
//	}
//}
//
//void Player::awardKillScore(Entity& victim, int score) {
//	mScore += score;
//}
//
//bool Player::isShootable() {
//	return true;
//}
//
//bool Player::isInCreativeMode() {
//	return mPlayerGameType == GameType::Creative;
//}
//
//bool Player::isCreativeModeAllowed() {
//	return true;
//}
//
//bool Player::isInTrialMode() {
//	return false;
//}
//
//std::vector<const ItemInstance*> Player::getAllHand() const {
//	std::vector<const ItemInstance*> allWeapon;
//	allWeapon.reserve(enum_cast(HandSlot::_count));
//
//	auto mainHand = getCarriedItem();
//	if (mainHand != nullptr && !mainHand->isNull()) {
//		allWeapon.push_back(mainHand);
//	}
//
//	if (!mHand[enum_cast(HandSlot::Offhand)].isNull()) {
//		allWeapon.push_back(&mHand[enum_cast(HandSlot::Offhand)]);
//	}
//
//	return allWeapon;
//}
//
//std::vector<ItemInstance*> Player::getAllHand() {
//	std::vector<ItemInstance*> allWeapon;
//	allWeapon.reserve(enum_cast(HandSlot::_count));
//
//	auto mainHand = getCarriedItem();
//	if (mainHand != nullptr && !mainHand->isNull()) {
//		allWeapon.push_back(mainHand);
//	}
//
//	if (!mHand[enum_cast(HandSlot::Offhand)].isNull()) {
//		allWeapon.push_back(&mHand[enum_cast(HandSlot::Offhand)]);
//	}
//
//	return allWeapon;
//}
//
//std::vector<const ItemInstance*> Player::getAllEquipment() const {
//	std::vector<const ItemInstance*> allEquipment;
//	allEquipment.reserve(enum_cast(EquipmentSlot::_count));
//
//	auto mainHand = getCarriedItem();
//	if (mainHand != nullptr && !mainHand->isNull()) {
//		allEquipment.push_back(mainHand);
//	}
//
//	if (!mHand[enum_cast(HandSlot::Offhand)].isNull()) {
//		allEquipment.push_back(&mHand[enum_cast(HandSlot::Offhand)]);
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
//std::vector<ItemInstance*> Player::getAllEquipment() {
//	std::vector<ItemInstance*> allEquipment;
//	allEquipment.reserve(enum_cast(EquipmentSlot::_count));
//
//	auto mainHand = getCarriedItem();
//	if (mainHand != nullptr && !mainHand->isNull()) {
//		allEquipment.push_back(mainHand);
//	}
//
//	if (!mHand[enum_cast(HandSlot::Offhand)].isNull()) {
//		allEquipment.push_back(&mHand[enum_cast(HandSlot::Offhand)]);
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
//bool Player::add(ItemInstance& item) {
//	return getSupplies().add(item);
//}
//
//float Player::getDestroySpeed(const Block& block) const {
//	// digging bonus enchant is applied to destroy speed when inventory queries held item with the targeted block
//	float speed = _getItemDestroySpeed(block);
//
//	if (hasEffect(*MobEffect::DIG_SPEED)) {
//		speed *= 1.0f + (getEffect(*MobEffect::DIG_SPEED)->getAmplifier() + 1) * .2f;
//	}
//	if (hasEffect(*MobEffect::DIG_SLOWDOWN)) {
//		speed *= std::pow(0.3f, getEffect(*MobEffect::DIG_SLOWDOWN)->getAmplifier() + 1);
//	}
//
//	if (mRiding) {
//		if (!mRiding->mOnGround) {
//			speed /= 5;
//		}
//	}
//	else if (!mOnGround && !mAbilities.isFlying()) {
//		speed /= 5;
//	}
//
//	if (isUnderLiquid(MaterialType::Water) && EnchantUtils::getRandomItemWith(Enchant::Type::WATER_AFFINITY, *this, EquipmentFilter::ARMOR) == nullptr) {
//		speed /= 5;
//	}
//
//	return speed;
//}
//
//float Player::getDestroyProgress(const Block& block) const {
//	if (block.getDestroySpeed() < 0) {
//		return 0;
//	}
//
//	if (!canDestroy(block)) {
//		return getDestroySpeed(block) / block.getDestroySpeed() / 100.0f;
//	}
//
//	return (getDestroySpeed(block) / block.getDestroySpeed()) / 30.0f;
//}
//
//CommandPermissionLevel Player::getPermissions() {
//	return mAbilities.getCommandPermissions();
//}
//
//void Player::setPermissions(CommandPermissionLevel permissions) {
//	mAbilities.setCommandPermissions(permissions);
//}
//
//bool Player::canDestroy(const Block& block) const {
//	if (block.getMaterial().isAlwaysDestroyable()) {
//		return true;
//	}
//
//	ItemInstance* item = getSelectedItem();
//	if (item != nullptr) {
//		return item->canDestroySpecial(&block);
//	}
//	return false;
//}
//
////@SuppressWarnings("unchecked")
//void Player::readAdditionalSaveData(const CompoundTag& tag) {
//	Mob::readAdditionalSaveData(tag);
//
//	if (tag.contains("Inventory", Tag::Type::List)) {
//		auto inventoryList = tag.getList("Inventory");
//		mInventoryProxy->load(*inventoryList);
//
//		if (tag.contains("SelectedInventorySlot") && tag.contains("SelectedContainerId")) {
//			mInventoryProxy->selectSlot(tag.getInt("SelectedInventorySlot"), static_cast<ContainerID>(tag.getInt("SelectedContainerId")));
//		}
//	}
//
//	if(tag.contains("Sleeping") && tag.contains("SleepTimer")
//		&& tag.contains("BedPositionX") && tag.contains("BedPositionY") && tag.contains("BedPositionZ")) {
//		mPlayerIsSleeping = tag.getBoolean("Sleeping");
//		mSleepCounter = tag.getShort("SleepTimer");
//		mBedPosition = BlockPos(tag.getInt("BedPositionX"), tag.getInt("BedPositionY"), tag.getInt("BedPositionZ"));
//	}
//	else {
//		mPlayerIsSleeping = false;
//		mBedPosition = BlockPos(0, 0, 0);
//	}
//
//	if(!mPlayerIsSleeping) {
//		stopSleepInBed(true, true);
//		mEntityData.clearFlag<PlayerFlagIDType>(enum_cast(EntityDataIDs::PLAYER_FLAGS), PLAYER_SLEEP_FLAG);
//	}
//	else {
//		mPlayerIsSleeping = false;
//		mEntityData.setFlag<PlayerFlagIDType>(enum_cast(EntityDataIDs::PLAYER_FLAGS), PLAYER_SLEEP_FLAG);
//	}
//	mEntityData.set<BlockPos>(enum_cast(EntityDataIDs::BED_POSITION), mBedPosition);
//
//	if (tag.contains("SpawnX") && tag.contains("SpawnY") && tag.contains("SpawnZ")) {
//		mRespawnPosition = BlockPos(tag.getInt("SpawnX"), tag.getInt("SpawnY"), tag.getInt("SpawnZ"));
//		mIsForcedRespawnPos = tag.getBoolean("SpawnForced");
//	}
//
//	if (tag.contains("PlayerLevel") && tag.contains("PlayerLevelProgress")) {
//		getMutableAttribute(Player::LEVEL)->setDefaultValue(static_cast<float>(tag.getInt("PlayerLevel")), enum_cast(AttributeOperands::OPERAND_CURRENT));
//		getMutableAttribute(Player::EXPERIENCE)->setDefaultValue(tag.getFloat("PlayerLevelProgress"), enum_cast(AttributeOperands::OPERAND_CURRENT));
//		getMutableAttribute(Player::LEVEL)->setDefaultValue(0, enum_cast(AttributeOperands::OPERAND_MIN));
//		getMutableAttribute(Player::EXPERIENCE)->setDefaultValue(0, enum_cast(AttributeOperands::OPERAND_MIN));
//
//		// Force recalculation of amount of XP needed to level up - otherwise,
//		// the value for level 0 will be used no matter what your actual level
//		// is until the first time you level up in this session.
//		mPlayerLevelChanged = true;
//		getXpNeededForNextLevel();
//	}
//
//	mAbilities.loadSaveData(tag);
//
//	if (tag.contains("RideID")) {
//		mPendingRideID = EntityUniqueID(tag.getInt64("RideID"));
//	}
//
//	if (tag.contains("EnchantmentSeed")) {
//		mEnchantmentSeed = tag.getInt("EnchantmentSeed");
//	}
//
//	if (tag.contains("DimensionId")) {
//		int dimensionIdValue = tag.getInt("DimensionId");
//		if (dimensionIdValue >= 0 && dimensionIdValue < DimensionId::Count) {
//			mDimensionId = static_cast<DimensionId>(dimensionIdValue);
//		}
//	}
//
//	if (tag.contains("EnderChestInventory", Tag::Type::List)) {
//		auto inventoryList = tag.getList("EnderChestInventory");
//		mEnderChestInventory->load(*inventoryList);
//	}
//	
//	if (tag.contains("PlayerGameMode")) {
//		mPlayerGameType = static_cast<GameType>(tag.getInt("PlayerGameMode"));
//	}
//	else {
//		mPlayerGameType = getLevel().getLevelData().getGameType();
//	}
//	setPlayerGameType(mPlayerGameType);
//
//	if (tag.contains("AgentID")) {
//		mAgentID = EntityUniqueID(tag.getInt64("AgentID"));
//	}
//}
//
//void Player::addAdditionalSaveData(CompoundTag& entityTag) {
//	Mob::addAdditionalSaveData(entityTag);
//	entityTag.put("Inventory", mInventoryProxy->save());
//
//	PlayerInventoryProxy::SlotData selectedSlot = mInventoryProxy->getSelectedSlot();
//	entityTag.putInt("SelectedInventorySlot", selectedSlot.mSlot);
//	entityTag.putInt("SelectedContainerId", selectedSlot.mContainerId);
//
//	entityTag.putBoolean("Sleeping", isSleeping());
//	entityTag.putShort("SleepTimer", mSleepCounter);
//	entityTag.putInt("BedPositionX", mBedPosition.x);
//	entityTag.putInt("BedPositionY", mBedPosition.y);
//	entityTag.putInt("BedPositionZ", mBedPosition.z);
//
//	entityTag.putInt("SpawnX", mRespawnPosition.x);
//	entityTag.putInt("SpawnY", mRespawnPosition.y);
//	entityTag.putInt("SpawnZ", mRespawnPosition.z);
//	entityTag.putBoolean("SpawnForced", mIsForcedRespawnPos);
//
//	entityTag.putInt("EnchantmentSeed", mEnchantmentSeed);
//
//	//players need to look for their ride when logging in again
//	if (isRiding()) {
//		entityTag.putInt64("RideID", mRiding->getUniqueID().rawID );
//	}
//	
//	entityTag.putInt("PlayerGameMode", static_cast<int>(mPlayerGameType));
//	entityTag.putInt("PlayerLevel", getPlayerLevel());
//	entityTag.putFloat("PlayerLevelProgress", getLevelProgress());
//	entityTag.putInt("DimensionId", mRespawnDimensionId);
//	entityTag.put("EnderChestInventory", mEnderChestInventory->save());
//	
//	if (mAgentID) {
//		entityTag.putInt64("AgentID", mAgentID.rawID);
//	}
//
//	mAbilities.addSaveData(entityTag);
//}
//
//void Player::startCrafting(const BlockPos& pos, bool workbench) {
//}
//
//void Player::startStonecutting(const BlockPos& pos) {
//}
//
//void Player::openFurnace(int containerSize, const BlockPos& pos) {
//}
//
//void Player::openBrewingStand(int containerSize, const BlockPos& pos) {
//}
//
//void Player::openHopper(int containerSize, const BlockPos& pos) {
//}
//
//void Player::openHopper(int containerSize, const EntityUniqueID& entityUniqueID) {
//}
//
//void Player::openDispenser(int containerSize, const BlockPos& pos, bool dispenser) {
//}
//
//void Player::openPortfolio() {
//}
//
//void Player::openHorseInventory(int containerSize, const EntityUniqueID& uniqueID) {
//}
//
//void Player::openNpcInteractScreen(Entity& npc) {
//}
//
//void Player::openInventory() {
//}
//
//void Player::openChalkboard(ChalkboardBlockEntity& chalkboard) {
//
//}
//
//void Player::openEnchanter(int containerSize, const BlockPos& pos) {
//}
//
//void Player::openBeacon(int containerSize, const BlockPos& pos) {
//}
//
//void Player::openStructureEditor(const BlockPos& pos) {
//}
//
//bool Player::canOpenContainerScreen() {
//	if (isRiding() && EntityClassTree::isInstanceOf(*mRiding, EntityType::EquineAnimal) && !(static_cast<Horse*>(mRiding)->isTame())) {
//		return false;
//	}
//	// We can only push a container screen if we don't have an active container manager
//	return mContainerManager == nullptr;
//}
//
//void Player::take(Entity& e, int orgCount) {
//	TakeItemEntityPacket packet(e.getRuntimeID(), getRuntimeID());
//	mOwnedBlockSource->getDimension().sendBroadcast(packet);
//}
//
//float Player::getHeadHeight() const {	
//	if (isGliding()) {
//		return PLAYER_GLIDING_CAMERA_OFFSET;
//	}
//
//	return 0.12f;
//}
//
//float Player::getCameraOffset() const {
//	if (isGliding()) {
//		return PLAYER_GLIDING_CAMERA_OFFSET;
//	}
//
//	return 0.0f;
//}
//
//float Player::getRidingHeight() {
//	return mHeightOffset - 0.4f;
//}
//
//void Player::setDefaultHeadHeight() {
//	mHeightOffset = DEFAULT_PLAYER_HEIGHT_OFFSET;
//}
//
//bool Player::isHurt() {
//	return getHealth() > 0 && getHealth() < getMaxHealth();
//}
//
//bool Player::isHungry() const {
//	auto& hunger = getAttribute(Player::HUNGER);
//	return hunger.getCurrentValue() < hunger.getDefaultValue(enum_cast(AttributeOperands::OPERAND_CURRENT));
//}
//
//bool Player::_hurt(const EntityDamageSource& source, int damage, bool knock, bool ignite) {
//	if (mAbilities.mInvulnerable) {	//Creative players should not be able to be killed by void in creative mode
//		return false;
//	}
//	//Players aren't allowed to hurt players if pvp is off
//	if (source.getEntity() && (source.getEntity()->getEntityTypeId() == EntityType::Player || source.getEntity()->getOwnerEntityType() == EntityType::Player) && getLevel().getGameRules().getBool(GameRules::PVP) == false) {
//		return false;
//	}
//	if (source.getCause() == EntityDamageCause::Drowning && getLevel().getGameRules().getBool(GameRules::DROWNING_DAMAGE) == false) {
//		return false;
//	}
//	else if (((source.getCause() == EntityDamageCause::Fire || source.getCause() == EntityDamageCause::FireTick || source.getCause() == EntityDamageCause::Lava)
//		&& getLevel().getGameRules().getBool(GameRules::FIRE_DAMAGE) == false)) {
//		return false;
//	}
//	mNoActionTime = 0;
//	if (getHealth() <= 0) {
//		return false;
//	}
//
//	if(isSleeping() && !getLevel().isClientSide()) {
//		if (source.getCause() == EntityDamageCause::Suffocation) {
//			return false;
//		}
//		stopSleepInBed(true, true);
//	}
//	if (source.isEntitySource()) {
//		Entity& entity = *source.getEntity();
//		mLastHurtBy = entity.getEntityTypeId();
//
//		//	Make sure to only scale damage from mobs that aren't shooting us with a bow. Also don't scale damage from other players against us.
//		if( entity.hasCategory(EntityCategory::Mob) &&
//			!entity.hasCategory(EntityCategory::Player) &&
//			!(source.getCause() == EntityDamageCause::Projectile && EntityClassTree::isInstanceOf(*source.getDamagingEntity(), EntityType::Arrow)) ) {
//
//			if(getLevel().getAdventureSettings().noMvP) {
//				return false;
//			}
//
//			/*if(getLevel().getDifficulty() == Difficulty::Peaceful) {
//				damage = 0;
//			}
//			else if(getLevel().getDifficulty() == Difficulty::Easy) {
//				damage = damage / 2 + 1;
//			}
//			else if(getLevel().getDifficulty() == Difficulty::Hard) {
//				damage = damage * 3 / 2;
//			}*/
//		}
//	}
//
//	if (damage == 0) {
//		return false;
//	}
//
//	if (getArmor(ArmorSlot::Head) && (source.getCause() == EntityDamageCause::Anvil || source.getCause() == EntityDamageCause::FallingBlock)) {
//		ItemInstance helm = *getArmor(ArmorSlot::Head);
//		helm.hurtAndBreak((int)(damage * 4 + mRandom.nextFloat() * damage * 2.0f), this);
//		setArmor(ArmorSlot::Head, &helm);
//		damage = (int)(damage * 0.75f);
//	}
//
//	// If we are gliding and have hit a wall, stop gliding
//	if (source.getCause() == EntityDamageCause::FlyIntoWall) {
//		stopGliding();
//	}
//
//	return Mob::_hurt(source, damage, knock, ignite);
//}
//
//bool Player::interact(Entity& entity) {
//	EntityInteraction interaction;
//	if (entity.getInteraction(*this, interaction)) {
//		interaction.interact();
//		if (entity.hasCategory(EntityCategory::Mob)) {
//			((Mob&)entity).setPersistent();
//		}
//		return true;
//	}
//
//	ItemInstance* item = getSelectedItem();
//	if (item && entity.hasCategory(EntityCategory::Mob)) {
//		if (item->interactEnemy((Mob*)&entity, this)) {
//			if (item->isEmptyStack()) {
//				//item.snap(this);
//				auto selectedSlot = getSupplies().getSelectedSlot();
//				mInventoryProxy->clearSlot(selectedSlot.mSlot, selectedSlot.mContainerId);
//			}
//			return true;
//		}
//	}
//
//	return false;
//}
//
////void Player::swing() {
////LOGI("swinging: %d\n", swinging);
////	if (!swinging || swingTime >= 3 || swingTime < 0) {
////		swingTime = -1;
////		swinging = true;
////	}
//
////getLevel().raknetInstance->send(owner, new AnimatePacket(AnimatePacket::Swing, this));
////}
////}
//
//void Player::attack(Entity& entity) {
//	//	First we'll grab the players base Attack damage.
//	int dmg = (int)getMutableAttribute(SharedAttributes::ATTACK_DAMAGE)->getCurrentValue();
//
//	//	Then we'll add onto that the bonus the player gets from their held item.
//	dmg += getAttackDamage();
//
//	// MC:PE Rebalance: To make the game slightly easier on the touchpad, bare hands and non-weapons deal 2 points of
//	// damage instead of 1
//	if (dmg == 1) {
//		dmg = 2;
//	}
//
//	if (hasEffect(*MobEffect::DAMAGE_BOOST)) {
//		int count = getEffect(*MobEffect::DAMAGE_BOOST)->getAmplifier() + 1;
//
//		for (int i = 0; i < count; i++) {
//			dmg = (int)(dmg * 1.3f + 1.0f);
//		}
//	}
//	if (hasEffect(*MobEffect::WEAKNESS)) {
//		int count = getEffect(*MobEffect::WEAKNESS)->getAmplifier() + 1;
//
//		for (int i = 0; i < count; i++) {
//			dmg = (int)(dmg * 0.8f - .5f);
//			if (dmg < 0) {
//				dmg = 0;
//				break;
//			}
//		}
//	}
//
//	// melee weapon damage enchants
//	Mob* mobTarget = nullptr;
//	if(entity.hasCategory(EntityCategory::Mob)) {
//		mobTarget = static_cast<Mob*>(&entity);
//		if (mobTarget) {
//			dmg += EnchantUtils::getMeleeDamageBonus(*mobTarget, *this);
//			EnchantUtils::doPostDamageEffects(*mobTarget, *this);
//		}
//	}
//
//	int knockbackBonus = getMeleeKnockbackBonus();
//	if (isSprinting()) {
//		knockbackBonus += 1;
//	}
//
//	if (dmg > 0) {
//		bool didCrit = mFallDistance > 0 && !mOnGround && !onLadder() && !isInWater() && !hasEffect(*MobEffect::BLINDNESS) && !mRiding && entity.hasCategory(EntityCategory::Mob);
//		if (didCrit && dmg > 0) {
//			dmg = static_cast<int>(dmg * 1.5f);
//		}
//
//		ItemInstance* item = getSelectedItem();
//		EntityDamageByEntitySource source = EntityDamageByEntitySource(*this, EntityDamageCause::EntityAttack);
//
//		if (entity.hurt(source, dmg) && mobTarget) {
//			setLastHurtMob(mobTarget);
//			causeFoodExhaustion(FoodConstants::EXHAUSTION_ATTACK);
//
//			if (knockbackBonus > 0) {
//				if (mobTarget->getAttribute(SharedAttributes::KNOCKBACK_RESISTANCE).getCurrentValue() < 1.0f) {
//					entity.push(Vec3(-Math::sin(mRot.y * PI / 180) * knockbackBonus * 0.5f, 0.1f, Math::cos(mRot.y * PI / 180) * knockbackBonus * 0.5f));
//				}
//				mPosDelta.x *= 0.6f;
//				mPosDelta.z *= 0.6f;
//				setSprinting(false);
//			}
//
//			if (entity.hasCategory(EntityCategory::Player) && !((Player*)&entity)->getLevel().isClientSide() && entity.mHurtMarked) {
//				entity.sendMotionToServer();
//			}
//
//			if (didCrit) {
//				_crit(entity);
//			}
//
//			if(item && entity.hasCategory(EntityCategory::Mob)) {
//				if(!isCreative()) {
//					ItemInstance previous = *item;
//
//					item->hurtEnemy((Mob*)&entity, this);
//					
//					if(item->isEmptyStack()) {
//						auto selectedSlot = getSupplies().getSelectedSlot();
//						mInventoryProxy->clearSlot(selectedSlot.mSlot, selectedSlot.mContainerId);
//					}
//
//					bool isDifferent = previous != *item;
//
//					if (isDifferent && !isLocalPlayer()) {
//						ReplaceItemInSlotPacket packet(*item, getSupplies().getSelectedSlot().mSlot);
//						getRegion().getLevel().getPacketSender()->send(mOwner, packet);
//					}
//				}
//			}
//		}
//		else {
//			playSynchronizedSound(LevelSoundEvent::AttackNoDamage, entity.getAttachPos(EntityLocation::Body));
//		}
//
//		//	Test to see how much damage the player did with their attack, if it's greater than or equal to 9 hearts
//		//	(18 damage), than we'll want to fire off the achievement for Overkill.
//		if (dmg >= 18) {
//			MinecraftEventing::fireEventAwardAchievement(this, MinecraftEventing::AchievementIds::Overkill);
//		}
//	}
//}
//
//void Player::recheckSpawnPosition() {
//	Vec3 spawnPosition;
//
//	// Don't check spawn position in the wrong dimension, wait until
//	// the player's dimension has changed before running this logic.
//	if (getDimensionId() != mRespawnDimensionId) {
//		return;
//	}
//
//	if (!isRespawnReady() && (!mSpawned || !isAlive() || mDimensionState == DimensionState::WaitingRespawn)) {
//		
//		if (isAlive() && mDimensionState != DimensionState::WaitingRespawn) {
//			// Get the feet position of the player, otherwise it will put the player one block above where it should be.
//			spawnPosition = Vec3(mPos.x, mBB.min.y, mPos.z);
//		}
//		else {
//			// Otherwise the player is dead and we need to get the spawn position.
//			spawnPosition = BlockPos(hasRespawnPosition() ? getSpawnPosition() : getLevel().getSharedSpawnPos());
//		}
//
//		// center to the spawn chunk (to load it) before checking if the bed is invalid
//		auto chunk = mSpawnChunkSource->getAvailableChunk(spawnPosition);
//		if (!chunk) {
//			if (mSpawnChunkSource) {
//				mSpawnChunkSource->centerAt(spawnPosition);
//			}
//			return;
//		}
//
//		// Previous respawn has been invalidated (if force respawn is set, don't care)
//		if (!isAlive() && !isForcedRespawn()) {
//			if (hasRespawnPosition() && getSpawnPosition() != getLevel().getSharedSpawnPos()) {
//				if (!checkBed(true)) {
//					mBedPosition = Vec3::ZERO;
//					mRespawnPosition = getLevel().getSharedSpawnPos();
//					mRespawnBlocked = true;
//					return;
//				}
//			}
//		}
//	}
//	else {
//		// this was a teleport command, lets do it!
//		spawnPosition = getPos();
//	}
//
//	BlockSource spawnBlockSource(getLevel(), *getDimension(), *mSpawnChunkSource);
//	if (spawnPosition.y >= getDimension()->getHeight()) {
//		spawnPosition.y = (float)spawnBlockSource.getAboveTopSolidBlock((int)spawnPosition.x, (int)spawnPosition.z, true, true);
//	}
//
//	if (mFellWorldAsRespawn == true) {
//		mFellWorldAsRespawn = false;
//		spawnPosition.x += 5;
//		spawnPosition.y = (float)spawnBlockSource.getAboveTopSolidBlock((int)spawnPosition.x, (int)spawnPosition.z, true, true);
//	}
//
//	BlockPos blockSpawnPos(spawnPosition);
//	// Only perform this check if we are performing a respawn, not on first spawn.
//	if (mSpawned) {
//		if (!fixSpawnPosition(blockSpawnPos, { &spawnBlockSource, mOwnedBlockSource.get() })) {
//			spawnPosition = getLevel().getSharedSpawnPos();
//			spawnPosition.y = (float)spawnBlockSource.getAboveTopSolidBlock((int)spawnPosition.x, (int)spawnPosition.z, true, true);
//
//			mRespawnBlocked = true;
//		}
//		else { // Spawn was adjusted, so use the new spawn position.
//			spawnPosition = blockSpawnPos;
//		}
//	}
//	// Rare case where first spawn location is not safe
//	else {
//		// Do not allow the player to spawn in water or lava on first spawn, be fair, give tha boi a chance
//		if (fixStartSpawnPosition(blockSpawnPos, { &spawnBlockSource, mOwnedBlockSource.get() })) {
//			spawnPosition = blockSpawnPos;
//		}
//	}
//
//	// The heightOffset could be wrong in this case since the player could be dead,
//	// we therefore fall back to default.
//	setTeleportDestination(spawnPosition + Vec3(0.0f, DEFAULT_PLAYER_HEIGHT_OFFSET + FLT_EPSILON, 0.0f));
//	mPacketSender.send(mOwner, RespawnPacket(mTeleportDestPos));
//}
//
//void Player::sendMotionPacketIfNeeded() {
//
//}
//
//void Player::onMovePlayerPacketNormal(const Vec3& pos, const Vec2& rot) {
//	if (getRegion().hasBlock(pos)) {
//		lerpTo(pos, rot, 3);
//	}
//	else {
//		setPos(pos);
//		setRot(rot);
//	}
//}
//
//void Player::handleMovePlayerPacket(PositionMode mode, const Vec3& pos, const Vec2& rot, float yHeadRot) {
//	mPositionMode = mode;
//	switch (mode) {
//	case PositionMode::Normal:
//		onMovePlayerPacketNormal(pos, rot);
//		break;
//	case PositionMode::Respawn:
//		setPos(pos);
//		setRot(rot);
//		resetPos(false);
//		break;
//	case PositionMode::OnlyHeadRot:
//		mRot.x = rot.x;
//		break;
//	case PositionMode::Teleport:
//		teleportTo(pos);
//		break;
//	default:
//		DEBUG_FAIL("Invalid move mode");
//		break;
//	}
//	
//	mYHeadRot = yHeadRot;
//}
//
//bool Player::fixStartSpawnPosition(BlockPos& spawnPosition, std::vector<BlockSource*> regions) const {
//	AABB boundingBox = mBB;
//	boundingBox.move((spawnPosition.x - mBB.min.x), ((spawnPosition.y - 1) - mBB.min.y) + FLT_EPSILON, (spawnPosition.z - mBB.min.z));
//	BlockPos oldSpawn = spawnPosition;
//
//	// Lets try in each of north, south, east and west from the starting position going until we get an unloaded chunk.
//	const std::array<BlockPos, 4> offsets = {
//		BlockPos(CHUNK_WIDTH / 2, 0, 0),
//		BlockPos(-CHUNK_WIDTH / 2, 0, 0),
//		BlockPos(0, 0, CHUNK_WIDTH / 2),
//		BlockPos(0, 0, -CHUNK_WIDTH / 2)
//	};
//	int currentDirection = 0;
//
//	for (auto& region : regions) {
//		bool dangerBelow = false;
//		int maxAttempts = 0x10;
//
//		if (region->hasChunksAt(boundingBox)) {
//
//			do {
//				// Search below to check if player is spawning over a dangerous area
//				BlockPos belowSpawn = spawnPosition.below();
//
//				for (; belowSpawn.y >= 0; belowSpawn.y--) {
//					const Material& blockMat = region->getMaterial(belowSpawn);
//
//					// Avoid burning, drowning and long falls for start locations
//					if (blockMat.isType(MaterialType::Lava) || blockMat.isType(MaterialType::Water)) {
//						dangerBelow = true;
//
//						// Jump, jump around
//						spawnPosition.x += offsets[currentDirection].x;
//						spawnPosition.z += offsets[currentDirection].z;
//
//						// Wandered in to unloaded territory, this direction is hopeless, try another one
//						if (!region->getChunkAt(spawnPosition)) {
//							++currentDirection;
//							maxAttempts = 0x10;
//							spawnPosition = oldSpawn;
//							break;
//						}
//						spawnPosition.y = region->getAboveTopSolidBlock(spawnPosition.x, spawnPosition.z, true, true);
//						break;
//					}
//					// No danger below
//					else if (blockMat.isSolidBlocking()) {
//						dangerBelow = false;
//						break;
//					}
//				}
//
//			} while (dangerBelow && --maxAttempts > 0 && currentDirection < (int)offsets.size());
//			if (currentDirection >= (int)offsets.size()) {
//				break;
//			}
//		}
//
//		// If there's no danger we can spawn here
//		if (!dangerBelow) {
//			// Caller should use new value if this changed anything
//			return oldSpawn != spawnPosition;
//		}
//	}
//
//	return false;
//}
//
//bool Player::fixSpawnPosition( BlockPos& spawnPosition, std::vector<BlockSource*> regions ) const {
//	AABB boundingBox = mBB;
//	boundingBox.move((spawnPosition.x - mBB.min.x), ((spawnPosition.y - 1) - mBB.min.y) + FLT_EPSILON, (spawnPosition.z - mBB.min.z));
//
//	bool lavaBelow = false;
//
//	for(auto& region : regions) {
//		if(region->hasChunksAt(boundingBox)) {
//
//			// Search below to check if player is spawning over lava
//			BlockPos belowSpawn = spawnPosition.below();
//
//			for(; belowSpawn.y >= 0; belowSpawn.y--) {
//				const Material& blockMat = region->getMaterial(belowSpawn);
//
//				if(blockMat.isType(MaterialType::Lava)) {
//					lavaBelow = true;
//					break;
//				}
//				// No lava below
//				else if(blockMat.isSolidBlocking()) {
//					break;
//				}
//			}
//
//			// Search upwards for a clear place to spawn
//			// Don't put a spawn position over lava unless there's a block in the way
//			for(; boundingBox.min.y <= getDimension()->getHeight(); boundingBox.move(0, 1, 0)) {
//				bool containsLava = region->containsLiquid(boundingBox, MaterialType::Lava);
//				lavaBelow |= containsLava;
//
//				if(region->isSolidBlockingBlock(BlockPos(spawnPosition.x, int(boundingBox.min.y), spawnPosition.z))) {
//					lavaBelow = false;
//
//					// Solid block, so we can't spawn here anyway. Skip following AABB check
//					continue;
//				}
//
//				if(region->fetchAABBs(boundingBox, false).empty() && !lavaBelow && !containsLava) {
//					spawnPosition.y = (int)(boundingBox.min.y);
//					return true;
//				}
//			}
//		}
//	}
//	return false;
//}
//
//void Player::respawn() {
//	// Spawn XP orbs if we have not already after death and respawning
//	if (!mSpawnedXP && !getLevel().isClientSide() && getHealth() <= 0) {
//		ExperienceOrb::spawnOrbs(getRegion(), getPos(), getExperienceReward());
//		mSpawnedXP = true;
//	}
//
//	reset();
//	// Only clear other stats if we're dead
//	resetPos(getHealth() <= 0);
//
//	if(getHealth() > 0) {
//		mDeathTime = 0;
//	}
//
//	setPos(mTeleportDestPos);
//	mPosPrev = mTeleportDestPos;
//
//	if(mSpawned && getLevel().isClientSide()) {
//		PlayerActionPacket packet(PlayerActionType::RESPAWN, BlockPos(0, 0, 0), -1, getRuntimeID());
//		mPacketSender.send(packet);
//	}
//
//	if(!getLevel().isClientSide()) {
//		SetHealthPacket packet(getHealth());
//		mPacketSender.send(mOwner, packet);
//	}
//
//	mRespawnReady = false;
//	mRespawnBlocked = false;
//	mSpawnedXP = false;
//}
//
//void Player::dropEquipment(const EntityDamageSource& source, int lootBonusLevel) {
//	Mob::dropEquipment(source, lootBonusLevel);
//	sendInventory();
//}
//
////void Player::dropAllGear(int lootBonusLevel) {
////	for (auto i : range(enum_cast(ArmorSlot::_count))) {
////		const ItemInstance& item = armor[i];
////		if (ItemInstance::isArmorItem(&item)) {
////			drop(&item, false);
////		}
////		armor[i].setNull();
////	}
////}
//
//const ItemInstance* Player::getCarriedItem() const{
//	return getSupplies().getSelectedItem();
//}
//
//ItemInstance* Player::getCarriedItem() {
//	return getSupplies().getSelectedItem();
//}
//
//void Player::remove() {
//	setInvisible(true);
//	Mob::remove();
//}
//
//// For regular skeletons, creepers, and zombies, check if player is wearing corresponding mob head
//bool Player::isHiddenFrom(Mob& target) const{
//	EntityType type = target.getEntityTypeId();
//
//	if(type == EntityType::Skeleton || 
//		type == EntityType::WitherSkeleton ||
//		type == EntityType::Creeper ||
//		type == EntityType::Zombie) {
//
//		const ItemInstance *headGear = getArmor(ArmorSlot::Head);
//		const Item *item = headGear ? headGear->getItem() : nullptr;
//
//		if(item == Item::mSkull) {
//			int auxValue = headGear->getAuxValue();
//
//			return (type == EntityType::Skeleton && auxValue == enum_cast(SkullBlockEntity::SkullType::SKELETON)) ||
//				(type == EntityType::WitherSkeleton && auxValue == enum_cast(SkullBlockEntity::SkullType::WITHER)) ||
//				(type == EntityType::Creeper && auxValue == enum_cast(SkullBlockEntity::SkullType::CREEPER)) ||
//				(type == EntityType::Zombie && auxValue == enum_cast(SkullBlockEntity::SkullType::ZOMBIE));
//		}
//	}
//
//	return false;
//}
//
//void Player::setSpeed(float _speed) {
//	mSpeed = _speed;
//}
//
//bool Player::hasResource(int id) {
//	return mInventoryProxy->hasResource(id);
//}
//
//BlockPos Player::getSpawnPosition(){
//	return mRespawnPosition;
//}
//
//bool Player::isForcedRespawn() const {
//	return mIsForcedRespawnPos;
//}
//
//void Player::setRespawnPosition(const BlockPos& inRespawnPosition, bool forced) {
//	mRespawnPosition = inRespawnPosition;
//	mIsForcedRespawnPos = forced;
//	LOGI("Set spawn position to %d, %d, %d : Forced %s\n", mRespawnPosition.x, mRespawnPosition.y, mRespawnPosition.z, forced ? "True" : "False");
//	getLevel().saveGameData();
//
//	if(mSpawnChunkSource) {
//		mSpawnChunkSource->centerAt(mRespawnPosition);
//	}
//}
//
//void Player::setRespawnDimensionId(DimensionId respawnDimensionId) {
//	mRespawnDimensionId = respawnDimensionId;
//}
//
//void Player::openContainer(int containerSize, const BlockPos& pos){
//}
//
//void Player::openContainer(int containerSize, const EntityUniqueID& uniqueID) {
//}
//
//void Player::openAnvil(int containerSize, const BlockPos& pos) {
//}
//
//bool Player::canUseCarriedItemWhileMoving() {
//	const ItemInstance* item = mHandsBusy ? nullptr : getSupplies().getSelectedItem();
//	return item &&
//		   (item->isInstance( Item::mBow )
//		   ||  item->getItem()->isFood());
//}
//
//void Player::handleEntityEvent( EntityEvent id, int data) {
//	if (id == EntityEvent::USE_ITEM_COMPLETE) {
//		completeUsingItem();
//	}
//	else if (id == EntityEvent::ADD_PLAYER_LEVELS) {
//		_addLevels(data);
//	}
//	else {
//		Mob::handleEntityEvent(id, data);
//	}
//}
//
//void Player::sendInventory() const {
//	std::vector<ItemInstance> inventoryItems = mInventoryProxy->getSlotCopies(CONTAINER_ID_INVENTORY);
//	ContainerSetContentPacket containerPacket(CONTAINER_ID_INVENTORY, inventoryItems, INVENTORY_MAX_SELECTION_SIZE);
//
//	//push the selection to the player
//	for (int i = 0; i < mInventoryProxy->getLinkedSlotsCount(); ++i) {
//		containerPacket.mLinkedSlots.push_back(mInventoryProxy->getLinkedSlot(i));
//	}
//
//	mPacketSender.send(mOwner, containerPacket.setReliableOrdered());
//
//	//TODO - move this out to mob, implement packet handler that knows what to do with this
//	// Armor
//	std::vector<ItemInstance> armorItems(4);
//
//	for (int i = 0; i < enum_cast(ArmorSlot::_count); ++i) {
//		armorItems[i] = mArmor[i].isValid() ? mArmor[i] : ItemInstance(false);
//	}
//
//	ContainerSetContentPacket setContainerPacket(CONTAINER_ID_ARMOR, armorItems, 0);
//	mPacketSender.send(mOwner, setContainerPacket.setReliableOrdered());
//
//	PlayerInventoryProxy::SlotData selectedSlot = mInventoryProxy->getSelectedSlot();
//	ContainerSetSlotPacket selectedSlotPacket(selectedSlot.mContainerId, 0, ItemInstance(), selectedSlot.mSlot, ContainerSetSlotPacket::SetSlotPacketType::SetSelectedSlot);
//	mPacketSender.send(mOwner, selectedSlotPacket.setReliableOrdered());
//}
//
//void Player::sendInventorySlot(int slot) const {
//	if (slot < 0 || slot >= mInventoryProxy->getLinkedSlotsCount()) {
//		return;
//	}
//	ItemInstance* pitem = getSupplies().getItem(getSupplies().getLinkedSlot(slot), CONTAINER_ID_INVENTORY);
//	ItemInstance item = (pitem == nullptr) ? ItemInstance() : *pitem;
//	ContainerSetSlotPacket containerPacket(CONTAINER_ID_INVENTORY, slot, item);
//	mPacketSender.send(mOwner, containerPacket.setReliableOrdered());
//}

ItemInstance* Player::getSelectedItem() const {
// 	return mInventoryProxy->getSelectedItem();
	return nullptr;
}
//void Player::openTextEdit(BlockEntity* blockEntity) {
//	// Do nothing, implemented on LocalPlayer
//}
//
//float Player::_getItemDestroySpeed(const Block& block) const {
//	float speed = 0;
//	ItemInstance* item;
//	item = getSelectedItem();
//	speed = item ? item->getDestroySpeed(&block) : 1.0f;
//
//	return speed;
//}
//
//int Player::getAttackDamage() {
//	int dmg = 0;
//	ItemInstance* item;
//	item = getSelectedItem();
//	dmg = item ? item->getAttackDamage() : 0;
//
//	return dmg;
//}
//
//bool Player::isLocalPlayer() const {
//	return false;
//}
//
//int Player::tickWorld(const Tick& currentTick) {
//	if (mUpdateMobs == false) {
//		return 0;
//	}
//
//	// Sync ComplexItem data to remote clients
//	if (!getLevel().isClientSide()) {
//		auto containerIds = getSupplies().getAllContainerIds();
//		for (auto& it : containerIds) {
//			auto items = getSupplies().getSlotCopies(it);
//			for (auto& currentItem : items) {
//				if (!currentItem.isNull() && currentItem.isItem()) {
//					if (currentItem.getItem()->isComplex()) {
//						auto packet = static_cast<ComplexItem*>(currentItem.getItem())->getUpdatePacket(currentItem, getRegion().getLevel(), *this);
//						if (packet != nullptr) {
//							getDimension()->sendBroadcast(*packet);
//						}
//					}
//				}
//			}
//		}
//	}
//
//	ChunkPos playerCPos(mPos);
//	int loaded = 0;
//
//	//for each offset, get the chunk and query if it's ticked
//	for(auto& offset : getLevel().getTickingOffsets()) {
//
//		auto lc = mOwnedBlockSource->getChunkSource().getExistingChunk(playerCPos + offset);
//		if(!lc) {
//			continue;
//		}
//
//		auto& s = lc->getState();
//		if(s >= ChunkState::Generated) {
//			++loaded;
//
//			if(s >= ChunkState::Loaded) {
//				++loaded;
//
//				//if loaded and not already ticked, tick it
//				if(!lc->wasTickedThisTick(currentTick)) {
//					if(!getLevel().isClientSide() && getLevel().getTearingDown()) {
//						lc->tickBlockEntities(getRegion());
//					}
//					else {
//						lc->tick(this, currentTick);
//					}
//				}
//			}
//		}
//	}
//
//	//tick itself as an entity
//	tick(getRegion());
//
//	if (!getLevel().isClientSide() && !isRespawnReady()) {
//		if (!mSpawned || getHealth() <= 0 || mDimensionState == DimensionState::WaitingRespawn) {
//			recheckSpawnPosition();
//		}
//	}
//
//	//if the player disconnected while in a ride, try to reconnect with it
//	const float MIN_RIDE_RECONNECT_DISTANCE = 10;
//
//	if (mSpawned && mPendingRideID) {
//		if (auto ride = getLevel().fetchEntity(mPendingRideID)) {
//			if (ride->canAddRider(*this) && ride->distanceTo(*this) < MIN_RIDE_RECONNECT_DISTANCE) {
//				startRiding(*ride);
//			}
//		}
//		mPendingRideID = {};
//	}
//
//	return loaded;
//}
//
//void Player::rideTick() {
//	float preYRot = mRot.y, preXRot = mRot.x;
//
//	// Player::travel doesn't get called while riding which would normally fire this event, so fire it while riding here
//	MinecraftEventing::fireEventPlayerTransform(*this);
//	Mob::rideTick();
//	mOBob = mBob;
//	mBob = 0;
//
////  checkRidingStatistiscs(x - preX, y - preY, z - preZ);
//
//	if(mRiding && mRiding->getEntityTypeId() ==  EntityType::Pig) {
//		mRot.x = preXRot;
//		mRot.y = preYRot;
//		mYBodyRot = ((Pig*)mRiding)->mYBodyRot;
//	}
//}
//
//Vec3 Player::getStandingPositionOnBlock(const BlockPos& pos) {
//	return pos.above().above() + Vec3(0, mHeightOffset, 0);
//}
//
//bool Player::isPushable() const {
//	return false;
//}
//
//bool Player::isPushableByPiston() const {
//	return true;
//}
//
//bool Player::hasRespawnPosition() const {
//	return mRespawnPosition.y >= 0;
//}
//
//int Player::getChunkRadius() const {
//	return mChunkRadius;
//}
//
//void Player::setChunkRadius(int chunkRadius) {
//	mChunkRadius = chunkRadius;
//#ifdef DEBUG_STATS
//	DebugStats::instance.setViewDistance(mChunkRadius);
//#endif
//
//	if (mChunkSource) {
//		mChunkSource->setRadius((mChunkRadius + 2) * CHUNK_WIDTH);
//	}
//}
//
//void Player::updateTeleportDestPos() {
//	mTeleportDestPos = hasRespawnPosition() ? getSpawnPosition() : getLevel().getSharedSpawnPos();
//}
//
//void Player::setTeleportDestination(const Vec3& newTeleportPos) {
//	mTeleportDestPos = newTeleportPos;
//	mRespawnReady = true;
//}
//
//void Player::setSkin(const std::string& skinId, const std::vector<uint8_t>& data) {
//	mSkin->updateSkin(skinId, data);
//	mEntityRendererId = ER_PLAYER_RENDERER;
//}
//
//SkinInfoData& Player::getSkin() const {
//	return *mSkin;
//}
//
//void Player::registerAttributes() {
//	mAttributes->registerAttribute(SharedAttributes::ATTACK_DAMAGE).setRange(2.0f, 2.0f, 2.0f);
//	mAttributes->registerAttribute(SharedAttributes::HEALTH).setRange(0.0f, 20.0f, 20.0f);
//	auto& hunger = mAttributes->registerAttribute(Player::HUNGER);
//	auto& exhaustion = mAttributes->registerAttribute(Player::EXHAUSTION);
//	mAttributes->registerAttribute(Player::SATURATION).setRange(0.0f, FoodConstants::START_SATURATION, static_cast<float>(FoodConstants::MAX_FOOD));
//	mAttributes->registerAttribute(Player::LEVEL).setDefaultValue(static_cast<float>(PLAYER_MAX_LEVEL), enum_cast(AttributeOperands::OPERAND_MAX));
//	mAttributes->registerAttribute(Player::EXPERIENCE).setDefaultValue(1.0f, enum_cast(AttributeOperands::OPERAND_MAX));
//
//	hunger.setRange(0.0f, static_cast<float>(FoodConstants::MAX_FOOD), static_cast<float>(FoodConstants::MAX_FOOD));	// Let's not data drive this for now
//	hunger.setDelegate(make_shared<HungerAttributeDelegate>(hunger, this));
//
//	exhaustion.setDefaultValue(static_cast<float>(FoodConstants::MAX_EXHAUSTION) + 1.0f, enum_cast(AttributeOperands::OPERAND_MAX));
//	exhaustion.setDelegate(make_shared<ExhaustionAttributeDelegate>(exhaustion));
//	exhaustion.registerListener(hunger);
//}
//
//int Player::getPortalCooldown() const {
//	return SharedConstants::TicksPerSecond / 2;
//}
//
//int Player::getPortalWaitTime() const {
//	return mAbilities.mInvulnerable ? 0 : SharedConstants::TicksPerSecond* 4;
//}
//
//void Player::setPlayerGameType(GameType gameType) {
//	const GameType oldGameType = mPlayerGameType;
//	mPlayerGameType = gameType;
//	//this is the rudimentary gamemode state machine -- it's terrible, and needs to be a fully connected graph with
//	//independent states and entry/exit methods
//	if ((getPlayerGameType() == GameType::CreativeViewer) ||
//		(getPlayerGameType() == GameType::SurvivalViewer)) {
//		//reset the SurvivalViewer position, if we didn't used to be in an SurvivalViewer-type mode
//		if ((oldGameType != GameType::CreativeViewer) && (oldGameType != GameType::SurvivalViewer)) {
//			mSurvivalViewerPosition = getInterpolatedPosition(1.0f);
//			LOGI("SET SurvivalViewer position: %.1f,%.1f,%.1f\n", mSurvivalViewerPosition.x, mSurvivalViewerPosition.y, mSurvivalViewerPosition.z);
//		}
//
//		mAbilities.mFlying = true;
//		mAbilities.mMayfly = true;
//		mAbilities.mInstabuild = true;
//		mAbilities.mInvulnerable = true;
//		mAbilities.mNoclip = true;
//		setInvisible(true);
//		//TODO: set Viewer abilities
//	}
//	else if (isCreative()) {
//		if ((oldGameType == GameType::CreativeViewer) || (oldGameType == GameType::SurvivalViewer)) {
//			setTeleportDestination(mSurvivalViewerPosition);
//			teleportTo(mSurvivalViewerPosition);
//			Vec3 debugPt = getInterpolatedPosition(1.0f);
//			LOGI("Teleport to SurvivalViewer position: %.1f,%.1f,%.1f -- from %.1f,%.1f,%.1f\n",
//				mSurvivalViewerPosition.x, mSurvivalViewerPosition.y, mSurvivalViewerPosition.z,
//				debugPt.x, debugPt.y, debugPt.z);
//			mAbilities.mFlying = false;
//			setInvisible(false);
//		}
//
//		mAbilities.mMayfly = true;
//		mAbilities.mInstabuild = true;
//		mAbilities.mInvulnerable = true;
//		mAbilities.mNoclip = false;
//		//TODO: unset Viewer abilities
//	}
//	else if (isSurvival()) {
//		if ((oldGameType == GameType::CreativeViewer) || (oldGameType == GameType::SurvivalViewer)) {
//			setTeleportDestination(mSurvivalViewerPosition);
//			teleportTo(mSurvivalViewerPosition);
//			Vec3 debugPt = getInterpolatedPosition(1.0f);
//			LOGI("Teleport to SurvivalViewer position: %.1f,%.1f,%.1f -- from %.1f,%.1f,%.1f\n",
//				mSurvivalViewerPosition.x, mSurvivalViewerPosition.y, mSurvivalViewerPosition.z,
//				debugPt.x, debugPt.y, debugPt.z);
//			setInvisible(false);
//		}
//
//		mAbilities.mFlying = false;
//		mAbilities.mMayfly = false;
//		mAbilities.mInstabuild = false;
//		mAbilities.mInvulnerable = false;
//		mAbilities.mNoclip = false;
//		//TODO: unset Viewer abilities
//	}
//	else{
//		DEBUG_FAIL("SetPlayerGameType: Unknown Game Type");
//	}
//}
//
//void Player::eat(const ItemInstance& instance) {
//
//	Item * item = instance.getItem();
//	FoodItemComponent * food = item->getFood();
//	DEBUG_ASSERT(item->getFood(), "Can't eat items that are *not* food buddy!");
//
//	// check for IronBelly achievement (starving & eating rotten flesh)
//	if ((getAttribute(Player::HUNGER).getCurrentValue() == 0)
//		&& (Item::mRotten_flesh->getId() == item->getId())) {
//		MinecraftEventing::fireEventAwardAchievement(this, MinecraftEventing::AchievementIds::IronBelly);
//	}
//
//	eat(food->getNutrition(), food->getSaturationModifier());
//	
//	MinecraftEventing::fireEventItemUsed(this, instance, MinecraftEventing::UseMethod::Eat);
//}
//
//void Player::eat(int hungerValue, float saturationModifier) {
//	auto hunger = getMutableAttribute(Player::HUNGER);
//	auto saturation = getMutableAttribute(Player::SATURATION);
//
//	hunger->addBuff(InstantaneousAttributeBuff(static_cast<float>(hungerValue)));
//	// Max out saturation at hunger's current value
//	saturation->setMaxValue(hunger->getCurrentValue());
//	saturation->addBuff(InstantaneousAttributeBuff(hungerValue * saturationModifier * 2.0f));
//}
//
//void Player::_crit(Entity& entity) {
//	if (!getLevel().isClientSide()) {
//		mPacketSender.send(AnimatePacket(AnimatePacket::CRITICAL_HIT, entity.getRuntimeID()));
//	}
//}
//
//void Player::causeFoodExhaustion(float exhaustionAmount) {
//	// Only update on server
//	if (getLevel().isClientSide() || isCreative()) {
//		return;
//	}
//
//	getMutableAttribute(Player::EXHAUSTION)->addBuff(InstantaneousAttributeBuff(exhaustionAmount));
//}
//
//int Player::getExperienceReward(void) const {
//	return std::min(getPlayerLevel() * 7, 100);
//}
//
//int Player::getEnchantmentSeed(void) const {
//	return mEnchantmentSeed;
//}
//
//void Player::getNewEnchantmentSeed(void) {
//	mEnchantmentSeed = mRandom.nextInt();
//}
//
//void Player::spawnExperienceOrb(const Vec3& pos, int count) {
//	if (!getLevel().isClientSide()) {
//		return;
//	}
//
//	SpawnExperienceOrbPacket packet(pos, count);
//	mPacketSender.send(packet);
//}
//
//void Player::lavaHurt() {
//	Mob::lavaHurt();
//	if (hasEffect(*MobEffect::FIRE_RESISTANCE)) {
//		if (!getLevel().isClientSide()) {
//			EventPacket packet(this, MinecraftEventing::AchievementIds::StayinFrosty);
//			sendEventPacket(packet);
//		}
//	}
//}
//
//void Player::addExperience(int xp) {
//	float progress = getLevelProgress() + (static_cast<float>(xp) / getXpNeededForNextLevel());
//
//	mScore += xp;
//
//	while (progress >= 1.0f) {
//		progress = (progress - 1) * getXpNeededForNextLevel();
//		addLevels(1);
//		progress /= getXpNeededForNextLevel();
//	}
//
//	auto experience = getMutableAttribute(Player::EXPERIENCE);
//	experience->resetToMinValue();
//	experience->addBuff(InstantaneousAttributeBuff(progress));
//}
//
//void Player::addLevels(int levels) {
//	if (getLevel().isClientSide()) {
//		EntityEventPacket p(getRuntimeID(), EntityEvent::ADD_PLAYER_LEVELS, levels);
//		getLevel().getPacketSender()->send(p);
//	}
//
//	_addLevels(levels);
//}
//
//void Player::_addLevels(int levels) {
//	if (getPlayerLevel() + levels < 0) {
//		resetPlayerLevel();
//	}
//	else {
//		getMutableAttribute(Player::LEVEL)->addBuff(InstantaneousAttributeBuff(static_cast<float>(levels)));
//	}
//
//	int playerLevel = getPlayerLevel();
//	if (levels > 0 && playerLevel % 5 == 0 && mLastLevelUpTime < mTickCount - SharedConstants::TicksPerSecond * 5.0f) {
//		float volume = playerLevel > 30 ? 1 : playerLevel / 30.0f;
//		getLevel().broadcastSoundEvent(getRegion(), LevelSoundEvent::LevelUp, getAttachPos(EntityLocation::Body), static_cast<int>(volume * 0.75f * std::numeric_limits<float>::max()));
//		mLastLevelUpTime = mTickCount;
//	}
//
//	mPlayerLevelChanged = true;
//}
//
//int Player::getPlayerLevel(void) const {
//	return static_cast<int>(getAttribute(Player::LEVEL).getCurrentValue());
//}
//
//float Player::getLevelProgress(void) const {
//	return getAttribute(Player::EXPERIENCE).getCurrentValue();
//}
//
//Dimension* Player::getDimension() const {
//	return getLevel().getDimension(getDimensionId());
//}
//
//Unique<Packet> Player::getAddPacket(){
//	return make_unique<AddPlayerPacket>(*this);
//}
//
//bool Player::isWorldBuilder() {
//	return mAbilities.isWorldBuilder();
//}
//
//int Player::getXpNeededForNextLevel(void) const {
//	// hold onto the previous computations and only recompute when player level changes
//	int result = mPreviousLevelRequirement;
//
//	if (mPlayerLevelChanged) {
//		// intentional int truncation -- minor hack that corresponds ranges of integers to a switch statement
//		int playerLevel = getPlayerLevel();
//		int index = playerLevel / 15;
//
//		switch (index) {
//		// level < 15
//		case 0:
//			result = 7 + playerLevel * 2;
//			break;
//
//		// level >= 15
//		case 1:
//			result = 7 + 30 + (playerLevel - 15) * 5;
//			break;
//
//		// level >= 30
//		default:
//			result = 7 + 30 + 15 * 5 + (playerLevel - 30) * 9;
//			break;
//		}
//
//		mPlayerLevelChanged = false;
//		mPreviousLevelRequirement = result;
//	}
//
//	return result;
//}
//
//std::string Player::filterValidUserName(const std::string& value) {
//	std::string validName;
//
//	if (value.length() >= MIN_NAME_LENGTH && value.length() <= MAX_NAME_LENGTH) {
//		for (auto i : range(value.length())) {
//			char c = value[i];
//			if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == '(' || c == ')' || c == ' ') {
//				validName.push_back(c);
//			}
//		}
//	}
//
//	return validName;
//}
//
//bool Player::hasOpenContainer() const {
//	return mContainerManager != nullptr;
//}
//
//std::weak_ptr<IContainerManager> Player::getContainerManager() {
//	return mContainerManager;
//}
//
//void Player::setContainerManager(std::shared_ptr<IContainerManager> manager) {
//	mContainerManager = manager;
//}
//
//std::weak_ptr<HudContainerManagerModel> Player::getHudContainerManagerModel() {
//	return mInventoryProxy->_getHudContainerManagerModel();
//}
//
//void Player::setEquippedSlot(ArmorSlot slot, int item, int auxValue) {
//	setEquippedSlot(slot, ItemInstance(item, 1, auxValue));
//}
//
//void Player::setEquippedSlot(ArmorSlot slot, const ItemInstance& item) {
//	mArmor[enum_cast(slot)] = item;
//}
//
//float Player::getLuck() {
//	return (float)getAttribute(SharedAttributes::LUCK).getCurrentValue();
//}
//
//
//bool Player::isValidUserName(const std::string& value) {
//	return filterValidUserName(value).empty() ? false : true;
//}
//
//void Player::resetPlayerLevel(void) {
//	getMutableAttribute(Player::LEVEL)->resetToMinValue();
//	getMutableAttribute(Player::EXPERIENCE)->resetToMinValue();
//
//	mLastLevelUpTime = 0;
//	mPlayerLevelChanged = false;
//	mPreviousLevelRequirement = 7;
//}
//
//MinecraftEventing* Player::getEventing() const {
//	return &(mLevel->getEventing());
//}
//
//void Player::sendEventPacket(const EventPacket& packet) const {
//	DEBUG_ASSERT(!getLevel().isClientSide(), "sendEventPacket should only be called from the server!");
//
//	getLevel().getPacketSender()->send(mOwner, packet);
//}
//
//PlayerInventoryProxy& Player::getSupplies() const {
//	return *mInventoryProxy;
//}
//
//bool Player::shouldUpdateBossGUIBinds() {
//	return mTrackedBossUpdatebinds;
//}
//
//bool Player::shouldUpdateBosGUIControls() {
//	return mTrackedBossUpdateControls;
//}
//
//void Player::updateTrackedBosses() {
//	//Due to the way entities are added and removed we may be tracking bosses that no longer exist. 
//	//e.g. EntityRemoved before BossRemoved on the network
//	//In that case just remove them. The server will add them back when they come back into range
//	std::vector<EntityUniqueID> mToBeRemoved;
//	for (auto& bossID : mTrackedBossIDs) {
//		auto entity = mLevel->fetchEntity(bossID);
//		if (!entity) {
//			mToBeRemoved.push_back(bossID);
//		}
//	}
//
//	for (auto& bossID : mToBeRemoved) {
//		unRegisterTrackedBoss(bossID);
//		mLevel->broadcastBossEvent(BossEventUpdateType::Remove, nullptr);
//	}
//}
//
//void Player::startGliding() {
//	setStatusFlag(EntityFlags::GLIDING, true);
//	setSize(mCurrentDescription->mBBDim->x, mCurrentDescription->mBBDim->x);
//}
//
//void Player::stopGliding() {
//	setStatusFlag(EntityFlags::GLIDING, false);
//	setSize(mCurrentDescription->mBBDim->x, mCurrentDescription->mBBDim->y);
//}
//
//void Player::onSuspension() {
//}
//
//void Player::_registerElytraLoopSound() {
//	//This should only be called from the constructor!
//	SoundPlayer& soundPlayer = getLevel().getSoundPlayer();
//	mElytraLoop = soundPlayer.registerLoop("elytra/elytra_loop",
//		[this](LoopingSoundState& state) {
//		state.volume = 0.0f;
//		state.pitch = 1.0f;
//		state.position = getPos();
//		float speed = mPosDelta.lengthSquared();
//		float scale = speed / 4.0f;
//		if (isGliding() && speed >= 0.01f) {
//			state.volume = Math::clamp(scale, 0.0f, 1.0f);
//		}
//		else {
//			state.volume = 0;
//		}
//	});
//}
//
//void Player::setAgent(Agent* agent) {
//	if (agent == nullptr) {
//		mAgentID = {};
//	}
//	else {
//		mAgentID = agent->getUniqueID();
//		agent->setOwner(this->getUniqueID());
//	}
//}
//
//Agent* Player::getAgent() const {
//	Entity* entity = getLevel().getAutonomousEntity(mAgentID);
//	if (entity) {
//		DEBUG_ASSERT(entity->getEntityTypeId() == EntityType::Agent, "should be Agent type. ");
//		return static_cast<Agent*>(entity);
//	}
//	return nullptr;
//}
//
//bool Player::canBePulledIntoVehicle() const {
//	return false;
//}
//
//void Player::fireDimensionChangedEvent(DimensionId destinationDimension) {
//	// Tamed animals will not follow the player into the end at this time.
//	if (destinationDimension == DimensionId::TheEnd) {
//		return;
//	}
//
//	// Find all mobs within 20 blocks of this entity
//	const EntityList& nearbyEntities = getRegion().getEntities(EntityType::Mob, mBB.grow(Vec3(20, 20, 20)), this);
//
//	for (Entity* e : nearbyEntities) {
//		Mob* mob = static_cast<Mob*>(e);
//		mob->onPlayerDimensionChanged(this, destinationDimension);
//	}
//}
//
//int Player::getDirection() const {
//	return Direction::getDirection(Math::sin(mRot.y * Math::DEGRAD), -Math::cos(mRot.y * Math::DEGRAD));
//}
//
//void replaceCurrentItem(Player& player, ItemInstance& newItem) {
//	ItemInstance *currentItem = player.getSelectedItem();
//	if (currentItem != nullptr) {
//		*currentItem = newItem;
//	}
//
//	if (!player.isLocalPlayer()) {
//		ReplaceItemInSlotPacket packet(newItem, player.getSupplies().getSelectedSlot().mSlot);
//		player.getRegion().getLevel().getPacketSender()->send(player.mOwner, packet);
//	}
//}
//
//void addItem(Player& player, ItemInstance& itemToAdd) {
//	if (player.isLocalPlayer()) {
//		if (!player.getSupplies().add(itemToAdd)) {
//			player.drop(itemToAdd, false);
//		}
//	}
//	else {
//		player.sendInventory();
//		AddItemPacket packet(&itemToAdd);
//		player.getRegion().getLevel().getPacketSender()->send(player.mOwner, packet);
//	}
//}
//
//void Player::startCooldown(Item* item) {
//	if (item->getCooldownType() != CooldownType::None) {
//		mCooldowns[enum_cast(item->getCooldownType())] = item->getCooldownTime();
//	}
//}
//
//int Player::getItemCooldownLeft(CooldownType type) const {
//	if (type == CooldownType::None) {
//		return 0;
//	}
//	return mCooldowns[enum_cast(type)];
//}
//
//bool Player::isItemInCooldown(CooldownType type) const {
//	return getItemCooldownLeft(type) > 0;
//}
