/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#include "Dungeons.h"

#include "world/level/Level.h"

#include "world/entity/Entity.h"
//#include "AppPlatformListener.h"
//#include "client/renderer/debug/BiomeMapDisplay.h"
//#include "client/sound/SoundPlayer.h"
#include "CommonTypes.h"
#include "Core/Utility/EnumCast.h"
#include "EntityUniqueID.h"
#include "LevelSettings.h"
#include "nbt/ListTag.h"
//#include "network/packet/LevelSoundEventPacket.h"
//#include "network/packet/BossEventPacket.h"
//#include "network/packet/ChangeDimensionPacket.h"
//#include "network/packet/EntityEventPacket.h"
//#include "network/packet/ExplodePacket.h"
//#include "network/packet/LevelEventPacket.h"
//#include "network/packet/MapInfoRequestPacket.h"
//#include "network/packet/MobArmorEquipmentPacket.h"
//#include "network/packet/MobEquipmentPacket.h"
//#include "network/packet/MovePlayerPacket.h"
//#include "network/packet/PlayerActionPacket.h"
//#include "network/packet/PlayerListPacket.h"
//#include "network/packet/RemoveEntityPacket.h"
//#include "network/packet/SetTimePacket.h"
//#include "network/packet/EventPacket.h"
//#include "network/packet/TextPacket.h"
//#include "network/PacketSender.h"
//#include "platform/AppPlatform.h"
//#include "platform/threading/WorkerPool.h"
#include "Core/Math/Color.h"
#include "SharedConstants.h"
//#include "server/commands/Command.h"
//#include "Events/MinecraftEventing.h"
#include "util/DebugStats.h"
//#include "util/PerfTimer.h"
#include "util/Random.h"
#include "util/StringUtils.h"
#include "world/Difficulty.h"
//#include "world/entity/ai/village/Villages.h"
//#include "world/entity/ai/goal/GoalDefinition.h"
#include "world/entity/Entity.h"

#include "world/entity/EntityClassTree.h"
//#include "world/entity/EntityDamageSource.h"
//#include "world/entity/EntityDefinitionGroup.h"
//#include "world/entity/EntityEvent.h"
//#include "world/entity/EntityFactory.h"
#include "world/entity/EntityTypes.h"
//#include "world/entity/monster/Zombie.h"
//#include "world/entity/ParticleType.h"
#include "world/entity/player/Player.h"
//#include "world/entity/projectile/ProjectileFactory.h"
//#include "world/entity/projectile/ThrownPotion.h"
#include "world/Facing.h"
//#include "world/inventory/BaseContainerMenu.h"
#include "world/level/biome/Biome.h"
#include "world/level/biome/BiomeSource.h"
#include "world/level/block/entity/BlockEntity.h"
#include "world/level/block/LevelEvent.h"
#include "world/level/block/LiquidBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/BlockSourceListener.h"
//#include "world/level/BlockTickingQueue.h"
#include "world/level/chunk/LevelChunk.h"
#include "world/level/chunk/MainChunkSource.h"
#include "world/level/chunk/NetworkChunkSource.h"
#include "world/level/dimension/Dimension.h"
//#include "world/level/Explosion.h"
#include "world/level/LevelConstants.h"
#include "world/level/LevelConstants.h"
#include "world/level/levelgen/flat/FlatLevelSource.h"
#include "world/level/levelgen/v1/HellRandomLevelSource.h"
#include "world/level/levelgen/v1/RandomLevelSource.h"
#include "world/level/LevelListener.h"
//#include "world/level/LightUpdate.h"
#include "world/level/material/Material.h"
//#include "world/level/Spawner.h"
#include "world/level/pathfinder/PathFinder.h"
//#include "world/level/PortalForcer.h"
#include "world/level/Seasons.h"
#include "world/level/levelgen/structure/StructureManager.h"
#include "world/level/storage/LevelData.h"
#include "world/level/storage/LevelStorage.h"
//#include "world/level/saveddata/maps/MapItemSavedData.h"
#include "world/level/storage/SavedDataStorage.h"
//#include "network/packet/ContainerSetContentPacket.h"
//#include "world/level/Tick.h"
#include "world/level/Weather.h"
//#include "world/Minecraft.h"
#include "world/phys/AABB.h"
#include "world/phys/HitResult.h"
//#include "world/redstone/circuit/CircuitSystem.h"
#include "util/StringByteInput.h"
#include "util/StringByteOutput.h"
#include "nbt/NbtIo.h"
#include "world/level/chunk/PlayerChunkSource.h"

//#include "world/entity/EntityDefinition.h"
//#include "world/entity/EntityFactory.h"

#include <random>
#include "util/ColorFormat.h"
const int MIN_TICKS_BEFORE_LIVE_SAVE = 20 * 3;
const int MAX_TICKS_WITHOUT_LIVE_SAVE = 20 * 15;

std::array<ChunkPos, 57> Level::tickingChunksOffset = { {
	{ -1, -4 }, { 0, -4 }, { 1, -4 }, { -2, -3 }, { -1, -3 }, { 0, -3 }, { 1, -3 }, { 2, -3 }, { -3, -2 },
	{ -2, -2 }, { -1, -2 }, { 0, -2 }, { 1, -2 }, { 2, -2 }, { 3, -2 }, { -4, -1 }, { -3, -1 }, { -2, -1 },
	{ -1, -1 }, { 0, -1 }, { 1, -1 }, { 2, -1 }, { 3, -1 }, { 4, -1 }, { -4, 0 }, { -3, 0 }, { -2, 0 }, { -1, 0 },
	{ 0, 0 }, { 1, 0 }, { 2, 0 }, { 3, 0 }, { 4, 0 }, { -4, 1 }, { -3, 1 }, { -2, 1 }, { -1, 1 }, { 0, 1 }, { 1, 1 },
	{ 2, 1 }, { 3, 1 }, { 4, 1 }, { -3, 2 }, { -2, 2 }, { -1, 2 }, { 0, 2 }, { 1, 2 }, { 2, 2 }, { 3, 2 }, { -2, 3 },
	{ -1, 3 }, { 0, 3 }, { 1, 3 }, { 2, 3 }, { -1, 4 }, { 0, 4 }, { 1, 4 }
}};


//std::string Level::createUniqueLevelID(int modifier) {
//	auto name = Util::toBase64(getEpochTimeS() ^ (((int64_t)getTimeMs()) << 32) ^ modifier);
//	std::replace(name.begin(), name.end(), '/', '-');
//	return name;
//}

RandomSeed Level::createRandomSeed() {
	std::uniform_int_distribution<RandomSeed> dist(0, std::numeric_limits<RandomSeed>::max());
	std::random_device rnd;
	return dist(rnd);
}

Level::Level(Unique<LevelStorage> levelStorage)
	: mLevelStorage(std::move(levelStorage))
	, mStructureManager(*(new StructureManager()))
{
}

//Level::Level(SoundPlayer& soundPlayer, Unique<LevelStorage> levelStorage, const std::string& levelName, const LevelSettings& settings, bool isClientSide, MinecraftEventing& eventing, ResourcePackManager& addOnResourcePackManager, StructureManager& structureManager)
//	: mLevelStorage(std::move(levelStorage))
//	, c(soundPlayer)
//	, mIsClientSide(isClientSide)
//	, mIsExporting(false)
//	, mVillages(make_unique<Villages>(this))
//	, mEntityDefinitions(make_unique<EntityDefinitionGroup>(addOnResourcePackManager))
//	, mRandom(settings.getSeed())
//	, mPortalForcer(make_unique<PortalForcer>(*this))
//	, mSavedDataStorage(make_unique<SavedDataStorage>(mLevelStorage.get()))
//	, mEventing(eventing)
//	, mStructureManager(structureManager) {
//	mMobSpawner = make_unique<Spawner>(*this, *mEntityDefinitions.get());
//	//EntityFactory::setDefinitionGroup(mEntityDefinitions.get());
//	mProjectileFactory = make_unique<ProjectileFactory>(*this);
//	//initListener(2.0f);
//
//	// Load villages
//	// Note: If load returns false, it means that 'villages' data was not saved for this level (no data to load).
//	//       In that case we will set this level's empty 'villages' inside savedDataStorage for future saving and
//	// access.
//	//if (!mSavedDataStorage->load(mVillages.get(), Villages::VILLAGE_FILE_ID)) {
//	//	mSavedDataStorage->set(Villages::VILLAGE_FILE_ID, mVillages.get());
//	//}
//
//	//AppPlatform::singleton().setSleepEnabled(false);
//
//	if (isClientSide || !mLevelStorage->loadLevelData(mLevelData)) {
//		// Calculate actual achievementsDisabled flag here, since this is where the level actually loads.
//		mLevelData = LevelData(settings, levelName, settings.getGenerator(), settings.getDefaultSpawn(),
//							   settings.hasAchievementsDisabled(), settings.isEduWorld(),
//							   settings.getRainLevel(), settings.getLightningLevel());
//	}
//
//	// Conversion of old respawn heights to "max", this ensures that, if we
//	// increase the allowed height, the max will always be an "invalid" value
//	// we can check for and not what could become a valid position.
//	auto& initialSpawn = mLevelData.getSpawn();
//	if (!isClientSide && (initialSpawn.y == 128 || initialSpawn.y == 256)) {
//		auto fixedAlready = mLevelStorage->loadData("LevelSpawnWasFixed");
//		if (fixedAlready.empty()) {
//			mLevelData.setSpawn(BlockPos(initialSpawn.x, LEVEL_SPAWN_HEIGHT, initialSpawn.z));
//			mLevelStorage->saveData("LevelSpawnWasFixed", "True");
//		}
//	}
//
//	mLevelData.setForceGameType(settings.forceGameType());
//	if(mLevelData.getForceGameType()){
//		mLevelData.setGameType(settings.getGameType());
//	}
//
//	// if cheats were enabled or if the game mode was set to creative before loading, the achievementsDisabled flag should now be set
//	if (mLevelData.achievementsWillBeDisabledOnLoad()) {
//		mLevelData.disableAchievements();
//	}
//
//
//	if (settings.shouldOverrideSavedSettings()) {
//		settings.overrideSavedSettings(mLevelData);
//	}
//
//	//if (!mSavedDataStorage->load(mPortalForcer.get(), PortalForcer::PORTAL_FILE_ID)) {
//	//	mSavedDataStorage->set(PortalForcer::PORTAL_FILE_ID, mPortalForcer.get());
//	//}
//
//	mLastUniqueID = EntityUniqueID((int64_t)mLevelData.getWorldStartCount() << 32);
//	mLevelData.recordStartUp();
//
//	mLevelData.setLevelName(levelName);
//
//	// Reset instance data because we may have left over biome settings from another level load
//	Biome::ResetInstanceData();
//
//	// Now load biome instance data
//	if (mLevelStorage) {
//		Biome::LoadInstanceData(*mLevelStorage);
//	}
//
//	if (mLevelStorage != nullptr) {
//		mLevelId = mLevelStorage->getLevelId();
//	}
//
//	loadAutonomousEntities();
//
//	mNightMode = false;
//}

Level::~Level() {
	//AppPlatform::singleton().setSleepEnabled(true);

	//LOGI("Destroying the Level...\n");

	//mTearingDown = true;

	//// If we're currently exporting and we don't have a LevelStorage yet
	////	sync up the disk thread to complete the export before we quit
	//if (mIsExporting == true && mLevelStorage == nullptr) {
	//	WorkerPool::getFor(WorkerRole::Disk).sync();
	//	DEBUG_ASSERT(mIsExporting == false, "Level exporting should be finished now");
	//}

	//saveGameData();
	//saveBiomeData();
	//mMapData.clear();

	//// looks odd, but you need to separate the flush and the sync.
	//for (auto&& queue : WorkerPool::getWorkersFor(WorkerRole::Streaming)) {
	//	// empty the work queue
	//	queue->flush();
	//}
	//for (auto&& queue : WorkerPool::getWorkersFor(WorkerRole::Streaming)) {
	//	// make sure than any currently executing work finishes
	//	queue->sync();
	//}

	//// could be holding the "to be destroyed" BlockSource
	//mLightUpdates.clear();
	//WorkerPool::getFor(WorkerRole::LightUpdate).flush(); // force any queued coroutines to flush - source is going away!

	//// now it is safe to clear these - all jobs are flushed
	//setNetEventCallback(nullptr);
	//setPacketSender(nullptr);


	//for (auto& r : mRegions) {	//remove all listeners from all regions
	//	for (auto & l : m_Listeners) {
	//		r->removeListener(*l);
	//	}
	//}

	////remove all listeners
	//m_Listeners.clear();

	////throw away all chunks owned by players
	//forEachPlayer([](Player& player) {
	//	player.destroyRegion();
	//	return true;
	//});

	////flush the coroutine (synchronous) queue too
	//WorkerPool::getFor(WorkerRole::MainThread).flush();

	////now actually remove the players
	//_removeAllPlayers();

	////release all dimensions AFTER releasing players, these own the chunk sources
	////need to clear the list before releasing its content because of callbacks looping over the list itself
	//auto dimensions = std::move(mDimensions);
	//dimensions.clear();

	////delete the storage if present
	//mLevelStorage.reset();

	////EntityFactory::clearDefinitionGroup();

	////remove chunk memory
	//LevelChunk::trimMemoryPool();
	//LevelChunk::SubChunk::trimMemoryPool();

}

//Dimension& Level::createDimension(DimensionId id) {
//	if (auto dim = getDimension(id)) {
//		return *dim;
//	}
//
//	Unique<Dimension> newDimension = Dimension::createNew(id, *this);
//	DEBUG_ASSERT(newDimension, "Failed to create dimension!");
//
//	auto& dimension = *newDimension;
//	dimension.init();
//
//	mDimensions.emplace(id, std::move(newDimension));
//
//	_loadDimensionStructures(id);
//
//	return dimension;
//}

Dimension* Level::getDimension(DimensionId id) const {

// 	DimensionMap::const_iterator dimensionIt = mDimensions.find(id);
// 	if (dimensionIt != mDimensions.end()) {
// 		return dimensionIt->second.get();
// 	}

	return nullptr;
}

//PortalForcer& Level::getPortalForcer() {
//	return *(mPortalForcer.get());
//}
//
////void Level::requestPlayerChangeDimension(Player& player, Unique<ChangeDimensionRequest> changeRequest) {
////
////	if (mChangeDimensionRequests.find(&player) == mChangeDimensionRequests.end()) {
////
////		player.mDimensionState = Player::DimensionState::Pending;
////		player.lerpMotion(Vec3(0.0f, 0.0f, 0.0f));
////
////		mChangeDimensionRequests.emplace(&player, std::move(changeRequest));
////	}
////
////	_handleChangeDimensionRequests();
////}
//
//void Level::entityChangeDimension(Entity& entity, DimensionId toId) {
//	//DimensionId fromId = entity.getDimensionId();
//	//if (fromId == toId) {
//	//	return;
//	//}
//
//	//if (entity.hasCategory(EntityCategory::Mob)) {
//	//	entity.lerpTo(Vec3::ZERO, Vec2::ZERO, 0);
//	//	Mob* mob = static_cast<Mob*>(&entity);
//	//	mob->dropLeash(true);
//	//}
//
//	//auto& toDimension = createDimension(toId);
//
//	//Vec3 targetPos = toDimension.translatePosAcrossDimension(entity.getPos(), fromId);
//	//entity.teleportTo(targetPos);
//
//	//Unique<CompoundTag> tag = make_unique<CompoundTag>();
//	//entity.save(*tag);
//
//	//entity.remove();
//
//	//toDimension.transferEntity(targetPos, std::move(tag));
//}
//
////bool Level::_playerChangeDimension(Player* player, ChangeDimensionRequest& changeRequest) {
////
////	DimensionId fromId = changeRequest.mFromDimensionId;
////	DimensionId toId = changeRequest.mToDimensionId;
////
////	if (changeRequest.mState == ChangeDimensionRequest::State::PrepareRegion) {
////
////		auto& toDimension = createDimension(toId);
////
////		Dimension* fromDimension = getDimension(fromId);
////		DEBUG_ASSERT(fromDimension != nullptr, "Level::_playerChangeDimension, failed to find from dimension.");
////
////		// wait for dimension
////		player->mDimensionState = Player::DimensionState::WaitingArea;
////
////		// release the chunks
////		player->suspendRegion();
////
////		// flush unsaved chunks before leaving
////		if (!mIsClientSide) {
////			fromDimension->mChunkSource->waitDiscardFinished();
////		}
////
////		if (player->isLocalPlayer()) {
////
////			// wait... make sure everyone (dirty chunk mgr) is done with the player's chunk source
////			for (auto&& queue : WorkerPool::getWorkersFor(WorkerRole::Streaming)) {
////				queue->sync();
////			}
////		}
////
////		// could be holding the "to be destroyed" BlockSource
////		mLightUpdates.clear();
////		WorkerPool::getFor(WorkerRole::LightUpdate).flush(); // force any queued coroutines to flush - source is going away!
////
////		// if using the portal (server) then changeRequest does not have an absolute position,
////		// use the portal to determine where to position the player.
////		if (!mIsClientSide && changeRequest.mUsePortal) {
////
////			// translate player position across dimension, this is not necessarily the final position
////			Vec3 targetPosition;
////			if (fromId == DimensionId::TheEnd) {				
////				targetPosition = player->hasRespawnPosition() ? player->getSpawnPosition() : getSharedSpawnPos();
////				player->mDimensionState = Player::DimensionState::WaitingRespawn;
////				changeRequest.mRespawn = true;
////			}
////			else {
////				targetPosition = toDimension.translatePosAcrossDimension(player->getPos(), fromId);
////			}
////
////			// try to find a portal for the exact position, otherwise use the raw translated pos.
////			BlockPos portalTargetBlockPos;
////			if (toId != DimensionId::TheEnd && fromId != DimensionId::TheEnd && mPortalForcer->findPortal(toDimension.getId(), targetPosition, PortalForcer::PORTAL_SEARCH_RADIUS, portalTargetBlockPos)) {
////				changeRequest.mPosition = Vec3(portalTargetBlockPos);
////			}
////			else {
////				changeRequest.mPosition = targetPosition;
////			}
////
////			// send portal use events
////			EventPacket packet(player, changeRequest.mFromDimensionId, changeRequest.mToDimensionId);
////			player->sendEventPacket(packet);
////		}
////
////		player->onPrepChangeDimension();
////
////		// We need to give global entities a valid block source if there are still players in the dimension.
////		// Find a player still in the dimension and assign their block source to the entity.
////		if (!mIsClientSide) {
////			bool foundPlayer = false;
////			forEachPlayer([&player, &fromId, this, &foundPlayer](Player& activePlayer) {
////				if (&activePlayer != player && activePlayer.getDimension()->getId() == fromId) {
////					for (auto&& entity : mGlobalEntities) {
////						if (entity->getDimension().getId() == fromId && &entity->getRegion() == &player->getRegion()) {
////							entity->setRegion(activePlayer.getRegion());
////						}
////					}
////
////					// We reassigned the region, so let's get out of here.
////					foundPlayer = true;
////					return false;
////				}
////				// Keep searching for a player in the dimension
////				return true;
////			});
////
////			// If there's no player left in the dimension, remove the global entities from that dimension.
////			// This can happen if we load in entities that are global, but their chunks were never loaded while the player was in the dimension.
////			if (!foundPlayer) {
////				std::vector<Entity*> removedGlobalEntities;
////
////				for (auto&& entity : mGlobalEntities) {
////					if (entity->getDimension().getId() == fromId && &entity->getRegion() == &player->getRegion()) {
////						removedGlobalEntities.emplace_back(entity);
////					}
////				}
////
////				for (auto&& entity : removedGlobalEntities) {
////					mGlobalEntities.erase(entity);
////				}
////			}
////
////		}
////
////		// release old region
////		player->destroyRegion();
////
////		
////		// move to new position in destination dimension, prevent interpolation
////		player->setPos(changeRequest.mPosition);
////		player->lerpTo(Vec3::ZERO, Vec2::ZERO, 0);
////
////		// server tells clients to go to new dimension before preparing chunks
////		if (!mIsClientSide) {
////			ChangeDimensionPacket packet(toId, changeRequest.mPosition, changeRequest.mRespawn);
////			mPacketSender->send(player->mOwner, packet);
////
////			// ServerPlayer take control of client movement.  ServerPlayer then needs to wait for
////			// client LocalPlayer to ack dimension complete before allowing client movement again.
////			// This is to compensate for lack of server authoritative movement
////			// in a general sense and forces it when necessary for dimension changes.
////			if (!player->isLocalPlayer()) {
////				player->mServerHasMovementAuthority = true;
////			}
////		}
////
////		player->mChunksSentSinceStart = 0;
////
////		// start bringing in chunks in new dimension
////		player->prepareRegion(toDimension.getChunkSource());
////
////		player->onDimensionChanged();
////
////		changeRequest.mState = ChangeDimensionRequest::State::WaitingForChunks;
////	}
////
////	if (changeRequest.mState == ChangeDimensionRequest::State::WaitingForChunks) {
////
////		// only server can create/use portals, client just goes where the server sends it.
////		if (!mIsClientSide && changeRequest.mUsePortal) {
////
////			if (!player->getRegion().hasChunksAt(BlockPos(player->mPos.x, player->mPos.y, player->mPos.z), PortalForcer::PORTAL_CREATION_RADIUS * 2)) {
////				return false;
////			}
////
////			// find or create a portal and move player to it
////			if (fromId != DimensionId::TheEnd) {
////				mPortalForcer->force(*player);
////			}
////
////			// final position has been found, force set it and remove lerp
////			player->setPos(player->getPos());
////			player->lerpTo(Vec3::ZERO, Vec2::ZERO, 0);
////
////			// force reset the client position to match server
////			mPacketSender->send(player->mOwner, MovePlayerPacket(*player, Player::PositionMode::Respawn));
////
////			changeRequest.mUsePortal = false;
////		}
////
////		if (!player->getRegion().hasChunksAt(player->getPos() - (float)CHUNK_WIDTH, player->getPos() + (float)CHUNK_WIDTH)) {
////			return false;
////		}
////
////		if (changeRequest.mRespawn) {
////			changeRequest.mState = ChangeDimensionRequest::State::WaitingForRespawn;
////		}
////		else {
////
////			// Client tells server they are done with the switch
////			if (mIsClientSide) {
////				PlayerActionPacket changeDimensionPacketAck(PlayerActionType::CHANGE_DIMENSION_ACK, player->getRuntimeID());
////				mPacketSender->send(changeDimensionPacketAck);
////			}
////
////			player->mDimensionState = Player::DimensionState::Ready;
////			return true;
////		}
////	}
////
////	if (changeRequest.mState == ChangeDimensionRequest::State::WaitingForRespawn) {
////
////		if (player->isRespawnReady()) {
////			player->mDimensionState = Player::DimensionState::Ready;
////			player->respawn();
////			mPacketSender->send(player->mOwner, MovePlayerPacket(*player, Player::PositionMode::Respawn));
////			return true;
////		}
////	}
////
////	return false;
////}
//
////void Level::_handleChangeDimensionRequests() {
////
////	if (mChangeDimensionRequests.size() == 0 || !mIsFinishedInitializing) {
////		return;
////	}
////
////	std::vector<Player*> toRemove;
////
////	for (auto& pair : mChangeDimensionRequests) {
////
////		Player* player = pair.first;
////		ChangeDimensionRequest& changeRequest = *pair.second.get();
////
////		if (changeRequest.mToDimensionId != changeRequest.mFromDimensionId && getPlayer(player->getUniqueID())) {
////			player->fireDimensionChangedEvent(changeRequest.mToDimensionId);
////
////			if (_playerChangeDimension(player, changeRequest)) {
////				toRemove.push_back(player);
////			}
////		}
////		else if(!isPlayerSuspended(*player)) {
////			toRemove.push_back(player);
////		}
////	}
////
////	for (auto&& p : toRemove) {
////		mChangeDimensionRequests.erase(p);
////	}
////}
////
////
////void Level::_handlePlayerSuspension() {
////	for (auto&& playerSuspension : mSuspendResumeList) {
////		if (playerSuspension.mSuspensionState == PlayerSuspension::State::Suspend) {
////			_suspendPlayer(playerSuspension.mId);
////		}
////		else if (playerSuspension.mSuspensionState == PlayerSuspension::State::Resume) {
////			_resumePlayer(playerSuspension.mId);
////		}
////	}
////
////	mSuspendResumeList.clear();
////}
////
////void Level::_suspendPlayer(const mce::UUID& id) {
////	Player* player = nullptr;
////	for (auto it = mActivePlayers.begin(); it != mActivePlayers.end(); ++it) {
////		if ((*it)->mClientUUID == id) {
////			player = *it;
////			mSuspendedPlayers.emplace_back(std::move(*it));
////			mActivePlayers.erase(it);
////			break;
////		}
////	}
////
////	if (player) {
////		player->onSuspension();
////		mRuntimeIdMap.erase(player->getRuntimeID());
////	}
////}
////
////void Level::_resumePlayer(const mce::UUID& id) {
////	Player* player = nullptr;
////	for (auto it = mSuspendedPlayers.begin(); it != mSuspendedPlayers.end(); ++it) {
////		if ((*it)->mClientUUID == id) {
////			player = *it;
////			mActivePlayers.emplace_back(std::move(*it));
////			mSuspendedPlayers.erase(it);
////			break;
////		}
////	}
////
////	if (player) {
////		mRuntimeIdMap.emplace(player->getRuntimeID(), player->getUniqueID());
////	}
////}
//
//void Level::_saveAllMapData() {
//	if (mLevelStorage) {
//		for (auto&& mapData : mMapData) {
//			_saveMapData(*mapData.second);
//		}
//	}
//}
//
//void Level::_saveDimensionStructures() {
//	if (mLevelStorage) {
//		for (auto&& dimension : mDimensions) {
//			std::string dimensionName = Util::format("dimension%d", dimension.first);
//			Unique<CompoundTag> dimensionStructureTag = dimension.second->getChunkSource().getStructureTag();
//			if (dimensionStructureTag) {
//				mLevelStorage->saveData(dimensionName, *dimensionStructureTag);
//			}
//		}
//	}
//}
//
//void Level::_loadDimensionStructures(int dimensionId) {
//	if (mLevelStorage) {
//		CompoundTag dimensionTag;
//
//		std::string dimensionName = Util::format("dimension%d", dimensionId);
//		std::string structureData = mLevelStorage->loadData(dimensionName);
//
//		if (!structureData.empty()) {
//			StringByteInput input(structureData);
//			auto structureTag = NbtIo::read(input);
//
//			if (structureTag) {
//				mDimensions[dimensionId]->getChunkSource().readStructureTag(*structureTag);
//			}
//		}
//	}
//}
//
////void Level::_removeAllPlayers() {
////
////	// mPlayers must never contain null objects. This ensures the list is cleared _before_ the objects' destructors run.
////	mActivePlayers.clear();
////	mSuspendedPlayers.clear();
////	std::vector<Unique<Player>>().swap(mPlayers);
////
////	//HACK
////	//  else    {
////	//      for (auto it = all.begin(); it != all.end(); ++it) {
////	//          // We need this new way to remove players because they can currently access block entities directly.
////	//          // Should be removed when we handle the moving correctly inside block entities when players are removed
////	//          // and for places where we do not interact with them through tick.
////	//          ++currentTick;
////	//          (*it)->tickWorld(currentTick);
////	//          (*it)->remove();
////	//          _cleanupDisconnectedPlayers();
////	//      }
////	//  }
////}
//
///*public*/
//bool Level::checkAndHandleMaterial(const AABB& box, MaterialType material, Entity* e) {
//
//	auto& region = e->getRegion();
//	AABB floored = box.flooredCopy(0, 1);
//
//	if (!region.hasChunksAt(floored)) {
//		return false;
//	}
//
//	bool ok = false;
//	Vec3 current(0, 0, 0);
//
//	for (auto x : rangef(floored.min.x, floored.max.x)) {
//		for (auto y : rangef(floored.min.y, floored.max.y)) {
//			for (auto z : rangef(floored.min.z, floored.max.z)) {
//				if (region.getMaterial(x, y, z).isType(material)) {
//					//seems like we should do the pos handling here, and return?  We currently
//					//guarantee worst-case behavior in this nested loop.
//					ok = true;
//					const Block* block = Block::mBlocks[region.getBlockID(x, y, z)];
//					block->handleEntityInside(region, BlockPos(x, y, z), e, current);
//				}
//			}
//		}
//	}
//
//	if (current.length() > 0) {
//		current = current.normalized();
//		const float pow = 0.014f;
//		e->mPosDelta.x += current.x * pow;
//		e->mPosDelta.y += current.y * pow;
//		e->mPosDelta.z += current.z * pow;
//	}
//
//	return ok;
//}
//
//bool Level::checkMaterial(const AABB& box, MaterialType material, Entity* e) {
//
//	auto& region = e->getRegion();
//	AABB floored = box.flooredCopy(0, 1);
//
//	if (!region.hasChunksAt(floored)) {
//		return false;
//	}
//
//	for (auto x : rangef(floored.min.x, floored.max.x)) {
//		for (auto y : rangef(floored.min.y, floored.max.y)) {
//			for (auto z : rangef(floored.min.z, floored.max.z)) {
//				if (region.getMaterial(x, y, z).isType(material)) {
//					return true;
//				}
//			}
//		}
//	}
//
//	return false;
//}
//
////Player* Level::getNearestPlayer(Entity& source, float maxDist) {
////	return getNearestPlayer(source.mPos.x, source.mPos.y, source.mPos.z, maxDist);
////}
////
////Player* Level::getNearestPlayer(float x, float y, float z, float maxDist) {
////	float maxDistSqr = maxDist * maxDist;
////	float best = -1;
////	Player* result = nullptr;
////	Vec3 pos(x, y, z);
////
////	forEachPlayer([&maxDist, &maxDistSqr, &best, &result, &pos](Player& player) {
////		if (player.isRemoved()) {
////			return true;
////		}
////
////		float dist = player.distanceToSqr(pos);
////		if ((maxDist < 0 || dist < maxDistSqr) && (best == -1 || dist < best)) {
////			best = dist;
////			result = &player;
////		}
////
////		return true;
////	});
////
////	return result;
////}
////
////Player* Level::getNearestAttackablePlayer(BlockPos source, float maxDist, Entity* sourceEntity /*= nullptr*/) {
////	float best = -1.0f;
////	Player* result = nullptr;
////
////	forEachPlayer([&best, &source, &maxDist, &sourceEntity, &result](Player& player) {
////		if (player.mAbilities.mInvulnerable || !player.isAlive()) {
////			return true;
////		}
////
////		float dist = player.distanceToSqr(source);
////		float visibleDist = maxDist;
////
////		// decrease the max attackable distance if the target player is sneaking or invisible
////		if (player.isSneaking()) {
////			visibleDist *= .8f;
////		}
////
////		if ((player.isInvisible() && !sourceEntity) || (player.isInvisible() && sourceEntity && !sourceEntity->canSeeInvisible())) {
////			float coverPercentage = player.getArmorCoverPercentage();
////			if (coverPercentage < .1f) {
////				coverPercentage = .1f;
////			}
////			visibleDist *= (.7f * coverPercentage);
////		}
////
////		if (sourceEntity && sourceEntity->hasCategory(EntityCategory::Mob) && player.isHiddenFrom(*static_cast<Mob*>(sourceEntity))) {
////			visibleDist *= 0.5f;
////		}
////
////		if ((maxDist < 0 || dist < visibleDist * visibleDist) && (best == -1 || dist < best)) {
////			best = dist;
////			result = &player;
////		}
////
////		return true;
////	});
////
////	return result;
////}
////
////Player* Level::getNearestAttackablePlayer(Entity& source, float maxDist) {
////	return getNearestAttackablePlayer(source.getPos(), maxDist, &source);
////}
////
////void Level::updateSleepingPlayerList() {
////}
//
////void Level::_syncTime(int time) {
////	if (!mIsClientSide) {
////		SetTimePacket packet(time, isDayCycleActive());
////		mPacketSender->send(packet);
////	}
////}
////
////void Level::_cleanupDisconnectedPlayers() {
////	ScopedProfile("removePendingPlayers");
////
////	for (auto player = mPlayers.begin(); player != mPlayers.end(); ) {
////		if ((*player)->isRemoved()) {
////
////			if ((*player)->isRiding()) {
////				(*player)->stopRiding(true, true);
////			}
////
////			for (auto&& listener : m_Listeners) {
////				listener->onEntityRemoved(**player);				
////			}
////			if (mHitResult.getEntity() == player->get()) {
////				mHitResult.resetHitType();
////				mHitResult.resetHitEntity();
////			}
////
////			if(!mIsClientSide) {
////				mRuntimeIdMap.erase((*player)->getRuntimeID());
////				mPlayerList.erase((*player)->mClientUUID);
////				PlayerListPacket packet((*player)->mClientUUID);
////				mPacketSender->send(packet);
////			}
////
////			for (auto lightUpdate = mLightUpdates.begin(); lightUpdate != mLightUpdates.end(); ) {
////				if (lightUpdate->getRegion() == &(*player)->getRegion()) {
////					lightUpdate = mLightUpdates.erase(lightUpdate);
////				} else {
////					lightUpdate++;
////				}
////			}
////
////			// The player is in either the active or suspended list so remove them from there.
////			auto it = std::find(mActivePlayers.begin(), mActivePlayers.end(), player->get());
////			if (it != mActivePlayers.end()) {
////				mActivePlayers.erase(it);
////			}
////			else {
////				auto suspendedIt = std::find(mSuspendedPlayers.begin(), mSuspendedPlayers.end(), player->get());
////
////				if (suspendedIt != mSuspendedPlayers.end()) {
////					mSuspendedPlayers.erase(suspendedIt);
////				}
////			}
////
////			// It's undefined which happens first, the destructor of the erased object or the removal of it from the list.
////			// Usually it's deleted first. This is problematic since our destructors do important things, and they sometimes
////			// loop over this list. Our list is not allowed to contain null elements.
////			Unique<Player> delayedDeletedPlayer = std::move(*player);
////			player = mPlayers.erase(player);
////
////			for (auto&& listener : m_Listeners) {				
////				listener->playerListChanged(); // This event needs to be sent after the player has been removed from the list
////			}
////		}
////		else {
////			player++;
////		}
////	}
////}
////
////void Level::onPlayerDeath(Player& player, const EntityDamageSource& source){
////	if (!mIsClientSide) {
////		int32_t killer = (int32_t)EntityType::Undefined;  //Undefined. If the player dies from falling or suffocating, we don't care about the killer.
////		if (source.getDamagingEntity()) {
////			killer = (int32_t)source.getDamagingEntity()->getEntityTypeId();
////		}
////		EventPacket deathPacket(&player, killer, source.getCause());
////		player.sendEventPacket(deathPacket);
////
////		std::pair<std::string, std::vector<std::string> > m = source.getDeathMessage(player.mName, &player);
////		TextPacket packet = TextPacket::createTranslated(m.first, m.second);
////		mPacketSender->send(packet);
////
////		player.setRespawnDimensionId(DimensionId::Overworld);
////
////		player.updateTeleportDestPos();
////		player.recheckSpawnPosition();
////	}
////}
//
///*public*/
////void Level::tick() {
////	tickEntities();
////
////	mAvailableImmediateLightUpdates = 100;	//replenish immediate light updates for this tick
//
//	//shuffle the order of ticks so that we don't mistakenly make blocks or player actions that rely on a specific ordering
//	//if (getCurrentTick().getTimeStamp() % 20 == 0) {
//	//	std::random_shuffle(tickingChunksOffset.begin(), tickingChunksOffset.end(), mRandom);
//	//	std::random_shuffle(mActivePlayers.begin(), mActivePlayers.end(), mRandom);
//	//}
//
//	////TODO move in ServerLevel?
//	//if (!mIsClientSide && mLevelData.getSpawnMobs() && !mActivePlayers.empty()) {
//	//	const int mobSpawnInterval = 10;
//	//	if (++mMobSpawnTick >= mobSpawnInterval) {
//	//		mMobSpawnTick = 0;
//	//		ScopedProfile("mobSpawner");
//
//	//		WorkerPool::getMain().queue([&] () {
//	//			ScopedProfile("tickMobSpawners");
//	//			if (!mActivePlayers.empty()) {
//	//				mMobSpawner->tick(*getRandomPlayer());
//	//			}
//
//	//			return true;
//	//		});
//	//	}
//	//}
//
//	//{
//	//	ScopedProfile("updateSky");
//	//	if (!mIsClientSide) {
//	//		if (mNightMode) {
//	//			auto curTime = mLevelData.getTime();
//	//			if (curTime % TICKS_PER_DAY != MIDDLE_OF_NIGHT_TIME) {
//	//				if (curTime % TICKS_PER_DAY < MIDDLE_OF_NIGHT_TIME && (curTime + 20) % TICKS_PER_DAY > MIDDLE_OF_NIGHT_TIME) {
//	//					curTime = MIDDLE_OF_NIGHT_TIME;
//	//					_syncTime(curTime);
//	//				}
//	//				else {
//	//					curTime += 20;
//	//					if (curTime % 20 == 0) {
//	//						_syncTime(curTime);
//	//					}
//	//				}
//	//				setTime(curTime);
//	//			}
//	//		}
//	//		else {
//	//			auto time = getTime() + 1;
//
//	//			if (setTime(time) > 255) {
//	//				_syncTime(time);
//
//	//				mLastTimePacketSent = getTime();
//	//			}
//	//		}
//	//	}
//	//}
//
//	// tick all dimensions
//	//for (auto dimIt = mDimensions.begin(); dimIt != mDimensions.end(); ++dimIt) {
//	//	dimIt->second->tick();
//	//}
//
//	//{
//	//	//ScopedProfile("backgroundTickSeasons");
//	//	_backgroundTickSeasons();
//	//}
//
//	//{
//	//	//ScopedProfile("tickTiles");
//
//	//	//let the players tick itself and all of the world
//	//	mLevelData.incrementTick();
//
//	//	forEachPlayer([](Player& player) {
//	//		player.moveView();
//	//		return true;
//	//	});
//
//	//	forEachPlayer([this](Player& player) {
//	//		player.tickWorld(mLevelData.getCurrentTick());
//	//		return true;
//	//	});
//	//}
//	//{
//	//	ScopedProfile("save dirty chunks");
//	//	_saveSomeChunks();
//	//}
//
//	//{
//	//	//kill old entities
//	//	ScopedProfile("removePendingEntities");
//	//	mPendingEntitiesToRemove.clear();
//	//}
//
//	//for (auto dimIt = mDimensions.begin(); dimIt != mDimensions.end(); ++dimIt) {
//	//	auto baseregion = dimIt->second->mBlockSource.get();
//	//	if (baseregion != nullptr) {
//	//		dimIt->second->getCircuitSystem().updateDependencies(baseregion);
//	//	}
//	//}
//
//	//// Save player info every n:th second
//	//if (getTimeS() - m_LastSaveDataTime >= 30) {
//	//	saveGameData();
//	//}
//
//	//_handlePlayerSuspension();
//
//	//_cleanupDisconnectedPlayers();
//
//	//_handleChangeDimensionRequests();
////}
//
////void Level::directTickEntities(BlockSource& blockSource) {
////	UNUSED_PARAMETER(blockSource);
////}
//
////void Level::_backgroundTickSeasons() {
////	// POST PROCESS LEVEL CHUNK for Seasons.
////	const int MAX_TRIES = 10;
////
////	Vec3 chunkPos, diff;
////	forEachPlayer([&](Player& player) {
////		if (PlayerChunkSource* chunks = player.getChunkSource()) {
////
////			//try a max number of times to find a good chunk
////			for (auto i : range(MAX_TRIES)) {
////				UNUSED_VARIABLE(i);
////
////				LevelChunk* lc = *(chunks->getArea().begin() + mRandom.nextInt(chunks->getArea().size()));
////
////				// Loaded yet?
////				if (!lc || lc->getState() != ChunkState::Loaded) {
////					continue;
////				}
////
////				// Need seasons post process at all?
////				if (!lc->checkSeasonsPostProcessDirty()) {
////					continue;
////				}
////
////				lc->applySeasonsPostProcess(player.getRegion());
////				break;
////			}
////		}
////		return true;
////	});
////}
//
//void Level::_saveSomeChunks() {
//
//	if (mChunkSaveInProgress || mLevelStorage == nullptr) {
//		return; 
//	}
//
//	// we're not currently saving a chunk, so let's check if there are any live chunks to save
//	if (!WorkerPool::isStarved()) {
//		int tries = 30;	//try some times to save stuff, stop at the first success
//
//		for (int i = 0; i < tries; ++i) {
//
//			//choose a random player
//			auto player = getRandomPlayer();
//			if (!player) {
//				break;	//no players
//
//			}
//			Dimension* dimension = player->getDimension();
//			if (!dimension) {
//				break;	//no dimension
//
//			}
//			if (!player->isRegionValid()) {
//				break;	//no region
//
//			}
//			auto& region = player->getRegion();
//			ChunkPos playerCPos(player->mPos);
//
//			//choose a random offset from the list of tickable chunks
//			auto& offset = tickingChunksOffset[mRandom.nextInt(tickingChunksOffset.size())];
//
//			ChunkPos p = playerCPos + offset;
//			auto lc = region.getChunk(p);
//
//			if (lc && lc->needsSaving(MIN_TICKS_BEFORE_LIVE_SAVE, MAX_TICKS_WITHOUT_LIVE_SAVE)) {
//
//				// live chunks get a distance of -1, so they get handled first
//				mLevelChunkSaveQueue.push(LevelChunkQueuedSavingElement(lc->getPosition(), dimension->getId(), -1));
//				break; // only process one chunk at a time
//			}
//		}
//	}
//
//	// now queue up a chunk to save if our queue isn't empty
//	if (!mLevelChunkSaveQueue.empty()) {
//		mChunkSaveInProgress = true;
//
//		WorkerPool::getMain().queue([this]() {
//
//			LevelChunkQueuedSavingElement itemToSave = mLevelChunkSaveQueue.top();
//			mLevelChunkSaveQueue.pop();
//
//			Dimension* dimension = getDimension(itemToSave.mDimensionId);
//
//			if (dimension != nullptr) {
//
//				// go through all players, first one that has this chunk gives us the levelChunk to save out
//				for (size_t i = 0; i < mActivePlayers.size(); i++) {
//					auto& region = mActivePlayers[i]->getRegion();
//					auto lc = region.getChunk(itemToSave.mPosition);
//
//					if (lc && (lc->getDimensionId() == itemToSave.mDimensionId) && lc->needsSaving(0, 0)) {
//
//						// found a player who knows about this chunk, and the chunk still needs saving,
//						// and the chunk dimension matches.  Try to save it
//						if (dimension->mChunkSource->saveLiveChunk(*lc)) {
//#ifdef DEBUG_STATS
//							DebugStats::instance.mLiveSavedChunks.push_back(lc->getPosition());
//#endif
//							break;	//done, saved our chunk for today!
//						}
//					}
//				}
//
//			}
//
//			mChunkSaveInProgress = false;
//			return true;
//		},
//			BackgroundWorker::NOP,
//			2000);	//quite not important
//	}
//}
//
//void Level::saveDirtyChunks() {
//
//	for (auto& dimension : mDimensions) {
//		ChunkSource& cs = dimension.second->getChunkSource();
//
//		for (auto& chunkRef : cs.getStoredChunks()) {
//			auto& lc = chunkRef.second.getChunk();
//
//			if (lc.isDirty() && lc.getState() == ChunkState::Loaded) {
//				cs.saveLiveChunk(lc);
//			}
//		}
//	}
//}
//
///*public*/
//void Level::loadPlayer(Unique<Player> newPlayer) {
//
//	//acquire the new player
//	auto& player = *newPlayer;
//	_validatePlayerName(player);
//	mActivePlayers.emplace_back(newPlayer.get());
//	mPlayers.emplace_back(std::move(newPlayer));
//
//	player.setPlayerGameType(getLevelData().getGameType());
//
//	auto localPlayerTag = mLevelStorage ? mLevelStorage->loadLocalPlayerData() : nullptr;
//	if (localPlayerTag != nullptr) {
//
//		//load the player position in advance
//		auto pos = localPlayerTag->getList("Pos");
//		Vec3 playerPos(pos->getFloat(0), pos->getFloat(1), pos->getFloat(2));
//		player.setPos(playerPos);
//
//		//load player's dimension id in advance
//		DimensionId dimensionId = DimensionId::Overworld;
//		if (localPlayerTag->contains("DimensionId")) {
//			int dimensionIdValue = localPlayerTag->getInt("DimensionId");
//			if (dimensionIdValue >= 0 && dimensionIdValue < DimensionId::Count) {
//				dimensionId = static_cast<DimensionId>(dimensionIdValue);
//			}
//		}
//		auto& dimension = createDimension(dimensionId);
//		player.prepareRegion(dimension.getChunkSource());		//acquire the part of the level the player is in
//
//		static_cast<Entity&>(player).mInitMethod = Entity::InitializationMethod::LOADED;
//
//		// Players do stuff in load and reload which could suspend the player...
//		player.reload();
//
//		//finish loading
//		player.load(*localPlayerTag);
//
//		if (!player.hasRespawnPosition()) {
//			player.setRespawnPosition(mLevelData.getSpawn(), false);
//		}
//
//		if (auto tag = localPlayerTag->getCompound(Entity::RIDING_TAG)) {
//			auto mount = EntityFactory::loadEntity(tag);
//			if (mount) {
//				addEntity(player.getRegion(), std::move(mount));
//				player.startRiding(*mount);
//			}
//		}
//
//		mLevelData.clearLoadedPlayerTag();
//	}
//	else {
//		player.respawn();	//place at spawn
//
//		auto& dimension = createDimension(DimensionId::Overworld);
//		player.prepareRegion(dimension.getChunkSource());		//acquire the part of the level the player is in
//		
//		static_cast<Entity&>(player).mInitMethod = Entity::InitializationMethod::SPAWNED;
//		player.reload();
//	}
//
//	// Remove duplicate, existing entities which have the same EntityUnqiqueID as the player
//	for (auto&& dimension : mDimensions) {
//		auto entityItr = dimension.second->mEntityIdLookup.find(player.getUniqueID());
//
//		// Found duplicate
//		if (entityItr != dimension.second->mEntityIdLookup.end()) {
//			LOGW("Duplicated entity, player loading with existing entity's UniqueEntityID\n");
//			entityItr->second->remove();
//		}
//	}
//
//	// place in playerList after player is loaded so that unique id is correct,
//	// before lazy init of uniqueid in PlayerListEntry constructor.
//	mPlayerList.emplace(player.mClientUUID, PlayerListEntry(player));
//}
//
//Unique<Path> Level::findPath(Entity& from, Entity& to, float maxDist, bool canPassDoors, bool canOpenDoors, bool avoidWater, bool canFloat, bool avoidPortals){
//	return (PathFinder(&from.getRegion(), canPassDoors, canOpenDoors, avoidWater, canFloat, avoidPortals)).findPath(&from, &to, maxDist);
//}
//
//Unique<Path> Level::findPath(Entity& from, int xBest, int yBest, int zBest, float maxDist, bool canPassDoors, bool canOpenDoors, bool avoidWater, bool canFloat, bool avoidPortals){
//	return (PathFinder(&from.getRegion(), canPassDoors, canOpenDoors, avoidWater, canFloat, avoidPortals)).findPath(&from, xBest, yBest, zBest, maxDist);
//}

void Level::broadcastLevelEvent(LevelEvent type, Vec3 const& pos, int data, Player* p) {
	if (mIsClientSide) {
		return;
	}

// 	LevelEventPacket packet((int)type, pos, data);
// 	if (p) {
// 		mPacketSender->sendBroadcast(p->mOwner, packet);
// 	}
// 	else {
// 		mPacketSender->send(packet);
// 	}
}

//void Level::playSound(BlockSource& region, LevelSoundEvent type, Vec3 const& pos, int blockID, EntityType entityType, bool isBabyMob, bool isGlobal) {
//	Player* player = getLocalPlayer();
//	if (player && (player->getDimensionId() == region.getDimensionId())) {
//		playSound(type, pos, blockID, entityType, isBabyMob, isGlobal);
//	}
//}
//
//void Level::playSound(LevelSoundEvent type, Vec3 const& pos, int blockID, EntityType entityType, bool isBabyMob, bool isGlobal) {
//	for (unsigned int i = 0; i < m_Listeners.size(); i++) {
//		m_Listeners[i]->levelSoundEvent(type, pos, blockID, entityType, isBabyMob, isGlobal);
//	}
//}
//
//void Level::playSynchronizedSound(BlockSource& region, LevelSoundEvent type, Vec3 const& pos, int blockID, EntityType entityType, bool isBabyMob, bool isGlobal) {
//	if (mIsClientSide) {
//		LevelSoundEventPacket packet(type, pos, blockID, entityType, isBabyMob, isGlobal);
//		mPacketSender->send(packet);
//	}
//	else {
//		broadcastSoundEvent(region, type, pos, blockID, entityType, isBabyMob, isGlobal);
//	}
//}
//
//void Level::broadcastSoundEvent(BlockSource& region, LevelSoundEvent type, Vec3 const& pos, int blockID, EntityType entityType, bool isBabyMob, bool isGlobal) {
//	if (mIsClientSide) {
//		return;
//	}
//	playSound(region, type, pos, blockID, entityType, isBabyMob, isGlobal);
//	LevelSoundEventPacket packet(type, pos, blockID, entityType, isBabyMob, isGlobal);
//	if (isGlobal) {
//		Player* player = getLocalPlayer();
//		if (player) {
//			mPacketSender->sendBroadcast(player->mOwner, packet);
//		}
//		else {
//			mPacketSender->sendBroadcast(packet);
//		}
//	}
//	else {
//		region.getDimension().sendPacketForPosition(pos, packet, getLocalPlayer());
//	}
//}

void Level::broadcastDimensionEvent(BlockSource& region, LevelEvent type, Vec3 const& pos, int data, Player* except) {
	if (mIsClientSide) {
		return;
	}

	// Same handler as broadcastLevelEvent, but only goes to players in same dimension as sender.
	//
// 	LevelEventPacket packet((int)type, pos, data);
// 	region.getDimension().sendBroadcast(packet, except);
}

void Level::handleLevelEvent(LevelEvent type, Vec3 const& pos, int data) {
	for (unsigned int i = 0; i < m_Listeners.size(); i++) {
		m_Listeners[i]->levelEvent(type, pos, data);
	}
}

//void Level::handleSoundEvent(LevelSoundEvent type, Vec3 const& pos, int blockID, EntityType entityType, bool isBabyMob, bool isGlobal) {
//	for (unsigned int i = 0; i < m_Listeners.size(); i++) {
//		m_Listeners[i]->levelSoundEvent(type, pos, blockID, entityType, isBabyMob, isGlobal);
//	}
//}
//
//Particle* Level::addParticle(ParticleType id, const Vec3& pos, const Vec3& dir, int data /* = 0*/) {
//	for (unsigned int i = 0; i < m_Listeners.size(); i++) {
//		if (auto p = m_Listeners[i]->addParticle(id, pos, dir, data)) {
//			return p;
//		}
//	}
//	return nullptr;
//}
//
//Entity * Level::addGlobalEntity(BlockSource &region, Unique<Entity> e) {
//	DEBUG_ASSERT_MAIN_THREAD;
//
//	Entity* entity = e.get();
//	if (addEntity(region, std::move(e))) {
//		entity->setGlobal(true);
//		mGlobalEntities.emplace(entity);
//		return entity;
//	}
//	return nullptr;
//}
//
//Entity * Level::addAutonomousEntity(BlockSource &region, Unique<Entity> e) {
//	DEBUG_ASSERT_MAIN_THREAD;
//
//	Entity* entity = e.get();
//	if (addEntity(region, std::move(e))) {
//		entity->setAutonomous(true);
//		if (entity->isGlobal()) {
//			mGlobalEntities.emplace(entity);
//		}
//		mAutonomousEntities.emplace(entity);
//		return entity;
//	}
//	return nullptr;
//}
//
//
//Entity * Level::addEntity(BlockSource &region, Unique<Entity> e) {
//	DEBUG_ASSERT_MAIN_THREAD;
//	if (!e) {
//		DEBUG_FAIL("Adding a null entity!");
//		return nullptr;
//	}
//
//	e->mLevel = &region.getLevel();
//	e->setRegion(region);
//
//	// Note: this assert should really never hit, since we now set the region in this function
//	DEBUG_ASSERT(e->isRegionValid(), "Entity not assigned a region");
//
//	if (region.getDimension().mEntityIdLookup.count(e->getUniqueID()) > 0) {
//		LOGW("Duplicated entity, probably for a crash during handoffs\n");
//		return nullptr;
//	}
//
//	if (getPlayer(e->getUniqueID()) != nullptr) {
//		LOGW("Duplicated entity, entity trying to load with player's EntityUniqueId\n");
//		return nullptr;
//	}
//
//
//	if (!e->canExistInPeaceful() && !mMobSpawner->getSpawnEnemies() && !e->mForcedLoading) {
//		LOGW("Attempting to spawn a hostile mob in a peaceful world");
//		return nullptr;
//	}
//	e->mForcedLoading = false;
//
//	if(!e->hasRuntimeID()){
//		e->setRuntimeID(getNextRuntimeID());
//	}
//
//	mRuntimeIdMap.insert(std::pair<EntityRuntimeID, EntityUniqueID>(e->getRuntimeID(), e->getUniqueID()));
//
//	//write the entity down in the chunk
//	ChunkPos cp(e->mPos);
//	LevelChunk* c = region.getChunk(cp);
//	if (c) {
//		Entity *entity = e.get();
//		// Add the entity to the chunk, the dimension, and register the entity as added
//		entity->mAdded = true;
//		c->addEntity(std::move(e));
//		
//		auto result = region.getDimension().getEntityIdMap().insert(std::make_pair(entity->getUniqueID(), entity));
//		DEBUG_ASSERT(result.second, "This entity somehow got past the first duplicate entity check.");
//		UNUSED_VARIABLE(result);
//
//		// Notify all listeners of the entity
//		for (unsigned int i = 0; i < m_Listeners.size(); i++) {
//			m_Listeners[i]->onEntityAdded(*entity);
//		}
//
//		entity->reload();
//
//		//entity->move(Vec3::ZERO);
//
//		return entity;
//	}
//	else {
//		LOGE("Tried to add Entity to level at position (%i, %i)", cp.x, cp.z);
//		DEBUG_FAIL("Tried to add the entity, but its chunk position was not loaded!");
//		return nullptr;
//	}
//}
//
//void Level::addPlayer(Unique<Player> player) {
//	auto& playerReference = *player;
//	if (!mIsClientSide) {
//		// Validate name before adding to the level
//		_validatePlayerName(playerReference);
//	}
//	//This needs to occur after name validation because it checks against whether the player exists in active players
//	mActivePlayers.emplace_back(player.get());
//	mPlayers.emplace_back(std::move(player));
//
//	if(!mIsClientSide) {
//		if(!playerReference.hasRuntimeID()){
//			playerReference.setRuntimeID(getNextRuntimeID());
//		}
//
//		mRuntimeIdMap.insert(std::pair<EntityRuntimeID, EntityUniqueID>(playerReference.getRuntimeID(), playerReference.getUniqueID()));
//
//		PlayerListEntry newEntry(playerReference);
//		mPlayerList.emplace(playerReference.mClientUUID, newEntry);
//		PlayerListPacket packet;
//		packet.add(newEntry);
//		mPacketSender->send(packet);
//		_syncTime(getTime());
//		playerReference.setPlayerIndex(getNewPlayerId());
//	}
//
//	for (unsigned int i = 0; i < m_Listeners.size(); i++) {
//		m_Listeners[i]->onEntityAdded(playerReference);
//		m_Listeners[i]->playerListChanged();
//	}
//
//	if (!mIsClientSide) {
//		// If we're in EDU mode, add all players to all maps
//		//	Note: this needs to be *after* they're added to the Level
//		if (mLevelData.isEduLevel()) {
//			for (auto&& data : mMapData) {
//				data.second->addTrackedMapEntity(playerReference, MapDecoration::Type::Player);
//			}
//		}
//	}
//
//	playerReference.mAdded = true;
//
//	// Only call if not already called.
//	if (playerReference.mInitMethod != Entity::InitializationMethod::UPDATED) {
//		playerReference.reload();
//	}
//}
//
//void Level::suspendPlayer(Player& player) {
//	mSuspendResumeList.emplace_back(player.mClientUUID, PlayerSuspension::State::Suspend);
//}
//
//void Level::resumePlayer(Player& player) {
//	mSuspendResumeList.emplace_back(player.mClientUUID, PlayerSuspension::State::Resume);
//}
//
//bool Level::isPlayerSuspended(Player& player) const {
//	const mce::UUID &id = player.mClientUUID;
//
//	auto suspendIt = std::find_if(mSuspendedPlayers.begin(), mSuspendedPlayers.end(), [&id](const Player *player) {
//		return player->mClientUUID == id;
//	});
//
//	if (suspendIt != mSuspendedPlayers.end()) {
//		return true;
//	}
//
//	auto it = std::find_if(mSuspendResumeList.begin(), mSuspendResumeList.end(), [&id](const PlayerSuspension& playerSuspension) {
//		return playerSuspension.mId == id;
//	});
//
//	if (it != mSuspendResumeList.end()) {
//		return true;
//	}
//
//	return false;
//}
//
//void Level::getEntities(DimensionId dimension, EntityType type, const AABB& aabb, std::vector<Entity*>& entities, bool ignoreTargetType) {
//	int xc0 = Math::floor((aabb.min.x - 2) / 16);
//	int xc1 = Math::floor((aabb.max.x + 2) / 16);
//	int zc0 = Math::floor((aabb.min.z - 2) / 16);
//	int zc1 = Math::floor((aabb.max.z + 2) / 16);
//
//	for (int xc = xc0; xc <= xc1; xc++) {
//		for (int zc = zc0; zc <= zc1; zc++) {
//			LevelChunk* chunk = getDimension(dimension)->getChunkSource().getAvailableChunk(ChunkPos(xc, zc));
//
//			if (chunk) {
//				chunk->getEntities(type, aabb, entities, ignoreTargetType);
//			}
//		}
//	}
//
//	// TODO: Remove this when players are loaded in with other entities
//	// Currently players are kept separate
//	if (EntityClassTree::isTypeInstanceOf(EntityType::Player, type)) {
//		forEachPlayer([&dimension, &aabb, &entities](Player& player) {
//			if (player.getDimensionId() == dimension && player.mBB.intersects(aabb)) {
//				entities.push_back(&player);
//			}
//			return true;
//		});
//	}
//}
//
//void Level::removeAllNonPlayerEntities(EntityUniqueID keepEntityId) {
//  std::vector<Entity*> removedEntities;
//  
//	// Remove all entities
//	for (auto& dim : mDimensions) {
//		EntityMap& entityMap = dim.second->getEntityIdMap();
//		for (auto i = entityMap.begin(); i != entityMap.end(); ++i) {
//			auto entity = i->second;
//			
//          // If 'player entity' or 'keep entity' then keep
//          if (entity->hasCategory(EntityCategory::Player) || entity->getUniqueID() == keepEntityId) {
//            continue;
//          }
//          else {
//            removedEntities.push_back(entity);
//          }
//		}
//      
//      while(!removedEntities.empty()) {
//        forceRemoveEntity(*removedEntities.back());
//        removedEntities.pop_back();
//      }
//	}
//}
//
//Entity* Level::fetchEntity(EntityUniqueID entityId, bool getRemoved) const {
//	if (!entityId) {
//		return nullptr;
//	}
//
//	for (auto& dim : mDimensions) {
//		Entity* e = dim.second->fetchEntity(entityId, getRemoved);
//		if (e) {
//			return e;
//		}
//	}
//
//	return nullptr;
//}
//
//Unique<Entity> Level::takeEntity(EntityUniqueID entityId) {
//	Entity* entity = fetchEntity(entityId, true);
//	if (entity) {
//		if (!entity->isRemoved()) {
//			entity->remove();
//		}
//
//		if (auto lc = entity->getRegion().getChunk(entity->mChunkPos)) {
//			if (lc->hasEntity(*entity)) {
//				Unique<Entity> removedEntity = lc->removeEntity(*entity);
//				return removedEntity;
//			}
//		}
//	}
//
//	return nullptr;
//}
//
//Unique<Entity> Level::borrowEntity(EntityUniqueID entityId, LevelChunk* lc) {
//	Entity* entity = fetchEntity(entityId, true);
//	if (entity) {
//		//if we're not given a chunk then attempt to grab one
//		if (lc == nullptr) {
//			lc = entity->getRegion().getChunk(entity->mChunkPos);
//		}
//		else {
//			DEBUG_ASSERT(lc->isBlockInChunk(BlockPos(entity->getPos())), "The given LevelChunk should contain this entity")
//		}
//		if (lc != nullptr) {
//			if (lc->hasEntity(*entity)) {
//				Unique<Entity> removedEntity = lc->removeEntity(*entity);
//				return removedEntity;
//			}
//		}
//	}
//
//	return nullptr;
//}
//
//Entity* Level::getRuntimeEntity(EntityRuntimeID runtimeId, bool getRemoved) const {
//	auto eit = mRuntimeIdMap.find(runtimeId);
//	if (eit != mRuntimeIdMap.end()) {
//		return fetchEntity(eit->second, getRemoved);
//	}
//
//	// then look through players
//	return findPlayer([&getRemoved, &runtimeId](const Player& player) {
//		return (!player.isRemoved() || getRemoved) && player.getRuntimeID() == runtimeId;
//	});
//}

void Level::removeEntityReferences(Entity& e, bool discardingChunk) {
	////DEBUG_ASSERT(entityIdLookup.count(e.getUniqueID()) > 0, "Can't remove an entity that wasn't added");

	////don't run this at all if the level is being erased
	//if (mTearingDown) {
	//	return;
	//}

	//if (mHitResult.getEntity() == &e) {
	//	mHitResult.resetHitType();
	//	mHitResult.resetHitEntity();
	//}

	////TODO should this go somewhere else? yes it should
	//if (!discardingChunk) {
	//	if (e.isRide()) {
	//		e.removeAllRiders(true);
	//	}

	//	if (e.isRiding()) {
	//		e.stopRiding(true, true);
	//	}
	//}

	//if (e.isGlobal()) {
	//	mGlobalEntities.erase(&e);
	//}

	////remove from entity id lookup map in dimension.
	//Dimension* dimension = getDimension(e.getDimensionId());
	//if (dimension != nullptr) {
	//	dimension->getEntityIdMap().erase(e.getUniqueID());
	//}

	//mRuntimeIdMap.erase(e.getRuntimeID());

	////call listeners
	//for (auto& listener : m_Listeners) {
	//	listener->onEntityRemoved(e);
	//}
}

//void Level::forceRemoveEntity(Entity& e) {
//  if (!e.isRemoved()) {
//    e.remove();
//  }
//	removeEntityReferences(e);
//	// take the entity from the level and don't give it back >_<
//	takeEntity(e.getUniqueID());
//}
//
//void Level::forceFlushRemovedPlayers() {
//	_cleanupDisconnectedPlayers();
//}

void Level::queueEntityRemoval(Unique<Entity>&& e, bool discardingChunk) {
	DEBUG_ASSERT(e->isRemoved(), "Can't remove an entity not flagged for removal");

	removeEntityReferences(*e, discardingChunk);

	//add to an entity buffer that will be destroyed at the end of the tick to prevent ordering issues
	mPendingEntitiesToRemove.emplace(std::move(e));
}

//void Level::registerTemporaryPointer(_TickPtr& ptr) {
//	mTempTickPointers.insert(&ptr);
//	}
//
//void Level::unregisterTemporaryPointer(_TickPtr& ptr) {
//	mTempTickPointers.erase(&ptr);
//}
//
//Mob* Level::getMob(EntityUniqueID entityId) const {
//	Entity* entity = fetchEntity(entityId);
//	return (entity && entity->hasCategory(EntityCategory::Mob)) ? (Mob*)entity : nullptr;
//}
//
//void Level::addListener(LevelListener& listener) {
//	m_Listeners.push_back(&listener);
//
//	//also add it to all the block sources
//	for (auto& region : mRegions) {
//		region->addListener(listener);
//	}
//}
//
//void Level::removeListener(LevelListener& listener) {
//	ListenerList::iterator it = std::find(m_Listeners.begin(), m_Listeners.end(), &listener);
//	if (it != m_Listeners.end()) {
//		m_Listeners.erase(it);
//	}
//
//	for (auto& region : mRegions) {
//		region->removeListener(listener);
//	}
//}
//
//void Level::setDayCycleActive(bool active) {
//
//	if (active == isDayCycleActive()) {
//		return;
//	}
//
//	if (!active) {
//		auto time = getTime();
//		mLevelData.setStopTime(time);
//
//		_syncTime(time);
//	}
//	else {
//		setTime(mLevelData.getStopTime());	//reset the time to the moment it was stopped, to keep consistency
//		mLevelData.setStopTime(-1);
//
//		_syncTime(getTime());
//	}
//}
//
//void Level::tickEntities() {
//	_tickTemporaryPointers();
//	ScopedProfile("entities");
//
//	for (size_t i = 0; i < mGlobalEntities.size(); ++i) {
//		Entity* entity = mGlobalEntities[i];
//
//		if (entity->isRemoved()) {
//			mGlobalEntities.erase(entity);
//			--i;
//			continue;
//		}
//		else {
//			entity->mPosPrev2 = entity->mPos;
//			entity->mRotPrev = entity->mRot;
//		}
//
//		//Currently, in a multiplayer game, entities are getting created with the server player's Block Source.
//		//Since Global Entities aren't confined to getting ticked by a nearby player, a Global Entity that spawns or exists in an area
//		//that the server player hasn't been to will not be able to collide with anything in that area, which is a big issue for projectiles.
//		//This will not be a problem once client/server split is complete and shipped and the server is completely authoritative about entity spawning.
//		//The Dimension's Block Source is exactly what a Global Entity needs: A "global" Block Source.
//		//Maybe we can rename the method to something else that better captures the use and purpose?
//		entity->setRegion(getDimension(entity->getDimensionId())->getBlockSourceDEPRECATEDUSEPLAYERREGIONINSTEAD());
//
//		entity->tick(entity->getRegion());
//
//		if (!isClientSide()) {
//			entity->sendMotionPacketIfNeeded();
//		}
//	}
//
//	for (size_t i = 0; i < mAutonomousEntities.size(); ++i) {
//		Entity* entity = mAutonomousEntities[i];
//
//		if (entity->isRemoved()) {
//			mAutonomousEntities.erase(entity);
//			--i;
//		}
//	}
//
//	// Yes, I know this is bad, but it's extremely few players
//	// to remove anyway.
//	for (auto i : reverse_range(mPendingPlayerRemovals.size())) {
//		PRInfo& pr = mPendingPlayerRemovals[i];
//		if (--pr.ticks <= 0) {
//			LOGI("deleting: %p\n", pr.e);
//			delete pr.e;
//			mPendingPlayerRemovals.erase(mPendingPlayerRemovals.begin() + i);
//		}
//	}
//
//	//	TIMER_POP();
//}
//
//class DistanceEntitySorter {
//	Vec3 c;
//public:
//
//	DistanceEntitySorter(float x, float y, float z) :
//		c(x, y, z) {
//	}
//
//	bool operator() (const Entity* c0, const Entity* c1) {
//		const float d0 = c.distanceToSqr(c0->mPos.x, c0->mPos.y, c0->mPos.z);
//		const float d1 = c.distanceToSqr(c1->mPos.x, c1->mPos.y, c1->mPos.z);
//		return d0 < d1;
//	}
//
//};
//
///* in java, this returns an Explosion */
//void Level::explode(BlockSource& region, Entity* source, const Vec3& pos, float explosionRadius, bool fire, bool breaksBlocks, float maxResistance) {
//	if (!mIsClientSide) {
//
//		Explosion explosion(region, source, pos, explosionRadius);
//
//		explosion.setFire(fire);
//		explosion.setBreaking(breaksBlocks);
//		explosion.setMaxResistance(maxResistance);
//		explosion.explode();
//
//		ExplodePacket packet(pos, explosionRadius, explosion.getToBlow());
//		region.getDimension().sendPacketForPosition(pos, packet);
//
//		explosion.finalizeExplosion();
//	}
//}
//
//void Level::denyEffect(const Vec3& pos) {
//	broadcastLevelEvent(LevelEvent::ParticleDenyBlock, pos, 0);
//	playSound(LevelSoundEvent::Deny, pos);
//}
//
//void Level::potionSplash(const Vec3& pos, const Color& color, bool instantaneousEffect) {
//	if (!mIsClientSide) {
//		LevelEventPacket packet((int) LevelEvent::ParticlesPotionSplash, pos, color.toARGB() | (instantaneousEffect  ? 1 << 24 : 0));
//		mPacketSender->send(packet);
//	}
//}
//
//bool Level::extinguishFire(BlockSource& region, const BlockPos& pos, FacingID face) {
//	const BlockPos neighbor = pos.neighbor(face);
//
//	if (region.getBlock(neighbor).isType(Block::mFire)) {
//		broadcastSoundEvent(region, LevelSoundEvent::ExtinguishFire, neighbor.center());
//		region.removeBlock(neighbor);
//		return true;
//	}
//
//	return false;
//}

bool Level::isDayCycleActive() {
	return mLevelData.getStopTime() < 0;
}

//int Level::getLightsToUpdate() {
//	return mLightUpdates.size();
//}
//
//void Level::updateLights(){
//	ScopedProfile("updateLights");
//	//no light update coroutine and there are pending updates, run first
//	if(!mLightUpdateRunning && !mLightUpdates.empty()) {
//		const int max = 60;
//		mLightUpdateRunning = true;
//		WorkerPool::getFor(WorkerRole::LightUpdate).queue([this, max] () {
//			ScopedProfile("updateLights");
//
//			int i = 0;
//			for (; !mLightUpdates.empty() && i != max; ++i) {
//				LightUpdate l = mLightUpdates.back();
//				mLightUpdates.pop_back();
//				l.updateFast();
//			}
//			mLightUpdateRunning = (i == max);
//			return !mLightUpdateRunning;
//		});
//	}
//}
//
//void Level::setSpawnSettings(bool spawnEntities) {
//	mSpawnEntities = spawnEntities;
//}
//
//bool Level::getSpawnEntities() const {
//	return mSpawnEntities;
//}
//
//void Level::setDifficulty(Difficulty difficulty) {
//
//	if (mLevelData.getGameDifficulty() != difficulty) {
//
//		mLevelData.setGameDifficulty(difficulty);
//
//		mMobSpawner->setSpawnEnemies(mLevelData.getGameDifficulty() != Difficulty::Peaceful);
//
//		if (!mMobSpawner->getSpawnEnemies()) {
//
//			for (auto& dim : mDimensions) {
//
//				// Intentionally make a copy of the current EntityIdMap because 
//				// 'forceRemoveEntity()' will erase from the original EntityIdMap 
//				// which will make the iterators invalid. 
//				// 
//				// This code only runs when the user changes difficulty to peaceful 
//				// so taking the hit from the copy is no big deal at all. -ee
//				EntityMap copyOfEntityMap = dim.second->getEntityIdMap();
//
//				for (auto i = copyOfEntityMap.begin(), iend = copyOfEntityMap.end(); i != iend; ++i ) {
//					Entity* entity = i->second;
//
//					// If 'player entity' or 'keep entity' then skip
//					if (!entity->canExistInPeaceful()) {
//						forceRemoveEntity(*entity);
//					}
//				}
//			}
//		}
//	}
//
//	saveLevelData();
//}
//
//void Level::setMultiplayerGame(bool multiplayerGame) {
//	mLevelData.setMultiplayerGame(multiplayerGame);
//	saveLevelData();
//}
//
//void Level::setLANBroadcast(bool broadcast) {
//	mLevelData.setLANBroadcast(broadcast);
//	saveLevelData();
//}
//
//void Level::setXBLBroadcast(bool broadcast) {
//	mLevelData.setXBLBroadcast(broadcast);
//	saveLevelData();
//}
//
//void Level::setCommandsEnabled(bool commandsEnabled) {
//	mLevelData.setCommandsEnabled(commandsEnabled);
//	if (commandsEnabled) {
//		mLevelData.disableAchievements();
//	}
//	saveLevelData();
//}
//
//void Level::animateTick(Entity& relativeTo) {
//	//this thing only runs for the local player!
//	int r = 16;
//	auto& region = relativeTo.getRegion();
//	auto basePos = BlockPos(relativeTo.getPos());
//
//	for (int i = 0; i < 100; i++) {
//		const int zPos = basePos.z + mRandom.nextGaussianInt(r);
//		const int yPos = basePos.y + mRandom.nextGaussianInt(r);
//		const int xPos = basePos.x + mRandom.nextGaussianInt(r);
//		BlockPos pos(xPos, yPos, zPos);
//
//		auto& block = region.getBlock(pos);
//		block.animateTick(region, pos, mAnimateRandom);
//	}
//}

int Level::setTime(int time) {

	if (isDayCycleActive()) {
		time %= TICKS_PER_MOON_MONTH;

		//clamp the value to avoid overflow
		mLevelData.setTime(time);
	}
	else {
		mLevelData.setTime(mLevelData.getStopTime());
	}

	//compute increase, checking foor loopbacks
	return (mLastTimePacketSent > time) ? (TICKS_PER_MOON_MONTH - mLastTimePacketSent + time) : (time - mLastTimePacketSent);
}

int Level::setStopTime(int time) {
	time %= TICKS_PER_MOON_MONTH;

	mLevelData.setStopTime(time);

	return (mLastTimePacketSent > time) ? (TICKS_PER_MOON_MONTH - mLastTimePacketSent + time) : (time - mLastTimePacketSent);
}

RandomSeed Level::getSeed() {
	return mLevelData.getSeed();
}

int Level::getTime() const {
	return mLevelData.getTime();
}

//const BlockPos& Level::getSharedSpawnPos(){
//	return mLevelData.getSpawn();
//}
//
//void Level::setDefaultSpawn(const BlockPos& spawnPos) {
//	mLevelData.setSpawn(spawnPos);
//	saveLevelData();
//}
//
//const BlockPos& Level::getDefaultSpawn() const {
//	return mLevelData.getSpawn();
//}
//
//void Level::broadcastEntityEvent(Entity* e, EntityEvent eventId) {
//	if (mIsClientSide) {
//		return;
//	}
//
//	EntityEventPacket packet(e->getRuntimeID(), eventId);
//	e->getRegion().getDimension().sendBroadcast(packet);
//}
//
//void Level::addBossEventListener(BossEventListener* listener) {
//	mBossEventListeners.push_back(listener);
//}
//
//void Level::removeBossEventListener(BossEventListener* listener) {
//	BossEventListenerList::iterator it = std::find(mBossEventListeners.begin(), mBossEventListeners.end(), listener);
//	if (it != mBossEventListeners.end()) {
//		mBossEventListeners.erase(it);
//	}
//}
//
//void Level::broadcastBossEvent(BossEventUpdateType type, Mob* boss) {
//	for (auto& it : mBossEventListeners) {
//		it->onBossEvent(type, boss);
//	}
//}

bool Level::hasLevelStorage() const {
	return mLevelStorage != nullptr;
}

LevelStorage& Level::getLevelStorage() {
	DEBUG_ASSERT(mLevelStorage != nullptr, "Trying to access null levelStorage, use hasLevelStorage check.");
	return *mLevelStorage.get();
}

LevelData& Level::getLevelData() {
	return mLevelData;
}

const LevelData& Level::getLevelData() const {
	return mLevelData;
}

////int Level::countInstanceOfType(EntityType typeId) {
////	return entityTypeCount[(int)typeId];
////}
//
//void Level::saveLevelData() {
//	if (mLevelStorage) {
//		mLevelStorage->saveLevelData(mLevelData);
//
//		_saveAllMapData();
//
//		_saveDimensionStructures();
//	}
//
//	if (mSavedDataStorage) {
//		mSavedDataStorage->save();
//	}
//}
//
//void Level::savePlayers() {
//	if (mLevelStorage) {
//		forEachPlayer([this](Player& player) {
//			mLevelStorage->save(player);
//			return true;
//		});
//	}
//}
//
//void Level::saveAutonomousEntities() {
//	if (mLevelStorage) {
//		std::unique_ptr<CompoundTag> entityListTag = make_unique<CompoundTag>();
//		std::unique_ptr<ListTag> entityTagList = make_unique<ListTag>();
//		for (auto& e : mAutonomousEntities) {
//			if (!e->isRemoved()) {
//				std::unique_ptr<CompoundTag> entityTag = make_unique<CompoundTag>();
//				e->save(*entityTag);
//				entityTagList->add(std::move(entityTag));
//			}
//		}
//		for (auto& e : mAutonomousLoadedEntities) {
//			std::unique_ptr<CompoundTag> entityTag = make_unique<CompoundTag>();
//			e->save(*entityTag);
//			entityTagList->add(std::move(entityTag));
//		}
//		entityListTag->put("AutonomousEntityList", std::move(entityTagList));
//		mLevelStorage->saveData("AutonomousEntities", *entityListTag);
//	}
//}
//
//void Level::loadAutonomousEntities() {
//	mAutonomousEntities.clear();
//	mAutonomousLoadedEntities.clear();
//
//	if (mLevelStorage) {
//		CompoundTag dimensionTag;
//
//		std::string entityList = mLevelStorage->loadData("AutonomousEntities");
//		
//		if (!entityList.empty()) {
//			StringByteInput input(entityList);
//			auto globalTag = NbtIo::read(input);
//
//			if (globalTag && globalTag->getList("AutonomousEntityList")) {
//				const ListTag* entityTagList = globalTag->getList("AutonomousEntityList");
//				for (int j = 0; j < entityTagList->size(); ++j) {
//					CompoundTag* entityTag = (CompoundTag*)entityTagList->get(j);
//					Unique<Entity> loaded = EntityFactory::loadEntity(entityTag);
//					if (loaded) {
//						mAutonomousLoadedEntities.push_back(std::move(loaded));
//					}
//				}
//			}
//		}
//	}
//}
//
//void Level::saveGameData() {
//	ScopedProfile("savePlayerInfo");
//
//	savePlayers();
//	saveAutonomousEntities();
//	saveLevelData();
//	m_LastSaveDataTime = getTimeS();
//}
//
//void Level::saveBiomeData() {
//
//	if (mLevelStorage) {
//		Biome::SaveInstanceData(*mLevelStorage);
//	}
//}
//
//std::string Level::getPlayerNames() {
//	std::stringstream returnString;
//	returnString << mActivePlayers.size() << ':';
//
//	for (auto it = mPlayers.begin(); it != mPlayers.end(); ++it) {
//		returnString << (*it)->mName << ',';
//	}
//	return returnString.str();
//}
//
//int Level::getActivePlayerCount() const {
//	return static_cast<int>(mActivePlayers.size());
//}
//
//Player* Level::getPlayer(const std::string& name) const {
//	return findPlayer([&name](const Player& player) {
//		return player.mName == name;
//	});
//}
//
//Player* Level::getPlayer(const mce::UUID& uuid) const {
//	return findPlayer([&uuid](const Player& player) {
//		return player.mClientUUID == uuid;
//	});
//}
//
//Player* Level::getNextPlayer(const EntityUniqueID& entityID, bool includeLocal) {
//	auto firstPlayer = mActivePlayers.begin();
//	if (!includeLocal && (getLocalPlayer() == *firstPlayer)) {
//		firstPlayer++;
//		//are there no players besides the local?
//		if (firstPlayer == mActivePlayers.end()) {
//			return nullptr;
//		}
//	}
//
//	Player* foundPlayer = nullptr;
//
//	for (auto&& player : mActivePlayers) {
//		if (nullptr != foundPlayer) {
//			return player;
//		}
//		if ((includeLocal || (getLocalPlayer() != player)) && (player->getUniqueID() == entityID)) {
//			foundPlayer = player;
//		}
//	}
//
//	//at the end of the list?
//	if (nullptr != foundPlayer) {
//		return *firstPlayer;
//	}
//	return nullptr;
//}
//
//Player* Level::getPrevPlayer(const EntityUniqueID& entityID, bool includeLocal) {
//	Player* lastPlayer = nullptr;
//	Player* foundPlayer = nullptr;
//
//	for (auto&& player : mActivePlayers) {
//		if (player->getUniqueID() == entityID) {
//			foundPlayer = player;
//			if (nullptr != lastPlayer) {
//				return lastPlayer;
//			}
//		}
//
//		if (includeLocal || (getLocalPlayer() != player)) {
//			lastPlayer = player;
//		}
//	}
//
//	//at the end of the list?
//	if (nullptr != foundPlayer) {
//		return lastPlayer;
//	}
//
//	return nullptr;
//}
//
//int Level::getNumRemotePlayers(){
//	int count = 0;
//
//	forEachPlayer([&count](const Player& player) {
//		if (!player.isLocalPlayer()) {
//			++count;
//		}
//		return true;
//	});
//
//	return count;
//}
//
//Player* Level::getPlayer(EntityUniqueID entityID) const {
//	auto e = fetchEntity(entityID);
//	return (e && e->hasCategory(EntityCategory::Player)) ? (Player*)e : nullptr;
//}
//
//Player* Level::getRuntimePlayer(EntityRuntimeID runtimeId) const {
//	auto e = getRuntimeEntity(runtimeId);
//	return (e && e->hasCategory(EntityCategory::Player)) ? (Player*)e : nullptr;
//}
//
//Player* Level::getLocalPlayer() const {
//	return findPlayer([](const Player& player) {
//		return player.isLocalPlayer();
//	});
//}
//
//Player* Level::getRandomPlayer() {
//	if (mActivePlayers.empty()) {
//		return nullptr;
//	}
//
//	return mActivePlayers[getRandom().nextInt(mActivePlayers.size())];
//}
//
//MinecraftEventing& Level::getEventing() {
//	return mEventing;
//}

StructureManager& Level::getStructureManager() {
	return mStructureManager;
}

//int32_t Level::getNewPlayerId() const {
//	std::set<int32_t> usedIndices;
//	for (auto&& player : mPlayers) {
//		usedIndices.insert(player->getPlayerIndex());
//	}
//
//	int32_t newPlayerIndex = 0;
//	// Find a new (unused) index for the player
//	while (usedIndices.find(newPlayerIndex) != usedIndices.end()) {
//		++newPlayerIndex;
//	}
//
//	return newPlayerIndex;
//}
//
//Color Level::getPlayerColor(const Player& player) const {
//	static const std::array<Color, 15> PlayerColors = {
//		*ColorFormat::ColorFromColorCode(ColorFormat::BLUE),
//		*ColorFormat::ColorFromColorCode(ColorFormat::RED),
//		*ColorFormat::ColorFromColorCode(ColorFormat::WHITE),
//		*ColorFormat::ColorFromColorCode(ColorFormat::GOLD),
//		*ColorFormat::ColorFromColorCode(ColorFormat::GREEN),
//		*ColorFormat::ColorFromColorCode(ColorFormat::AQUA),
//		*ColorFormat::ColorFromColorCode(ColorFormat::YELLOW),
//		*ColorFormat::ColorFromColorCode(ColorFormat::LIGHT_PURPLE),
//		*ColorFormat::ColorFromColorCode(ColorFormat::DARK_AQUA),
//		*ColorFormat::ColorFromColorCode(ColorFormat::DARK_PURPLE),
//		*ColorFormat::ColorFromColorCode(ColorFormat::DARK_GREEN),
//		*ColorFormat::ColorFromColorCode(ColorFormat::DARK_RED),
//		*ColorFormat::ColorFromColorCode(ColorFormat::DARK_BLUE),
//		*ColorFormat::ColorFromColorCode(ColorFormat::GRAY),
//		*ColorFormat::ColorFromColorCode(ColorFormat::DARK_GRAY)
//	};
//
//	return PlayerColors[player.getPlayerIndex() % PlayerColors.size()];
//}

void Level::onSourceCreated(BlockSource& source) {
	mRegions.insert(&source);

	//add all the listeners to the region
	for (auto&& l : m_Listeners) {
		source.addListener(*l);
	}
}

void Level::onSourceDestroyed(BlockSource& source) {
	mRegions.erase(&source);

	for (auto&& l : m_Listeners) {
		source.removeListener(*l);
	}
}

void Level::onNewChunkFor(Player& p, LevelChunk& lc){
	DEBUG_ASSERT_MAIN_THREAD;

	//load with a valid region
	lc.onLoaded(p.getRegion());

	for (auto iter = mAutonomousLoadedEntities.begin(); iter != mAutonomousLoadedEntities.end();) {
		if (lc.getDimensionId() == (*iter)->getDimensionId() && lc.isBlockInChunk(BlockPos((*iter)->mPos))) {
// 			addAutonomousEntity(p.getRegion(), std::move(*iter));
			iter = mAutonomousLoadedEntities.erase(iter);
		}
		else {
			++iter;
		}
	}

	for (auto&& l : m_Listeners) {
		l->onNewChunkFor(p, lc);
	}
}

void Level::onChunkLoaded(LevelChunk& lc){
	DEBUG_ASSERT_MAIN_THREAD;

	// It's important that SeasonsPostProcess happens 'On Chunk Load'
	// This seems to be the best place to ensure that happens.
	if (lc.checkSeasonsPostProcessDirty()) {
		lc.applySeasonsPostProcess(lc.getDimension().getBlockSourceDEPRECATEDUSEPLAYERREGIONINSTEAD());
	}

	int minDist = INT_MAX;

	forEachPlayer([&lc, this, &minDist](Player& player) {
		//if players region is in the right dimension and has the chunk update them
		if (player.getRegion().getDimensionId() == lc.getDimensionId() && player.getRegion().getChunk(lc.getPosition())) {
			onNewChunkFor(player, lc);
		}

		// grab the chunk position for the player
		ChunkPos playerChunkPos(player.getPos());

		int dist = playerChunkPos.distanceToSqr(lc.getPosition());

		minDist = Math::min(dist, minDist);

		return true;
	});

	for (auto& listener : m_Listeners) {
		listener->onChunkLoaded(lc);
	}

	// save out chunk if it's dirty from loading
// 	if (lc.needsSaving(0, 0)) {
// 		LevelChunkQueuedSavingElement toSave(lc.getPosition(), lc.getDimensionId(), minDist);
// 
// 		mLevelChunkSaveQueue.push(toSave);
// 	}
}

void Level::onChunkDiscarded(LevelChunk& lc) {
	if (!mTearingDown) {
		for (size_t i = 0; i < mAutonomousEntities.size(); ++i) {
			Entity* e = mAutonomousEntities[i];

			if (lc.isBlockInChunk(BlockPos(e->mPos))) {
				mAutonomousEntities.erase(e);

// 				Unique<Entity> takenEntity = borrowEntity(e->getUniqueID(), &lc);
// 				if (takenEntity) {
// 					mAutonomousLoadedEntities.push_back(std::move(takenEntity));
// 					removeEntityReferences(*(mAutonomousLoadedEntities.back()));
// 					// being in the loaded list means the entity is no longer loaded into a chunk
// 					e->mAdded = false;
// 				}
// 				--i;
			}
		}

	}
	
	for (auto& entity : lc.getEntities()) {
		removeEntityReferences(*entity, true);
	}

// 	forEachPlayer([&lc](Player& player) {
// 		if (player.isRegionValid()) {
// 			player.getRegion().onChunkDiscarded(lc);
// 		}
// 		return true;
// 	});
	for (auto& dimensionPair : mDimensions) {
		dimensionPair.second->getBlockSourceDEPRECATEDUSEPLAYERREGIONINSTEAD().onChunkDiscarded(lc);
	}
	for (auto& listener : m_Listeners) {
		listener->onChunkUnloaded(lc);
	}
}

void Level::forEachPlayer(std::function<bool(Player&)> callback) {
	for (auto&& player : mActivePlayers) {
		if (!callback(*player)) {
			break;
		}
	}
}

void Level::forEachPlayer(std::function<bool(const Player&)> callback) const {
	for (auto&& player : mActivePlayers) {
		if (!callback(*player)) {
			break;
		}
	}
}

//Player* Level::findPlayer(std::function<bool(const Player&)> pred) const {
//	for (auto&& player : mActivePlayers) {
//		if (pred(*player)) {
//			return player;
//		}
//	}
//
//	return nullptr;
//}
//
//int Level::getUserCount() const {
//	return mPlayers.size();
//}
//
//std::vector<Unique<Player>>& Level::getUsers() {
//	return mPlayers;
//}
//
//const std::vector<Unique<Player>>& Level::getUsers() const {
//	return mPlayers;
//}
//
//void Level::runLightUpdates(BlockSource& source, const LightLayer& layer, const BlockPos& min, const BlockPos& max) {
//
//	if (layer.isSky() && layer.getSurrounding() == Brightness::MIN) {
//		return;
//	}
//
//	mMaxLightLoopCount++;
//	if (mMaxLightLoopCount == 50) {
//		mMaxLightLoopCount--;
//		return;
//	}
//
//	if (!source.hasChunksAt(min, max)) {
//		mMaxLightLoopCount--;
//		return;
//	}
//
//	int count = mLightUpdates.size();
//
//	int toCheck = 5;
//	if (toCheck > count) {
//		toCheck = count;
//	}
//
//	for (int i = 0; i < toCheck; i++) {
//		LightUpdate& last = mLightUpdates[mLightUpdates.size() - 1 - i];
//		if (last.getLayer() == &layer && last.expandIfCloseEnough(min, max)) {
//			mMaxLightLoopCount--;
//			return;
//		}
//	}
//
//	LightUpdate l(source, layer, min, max);
//	if (mAvailableImmediateLightUpdates) {
//		--mAvailableImmediateLightUpdates;
//		l.updateFast();
//	}
//	else{
//		mLightUpdates.emplace_back(l);
//	}
//
//	int MAX_UPDATES = 10000;
//	//TODO: rather than aborting, we should be allowing the work to trickle through -- slower than normal, if necessary
//	if ((int)mLightUpdates.size() > MAX_UPDATES) {
//		LOGI("More than %d updates, aborting lighting updates\n", MAX_UPDATES);
//		mLightUpdates.clear();
//	}
//	mMaxLightLoopCount--;
//}
//
//
//void Level::_validatePlayerName(Player& player) {
//#ifdef MCPE_EDU
//	std::string originalName = player.getNameTag();
//	std::string playerName = originalName;
//
//	// Pad player name with number
//	int namePad = 2;
//	while (getPlayer(playerName) != nullptr) {
//		playerName = originalName + Util::toString(namePad++);
//	}
//
//	if (playerName != player.getNameTag()) {
//		EntityUniqueID newPlayerId = player.getUniqueID();
//
//		// Delay a bit so the player is added to the level
//		WorkerPool::getFor(WorkerRole::MainThread).queue(
//			[newPlayerId, this]() {
//			Player* newPlayer = getPlayer(newPlayerId);
//			if (newPlayer != nullptr) {
//				newPlayer->displayLocalizableMessage(ColorFormat::ITALIC + ColorFormat::GRAY + I18n::get("chat.renamed", { newPlayer->getNameTag() }));
//			}
//
//			return true;
//		});
//	}
//	player.setName(playerName);
//#else
//	UNUSED_PARAMETER(player);
//#endif
//}
//
//void Level::_tickTemporaryPointers() {
//	for (auto& p : mTempTickPointers) {
//		p->invalidate();
//	}
//}
//
//bool Level::destroyBlock(BlockSource& region, const BlockPos& pos, bool dropResources){
//	auto t = region.getBlockAndData(pos);
//	if (t.id == 0) {
//		return false;
//	}
//
//	broadcastDimensionEvent(region, LevelEvent::ParticlesDestroyBlock, pos + Vec3(0.5f, 0.5f, 0.5f), t.id + (t.data << Block::TILE_NUM_SHIFT));
//	const Block* block = Block::mBlocks[t.id];
//	if (dropResources && block != nullptr) {
//		block->spawnResources(region, pos, t.data, 1.0f);
//	}
//	return region.setBlockAndData(pos, FullBlock(), Block::UPDATE_ALL);
//}
//
//Spawner& Level::getSpawner() const {
//	return *mMobSpawner;
//}
//
//ProjectileFactory& Level::getProjectileFactory() const {
//	return *mProjectileFactory;
//}
//
//EntityDefinitionGroup& Level::getEntityDefinitions() const {
//	return *mEntityDefinitions;
//}
//
//int Level::isNightMode() const {
//	return mNightMode;
//}
//
//void Level::setNightMode(bool isNightMode) {
//	mNightMode = isNightMode;
//}

bool Level::isClientSide() const {
// 	return mIsClientSide;
	return false;
}

void Level::setIsClientSide(bool val) {
// 	mIsClientSide = val;
}

//Unique<Villages>& Level::getVillages() {
//	return mVillages;
//}
//
//std::unordered_map<mce::UUID, PlayerListEntry>& Level::getPlayerList() {
//	return mPlayerList;
//}
//
//GlobalEntityList& Level::getGlobalEntities() {
//	return mGlobalEntities;
//}
//
//AutonomousEntityList& Level::getAutonomousEntities() {
//	return mAutonomousEntities;
//}
//
//Entity* Level::getAutonomousActiveEntity(EntityUniqueID id) const {
//	for (auto&& entity : mAutonomousEntities) {
//		if (entity->getUniqueID() == id) {
//			return entity;
//		}
//	}
//	return nullptr;
//}
//
//Entity* Level::getAutonomousInactiveEntity(EntityUniqueID id) const {
//	for (auto&& entity : mAutonomousLoadedEntities) {
//		if (entity->getUniqueID() == id) {
//			return entity.get();
//		}
//	}
//	return nullptr;
//}
//
//Entity* Level::getAutonomousEntity(EntityUniqueID id) const {
//	if (Entity* result = getAutonomousActiveEntity(id)) {
//		return result;
//	}
//	return getAutonomousInactiveEntity(id);
//}
//
//Entity* Level::moveAutonomousEntityTo(const Vec3& to, EntityUniqueID id, BlockSource& toRegion) {
//	// See if the entity is in a loaded chunk somewhere
//	Entity* found = getAutonomousActiveEntity(id);
//
//	// If we found it, it's in a loaded chunk somewhere, find it and transfer ownership to destination chunk
//	if (found) {
//		// Use dimension source as player's source may not contain the chunk we need
//		ChunkSource& chunkSource = found->getDimension().getChunkSource();
//
//		LevelChunk* toChunk = chunkSource.getExistingChunk(ChunkPos(to));
//		// Need to put it into a chunk, so if there isn't one, we can't do anything
//		if (!toChunk) {
//			return nullptr;
//		}
//
//		ChunkPos fromChunkPos = ChunkPos(found->getPos());
//		found->teleportTo(to);
//		// teleportTo will transfer ownership to new chunk unless the chunk source couldn't get the chunk, which can happen for spawn chunks and maybe others
//		// in that case we need to transfer ownership manually, using the dimension source, as it can get any chunk
//		if (!toChunk->hasEntity(*found)) {
//			LevelChunk* fromChunk = chunkSource.getAvailableChunk(fromChunkPos);
//			DEBUG_ASSERT(fromChunk, "autonomous entity was in active list, but couldn't be found in loaded chunk");
//			toChunk->addEntity(fromChunk->removeEntity(*found));
//		}
//		return found;
//	}
//
//	// Look for the autonomous entity in a suspended state, not in a loaded chunk, then put it into a chunk if found
//	for (auto it = mAutonomousLoadedEntities.begin(); it != mAutonomousLoadedEntities.end(); ++it) {
//		if ((*it)->getUniqueID() == id) {
//			found = it->get();
//			// Move to destination
//			found->teleportTo(to);
//			// Adds entity to mAutonomousEntities and transfers ownership to destination chunk
//			// Can't use found's region here because he may not have one, since he's not in loaded chunks
//			addAutonomousEntity(toRegion, std::move(*it));
//			// It's no longer in the suspended state, take it out of this container
//			mAutonomousLoadedEntities.erase(it);
//			return found;
//		}
//	}
//	return nullptr;
//}
//
//OwnedEntityList& Level::getAutonomousLoadedEntities() {
//	return mAutonomousLoadedEntities;
//}
//
//PacketSender* Level::getPacketSender() const {
//	return mPacketSender;
//}
//
//void Level::setPacketSender(PacketSender* val) {
//	mPacketSender = val;
//}
//
//void Level::sendFixedInventoryUpdatePacket() {
//	ContainerSetContentPacket packet(CONTAINER_ID_FIXEDINVENTORY, getFixedInventory().getItems(), 0);
//	mPacketSender->send(packet);
//}
//
//NetEventCallback* Level::getNetEventCallback() const {
//	return mNetEventCallback;
//}
//
//void Level::setNetEventCallback(NetEventCallback* val) {
//	mNetEventCallback = val;
//}

Random& Level::getRandom() const {
	return mRandom;
}

//HitResult& Level::getHitResult() {
//	return mHitResult;
//}
//
//AdventureSettings& Level::getAdventureSettings() {
//	return mLevelData.getAdventureSettings();
//}
//
//GameRules& Level::getGameRules() {
//	return mLevelData.getGameRules();
//}

bool Level::getTearingDown() const {
// 	return mTearingDown;
	return false;
}

//void Level::setFixedInventorySlot(int slot, ItemInstance* item) {
//	mLevelData.getFixedInventory().setItem(slot, item);
//}
//
//ItemInstance* Level::getFixedInventorySlot(int slot) {
//	return mLevelData.getFixedInventory().getItem(slot);
//}
//
//int Level::getFixedInventorySlotCount() {
//	return mLevelData.getFixedInventory().getContainerSize();
//}
//
//void Level::setFixedInventorySlotCount(int val) {
//	mLevelData.getFixedInventory().setContainerSize(val);
//}
//
//void Level::setFinishedInitializing() {
//	mIsFinishedInitializing = true;
//}
//
//void Level::takePicture(TextureData& outImage, Entity* camera, Entity* target, bool applySquareFrame, const std::string & filename) {
//	for (auto i = m_Listeners.begin(), iend = m_Listeners.end(); i != iend; ++i) {
//		(*i)->takePicture(outImage, camera, target, applySquareFrame, filename);
//	}
//}
//
//void Level::upgradeStorageVersion(StorageVersion v) {
//	if (mLevelStorage) {
//		DEBUG_ASSERT(v > getLevelData().getStorageVersion(), "The storage version is already up to date");
//
//		getLevelData().setStorageVersion(v);
//
//		mLevelStorage->saveLevelData(mLevelData);
//	}
//}
//
//void Level::onAppSuspended() {
//	saveGameData();
//	saveBiomeData();
//
//	// We're suspending the app! Don't throw critical work onto a background
//	// thread; the thread pool gets paused too!  Ack!  Leaving some of this
//	// commented stuff in, because this will need more work.
//	//WorkerPool::getFor(WorkerRole::MainThread).queue([this]()
//	{
//		if (mLevelStorage == nullptr) {
//			LevelChunk::trimMemoryPool();
//			LevelChunk::SubChunk::trimMemoryPool();
//			return;// true;
//		}
//
//		if (!mIsClientSide) {
//			saveDirtyChunks();
//		}
//
//#ifndef MCPE_PLATFORM_WIN10UAP
//		// WIN10 does this in an aysnc wait call - it is ok to call it twice, if it works out that way.
//		// the first call will do the wait, subsequent calls will just immediately complete.
//		// todo: this is a temporary measure and will change when the final version of the suspend/resume architecture is in place.
//		waitAsyncSuspendWork();
//#endif
//
//		LevelChunk::trimMemoryPool();
//		LevelChunk::SubChunk::trimMemoryPool();
//		//return true;
//	}//);
//}
//
//void Level::waitAsyncSuspendWork() {
//	//wait until all chunks are passed on to leveldb and written to disk
//	//leveldb does not cache writes, so all chunks are put on disk immediately
//	//it doesn't use fsync, but fsync is only needed in the case of a full device shutdown (eg. out of battery)
//	for (auto& dim : mDimensions) {
//		dim.second->mChunkSource->waitDiscardFinished();
//	}
//
//	// After all necessary work is complete, suspend storage so no more background work kicks off
//	if (mLevelStorage) {
//		mLevelStorage->suspendStorage();
//	}
//}
//
//
//bool Level::isUpdatingLights() const {
//	return mLightUpdates.size() > 0;
//}
//
//void Level::_destroyEffect(const BlockPos& pos, FullBlock block, const Vec3& s, float soundScale) {
//	UNUSED_PARAMETER(soundScale);
//	if (block.id == 0) {
//		return;
//	}
//
//	const Block* oldBlock = Block::mBlocks[block.id];
//
//	//HACK find the quantity of particles deriving it from the model aabbs.
//	const int SD = oldBlock->hasProperty(BlockProperty::CubeShaped) ? 6 : 5;
//
//	for (BlockPos p; p.x < SD; p.x++) {
//		for (p.y = 1; p.y < SD; p.y++) {
//			for (p.z = 0; p.z < SD; p.z++) {
//				Vec3 v(pos.x + (p.x + 0.5f) / SD, pos.y + (p.y + 0.5f) / SD, pos.z + (p.z + 0.5f) / SD);
//				Vec3 d((v.x - pos.x - 0.5f) * 0.2f + s.x, (v.y - pos.y - 0.5f) * 0.2f + s.y, (v.z - pos.z - 0.5f) * 0.2f + s.z);
//				addParticle(ParticleType::Terrain, v, d, block.toInt());
//			}
//		}
//	}
//}

Difficulty Level::getDifficulty() const {
	return mLevelData.getGameDifficulty();
}

//bool Level::isMultiplayerGame() const {
//	return mLevelData.isMultiplayerGame();
//}
//
//bool Level::hasLANBroadcast() const {
//	return mLevelData.hasLANBroadcast();
//}
//
//bool Level::hasXBLBroadcast() const {
//	return mLevelData.hasXBLBroadcast();
//}
//
//bool Level::hasCommandsEnabled() const {
//	return mLevelData.hasCommandsEnabled();
//}
//
////regional_difficulty in java
//float Level::getSpecialMultiplier(DimensionId dimension) {
//	// difficulty
//	Difficulty base = mLevelData.getGameDifficulty();
//	int totalGameTime = mLevelData.getTime();
//	int localGameTime = MAX_DIFFICULTY_TIME_LOCAL / 2;	// TODO - ticks player has spent inhabiting this chunk
//	float multiplier = 0.0f;
//	float moonBrightness = getDimension(dimension)->getMoonBrightness();
//
//	if (base != Difficulty::Peaceful) {
//		bool isHard = base == Difficulty::Hard;
//		multiplier = .75f;
//
//		// adjust for total game. Bonus does not begin to apply until total game time is greater than global difficulty
//		// offset
//		float globalScale = Math::clamp((float)totalGameTime + (float)DIFFICULTY_TIME_GLOBAL_OFFSET / (float)MAX_DIFFICULTY_TIME_GLOBAL, 0.0f, 1.0f) * .25f;
//		multiplier += globalScale;
//
//		float localScale = 0;
//
//		// local scale is 0 - 1 for easy and normal, and 0 - 1.25 for hard
//		localScale += Math::clamp((float)localGameTime / (float)MAX_DIFFICULTY_TIME_LOCAL, 0.0f, 1.0f) * (isHard ? 1.0f : 0.75f);
//		localScale += Math::clamp(moonBrightness * 0.25f, 0.0f, globalScale);
//
//		if (base == Difficulty::Easy) {
//			localScale *= .5f;
//		}
//		multiplier += localScale;
//
//		// easy ranges from .75 to 1.5
//		// normal ranges from 1.5 to 4.0
//		// hard ranges from 2.25 to 6.75
//		multiplier *= enum_cast(base);
//
//		if (multiplier < 2.0f) {
//			multiplier = 0;
//		}
//		else if (multiplier > 4.0f) {
//			multiplier = 1.0f;
//		}
//		else {
//			multiplier = (multiplier - 2.0f) / 2.0f;
//		}
//	}
//	return multiplier;
//}

EntityUniqueID Level::getNewUniqueID() {
	DEBUG_ASSERT_MAIN_THREAD;
	DEBUG_ASSERT(!mIsClientSide, "Cannot do this on clients, you should feel bad!");

	return ++mLastUniqueID;
}

//FixedInventoryContainer& Level::getFixedInventory() {
//	return mLevelData.getFixedInventory();
//}
//EntityRuntimeID Level::getNextRuntimeID() {
//	DEBUG_ASSERT_MAIN_THREAD;
//	DEBUG_ASSERT(!mIsClientSide, "Cannot do this on clients, you should feel bad!");
//
//	return ++mLastRuntimeID;
//}
//
//
//bool Level::isExporting() const {
//	return mIsExporting;
//}
//
//void Level::setIsExporting(bool IsExporting) {
//	mIsExporting = IsExporting;
//}
//
//void Level::setSavedData(const std::string& id, SavedData* data) {
//	mSavedDataStorage->set(id, data);
//}
//
//SavedDataStorage& Level::getSavedData() const {
//	return *mSavedDataStorage;
//}
//
//bool Level::doesMapExist(EntityUniqueID uuid) {
//	auto mapItr = mMapData.find(uuid);
//	if (mapItr == mMapData.end() || mapItr->second == nullptr) {
//		return _loadMapData(uuid);
//	}
//	return true;
//}
//
//MapItemSavedData& Level::getMapSavedData(const EntityUniqueID uuid) {
//	auto mapItr = mMapData.find(uuid);
//	if (mapItr == mMapData.end()) {
//
//		if (_loadMapData(uuid)) {
//			return getMapSavedData(uuid);
//		}
//
//		DEBUG_FAIL("Invalid map requested");
//	}
//
//	return *(mapItr->second);
//}
//
//
//void Level::requestMapInfo(const EntityUniqueID uuid) {
//	// Ask server for this specific map
//	if (mIsClientSide) {
//		if (mMapData.find(uuid) == mMapData.end()) {
//			MapInfoRequestPacket packet(uuid);
//			getPacketSender()->send(packet);
//			mMapData.emplace(uuid, std::unique_ptr<MapItemSavedData>());
//		}
//	}
//}
//
//MapItemSavedData& Level::createMapSavedData(const EntityUniqueID& uuid) {
//
//	// No pre-defined UUID, create entire map hierarchy
//	if (uuid == EntityUniqueID()) {
//		std::vector<EntityUniqueID> mapHierarchyIds;
//		mapHierarchyIds.resize(MapConstants::MAX_SCALE - MapConstants::DEFAULT_SCALE + 1);
//		for (size_t i = 0; i < mapHierarchyIds.size(); ++i) {
//			mapHierarchyIds[i] = getNewUniqueID();
//		}
//
//		return createMapSavedData(mapHierarchyIds);
//	}
//
//	
//	return _createMapSavedData(uuid);
//}
//
//
//MapItemSavedData& Level::createMapSavedData(const std::vector<EntityUniqueID>& mapIds) {
//	MapItemSavedData* returnValue = nullptr;
//	MapItemSavedData* lastData = nullptr;
//
//	// Pre-generate all levels of Maps
//	for (size_t i = 0; i < mapIds.size(); ++i) {
//		EntityUniqueID currentMapId = mapIds[i];
//		MapItemSavedData* newData = &_createMapSavedData(currentMapId);
//
//		newData->mScale = MapConstants::DEFAULT_SCALE + i;
//
//		if (lastData != nullptr) {
//			lastData->setParentMapId(currentMapId);
//		}
//		lastData = newData;
//
//		if (i == 0) {
//			returnValue = newData;
//		}
//	}
//
//	return *returnValue;
//}
//
//
//std::string Level::getScreenshotsFolder() const {
//	if (mLevelStorage != nullptr) {
//		return mLevelStorage->getFullPath() + "/screenshots/";
//	}
//	else {
//		return AppPlatform::singleton().getExternalStoragePath() + "/screenshots/" + mLevelId + "/";
//	}
//}
//
//std::string Level::getLevelId() const {
//	return mLevelId;
//}
//
//
//void Level::setLevelId(std::string LevelId) {
//	mLevelId = LevelId;
//}
//
//ResourcePackManager* Level::getClientResourcePackManager() const {
//	return nullptr;
//}
//ResourcePackManager* Level::getServerResourcePackManager() const {
//	return nullptr;
//}
//
//static std::string _mapKey(EntityUniqueID id) {
//	return std::string("map_" + Util::toString(id.rawID));
//}
//
//bool Level::_loadMapData(const EntityUniqueID& id) {
//	// Try to load Map data from disk
//	if (mLevelStorage != nullptr) {
//		std::string mapTagData = mLevelStorage->loadData(_mapKey(id));
//		if (mapTagData.empty() == false) {
//			StringByteInput input(mapTagData);
//			auto mapTag = NbtIo::read(input);
//
//			if (mapTag != nullptr) {
//				auto loadedMap = std::make_unique<MapItemSavedData>(id);
//				loadedMap->load(*mapTag);
//
//				mMapData.emplace(id, std::move(loadedMap));
//				return true;
//			}
//		}
//	}
//
//	return false;
//}
//
//const Tick& Level::getCurrentTick() const {
//	return mLevelData.getCurrentTick();
//}
//
//MapItemSavedData& Level::_createMapSavedData(const EntityUniqueID& uuid) {
//	DEBUG_ASSERT((mMapData.find(uuid) == mMapData.end()) || mMapData.find(uuid)->second == nullptr, "Duplicate map IDs create!");
//	if (mMapData.find(uuid) != mMapData.end()) {
//		mMapData.erase(uuid);
//	}
//
//	mMapData.emplace(uuid, std::make_unique<MapItemSavedData>(uuid));
//	MapItemSavedData* newData = mMapData[uuid].get();
//	
//	// If we're in EDU mode, add all players to all maps
//	//	Note: Removal is handled automatically when the entity
//	//	is no longer available from the Level
//	if (!mIsClientSide) {
//		if (mLevelData.isEduLevel()) {
//			forEachPlayer([&newData](Player& player) {
//				newData->addTrackedMapEntity(player, MapDecoration::Type::Player);
//				return true;
//			});
//		}
//	}
//
//	return *newData;
//}
//
//void Level::_saveMapData(MapItemSavedData& data) {
//	std::unique_ptr<CompoundTag> dataTag = std::make_unique<CompoundTag>();
//	data.save(*dataTag.get());
//
//	std::string outputString;
//	StringByteOutput output(outputString);
//	NbtIo::write(dataTag.get(), output);
//
//	mLevelStorage->saveData(_mapKey(data.getMapId()), std::move(outputString));
//}
