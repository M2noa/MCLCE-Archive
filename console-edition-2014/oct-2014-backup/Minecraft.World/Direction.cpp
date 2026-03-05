#include "stdafx.h"
#include "Direction.h"
#include "Facing.h"

const int Direction::STEP_X[] =
{
        0, -1, 0, 1
};

const int Direction::STEP_Z[] =
{
        1, 0, -1, 0
};

// for [direction] it gives [tile-face]
int Direction::DIRECTION_FACING[4] =
{
    Facing::SOUTH, Facing::WEST, Facing::NORTH, Facing::EAST
};

// for [facing] it gives [direction]
int  Direction::FACING_DIRECTION[] =
{
	UNDEFINED, UNDEFINED, NORTH, SOUTH, WEST, EAST
};

int Direction::DIRECTION_OPPOSITE[4] =
{
	Direction::NORTH, Direction::EAST, Direction::SOUTH, Direction::WEST
};

// for [direction] it gives [90 degrees clockwise direction]
int Direction::DIRECTION_CLOCKWISE[] =
{
	Direction::WEST, Direction::NORTH, Direction::EAST, Direction::SOUTH
};

// for [direction] it gives [90 degrees counter clockwise direction]
int Direction::DIRECTION_COUNTER_CLOCKWISE[] =
{
	Direction::EAST, Direction::SOUTH, Direction::WEST, Direction::NORTH
};

int Direction::RELATIVE_DIRECTION_FACING[4][6] =
{
    // south
    {
            Facing::UP, Facing::DOWN, Facing::SOUTH, Facing::NORTH, Facing::EAST, Facing::WEST
    },
    // west
    {
            Facing::UP, Facing::DOWN, Facing::EAST, Facing::WEST, Facing::NORTH, Facing::SOUTH
    },
    // north
    {
            Facing::UP, Facing::DOWN, Facing::NORTH, Facing::SOUTH, Facing::WEST, Facing::EAST
    },
    // east
    {
            Facing::UP, Facing::DOWN, Facing::WEST, Facing::EAST, Facing::SOUTH, Facing::NORTH
    }
};
