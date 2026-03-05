/*********************************************************
*   (c) Mojang. All rights reserved                      *
*   (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "CommonTypes.h"

class Direction {
public:
	enum Type : unsigned char {
		SOUTH = 0,
		WEST,
		NORTH,
		EAST,
		UNDEFINED = 0xFF
	};

	// for [direction] it gives [block-face]
	static const FacingID DIRECTION_FACING[4];

	// for a given face gives direction
	static const int FACING_DIRECTION[6];

	// for [direction] it gives [opposite direction]
	static const int DIRECTION_OPPOSITE[4];

	// for [direction][world-facing] it gives [block-facing]
	static const FacingID RELATIVE_DIRECTION_FACING[4][6];

	// for [direction] it gives [90 degrees clockwise direction]
	static const int DIRECTION_CLOCKWISE[4];

	// for [direction] it gives [90 degrees counter-clockwise direction]
	static const int DIRECTION_COUNTER_CLOCKWISE[4];

	static const int STEP_X[4];
	static const int STEP_Z[4];

	static int getDirection(float xd, float zd);
	static int getDirection(int x0, int z0, int x1, int z1);

};
