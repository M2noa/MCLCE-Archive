/*********************************************************
*   (c) Mojang. All rights reserved                      *
*   (c) Microsoft. All rights reserved.                  *
*********************************************************/

#include "Dungeons.h"

#include "world/Container.h"

#include "world/item/ItemInstance.h"
#include "world/redstone/Redstone.h"
// #include "world/entity/item/ItemEntity.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "util/Random.h"

// #include "world/level/Spawner.h"

Container::Container(ContainerType type)
	: mContainerType(type)
	, mName("")
	, mCustomName(false) {
}

Container::Container(ContainerType type, std::string const& name, bool customName)
	: mContainerType(type)
	, mName(name)
	, mCustomName(customName) {

}

Container::~Container() {

}

void Container::addSizeChangeListener(ContainerSizeChangeListener* listener) {
	mSizeChangeListeners.insert(listener);
}

void Container::removeSizeChangeListener(ContainerSizeChangeListener* listener) {
	mSizeChangeListeners.erase(listener);
}

void Container::addContentChangeListener(ContainerContentChangeListener* listener) {
	mContentChangeListeners.insert(listener);
}

void Container::removeContentChangeListener(ContainerContentChangeListener* listener) {
	mContentChangeListeners.erase(listener);
}

void Container::addItem(ItemInstance* item) {
// 	int size = getContainerSize();
// 	for (int i : range(size)) {
// 		if (item->isNull() || item->getStackSize() <= 0) {
// 			return;
// 		}
// 
// 		ItemInstance* slotItem = getItem(i);
// 		if (slotItem == nullptr || slotItem->isNull()) {
// 			setItem(i, item);
// 			return;
// 		} else if (ItemInstance::isStackable(slotItem, item)) {
// 			if (slotItem->getStackSize() != slotItem->getMaxStackSize()) {
// 				int newStackSize = slotItem->getStackSize() + item->getStackSize();
// 				if (newStackSize > slotItem->getMaxStackSize()) {
// 					item->setStackSize(newStackSize - slotItem->getMaxStackSize());
// 					slotItem->setStackSize(slotItem->getMaxStackSize());
// 				} else {
// 					item->setStackSize(0);
// 					slotItem->setStackSize(newStackSize);
// 				}
// 			}
// 		}
// 	}
}

bool Container::addItemToFirstEmptySlot(ItemInstance* item) {
// 	int size = getContainerSize();
// 	for (int i : range(size)) {
// 		ItemInstance* slotItem = getItem(i);
// 		if (slotItem == nullptr || slotItem->isNull()) {
// 			setItem(i, item);
// 			return true;
// 		}
// 	}

	return false;
}

int Container::getRandomEmptySlot(Random& random) {
	int replaceSlot = -1;
	int replaceOdds = 1;

// 	for (int i = 0; i < getContainerSize(); i++) {
// 		ItemInstance* item = getItem(i);
// 		if (!item->isNull() && random.nextInt(replaceOdds++) == 0) {
// 			replaceSlot = i;
// 		}
// 	}

	return replaceSlot;
}

void Container::dropContents(BlockSource& region, const Vec3& pos) {
// 	Random& random = region.getLevel().getRandom();
// 
// 	for (int i = 0; i < getContainerSize(); i++) {
// 		ItemInstance *item = getItem(i);
// 		if (item != nullptr) {
// 			float xo = random.nextFloat() * 0.8f + 0.1f;
// 			float yo = random.nextFloat() * 0.8f + 0.1f;
// 			float zo = random.nextFloat() * 0.8f + 0.1f;
// 
// 			while (item->getStackSize() > 0) {
// 				int count = random.nextInt(21) + 10;
// 				if (count > item->getStackSize()) {
// 					count = item->getStackSize();
// 				}
// 
// 				ItemEntity *itemEntity = region.getLevel().getSpawner().spawnItem(region, ItemInstance(item->getItem(), count, item->getAuxValue(), item->hasUserData() ? item->getUserData().get() : nullptr), nullptr, pos + Vec3(xo, yo, zo), 10);
// 
// 				item->remove(count);
// 
// 				float pow = 0.05f;
// 				float offx = random.nextGaussian() * pow;
// 				float offy = random.nextGaussian() * pow + 0.2f;
// 				float offz = random.nextGaussian() * pow;
// 				itemEntity->mPosDelta = Vec3(offx, offy, offz);
// 			}
// 
// 			setContainerChanged(i);
// 		}
// 	}
}

std::vector<ItemInstance> Container::getSlotCopies() const {
	std::vector<ItemInstance> items;
// 	ItemInstance NullItem;
// 
// 	for (int i = 0; i < getContainerSize(); ++i) {
// 		ItemInstance* item = getItem(i);
// 		items.push_back( item ? *item : NullItem );
// 	}
	return items;
}

std::vector<ItemInstance*> Container::getSlots() {
	std::vector<ItemInstance*> items;

	for (int i = 0; i < getContainerSize(); ++i) {
		items.push_back(getItem(i));
	}
	return items;
}

int Container::getItemCount(int itemId) {
	int itemCount = 0;

// 	for (int i = 0; i < getContainerSize(); ++i) {
// 		ItemInstance* item = getItem(i);
// 		if (item != nullptr && item->getId() == itemId) {
// 			itemCount += item->getStackSize();
// 		}
// 	}
	return itemCount;
}

bool Container::canPushInItem(BlockSource& region, int slot, int face, ItemInstance* item) {
	return true;
}

bool Container::canPullOutItem(BlockSource& region, int slot, int face, ItemInstance* item) {
	return true;
}

ContainerType Container::getContainerType() {
	return mContainerType;
}

int Container::getRedstoneSignalFromContainer() {
	int count = 0;
	float totalPct = 0;

// 	for (int i = 0; i < getContainerSize(); i++) {
// 		auto item = getItem(i);
// 		if (item != nullptr && item->getStackSize() > 0) {
// 			totalPct += item->getStackSize() / (float)Math::min(getMaxStackSize(), item->getMaxStackSize());
// 			count++;
// 		}
// 	}

	totalPct /= getContainerSize();
	return Math::floor(totalPct * (Redstone::SIGNAL_MAX - 1)) + (count > 0 ? 1 : 0);
}

void Container::setSizeChanged(int slot) {
	for (ContainerSizeChangeListener* listener : mSizeChangeListeners) {
		if (listener) {
			listener->containerSizeChanged(slot);
		}
	}
}


void Container::setContainerChanged(int slot) {
	for (ContainerContentChangeListener* listener : mContentChangeListeners) {
		if (listener) {
			listener->containerContentChanged(slot);
		}
	}
}

void Container::setContainerMoved() {
	for (auto it = mContentChangeListeners.begin(); it != mContentChangeListeners.end();) {
		if ((*it) != nullptr) {
			it = mContentChangeListeners.erase(it);
		} else
			it++;
	}
}

void Container::setCustomName(const std::string& name) {
	mCustomName = true;
	mName = name;
}

bool Container::hasCustomName() const {
	return mCustomName;
}

void Container::readAdditionalSaveData(const CompoundTag& tag) {
// 	const int size = getContainerSize();
// 	for (int i = 0; i < size; i++) {
// 		ItemInstance* item = getItem(i);
// 		if (item != nullptr) {
// 			item->setNull();
// 		}
// 	}
// 
// 	if (auto inventoryList = tag.getList("Items")) {
// 		for (int i = 0; i < inventoryList->size(); i++) {
// 			Tag* tt = inventoryList->get(i);
// 			if (tt->getId() == Tag::Type::Compound) {
// 				CompoundTag* currentTag = (CompoundTag*)tt;
// 				int slot = currentTag->getByte("Slot");
// 				if (slot >= 0 && slot < size) {
// 					ItemInstance* loaded = ItemInstance::fromTag(*currentTag);
// 					if (loaded) {
// 						setItem(slot, loaded);
// 						delete loaded;
// 					} else {
// 						ItemInstance* item = getItem(i);
// 						if (item != nullptr) {
// 							item->setNull();
// 						}
// 					}
// 				}
// 			}
// 		}
// 	}
// 
// 	if (tag.contains("CustomName")) {
// 		mName = tag.getString("CustomName");
// 	}
// 
// 	mCustomName = tag.getBoolean("HasCustomName");
}

void Container::addAdditionalSaveData(CompoundTag& tag) {
// 	auto listTag = make_unique<ListTag>();
// 
// 	const int size = getContainerSize();
// 	for (int i = 0; i < size; i++) {
// 		auto item = getItem(i);
// 		if (item != nullptr && !item->isNull()) {
// 			auto currentTag = item->save();
// 			currentTag->putByte("Slot", (char)i);
// 			listTag->add(std::move(currentTag));
// 		}
// 	}
// 
// 	tag.put("Items", std::move(listTag));
// 
// 	if (!mName.empty()) {
// 		tag.putString("CustomName", mName);
// 	}
// 
// 	tag.putBoolean("HasCustomName", mCustomName);
}

ContainerType Container::StringToContainerType(const std::string& name) {
	static const std::unordered_map<std::string, ContainerType> nameMap = {
		{"INVENTORY",		ContainerType::INVENTORY},
		{"CONTAINER",		ContainerType::CONTAINER},
		{"WORKBENCH",		ContainerType::WORKBENCH},
		{"FURNACE",			ContainerType::FURNACE},
		{"ENCHANTMENT",		ContainerType::ENCHANTMENT},
		{"BREWING_STAND",	ContainerType::BREWING_STAND},
		{"ANVIL",			ContainerType::ANVIL},
		{"DISPENSER",		ContainerType::DISPENSER},
		{"DROPPER",			ContainerType::DROPPER},
		{"HOPPER",			ContainerType::HOPPER},
		{"CAULDRON",		ContainerType::CAULDRON},
		{"MINECART_CHEST",	ContainerType::MINECART_CHEST},
		{"MINECART_HOPPER", ContainerType::MINECART_HOPPER},
		{"HORSE",			ContainerType::HORSE}
	};
	auto iter = nameMap.find(Util::toUpper(name));
	if(iter != nameMap.end()) {
		return iter->second;
	}
	return ContainerType::NONE;
}