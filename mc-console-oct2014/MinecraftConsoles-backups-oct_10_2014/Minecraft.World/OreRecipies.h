#pragma once

#define MAX_ORE_RECIPES 5

class OreRecipies 
{
public:
	// 4J - added for common ctor code
	void _init();
	OreRecipies()			{_init();}

private:
	vector <Object *> *map;

public:
	void addRecipes(Recipes *r);
};