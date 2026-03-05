// package net.minecraft.world.item.crafting;
// 
// import net.minecraft.world.item.DyePowderItem;
// import net.minecraft.world.item.Item;
// import net.minecraft.world.item.ItemInstance;
// import net.minecraft.world.level.tile.Tile;

#include "stdafx.h"
#include "net.minecraft.world.item.h"
#include "DyePowderItem.h"
#include "Tile.h"
#include "Recipy.h"
#include "Recipes.h"
#include "OreRecipies.h"


/*
	private Object[][] map = {
		{
			Tile.goldBlock, new ItemInstance(Item.goldIngot, 9)
		}, {
			Tile.ironBlock, new ItemInstance(Item.ironIngot, 9)
		}, {
			Tile.diamondBlock, new ItemInstance(Item.diamond, 9)
			}, {
				Tile.lapisBlock, new ItemInstance(Item.dye_powder, 9, DyePowderItem.BLUE)
			},
	};
*/

void OreRecipies::_init()
{
	map = new vector <Object *> [MAX_ORE_RECIPES];

	ADD_OBJECT(map[0],Tile::goldBlock);
	ADD_OBJECT(map[0],new ItemInstance(Item::goldIngot, 9));

	ADD_OBJECT(map[1],Tile::ironBlock);
	ADD_OBJECT(map[1],new ItemInstance(Item::ironIngot, 9));

	ADD_OBJECT(map[2],Tile::diamondBlock);
	ADD_OBJECT(map[2],new ItemInstance(Item::diamond, 9));

	ADD_OBJECT(map[3],Tile::emeraldBlock);
	ADD_OBJECT(map[3],new ItemInstance(Item::emerald, 9));

	ADD_OBJECT(map[4],Tile::lapisBlock);
	ADD_OBJECT(map[4],new ItemInstance(Item::dye_powder, 9, DyePowderItem::BLUE));
}
void OreRecipies::addRecipes(Recipes *r) 
{

	for (int i = 0; i < MAX_ORE_RECIPES; i++) 
	{
		Tile *from = (Tile*) map[i].at(0)->tile;
		ItemInstance *to = (ItemInstance*) map[i].at(1)->iteminstance;
		r->addShapedRecipy(new ItemInstance(from), //
			L"sssczg",
			L"###", //
			L"###", //
			L"###", //

			L'#', to,
			L'D');

		r->addShapedRecipy(to, //
			L"sctg",
			L"#", //

			L'#', from,
			L'D');
	}		
}


