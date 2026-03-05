/*********************************************************
*   (c) Mojang. All rights reserved                      *
*   (c) Microsoft. All rights reserved.                  *
*********************************************************/

#include "Dungeons.h"

#include "world/item/ItemInstance.h"

#include "nbt/ByteArrayTag.h"
#include "util/ColorFormat.h"
#include "util/StringUtils.h"
#include "world/entity/Mob.h"
#include "world/entity/player/Player.h"
// #include "world/item/alchemy/Potion.h"
// #include "world/item/PotionItem.h"
#include "world/item/Item.h"
#include "world/level/block/Block.h"
#include "world/level/Level.h"

const std::string ItemInstance::TAG_DISPLAY = "display";
const std::string ItemInstance::TAG_DISPLAY_NAME = "Name";
const std::string ItemInstance::TAG_REPAIR_COST = "RepairCost";
const std::string ItemInstance::TAG_ENCHANTS = "ench";

const int ItemInstance::MAX_STACK_SIZE = 255;

struct IdPair
{
	short id, aux;
	IdPair(short i, short a) :id(i), aux(a) { }
	bool operator == (const IdPair & rhs) const { return id == rhs.id && aux == rhs.aux; }
};

// Promote old id/aux pairs to new id/aux pairs.
static void PromoteIdPair(IdPair & inout)
{
	struct IdPairHasher
	{
		std::size_t operator()(const IdPair & k) const { return ((std::hash<int>()(k.id) ^ (std::hash<int>()(k.aux) << 1)) >> 1); }
	};

	static std::unordered_map<IdPair, IdPair, IdPairHasher> PROMOTE_MAP = {

		// Old (id,aux)      -> New (id,aux)
		{ IdPair(256 + 93, 1), IdPair(256 + 204, 0) }, // fish.raw.salmon	
		{ IdPair(256 + 93, 2), IdPair(256 + 205, 0) }, // fish.raw.clownfish
		{ IdPair(256 + 93, 3), IdPair(256 + 206, 0) }, // fish.raw.pufferfish
		{ IdPair(256 + 94, 1), IdPair(256 + 207, 0) }, // fish.cooked.salmon
		{ IdPair(256 + 66, 1), IdPair(256 + 210, 0) }, // appleEnchanted
	};

	auto i = PROMOTE_MAP.find(inout);
	if (i != PROMOTE_MAP.end()) {
		inout = i->second;
	}
}

ItemInstance::ItemInstance() {
	init(0, 0, 0);
}

ItemInstance::ItemInstance(bool valid_) {
	init(0, 0, 0);
	mValid = valid_;
}

ItemInstance::ItemInstance(const Block* block) {
	init(block->mID, 1, 0);
}

ItemInstance::ItemInstance(const Block* block, int count) {
	init(block->mID, count, 0);
}

ItemInstance::ItemInstance(const Block* block, int count, int auxValue) {
	init(block->mID, count, auxValue);
}

ItemInstance::ItemInstance(const Item* item) {
	init(item->getId(), 1, 0);
}

ItemInstance::ItemInstance(const Item* item, int count) {
	init(item->getId(), count, 0);
}

ItemInstance::ItemInstance(const Item* item, int count, int auxValue) {
	init(item->getId(), count, auxValue);
}

ItemInstance::ItemInstance(const Item* item, int count, int auxValue, const CompoundTag* _userData) {
	init(item->getId(), count, auxValue);
	mUserData = _userData ? _userData->clone() : nullptr;
}

ItemInstance::ItemInstance(int id, int count, int auxValue) {
	init(id, count, auxValue);
}

ItemInstance::ItemInstance(int id, int count, int auxValue, const CompoundTag* _userData) {
	init(id, count, auxValue);
	mUserData = _userData ? _userData->clone() : nullptr;
}

ItemInstance::ItemInstance(const ItemInstance& rhs) {
	init(rhs.getId(), rhs.mCount, rhs.mAuxValue);
	mUserData = rhs.mUserData ? rhs.mUserData->clone() : nullptr;
}

ItemInstance ItemInstance::getStrippedNetworkItem() const {
	if (!mItem) {
		return ItemInstance();
	}

	return ItemInstance(mItem, mCount, mAuxValue, getNetworkUserData().get());
}

bool ItemInstance::_setItem(int id) {

	mItem = (id >= 0 && id < Item::MAX_ITEMS) ? Item::mItems[id] : nullptr;

	// Block Item's specify their own blockId.
	if (mItem != nullptr)
		id = mItem->getBlockId();

	mValid = mItem != nullptr || id == 0;	//id 0 is allowed to have a null item... not great but duh
	if (!mValid) {
		mItem = nullptr;
		mBlock = 0;
		mCount = 0;
		mAuxValue = -1;
	}
	else {
		mBlock = id > 0 && id < 256 ? Block::mBlocks[id] : nullptr;
	}

	return mValid;
}

void ItemInstance::init(int id, int count_, int aux_) {
	DEBUG_ASSERT(count_ >= 0, "A negative value will wrap around!");
	mCount = std::max(count_, 0);
	mAuxValue = aux_;
	_setItem(id);
}

bool ItemInstance::isNull() const {
	return !mValid || (mCount == 0 && mAuxValue == 0 && mItem == nullptr && mBlock == nullptr && mUserData == nullptr);
}

void ItemInstance::setNull() {
	mCount = 0;
	mAuxValue = 0;
	mItem = nullptr;
	mBlock = nullptr;
	mUserData.reset();
}

void ItemInstance::add(const int inCount) {
	set(mCount + inCount);
}

void ItemInstance::remove(int inCount) {
	set(mCount - inCount);
}

void ItemInstance::set(const int inCount) {
	DEBUG_ASSERT(inCount >= 0, "stack was negative");
	DEBUG_ASSERT(inCount <= getMaxStackSize(), "itemStack too big!");
	mCount = Math::clamp((byte)inCount, (byte)0, getMaxStackSize());

	if (isEmptyStack()) {
		setNull();
	}
}

void ItemInstance::onCraftedBy(Level& level, Player& player) {
	if (isItem()) {
		mItem->onCraftedBy(*this, level, player);
	}
}

bool ItemInstance::hasUserData(void) const {
	return mUserData != nullptr;
}

bool ItemInstance::hasSameUserData(const ItemInstance& b) const {
	if (isNull() && b.isNull()) {
		return true;
	}

	if (isNull() || b.isNull()) {
		return false;
	}

	if ((mUserData == nullptr && b.getUserData() != nullptr) || (mUserData != nullptr && b.getUserData() == nullptr)) {
		return false;
	}

	if (mUserData != nullptr && !mUserData->equals(*b.getUserData())) {
		return false;
	}

	return true;
}

void ItemInstance::setUserData(Unique<CompoundTag> tag) {
	mUserData = std::move(tag);
}

const Unique<CompoundTag>& ItemInstance::getUserData(void) const {
	return mUserData;
}

Unique<CompoundTag> ItemInstance::getNetworkUserData(void) const {
	if (!mUserData) {
		return nullptr;
	}

	auto tag = make_unique<CompoundTag>();

// 	if (mUserData) {
// 		for (auto&& entry : mUserData->rawView()) {
// 			const std::string& name = entry.first;
// 
// 			if (name == TAG_REPAIR_COST) {
// 				continue;
// 			}
// 			else if (name == TAG_ENCHANTS) {
// 				auto emptyTag = make_unique<ListTag>();
// 				tag->put(name, std::move(emptyTag));
// 			}
// 			else {
// 				tag->put(name, entry.second->copy());
// 			}
// 		}
// 	}

	return tag;
}

bool ItemInstance::useOn(Entity& entity, int x, int y, int z, FacingID face, float clickX, float clickY, float clickZ) {
	return mItem->useOn(*this, entity, x, y, z, face, clickX, clickY, clickZ);
}

bool ItemInstance::isGlint() const {
	return (mItem != nullptr) ? mItem->isGlint(this) : false;
}

void ItemInstance::setJustBrewed(bool crafted) {
	if (crafted) {
		if (!hasUserData()) {
			setUserData(make_unique<CompoundTag>());
		}

		CompoundTag* tag = getUserData().get();
		tag->putBoolean("wasJustBrewed", crafted);
	}
	else if(hasUserData()) {
		CompoundTag* tag = getUserData().get();
		tag->remove("wasJustBrewed");
	}
}

bool ItemInstance::wasJustBrewed() const {
	if (hasUserData()) {
		CompoundTag* tag = getUserData().get();
		return tag->getBoolean("wasJustBrewed");
	}

	return false;
}

void ItemInstance::startCoolDown(Player* player) {
// 	if (mItem != nullptr && player != nullptr) {
// 		player->startCooldown(mItem);
// 	}
}

bool ItemInstance::isEquivalentArmor(const ItemInstance& rhs) {
	// Check item ID and user data for equivalence
	if (rhs.getId() != getId()) {
		return false;
	}

	if (rhs.getUserData() != getUserData()) {
		return false;
	}

	// If neither item is damageable, check for identical aux values
	if (!rhs.isDamageableItem() && !isDamageableItem()) {
		if (rhs.getAuxValue() != getAuxValue()) {
			return false;
		}
	}

	return true;
}

int ItemInstance::retrieveIDFromIDAux(int idAux) {
	// AuxID is a 4-byte integer split into two halves
	// Left two bytes represents an Item's ID
	// Shift right by 16 bits and mask with ID Mask to retrieve Item's ID
	return ((idAux >> 16) & ID_MASK);
}

bool ItemInstance::retrieveEnchantFromIDAux(int idAux) {
	// Enchanted status is stored as a single-bit boolean flag held in the rightmost bit
	// Of the two rightmost bytes of the auxID
	// [byte] [byte] ([byte] [byte]) -- these two are the aux value and enchanted bit
	// [Exxx] [xxxx] [xxxx] [xxxx] -- E is the enchanted bit
	// To retrieve, use the Enchant mask
	return ((idAux & ENCHANT_MASK) == ENCHANT_MASK ? true : false);
}

int ItemInstance::retrieveAuxValFromIDAux(int idAux) {
	// Aux value is stored in the rightmost 2 bytes
	// To retrieve it properly we must ignore the enchanted status bit within those, however
	return (idAux & AUX_VALUE_MASK);
}

bool ItemInstance::isThrowable() const {
	return mItem && mItem->isThrowable();
}

float ItemInstance::getDestroySpeed(const Block* block) {
	return mItem->getDestroySpeed(this, block);
}

ItemInstance& ItemInstance::use(Player& player) {
// 	if (!player.isItemInCooldown(mItem->getCooldownType())) {
// 		return mItem->use(*this, player);
// 	}
	return *this;
}

void ItemInstance::inventoryTick(Level& level, Entity& owner, int slot, bool selected) {
	if (mItem != nullptr) {
		mItem->inventoryTick(*this, level, owner, slot, selected);
	}
}

void ItemInstance::useAsFuel() {
	if (mCount == 1 && mItem == Item::mBucket && getAuxValue() == Block::mFlowingLava->mID) {//transform this item into an empty
																				// bucket
		*this = ItemInstance(mItem, 1, 0);
	}
	else {
		remove(1);
	}
}

bool ItemInstance::isEnchanted() const {
	return mUserData && mUserData->contains(TAG_ENCHANTS, Tag::Type::List);
}

// ItemEnchants ItemInstance::getEnchantsFromUserData() const {
// 	return isEnchanted() ? ItemEnchants(getEnchantSlot(), *mUserData->getList(TAG_ENCHANTS)) : ItemEnchants(getEnchantSlot());
// }
// 
// void ItemInstance::saveEnchantsToUserData(const ItemEnchants& enchant) {
// 	// don't add to userdata if empty enchants, this will cause isEnchanted() to incorrectly return true
// 	if (!enchant.isEmpty()) {
// 		// if no userdata exists yet, add it
// 		if (!mUserData) {
// 			mUserData = std::make_unique<CompoundTag>();
// 		}
// 		mUserData->put(TAG_ENCHANTS, enchant._toList());
// 	}
// 	// empty enchant saved, remove the previous enchant save data from userdata
// 	else if (mUserData) {
// 		mUserData->remove(TAG_ENCHANTS);
// 	}
// }

int ItemInstance::getEnchantValue() const {
	return mItem ? mItem->getEnchantValue() : 0;
}

int ItemInstance::getEnchantSlot() const {
// 	return mItem ? mItem->getEnchantSlot() : Enchant::Slot::NONE;
	return mItem ? mItem->getEnchantSlot() : 0;
}

bool ItemInstance::isEnchantingBook() const {
	return isValid() && isItem() && mItem->getId() == Item::mEnchanted_book->getId() && isEnchanted();
}

byte ItemInstance::getMaxStackSize() const {
	return mItem ? mItem->getMaxStackSize(this) : MAX_STACK_SIZE;
}

bool ItemInstance::isFullStack() const {
	return mCount >= getMaxStackSize();
}

bool ItemInstance::isStackable() const {
	return getMaxStackSize() > 1 && (!isDamageableItem() || !isDamaged());
}

bool ItemInstance::isStackable(const ItemInstance* a, const ItemInstance* b) {
	return a && b && a->mItem == b->mItem && b->isStackable()
		&& (!b->isStackedByData() || a->getAuxValue() == b->getAuxValue())
		&& a->hasSameUserData(*b);
}

bool ItemInstance::isDamageableItem() const {
	return isItem() && mItem->getMaxDamage() > 0;
}

/**
 * Returns true if this item type only can be stacked with items that have
 * the same auxValue data.
 *
 * @return
 */
bool ItemInstance::isStackedByData() const {
	return mItem->isStackedByData();
}

bool ItemInstance::isDamaged() const {
	return isDamageableItem() && mAuxValue > 0;
}

short ItemInstance::getDamageValue() const {
	return mAuxValue;
}

short ItemInstance::getAuxValue() const {
	return mAuxValue;
}

void ItemInstance::setAuxValue(short value) {
	mAuxValue = value;
}

short ItemInstance::getMaxDamage() const {
	return mItem->getMaxDamage();
}

void ItemInstance::hurtAndBreak(int i, Entity* owner) {
// 	bool creative = owner != nullptr && owner->hasCategory(EntityCategory::Player) && ((Player*)owner)->mAbilities.mInstabuild;
// 	if (!isDamageableItem() || creative) {
// 		return;
// 	}
// 
// 	if (mUserData && mUserData->getBoolean("Unbreakable")) {
// 		return;
// 	}
// 
// 	// unbreaking enchant, has a chance to not reduce durability.  If enchant not present, returns zero
// 	int unbreaking = EnchantUtils::getEnchantLevel(Enchant::Type::MINING_DURABILITY, *this);
// 	int chance = 100;	// durability reduction chance
// 
// 	// armor has a 20%/27%/30% to ignore durability loss, other items have a 100 / (Level + 1) chance to ignore
// 	chance = mItem->getDamageChance(unbreaking);
// 
// 	// roll to determine if durability is actually lost
// 	if (unbreaking <= 0 || EnchantUtils::mSharedRandom.nextInt(100) < chance) {
// 		mAuxValue += i;
// 
// 		if (mAuxValue > getMaxDamage()) {
// 			remove(1);
// 
// 			if (owner != nullptr) {
// 				owner->getLevel().playSound(LevelSoundEvent::Break, owner->mPos);
// 			}
// 		}
// 	}
}

void ItemInstance::hurtEnemy(Mob* mob, Mob* attacker) {
// 	if (attacker->getHealth() > 0) {
// 		mItem->hurtEnemy(this, mob, attacker);
// 	}
}

void ItemInstance::mineBlock(BlockID block, int x, int y, int z, Mob* owner) {
	mItem->mineBlock(this, block, x, y, z, owner);
}

int ItemInstance::getAttackDamage() {
	return mItem->getAttackDamage();
}

bool ItemInstance::canDestroySpecial(const Block* block) {
	return mItem->canDestroySpecial(block);
}

void ItemInstance::snap(Player* player) {
}

bool ItemInstance::interactEnemy(Mob* mob, Player* player) {
	return mItem->interactEnemy(this, mob, player);
}

/*static*/
bool ItemInstance::matches(const ItemInstance* a, const ItemInstance* b) {
	if (a == nullptr && b == nullptr) {
		return true;
	}
	if (a == nullptr || b == nullptr) {
		return false;
	}
	return a->matches(b);
}

/*static*/
bool ItemInstance::matchesNulls(const ItemInstance* a, const ItemInstance* b) {
	bool aNull = !a || a->isNull();
	bool bNull = !b || b->isNull();
	if (aNull && bNull) {
		return true;
	}
	if (aNull || bNull) {
		return false;
	}
	return a->matches(b);
}

/**
 * Checks if this item is the same item as the other one, disregarding the
 * 'count' value.
 *
 * @param b
 * @return
 */
bool ItemInstance::sameItemAndAux(const ItemInstance* b) const {
	return sameItem(b) && mAuxValue == b->mAuxValue;
}

ItemInstance* ItemInstance::setDescriptionId(const std::string& id) {
	return this;
}

std::string ItemInstance::getName() const {
	return (hasCustomHoverName()) ? getCustomName() : mItem->buildDescriptionName(*this);
}

std::string ItemInstance::getRawNameId() const {
	std::string result;
	// Blocks are items and blocks, but the item raw name is tile.blockname where block is just blockname, so use block first
	if (mBlock) {
		result = mBlock->getRawNameId();
	}
	else if (mItem) {
		result = mItem->getRawNameId();
	}
	return result;
}

std::string ItemInstance::getEffectName() const {
	return mItem->buildEffectDescriptionName(*this);
}

std::string ItemInstance::getFormattedHovertext(Level& level, const bool advancedToolTops) const {
	std::string hoverText;
	if (isItem() && isValid()) {
		mItem->appendFormattedHovertext(*this, level, hoverText, advancedToolTops);
	}
	return hoverText;
}

int ItemInstance::getId() const {
	if (!mValid) {
		return -1;
	}
	else {
		return mItem ? mItem->getId() : (mBlock ? mBlock->mID : 0);
	}
}

int ItemInstance::getIdAux() const {
	// TODO breaks when types are changed
	DEBUG_ASSERT(mItem, "getting item ID without item.");

	return (mItem->getId() << 16) | (mAuxValue);
}

int ItemInstance::getIdAuxEnchanted() const {
	return getIdAux() | (isEnchanted() ? 1 << 15 : 0);
}

std::string ItemInstance::toString() const {
	std::stringstream ss;
	ss << mCount << " x " << getName() << "(" << mItem->getId() << ")" << "@" << mAuxValue;
	return ss.str();
}

/*static*/
ItemInstance* ItemInstance::clone(const ItemInstance* item) {
	return (item == nullptr || item->isNull()) ? nullptr : new ItemInstance(*item);
}

/*static*/
ItemInstance ItemInstance::cloneSafe(const ItemInstance* item) {
	return (item == nullptr || item->isNull()) ? ItemInstance() : *item;
}

/*private*/
bool ItemInstance::matches(const ItemInstance* b) const {
	return (mItem == b->mItem)
		&& (mCount == b->mCount)
		&& (mAuxValue == b->mAuxValue)
		&& hasSameUserData(*b);
}

ItemInstance& ItemInstance::operator=(const ItemInstance& rhs) {
	mCount = rhs.mCount;
	mAuxValue = rhs.mAuxValue;
	mItem = rhs.mItem;
	mBlock = rhs.mBlock;
	mValid = rhs.mValid;

	mUserData = rhs.mUserData ? rhs.mUserData->clone() : nullptr;

	return *this;
}

Color ItemInstance::getColor() const {
	return getItem()->getColor(*this);
}

Unique<CompoundTag> ItemInstance::save() {
	auto compoundTag = make_unique<CompoundTag>();
	compoundTag->putShort("id", (short)getId());
	compoundTag->putByte("Count", (byte)mCount);
	compoundTag->putShort("Damage", (short)mAuxValue);

	if (mUserData) {
		compoundTag->putCompound("tag", mUserData->clone());
	}

	return compoundTag;
}

void ItemInstance::load(const CompoundTag& compoundTag) {
	// item id - item aux
	IdPair idpair(compoundTag.getShort("id"), compoundTag.getShort("Damage"));

	// promote old item id/aux pairs to new ids (if needed).
	PromoteIdPair(idpair);

	_setItem(idpair.id);
	mAuxValue = idpair.aux;

	mCount = std::max(compoundTag.getByte("Count"), (byte)0);
	mUserData = compoundTag.contains("tag", Tag::Type::Compound) ? compoundTag.getCompound("tag")->clone() : nullptr;
}

bool ItemInstance::operator!=(const ItemInstance& rhs) const {
	return !matches(&rhs);
}

bool ItemInstance::operator==(const ItemInstance& rhs) const {
	return matches(&rhs);
}

ItemInstance* ItemInstance::fromTag(const CompoundTag& tag) {
	ItemInstance* ii = new ItemInstance();
	ii->load(tag);
	if (ii->mItem == nullptr) {
		delete ii;
		ii = nullptr;
	}
	return ii;
}

const TextureUVCoordinateSet& ItemInstance::getIcon(int frame, bool inInventoryPane) const {
	return mItem->getIcon(mAuxValue, frame, inInventoryPane);
}

void ItemInstance::releaseUsing(Player* player, int durationLeft) {
	mItem->releaseUsing(this, player, durationLeft);
}

int ItemInstance::getMaxUseDuration() const {
	return mItem->getMaxUseDuration();
}

UseAnimation ItemInstance::getUseAnimation() const {
	return mItem->getUseAnimation();
}

void ItemInstance::useTimeDepleted(Level* level, Player* player) {
	return mItem->useTimeDepleted(this, level, player);
}

bool ItemInstance::isArmorItem(const ItemInstance* instance) {
	if (!instance) {
		return false;
	}

	const Item* item = instance->getItem();
	return item && item->isArmor();
}

bool ItemInstance::isHorseArmorItem(const ItemInstance* instance) {
	if (!instance) {
		return false;
	}

	const Item* item = instance->getItem();
	return item && (item == Item::mLeatherHorseArmor || item == Item::mIronHorseArmor || item == Item::mGoldHorseArmor || item == Item::mDiamondHorseArmor);;
}

bool ItemInstance::isWearableItem(const ItemInstance* instance) {
	if (!instance) {
		return false;
	}

	return isArmorItem(instance) ||
		(instance->getBlock() == Block::mPumpkin) ||
		(instance->getItem() == Item::mSkull);
}

bool ItemInstance::isPotionItem(const ItemInstance* instance) {
	if (!instance) {
		return false;
	}

// 	int id = instance->getId();
// 	if (id == Item::mPotion->getId() || id == Item::mSplash_potion->getId() || id == Item::mLingering_potion->getId()) {
// 		auto potion = static_cast<PotionItem*>(instance->getItem());
// 		return potion->isValidAuxValue(instance->getAuxValue());
// 	}

	return false;
}

bool ItemInstance::isItem(const ItemInstance* instance) {
	return instance && instance->getItem();
}

bool ItemInstance::isLiquidClipItem() {
	return mItem && mItem->isLiquidClipItem(mAuxValue);
}

std::string ItemInstance::getCustomName() const {
	std::string name;

	if (mUserData && mUserData->contains(TAG_DISPLAY, Tag::Type::Compound)) {
		const CompoundTag& display = *mUserData->getCompound(TAG_DISPLAY);
		if (display.contains(TAG_DISPLAY_NAME)) {
			name = display.getString(TAG_DISPLAY_NAME);
		}
	}
	return name;
}

std::string ItemInstance::_getHoverFormattingPrefix() const {
	std::string hoverName = hasCustomHoverName() ? ColorFormat::ITALIC : "";

	if (isEnchantingBook()) {
		hoverName += ColorFormat::YELLOW;
	}
	else if (isEnchanted()) {
		hoverName += ColorFormat::AQUA;
	}

	return hoverName;
}

std::string ItemInstance::getHoverName() const {
	return _getHoverFormattingPrefix() + getName() + ColorFormat::RESET;
}

ItemInstance* ItemInstance::setCustomName(const std::string& name) {
	if (!mUserData) {
		mUserData = make_unique<CompoundTag>("tag");
	}

	if (!mUserData->contains(TAG_DISPLAY, Tag::Type::Compound)) {
		mUserData->putCompound(TAG_DISPLAY, make_unique<CompoundTag>());
	}

	Unique<CompoundTag> displayTag = mUserData->getCompound(TAG_DISPLAY)->clone();
	displayTag->putString(TAG_DISPLAY_NAME, name);
	mUserData->putCompound(TAG_DISPLAY, std::move(displayTag));
	return this;
}

void ItemInstance::resetHoverName() {
	if (mUserData == nullptr || !mUserData->contains(TAG_DISPLAY)) {
		return;
	}

	if (mUserData->contains(TAG_DISPLAY, Tag::Type::Compound)) {
		Unique<CompoundTag> displayTag = mUserData->getCompound(TAG_DISPLAY)->clone();
		displayTag->remove(TAG_DISPLAY_NAME);

		if (displayTag->isEmpty()) {
			mUserData->remove(TAG_DISPLAY);
		} else {
			mUserData->putCompound(TAG_DISPLAY, std::move(displayTag));
		}
	}
}

bool ItemInstance::hasCustomHoverName() const {
	if (!mUserData || !mUserData->contains(TAG_DISPLAY)) {
		return false;
	}

	auto tag = mUserData->getCompound(TAG_DISPLAY);
	return tag && tag->contains(TAG_DISPLAY_NAME);
}

int ItemInstance::getBaseRepairCost() const {
	if (mUserData && mUserData->contains(TAG_REPAIR_COST)) {
		return mUserData->getInt(TAG_REPAIR_COST);
	}

	return 0;
}

void ItemInstance::setRepairCost(int cost) {
	if (!mUserData) {
		mUserData = make_unique<CompoundTag>("tag");
	}

	mUserData->putInt(TAG_REPAIR_COST, cost);
}
