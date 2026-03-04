#include "stdafx.h"
#include "PressurePlateTile_SPU.h"
#include "ChunkRebuildData.h"


bool PressurePlateTile_SPU::isSolidRender(bool isServerLevel)
{
	return false;
}

bool PressurePlateTile_SPU::blocksLight()
{
	return false;
}


void PressurePlateTile_SPU::updateShape(ChunkRebuildData *level, int x, int y, int z, int forceData, TileEntity* forceEntity) // 4J added forceData, forceEntity param
{
    bool pressed = level->getData(x, y, z) == 1;

    float o = 1 / 16.0f;
    if (pressed)
	{
        this->setShape(o, 0, o, 1 - o, 0.5f / 16.0f, 1 - o);
    }
	else
	{
        setShape(o, 0, o, 1 - o, 1 / 16.0f, 1 - o);
    }
}


void PressurePlateTile_SPU::updateDefaultShape()
{
    float x = 8 / 16.0f;
    float y = 2 / 16.0f;
    float z = 8 / 16.0f;
    setShape(0.5f - x, 0.5f - y, 0.5f - z, 0.5f + x, 0.5f + y, 0.5f + z);

}

