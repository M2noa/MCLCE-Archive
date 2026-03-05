/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/pathfinder/PathFinder.h"
#include "world/level/pathfinder/Path.h"
#include "world/level/block/Block.h"
#include "world/level/block/TopSnowBlock.h"
#include "world/level/block/FenceGateBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/material/Material.h"
#include "world/entity/Entity.h"
//#include "world/entity/ai/village/Village.h"
// #include "client/renderer/debug/DebugRenderer.h"
#include "world/Direction.h"
#include "world/Facing.h"

PathFinder::PathFinder(BlockSource* level, bool canPassDoors, bool canOpenDoors, bool avoidWater, bool canFloat, bool avoidPortals) :
	  mCanPassDoors(canPassDoors)
	, mCanOpenDoors(canOpenDoors)
	, mAvoidWater(avoidWater)
	, mCanFloat(canFloat)
	, mRegion(level)
	, mAvoidPortals(avoidPortals) {
}

PathFinder::~PathFinder() {

}

Unique<Path> PathFinder::findPath(Entity* from, Entity* to, float maxDist) {
	return findPath(from, to->mPos.x, to->mBB.min.y, to->mPos.z, maxDist);
}

Unique<Path> PathFinder::findPath(Entity* from, int x, int y, int z, float maxDist) {
	return findPath(from, x + 0.5f, y + 0.05f, z + 0.5f, maxDist);
}

Unique<Path> PathFinder::findPath(Entity* e, float xt, float yt, float zt, float maxDist) {
	mOpenSet.clear();
	mNodes.clear();

	if (!e->isSwimmer()) {

		bool resetAvoidWater = mAvoidWater;
		int startY = (int)e->mBB.min.y;
		if (mCanFloat && e->isInWater()) {
			startY = (int)(e->mBB.min.y);

			int floor_x = (int)Math::floor(e->mPos.x);
			int floor_z = (int)Math::floor(e->mPos.z);
			const Material* material = &mRegion->getMaterial(floor_x, startY, floor_z);
			while (material->isType(MaterialType::Water)) {
				++startY;
				material = &mRegion->getMaterial(floor_x, startY, floor_z);
			}
			resetAvoidWater = mAvoidWater;
			mAvoidWater = false;
		}
		else {
			startY = (int)e->mBB.min.y;
		}

		BlockPos startPos((int)floor(e->mBB.min.x), startY, (int)floor(e->mBB.min.z));

		// Make sure our starting 'startPos' is a VALID pathable pos, if it's not use
		// the nearest pathable neighbor that this entity can reach.
		if (!_findNearestPathableNeighbor(startPos, e, startPos)) {
			return Unique<Path>();	// can't path from this start pos
		}

		Node* from = getNode(startPos.x, startPos.y, startPos.z);

		// Try finding a To node that doesn't have air below
		const int xx0 = Math::floor(xt - e->mBBDim.x / 2);
		const int yy0 = Math::floor(yt);
		const int zz0 = Math::floor(zt - e->mBBDim.x / 2);

		Node* to = nullptr;
		if (mRegion->getBlockID(xx0, yy0 - 1, zz0) || e->canFly()) {
			to = getNode(xx0, yy0, zz0);
		}
		else 
		{
			const int xx1 = Math::floor(xt + e->mBBDim.x / 2);
			const int zz1 = Math::floor(zt + e->mBBDim.x / 2);

			for (int xx = xx0; xx <= xx1; ++xx) {
				for (int zz = zz0; zz <= zz1; ++zz) {
					if (mRegion->getBlockID(xx, yy0 - 1, zz) != 0) {
						to = getNode(xx, yy0, zz);
						break;
					}
				}
			}
			if (!to) {	// Find the first non-air block below
				int yy = yy0;

				while (!mRegion->getBlockID(xx0, yy - 1, zz0) && yy > 0) {
					--yy;
				}
				to = getNode(xx0, yy, zz0);
			}
		}

		Node size(Vec3(e->mBBDim.x + 1, e->mBBDim.y + 1, e->mBBDim.x + 1));
		Unique<Path> path = findPath(e, from, to, &size, maxDist);

		mAvoidWater = resetAvoidWater;
		return path;
	} 
	else {
		Node* from = getNode(Math::floor(e->mBB.min.x), Math::floor(e->mBB.min.y + 0.5f), Math::floor(e->mBB.min.z));
		Node* to = getNode(Math::floor(xt - (*e).mBBDim.x / 2), Math::floor(yt + 0.5f), Math::floor(zt - (*e).mBBDim.x / 2));
		Node size(Vec3(e->mBBDim.x + 1, e->mBBDim.y + 1, e->mBBDim.x + 1));
		Unique<Path> path = findPath(e, from, to, &size, maxDist);
		return path;
	}
}

// function A*(start,goal)
Unique<Path> PathFinder::findPath(Entity* e, Node* from, Node* to, Node* size, float maxDist) {
	int numIterations = 0;

	from->g = 0;
	from->h = from->distanceTo(to);
	from->f = from->h;

	mOpenSet.clear();
	mOpenSet.insert(from);

	Node* closest = from;

	while (!mOpenSet.isEmpty() && numIterations < 200) {
		Node* x = mOpenSet.pop();

		if (x->equals(to)) {
			return _reconstructPath(from, to);
		}

		if (x->distanceTo(to) < closest->distanceTo(to)) {
			closest = x;
		}
		x->closed = true;

		int neighborCount = getNeighbors(e, x, size, to, maxDist);

		for (int i = 0; i < neighborCount; i++) {
			Node* y = mNeighbors[i];

			float tentative_g_score = x->g + x->distanceTo(y);
			if (!y->inOpenSet() || tentative_g_score < y->g) {
				y->cameFrom = x;
				y->g = tentative_g_score;
				y->h = y->distanceTo(to);
				if (y->inOpenSet()) {
					mOpenSet.changeCost(y, y->g + y->h);
				}
				else{
					y->f = y->g + y->h;
					mOpenSet.insert(y);
				}
			}
		}
		++numIterations;
	}

	if (closest == from) {
		return nullptr;
	}

	return _reconstructPath(from, closest);
}

int PathFinder::getNeighbors(Entity* entity, Node* pos, Node* size, Node* target, float maxDist) {

	if (!entity->isSwimmer()) {

		int p = 0;

		int jumpSize = 0;
		const BlockPos & blockPos = pos->pos;
		if (_isFree(entity, blockPos, blockPos.above(), size) == NodeType::OPEN) {
			jumpSize = 1;
		}

		Node* n = getNode(entity, blockPos, blockPos.x, blockPos.y, blockPos.z + 1, size, jumpSize);
		Node* w = getNode(entity, blockPos, blockPos.x - 1, blockPos.y, blockPos.z, size, jumpSize);
		Node* e = getNode(entity, blockPos, blockPos.x + 1, blockPos.y, blockPos.z, size, jumpSize);
		Node* s = getNode(entity, blockPos, blockPos.x, blockPos.y, blockPos.z - 1, size, jumpSize);

		if (n != nullptr && !n->closed && n->distanceTo(target) < maxDist) {
			mNeighbors[p++] = n;
		}
		if (w != nullptr && !w->closed && w->distanceTo(target) < maxDist) {
			mNeighbors[p++] = w;
		}
		if (e != nullptr && !e->closed && e->distanceTo(target) < maxDist) {
			mNeighbors[p++] = e;
		}
		if (s != nullptr && !s->closed && s->distanceTo(target) < maxDist) {
			mNeighbors[p++] = s;
		}
		return p;
	}
	else {
		int count = 0;

		for (const FacingID& direction : Facing::ALL_FACES) {
			Node* node = getWaterNode(entity, pos->pos.x + (float)Facing::getStepX(direction), pos->pos.y + (float)Facing::getStepY(direction), pos->pos.z + (float)Facing::getStepZ(direction));
			if (node != nullptr && !node->closed && node->distanceTo(target) < maxDist) {
				mNeighbors[count++] = node;
			}
		}

		return count;
	}
}

Node* PathFinder::getWaterNode(Entity* entity, float x, float y, float z) {
	if (isFree(entity, x, y, z)) {
		return getNode((int)x, (int)y, (int)z);
	}
	return nullptr;
}

bool PathFinder::isFree(Entity* entity, float x, float y, float z) {
	for (int xx = (int)x; xx < (int)x + Math::floor(entity->mBBDim.x + 1.f); xx++) {
		for (int yy = (int)y; yy < (int)y + Math::floor(entity->mBBDim.y + 1.f); yy++) {
			for (int zz = (int)z; zz < (int)z + Math::floor(entity->mBBDim.x + 1.f); zz++) {
				if (!entity->getRegion().getMaterial(Vec3((float)xx, (float)yy, (float)zz)).isType(MaterialType::Water)) {
					return false;
				}
			}
		}
	}
	return true;
}

Node* PathFinder::getNode(Entity* entity, BlockPos const& fromPos, int x, int y, int z, Node* size, int jumpSize) {
	Node* best = nullptr;
	NodeType pathType = _isFree(entity, fromPos, BlockPos(x, y, z), size);
	if (pathType == NodeType::WALKABLE) {
		return getNode(x, y, z);
	}

	if (pathType == NodeType::OPEN) {
		best = getNode(x, y, z);
	}

	if (best == nullptr && jumpSize > 0 && pathType != NodeType::FENCE && pathType != NodeType::TRAP && _isFree(entity, fromPos, BlockPos(x, y + jumpSize, z), size) == NodeType::OPEN) {
		best = getNode(x, y + jumpSize, z);
		y += jumpSize;
	}

	if (best != nullptr) {
		int drop = 0;
		NodeType cost = NodeType::BLOCKED;

		while (y > 0) {
			cost = _isFree(entity, fromPos, BlockPos(x, y - 1, z), size);
			if (mAvoidWater && cost == NodeType::WATER) {
				return nullptr;
			}

			if (cost != NodeType::OPEN) {
				break;
			}

// 			if (entity->canFly()) {
// 				break;
// 			}

			// fell too far?
			if (++drop >= 4) {
				return nullptr;
			}
			y--;

			if (y > 0) {
				best = getNode(x, y, z);
			}
		}
		// fell into lava?
		if (cost == NodeType::LAVA) {
			return nullptr;
		}
	}

	return best;
}

Node* PathFinder::getNode(int x, int y, int z) {
	BlockPos pos(x, y, z);
	auto it = mNodes.find(pos);
	if ( it == mNodes.end()) {
		mNodes.emplace(pos, Node(pos));
		return &mNodes[pos];
	}
	else{
		return &(*it).second;
	}
}

bool PathFinder::_findNearestPathableNeighbor(BlockPos& outPos, Entity* entity, const BlockPos& testPos) {
	outPos = testPos;

	// Is the fromPos already open?
	if (_classifyNode(entity, testPos, testPos) == NodeType::OPEN) {
		return true;
	}

	// Move start pos to the closest non-fence neighbor.
	bool validPos = false;
	float distSq = FLT_MAX;
	const BlockPos fromPos = testPos;
	Vec3 entityPos(entity->mPos.x, entity->mPos.y, entity->mPos.z);

	// North
	if (_classifyNode(entity, fromPos, fromPos.north()) == NodeType::OPEN) {
		float testDistSq = entityPos.distanceToSqr(Vec3((float)fromPos.x + 0.5f, (float)fromPos.y, (float)fromPos.z - 0.5f));
		if (testDistSq < distSq) {
			outPos.x = fromPos.x;
			outPos.z = fromPos.z - 1;
			distSq = testDistSq;
			validPos = true;
		}
	}

	// South
	if (_classifyNode(entity, fromPos, fromPos.south()) == NodeType::OPEN) {
		float testDistSq = entityPos.distanceToSqr(Vec3((float)fromPos.x + 0.5f, (float)fromPos.y, (float)fromPos.z + 1.5f));
		if (testDistSq < distSq) {
			outPos.x = fromPos.x;
			outPos.z = fromPos.z + 1;
			distSq = testDistSq;
			validPos = true;
		}
	}
	
	// East
	if (_classifyNode(entity, fromPos, fromPos.east()) == NodeType::OPEN) {
		float testDistSq = entityPos.distanceToSqr(Vec3((float)fromPos.x + 1.5f, (float)fromPos.y, (float)fromPos.z + 0.5f));
		if (testDistSq < distSq) {
			outPos.x = fromPos.x + 1;
			outPos.z = fromPos.z;
			distSq = testDistSq;
			validPos = true;
		}
	}

	// West
	if (_classifyNode(entity, fromPos, fromPos.west()) == NodeType::OPEN) {
		float testDistSq = entityPos.distanceToSqr(Vec3((float)fromPos.x - 0.5f, (float)fromPos.y, (float)fromPos.z + 0.5f));
		if (testDistSq < distSq) {
			outPos.x = fromPos.x - 1;
			outPos.z = fromPos.z;
			distSq = testDistSq;
			validPos = true;
		}
	}

	return validPos;
}

PathFinder::NodeType PathFinder::_classifyNode(Entity* entity, const BlockPos& fromPos, const BlockPos& testPos) {
	return _classifyNode(entity, fromPos, mRegion->getBlockID(testPos), testPos);
}

PathFinder::NodeType PathFinder::_classifyNode(Entity* entity, const BlockPos& lastPathPos, BlockID blockId, const BlockPos& testPos) {
	if (blockId <= 0) {
		return NodeType::OPEN;
	}

	const Block* block = Block::mBlocks[blockId];
	if (block->getMaterial().isType(MaterialType::Water)) {
		return NodeType::WATER;
	}

	// Can you pathfind through portals?
	if (mAvoidPortals && block->getMaterial().isType(MaterialType::Portal)) {
		return NodeType::BLOCKED;
	}

	// Can you pass through doors?
	if (!mCanPassDoors && block->hasProperty(BlockProperty::Door) ) {
		return NodeType::BLOCKED;
	}

	if (block->checkIsPathable(*entity, lastPathPos, testPos)) {
		return NodeType::OPEN;
	}

// 	if (mCanOpenDoors && Village::isVillageDoor(*block)) {
// 		return NodeType::OPEN;
// 	}

	if (block->hasProperty(BlockProperty::Connects2D)) {
		return NodeType::FENCE;
	}

	if (block->hasProperty(BlockProperty::FenceGate)) {
		auto &region = entity->getRegion();
		if (Block::mFenceGateOak->getBlockState(BlockState::OpenBit).getBool(region.getData(testPos))) {
			return NodeType::OPEN;	// open gate
		}
		else{
			return NodeType::FENCE;	// closed gate
		}
	}

	if (block->hasProperty(BlockProperty::Trap)) {
		return NodeType::TRAP;
	}

	if (block->getMaterial().isType(MaterialType::Lava)) {
		return NodeType::LAVA;
	}

	return NodeType::BLOCKED;
}

PathFinder::NodeType PathFinder::_isFree(Entity* entity, const BlockPos& lastPathPos, const BlockPos& testPos, Node* size) {
	bool walkable = false;

	for (int xx = testPos.x; xx < testPos.x + size->pos.x; xx++) {
		for (int yy = testPos.y; yy < testPos.y + size->pos.y; yy++) {
			for (int zz = testPos.z; zz < testPos.z + size->pos.z; zz++) {

				BlockID blockId = mRegion->getBlockID(xx, yy, zz);
				const Block* block = Block::mBlocks[blockId];

				if (block->hasProperty(BlockProperty::Walkable)) {
					walkable = true;
				}
				else if (!mAvoidWater && block->getMaterial().isType(MaterialType::Water)) {
					walkable = true;
				}

				if (!mAvoidPortals && block->getMaterial().isType(MaterialType::Portal)) {
					walkable = true;
				}

				NodeType ntype = _classifyNode(entity, lastPathPos, blockId, BlockPos(xx, yy, zz));
				if (ntype == NodeType::OPEN) {
					continue;
				}

				return ntype;
			}
		}
	}

	if (walkable) {
		return NodeType::WALKABLE;
	}

	return NodeType::OPEN;
}

// function reconstruct_path(came_from,current_node)
Unique<Path> PathFinder::_reconstructPath(Node* from, Node* to) {
	int count = 1;
	Node* n = to;

	while (n->cameFrom != nullptr) {
		count++;
		n = n->cameFrom;
	}

	NodeArray nodes = NodeArray(count);
	n = to;
	nodes[--count] = n;

	while (n->cameFrom != nullptr) {
		n = n->cameFrom;
		nodes[--count] = n;
	}

	auto ret = make_unique<Path>(nodes);

	return ret;
}
