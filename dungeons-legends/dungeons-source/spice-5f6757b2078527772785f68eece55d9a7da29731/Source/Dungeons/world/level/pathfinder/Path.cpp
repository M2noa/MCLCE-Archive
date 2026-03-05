#include "Dungeons.h"

#include "world/level/pathfinder/Path.h"

#include "world/level/pathfinder/Node.h"
#include "world/entity/Entity.h"

Path::~Path(){

}

Path::Path(const NodeArray& nodes){
	index = 0;

	// 4J - copying these mNodes over from a NodeArray (which is an array of Node * references) to just a straight array
	// of mNodes,
	// so that this Path is no longer dependent of mNodes allocated elsewhere and can handle its own destruction
	// Note: cameFrom pointer will be useless now but that isn't used once this is just a path

	//do a deep copy
	mNodes.reserve(nodes.size());

	for(auto i : range(nodes.size())) {
		mNodes.emplace_back(*nodes[i]);
	}
}

void Path::next(){
	index++;
}

bool Path::isDone(){
	return index >= (int)mNodes.size();
}

Node* Path::last(){
	if (mNodes.size() > 0) {
		return &mNodes.back();
	}
	return nullptr;
}

Node* Path::get(int i){
	return &mNodes[i];
}

int Path::getSize() const {
	return mNodes.size();
}

void Path::setSize(int length){
	//need to refactor to unsigned...
	if (length < 0) {
		length = 0;
	}

	mNodes.resize(length);
}

int Path::getIndex() const {
	return index;
}

void Path::setIndex(int index){
	this->index = index;
}

Vec3 Path::getPos(const Entity* e, int index) const {
	Vec3 pos = mNodes[index].pos;
	// TODO: rherlitz
	//pos.x += (int) (e->mBBDim.x + 1) * 0.5f;
	//pos.z += (int) (e->mBBDim.x + 1) * 0.5f;
	return pos;
}

Vec3 Path::currentPos(const Entity* e) const {
	return getPos(e, index);
}

BlockPos const& Path::currentPos() const {
	return mNodes[index].pos;
}

Vec3 Path::getEndPos() const {
	return mNodes.back().pos;
}

bool Path::sameAs(Path* path) const {
	if (path == nullptr) {
		return false;
	}
	if (path->mNodes.size() != mNodes.size()) {
		return false;
	}

	for (unsigned int i = 0; i < mNodes.size(); ++i) {
		if (mNodes[i].pos != path->mNodes[i].pos) {
			return false;
		}
	}
	return true;
}

bool Path::endsInXZ(const Vec3& pos){
	Node const* lastNode = (Node const*)last();
	if (lastNode == nullptr) {
		return false;
	}
	BlockPos p(pos);
	return lastNode->pos.x == p.x && lastNode->pos.z == p.z;
}
