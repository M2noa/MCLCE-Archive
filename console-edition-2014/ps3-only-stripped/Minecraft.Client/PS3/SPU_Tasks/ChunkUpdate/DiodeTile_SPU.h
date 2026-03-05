#pragma once
#include "Tile_SPU.h"
#include "Facing_SPU.h"

class DiodeTile_SPU : public Tile_SPU
{
public:
    static const int DIRECTION_MASK = 0x3;
    static const int DELAY_MASK = 0xC;
    static const int DELAY_SHIFT = 2;

    static const double DELAY_RENDER_OFFSETS[4];
    static const int DELAYS[4];

public:
	DiodeTile_SPU(int id) : Tile_SPU(id) {}
    virtual Icon_SPU *getTexture(int face, int data)
	{
		// down is used by the torch tesselator
		if (face == Facing::DOWN)
		{
			if (id==diode_on_Id)
			{
				return TileRef_SPU(notGate_on_Id)->getTexture(face);
			}
			return TileRef_SPU(notGate_off_Id)->getTexture(face);
		}
		if (face == Facing::UP)
		{
			return icon();
		}
		// edge of stone half-step
		return TileRef_SPU(stoneSlab_Id)->getTexture(Facing::UP);
	}
    virtual bool shouldRenderFace(LevelSource *level, int x, int y, int z, int face)
	{
		if (face == Facing::DOWN || face == Facing::UP)
			return false;
		return true;
	}

    virtual int getRenderShape() { return SHAPE_DIODE; }
    virtual bool isSolidRender(bool isServerLevel = false) { return false; }
};
