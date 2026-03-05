#pragma once
#include "DiggerItem.h"

#define SHOVEL_DIGGABLES 10
class ShovelItem : public DiggerItem
{
private:
	static TileArray *diggables;

public:
	static void staticCtor();
	ShovelItem(int id, const Tier *tier);

	bool canDestroySpecial(Tile *tile);
};