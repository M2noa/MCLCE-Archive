/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/block/Block.h"

class Level;

class LiquidBlock : public Block {
public:
	static float getHeightFromData(int d);
	static float getSlopeAngle(BlockSource& region, const BlockPos& pos, const Material& m);

	LiquidBlock(const std::string& nameId, int id, const Material& material);

	bool mayPick(BlockSource& region, int data, bool liquid) const override;

	bool shouldRenderFace(BlockSource& region, const BlockPos& pos, FacingID face, const AABB& shape) const override;

	const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const override;

	int getResource(Random& random, int data, int bonusLootLevel = 0) const override;
	int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const override;

	int getColor(BlockSource& region, const BlockPos& pos) const override;
	virtual Color getMapColor(BlockSource& region, const BlockPos& pos) const override;

	void handleEntityInside(BlockSource& region, const BlockPos& pos, Entity* entity, Vec3& current) const override;

	int getTickDelay(BlockSource& region) const;

	float getBrightness(BlockSource& region, const BlockPos& pos);

	void animateTick(BlockSource& region, const BlockPos& pos, Random& random) const override;

	void onPlace(BlockSource& region, const BlockPos& pos) const override;

	void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const override;

	const MobSpawnerData* getMobToSpawn(BlockSource& region, const BlockPos& pos) const override;

	bool checkIsPathable(Entity& entity, const BlockPos& lastPathPos, const BlockPos& pathPos) const override;

protected:
	void emitFizzParticle(BlockSource& region, const BlockPos& p) const;
	int getDepth(BlockSource& region, const BlockPos& pos) const;

	int getRenderedDepth(BlockSource& region, const BlockPos& pos) const;

	void trySpreadFire(BlockSource& region, const BlockPos& pos, Random& random) const;
	bool solidify(BlockSource& region, const BlockPos& pos, const BlockPos& neighbor) const;

private:
	Vec3 _getFlow(BlockSource& region, const BlockPos& pos) const;
};
