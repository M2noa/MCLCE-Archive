#pragma once

enum class ItemCategory : int
{
	All = 0,
	Structures = 1,
	Tools = 2,
	FoodArmor = 4,
	Decorations = 8,
	Mechanisms = 16,

	NUM_CATEGORIES = 5,
};
ItemCategory ItemCategoryFromString(const std::string& str);


enum class CreativeItemCategory : int
{
	All = 0,
	BuildingBlocks = 1,
	Decorations = 2,
	Tools = 3,
	Miscellaneous = 4,

	None = 5,

	NUM_CATEGORIES = 6,
};
CreativeItemCategory CreativeItemCategoryFromString(const std::string& str);
