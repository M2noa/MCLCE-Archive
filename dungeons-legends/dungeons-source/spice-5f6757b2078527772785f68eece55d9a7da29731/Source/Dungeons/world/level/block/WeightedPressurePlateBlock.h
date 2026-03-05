#pragma once

#include "BasePressurePlateBlock.h"

class Material;

class Level;


class WeightedPressurePlateBlock : public BasePressurePlateBlock {

public:
	static const int MAX_WEIGHT_LIGHT;
	static const int MAX_WEIGHT_HEAVY;

public:

    WeightedPressurePlateBlock(const std::string& nameId, int id, const Material &material, const int maxWeight);

    virtual int getTickDelay() const override;

	virtual ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;

private:
	const int maxWeight;

protected:
	virtual int getSignalStrength(BlockSource& region, const BlockPos &pos) const override;
	virtual int getSignalForData(int data) const override;
	virtual int getDataForSignal(int signal) const override;

};
