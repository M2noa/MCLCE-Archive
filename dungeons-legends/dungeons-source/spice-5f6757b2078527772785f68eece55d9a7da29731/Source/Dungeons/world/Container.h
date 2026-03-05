/*********************************************************
*   (c) Mojang. All rights reserved                      *
*   (c) Microsoft. All rights reserved.                  *
*********************************************************/

#pragma once

class ItemInstance;
class CompoundTag;
class BlockSource;
class Player;
class Vec3;

enum class ContainerType : signed char {
	NONE = -9,
	INVENTORY = -1,
	CONTAINER = 0,
	WORKBENCH = 1,
	FURNACE = 2,
	ENCHANTMENT = 3,
	BREWING_STAND = 4,
	ANVIL = 5,
	DISPENSER = 6,
	DROPPER = 7,
	HOPPER = 8,
	CAULDRON = 9,
	MINECART_CHEST = 10,
	MINECART_HOPPER = 11,
	HORSE = 12,
	BEACON = 13,
	STRUCTURE_EDITOR = 14
};

//This looks like a collection of weird special values, can't use as enum class
enum ContainerID : signed char {
	CONTAINER_ID_NONE = -1,
	CONTAINER_ID_INVENTORY = 0,

	CONTAINER_ID_FIRST = 1,

	CONTAINER_ID_LAST = 100,

	CONTAINER_ID_ARMOR = 120,
	CONTAINER_ID_CREATIVE = 121,
	CONTAINER_ID_SELECTION_SLOTS = 122,
	CONTAINER_ID_FIXEDINVENTORY = 123
};

class ContainerSizeChangeListener {
public:
	virtual void containerSizeChanged(int size) = 0;
	virtual ~ContainerSizeChangeListener() {
	}
};

class ContainerContentChangeListener {
public:

	virtual void containerContentChanged(int slot) = 0;
	virtual ~ContainerContentChangeListener() {
	}

};

class Container {
public:

	static const int LARGE_MAX_STACK_SIZE = 64;
	static const int DEFAULT_CONTAINER_SIZE = 27;

	Container(ContainerType type);
	Container(ContainerType type, std::string const& name, bool customName);
	virtual ~Container();

	void addSizeChangeListener(ContainerSizeChangeListener* listener);
	void removeSizeChangeListener(ContainerSizeChangeListener* listener);
	virtual void addContentChangeListener(ContainerContentChangeListener* listener);
	virtual void removeContentChangeListener(ContainerContentChangeListener* listener);

	virtual ItemInstance* getItem(int slot) const = 0;
	virtual void addItem(ItemInstance* item);
	virtual bool addItemToFirstEmptySlot(ItemInstance* item);
	virtual void setItem(int slot, const ItemInstance* item) = 0;
	virtual void removeItem(int slot, int i) = 0;

	virtual int getRandomEmptySlot(Random& random);

	virtual void dropContents(BlockSource& region, const Vec3& pos);

	virtual std::string getName() const = 0;
	virtual int getContainerSize() const = 0;
	virtual int getMaxStackSize() const = 0;

	virtual void startOpen(Player&) = 0;
	virtual void stopOpen(Player&) = 0;

	virtual std::vector<ItemInstance> getSlotCopies() const;
	virtual std::vector<ItemInstance*> getSlots();
	virtual int getItemCount(int itemId);

	virtual bool canPushInItem(BlockSource& region, int slot, int face, ItemInstance* item);
	virtual bool canPullOutItem(BlockSource& region, int slot, int face, ItemInstance* item);

	ContainerType getContainerType();
	int getRedstoneSignalFromContainer();

	void setSizeChanged(int slot);
	virtual void setContainerChanged(int slot);
	virtual void setContainerMoved();

	virtual void setCustomName(const std::string& name);
	virtual bool hasCustomName() const;

	virtual void readAdditionalSaveData(const CompoundTag& tag);
	virtual void addAdditionalSaveData(CompoundTag& tag);

	static ContainerType StringToContainerType(const std::string& name);

protected:
	ContainerType mContainerType;
	std::unordered_set<ContainerContentChangeListener*> mContentChangeListeners;
	std::unordered_set<ContainerSizeChangeListener*> mSizeChangeListeners;
	std::string mName;
	bool mCustomName = false;
};
