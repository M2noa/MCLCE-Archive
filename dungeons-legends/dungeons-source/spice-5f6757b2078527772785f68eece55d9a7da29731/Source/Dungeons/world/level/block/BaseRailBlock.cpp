/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/BaseRailBlock.h"

#include "world/item/ItemInstance.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/material/Material.h"
#include "world/Facing.h"
// #include "world/redstone/circuit/CircuitSystem.h"
// #include "world/redstone/circuit/components/BaseRailTransporter.h"
// #include "world/redstone/circuit/components/ConsumerComponent.h"
const DataID BaseRailBlock::DIR_FLAT_Z(0);
const DataID BaseRailBlock::DIR_FLAT_X(1);

BaseRailBlock::Rail::Rail(BlockSource& region, const BlockPos& pos)
	: mRegion(region)
	, mPos(pos) {
	auto t = region.getBlockAndData(pos);

	const Block* railBlock = Block::mBlocks[t.id];
	if (railBlock && ((BaseRailBlock*)railBlock)->mUsesDataBit && 
		railBlock->getBlockState(BlockState::RailDataBit).isInitialized()) {
		mUsesDataBit = true;
		railBlock->getBlockState(BlockState::RailDataBit).set(t.data, 0);
	}
	else {
		mUsesDataBit = false;
	}
	updateConnections(t.data);
}

void BaseRailBlock::Rail::updateConnections(int direction) {
	mConnections.clear();

	const BlockPos north = mPos.north();	// z-1
	const BlockPos south = mPos.south();	// z+1
	const BlockPos east = mPos.east();	// x+1
	const BlockPos west = mPos.west();	// x-1

	if (direction == DIR_FLAT_Z) {
		mConnections.push_back(north);
		mConnections.push_back(south);
	}
	else if (direction == DIR_FLAT_X) {
		mConnections.push_back(west);
		mConnections.push_back(east);
	}
	else if (direction == 2) {
		mConnections.push_back(west.above());
		mConnections.push_back(east);
	}
	else if (direction == 3) {
		mConnections.push_back(west.above());
		mConnections.push_back(east);
	}
	else if (direction == 4) {
		mConnections.push_back(north.above());
		mConnections.push_back(south);
	}
	else if (direction == 5) {
		mConnections.push_back(north);
		mConnections.push_back(south.above());
	}
	else if (direction == 6) {
		mConnections.push_back(east);
		mConnections.push_back(south);
	}
	else if (direction == 7) {
		mConnections.push_back(west);
		mConnections.push_back(south);
	}
	else if (direction == 8) {
		mConnections.push_back(west);
		mConnections.push_back(north);
	}
	else if (direction == 9) {
		mConnections.push_back(east);
		mConnections.push_back(north);
	}
}

void BaseRailBlock::Rail::removeSoftConnections() {
	for (std::vector<BlockPos>::iterator it = mConnections.begin(); it != mConnections.end(); ) {
		std::shared_ptr<BaseRailBlock::Rail> rail = getRail(*it);
		if (rail == nullptr || !rail->connectsTo(*this)) {
			it = mConnections.erase(it);
		}
		else {
			*it = rail->mPos;
			it++;
		}
	}
}

std::shared_ptr<BaseRailBlock::Rail> BaseRailBlock::Rail::getRail(const BlockPos& p) {
	if (BaseRailBlock::isRail(mRegion, p)) {
		return make_shared<Rail>(mRegion, p);
	}
	if (BaseRailBlock::isRail(mRegion, p.above())) {
		return make_shared<Rail>(mRegion, p.above());
	}
	if (BaseRailBlock::isRail(mRegion, p.below())) {
		return make_shared<Rail>(mRegion, p.below());
	}
	return std::shared_ptr<BaseRailBlock::Rail>(nullptr);
}

bool BaseRailBlock::Rail::connectsTo(Rail& rail) {
	for (size_t i = 0; i < mConnections.size(); i++) {
		BlockPos p = mConnections[i];
		if (p.x == rail.mPos.x && p.z == rail.mPos.z) {
			return true;
		}
	}
	return false;
}

bool BaseRailBlock::Rail::hasConnection(const BlockPos& pos) {
	for (size_t i = 0; i < mConnections.size(); i++) {
		BlockPos& p = mConnections[i];
		if (p.x == pos.x && p.z == pos.z) {
			return true;
		}
	}
	return false;
}

bool BaseRailBlock::Rail::canConnectTo(Rail& rail) {
	if (connectsTo(rail)) {
		return true;
	}
	if (mConnections.size() == 2) {
		return false;
	}
	if (mConnections.empty()) {
		return true;
	}

	return true;
}

void BaseRailBlock::Rail::connectTo(Rail& rail) {
	mConnections.push_back(rail.mPos);

	bool n = hasConnection(mPos.north());
	bool s = hasConnection(mPos.south());
	bool w = hasConnection(mPos.west());
	bool e = hasConnection(mPos.east());

	DataID dir(0xff);

	if (n || s) {
		dir = DIR_FLAT_Z;
	}
	if (w || e) {
		dir = DIR_FLAT_X;
	}
	if (!mUsesDataBit) {
		if (s && e && !n && !w) {
			dir = (DataID)6;
		}
		if (s && w && !n && !e) {
			dir = (DataID)7;
		}
		if (n && w && !s && !e) {
			dir = (DataID)8;
		}
		if (n && e && !s && !w) {
			dir = (DataID)9;
		}
	}

	BlockPos above = mPos.above();
	if (dir == DIR_FLAT_Z) {
		if (isRail(mRegion, above.north())) {
			dir = (DataID)4;
		}
		if (isRail(mRegion, above.south())) {
			dir = (DataID)5;
		}
	}
	if (dir == DIR_FLAT_X) {
		if (isRail(mRegion, above.east())) {
			dir = (DataID)2;
		}
		if (isRail(mRegion, above.west())) {
			dir = (DataID)3;
		}
	}

	if (dir < 0) {
		dir = DIR_FLAT_Z;
	}

	auto block = mRegion.getBlockAndData(mPos);

	DataID data = 0;
	const Block* railBlock = Block::mBlocks[block.id];
	if (railBlock && railBlock->getBlockState(BlockState::RailDirection).isInitialized()) {
		Block::mBlocks[block.id]->getBlockState(BlockState::RailDirection).set(data, dir);
	}

	if (mUsesDataBit && railBlock && railBlock->getBlockState(BlockState::RailDataBit).isInitialized()) {
		int railBit = railBlock->getBlockState(BlockState::RailDataBit).get<int>(block.data);
		railBlock->getBlockState(BlockState::RailDataBit).set(data, railBit);
	}

	mRegion.setBlockAndData(mPos, {mRegion.getBlockID(mPos), data}, Block::UPDATE_ALL);

	BaseRailBlock::_createCircuitComponent(mRegion, mPos, data);
}

void BaseRailBlock::_createCircuitComponent(BlockSource& region, const BlockPos& pos, DataID data) {

// 	auto block = region.getBlockAndData(pos);
// 	if (block.id == Block::mGoldenRail->mID || block.id == Block::mActivatorRail->mID) {
// 		FacingID facingDir = Facing::SOUTH;
// 		if (BaseRailBlock::_isFacingWestEast(region, pos)) {
// 			facingDir = Facing::WEST;
// 		}
// 
// 		if (!region.getLevel().isClientSide()) {
// 			auto& circuit = region.getDimension().getCircuitSystem();
// 			 
// 			if (circuit.isAvailableAt(pos)) {
// 				circuit.removeComponents(pos);
// 			}
// 
// 			auto component = circuit.create<BaseRailTransporter>(pos, &region, facingDir);
// 			if (component) {
// 				if (block.id == Block::mGoldenRail->mID) {
// 					component->setType(BaseRailTransporter::RailType::Power);
// 				}
// 				else {
// 					component->setType(BaseRailTransporter::RailType::Activator);
// 				}
// 			}
// 		 
// 		}
// 	}
// 	 
// 	else {
// 		const Block* railBlock = Block::mBlocks[block.id];
// 		if (railBlock && railBlock->getBlockState(BlockState::RailDirection).isInitialized()) {
// 			auto dir = Block::mBlocks[block.id]->getBlockState(BlockState::RailDirection).get<DataID>(block.data);
// 			if (dir >= 6) {
// 				if (!region.getLevel().isClientSide()) {
// 					auto& circuit = region.getDimension().getCircuitSystem();
// 					if (circuit.isAvailableAt(pos) == false) {
// 						circuit.create<ConsumerComponent>(pos, &region);
// 					}
// 				}
// 			}
// 		}
// 	}
}


bool isConnectedToRail(BlockSource &region, const BlockPos &pos)
{
	auto id = region.getBlockID(pos);
	return (id == Block::mGoldenRail->mID || id == Block::mRail->mID);

}


void BaseRailBlock::onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const {
// 	if (region.getLevel().isClientSide() == false) {
// 
// 		//don't change the order the first time
// 		if (isFirstTime == true) {
// 			return;
// 		}
// 
//  
// 		auto& circuit = region.getDimension().getCircuitSystem();
// 		circuit.lockGraph(true);
// 		Rail rail(region, pos);
// 		rail.place(strength != 0, true);
// 	 
// 		circuit.lockGraph(false);
// 	}
}



bool BaseRailBlock::Rail::hasNeighborRail(const BlockPos& pos) {
	std::shared_ptr<BaseRailBlock::Rail> neighbor = getRail(pos);
	if (neighbor == nullptr) {
		return false;
	}
	neighbor->removeSoftConnections();
	return neighbor->canConnectTo(*this);
}

void BaseRailBlock::Rail::place(bool hasSignal, bool first) {
	bool n = hasNeighborRail(mPos.north());
	bool s = hasNeighborRail(mPos.south());
	bool w = hasNeighborRail(mPos.west());
	bool e = hasNeighborRail(mPos.east());

	DataID dir(0xff);

	if ((n || s) && !w && !e) {
		dir = DIR_FLAT_Z;
	}
	if ((w || e) && !n && !s) {
		dir = DIR_FLAT_X;
	}

	if (!mUsesDataBit) {
		if (s && e && !n && !w) {
			dir = (DataID)6;
		}
		if (s && w && !n && !e) {
			dir = (DataID)7;
		}
		if (n && w && !s && !e) {
			dir = (DataID)8;
		}
		if (n && e && !s && !w) {
			dir = (DataID)9;
		}
	}
	if (dir == 0xff) {
		if (n || s) {
			dir = DIR_FLAT_Z;
		}
		if (w || e) {
			dir = DIR_FLAT_X;
		}

		if (!mUsesDataBit) {
			if (hasSignal) {
				if (s && e) {
					dir = (DataID)6;
				}
				if (w && s) {
					dir = (DataID)7;
				}
				if (e && n) {
					dir = (DataID)9;
				}
				if (n && w) {
					dir = (DataID)8;
				}
			}
			else {
				if (n && w) {
					dir = (DataID)8;
				}
				if (e && n) {
					dir = (DataID)9;
				}
				if (w && s) {
					dir = (DataID)7;
				}
				if (s && e) {
					dir = (DataID)6;
				}
			}
		}
	}

	const BlockPos above = mPos.above();
	if (dir == DIR_FLAT_Z) {
		if (isRail(mRegion, above.north())) {
			dir = (DataID)4;
		}
		if (isRail(mRegion, above.south())) {
			dir = (DataID)5;
		}
	}
	if (dir == DIR_FLAT_X) {
		if (isRail(mRegion, above.east())) {
			dir = (DataID)2;
		}
		if (isRail(mRegion, above.west())) {
			dir = (DataID)3;
		}
	}

	if (dir > 0xf) {
		dir = DIR_FLAT_Z;
	}

	updateConnections(dir);

	auto block = mRegion.getBlockAndData(mPos);

	DataID data = 0;
	const Block* railBlock = Block::mBlocks[block.id];
	if (railBlock && railBlock->getBlockState(BlockState::RailDirection).isInitialized()) {
		railBlock->getBlockState(BlockState::RailDirection).set(data, dir);
	}

	if (mUsesDataBit && hasSignal && railBlock && railBlock->getBlockState(BlockState::RailDataBit).isInitialized()) {
		railBlock->getBlockState(BlockState::RailDataBit).set(data, 1);
	}

	if (first || mRegion.getData(mPos) != data) {
		mRegion.setBlockAndData(mPos, {mRegion.getBlockID(mPos), data}, Block::UPDATE_CLIENTS);

		BaseRailBlock::_createCircuitComponent(mRegion, mPos, data);

		for (size_t i = 0; i < mConnections.size(); i++) {
			std::shared_ptr<BaseRailBlock::Rail> neighbor = getRail(mConnections[i]);
			if (neighbor == nullptr) {
				continue;
			}
			neighbor->removeSoftConnections();

			if (neighbor->canConnectTo(*this)) {
				neighbor->connectTo(*this);
			}
		}
	}
}

bool BaseRailBlock::_isFacingWestEast(BlockSource& region, const BlockPos& pos) {

	auto block = region.getBlockAndData(pos);

	int dir = 0;
	const Block* railBlock = Block::mBlocks[block.id];
	if (railBlock && railBlock->getBlockState(BlockState::RailDirection).isInitialized()) {
		dir = railBlock->getBlockState(BlockState::RailDirection).get<int>(block.data);
	}

	return (dir == 2 || dir == 3 || dir == DIR_FLAT_X);
}

bool BaseRailBlock::isRail(BlockSource& region, const BlockPos& pos) {
	return isRail(region.getBlockID(pos));
}

bool BaseRailBlock::isRail(int id) {
	return id == Block::mRail->mID || id == Block::mGoldenRail->mID || id == Block::mDetectorRail->mID || id == Block::mActivatorRail->mID;
}

BaseRailBlock::BaseRailBlock(const std::string& nameId, int id, bool usesDataBit)
	: Block(nameId, id, Material::getMaterial(MaterialType::Decoration))
	, mUsesDataBit(usesDataBit) {
	setVisualShape(Vec3::ZERO, Vec3(1, 2 / 16.0f, 1));

	setSolid(false);
	mProperties = BlockProperty::Unspecified;
	mRenderLayer = RENDERLAYER_ALPHATEST;
	mThickness = 0.07f;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

bool BaseRailBlock::isUsesDataBit() const {
	return mUsesDataBit;
}

const AABB& BaseRailBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	if (isClipping) {
		int data = getBlockState(BlockState::RailDirection).get<int>(region.getData(pos));
		bool isSlope = data >= 2 && data <= 6;
		const float normalHeight = 1.0f / 16.0f;
		const float slopeHeight = 0.5f;
		return bufferValue.set(Vec3::ZERO, Vec3(1, isSlope ? slopeHeight : normalHeight, 1)).move(Vec3(pos));
	}
	else {
		return AABB::EMPTY;
	}
}

const AABB& BaseRailBlock::getVisualShape(DataID data, AABB& bufferAABB, bool isClipping) const {
	if (data >= 2 && data <= 5) {
		bufferAABB.set(0, 0, 0, 1, 2 / 16.0f + 0.5f, 1);
	}
	else {
		bufferAABB.set(0, 0, 0, 1, 2 / 16.0f, 1);
	}
	return bufferAABB;
}

int BaseRailBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 1;
}

bool BaseRailBlock::mayPlace(BlockSource& region, const BlockPos& pos) const {
	//Check the Block below what we're looking at
	return region.canProvideSupport(pos.below(), Facing::UP, BlockSupportType::Edge);
}

void BaseRailBlock::_updatePlacement(BlockSource& region, const BlockPos& pos, int type) const {
	if (region.getLevel().isClientSide()) {
		return;
	}

	auto t = region.getBlockAndData(pos);
	auto dir = t.data;

	const Block* railBlock = Block::mBlocks[t.id];
	if (mUsesDataBit && railBlock->getBlockState(BlockState::RailDataBit).isInitialized()) {
		railBlock->getBlockState(BlockState::RailDataBit).set<DataID>(dir, 0);
	}

	if (!region.canProvideSupport(pos.below(), Facing::UP, BlockSupportType::Edge) ||
		(dir == 2 && !region.canProvideSupport(pos.east(), Facing::UP, BlockSupportType::Edge)) ||
		(dir == 3 && !region.canProvideSupport(pos.west(), Facing::UP, BlockSupportType::Edge)) ||
		(dir == 4 && !region.canProvideSupport(pos.north(), Facing::UP, BlockSupportType::Edge)) ||
		(dir == 5 && !region.canProvideSupport(pos.south(), Facing::UP, BlockSupportType::Edge))) {
		spawnResources(region, pos, t.data, 1);
		region.setBlockAndData(pos, FullBlock(), Block::UPDATE_ALL);
	}
	else {
		updateState(region, pos, t.data, dir, type);
	}


}

void BaseRailBlock::onLoaded(BlockSource& region, const BlockPos& pos) const {
	auto t = region.getBlockAndData(pos);
	BaseRailBlock::_createCircuitComponent(region, pos, t.data);
}

bool BaseRailBlock::isRailBlock() const {
	return true;
}

void BaseRailBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	if (!region.getLevel().isClientSide()) {

		updateDir(region, pos, true);

		if (mUsesDataBit) {
			_updatePlacement(region, pos, mID);
		}
	}

	onLoaded(region, pos);
}

void BaseRailBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	_updatePlacement(region, pos, region.getBlockID(neighborPos));
}

void BaseRailBlock::updateState(BlockSource& region, const BlockPos& pos, int data, int dir, int type) const {
}

void BaseRailBlock::updateDir(BlockSource& region, const BlockPos& pos, bool first) const {
// 	if (region.getLevel().isClientSide() == false) {
// 		auto& circuit = region.getDimension().getCircuitSystem();
// 		Rail rail(region, pos);
// 		rail.place(circuit.getStrength(pos) != 0, first);
// 	}
}

ItemInstance BaseRailBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(mID, 1, 0);
}

