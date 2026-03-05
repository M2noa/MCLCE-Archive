/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "VisibilityNode.h"

VisibilityNode::VisibilityNode(bool empty /*= false*/) {
	if (empty) {
		setEmpty();
	}
}

void VisibilityNode::setEmpty() {
	for (auto& s : mVisibility) {
		s.setEmpty();
	}
}

void VisibilityNode::connect(const ByteMask& group) {
	if (group) {
		for (int i = 0; i < 6; ++i) {
			if (group.contains(i)) {
				connect(i, group);
			}
		}
	}
}

void VisibilityNode::connect(FacingID A, const ByteMask& connected) {
	for (FacingID i = 0; i < 6; ++i) {
		if (i != A && connected.contains(i)) {
			connect(A, i);
		}
	}
}

void VisibilityNode::connect(FacingID A, FacingID B) {
	if (A != B) {
		mVisibility[A].add(B);
		mVisibility[B].add(A);
	}
}

const ByteMask& VisibilityNode::from(FacingID facing) const {
	return mVisibility[facing];
}
