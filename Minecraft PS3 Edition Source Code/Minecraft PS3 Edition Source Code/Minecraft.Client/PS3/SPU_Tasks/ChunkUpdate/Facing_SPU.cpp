#include "stdafx.h"
#include "Facing_SPU.h"

const int Facing::OPPOSITE_FACING[6] =
{
	UP, DOWN, SOUTH, NORTH, EAST, WEST
};

const int Facing::STEP_X[6] =
{
	0, 0, 0, 0, -1, 1
};

const int Facing::STEP_Y[6] =
{
	-1, 1, 0, 0, 0, 0
};

const int Facing::STEP_Z[6] =
{
	0, 0, -1, 1, 0, 0
};
