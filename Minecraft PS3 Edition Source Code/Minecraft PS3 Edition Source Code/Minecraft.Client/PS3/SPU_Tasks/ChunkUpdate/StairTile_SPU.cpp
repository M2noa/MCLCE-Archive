#include "stdafx.h"
#include "StairTile_SPU.h"
#include "ChunkRebuildData.h"


void StairTile_SPU::updateShape(ChunkRebuildData *level, int x, int y, int z, int forceData, TileEntity* forceEntity) // 4J added forceData, forceEntity param
{
	setShape(0, 0, 0, 1, 1, 1);
}

bool StairTile_SPU::isSolidRender(bool isServerLevel)
{
	return false;
}

int StairTile_SPU::getRenderShape()
{
	return Tile_SPU::SHAPE_STAIRS;
}

void StairTile_SPU::setBaseShape(ChunkRebuildData *level, int x, int y, int z)
{
	int data = level->getData(x, y, z);

	if ((data & UPSIDEDOWN_BIT) != 0)
	{
		setShape(0, .5f, 0, 1, 1, 1);
	}
	else
	{
		setShape(0, 0, 0, 1, .5f, 1);
	}
}

bool StairTile_SPU::isStairs(int id)
{
	switch(id)
	{
	case Tile_SPU::stairs_wood_Id:
	case Tile_SPU::stairs_stone_Id:
	case Tile_SPU::stairs_bricks_Id:	
	case Tile_SPU::stairs_stoneBrickSmooth_Id:
	case Tile_SPU::stairs_netherBricks_Id:
	case Tile_SPU::stairs_sandstone_Id:
	case Tile_SPU::stairs_sprucewood_Id:
	case Tile_SPU::stairs_birchwood_Id:
		return true;
	default:
		return false;
	}
	return false;
}

bool StairTile_SPU::isLockAttached(ChunkRebuildData *level, int x, int y, int z, int data)
{
	int lockTile = level->getTile(x, y, z);
	if (isStairs(lockTile) && level->getData(x, y, z) == data)
	{
		return true;
	}

	return false;
}

bool StairTile_SPU::setStepShape(ChunkRebuildData *level, int x, int y, int z)
{
	int data = level->getData(x, y, z);
	int dir = data & 0x3;

	float bottom = 0.5f;
	float top = 1.0f;

	if ((data & UPSIDEDOWN_BIT) != 0)
	{
		bottom = 0;
		top = .5f;
	}

	float west = 0;
	float east = 1;
	float north = 0;
	float south = .5f;

	bool checkInnerPiece = true;

	if (dir == DIR_EAST)
	{
		west = .5f;
		south = 1;

		int backTile = level->getTile(x + 1, y, z);
		int backData = level->getData(x + 1, y, z);
		if (isStairs(backTile) && ((data & UPSIDEDOWN_BIT) == (backData & UPSIDEDOWN_BIT)))
		{
			int backDir = backData & 0x3;
			if (backDir == DIR_NORTH && !isLockAttached(level, x, y, z + 1, data))
			{
				south = .5f;
				checkInnerPiece = false;
			}
			else if (backDir == DIR_SOUTH && !isLockAttached(level, x, y, z - 1, data))
			{
				north = .5f;
				checkInnerPiece = false;
			}
		}
	}
	else if (dir == DIR_WEST)
	{
		east = .5f;
		south = 1;

		int backTile = level->getTile(x - 1, y, z);
		int backData = level->getData(x - 1, y, z);
		if (isStairs(backTile) && ((data & UPSIDEDOWN_BIT) == (backData & UPSIDEDOWN_BIT)))
		{
			int backDir = backData & 0x3;
			if (backDir == DIR_NORTH && !isLockAttached(level, x, y, z + 1, data))
			{
				south = .5f;
				checkInnerPiece = false;
			}
			else if (backDir == DIR_SOUTH && !isLockAttached(level, x, y, z - 1, data))
			{
				north = .5f;
				checkInnerPiece = false;
			}
		}
	}
	else if (dir == DIR_SOUTH)
	{
		north = .5f;
		south = 1;

		int backTile = level->getTile(x, y, z + 1);
		int backData = level->getData(x, y, z + 1);
		if (isStairs(backTile) && ((data & UPSIDEDOWN_BIT) == (backData & UPSIDEDOWN_BIT)))
		{
			int backDir = backData & 0x3;
			if (backDir == DIR_WEST && !isLockAttached(level, x + 1, y, z, data))
			{
				east = .5f;
				checkInnerPiece = false;
			}
			else if (backDir == DIR_EAST && !isLockAttached(level, x - 1, y, z, data))
			{
				west = .5f;
				checkInnerPiece = false;
			}
		}
	}
	else if (dir == DIR_NORTH)
	{
		int backTile = level->getTile(x, y, z - 1);
		int backData = level->getData(x, y, z - 1);
		if (isStairs(backTile) && ((data & UPSIDEDOWN_BIT) == (backData & UPSIDEDOWN_BIT)))
		{
			int backDir = backData & 0x3;
			if (backDir == DIR_WEST && !isLockAttached(level, x + 1, y, z, data))
			{
				east = .5f;
				checkInnerPiece = false;
			}
			else if (backDir == DIR_EAST && !isLockAttached(level, x - 1, y, z, data))
			{
				west = .5f;
				checkInnerPiece = false;
			}
		}
	}

	setShape(west, bottom, north, east, top, south);
	return checkInnerPiece;
}

/*
* This method adds an extra 1/8 block if the stairs can attach as an
* "inner corner."
*/
bool StairTile_SPU::setInnerPieceShape(ChunkRebuildData *level, int x, int y, int z)
{
	int data = level->getData(x, y, z);
	int dir = data & 0x3;

	float bottom = 0.5f;
	float top = 1.0f;

	if ((data & UPSIDEDOWN_BIT) != 0)
	{
		bottom = 0;
		top = .5f;
	}

	float west = 0;
	float east = .5f;
	float north = .5f;
	float south = 1.0f;

	bool hasInnerPiece = false;

	if (dir == DIR_EAST)
	{
		int frontTile = level->getTile(x - 1, y, z);
		int frontData = level->getData(x - 1, y, z);
		if (isStairs(frontTile) && ((data & UPSIDEDOWN_BIT) == (frontData & UPSIDEDOWN_BIT)))
		{
			int frontDir = frontData & 0x3;
			if (frontDir == DIR_NORTH && !isLockAttached(level, x, y, z - 1, data))
			{
				north = 0;
				south = .5f;
				hasInnerPiece = true;
			}
			else if (frontDir == DIR_SOUTH && !isLockAttached(level, x, y, z + 1, data))
			{
				north = .5f;
				south = 1;
				hasInnerPiece = true;
			}
		}
	}
	else if (dir == DIR_WEST)
	{
		int frontTile = level->getTile(x + 1, y, z);
		int frontData = level->getData(x + 1, y, z);
		if (isStairs(frontTile) && ((data & UPSIDEDOWN_BIT) == (frontData & UPSIDEDOWN_BIT)))
		{
			west = .5f;
			east = 1.0f;
			int frontDir = frontData & 0x3;
			if (frontDir == DIR_NORTH && !isLockAttached(level, x, y, z - 1, data))
			{
				north = 0;
				south = .5f;
				hasInnerPiece = true;
			}
			else if (frontDir == DIR_SOUTH && !isLockAttached(level, x, y, z + 1, data))
			{
				north = .5f;
				south = 1;
				hasInnerPiece = true;
			}
		}
	}
	else if (dir == DIR_SOUTH)
	{
		int frontTile = level->getTile(x, y, z - 1);
		int frontData = level->getData(x, y, z - 1);
		if (isStairs(frontTile) && ((data & UPSIDEDOWN_BIT) == (frontData & UPSIDEDOWN_BIT)))
		{
			north = 0;
			south = .5f;

			int frontDir = frontData & 0x3;
			if (frontDir == DIR_WEST && !isLockAttached(level, x - 1, y, z, data))
			{
				hasInnerPiece = true;
			}
			else if (frontDir == DIR_EAST && !isLockAttached(level, x + 1, y, z, data))
			{
				west = .5f;
				east = 1.0f;
				hasInnerPiece = true;
			}
		}
	}
	else if (dir == DIR_NORTH)
	{
		int frontTile = level->getTile(x, y, z + 1);
		int frontData = level->getData(x, y, z + 1);
		if (isStairs(frontTile) && ((data & UPSIDEDOWN_BIT) == (frontData & UPSIDEDOWN_BIT)))
		{
			int frontDir = frontData & 0x3;
			if (frontDir == DIR_WEST && !isLockAttached(level, x - 1, y, z, data))
			{
				hasInnerPiece = true;
			}
			else if (frontDir == DIR_EAST && !isLockAttached(level, x + 1, y, z, data))
			{
				west = .5f;
				east = 1.0f;
				hasInnerPiece = true;
			}
		}
	}

	if (hasInnerPiece)
	{
		setShape(west, bottom, north, east, top, south);
	}
	return hasInnerPiece;
}
