//package net.minecraft.world.item.crafting;

//import net.minecraft.world.item.*;
//import net.minecraft.world.level.tile.Tile;
#pragma once

#define MAX_ARMOUR_RECIPES 5
class ArmorRecipes 
{
public:
	enum _eArmorType
	{
		eArmorType_None=0,
		eArmorType_Helmet,
		eArmorType_Chestplate,
		eArmorType_Leggings,
		eArmorType_Boots,
	}
	eArmorType;

	// 4J - added for common ctor code
	void _init();
	ArmorRecipes()			{_init();}
private:
	// 4J-PB - this wasn't static in java, so might not be right
	static wstring shapes[][4];

private:	
	vector <Object *> *map;

public:
	void addRecipes(Recipes *r); 
	static _eArmorType GetArmorType(int iId);
};
