#include "stdafx.h"
#include "net.minecraft.world.level.tile.h"
#include "AuxDataTileItem.h"

AuxDataTileItem::AuxDataTileItem(int id, Tile *parentTile) : TileItem(id)
{        
	this->parentTile = parentTile;

	setMaxDamage(0);
	setStackedByData(true);
}

Icon *AuxDataTileItem::getIcon(int itemAuxValue)
{
	return parentTile->getTexture(2, itemAuxValue);
}

int AuxDataTileItem::getLevelDataForAuxValue(int auxValue)
{
	return auxValue;
}