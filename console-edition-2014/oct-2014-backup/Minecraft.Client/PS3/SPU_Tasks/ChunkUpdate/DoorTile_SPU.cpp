#include "stdafx.h"
#include "DoorTile_SPU.h"
#include "Facing_SPU.h"
#include "ChunkRebuildData.h"



Icon_SPU *DoorTile_SPU::getTexture(int face, int data)
{
	int texBase;
	if (getMaterial()->getID() == Material_SPU::metal_Id)
		texBase = 2;
	else
		texBase = 0;

	return &ms_pTileData->doorTile_Icons[texBase];
}

Icon_SPU *DoorTile_SPU::getTexture(ChunkRebuildData *level, int x, int y, int z, int face)
{
	int texBase;
	if (getMaterial()->getID() == Material_SPU::metal_Id)
		texBase = 2;
	else
		texBase = 0;

	if (face == Facing::UP || face == Facing::DOWN) return &ms_pTileData->doorTile_Icons[texBase];

	int compositeData = getCompositeData(level, x, y, z);
	int dir = compositeData & C_DIR_MASK;
	bool isOpen = (compositeData & C_OPEN_MASK) != 0;
	bool flip = false;
	bool upper = (compositeData & C_IS_UPPER_MASK) != 0;

	if (isOpen)
	{
		if (dir == 0 && face == 2) flip = !flip;
		else if (dir == 1 && face == 5) flip = !flip;
		else if (dir == 2 && face == 3) flip = !flip;
		else if (dir == 3 && face == 4) flip = !flip;
	}
	else
	{
		if (dir == 0 && face == 5) flip = !flip;
		else if (dir == 1 && face == 3) flip = !flip;
		else if (dir == 2 && face == 4) flip = !flip;
		else if (dir == 3 && face == 2) flip = !flip;
		if ((compositeData & C_RIGHT_HINGE_MASK) != 0) flip = !flip;
	}

	return &ms_pTileData->doorTile_Icons[texBase + (flip ? DOOR_TILE_TEXTURE_COUNT : 0) + (upper ? 1 : 0)];
}

void DoorTile_SPU::updateShape(ChunkRebuildData *level, int x, int y, int z, int forceData, TileEntity* forceEntity) // 4J added forceData, forceEntity param
{
	setShape(getCompositeData(level,x, y, z));
}

bool DoorTile_SPU::isOpen(ChunkRebuildData *level, int x, int y, int z)
{
	return (getCompositeData(level, x, y, z) & C_OPEN_MASK) != 0;
}

void DoorTile_SPU::setShape(int compositeData)
{
	float r = 3 / 16.0f;
	Tile_SPU::setShape(0, 0, 0, 1, 2, 1);
	int dir = compositeData & C_DIR_MASK;
	bool open = (compositeData & C_OPEN_MASK) != 0;
	bool hasRightHinge = (compositeData & C_RIGHT_HINGE_MASK) != 0;
	if (dir == 0)
	{
		if (open)
		{
			if (!hasRightHinge) setShape(0, 0, 0, 1, 1, r);
			else setShape(0, 0, 1 - r, 1, 1, 1);
		}
		else setShape(0, 0, 0, r, 1, 1);
	}
	else if (dir == 1)
	{
		if (open)
		{
			if (!hasRightHinge) setShape(1 - r, 0, 0, 1, 1, 1);
			else setShape(0, 0, 0, r, 1, 1);
		}
		else setShape(0, 0, 0, 1, 1, r);
	}
	else if (dir == 2)
	{
		if (open)
		{
			if (!hasRightHinge) setShape(0, 0, 1 - r, 1, 1, 1);
			else setShape(0, 0, 0, 1, 1, r);
		}
		else setShape(1 - r, 0, 0, 1, 1, 1);
	}
	else if (dir == 3)
	{
		if (open)
		{
			if (!hasRightHinge) setShape(0, 0, 0, r, 1, 1);
			else setShape(1 - r, 0, 0, 1, 1, 1);
		}
		else setShape(0, 0, 1 - r, 1, 1, 1);
	}
}


bool DoorTile_SPU::isOpen(int data)
{
	return (data & 4) != 0;
}


int DoorTile_SPU::getCompositeData(ChunkRebuildData *level, int x, int y, int z)
{
	int data = level->getData(x, y, z);
	bool isUpper = (data & UPPER_BIT) != 0;
	int lowerData;
	int upperData;
	if (isUpper)
	{
		lowerData = level->getData(x, y - 1, z);
		upperData = data;
	}
	else
	{
		lowerData = data;
		upperData = level->getData(x, y + 1, z);
	}

	// bits: dir, dir, open/closed, isUpper, isRightHinge
	bool isRightHinge = (upperData & 1) != 0;
	return lowerData & C_LOWER_DATA_MASK | (isUpper ? 8 : 0) | (isRightHinge ? 16 : 0);
}