/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/

#include "Dungeons.h"

#include "world/level/block/entity/BlockEntity.h"

#include "nbt/CompoundTag.h"
//#include "network/Packet.h"
#include "Core/Debug/Log.h"
#include "world/level/BlockSource.h"
// #include "world/level/block/entity/BeaconBlockEntity.h"
// #include "world/level/block/entity/BrewingStandBlockEntity.h"
// #include "world/level/block/entity/CauldronBlockEntity.h"
// #include "world/level/block/entity/ChestBlockEntity.h"
// #include "world/level/block/entity/DispenserBlockEntity.h"
// #include "world/level/block/entity/DropperBlockEntity.h"
// #include "world/level/block/entity/EnchantingTableEntity.h"
// #include "world/level/block/entity/FlowerPotBlockEntity.h"
// #include "world/level/block/entity/FurnaceBlockEntity.h"
// #include "world/level/block/entity/HopperBlockEntity.h"
// #include "world/level/block/entity/MobSpawnerBlockEntity.h"
// #include "world/level/block/entity/NetherReactorBlockEntity.h"
// #include "world/level/block/entity/SignBlockEntity.h"
// #include "world/level/block/entity/SkullBlockEntity.h"
// #include "world/level/block/entity/DaylightDetectorBlockEntity.h"
// #include "world/level/block/entity/MusicBlockEntity.h"
// #include "world/level/block/entity/ComparatorBlockEntity.h"
// #include "world/level/block/entity/ItemFrameBlockEntity.h"
// #include "world/level/block/entity/PistonBlockEntity.h" 
// #include "world/level/block/entity/EndPortalBlockEntity.h"
// #include "world/level/block/entity/StructureBlockEntity.h" 
// #include "world/level/block/entity/MovingBlockEntity.h" 
// #include "world/level/block/entity/ChestBlockEntity.h"
// #include "world/level/block/entity/EndGatewayBlockEntity.h"
// #include "world/level/block/entity/ChalkboardBlockEntity.h" 
#include "world/level/BlockSource.h"

BlockEntity::MapIdType BlockEntity::idClassMap;
BlockEntity::MapTypeId BlockEntity::classIdMap;

int BlockEntity::_runningId = 0;

Unique<BlockEntity> BlockEntityFactory::createBlockEntity(BlockEntityType type, const BlockPos& pos, const BlockID blockID) {
// 	switch(type) {
// 	case BlockEntityType::Furnace:
// 		return make_unique<FurnaceBlockEntity>(pos);
// 	case BlockEntityType::Chest:
// 		return make_unique<ChestBlockEntity>(BlockEntityType::Chest, "Chest", TR_CHEST_RENDERER, pos);
// 	case BlockEntityType::Sign:
// 		return make_unique<SignBlockEntity>(pos);
// 	case BlockEntityType::NetherReactor:
// 		return make_unique<NetherReactorBlockEntity>(pos);
// 	case BlockEntityType::MobSpawner:
// 		return make_unique<MobSpawnerBlockEntity>(pos);
// 	case BlockEntityType::Skull:
// 		return make_unique<SkullBlockEntity>(pos);
// 	case BlockEntityType::FlowerPot:
// 		return make_unique<FlowerPotBlockEntity>(pos);
// 	case BlockEntityType::BrewingStand:
// 		return make_unique<BrewingStandBlockEntity>(pos);
// 	case BlockEntityType::EnchantingTable:
// 		return make_unique<EnchantingTableEntity>(pos);
// 	case BlockEntityType::DaylightDetector:
// 		return make_unique<DaylightDetectorBlockEntity>(pos);
// 	case BlockEntityType::Music:
// 		return make_unique<MusicBlockEntity>(pos);
// 	case BlockEntityType::Comparator:
// 		return make_unique<ComparatorBlockEntity>(pos);
// 	case BlockEntityType::Dispenser:
// 		return make_unique<DispenserBlockEntity>(pos);
// 	case BlockEntityType::Dropper:
// 		return make_unique<DropperBlockEntity>(pos);
// 	case BlockEntityType::Hopper:
// 		return make_unique<HopperBlockEntity>(pos);
// 	case BlockEntityType::Cauldron:
// 		return make_unique<CauldronBlockEntity>(pos);
// 	case BlockEntityType::Beacon:
// 		return make_unique<BeaconBlockEntity>(pos);
// 	case BlockEntityType::ItemFrame:
// 		return make_unique<ItemFrameBlockEntity>(pos);
// 	case BlockEntityType::PistonArm:
// 		return make_unique<PistonBlockEntity>(pos, blockID == Block::mStickyPiston->mID);	
// 	case BlockEntityType::MovingBlock:
// 		return make_unique<MovingBlockEntity>(pos);
// 	case BlockEntityType::EndPortal:
// 		return make_unique<EndPortalBlockEntity>(pos);
// 	case BlockEntityType::EnderChest:
// 		return make_unique<ChestBlockEntity>(BlockEntityType::EnderChest, "EnderChest", TR_ENDERCHEST_RENDERER, pos);
// 	case BlockEntityType::EndGateway:
// 		return make_unique<EndGatewayBlockEntity>(pos);
// 	case BlockEntityType::StructureBlock:
// 		return make_unique<StructureBlockEntity>(pos);
// 	case BlockEntityType::Chalkboard:
// 		return make_unique<ChalkboardBlockEntity>(pos);
// 	default:
// 		LOGE("Can't find TileEntity of type: %d\n", static_cast<int>(type));
// 		return nullptr;
// 	}

	return nullptr;
}

void BlockEntity::setId(BlockEntityType type, const std::string& id) {
	//make class things, but only once pls (was a static)
	MapIdType::const_iterator cit = idClassMap.find(id);
	if (cit == idClassMap.end()) {
		idClassMap.insert(std::make_pair(id, type));
		classIdMap.insert(std::make_pair(type, id));
	}
	else{
		DEBUG_FAIL("Pair added twice");
	}
}

void BlockEntity::initBlockEntities() {
	setId(BlockEntityType::Furnace, "Furnace");
	setId(BlockEntityType::Chest, "Chest");
	setId(BlockEntityType::NetherReactor, "NetherReactor");
	setId(BlockEntityType::Sign, "Sign");
	setId(BlockEntityType::MobSpawner, "MobSpawner");
	setId(BlockEntityType::EnchantingTable, "EnchantTable");
	setId(BlockEntityType::Skull, "Skull");
	setId(BlockEntityType::FlowerPot, "FlowerPot");
	setId(BlockEntityType::BrewingStand, "BrewingStand");
	setId(BlockEntityType::DaylightDetector, "DaylightDetector");
	setId(BlockEntityType::Music, "Music");
	setId(BlockEntityType::Comparator, "Comparator");
	setId(BlockEntityType::Dispenser, "Dispenser");
	setId(BlockEntityType::Dropper, "Dropper");
	setId(BlockEntityType::Hopper, "Hopper");
	setId(BlockEntityType::Cauldron, "Cauldron");
	setId(BlockEntityType::ItemFrame, "ItemFrame");
	setId(BlockEntityType::PistonArm, "PistonArm");
	setId(BlockEntityType::Beacon, "Beacon");
	setId(BlockEntityType::MovingBlock, "MovingBlock");
	setId(BlockEntityType::EndPortal, "EndPortal");
	setId(BlockEntityType::EnderChest, "EnderChest");
	setId(BlockEntityType::EndGateway, "EndGateway");
	setId(BlockEntityType::StructureBlock, "StructureBlock");
	setId(BlockEntityType::Chalkboard, "ChalkboardBlock");
}

//
// BlockEntity
//

BlockEntity::BlockEntity(BlockEntityType type, const BlockPos& pos, const std::string& id)
	: mBlock(nullptr)
	, mDestroyTimer(0)
	, mDestroyProgress(0.0f)
	, mPosition(pos)
	, mBB(pos, pos + Vec3::ONE)
	, mData(-1)
	, mType(type)
	, mRunningId(++_runningId)
	, mClientSideOnly(false)
	, mRendererId(TR_DEFAULT_RENDERER) {

	//init position stuff
	_resetAABB();
}

BlockEntity::~BlockEntity() {
#ifdef ENABLE_DEBUG_RENDERING
	if (this == DebugRenderer::getDebugBlockEntity()) {
		DebugRenderer::clearDebugBlock();
	}
#endif
}

void BlockEntity::load( const CompoundTag& tag ) {
	if (tag.contains("isMovable")) {
		mIsMovable = tag.getBoolean("isMovable");
	}
}

bool BlockEntity::save( CompoundTag& tag ) {
	MapTypeId::const_iterator it = classIdMap.find(mType);
	if (it == classIdMap.end()) {
		LOGE("TileEntityType %d is missing a mapping! This is a bug!\n", static_cast<int>(mType));
		return false;
	}
	tag.putString("id", it->second);
	tag.putInt("x", mPosition.x);
	tag.putInt("y", mPosition.y);
	tag.putInt("z", mPosition.z);
	tag.putBoolean("isMovable", mIsMovable);
	return true;
}

void BlockEntity::tick(BlockSource& region ) {
	++mTickCount;

	if (!mBlock) {//first tick
		mBlock = Block::mBlocks[region.getBlockID(mPosition)];
	}

	if (mChanged) {
		region.fireBlockEntityChanged(*this);
		onChanged(region);
		mChanged = false;
	}
}

bool BlockEntity::isFinished() {
	return false;
}

Unique<BlockEntity> BlockEntity::loadStatic(const CompoundTag& tag) {
	std::string id = tag.getString("id");
	MapIdType::const_iterator cit = idClassMap.find(id);
	if (cit != idClassMap.end()) {
		//LOGI("Loading Tile Entity @ %d,%d,%d\n", ));

		//the block entities will never change position, so it has to be known beforehand
		BlockPos pos;
		pos.x = tag.getInt("x");
		pos.y = tag.getInt("y");
		pos.z = tag.getInt("z");

		if (auto entity = BlockEntityFactory::createBlockEntity(cit->second, pos, (BlockID)0/*this id is not important because it will load data relay on this after this*/)) {
			//LOGI("Loading TileEntity: %d\n", entity->type);
			entity->load(tag);
			return entity;
		}
		else{
			LOGE("Couldn't create TileEntity of type: '%d'\n", static_cast<int>(cit->second));
		}
	}
	else {
		LOGE("Couldn't find TileEntity type: '%s'\n", id.c_str());
	}
	return nullptr;
}

void BlockEntity::setChanged() {
	mChanged = true;
}

void BlockEntity::onChanged(BlockSource& region) {

}

void BlockEntity::setMovable(bool canMove) { 
	mIsMovable = canMove; 
}

bool BlockEntity::isMovable() { 
	return mIsMovable; 
}

float BlockEntity::distanceToSqr(const Vec3& to) {
	return to.distanceToSqr(mPosition + Vec3(0.5f));
}

// Unique<Packet> BlockEntity::getUpdatePacket(BlockSource& region) {
// 	return nullptr;
// }

void BlockEntity::onUpdatePacket(const CompoundTag& data, BlockSource& region) {

}

void BlockEntity::onMove() {

}

void BlockEntity::onRemoved(BlockSource& region) {

}

void BlockEntity::triggerEvent( int b0, int b1 ) {
}

void BlockEntity::clearCache() {
	mBlock = nullptr;
	mData = -1;
}

bool BlockEntity::isType( BlockEntityType type ) {
	return mType == type;
}

bool BlockEntity::isType(BlockEntity& te, BlockEntityType type) {
	return te.isType(type);
}

bool BlockEntity::isInWorld() const {
	return mPosition != BlockPos::MIN;
}

void BlockEntity::onNeighborChanged(BlockSource& region, const BlockPos& position) {

}

float BlockEntity::getShadowRadius(BlockSource& region) const {	//todo WHYYYYYY ISNT THIS INHERITING ENTITY
	return 0;
}

bool BlockEntity::hasAlphaLayer() const {
	return false;
}

void BlockEntity::stopDestroy() {
	mDestroyProgress = 0;
	mDestroyTimer = 0;
}

BlockEntity& BlockEntity::getCrackEntity(BlockSource& region, const BlockPos& pos) {
	return *this;
}

const AABB& BlockEntity::getAABB() const {
	return mBB;
}

void BlockEntity::setBB(AABB value) {
	mBB = value;
}

const BlockPos& BlockEntity::getPosition() const {
	return mPosition;
}

int BlockEntity::getData() const {
	return mData;
}

void BlockEntity::setData(int value) {
	mData = value;
}

void BlockEntity::moveTo(const BlockPos& newPos) {
	mPosition = newPos;
}

const BlockEntityType& BlockEntity::getType() const {
	return mType;
}

int BlockEntity::getRunningId() const {
	return mRunningId;
}

void BlockEntity::setRunningId(int value) {
	mRunningId = value;
}

bool BlockEntity::isClientSideOnly() const {
	return mClientSideOnly;
}

void BlockEntity::setClientSideOnly(bool value) {
	mClientSideOnly = value;
}

const BlockEntityRendererId& BlockEntity::getRendererId() const {
	return mRendererId;
}

void BlockEntity::setRendererId(BlockEntityRendererId value) {
	mRendererId = value;
}

void BlockEntity::getDebugText(std::vector<std::string>& outputInfo) {
	if (mBlock) {
		mBlock->getDebugText(outputInfo);
		outputInfo.push_back("");
	}
	else {
		outputInfo.push_back("No valid block");
	}
	outputInfo.push_back("Type: " + classIdMap.at(getType()));
	outputInfo.push_back("Position: " + getPosition().toString());
	outputInfo.push_back("Data: " + Util::toString(mData));
}

const Block* BlockEntity::getBlock() {
	return mBlock;
}

void BlockEntity::_resetAABB() {
	Vec3 min = mPosition;
	Vec3 max = min + Vec3::ONE;
	mBB.set(min, max);
}

void BlockEntity::_destructionWobble(float& x, float& y, float& z) {
	if (mDestroyProgress > 0 ) {
		mDestroyTimer += 1.f / 60.f;

		if (mDestroyTimer > 0.2f) {
			const static float a = 0.15f;
			float s = 0.01f + 0.05f * mDestroyProgress;
			Vec3 o = mDestroyDirection * s;
			o.xRot((Math::random() - 0.5f) * Math::TAU * a);
			o.yRot((Math::random() - 0.5f) * Math::TAU * a);

			x += o.x;
			y += o.y;
			z += o.z;

			mDestroyTimer = 0;
		}
	}
}
