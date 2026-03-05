//package net.minecraft.world.item.crafting;

//import net.minecraft.world.item.*;
//import net.minecraft.world.level.tile.Tile;

#pragma once

#define MAX_WEAPON_RECIPES 2
class WeaponRecipies 
{
public:
	// 4J - added for common ctor code
	void _init();
	WeaponRecipies()			{_init();}

private:
	static wstring shapes[][4];
	vector <Object *> *map;
	
public:
	void addRecipes(Recipes *r);
};
