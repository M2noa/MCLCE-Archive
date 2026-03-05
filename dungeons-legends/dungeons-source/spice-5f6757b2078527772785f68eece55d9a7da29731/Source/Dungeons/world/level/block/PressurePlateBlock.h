/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "BasePressurePlateBlock.h"

class Material;
class Level;

class PressurePlateBlock : public BasePressurePlateBlock {
public:
	enum class Sensitivity {
		EVERYTHING,
		MOBS,
		PLAYERS
	};
    PressurePlateBlock(const std::string& nameId, int id, const Material &material, Sensitivity sensitivity);

	virtual ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const override;

private:
	Sensitivity sensitivity = static_cast<Sensitivity>(0);

protected:
	virtual int getSignalStrength(BlockSource& region, const BlockPos &pos) const override;
	virtual int getSignalForData(int data) const override;
	virtual int getDataForSignal(int signal) const override;

};
