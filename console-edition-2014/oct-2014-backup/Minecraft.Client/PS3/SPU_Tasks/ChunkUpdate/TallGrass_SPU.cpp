#include "stdafx.h"
#include "TallGrass_SPU.h"
#include "ChunkRebuildData.h"

// const unsigned int TallGrass_SPU::TALL_GRASS_TILE_NAMES[TALL_GRASS_TILE_NAMES_LENGTH] = {	IDS_TILE_SHRUB,
// 													IDS_TILE_GRASS,
// 													IDS_TILE_FERN,
// 												};

const int TallGrass_SPU::DEAD_SHRUB = 0;
const int TallGrass_SPU::TALL_GRASS = 1;
const int TallGrass_SPU::FERN = 2;

// const wstring TallGrass::TEXTURE_NAMES[] = {L"deadbush", L"tallgrass", L"fern"};


Icon_SPU *TallGrass_SPU::getTexture(int face, int data)
{
	if (data >= TALL_GRASS_TILE_NAMES_LENGTH) data = 0;
	return &ms_pTileData->tallGrass_Icons[data];
}

int TallGrass_SPU::getColor(ChunkRebuildData *level, int x, int y, int z )
{
	return getColor( level, x, y, z, level->getData(x, y, z) );
}

// 4J - changed interface to have data passed in, and put existing interface as wrapper above
int TallGrass_SPU::getColor(ChunkRebuildData *level, int x, int y, int z, int data)
{
	int d = data;
	if (d == DEAD_SHRUB) return 0xffffff;

	return level->getGrassColor(x, z);
}
