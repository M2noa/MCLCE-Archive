#pragma once

#include "MultiTextureTileItem.h"

class AnvilTileItem : public MultiTextureTileItem
{
public:
	AnvilTileItem(Tile *tile);

	int getLevelDataForAuxValue(int auxValue);
	virtual unsigned int getDescriptionId(int iData);
private:
};
