/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "util/ItemListSerializer.h"

#include "nbt/Tag.h"
#include "nbt/ListTag.h"
#include "nbt/StringTag.h"
#include "world/item/Item.h"

void loadItem(std::string & str, std::set<const Item*>& items) {
	Item * item = Item::lookupByName(str);
	if (item) {
		items.insert(item);
	}
}

void ItemListSerializer::loadJSONSet(const std::string& name, std::set<const Item*>& itemSet, Json::Value root) {
	Json::Value items = root[name];
	if (!items.empty()) {
		if (items.isString()) {
			std::string tname = items.asString();
			loadItem(tname, itemSet);
		}
		else if (items.isArray()) {
			for (auto iter = items.begin(); iter != items.end(); ++iter) {
				std::string tname = (*iter).asString();
				loadItem(tname, itemSet);
			}
		}
	}
}

void ItemListSerializer::saveNBTSet(const std::string& name, CompoundTag& tag, const std::set<const Item*>& items) {
	auto listTag = make_unique<ListTag>();
	for (auto& item : items) {
		listTag->add(make_unique<StringTag>(item->getDescriptionId() ));
	}
	tag.put(name, std::move(listTag));
}

void ItemListSerializer::loadNBTSet(const std::string& name, std::set<const Item*>& itemSet, const CompoundTag& tag) {
	if(tag.contains(name, Tag::Type::List)) {
		auto listTag = tag.getList(name);
		for (int i = 0; i < listTag->size(); ++i) {
			std::string tname = listTag->get(i)->toString();
			loadItem(tname, itemSet);
		}		
	}
}
