/*********************************************************
*   (c) Mojang. All rights reserved                      *
*   (c) Microsoft. All rights reserved.                  *
*********************************************************/

#pragma once

#include "client/renderer/texture/TextureUVCoordinateSet.h"
#include "CommonTypes.h"
#include "Core/Math/Color.h"
#include "nbt/CompoundTag.h"
#include "world/item/Item.h"
// #include "world/item/enchanting/ItemEnchants.h"
// #include "world/item/UseAnim.h"
// #include "world/item/MapItem.h"
#include "EntityUniqueID.h"
#include "Core/Debug/Log.h"

class Block;
class Entity;
// class Item;
class Level;
class Mob;
class Player;

/*final*/
class ItemInstance {

	static const int MAX_STACK_SIZE;

public:

	ItemInstance();
	explicit ItemInstance(bool valid);

	explicit ItemInstance(const Block* block);// for catching nullptr inits
	ItemInstance(const Block* block, int count);
	ItemInstance(const Block* block, int count, int auxValue);

	explicit ItemInstance(const Item* item);// for catching nullptr inits
	ItemInstance(const Item* item, int count);
	ItemInstance(const Item* item, int count, int auxValue);
	ItemInstance(const Item* item, int count, int auxValue, const CompoundTag* _userData);
	ItemInstance(const ItemInstance& rhs);
	ItemInstance(int id, int count, int auxValue);
	ItemInstance(int id, int count, int auxValue, const CompoundTag* _userData);

	void init(int id, int count, int aux_);
	bool isNull() const;
	void setNull();
	bool operator==(const ItemInstance& rhs) const;
	bool operator!=(const ItemInstance& rhs) const;
	bool matches(const ItemInstance* b) const;

	// assignment overload is necessary because of compound tag
	ItemInstance& operator=(const ItemInstance& rhs);

	// adding and removing amounts to and from the stackSize
	void add(const int inCount);
	void remove(const int inCount);
	void set(const int inCount);

	void onCraftedBy(Level& level, Player& player);

	bool hasUserData(void) const;
	bool hasSameUserData(const ItemInstance& rhs) const;
	void setUserData(Unique<CompoundTag> tag);

	const Unique<CompoundTag>& getUserData(void) const;

	Unique<CompoundTag> getNetworkUserData(void) const;

	const TextureUVCoordinateSet& getIcon(int frame, bool inInventoryPane = false) const;

	Item* getItem() const {
		return mItem;
	}

	const Block* getBlock() const {
		return mBlock;
	}

	int getId() const;
	int getIdAux() const;
	int getIdAuxEnchanted() const;

	bool isInstance(const Item* i) const {
		return i == mItem && i != nullptr;	//null is not an instance of null
	}

	bool isInstance(const Block* t) const {
		return t == mBlock && t != nullptr;
	}

	bool isItem() const {
		return mItem != nullptr;
	}

	bool isBlock() const {
		return mBlock != nullptr;
	}

	bool isValid() const {
		return mValid;
	}

	bool isValidAuxValue(int value) const {
		if (isItem()) {
			return mItem->isValidAuxValue(value);
		}
		else {
			return mBlock->isValidAuxValue(value);
		}
	}

	bool isThrowable() const;

	bool isExplodable() const { return mItem ? mItem->isExplodable() : false; }

	float getDestroySpeed(const Block* block);

	bool useOn(Entity& entity, int x, int y, int z, FacingID face, float clickX, float clickY, float clickZ);
	ItemInstance& use(Player& player);
	
	void inventoryTick(Level& level, Entity& owner, int slot, bool selected);
	
	/**
	 * Returns true if this item type only can be stacked with items that have
	 * the same auxValue data.
	 */
	bool isStackedByData() const;
	bool isStackable() const;
	byte getMaxStackSize() const;
	bool isFullStack() const;

	byte getStackSize() const {
		return mCount;
	}

	void setStackSize(const byte inCount) {
		DEBUG_ASSERT(inCount >= 0, "stack was negative");
		DEBUG_ASSERT(inCount <= getMaxStackSize(), "itemStack too big!");
		mCount = Math::clamp(inCount, (byte)0, getMaxStackSize());
	}

	void forceSetCount(const byte inCount) {
		mCount = inCount;
	}

	bool isEmptyStack() const {
		DEBUG_ASSERT(mCount >= 0, "stack was negative");
		return mCount == 0;
	}

	static bool isStackable(const ItemInstance* a, const ItemInstance* b);

	bool isDamaged() const;
	bool isDamageableItem() const;
	short getDamageValue() const;
	short getMaxDamage() const;

	short getAuxValue() const;
	void setAuxValue(short value);

	void hurtAndBreak(int i, Entity* owner);
	void hurtEnemy(Mob* mob, Mob* attacker);

	void mineBlock(BlockID block, int x, int y, int z, Mob* owner);
	int getAttackDamage();
	bool canDestroySpecial(const Block* block);
	void snap(Player* player);
	void useTimeDepleted(Level* level, Player* player);
	bool isLiquidClipItem();

	bool interactEnemy(Mob* mob, Player* player);

	static bool matches(const ItemInstance* a, const ItemInstance* b);
	static bool matchesNulls(const ItemInstance* a, const ItemInstance* b);

	static bool isItem(const ItemInstance* instance);
	static bool isArmorItem(const ItemInstance* instance);
	static bool isHorseArmorItem(const ItemInstance* instance);
	static bool isPotionItem(const ItemInstance* instance);
	static bool isWearableItem(const ItemInstance* instance);

	bool sameItem(const ItemInstance* b) const {
		return b && b->isInstance(mItem) /*&& MapItem::getMapId(*this) == MapItem::getMapId(*b)*/;
	}

	bool sameItemAndAux(const ItemInstance* b) const;

	static ItemInstance* clone(const ItemInstance* item);
	static ItemInstance cloneSafe(const ItemInstance* item);

	ItemInstance getStrippedNetworkItem() const;

	std::string getName() const;
	std::string getRawNameId() const;
	std::string getEffectName() const;
	std::string getFormattedHovertext(Level& level, const bool advancedToolTops) const;
	ItemInstance* setDescriptionId(const std::string& id);
	std::string toString() const;
	Color getColor() const;

	Unique<CompoundTag> save();
	void load(const CompoundTag& compoundTag);
	static ItemInstance* fromTag(const CompoundTag& tag);

	void releaseUsing(Player* player, int durationLeft);
	int getMaxUseDuration() const;

	UseAnimation getUseAnimation() const;

	void useAsFuel();

	std::string getHoverName() const;
	std::string getCustomName() const;
	ItemInstance* setCustomName(const std::string& name);

	void resetHoverName();
	bool hasCustomHoverName() const;

	bool isEnchanted() const;
// 	void saveEnchantsToUserData(const ItemEnchants& enchant);
// 	ItemEnchants getEnchantsFromUserData() const;
	int getEnchantValue() const;
	int getEnchantSlot() const;
	bool isEnchantingBook() const;

	int getBaseRepairCost() const;
	void setRepairCost(int cost);

	bool isGlint() const;

	//Just Crafted means the ItemInstance is waiting in a container to be picked up
	void setJustBrewed(bool crafted);
	bool wasJustBrewed() const;

	//Cooldown functions.
	void startCoolDown(Player* player);

	static const std::string TAG_DISPLAY;
	static const std::string TAG_DISPLAY_NAME;
	static const std::string TAG_REPAIR_COST;
	static const std::string TAG_ENCHANTS;

	bool isEquivalentArmor(const ItemInstance& rhs);

	static const int ID_MASK = ((1 << 16) - 1);         // (1 << 16) - 1 or [1111] [1111] [1111] [1111]
	static const int ENCHANT_MASK = (1 << 15);          // (1 << 15)     or [1000] [0000] [0000] [0000]
	static const int AUX_VALUE_MASK = ((1 << 15) - 1);  // (1 << 15) - 1 or [0111] [1111] [1111] [1111]  // To ignore the sign-bit/enchanted flag for UI rendering

	static int retrieveIDFromIDAux(int idAux);
	static bool retrieveEnchantFromIDAux(int idAux);
	static int retrieveAuxValFromIDAux(int idAux);

protected:

	/**
	 * This was previously the damage value, but is now used for different stuff
	 * depending on item / block. Use the getter methods to make sure the value
	 * is interpreted correctly.
	 */
	byte mCount;			// to modify this, we need checks. so call removeItem or addItem
	short mAuxValue;
	bool mValid:1;

	// extra data some items require when saving
	Unique<CompoundTag> mUserData = nullptr;

	Item* mItem;
	const Block* mBlock;


	bool _setItem(int id);
	std::string _getHoverFormattingPrefix() const;

};

//specialization for serialization
template<>
struct serialize<ItemInstance>{
	void static write(const ItemInstance& item, BinaryStream& stream){
		int id = item.getId();
		int extraValue = (item.getAuxValue() << 8) | (item.getStackSize() & 0xff);

		if(id <= 0 || !item.isValid()) {
			stream.writeVarInt(0);
		}
		else{
			stream.writeVarInt(id);
			stream.writeVarInt(extraValue);

			BinaryDataOutput output(stream);
			Item::mItems[id]->writeUserData(&item, output);
		}
	}

	ItemInstance static read(ReadOnlyBinaryStream& stream){
		byte count;
		short aux;

		int id = stream.getVarInt();

		if(id <= 0) {
			id = 0;
			count = 0;
			aux = 0;
		}
		else{
			int extraValue = stream.getVarInt();
			count = extraValue & 0xff;
			aux = (extraValue >> 8) & 0xffff;
		}

		BinaryDataInput input(stream);
		ItemInstance itemInstance(id, count, aux);
		if (itemInstance.isValid()) {
			if(id > 0 && id < Item::MAX_ITEMS) {
				auto& item = Item::mItems[id];
				if(item) {
					item->readUserData(&itemInstance, input);
				}
				else {
					Item dummy("",0);
					dummy.readUserData(&itemInstance, input);
					LOGW("ItemInstance deserialisation error: id %d is null\n", id);
					return ItemInstance(false);
				}
			}
			else if(id != 0) {
				Item dummy("",0);
				dummy.readUserData(&itemInstance, input);	// still eat data from the stream, so the rest doesnt break
				LOGW("ItemInstance id %d is out of range (0/%d)\n", id, Item::MAX_ITEMS);
				return ItemInstance(false);
			}

			return itemInstance;
		}
		else {
			LOGI("Error: bad item ID received from the server (%hd)\n", id);
			return ItemInstance(false);
		}
	}
};
