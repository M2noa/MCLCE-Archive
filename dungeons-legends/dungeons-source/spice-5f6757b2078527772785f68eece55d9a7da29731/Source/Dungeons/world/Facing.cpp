#include "Dungeons.h"

#include "world/Facing.h"

const FacingID Facing::OPPOSITE_FACING[6] = {
	Facing::UP, Facing::DOWN, Facing::SOUTH, Facing::NORTH, Facing::EAST, Facing::WEST
};

const int Facing::STEP_X[6] = {
	0, 0, 0, 0, -1, 1
};
const int Facing::STEP_Y[6] = {
	-1, 1, 0, 0, 0, 0
};
const int Facing::STEP_Z[6] = {
	0, 0, -1, 1, 0, 0
};

const Facing::Name Facing::DIRECTIONS[] = {
	DOWN,
	UP,
	NORTH,
	SOUTH,
	WEST,
	EAST
};

const Facing::Name Facing::BY2DDATA[] = {
	NORTH,
	SOUTH,
	WEST,
	EAST
};

const BlockPos Facing::DIRECTION[] = {
	{ STEP_X[0], STEP_Y[0], STEP_Z[0] },
	{ STEP_X[1], STEP_Y[1], STEP_Z[1] },
	{ STEP_X[2], STEP_Y[2], STEP_Z[2] },
	{ STEP_X[3], STEP_Y[3], STEP_Z[3] },
	{ STEP_X[4], STEP_Y[4], STEP_Z[4] },
	{ STEP_X[5], STEP_Y[5], STEP_Z[5] }
};

const Vec3 Facing::NORMAL[] = {
	{ (float)STEP_X[0], (float)STEP_Y[0], (float)STEP_Z[0] },
	{ (float)STEP_X[1], (float)STEP_Y[1], (float)STEP_Z[1] },
	{ (float)STEP_X[2], (float)STEP_Y[2], (float)STEP_Z[2] },
	{ (float)STEP_X[3], (float)STEP_Y[3], (float)STEP_Z[3] },
	{ (float)STEP_X[4], (float)STEP_Y[4], (float)STEP_Z[4] },
	{ (float)STEP_X[5], (float)STEP_Y[5], (float)STEP_Z[5] }
};

FacingID Facing::getClockWise(const FacingID face) {
	switch(face) {
	case NORTH:
		return EAST;
	case EAST:
		return SOUTH;
	case SOUTH:
		return WEST;
	case WEST:
		return NORTH;
	default: {
		DEBUG_FAIL("Unable to get Y-rotated facing");
		return NORTH;
	}
	}
}

std::vector<FacingID> Facing::Plane::HORIZONTAL = {
	Facing::NORTH,
	Facing::EAST,
	Facing::SOUTH,
	Facing::WEST
};


const std::initializer_list<FacingID> Facing::ALL_FACES = {
	Facing::UP,
	Facing::DOWN,
	Facing::NORTH,
	Facing::SOUTH,
	Facing::WEST,
	Facing::EAST
};

const std::array<std::initializer_list<FacingID>, 6> Facing::ALL_EXCEPT = { {
	// Down
	{
		Facing::UP,
		Facing::NORTH,
		Facing::SOUTH,
		Facing::WEST,
		Facing::EAST
	},
	// Up
	{
		Facing::DOWN,
		Facing::NORTH,
		Facing::SOUTH,
		Facing::WEST,
		Facing::EAST
	},
	// North
	{
		Facing::UP,
		Facing::DOWN,
		Facing::SOUTH,
		Facing::WEST,
		Facing::EAST
	},
	// South
	{
		Facing::UP,
		Facing::DOWN,
		Facing::NORTH,
		Facing::WEST,
		Facing::EAST
	},
	// West
	{
		Facing::UP,
		Facing::DOWN,
		Facing::NORTH,
		Facing::SOUTH,
		Facing::EAST
	},
	// East
	{
		Facing::UP,
		Facing::DOWN,
		Facing::NORTH,
		Facing::SOUTH,
		Facing::WEST
	}
	} 
};

const std::array<std::initializer_list<FacingID>, 6> Facing::ALL_EXCEPT_AXIS_Y = { {
	// Down
		{

		},
	// Up
		{

		},
	// North
		{
			Facing::SOUTH,
			Facing::WEST,
			Facing::EAST
		},
	// South
		{
			Facing::NORTH,
			Facing::WEST,
			Facing::EAST
		},
	// West
		{
			Facing::NORTH,
			Facing::SOUTH,
			Facing::EAST
		},
	// East
		{
			Facing::NORTH,
			Facing::SOUTH,
			Facing::WEST
		}
	} 
};
