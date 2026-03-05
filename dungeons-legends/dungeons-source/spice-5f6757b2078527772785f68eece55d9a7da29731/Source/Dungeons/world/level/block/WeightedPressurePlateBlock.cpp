#include "Dungeons.h"

#include "WeightedPressurePlateBlock.h"
#include "world/item/ItemInstance.h"
#include "world/level/Level.h"
#include "world/entity/Entity.h"
#include "world/level/BlockSource.h"
#include "world/redstone/Redstone.h"
#include "SharedConstants.h"

const int WeightedPressurePlateBlock::MAX_WEIGHT_LIGHT = 15;
const int WeightedPressurePlateBlock::MAX_WEIGHT_HEAVY = 15 * 10;

WeightedPressurePlateBlock::WeightedPressurePlateBlock(const std::string& nameId, int id, const Material &material, const int maxWeight) :
	  BasePressurePlateBlock(nameId, id, material)
	, maxWeight(maxWeight) {
    
	//updateVisualShape(0);
}

int WeightedPressurePlateBlock::getSignalStrength(BlockSource& region, const BlockPos &pos) const {

	const EntityList& entityList = region.getEntities(nullptr, getSensitiveAABB(pos));
	int count = 0;
	for (auto& entity : entityList) {
		if (entity->getEntityTypeId() != EntityType::Experience) {
			++count;
		}
	}
	count = std::min(count, maxWeight);

	if (count <= 0) {
		return 0;
	}
	else {
		float pct = std::min(maxWeight, count) / static_cast<float>(maxWeight);
		return (int)ceil(pct * Redstone::SIGNAL_MAX);
	}
}

int WeightedPressurePlateBlock::getSignalForData(int data) const {
	return data;
}

int WeightedPressurePlateBlock::getDataForSignal(int signal) const {
	return signal;
}

int WeightedPressurePlateBlock::getTickDelay() const
{
	return SharedConstants::TicksPerSecond / 2;
}

ItemInstance WeightedPressurePlateBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(maxWeight == MAX_WEIGHT_LIGHT ? Block::mLightWeightedPressurePlate : Block::mHeavyWeightedPressurePlate);
}
