/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "CommonTypes.h"

#include "Core/Utility/ByteMask.h"

class VisibilityNode {
public:

	VisibilityNode(bool empty = false);

	void setEmpty();

	void connect(FacingID A, FacingID B);
	void connect(FacingID A, const ByteMask& connected);

	//doubly connects all of the sides in a group
	void connect(const ByteMask& group);

	const ByteMask& from(FacingID facing) const;

protected:

	ByteMask mVisibility[6];
};
