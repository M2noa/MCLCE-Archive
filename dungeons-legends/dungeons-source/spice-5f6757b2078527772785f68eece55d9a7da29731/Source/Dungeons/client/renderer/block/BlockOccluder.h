/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "CommonTypes.h"
#include "world/Facing.h"

class BlockPos;
class Block;
class AABB;
class BlockTessellatorCache;

class BlockOccluder {
public:
	BlockOccluder(BlockTessellatorCache& cache, const Block& block, const BlockPos& blockPos, const AABB& shape, const Facing::FacingIDList& checkFace, bool renderingGUI);
	bool occludes(FacingID face) const;
	bool anyVisible() const;
	
private:
	void _setOccluded(FacingID face, bool occluded);
	bool _shouldOcclude(FacingID face, const AABB& shape, const BlockPos& p);
	bool _shouldOccludeSlab(FacingID face, const AABB& shape, const BlockPos& pos);
	void _updateRenderFace(const Block& block, const BlockPos& p, const AABB& shape, FacingID face);
	bool _shouldOccludeLeaves(FacingID face, const AABB& shape, const BlockPos& p);
	bool _shouldRenderFace(const BlockPos& pos, FacingID face, const AABB& shape);

	bool _isOpaque(const Block& pos);
	bool _isHalfCubeOpaque(const Block& block);
	bool _isTransparent(const BlockPos& pos);

	bool isDeepLeafBlock(const BlockPos& pos);
	bool _shouldOccludeGlass(FacingID face, const AABB& shape, const BlockPos& pos);
	bool _shouldOccludeThinFence(FacingID face, const AABB& shape, const BlockPos& pos);
	bool _shouldOccludeFence(FacingID face, const AABB& shape, const BlockPos& pos);
	bool _shouldOccludeLiquid(FacingID face, const AABB& shape, const BlockPos& pos);
	bool _shouldOccludePortal(FacingID face, const AABB& shape, const BlockPos& pos);
	bool _shouldOccludeSlime(FacingID face, const AABB& shape, const BlockPos& pos);

	std::bitset<6> mFacingOccluded;
	BlockTessellatorCache& mBlockCache;
};
