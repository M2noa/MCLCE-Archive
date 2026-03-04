#include "stdafx.h"
#include "net.minecraft.h"
#include "net.minecraft.world.level.h"
#include "BlockGenMethods.h"

void BlockGenMethods::generateBox(Level *level, byteArray blocks, int sx, int sy, int sz, int ex, int ey, int ez, BYTE edge, BYTE filling)
{

	sx = Mth::clamp(sx, 0, 15);
	sy = Mth::clamp(sy, 0, Level::genDepthMinusOne);
	sz = Mth::clamp(sz, 0, 15);
	ex = Mth::clamp(ex, 0, 15);
	ey = Mth::clamp(ey, 0, Level::genDepthMinusOne);
	ez = Mth::clamp(ez, 0, 15);

	for (int x = sx; x <= ex; x++)
	{
		for (int y = sy; y <= ey; y++)
		{
			for (int z = sz; z <= ez; z++)
			{
				int p = (x * 16 + z) * Level::genDepth + y;

				if (x == sx || x == ex || y == sy || y == ey || z == sz || z == ez)
				{
					blocks[p] = edge;
				}
				else
				{
					blocks[p] = filling;
				}
			}
		}
	}
}

void BlockGenMethods::generateFrame(Level *level, byteArray blocks, int sx, int sy, int ex, int ey, int flatZ, int direction, BYTE edge, BYTE filling)
{

	sx = Mth::clamp(sx, 0, 15);
	sy = Mth::clamp(sy, 0, Level::genDepthMinusOne);
	ex = Mth::clamp(ex, 0, 15);
	ey = Mth::clamp(ey, 0, Level::genDepthMinusOne);
	int sz = Mth::clamp(flatZ, 0, 15);
	int ez = sz;

	bool alongX = true;

	switch (direction)
	{
	case Direction::WEST:
		{
			// rotate counter-clockwise
			int temp = sz;
			sz = 15 - ex;
			ez = 15 - sx;
			sx = ex = temp;
			alongX = false;
		}
		break;
	case Direction::EAST:
		{
			// rotate clockwise
			int temp = sz;
			sz = sx;
			ez = ex;
			sx = ex = 15 - temp;
			alongX = false;
		}
		break;
	case Direction::SOUTH:
		{
			// rotate 180
			sz = ez = 15 - sz;
			int temp = sx;
			sx = 15 - ex;
			ex = 15 - temp;
		}
		break;
	}

	for (int x = sx; x <= ex; x++)
	{
		for (int y = sy; y <= ey; y++)
		{
			for (int z = sz; z <= ez; z++)
			{
				int p = (x * 16 + z) * Level::genDepth + y;

				if (y == sy || y == ey || (alongX && (x == sx || x == ex)) || (!alongX && (z == sz || z == ez)))
				{
					blocks[p] = edge;
				}
				else
				{
					blocks[p] = filling;
				}
			}
		}
	}
}

void BlockGenMethods::generateDirectionLine(Level *level, byteArray blocks, int sx, int sy, int sz, int ex, int ey, int ez, int startDirection, int endDirection, BYTE block)
{

	sx = Mth::clamp(sx, 0, 15);
	sy = Mth::clamp(sy, 0, Level::genDepthMinusOne);
	sz = Mth::clamp(sz, 0, 15);
	ex = Mth::clamp(ex, 0, 15);
	ey = Mth::clamp(ey, 0, Level::genDepthMinusOne);
	ez = Mth::clamp(ez, 0, 15);

	switch (startDirection)
	{
	case Direction::WEST:
		{
			// rotate counter-clockwise
			int temp = sz;
			sz = 15 - sx;
			sx = temp;
		}
		break;
	case Direction::EAST:
		{
			// rotate clockwise
			int temp = sz;
			sz = sx;
			sx = 15 - temp;
		}
		break;
	case Direction::SOUTH:
		{
			// rotate 180
			sz = 15 - sz;
			sx = 15 - sx;
		}
		break;
	}

	switch (endDirection)
	{
	case Direction::WEST:
		{
			// rotate counter-clockwise
			int temp = ez;
			ez = 15 - ex;
			ex = temp;
		}
		break;
	case Direction::EAST:
		{
			// rotate clockwise
			int temp = ez;
			ez = ex;
			ex = 15 - temp;
		}
		break;
	case Direction::SOUTH:
		{
			// rotate 180
			ez = 15 - ez;
			ex = 15 - ex;
		}
		break;
	}

	int dx = Mth::abs(ex - sx);
	int dz = Mth::abs(ez - sz);
	int dy = Mth::abs(ey - sy);

	int slopeX = sx < ex ? 1 : -1;
	int slopeZ = sz < ez ? 1 : -1;
	int slopeY = sy < ey ? 1 : -1;

	int err = dx - dz;
	int yOppositeDelta = (dz > dx) ? dz : dx;
	int yErr = dy - yOppositeDelta;
	bool doYMovement = true;

	while (true)
	{
		blocks[(sx * 16 + sz) * Level::genDepth + sy] = block;

		if (sx == ex && sz == ez)
		{
			break;
		}
		if (sy == ey)
		{
			doYMovement = false;
		}
		int e2 = 2 * err;
		if (e2 > -dz)
		{
			err = err - dz;
			sx = sx + slopeX;
		}
		if (e2 < dx)
		{
			err = err + dx;
			sz = sz + slopeZ;
		}

		if (doYMovement)
		{
			e2 = 2 * yErr;
			if (e2 > -yOppositeDelta)
			{
				yErr = yErr - yOppositeDelta;
				sy = sy + slopeY;
			}
			if (e2 < dy)
			{
				yErr = yErr + dy;
				// don't modify sz here, let the plane decide
			}
		}
	}
}

void BlockGenMethods::generateLine(Level *level, byteArray blocks, int sx, int sy, int sz, int ex, int ey, int ez, BYTE block)
{
	generateDirectionLine(level, blocks, sx, sy, sz, ex, ey, ez, 0, 0, block);
}