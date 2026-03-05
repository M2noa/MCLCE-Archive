#pragma once

class FurnaceRecipes 
{

private:
	static FurnaceRecipes *instance;

public:
	static void staticCtor();

private:
	//Map<Integer, ItemInstance> recipies = new HashMap<Integer, ItemInstance>();
	unordered_map<int, ItemInstance *> recipies;
	unordered_map<int, float> recipeValue;

public:
	static FurnaceRecipes *getInstance();

public:
	FurnaceRecipes();

public:
	void addFurnaceRecipy(int itemId, ItemInstance *result, float value);
	bool isFurnaceItem(int itemId);
	ItemInstance *getResult(int itemId);
	unordered_map<int, ItemInstance *> *getRecipies();
	float getRecipeValue(int itemId);

};
