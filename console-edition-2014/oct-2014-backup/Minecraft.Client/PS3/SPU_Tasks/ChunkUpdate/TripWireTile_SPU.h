#pragma once

#include "Tile_SPU.h"
#include "TripWireSourceTile_SPU.h"
#include "Direction_SPU.h"

class TripWireTile_SPU : public Tile_SPU
{
public:
	static const int MASK_POWERED = 0x1;
	static const int MASK_SUSPENDED = 0x2;
	static const int MASK_ATTACHED = 0x4;
	static const int MASK_DISARMED = 0x8;

	TripWireTile_SPU(int id) : Tile_SPU(id) {}

	bool blocksLight() { return false; }
 	bool isSolidRender(bool isServerLevel = false) { return false; }
	bool isCubeShaped() { return false;}
	int getRenderLayer() { return 1; }
	int getRenderShape() { return Tile_SPU::SHAPE_TRIPWIRE; }

	void updateShape(ChunkRebuildData *level, int x, int y, int z)
	{
		int data = level->getData(x, y, z);
		bool attached = (data & MASK_ATTACHED) == MASK_ATTACHED;
		bool suspended = (data & MASK_SUSPENDED) == MASK_SUSPENDED;

		if (!suspended)
		{
			setShape(0, 0, 0, 1, 1.5f / 16.0f, 1);
		}
		else if (!attached)
		{
			setShape(0, 0, 0, 1, 8.0f / 16.0f, 1);
		}
		else
		{
			setShape(0, 1.0f / 16.0f, 0, 1, 2.5f / 16.0f, 1);
		}

	}
	static bool shouldConnectTo(ChunkRebuildData *level, int x, int y, int z, int data, int dir)
	{
		{
			int tx = x + Direction::STEP_X[dir];
			int ty = y;
			int tz = z + Direction::STEP_Z[dir];
			int t = level->getTile(tx, ty, tz);
			bool suspended = (data & MASK_SUSPENDED) == MASK_SUSPENDED;

			if (t == Tile_SPU::tripWireSource_Id)
			{
				int otherData = level->getData(tx, ty, tz);
				int facing = otherData & TripWireSourceTile_SPU::MASK_DIR;

				return facing == Direction::DIRECTION_OPPOSITE[dir];
			}

			if (t == Tile_SPU::tripWire_Id)
			{
				int otherData = level->getData(tx, ty, tz);
				bool otherSuspended = (otherData & MASK_SUSPENDED) == MASK_SUSPENDED;
				return suspended == otherSuspended;
			}

			return false;
		}
	}
};