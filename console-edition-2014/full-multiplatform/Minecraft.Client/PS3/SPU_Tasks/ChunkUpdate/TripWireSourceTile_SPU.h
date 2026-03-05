#pragma once

#include "Tile_SPU.h"

class TripWireSourceTile_SPU : public Tile_SPU
{
public:
	static const int MASK_DIR = 0x3;
	static const int MASK_ATTACHED = 0x4;
	static const int MASK_POWERED = 0x8;
	static const int WIRE_DIST_MIN = 1;
	static const int WIRE_DIST_MAX = 2 + 40; // 2 hooks + x string

	TripWireSourceTile_SPU(int id) : Tile_SPU(id) {}

	bool blocksLight() { return false; }
 	bool isSolidRender(bool isServerLevel = false) { return false; }
	bool isCubeShaped() { return false; }
	int getRenderShape() { return Tile_SPU::SHAPE_TRIPWIRE_SOURCE; }
};