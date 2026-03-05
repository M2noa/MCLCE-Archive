/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/RotatedPillarBlock.h"

class QuartzBlockBlock : public RotatedPillarBlock {
public:
	enum class Type : int {
		Default = 0,
		Chiseled = 1,
		Lines = 2
	};

	QuartzBlockBlock(const std::string& nameId, int id);
	~QuartzBlockBlock();

	int getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const override;

	std::string buildDescriptionName(DataID data) const override;

protected:
	DataID getSpawnResourcesAuxValue(DataID data) const override;

};
