/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/

#pragma once

#include "world/level/block/Block.h"
#include "world/level/block/entity/BlockEntityRendererId.h"
#include "world/level/BlockPos.h"

class BlockEntity;
class BlockSource;
class CompoundTag;
class Level;
// class Packet;

class BlockEntityFactory {

public:

	static Unique<BlockEntity> createBlockEntity(BlockEntityType type, const BlockPos& pos, const BlockID blockID);

};

class BlockEntity {

	using MapIdType = std::map<std::string, BlockEntityType>;
	using MapTypeId = std::map<BlockEntityType, std::string>;

	static int _runningId;

public:

	static void setId(BlockEntityType type, const std::string& id);
	static void initBlockEntities();

	BlockEntity(BlockEntityType type, const BlockPos& pos, const std::string& id);
	virtual ~BlockEntity();

	virtual void load(const CompoundTag& tag);
	virtual bool save(CompoundTag& tag);

	virtual void tick(BlockSource& region);
	virtual bool isFinished();

	static Unique<BlockEntity> loadStatic(const CompoundTag& tag);

	void setChanged();
	virtual void onChanged(BlockSource& region);

	void setMovable(bool canMove);
	virtual bool isMovable();

	float distanceToSqr(const Vec3& to);

// 	virtual Unique<Packet> getUpdatePacket(BlockSource& region);

	virtual void onUpdatePacket(const CompoundTag& data, BlockSource& region);
	virtual void onMove();	//	Handles anything we need to do before we start moving the block entity.
	virtual void onRemoved(BlockSource& region);

	virtual void triggerEvent(int b0, int b1);
	virtual void clearCache();

	bool isType(BlockEntityType type);
	static bool isType(BlockEntity& te, BlockEntityType type);

	bool isInWorld() const;

	virtual void onNeighborChanged(BlockSource& region, const BlockPos& position);

	virtual float getShadowRadius(BlockSource& region) const;

	// Whether this BlockEntity has an alpha-blended layer it needs to render (like beacon beam glow)
	virtual bool hasAlphaLayer() const;

	void stopDestroy();

	virtual BlockEntity& getCrackEntity(BlockSource& region, const BlockPos& pos);
	
	const AABB& getAABB() const;
	void setBB(AABB value);
	const BlockPos& getPosition() const;
	int getData() const;
	void setData(int value);
	void moveTo(const BlockPos& newPos);
	const BlockEntityType& getType() const;
	int getRunningId() const;
	void setRunningId(int value);
	bool isClientSideOnly() const;
	void setClientSideOnly(bool value);
	const BlockEntityRendererId& getRendererId() const;
	void setRendererId(BlockEntityRendererId value);

	virtual void getDebugText(std::vector<std::string>& outputInfo);

	const Block* getBlock();

	int mTickCount = 0;

protected:

	const Block* mBlock;
	float mDestroyTimer;
	Vec3 mDestroyDirection;
	float mDestroyProgress;
	BlockPos mPosition;
	AABB mBB;
	int mData;
	const BlockEntityType mType;
	int mRunningId;
	bool mClientSideOnly;
	bool mIsMovable = true;
	BlockEntityRendererId mRendererId;

	void _resetAABB();
	void _destructionWobble(float& x, float& y, float& z);

private:

	bool mChanged = true;
	static MapIdType idClassMap;
	static MapTypeId classIdMap;
};
