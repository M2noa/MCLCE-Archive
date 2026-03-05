/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/ComparatorBlock.h"
// #include "world/level/block/entity/ComparatorBlockEntity.h"
#include "world/level/block/entity/BlockEntity.h"
#include "world/item/Item.h"
#include "world/level/BlockSource.h"
#include "world/level/dimension/Dimension.h"
// #include "world/level/BlockTickingQueue.h"
#include "world/level/material/Material.h"
#include "world/level/Level.h"
#include "world/entity/Mob.h"
#include "world/entity/player/Player.h"
#include "world/Direction.h"
#include "world/redstone/Redstone.h"
// #include "world/redstone/circuit/CircuitSystem.h"
// #include "world/redstone/circuit/components/ComparatorCapacitor.h"
#include "world/Facing.h"
#include "world/item/ItemInstance.h"

ComparatorBlock::ComparatorBlock(const std::string& nameId, int id, bool on)
	: EntityBlock(nameId, id, Material::getMaterial(MaterialType::Decoration)) {
	mBlockEntityType = BlockEntityType::Comparator;
	setVisualShape(Vec3::ZERO, Vec3(1, 2.0f / 16.0f, 1));
	mRenderLayer = BlockRenderLayer::RENDERLAYER_ALPHATEST_SINGLE_SIDE;
	mProperties = BlockProperty::BreakOnPush;
	setSolid(false);
	mOn = on;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

int ComparatorBlock::getVariant(int data) const {
	return mOn ? 1 : 0;
}

bool ComparatorBlock::shouldRenderFace(BlockSource& region, const BlockPos& pos, FacingID face, const AABB& shape) const {
	if (face == Facing::DOWN || face == Facing::UP) {
		// up and down is a special case handled by the shape renderer
		return false;
	}
	return true;
}

bool ComparatorBlock::use(Player& player, const BlockPos& pos) const {
	BlockSource& region = player.getRegion();

	DataID data = region.getData(pos);
	bool subtractBit = getBlockState(BlockState::OutputSubtractBit).getBool(data);
	bool litBit = getBlockState(BlockState::OutputLitBit).getBool(data);

	getBlockState(BlockState::OutputSubtractBit).set(data, !subtractBit);

// 	if (!region.getLevel().isClientSide()) {
// 		auto& circuit = region.getDimension().getCircuitSystem();
// 		auto component = circuit.getComponent<ComparatorCapacitor>(pos);
// 		if (component != nullptr) {
// 			circuit.lockGraph(true);
// 			bool isLit = component->getStrength() > 0 || litBit;
// 			getBlockState(BlockState::OutputLitBit).set(data, isLit);
// 
// 			component->setMode(subtractBit ? ComparatorCapacitor::Mode::COMPARE_MODE : ComparatorCapacitor::Mode::SUBTRACT_MODE);
// 			_refreshOutputState(region, pos, component->getStrength());
// 			circuit.lockGraph(false);
// 		}
// 	}
// 
// 	region.getLevel().broadcastDimensionEvent(region, LevelEvent::ActivateBlock, pos + Vec3(0.5f, 0.5f, 0.5f), subtractBit ? 500 : 550);
	region.setBlockAndData(pos, {mID, data}, UPDATE_ALL, &player);

	return true;
}

bool ComparatorBlock::isInteractiveBlock() const {
	return true;
}

void ComparatorBlock::onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const {
	if (!region.getLevel().isClientSide()) {
		_refreshOutputState(region, pos, strength);
	}
}

void ComparatorBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	EntityBlock::onPlace(region, pos);

	_installCircuit(region, pos, false);
}

void ComparatorBlock::onLoaded(BlockSource& region, const BlockPos& pos) const {
	EntityBlock::onLoaded(region, pos);

	_installCircuit(region, pos, true);
}

bool ComparatorBlock::mayPlace(BlockSource& region, const BlockPos& pos) const {
	if (!region.canProvideSupport(pos.below(), Facing::UP, BlockSupportType::Any)) {
		return false;
	}
	return EntityBlock::mayPlace(region, pos);
}

bool ComparatorBlock::canSurvive(BlockSource& region, const BlockPos& pos) const {
	if (!region.canProvideSupport(pos.below(), Facing::UP, BlockSupportType::Any)) {
		return false;
	}
	return EntityBlock::canSurvive(region, pos);
}

int ComparatorBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	return (((Math::floor(by.mRot.y * 4 / (360) + 0.5f)) & 3) + 2) % 4;
}

void ComparatorBlock::triggerEvent(BlockSource& region, const BlockPos& pos, int b0, int b1) const {
	EntityBlock::triggerEvent(region, pos, b0, b1);
	BlockEntity *be = region.getBlockEntity(pos);
	if (be != nullptr) {
		return be->triggerEvent(b0, b1);
	}
}

int ComparatorBlock::getDirectSignal(BlockSource& region, const BlockPos& pos, int dir) const {
	return getSignal(region, pos, dir);
}

bool ComparatorBlock::shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const
{
	return true;
}

int ComparatorBlock::getSignal(BlockSource& region, const BlockPos& pos, int dir) const {

	int dirFacing = Direction::DIRECTION_FACING[getBlockState(BlockState::Direction).get<int>(region.getData(pos))];
	if (dir != dirFacing) {
		return Redstone::SIGNAL_NONE;
	}
	return (mOn ? Redstone::SIGNAL_MAX : Redstone::SIGNAL_NONE);
}

bool ComparatorBlock::isSignalSource() const {
	return true;
}

void ComparatorBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	if (!canSurvive(region, pos)) {
		popResource(region, pos, ItemInstance(Item::mComparator));
		region.removeBlock(pos);
	}
}

int ComparatorBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Item::mComparator->getId();
}

ItemInstance ComparatorBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(Item::mComparator);
}

bool ComparatorBlock::canBeSilkTouched() const {
	return false;
}

bool ComparatorBlock::isSubtractMode(BlockSource& region, const BlockPos& pos) const {
	return getBlockState(BlockState::OutputSubtractBit).getBool(region.getData(pos));
}

void ComparatorBlock::_installCircuit(BlockSource &region, const BlockPos& pos, bool bLoading) const {
// 	if (!region.getLevel().isClientSide()) {
// 		int dir = Direction::DIRECTION_FACING[getBlockState(BlockState::Direction).get<int>(region.getData(pos))];
// 
// 		auto& circuit = region.getDimension().getCircuitSystem();
// 		auto component = static_cast<ComparatorCapacitor*>(circuit.create<ComparatorCapacitor>(pos, &region, dir));
// 		if (component) {
// 
// 			if ((bLoading == true) && (mID == Block::mPoweredComparator->mID)) {
// 				auto be = region.getBlockEntity(pos);
// 				if (be != nullptr && be->getType() == BlockEntityType::Comparator) {
// 					auto comparator = static_cast<ComparatorBlockEntity*>(be);
// 					component->setCachedStrength(comparator->getOutputSignal());
// 				}
// 			}
// 
// 			component->allowAttachments(true);
// 			component->setMode(isSubtractMode(region, pos) ? ComparatorCapacitor::Mode::SUBTRACT_MODE : ComparatorCapacitor::Mode::COMPARE_MODE);
// 		}
// 	}
}

void ComparatorBlock::_refreshOutputState(BlockSource& region, const BlockPos& pos, int strength) const {
// 	DataID data = region.getData(pos);
// 	auto& circuit = region.getDimension().getCircuitSystem();
// 	auto component = circuit.getComponent<ComparatorCapacitor>(pos);
// 	if (component != nullptr) {
// 		circuit.lockGraph(true);
// 		int oldStrength = component->getOldStrength();
// 		if (strength != oldStrength || !component->isSubtractMode()) {
// 			if (oldStrength > 0 && strength == 0) {
// 				getBlockState(BlockState::OutputLitBit).set(data, false);
// 				region.setBlockAndData(pos, Block::mUnpoweredComparator->mID, data, UPDATE_ALL);
// 			}
// 			else if (oldStrength == 0 && strength > 0) {
// 				getBlockState(BlockState::OutputLitBit).set(data, true);
// 				region.setBlockAndData(pos, Block::mPoweredComparator->mID, data, UPDATE_ALL);
// 			}
// 
// 			//	Let the comparator block entity know it's strength for when we load/save.
// 			auto be = region.getBlockEntity(pos);
// 			if (be != nullptr && be->getType() == BlockEntityType::Comparator) {
// 				auto comparator = static_cast<ComparatorBlockEntity*>(be);
// 				comparator->setOutputSignal(strength);
// 			}
// 		}
// 		circuit.lockGraph(false);
// 	}
}
