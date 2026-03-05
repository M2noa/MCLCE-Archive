
#include "Dungeons.h"

#include "world/level/block/RedstoneTorchBlock.h"

#include "world/Facing.h"
#include "world/item/ItemInstance.h"
#include "world/level/BlockSource.h"
// #include "world/level/BlockTickingQueue.h"
#include "world/level/Level.h"
#include "world/level/dimension/Dimension.h"
#include "world/redstone/Redstone.h"
// #include "world/redstone/circuit/CircuitSystem.h"
// #include "world/redstone/circuit/components/RedstoneTorchCapacitor.h"
// #include "world/entity/ParticleType.h"

RedstoneTorchBlock::RedstoneTorchBlock(const std::string& nameId, int id, bool on)
	: TorchBlock(nameId, id) 
	, mOn(on) {

	setTicking(false);

	mRenderLayer = BlockRenderLayer::RENDERLAYER_ALPHATEST_SINGLE_SIDE;	
}

void RedstoneTorchBlock::onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const {

	if (region.getLevel().isClientSide() == false) {

		DataID data = region.getData(pos);
		BlockID current = region.getBlockID(pos);
// 		auto &circuit = region.getDimension().getCircuitSystem();
		//Minecraft will change, but we don't want our graph to change during this time
// 		circuit.lockGraph(true);
		if (strength == 0) {
			if (current != Block::mUnlitRedStoneTorch->mID) {
				region.setBlockAndData(pos, Block::mUnlitRedStoneTorch->mID, data, UPDATE_CLIENTS);
			}
		}
		else {
			if (current != Block::mLitRedStoneTorch->mID) {
				region.setBlockAndData(pos, Block::mLitRedStoneTorch->mID, data, UPDATE_CLIENTS);
			}
		}
		//Ok, let others change now
// 		circuit.lockGraph(false);
	}
}

void RedstoneTorchBlock::onLoaded(BlockSource& region, const BlockPos& pos) const {
	installCircuit(region, pos, true);
}

void RedstoneTorchBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	onLoaded(region, pos);
	installCircuit(region, pos, false);	
}

void RedstoneTorchBlock::installCircuit(BlockSource &source, const BlockPos& pos, bool bLoading) const
{
	if (!source.getLevel().isClientSide()) {

// 		auto &circuit = source.getDimension().getCircuitSystem();

// 		if (circuit.isAvailableAt(pos) == false) {
// 			int dir = Facing::DOWN;
// 
// 			int torchAttach = getBlockState(BlockState::FacingDirection).get<int>(source.getData(pos));
// 			if (torchAttach == TorchBlock::TORCH_WEST) { dir = Facing::WEST; }
// 			else if (torchAttach == TorchBlock::TORCH_EAST) { dir = Facing::EAST; }
// 			else if (torchAttach == TorchBlock::TORCH_NORTH) { dir = Facing::NORTH; }
// 			else if (torchAttach == TorchBlock::TORCH_SOUTH) { dir = Facing::SOUTH; }
// 
// 			auto torchCapicator = circuit.create<RedstoneTorchCapacitor>(pos, &source, dir);
// 			if (torchCapicator) {
// 				bool bOn = source.getBlockID(pos) == Block::mLitRedStoneTorch->mID;
// 				torchCapicator->setOn(bOn, bLoading);
// 			}
// 		}
	}
}

void RedstoneTorchBlock::tick(BlockSource& region, const BlockPos& pos, Random& random) const {
}

void RedstoneTorchBlock::animateTick(BlockSource& region, const BlockPos& pos, Random& random) const {

	if(!mOn) {
		return;
	}

	auto &level = region.getLevel();

	int dir = getBlockState(BlockState::FacingDirection).get<int>(region.getData(pos));
	float x = pos.x + 0.5f + (random.nextFloat() - 0.5f) * 0.2f;
	float y = pos.y + 0.7f + (random.nextFloat() - 0.5f) * 0.2f;
	float z = pos.z + 0.5f + (random.nextFloat() - 0.5f) * 0.2f;
	float h = 0.22f;
	float r = 0.27f;

// 	if(dir == Facing::UP) {
// 		level.addParticle(ParticleType::RedDust, Vec3(x - r, y + h, z), Vec3::ZERO);
// 	}
// 	else if(dir == Facing::NORTH) {
// 		level.addParticle(ParticleType::RedDust, Vec3(x + r, y + h, z), Vec3::ZERO);
// 	}
// 	else if(dir == Facing::SOUTH) {
// 		level.addParticle(ParticleType::RedDust, Vec3(x, y + h, z - r), Vec3::ZERO);
// 	}
// 	else if(dir == Facing::WEST) {
// 		level.addParticle(ParticleType::RedDust, Vec3(x, y + h, z + r), Vec3::ZERO);
// 	}
// 	else {
// 		level.addParticle(ParticleType::RedDust, Vec3(x, y, z), Vec3::ZERO);
// 	}
}

int RedstoneTorchBlock::getTickDelay() {
	return 2;
}

bool RedstoneTorchBlock::shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const
{
	return true;
}

ItemInstance RedstoneTorchBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(Block::mLitRedStoneTorch);
}

ItemInstance RedstoneTorchBlock::getSilkTouchItemInstance(DataID data) const {
	return ItemInstance(Block::mLitRedStoneTorch);
}

int RedstoneTorchBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Block::mLitRedStoneTorch->mID;
}
