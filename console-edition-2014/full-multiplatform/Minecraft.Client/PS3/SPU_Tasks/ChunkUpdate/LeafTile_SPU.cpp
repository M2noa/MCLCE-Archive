#include "stdafx.h"
#include "LeafTile_SPU.h"
#include "ChunkRebuildData.h"


// const unsigned int LeafTile::LEAF_NAMES[LEAF_NAMES_LENGTH] = {	IDS_TILE_LEAVES_OAK,
// 													IDS_TILE_LEAVES_SPRUCE,
// 													IDS_TILE_LEAVES_BIRCH,
// 												};
// 
// const wstring LeafTile::TEXTURES[2][4] = { {L"leaves", L"leaves_spruce", L"leaves", L"leaves_jungle"}, {L"leaves_opaque", L"leaves_spruce_opaque", L"leaves_opaque", L"leaves_jungle_opaque"},};


// from TransparentTile, since we're no longer inheriting
bool LeafTile_SPU::shouldRenderFace(ChunkRebuildData *level, int x, int y, int z, int face)
{
	int id = level->getTile(x, y, z);
	if (!ms_pTileData->leafTile_allowSame && id == this->id) return false;
	return Tile_SPU::shouldRenderFace(level, x, y, z, face);
}

int LeafTile_SPU::getColor(ChunkRebuildData *level, int x, int y, int z)
{
	return getColor(level, x, y, z, level->getData(x, y, z) );
}

// 4J - changed interface to have data passed in, and put existing interface as wrapper above
int LeafTile_SPU::getColor(ChunkRebuildData *level, int x, int y, int z, int data)
{
    if ((data & LEAF_TYPE_MASK) == EVERGREEN_LEAF)
	{
		return 	ms_pTileData->foliageColor_evergreenColor;	//FoliageColor::getEvergreenColor();
    }
    if ((data & LEAF_TYPE_MASK) == BIRCH_LEAF)
	{
        return ms_pTileData->foliageColor_birchColor;//FoliageColor::getBirchColor();
    }

	//return level->getBiomeSource()->getBiome(x, z)->getFolageColor(level, x, y, z);

	int totalRed = 0;
	int totalGreen = 0;
	int totalBlue = 0;

	for (int oz = -1; oz <= 1; oz++)
	{
		for (int ox = -1; ox <= 1; ox++)
		{
			int foliageColor = level->getFoliageColor(x + ox, z + oz);

			totalRed += (foliageColor & 0xff0000) >> 16;
			totalGreen += (foliageColor & 0xff00) >> 8;
			totalBlue += (foliageColor & 0xff);
		}
	}

	//        return level.getBiomeSource().getBiome(x, z).getGrassColor(level, x, y, z);
	return (((totalRed / 9) & 0xFF) << 16) | (((totalGreen / 9) & 0xFF) << 8) | (((totalBlue / 9) & 0xFF));
}



bool LeafTile_SPU::isSolidRender(bool isServerLevel)
{
	// 4J Stu - The server level shouldn't care how the tile is rendered!
	// Fix for #9407 - Gameplay: Destroying a block of snow on top of trees, removes any adjacent snow.
	if(isServerLevel) return true;
	return !ms_pTileData->leafTile_allowSame;
}

Icon_SPU *LeafTile_SPU::getTexture(int face, int data)
{
    if ((data & LEAF_TYPE_MASK) == EVERGREEN_LEAF)
	{
        return &ms_pTileData->leafTile_icons[ms_pTileData->leafTile_fancyTextureSet][EVERGREEN_LEAF];
    }
    if ((data & LEAF_TYPE_MASK) == JUNGLE_LEAF)
	{
        return &ms_pTileData->leafTile_icons[ms_pTileData->leafTile_fancyTextureSet][JUNGLE_LEAF];
    }
    return &ms_pTileData->leafTile_icons[ms_pTileData->leafTile_fancyTextureSet][0];
}

void LeafTile_SPU::setFancy(bool fancyGraphics)
{
    ms_pTileData->leafTile_allowSame = fancyGraphics;
    ms_pTileData->leafTile_fancyTextureSet = (fancyGraphics ? 0 : 1);
}
