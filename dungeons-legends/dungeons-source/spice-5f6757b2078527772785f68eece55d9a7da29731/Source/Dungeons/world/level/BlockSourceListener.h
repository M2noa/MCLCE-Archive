/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "CommonTypes.h"

class BlockSource;
class LightLayer;
class BlockEntity;
class BlockPos;

class BlockSourceListener {
public:

	virtual ~BlockSourceListener() {

	}

	virtual void onSourceCreated(BlockSource& source) {
		UNUSED_PARAMETER(source);
	}

	virtual void onSourceDestroyed(BlockSource& source) {
		UNUSED_PARAMETER(source);
	}

	virtual void onBlocksDirty(BlockSource& source, const BlockPos& pos0, const BlockPos& pos1) {
		UNUSED_PARAMETER(pos1, pos0, source);
	}

	virtual void onAreaChanged(BlockSource& source, const BlockPos& min, const BlockPos& max) {
		UNUSED_PARAMETER(max, min, source);
	}

	virtual void onBlockChanged(BlockSource& source, const BlockPos& pos, FullBlock block, FullBlock oldBlock, int updateFlags, Entity* changer) {
		UNUSED_PARAMETER(block, updateFlags, oldBlock, pos, source, changer);
	}

	virtual void onBrightnessChanged(BlockSource& source, const BlockPos& pos) {
		onAreaChanged(source, pos, pos);
	}

	//virtual void onBlockEntityChanged(BlockSource& source, BlockEntity& te) {
	//	UNUSED_PARAMETER(te, source);
	//}

	//virtual void onEntityChanged(BlockSource& source, Entity& entity) {
	//	UNUSED_PARAMETER(entity, source);
	//}

	//virtual Unique<BlockEntity> onBlockEntityRemoved(BlockSource& source, Unique<BlockEntity> te);

	//virtual void onBlockEvent(BlockSource& source, int x, int y, int z, int b0, int b1 ) {
	//	UNUSED_PARAMETER(x, b1, b0, x, y, z, source);
	//}

};
