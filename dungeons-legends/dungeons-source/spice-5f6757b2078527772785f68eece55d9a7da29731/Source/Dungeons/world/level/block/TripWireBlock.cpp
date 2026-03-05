#include "Dungeons.h"

#include "TripWireBlock.h"
#include "world/level/Level.h"
#include "world/phys/AABB.h"
#include "world/item/Item.h"
#include "world/level/BlockSource.h"
#include "world/level/material/Material.h"
#include "world/entity/player/Player.h"
#include "TripWireHookBlock.h"
#include "world/Direction.h"
#include "world/entity/Entity.h"
#include "util/Random.h"
// #include "world/level/BlockTickingQueue.h"

TripWireBlock::TripWireBlock(const std::string& nameId, int id) 
	: Block(nameId, id, Material::getMaterial(MaterialType::Decoration)) {
	setVisualShape( Vec3::ZERO, Vec3(1, 2.5f / 16.0f, 1));
	setTicking(true);
	setSolid(false);
	mProperties = BlockProperty::BreakOnPush;
	mRenderLayer = RENDERLAYER_ALPHATEST;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

int TripWireBlock::getTickDelay() const {
	return 10;
}

const AABB& TripWireBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	if (isClipping) {
		getVisualShape(region, pos, bufferValue, true);
		return bufferValue.move(pos);
	}
	return AABB::EMPTY;
}

const AABB& TripWireBlock::getVisualShape(DataID data, AABB& bufferAABB, bool isClipping) const {
	bool attached = isAttached(data);
	bool suspended = isSuspended(data);

	if (!suspended) {
		bufferAABB.set(Vec3::ZERO, Vec3(1, 1.5f / 16.0f, 1));
	} else if (!attached) {
		bufferAABB.set(Vec3::ZERO, Vec3(1, 8.0f / 16.0f, 1));
	} else {
		bufferAABB.set(Vec3(0, 1.0f / 16.0f, 0), Vec3(1, 2.5f / 16.0f, 1));
	}

	return bufferAABB;
}

const AABB& TripWireBlock::getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping) const {
	return getVisualShape(region.getData(pos), bufferAABB, isClipping);
}

void TripWireBlock::onPlace(BlockSource& region, const BlockPos& pos) const {

	DataID data = 0;
	getBlockState(BlockState::SuspendedBit).set(data, true);

	FullBlock ft = region.getBlockAndData(pos);
	ft.data = data;
	region.setBlockAndData(pos, ft, Block::UPDATE_ALL);
	updateSource(region, pos, data);
}

void TripWireBlock::onRemove(BlockSource& region, const BlockPos& pos) const {
	Block::onRemove(region, pos);

}

bool TripWireBlock::playerWillDestroy(Player& player, const BlockPos& pos, int data) const {
	BlockSource& region = player.getRegion();

	DataID newData = data;

	if (player.getSelectedItem() != nullptr && player.getSelectedItem()->getItem() == Item::mShears) {
		getBlockState(BlockState::PoweredBit).set(newData, false);
		updateSource(region, pos, newData);
	}
	else {
		getBlockState(BlockState::PoweredBit).set(newData, true);
		updateSource(region, pos, newData);
	}

	return Block::playerWillDestroy(player, pos, data);
}

int TripWireBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Item::mString->getId();
}

ItemInstance TripWireBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(Item::mString);
}

ItemInstance TripWireBlock::getSilkTouchItemInstance(DataID data) const {
	return ItemInstance(Item::mString);
}

void TripWireBlock::updateSource(BlockSource &region, const BlockPos& pos, int data) const {
	for (int dir = 0; dir < 2; dir++) {
		for (int i = 1; i < TripWireHookBlock::WIRE_DIST_MAX; i++) {
			const BlockPos neighborPos(pos.x + Direction::STEP_X[dir] * i, pos.y, pos.z + Direction::STEP_Z[dir] * i);
			const Block& block = region.getBlock(neighborPos);

			if (block.isType(Block::mTripwireHook)) {
				int sourceDir = Block::mTripwireHook->getBlockState(BlockState::Direction).get<int>(region.getData(neighborPos));
				if (sourceDir == Direction::DIRECTION_OPPOSITE[dir]) {
					static_cast<const TripWireHookBlock*>(Block::mTripwireHook)->calculateState(region, neighborPos, false, region.getData(neighborPos), true, i, data);
				}

				break;
			}
			else if (!block.isType(Block::mTripwire)) {
				break;
			}
		}
	}
}

void TripWireBlock::entityInside(BlockSource& region, const BlockPos& pos, Entity& entity) const{
	if (region.getLevel().isClientSide()) {
		return;
	}

	if (isPowered(region.getData(pos))) {
		return;
	}

	checkPressed(region, pos);
}

void TripWireBlock::tick(BlockSource& region, const BlockPos& pos, Random& random) const {
	if (region.getLevel().isClientSide()) {
		return;
	}

	if (!isPowered(region.getData(pos))) {
		return;
	}

	checkPressed(region, pos);
}

void TripWireBlock::checkPressed(BlockSource& region, const BlockPos& pos) const {
	FullBlock fb = region.getBlockAndData(pos);
	bool wasPressed = isPowered(fb.data);
	bool shouldBePressed = false;

	AABB buffer, result;
	result = getVisualShape(fb.data, buffer);

	EntityList entities = region.getEntities(nullptr, AABB(pos.x + result.min.x, pos.y + result.min.y, pos.z + result.min.z, pos.x + result.max.x, pos.y + result.max.y, pos.z + result.max.z));
	if (!entities.empty()) {
		//for (auto e : entities) {
		//	if (!e->isIgnoringBlockTriggers()) {
				shouldBePressed = true;
		//		break;
		//	}
		//}
	}

	if (shouldBePressed && !wasPressed) {
		getBlockState(BlockState::PoweredBit).set(fb.data, true);
	}

	if (!shouldBePressed && wasPressed) {
		getBlockState(BlockState::PoweredBit).set(fb.data, false);
	}

	if (shouldBePressed != wasPressed) {
		region.setBlockAndData(pos, fb, Block::UPDATE_ALL);
		updateSource(region, pos, fb.data);
	}

// 	if (shouldBePressed) {
// 		region.getTickQueue(pos)->add(region, pos, mID, getTickDelay());
// 	}
}

bool TripWireBlock::shouldConnectTo(BlockSource& region, const BlockPos& pos, int data, int dir){
	BlockPos neighborPos(pos.x + Direction::STEP_X[dir], pos.y, pos.z + Direction::STEP_Z[dir]);
	const Block& block = region.getBlock(neighborPos);

	if (block.isType(Block::mTripwireHook)) {
		int otherData = region.getData(neighborPos);
		int facing = Block::mTripwireHook->getBlockState(BlockState::Direction).get<int>(otherData);

		return facing == Direction::DIRECTION_OPPOSITE[dir];
	}

	if (block.isType(Block::mTripwire)) {
		return true;
	}

	return false;
}

bool TripWireBlock::isPowered(DataID data) const {
	return getBlockState(BlockState::PoweredBit).getBool(data);
}

bool TripWireBlock::isSuspended(DataID data) const {
	return getBlockState(BlockState::SuspendedBit).getBool(data);
}

bool TripWireBlock::isAttached(DataID data) const {
	return getBlockState(BlockState::AttachedBit).getBool(data);
}

bool TripWireBlock::isDisarmed(DataID data) const {
	return getBlockState(BlockState::DisarmedBit).getBool(data);
}
