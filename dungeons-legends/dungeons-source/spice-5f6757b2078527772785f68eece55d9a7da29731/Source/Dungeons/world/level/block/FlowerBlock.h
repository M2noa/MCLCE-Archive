/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/BushBlock.h"

class ItemInstance;

class FlowerBlock : public BushBlock {
public:
	enum class Type {
		Yellow = 0,
		Red = 1
	};

	enum class DANDELIONS {
		DANDELION = 0,
		_count
	};

	enum class ROSES : unsigned char {
		POPPY = 0,
		ORCHID,
		ALLIUM,
		HOUSTONIA,
		TULIP_RED,
		TULIP_ORANGE,
		TULIP_WHITE,
		TULIP_PINK,
		OXEYE,
		_count
	};

	FlowerBlock(const std::string& nameId, int id, Type category);

	virtual Vec3 randomlyModifyPosition(const BlockPos& pos) const override;

	const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const override;

	std::string buildDescriptionName(DataID data) const override;

	bool onFertilized(BlockSource& region, const BlockPos& pos, Entity* entity) const override;

protected:
	DataID getSpawnResourcesAuxValue(DataID data) const override;

private:
	bool _randomWalk(BlockSource& region, int& xx, int& yy, int& zz, int j) const;

	Type mType;
};
