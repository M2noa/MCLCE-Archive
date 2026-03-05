/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/BlockPos.h"
#include "world/level/pathfinder/BinaryHeap.h"
#include "world/level/pathfinder/Node.h"
#include "world/level/material/Material.h"

class BlockPos;
class BlockSource;
class Entity;
class Path;

// Java doesn't have a default hash value for ints, however, the hashmap itself does some "supplemental" hashing, so
// our ints actually get hashed by code as implemented below. std templates *do* have a standard hash for ints, but it
// would appear to be a bit expensive so matching the java one for now anyway. This code implements the supplemental
// hashing that happens in java so we can match what their maps are doing with ints.

typedef struct {
	int operator() (const int& k) const {
		int h = k;
		h += ~(h << 9);
		h ^= (((unsigned int)h) >> 14);
		h += (h << 4);
		h ^= (((unsigned int)h) >> 10);
		return h;
	}

} IntKeyHash;

typedef struct {
	bool operator() (const int& x, const int& y) const {
		return x == y;
	}

} IntKeyEq;

class PathFinder {
private:

	BlockSource* mRegion;

	BinaryHeap mOpenSet;

	std::unordered_map<BlockPos, Node> mNodes;

	Node* mNeighbors[32];

	bool mCanPassDoors;
	bool mCanOpenDoors;
	bool mAvoidWater;
	bool mCanFloat;
	bool mAvoidPortals;

public:

	enum class NodeType : int {
		TRAP = -4,
		FENCE = -3,
		LAVA = -2,
		WATER = -1,
		BLOCKED = 0,
		OPEN = 1,
		WALKABLE = 2,
	};

	PathFinder(BlockSource* level, bool canPassDoors, bool canOpenDoors, bool avoidWater, bool canFloat, bool avoidPortals);
	~PathFinder();

	Unique<Path> findPath(Entity* from, Entity* to, float maxDist);

	Unique<Path> findPath(Entity* from, int x, int y, int z, float maxDist);

	NodeType _isFree(Entity* entity, const BlockPos& fromPos, const BlockPos& testPos, Node* size);

private:

	Unique<Path> findPath(Entity* e, float xt, float yt, float zt, float maxDist);

	// function A*(start,goal)
	Unique<Path> findPath(Entity* e, Node* from, Node* to, Node* size, float maxDist);
	int getNeighbors(Entity* entity, Node* pos, Node* size, Node* target, float maxDist);
	Node* getNode(Entity* entity, BlockPos const& fromPos, int x, int y, int z, Node* size, int jumpSize);

	Node* getNode(int x, int y, int z);
	Node* getWaterNode(Entity* entity, float x, float y, float z);
	bool isFree(Entity* entity, float x, float y, float z);

	bool _findNearestPathableNeighbor(BlockPos& outPos, Entity* entity, const BlockPos& fromPos);
	NodeType _classifyNode(Entity* entity, const BlockPos& fromPos, const BlockPos& testPos);
	NodeType _classifyNode(Entity* entity, const BlockPos& fromPos, BlockID blockId, const BlockPos& testPos);

	Unique<Path> _reconstructPath(Node* from, Node* to);

};
