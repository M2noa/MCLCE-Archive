#pragma once
#include "common_header.h"
#include "util/ArrayWithLength.h"
#include "world/phys/Vec3.h"
#include "CommonTypes.h"

class Entity;

class Path {
	friend class PathFinder;

private:

	std::vector<Node> mNodes;
	int index;

public:

	Path(const NodeArray& nodes);
	~Path();

	void next();
	bool isDone();
	Node* last();
	Node* get(int i);
	int getSize() const;
	void setSize(int length);
	int getIndex() const;
	void setIndex(int index);
	Vec3 getPos(const Entity* e, int index) const;
	Vec3 currentPos(const Entity* e) const;
	Vec3 getEndPos() const;
	BlockPos const& currentPos() const;

	std::vector<Node> const& getNodes() const {
		return mNodes;
	}

	bool sameAs(Path* path) const;
	bool endsInXZ(const Vec3& pos);

};
