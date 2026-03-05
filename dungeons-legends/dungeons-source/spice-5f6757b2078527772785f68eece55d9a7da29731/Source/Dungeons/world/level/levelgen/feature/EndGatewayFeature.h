#pragma once

#include "world/level/levelgen/feature/Feature.h"

class EndGatewayFeature : public Feature {
public:
	virtual bool place(BlockSource& region, const BlockPos& pos, Random& random) const override;
	void testEnd(BlockSource& region, const BlockPos& pos, Random& random) const;
};