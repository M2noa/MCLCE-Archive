#include "Dungeons.h"
#include "world/level/pathfinder/Node.h"
#include "util/Math.h"
#include "world/phys/Vec3.h"

Node::Node(const BlockPos& pos)
	: pos(pos){

}

Node::Node(const Node& node) :
	pos(node.pos){

}

float Node::distanceTo(Node* to){
	Vec3 A = pos;
	Vec3 B = to->pos;
	return A.distanceTo(B);
}

float Node::distanceToSqr(Node* to) {
	Vec3 A = pos;
	Vec3 B = to->pos;
	return A.distanceToSqr(B);
}

bool Node::equals(Node* o){
	return pos == o->pos;
}

int Node::hashCode(){
	return pos.hashCode();
}

bool Node::inOpenSet(){
	return heapIdx >= 0;
}
