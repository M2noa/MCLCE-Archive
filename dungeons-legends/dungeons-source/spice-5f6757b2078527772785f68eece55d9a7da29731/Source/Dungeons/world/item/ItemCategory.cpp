#include "Dungeons.h"

#include "world/item/ItemCategory.h"

ItemCategory ItemCategoryFromString(const std::string& str) {
	if (str.empty()) {
		return ItemCategory::All;
	}

	// Lazy Init of static lookup map
	static std::unordered_map<std::string, ItemCategory> ENUM_MAP;
	if (ENUM_MAP.empty()) {
		ENUM_MAP["all"] = ItemCategory::All;
		ENUM_MAP["structures"] = ItemCategory::Structures;
		ENUM_MAP["tools"] = ItemCategory::Tools;
		ENUM_MAP["foodarmor"] = ItemCategory::FoodArmor;
		ENUM_MAP["decorations"] = ItemCategory::Decorations;
		ENUM_MAP["mechanisms"] = ItemCategory::Mechanisms;
	}

	// Lookup case insensitive by using *lower case* keys.
	auto i = ENUM_MAP.find(Util::toLower(str));
	if (i != ENUM_MAP.end()) {
		return i->second;
	}

	return ItemCategory::All;
}

CreativeItemCategory CreativeItemCategoryFromString(const std::string& str) {
	if (str.empty()) {
		return CreativeItemCategory::All;
	}

	// Lazy Init of static lookup map
	static std::unordered_map<std::string, CreativeItemCategory> ENUM_MAP = {
		{ "all", CreativeItemCategory::All },
		{ "buildingblocks", CreativeItemCategory::BuildingBlocks },
		{ "decorations", CreativeItemCategory::Decorations },
		{ "tools", CreativeItemCategory::Tools },
		{ "miscellaneous", CreativeItemCategory::Miscellaneous },
		{ "none", CreativeItemCategory::None }
	};

	// Lookup case insensitive by using *lower case* keys.
	auto i = ENUM_MAP.find(Util::toLower(str));
	if (i != ENUM_MAP.end()) {
		return i->second;
	}

	return CreativeItemCategory::All;
}