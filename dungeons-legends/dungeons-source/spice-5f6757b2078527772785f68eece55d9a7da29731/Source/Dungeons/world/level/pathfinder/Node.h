#pragma once

#include "world/level/BlockPos.h"

class Node {
	// 4J Jev, these classes were accessing protected members.
	friend class BinaryHeap;
	friend class PathFinder;
	friend class EnderDragon;

public:

	const BlockPos pos;

protected:

	int heapIdx = -1;
	float g, h, f;
	Node* cameFrom = nullptr;

public:

	bool closed = false;

	Node() {
	}				// 4J - added default constructor so we can make an empty of array of these as a copy target

	Node(const BlockPos& pos);
	Node(const Node& node);

	float distanceTo(Node* to);
	float distanceToSqr(Node* to);
	bool equals(Node* o);
	int hashCode();
	bool inOpenSet();

};
