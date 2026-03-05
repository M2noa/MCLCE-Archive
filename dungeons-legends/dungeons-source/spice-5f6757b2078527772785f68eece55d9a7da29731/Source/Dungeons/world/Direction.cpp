#include "Dungeons.h"

#include "world/Direction.h"
#include "world/Facing.h"

const FacingID Direction::DIRECTION_FACING[4] = {
	Facing::SOUTH,
	Facing::WEST,
	Facing::NORTH,
	Facing::EAST
};
const int Direction::FACING_DIRECTION[6] = {
	Direction::UNDEFINED,
	Direction::UNDEFINED,
	Direction::NORTH,
	Direction::SOUTH,
	Direction::WEST,
	Direction::EAST
};
const int Direction::DIRECTION_OPPOSITE[4] = {
	Direction::NORTH,
	Direction::EAST,
	Direction::SOUTH,
	Direction::WEST
};

const int Direction::DIRECTION_CLOCKWISE[4] = {
	Direction::WEST, Direction::NORTH, Direction::EAST, Direction::SOUTH
};

const int Direction::DIRECTION_COUNTER_CLOCKWISE[4] = {
	Direction::EAST, Direction::SOUTH, Direction::WEST, Direction::NORTH
};

const FacingID Direction::RELATIVE_DIRECTION_FACING[4][6] = {
	// south
	{   Facing::DOWN,
		Facing::UP,
		Facing::SOUTH,
		Facing::NORTH,
		Facing::EAST,
		Facing::WEST },
	// west
	{   Facing::DOWN,
		Facing::UP,
		Facing::EAST,
		Facing::WEST,
		Facing::NORTH,
		Facing::SOUTH },
	// north
	{   Facing::DOWN,
		Facing::UP,
		Facing::NORTH,
		Facing::SOUTH,
		Facing::WEST,
		Facing::EAST },
	// east
	{   Facing::DOWN,
		Facing::UP,
		Facing::WEST,
		Facing::EAST,
		Facing::SOUTH,
		Facing::NORTH }
};

const int Direction::STEP_X[4] = {
	0, -1, 0, 1
};

const int Direction::STEP_Z[4] = {
	1, 0, -1, 0
};

int Direction::getDirection(float xd, float zd) {
	if (std::abs(xd) > std::abs(zd)) {
		if (xd > 0) {
			return WEST;
		}
		else {
			return EAST;
		}
	}
	else {
		if (zd > 0) {
			return NORTH;
		}
		else {
			return SOUTH;
		}
	}
}

int Direction::getDirection(int x0, int z0, int x1, int z1) {
	int xd = x0 - x1;
	int zd = z0 - z1;

	return getDirection((float)xd, (float)zd);
}
