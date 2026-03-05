#include "stdafx.h"
#include "SmoothStoneBrickTile.h"
#include "net.minecraft.world.h"

const wstring SmoothStoneBrickTile::TEXTURE_NAMES[] = {L"stonebricksmooth", L"stonebricksmooth_mossy", L"stonebricksmooth_cracked", L"stonebricksmooth_carved"};

const unsigned int SmoothStoneBrickTile::SMOOTH_STONE_BRICK_NAMES[SMOOTH_STONE_BRICK_NAMES_LENGTH] = {	IDS_TILE_STONE_BRICK_SMOOTH,
													IDS_TILE_STONE_BRICK_SMOOTH_MOSSY,
													IDS_TILE_STONE_BRICK_SMOOTH_CRACKED,
													IDS_TILE_STONE_BRICK_SMOOTH_CHISELED
												};

SmoothStoneBrickTile::SmoothStoneBrickTile(int id) : Tile(id, Material::stone)
{
	icons = NULL;
}

Icon *SmoothStoneBrickTile::getTexture(int face, int data)
{
	if (data < 0 || data >= SMOOTH_STONE_BRICK_NAMES_LENGTH) data = 0;
	return icons[data];
}

int SmoothStoneBrickTile::getSpawnResourcesAuxValue(int data)
{
	return data;
}

unsigned int SmoothStoneBrickTile::getDescriptionId(int iData /*= -1*/)
{
	if(iData < 0 ) iData = 0;
	return SmoothStoneBrickTile::SMOOTH_STONE_BRICK_NAMES[iData];
}

void SmoothStoneBrickTile::registerIcons(IconRegister *iconRegister)
{
	icons = new Icon*[SMOOTH_STONE_BRICK_NAMES_LENGTH];

	for (int i = 0; i < SMOOTH_STONE_BRICK_NAMES_LENGTH; i++)
	{
		icons[i] = iconRegister->registerIcon(TEXTURE_NAMES[i]);
	}
}