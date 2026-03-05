/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/

#include "Dungeons.h"

#include "world/item/Item.h"

#include "world/item/ItemCategory.h"
#include "world/level/block/Block.h"
// #include "world/item/ItemInstance.h"
#include "util/StringUtils.h"
// #include "world/item/MinecartItem.h"
// #include "world/item/SkullItem.h"
// #include "world/entity/item/Minecart.h"
// #include "world/entity/item/MinecartChest.h"
#include "world/item/ItemCategory.h"
#include "world/item/ItemInstance.h"
#include "world/level/block/Block.h"
#include "world/item/Item.h"
#include "world/level/block/WoodBlock.h"
#include "world/level/block/WallBlock.h"
#include "world/level/block/StoneSlabBlock.h"
#include "world/level/block/QuartzBlockBlock.h"
#include "world/level/block/PrismarineBlock.h"
#include "world/level/block/SeaLanternBlock.h"
#include "world/level/block/TallGrass.h"
// #include "world/item/DyePowderItem.h"
#include "world/level/block/OldLogBlock.h"
#include "world/level/block/NewLogBlock.h"
#include "world/level/block/DirtBlock.h"
#include "world/level/block/SandBlock.h"
#include "world/level/block/RepeaterBlock.h"
#include "world/level/block/SpongeBlock.h"
// #include "world/entity/player/PlayerInventoryProxy.h"
#include "world/entity/EntityClassTree.h"
#include "client/renderer/texture/TextureAtlas.h"
// #include "world/item/BoatItem.h"
// #include "world/effect/MobEffect.h"
// #include "world/level/block/entity/SkullBlockEntity.h"
// #include "world/item/enchanting/Enchant.h"
#include "nbt/NbtIo.h"
#include "util/StringByteOutput.h"
#include "util/StringByteInput.h"
// #include "world/item/EnchantedBookItem.h"
// #include "world/item/ExperiencePotionItem.h"
// #include "world/item/FireChargeItem.h"
#include "world/level/block/AnvilBlock.h"
// #include "world/item/alchemy/Potion.h"
// #include "world/item/CarrotOnAStickItem.h"
#include "util/ColorFormat.h"
#include "Core/Utility/EnumCast.h"
#include "util/DataIO.h"
#include "Core/Resource/ResourceHelper.h"
// #include "resources/ResourcePackManager.h"

// #include "Core/Math/MathUtility.h"

#include "world/item/Item.h"
// #include "world/item/BedItem.h"
// #include "world/item/CoalItem.h"
// #include "world/item/DoorItem.h"
// #include "world/item/EggItem.h"
// #include "world/item/ArrowItem.h"
// #include "world/item/FlintAndSteelItem.h"
// #include "world/item/SnowballItem.h"
// #include "world/item/BlockPlanterItem.h"
// 
// #include "world/item/AuxDataBlockItem.h"
// #include "world/item/ClothBlockItem.h"
// #include "world/item/DyePowderItem.h"
// #include "world/item/LeafBlockItem.h"
// #include "world/item/StoneSlabBlockItem.h"
// 
// #include "world/item/ArmorItem.h"
// #include "world/item/BowItem.h"
// #include "world/item/FishingRodItem.h"
// #include "world/item/DiggerItem.h"
// #include "world/item/HatchetItem.h"
// #include "world/item/HoeItem.h"
// #include "world/item/HorseArmorItem.h"
// #include "world/item/PickaxeItem.h"
// #include "world/item/ShovelItem.h"
// #include "world/item/ShearsItem.h"
// #include "world/item/WeaponItem.h"
// 
// #include "world/item/MobPlacerItem.h"
// 
// #include "world/item/HangingEntityItem.h"
// #include "world/item/SignItem.h"
// #include "world/item/BucketItem.h"
// 
// #include "world/item/ClockItem.h"
// #include "world/item/CompassItem.h"
// 
// #include "world/item/BottleItem.h"
// #include "world/item/PotionItem.h"
// #include "world/item/SplashPotionItem.h"
// #include "world/item/LingeringPotionItem.h"
// #include "world/item/WaterlilyBlockItem.h"
// #include "world/item/SaplingBlockItem.h"
// #include "world/level/block/WoodSlabBlock.h"
// #include "world/item/TopSnowBlockItem.h"
// #include "world/item/RedStoneDustItem.h"
// #include "world/item/EnderpearlItem.h"
// #include "world/item/EnderEyeItem.h"
// #include "world/item/EndCrystalItem.h"
// #include "world/item/PortfolioBookItem.h"
// 
// #include "world/item/FoodItemComponent.h"
// #include "world/item/SeedItemComponent.h"
// #include "world/item/CameraItemComponent.h"
// #include "platform/AppPlatform.h"
// #include "world/item/MapItem.h"
// #include "world/item/EmptyMapItem.h"
// #include "world/item/ChorusFruit.h"
// #include "world/item/LeadItem.h"
// #include "world/item/ChalkboardItem.h"
// #include "world/level/block/entity/ChalkboardBlockEntity.h"
// #include "world/level/block/MonsterEggBlock.h"
// 
// #include "world/entity/components/BoostableComponent.h"

#include "util/JSONUtils.h"

const std::string Item::ICON_DESCRIPTION_PREFIX("item.");
Random Item::mRandom;

std::shared_ptr<TextureAtlas> Item::mItemTextureAtlas;
std::unordered_map<std::string, Unique<Item>> Item::mItemLookupMap;
Item* Item::mItems[MAX_ITEMS] = { nullptr };

Item::Tier const Item::Tier::WOOD(0, 59, 2, 0, 15);
Item::Tier const Item::Tier::STONE(1, 131, 4, 1, 5);
Item::Tier const Item::Tier::IRON(2, 250, 6, 2, 14);
Item::Tier const Item::Tier::DIAMOND(3, 1561, 8, 3, 10);
Item::Tier const Item::Tier::GOLD(0, 32, 12, 0, 22);

std::vector<ItemInstance> Item::mCreativeList;

ItemInstance Item::Tier::getTierItem(void) const {
	switch (getLevel()) {
	case 0:
		if (getSpeed() == 2) {
			return ItemInstance(Block::mWoodPlanks);
		}
		else {
			return ItemInstance(Item::mGoldIngot);
		}

	case 1:
		return ItemInstance(Block::mStone);
	case 2:
		return ItemInstance(Item::mIronIngot);
	case 3:
		return ItemInstance(Item::mDiamond);
	default:
		return ItemInstance();
	}
}

// Static Item Definitions
Item* Item::mShovel_iron = nullptr;
Item* Item::mPickAxe_iron = nullptr;
Item* Item::mHatchet_iron = nullptr;
Item* Item::mFlintAndSteel = nullptr;
Item* Item::mApple = nullptr;
Item* Item::mBow = nullptr;
Item* Item::mArrow = nullptr;
Item* Item::mCoal = nullptr;
Item* Item::mDiamond = nullptr;
Item* Item::mIronIngot = nullptr;
Item* Item::mGoldIngot = nullptr;
Item* Item::mSword_iron = nullptr;

Item* Item::mSword_wood = nullptr;
Item* Item::mShovel_wood = nullptr;
Item* Item::mPickAxe_wood = nullptr;
Item* Item::mHatchet_wood = nullptr;

Item* Item::mSword_stone = nullptr;
Item* Item::mShovel_stone = nullptr;
Item* Item::mPickAxe_stone = nullptr;
Item* Item::mHatchet_stone = nullptr;

Item* Item::mSword_diamond = nullptr;
Item* Item::mShovel_diamond = nullptr;
Item* Item::mPickAxe_diamond = nullptr;
Item* Item::mHatchet_diamond = nullptr;

Item* Item::mStick = nullptr;
Item* Item::mBowl = nullptr;
Item* Item::mMushroomStew = nullptr;

Item* Item::mSword_gold = nullptr;
Item* Item::mShovel_gold = nullptr;
Item* Item::mPickAxe_gold = nullptr;
Item* Item::mHatchet_gold = nullptr;

Item* Item::mString = nullptr;
Item* Item::mFeather = nullptr;
Item* Item::mSulphur = nullptr;

Item* Item::mHoe_wood = nullptr;
Item* Item::mHoe_stone = nullptr;
Item* Item::mHoe_iron = nullptr;
Item* Item::mHoe_diamond = nullptr;
Item* Item::mHoe_gold = nullptr;

Item* Item::mSeeds_wheat = nullptr;
Item* Item::mWheat = nullptr;
Item* Item::mBread = nullptr;

Item* Item::mHelmet_cloth = nullptr;
Item* Item::mChestplate_cloth = nullptr;
Item* Item::mLeggings_cloth = nullptr;
Item* Item::mBoots_cloth = nullptr;

Item* Item::mElytra = nullptr;

Item* Item::mHelmet_chain = nullptr;
Item* Item::mChestplate_chain = nullptr;
Item* Item::mLeggings_chain = nullptr;
Item* Item::mBoots_chain = nullptr;

Item* Item::mHelmet_iron = nullptr;
Item* Item::mChestplate_iron = nullptr;
Item* Item::mLeggings_iron = nullptr;
Item* Item::mBoots_iron = nullptr;

Item* Item::mHelmet_diamond = nullptr;
Item* Item::mChestplate_diamond = nullptr;
Item* Item::mLeggings_diamond = nullptr;
Item* Item::mBoots_diamond = nullptr;

Item* Item::mHelmet_gold = nullptr;
Item* Item::mChestplate_gold = nullptr;
Item* Item::mLeggings_gold = nullptr;
Item* Item::mBoots_gold = nullptr;

Item* Item::mFlint = nullptr;
Item* Item::mPorkChop_raw = nullptr;
Item* Item::mPorkChop_cooked = nullptr;
Item* Item::mPainting = nullptr;

Item* Item::mChorusFruit;
Item* Item::mChorusFruitPopped;

Item* Item::mApple_gold = nullptr;
Item* Item::mApple_enchanted = nullptr;

Item* Item::mSign = nullptr;
Item* Item::mDoor_wood = nullptr;

Item* Item::mBucket = nullptr;
//Item* Item::bucket_empty = nullptr;
//Item* Item::bucket_water = nullptr;
//Item* Item::bucket_lava = nullptr;

Item* Item::mMinecart = nullptr;
Item* Item::mSaddle = nullptr;
Item* Item::mDoor_iron = nullptr;
Item* Item::mRedStone = nullptr;
Item* Item::mSnowBall = nullptr;

Item* Item::mBoat = nullptr;

Item* Item::mMobPlacer = nullptr;
Item* Item::mExperiencePotionItem = nullptr;
Item* Item::mFireCharge = nullptr;

Item* Item::mPumpkinPie = nullptr;

Item* Item::mLeatherHorseArmor = nullptr;
Item* Item::mIronHorseArmor = nullptr;
Item* Item::mGoldHorseArmor = nullptr;
Item* Item::mDiamondHorseArmor = nullptr;
Item* Item::mLead = nullptr;

Item* Item::mLeather = nullptr;
Item* Item::mRabbitHide = nullptr;
//Item* Item::milk = nullptr;
Item* Item::mBrick = nullptr;
Item* Item::mClay = nullptr;
Item* Item::mReeds = nullptr;
Item* Item::mPaper = nullptr;
Item* Item::mBook = nullptr;
Item* Item::mSlimeBall = nullptr;
Item* Item::mChestMinecart = nullptr;
//Item* Item::minecart_furnace = nullptr;
Item* Item::mEgg = nullptr;
Item* Item::mCompass = nullptr;
Item* Item::mFishingRod = nullptr;
Item* Item::mClock = nullptr;
Item* Item::mYellowDust = nullptr;
Item* Item::mCarrotOnAStick = nullptr;

Item* Item::mFish_raw_cod = nullptr;			
Item* Item::mFish_raw_salmon = nullptr;			
Item* Item::mFish_raw_clownfish = nullptr;		
Item* Item::mFish_raw_pufferfish = nullptr;		

Item* Item::mFish_cooked_cod = nullptr;			
Item* Item::mFish_cooked_salmon = nullptr;		

Item* Item::mMelon = nullptr;

Item* Item::mSpeckled_melon = nullptr;

Item* Item::mSeeds_pumpkin = nullptr;
Item* Item::mSeeds_melon = nullptr;

Item* Item::mDye_powder = nullptr;
Item* Item::mBone = nullptr;
Item* Item::mSugar = nullptr;
Item* Item::mCake = nullptr;

Item* Item::mBed = nullptr;
Item* Item::mRepeater = nullptr;
Item* Item::mCookie = nullptr;

Item* Item::mPotato = nullptr;
Item* Item::mPotatoBaked = nullptr;
Item* Item::mCarrot = nullptr;
Item* Item::mGoldenCarrot = nullptr;
Item* Item::mBeetroot = nullptr;
Item* Item::mSeeds_beetroot = nullptr;
Item* Item::mBeetrootSoup = nullptr;

Item* Item::mSpider_eye = nullptr;
Item* Item::mFermented_spider_eye = nullptr;
Item* Item::mPoisonous_potato = nullptr;

Item* Item::mSkull = nullptr;

Item* Item::mEnderpearl = nullptr;
Item* Item::mEnderEye = nullptr;
Item* Item::mEndCrystal = nullptr;
Item* Item::mBlazeRod = nullptr;
Item* Item::mNether_wart = nullptr;
Item* Item::mGold_nugget = nullptr;
Item* Item::mBlazePowder = nullptr;

//Item* Item::repeater = nullptr;
Item* Item::mNetherStar = nullptr;

Item* Item::mShears = nullptr;
Item* Item::mBeef_raw = nullptr;
Item* Item::mBeef_cooked = nullptr;
Item* Item::mChicken_raw = nullptr;
Item* Item::mChicken_cooked = nullptr;
Item* Item::mMutton_raw = nullptr;
Item* Item::mMutton_cooked = nullptr;
Item* Item::mRotten_flesh = nullptr;
Item* Item::mRabbitRaw = nullptr;
Item* Item::mRabbitCooked = nullptr;
Item* Item::mRabbitStew = nullptr;
Item* Item::mRabbitFoot = nullptr;

Item* Item::mNameTag = nullptr;

Item* Item::mDoor_spruce = nullptr;
Item* Item::mDoor_birch = nullptr;
Item* Item::mDoor_jungle = nullptr;
Item* Item::mDoor_acacia = nullptr;
Item* Item::mDoor_darkoak = nullptr; 

Item* Item::mGhast_tear = nullptr;

Item* Item::mMagma_cream = nullptr;
Item* Item::mBrewing_stand = nullptr;
Item* Item::mCauldron = nullptr;

Item* Item::mEnchanted_book = nullptr;
Item* Item::mComparator = nullptr;
Item* Item::mNetherbrick = nullptr;
Item* Item::mNetherQuartz = nullptr;
Item* Item::mTNTMinecart = nullptr;

Item* Item::mPrismarineShard = nullptr;
Item* Item::mPrismarineCrystals = nullptr;

//Item* Item::record_01 = nullptr;
//Item* Item::record_02 = nullptr;
Item* Item::mEmerald = nullptr;

Item* Item::mItemFrame = nullptr;

Item* Item::mFlowerPot = nullptr;

Item* Item::mPotion = nullptr;
Item* Item::mSplash_potion = nullptr;
Item* Item::mLingering_potion = nullptr;
Item* Item::mGlass_bottle = nullptr;
Item* Item::mDragon_breath = nullptr;

Item* Item::mHopperMinecart = nullptr;
Item* Item::mHopper = nullptr;

Item* Item::mFilledMap = nullptr;
Item* Item::mEmptyMap = nullptr;

Item* Item::mPortfolioBook = nullptr;
Item* Item::mChalkboard = nullptr;
Item* Item::mCamera = nullptr;

Item* Item::mShulkerShell = nullptr;

void Item::setTextureAtlas(std::shared_ptr<TextureAtlas> itemsTextureAtlas) {
	mItemTextureAtlas = itemsTextureAtlas;
}

template<class T, class ... Args>
T& registerItem(Args&& ... args) {
	auto item = make_unique<T>(std::forward<Args>(args) ...);

	short actualId = item->getId();
	std::string descId = Util::toLower(item->getRawNameId());

	DEBUG_ASSERT(actualId >= 0 && actualId <= Item::MAX_ITEMS, "Invalid ID value");
	DEBUG_ASSERT(!item->getDescriptionId().empty(), "Item missing a name?! please fix");
	DEBUG_ASSERT(Item::mItems[actualId] == nullptr, "ID Already taken! Please fix!");

	// Id lookup
	Item::mItems[actualId] = item.get();
	
	// Name lookup
	Item::mItemLookupMap[descId] = std::move(item);

	return static_cast<T&>(*Item::mItems[actualId]);
}

int itemVarientFromString(const std::string& str) {
	if (str.empty()) {
		return 0;
	}
	
	// Lazy Init of static lookup map
	static std::unordered_map<std::string, int> LOOKUP_MAP;
	if (LOOKUP_MAP.empty()) {
		LOOKUP_MAP["wood"] = 0;
		LOOKUP_MAP["stone"] = 1;
		LOOKUP_MAP["iron"] = 2;
		LOOKUP_MAP["gold"] = 3;
		LOOKUP_MAP["diamond"] = 4;
	}

	// Lookup case insensitive by using *lower case* keys.
	auto i = LOOKUP_MAP.find(Util::toLower(str));
	return i != LOOKUP_MAP.end() ? i->second : 0;
}

template<class T>
std::unique_ptr<T> registerComponent(Item& item, Json::Value & data, const std::string & componentName) {

	// Is the component data there?
	Json::Value & componentData = data[componentName];
	if (componentData.isNull())
		return std::unique_ptr<T>();

	// Create component
	std::unique_ptr<T> component = make_unique<T>(item);
	if (!component->init(componentData))
		return std::unique_ptr<T>();

	return std::move(component);
}

bool Item::initServer(Json::Value & data)
{
	if (data.isObject()) {
		// Block Item?
		const Block* block = Block::lookupByName(data["block"].asString(), true);
		mBlockId = block ? block->getId() : BlockID::AIR;

		mMaxDamage = data["max_damage"].asInt(0);
		mHandEquipped = data["hand_equipped"].asBool(false);
		mIsStackedByData = data["stacked_by_data"].asBool(false);
		mIsGlint = data["foil"].asBool(false);

		mMaxUseDuration = data["use_duration"].asInt(0);
		m_maxStackSize = data["max_stack_size"].asInt(MAX_STACK_SIZE);

		// Register Components
// 		mFoodComponent = registerComponent<FoodItemComponent>(*this, data, "food");
// 		mSeedComponent = registerComponent<SeedItemComponent>(*this, data, "seed");
// 		mCameraComponent = registerComponent<CameraItemComponent>(*this, data, "camera");
	}

	return true;
}

bool Item::initClient(Json::Value & data, Json::Value & offsetsList)
{
	if (data.isObject()) {
		// Icon Texture
		std::string icon_name = data["icon"].asString("");
		int icon_varient = itemVarientFromString(data["icon_varient"].asString(""));
		if (!icon_name.empty()) {
			// note that this can be null in the event of 3d items
			mIconTexture = &mItemTextureAtlas->getTextureItem(icon_name)[icon_varient];
		}

		// Icon Atlas
		std::string iconAtlasName = data["icon_atlas"].asString("");
		if (!iconAtlasName.empty()) {
			// note that this can be null in the event of 3d items
			mIconAtlas = &mItemTextureAtlas->getTextureItem(iconAtlasName);
		}

		std::string creativeCategoryStr = data["category"].asString("");
		if (!creativeCategoryStr.empty()) {
			mCreativeCategory = CreativeItemCategoryFromString(creativeCategoryStr);
		}

		std::string useAnimStr = data["use_animation"].asString("");
// 		if (!useAnimStr.empty()) {
// 			mUseAnim = UseAnimationFromString(useAnimStr);
// 		}

		// Color Format
		std::string color_format = data["hover_text_color"].asString("");
		if (!color_format.empty()) {
			mHoverTextColorFormat = ColorFormat::FromString(color_format);
		}

		std::string renderOffset = data["render_offsets"].asString("");
		mUsesRenderingAdjustment = false;
		if (!offsetsList.isNull() && !renderOffset.empty()) {

			// look for the renderOffset in the offsets list
			Json::Value entry = offsetsList[renderOffset];
			if (entry.isNull()) {
				JsonUtil::errorMessage("Item referenced render_offset" + renderOffset + " that doesn't exist in items_offset_client.json");
			}
			else {
				// found entry, parse anything in it
				mUsesRenderingAdjustment = true;

				Json::Value posAdjEntry = entry["VR_hand_controller_position_adjust"];
				if (!posAdjEntry.isNull()) {
					JsonUtil::parseValue(posAdjEntry, mRenderingAdjTrans);
				}
				else {
					// ensure default is enforced
					mRenderingAdjTrans.set(0.0f, 0.0f, 0.0f);
				}

				Json::Value rotAdjEntry = entry["VR_hand_controller_rotation_adjust"];
				if (!rotAdjEntry.isNull()) {
					JsonUtil::parseValue(rotAdjEntry, mRenderingAdjRot);
				}
				else {
					mRenderingAdjRot.set(0.0f, 0.0f, 0.0f);
				}

				Json::Value scaleAdjEntry = entry["VR_hand_controller_scale"];
				if (!scaleAdjEntry.isNull()) {
					JsonUtil::parseValue(scaleAdjEntry, mRenderingAdjScale);
				}
				else {
					mRenderingAdjScale = 1.0f;
				}
			}
		}
	}
	
	mExplodable = true;

	return true;
}

/*static*/
void Item::registerItems() {
	// Set static pointers
	Item::mApple = &registerItem<Item>("apple", 4);
	Item::mApple_gold = &registerItem<Item>("golden_apple", 66);
	Item::mApple_enchanted = &registerItem<Item>("appleEnchanted", 210); //Parity: Separate item instead of data value 1 on golden apple
	Item::mMushroomStew = &registerItem<Item>("mushroom_stew", 26);
	Item::mBread = &registerItem<Item>("bread", 41);
	Item::mPorkChop_raw = &registerItem<Item>("porkchop", 63);
	Item::mPorkChop_cooked = &registerItem<Item>("cooked_porkchop", 64);

	Item::mFish_raw_cod = &registerItem<Item>("fish", 93); //Parity: Fish are separate items instead of augmented by data value.
	Item::mFish_raw_salmon = &registerItem<Item>("salmon", 204);
	Item::mFish_raw_clownfish = &registerItem<Item>("clownfish", 205);
	Item::mFish_raw_pufferfish = &registerItem<Item>("pufferfish", 206);
	Item::mFish_cooked_cod = &registerItem<Item>("cooked_fish", 94);
	Item::mFish_cooked_salmon = &registerItem<Item>("cooked_salmon", 207);

	Item::mCookie = &registerItem<Item>("cookie", 101);
	Item::mMelon = &registerItem<Item>("melon", 104);
	Item::mBeef_raw = &registerItem<Item>("beef", 107);
	Item::mBeef_cooked = &registerItem<Item>("cooked_beef", 108);
	Item::mChicken_raw = &registerItem<Item>("chicken", 109);
	Item::mChicken_cooked = &registerItem<Item>("cooked_chicken", 110);
	Item::mMutton_raw = &registerItem<Item>("muttonRaw", 167);
	Item::mMutton_cooked = &registerItem<Item>("muttonCooked", 168);
	Item::mRotten_flesh = &registerItem<Item>("rotten_flesh", 111);
	Item::mSpider_eye = &registerItem<Item>("spider_eye", 119);
	Item::mCarrot = &registerItem<Item>("carrot", 135);
	Item::mPotato = &registerItem<Item>("potato", 136);
	Item::mPotatoBaked = &registerItem<Item>("baked_potato", 137);
	Item::mPoisonous_potato = &registerItem<Item>("poisonous_potato", 138);
	Item::mGoldenCarrot = &registerItem<Item>("golden_carrot", 140);
	Item::mPumpkinPie = &registerItem<Item>("pumpkin_pie", 144);
	Item::mBeetroot = &registerItem<Item>("beetroot", 201);
	Item::mBeetrootSoup = &registerItem<Item>("beetroot_soup", 203);
	Item::mRabbitRaw = &registerItem<Item>("rabbit", 155);
	Item::mRabbitCooked = &registerItem<Item>("cooked_rabbit", 156);
	Item::mRabbitStew = &registerItem<Item>("rabbit_stew", 157);

	Item::mSeeds_wheat = &registerItem<Item>("wheat_seeds", 39);
	Item::mSeeds_pumpkin = &registerItem<Item>("pumpkin_seeds", 105);
	Item::mSeeds_melon = &registerItem<Item>("melon_seeds", 106);
	Item::mNether_wart = &registerItem<Item>("nether_wart", 116);
	Item::mSeeds_beetroot = &registerItem<Item>("beetroot_seeds", 202);

// 	Item::mShovel_iron = &registerItem<ShovelItem>("iron_shovel", 0, Tier::IRON);
// 	Item::mPickAxe_iron = &registerItem<PickaxeItem>("iron_pickaxe", 1, Tier::IRON);
// 	Item::mHatchet_iron = &registerItem<HatchetItem>("iron_axe", 2, Tier::IRON);
// 	Item::mFlintAndSteel = &registerItem<FlintAndSteelItem>("flint_and_steel", 3);
// 
// 	Item::mBow = &registerItem<BowItem>("bow", 5);
// 	Item::mArrow = &registerItem<ArrowItem>("arrow", 6);
// 	Item::mCoal = &registerItem<CoalItem>("coal", 7);
// 	Item::mDiamond = &registerItem<Item>("diamond", 8);
// 	Item::mIronIngot = &registerItem<Item>("iron_ingot", 9);
// 	Item::mGoldIngot = &registerItem<Item>("gold_ingot", 10);
// 	Item::mSword_iron = &registerItem<WeaponItem>("iron_sword", 11, Tier::IRON);
// 	Item::mSword_wood = &registerItem<WeaponItem>("wooden_sword", 12, Tier::WOOD);
// 	Item::mShovel_wood = &registerItem<ShovelItem>("wooden_shovel", 13, Tier::WOOD);
// 	Item::mPickAxe_wood = &registerItem<PickaxeItem>("wooden_pickaxe", 14, Tier::WOOD);
// 	Item::mHatchet_wood = &registerItem<HatchetItem>("wooden_axe", 15, Tier::WOOD);
// 	Item::mSword_stone = &registerItem<WeaponItem>("stone_sword", 16, Tier::STONE);
// 	Item::mShovel_stone = &registerItem<ShovelItem>("stone_shovel", 17, Tier::STONE);
// 	Item::mPickAxe_stone = &registerItem<PickaxeItem>("stone_pickaxe", 18, Tier::STONE);
// 	Item::mHatchet_stone = &registerItem<HatchetItem>("stone_axe", 19, Tier::STONE);
// 	Item::mSword_diamond = &registerItem<WeaponItem>("diamond_sword", 20, Tier::DIAMOND);
// 	Item::mShovel_diamond = &registerItem<ShovelItem>("diamond_shovel", 21, Tier::DIAMOND);
// 	Item::mPickAxe_diamond = &registerItem<PickaxeItem>("diamond_pickaxe", 22, Tier::DIAMOND);
// 	Item::mHatchet_diamond = &registerItem<HatchetItem>("diamond_axe", 23, Tier::DIAMOND);
// 	Item::mStick = &registerItem<Item>("stick", 24).setHandEquipped();
// 	Item::mBowl = &registerItem<Item>("bowl", 25);
// 
// 	Item::mSword_gold = &registerItem<WeaponItem>("golden_sword", 27, Tier::GOLD);
// 	Item::mShovel_gold = &registerItem<ShovelItem>("golden_shovel", 28, Tier::GOLD);
// 	Item::mPickAxe_gold = &registerItem<PickaxeItem>("golden_pickaxe", 29, Tier::GOLD);
// 	Item::mHatchet_gold = &registerItem<HatchetItem>("golden_axe", 30, Tier::GOLD);
// 	Item::mString = &registerItem<BlockPlanterItem>("string", 31, Block::mTripwire);
// 	Item::mFeather = &registerItem<Item>("feather", 32);
// 	Item::mSulphur = &registerItem<Item>("gunpowder", 33);
// 	Item::mHoe_wood = &registerItem<HoeItem>("wooden_hoe", 34, Tier::WOOD);
// 	Item::mHoe_stone = &registerItem<HoeItem>("stone_hoe", 35, Tier::STONE);
// 	Item::mHoe_iron = &registerItem<HoeItem>("iron_hoe", 36, Tier::IRON);
// 	Item::mHoe_diamond = &registerItem<HoeItem>("diamond_hoe", 37, Tier::DIAMOND);
// 	Item::mHoe_gold = &registerItem<HoeItem>("golden_hoe", 38, Tier::GOLD);
// 	Item::mWheat = &registerItem<Item>("wheat", 40);
// 
// 	Item::mHelmet_cloth = &registerItem<ArmorItem>("leather_helmet", 42, ArmorItem::CLOTH, 0, ArmorSlot::Head);
// 	Item::mChestplate_cloth = &registerItem<ArmorItem>("leather_chestplate", 43, ArmorItem::CLOTH, 0, ArmorSlot::Torso);
// 	Item::mLeggings_cloth = &registerItem<ArmorItem>("leather_leggings", 44, ArmorItem::CLOTH, 0, ArmorSlot::Legs);
// 	Item::mBoots_cloth = &registerItem<ArmorItem>("leather_boots", 45, ArmorItem::CLOTH, 0, ArmorSlot::Feet);
// 
// 	Item::mHelmet_chain = &registerItem<ArmorItem>("chainmail_helmet", 46, ArmorItem::CHAIN, 1, ArmorSlot::Head);
// 	Item::mChestplate_chain = &registerItem<ArmorItem>("chainmail_chestplate", 47, ArmorItem::CHAIN, 1, ArmorSlot::Torso);
// 	Item::mLeggings_chain = &registerItem<ArmorItem>("chainmail_leggings", 48, ArmorItem::CHAIN, 1, ArmorSlot::Legs);
// 	Item::mBoots_chain = &registerItem<ArmorItem>("chainmail_boots", 49, ArmorItem::CHAIN, 1, ArmorSlot::Feet);
// 
// 	Item::mHelmet_iron = &registerItem<ArmorItem>("iron_helmet", 50, ArmorItem::IRON, 2, ArmorSlot::Head);
// 	Item::mChestplate_iron = &registerItem<ArmorItem>("iron_chestplate", 51, ArmorItem::IRON, 2, ArmorSlot::Torso);
// 	Item::mLeggings_iron = &registerItem<ArmorItem>("iron_leggings", 52, ArmorItem::IRON, 2, ArmorSlot::Legs);
// 	Item::mBoots_iron = &registerItem<ArmorItem>("iron_boots", 53, ArmorItem::IRON, 2, ArmorSlot::Feet);
// 
// 	Item::mHelmet_diamond = &registerItem<ArmorItem>("diamond_helmet", 54, ArmorItem::DIAMOND, 3, ArmorSlot::Head);
// 	Item::mChestplate_diamond = &registerItem<ArmorItem>("diamond_chestplate", 55, ArmorItem::DIAMOND, 3, ArmorSlot::Torso);
// 	Item::mLeggings_diamond = &registerItem<ArmorItem>("diamond_leggings", 56, ArmorItem::DIAMOND, 3, ArmorSlot::Legs);
// 	Item::mBoots_diamond = &registerItem<ArmorItem>("diamond_boots", 57, ArmorItem::DIAMOND, 3, ArmorSlot::Feet);
// 
// 	Item::mHelmet_gold = &registerItem<ArmorItem>("golden_helmet", 58, ArmorItem::GOLD, 4, ArmorSlot::Head);
// 	Item::mChestplate_gold = &registerItem<ArmorItem>("golden_chestplate", 59, ArmorItem::GOLD, 4, ArmorSlot::Torso);
// 	Item::mLeggings_gold = &registerItem<ArmorItem>("golden_leggings", 60, ArmorItem::GOLD, 4, ArmorSlot::Legs);
// 	Item::mBoots_gold = &registerItem<ArmorItem>("golden_boots", 61, ArmorItem::GOLD, 4, ArmorSlot::Feet);
// 
// 	Item::mFlint = &registerItem<Item>("flint", 62);
// 	Item::mPainting = &registerItem<HangingEntityItem>("painting", 65, EntityType::Painting);
// 
// 	Item::mSign = &registerItem<SignItem>("sign", 67);
// 	Item::mDoor_wood = &registerItem<DoorItem>("wooden_door", 68, Material::getMaterial(MaterialType::Wood), DoorBlock::OAK);
// 	Item::mBucket = &registerItem<BucketItem>("bucket", 69); //Parity: Java has different buckets. Getting lava_bucket by name, for instance, will be an issue.
// 
// 	Item::mMinecart = &registerItem<MinecartItem>("minecart", 72, MinecartType::Rideable);
// 
// 	Item::mSaddle = &registerItem<Item>("saddle", 73).setMaxStackSize(1);;
// 	Item::mDoor_iron = &registerItem<DoorItem>("iron_door", 74, Material::getMaterial(MaterialType::Metal), DoorBlock::IRON);
// 	Item::mRedStone = &registerItem<RedStoneDustItem>("redstone", 75);
// 	Item::mSnowBall = &registerItem<SnowballItem>("snowball", 76);
// 
// 	Item::mBoat = &registerItem<BoatItem>("boat", 77);
// 
// 	Item::mLeather = &registerItem<Item>("leather", 78);
// 	//Item::milk = &registerItem<BucketItem>( 79, -1).setIcon(13,
// 	// 4).setCategory(ItemCategory::FoodArmor).setDescriptionId("milk").setCraftingRemainingItem>( Item.bucket_empty);
// 	Item::mBrick = &registerItem<Item>("brick", 80);
// 	Item::mClay = &registerItem<Item>("clay_ball", 81);
// 	Item::mReeds = &registerItem<BlockPlanterItem>("reeds", 82, Block::mReeds);
// 	Item::mPaper = &registerItem<Item>("paper", 83);
// 	Item::mBook = &registerItem<Item>("book", 84);
// 	Item::mSlimeBall = &registerItem<Item>("slime_ball", 85);
// 	Item::mChestMinecart = &registerItem<MinecartItem>("chest_minecart", 86, MinecartType::Chest);
// 	//Item::mMinecart_furnace = &registerItem<MinecartItem>( 87, Minecart::FURNACE).setIcon(7,
// 	// 10).setCategory(ItemCategory::Mechanisms).setDescriptionId("minecartFurnace");
// 	Item::mEgg = &registerItem<EggItem>("egg", 88);
// 	// Compass is currently not implemented, uses a lot of textures so lets wait with that
// 	Item::mCompass = &registerItem<CompassItem>("compass", 89);
// 	Item::mFishingRod = &registerItem<FishingRodItem>("fishing_rod", 90);
// 
// 	// The clock is the same as the compass, uses a lot of textures
// 	Item::mClock = &registerItem<ClockItem>("clock", 91);
// 	Item::mYellowDust = &registerItem<Item>("glowstone_dust", 92);
// 
// 	Item::mDye_powder = &registerItem<DyePowderItem>("dye", 95);
// 	Item::mBone = &registerItem<Item>("bone", 96).setHandEquipped();
// 	Item::mSugar = &registerItem<Item>("sugar", 97);
// 	Item::mCake = &registerItem<BlockPlanterItem>("cake", 98, Block::mCake).setMaxStackSize(1);
// 	Item::mBed = &registerItem<BedItem>("bed", 99).setMaxStackSize(1);
// 	Item::mRepeater = &registerItem<BlockPlanterItem>("repeater", 100, Block::mUnpoweredRepeater);
// 	//Item::mDiode = &registerItem<BlockPlanterItem>( 100, Block::diode_off).setIcon(6,
// 	// 5).setCategory(ItemCategory::Mechanisms).setDescriptionId("repeater");
// 	Item::mFilledMap = &registerItem<MapItem>();
// 	Item::mShears = &registerItem<ShearsItem>("shears", 103);
// 
// 	Item::mEnderpearl = &registerItem<EnderpearlItem>("ender_pearl", 112);
// 	Item::mBlazeRod = &registerItem<Item>("blaze_rod", 113);
// 	Item::mGhast_tear = &registerItem<Item>("ghast_tear", 114);
// 	Item::mGold_nugget = &registerItem<Item>("gold_nugget", 115);
// 
// 	Item::mPotion = &registerItem<PotionItem>("potion", 117);
// 	Item::mGlass_bottle = &registerItem<BottleItem>("glass_bottle", 118);
// 
// 
// 	Item::mFermented_spider_eye = &registerItem<Item>("fermented_spider_eye", 120);
// 
// 	Item::mBlazePowder = &registerItem<Item>("blaze_powder", 121);
// 	Item::mMagma_cream = &registerItem<Item>("magma_cream", 122);
// 	Item::mBrewing_stand = &registerItem<BlockPlanterItem>("brewing_stand", 123, Block::mBrewingStand);
// 	Item::mCauldron = &registerItem<BlockPlanterItem>("cauldron", 124, Block::mCauldron);
// 	Item::mEnderEye = &registerItem<EnderEyeItem>("ender_eye", 125);
// 	Item::mSpeckled_melon = &registerItem<Item>("speckled_melon", 126);
// 
// 	Item::mMobPlacer = &registerItem<MonsterPlacerItem>("spawn_egg", 127);
// 	Item::mExperiencePotionItem = &registerItem<ExperiencePotionItem>("experience_bottle", 128);
// 
// 	Item::mFireCharge = &registerItem<FireChargeItem>("fireball", 129);
// 
// 	Item::mEmerald = &registerItem<Item>("emerald", 132);
// 	Item::mItemFrame = &registerItem<BlockPlanterItem>("frame", 133, Block::mItemFrame);
// 	Item::mFlowerPot = &registerItem<BlockPlanterItem>("flower_pot", 134, Block::mFlowerPot);
// 	Item::mEmptyMap = &registerItem<EmptyMapItem>();
// 	Item::mSkull = &registerItem<SkullItem>("skull", 141);
// 	Item::mCarrotOnAStick = &registerItem<CarrotOnAStickItem>("carrotOnAStick", 142);
// 	Item::mNetherStar = &registerItem<Item>("netherStar", 143);
// 	Item::mEnchanted_book = &registerItem<EnchantedBookItem>("enchanted_book", 147);
// 	Item::mComparator = &registerItem<BlockPlanterItem>("comparator", 148, Block::mUnpoweredComparator);
// 	Item::mNetherbrick = &registerItem<Item>("netherbrick", 149);
// 	Item::mNetherQuartz = &registerItem<Item>("quartz", 150);
// 	Item::mTNTMinecart = &registerItem<MinecartItem>("tnt_minecart", 151, MinecartType::TNT);
// 	Item::mHopperMinecart = &registerItem<MinecartItem>("hopper_minecart", 152, MinecartType::Hopper);
// 
// 	Item::mHopper = &registerItem<BlockPlanterItem>("hopper", 154, Block::mHopper, true);
// 
// 	Item::mRabbitFoot = &registerItem<Item>("rabbit_foot", 158);
// 	Item::mRabbitHide = &registerItem<Item>("rabbit_hide", 159);
// 
// 	Item::mLeatherHorseArmor = &registerItem<HorseArmorItem>("horsearmorleather", 160, 0, HorseArmorItem::HorseArmorTier::TIER_LEATHER).setMaxStackSize(1);
// 	Item::mIronHorseArmor = &registerItem<HorseArmorItem>("horsearmoriron", 161, 1, HorseArmorItem::HorseArmorTier::TIER_IRON).setMaxStackSize(1);
// 	Item::mGoldHorseArmor = &registerItem<HorseArmorItem>("horsearmorgold", 162, 2, HorseArmorItem::HorseArmorTier::TIER_GOLD).setMaxStackSize(1);
// 	Item::mDiamondHorseArmor = &registerItem<HorseArmorItem>("horsearmordiamond", 163, 3, HorseArmorItem::HorseArmorTier::TIER_DIAMOND).setMaxStackSize(1);
// 	//Item::mLead = &registerItem<LeadItem>("leash", 164)
// 
// 	//Item::mRecord_01 = &registerItem<RecordingItem>( 2000, "13")
// 	// 15).setCategory(ItemCategory::Decorations).setNameID("record");
// 	//Item::mRecord_02 = &registerItem<RecordingItem>( 2001, "cat")
// 	// 15).setCategory(ItemCategory::Decorations).setNameID("record");
// 	// Not implemented, using egg icon for now
// 
// 	Item::mLead = &registerItem<LeadItem>("lead", 164);
// 	Item::mNameTag = &registerItem<Item>("nameTag", 165);
// 
// 	Item::mEndCrystal = &registerItem<EndCrystalItem>("end_crystal", 170);
// 	Item::mDoor_spruce = &registerItem<DoorItem>("spruce_door", 171, Material::getMaterial(MaterialType::Wood), DoorBlock::SPRUCE);
// 	Item::mDoor_birch = &registerItem<DoorItem>("birch_door", 172, Material::getMaterial(MaterialType::Wood), DoorBlock::BIRCH);
// 	Item::mDoor_jungle = &registerItem<DoorItem>("jungle_door", 173, Material::getMaterial(MaterialType::Wood), DoorBlock::JUNGLE);
// 	Item::mDoor_acacia = &registerItem<DoorItem>("acacia_door", 174, Material::getMaterial(MaterialType::Wood), DoorBlock::ACACIA);
// 	Item::mDoor_darkoak = &registerItem<DoorItem>("dark_oak_door", 175, Material::getMaterial(MaterialType::Wood), DoorBlock::DARKOAK);
// 	Item::mChorusFruit = &registerItem<ChorusFruit>("chorus_fruit", 176);
// 	Item::mChorusFruitPopped = &registerItem<Item>("chorus_fruit_popped", 177);
// 
// 	Item::mDragon_breath = &registerItem<Item>("dragon_breath", 181);
// 	Item::mSplash_potion = &registerItem<SplashPotionItem>("splash_potion", 182);
// 	
// 	Item::mLingering_potion = &registerItem<LingeringPotionItem>("lingering_potion", 185);
// 	
// 	Item::mElytra = &registerItem<ArmorItem>("elytra", 188, ArmorItem::ELYTRA, ArmorItem::TIER_ELYTRA, ArmorSlot::Torso);

	Item::mPrismarineShard = &registerItem<Item>("prismarine_shard", 153);
	Item::mPrismarineCrystals = &registerItem<Item>("prismarine_crystals", 166);

	Item::mShulkerShell = &registerItem<Item>("shulker_shell", 189);

	// EDU Items
#ifdef MCPE_EDU
	Item::mChalkboard = &registerItem<ChalkboardItem>("board", 198).setCategory(CreativeItemCategory::Decorations);
	Item::mCamera = &registerItem<Item>("camera", 242).setIsMirroredArt(true);
	Item::mPortfolioBook = &registerItem<PortfolioBookItem>("portfolio", 200);
#endif

	initCreativeItems();
}

// void Item::initServerData(ResourcePackManager& loader) {
// 	// ------------------------------------------------------------------------------
// 	// Load Items from Json
// 	// ------------------------------------------------------------------------------
// 	Json::Value list;
// 	Json::Reader reader;
// 	std::string nameId;
// 	std::string fileContent;
// 
// 	// If we have a server file, then load it here
// 	if (loader.load(ResourceLocation("items.json", ResourceFileSystem::ServerPackage), fileContent)) {
// 		if (reader.parse(fileContent, list)) {
// 			Json::Value root = list["items"];
// 
// 			if (!root.isNull()) {
// 				for (int itemIndex = 0; itemIndex < MAX_ITEMS; ++itemIndex) {
// 					if (mItems[itemIndex]) {
// 						Item* item = mItems[itemIndex];
// 						Json::Value itemJson = root[item->getRawNameId()];
// 						if (!itemJson.isNull() && itemJson.isObject()) {
// 							item->initServer(itemJson);
// 						}
// 					}
// 				}
// 			}
// 		}
// 	}
// }

void Item::initClientData() {

// 	int wood(0), stone(1), iron(2), gold(3), diamond(4);
// 
// 	Item::mShovel_iron->setIcon("shovel", iron).setCategory(CreativeItemCategory::Tools);
// 	Item::mPickAxe_iron->setIcon("pickaxe", iron).setCategory(CreativeItemCategory::Tools);
// 	Item::mHatchet_iron->setIcon("axe", iron).setCategory(CreativeItemCategory::Tools);
// 	Item::mFlintAndSteel->setIcon("flint_and_steel").setCategory(CreativeItemCategory::Tools);
// 
// 	Item::mBow->setIcon("bow_standby").setCategory(CreativeItemCategory::Tools);
// 	Item::mArrow->setIcon("arrow").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mCoal->setIcon("coal").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mDiamond->setIcon("diamond").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mIronIngot->setIcon("iron_ingot").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mGoldIngot->setIcon("gold_ingot").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mSword_iron->setIcon("sword", iron).setCategory(CreativeItemCategory::Tools);
// 	Item::mSword_wood->setIcon("sword", wood).setCategory(CreativeItemCategory::Tools);
// 	Item::mShovel_wood->setIcon("shovel", wood).setCategory(CreativeItemCategory::Tools);
// 	Item::mPickAxe_wood->setIcon("pickaxe", wood).setCategory(CreativeItemCategory::Tools);
// 	Item::mHatchet_wood->setIcon("axe", wood).setCategory(CreativeItemCategory::Tools);
// 	Item::mSword_stone->setIcon("sword", stone).setCategory(CreativeItemCategory::Tools);
// 	Item::mShovel_stone->setIcon("shovel", stone).setCategory(CreativeItemCategory::Tools);
// 	Item::mPickAxe_stone->setIcon("pickaxe", stone).setCategory(CreativeItemCategory::Tools);
// 	Item::mHatchet_stone->setIcon("axe", stone).setCategory(CreativeItemCategory::Tools);
// 	Item::mSword_diamond->setIcon("sword", diamond).setCategory(CreativeItemCategory::Tools);
// 	Item::mShovel_diamond->setIcon("shovel", diamond).setCategory(CreativeItemCategory::Tools);
// 	Item::mPickAxe_diamond->setIcon("pickaxe", diamond).setCategory(CreativeItemCategory::Tools);
// 	Item::mHatchet_diamond->setIcon("axe", diamond).setCategory(CreativeItemCategory::Tools);
// 	Item::mStick->setIcon("stick").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mBowl->setIcon("bowl").setCategory(CreativeItemCategory::Miscellaneous);
// 
// 	Item::mSword_gold->setIcon("sword", gold).setCategory(CreativeItemCategory::Tools);
// 	Item::mShovel_gold->setIcon("shovel", gold).setCategory(CreativeItemCategory::Tools);
// 	Item::mPickAxe_gold->setIcon("pickaxe", gold).setCategory(CreativeItemCategory::Tools);
// 	Item::mHatchet_gold->setIcon("axe", gold).setCategory(CreativeItemCategory::Tools);
// 	Item::mString->setIcon("string").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mFeather->setIcon("feather").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mSulphur->setIcon("gunpowder").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mHoe_wood->setIcon("hoe", wood).setCategory(CreativeItemCategory::Tools);
// 	Item::mHoe_stone->setIcon("hoe", stone).setCategory(CreativeItemCategory::Tools);
// 	Item::mHoe_iron->setIcon("hoe", iron).setCategory(CreativeItemCategory::Tools);
// 	Item::mHoe_diamond->setIcon("hoe", diamond).setCategory(CreativeItemCategory::Tools);
// 	Item::mHoe_gold->setIcon("hoe", gold).setCategory(CreativeItemCategory::Tools);
// 	Item::mWheat->setIcon("wheat").setCategory(CreativeItemCategory::Miscellaneous);
// 
// 	int cloth(0), chain(1);
// 
// 	Item::mHelmet_cloth->setIcon("helmet", cloth).setCategory(CreativeItemCategory::Tools);
// 	Item::mChestplate_cloth->setIcon("chestplate", cloth).setCategory(CreativeItemCategory::Tools);
// 	Item::mLeggings_cloth->setIcon("leggings", cloth).setCategory(CreativeItemCategory::Tools);
// 	Item::mBoots_cloth->setIcon("boots", cloth).setCategory(CreativeItemCategory::Tools);
// 
// 	Item::mHelmet_chain->setIcon("helmet", chain).setCategory(CreativeItemCategory::Tools);
// 	Item::mChestplate_chain->setIcon("chestplate", chain).setCategory(CreativeItemCategory::Tools);
// 	Item::mLeggings_chain->setIcon("leggings", chain).setCategory(CreativeItemCategory::Tools);
// 	Item::mBoots_chain->setIcon("boots", chain).setCategory(CreativeItemCategory::Tools);
// 
// 	Item::mHelmet_iron->setIcon("helmet", iron).setCategory(CreativeItemCategory::Tools);
// 	Item::mChestplate_iron->setIcon("chestplate", iron).setCategory(CreativeItemCategory::Tools);
// 	Item::mLeggings_iron->setIcon("leggings", iron).setCategory(CreativeItemCategory::Tools);
// 	Item::mBoots_iron->setIcon("boots", iron).setCategory(CreativeItemCategory::Tools);
// 
// 	Item::mHelmet_diamond->setIcon("helmet", diamond).setCategory(CreativeItemCategory::Tools);
// 	Item::mChestplate_diamond->setIcon("chestplate", diamond).setCategory(CreativeItemCategory::Tools);
// 	Item::mLeggings_diamond->setIcon("leggings", diamond).setCategory(CreativeItemCategory::Tools);
// 	Item::mBoots_diamond->setIcon("boots", diamond).setCategory(CreativeItemCategory::Tools);
// 
// 	Item::mHelmet_gold->setIcon("helmet", gold).setCategory(CreativeItemCategory::Tools);
// 	Item::mChestplate_gold->setIcon("chestplate", gold).setCategory(CreativeItemCategory::Tools);
// 	Item::mLeggings_gold->setIcon("leggings", gold).setCategory(CreativeItemCategory::Tools);
// 	Item::mBoots_gold->setIcon("boots", gold).setCategory(CreativeItemCategory::Tools);
// 
// 	Item::mFlint->setIcon("flint").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mPainting->setIcon("painting").setCategory(CreativeItemCategory::Decorations);
// 
// 	Item::mSign->setIcon("sign").setCategory(CreativeItemCategory::Decorations);
// 	Item::mDoor_wood->setIcon("wooden_door").setCategory(CreativeItemCategory::Decorations);
// 	Item::mBucket->setIcon("bucket").setCategory(CreativeItemCategory::Tools); //Parity: Java has different buckets. Getting lava_bucket by name, for instance, will be an issue.
// 
// 	Item::mMinecart->setIcon("minecart_normal").setCategory(CreativeItemCategory::Tools);
// 
// 	Item::mSaddle->setIcon("saddle").setCategory(CreativeItemCategory::Tools);
// 	Item::mDoor_iron->setIcon("iron_door").setCategory(CreativeItemCategory::Decorations);
// 	Item::mRedStone->setIcon("redstone_dust").setCategory(CreativeItemCategory::Tools);
// 	Item::mSnowBall->setIcon("snowball").setCategory(CreativeItemCategory::Tools);
// 
// 	Item::mBoat->setIcon("boat").setCategory(CreativeItemCategory::Tools);
// 
// 	Item::mLeather->setIcon("leather").setCategory(CreativeItemCategory::Miscellaneous);
// 	//Item::milk = &registerItem<BucketItem>( 79, -1).setIcon(13,
// 	// 4).setCategory(ItemCategory::FoodArmor).setDescriptionId("milk").setCraftingRemainingItem>( Item.bucket_empty);
// 	Item::mBrick->setIcon("brick").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mClay->setIcon("clay_ball").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mReeds->setIcon("reeds").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mPaper->setIcon("paper").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mBook->setIcon("book_normal").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mSlimeBall->setIcon("slimeball").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mChestMinecart->setIcon("minecart_chest").setCategory(CreativeItemCategory::Tools);
// 	//Item::mMinecart_furnace = &registerItem<MinecartItem>( 87, Minecart::FURNACE).setIcon(7,
// 	// 10).setCategory(ItemCategory::Mechanisms).setDescriptionId("minecartFurnace");
// 	Item::mEgg->setIcon("egg").setCategory(CreativeItemCategory::Miscellaneous);
// 	// Compass is currently not implemented, uses a lot of textures so lets wait with that
// 	Item::mCompass->setIcon("compass_item").setCategory(CreativeItemCategory::Tools);
// 	Item::mFishingRod->setIcon("fishing_rod").setCategory(CreativeItemCategory::Tools);
// 
// 	// The clock is the same as the compass, uses a lot of textures
// 	Item::mClock->setIcon("clock_item").setCategory(CreativeItemCategory::Tools);
// 	Item::mYellowDust->setIcon("glowstone_dust").setCategory(CreativeItemCategory::Miscellaneous);
// 
// 	Item::mDye_powder->setIcon("dye_powder").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mBone->setIcon("bone").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mSugar->setIcon("sugar").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mCake->setIcon("cake").setMaxStackSize(1).setCategory(CreativeItemCategory::Decorations);
// 	Item::mBed->setIcon("bed").setMaxStackSize(1).setCategory(CreativeItemCategory::Decorations);
// 	Item::mRepeater->setIcon("repeater").setCategory(CreativeItemCategory::Tools);
// 	//Item::mDiode = &registerItem<BlockPlanterItem>( 100, Block::diode_off).setIcon(6,
// 	// 5).setCategory(ItemCategory::Mechanisms).setDescriptionId("repeater");
// 	Item::mFilledMap->setIcon("map_filled").setCategory(CreativeItemCategory::None);
// 	Item::mShears->setIcon("shears").setCategory(CreativeItemCategory::Tools);
// 
// 	Item::mEnderpearl->setIcon("ender_pearl").setCategory(CreativeItemCategory::Tools);
// 	Item::mBlazeRod->setIcon("blaze_rod").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mGhast_tear->setIcon("ghast_tear").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mGold_nugget->setIcon("gold_nugget").setCategory(CreativeItemCategory::Miscellaneous);
// 
// 	Item::mPotion->setIcon("potion_overlay").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mGlass_bottle->setIcon("potion_bottle_empty").setCategory(CreativeItemCategory::Miscellaneous);
// 	
// 	Item::mFermented_spider_eye->setIcon("spider_eye_fermented").setCategory(CreativeItemCategory::Miscellaneous);	//.setPotionBrewingFormula(PotionBrewing.MOD_SPIDEREYE));
// 
// 	Item::mBlazePowder->setIcon("blaze_powder").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mMagma_cream->setIcon("magma_cream").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mBrewing_stand->setIcon("brewing_stand").setCategory(CreativeItemCategory::Decorations);
// 	Item::mCauldron->setIcon("cauldron").setCategory(CreativeItemCategory::Decorations);
// 	Item::mEnderEye->setIcon("ender_eye").setCategory(CreativeItemCategory::Tools);
// 	Item::mSpeckled_melon->setIcon("melon_speckled").setCategory(CreativeItemCategory::Miscellaneous);
// 
// 	Item::mMobPlacer->setIcon("spawn_egg").setCategory(CreativeItemCategory::Tools);
// 	Item::mExperiencePotionItem->setIcon("experience_bottle").setCategory(CreativeItemCategory::Miscellaneous);
// 
// 	Item::mFireCharge->setIcon("fireball").setCategory(CreativeItemCategory::Tools);
// 
// 	Item::mEmerald->setIcon("emerald").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mItemFrame->setIcon("item_frame").setCategory(CreativeItemCategory::Decorations);
// 	Item::mFlowerPot->setIcon("flower_pot").setCategory(CreativeItemCategory::Decorations);
// 	Item::mEmptyMap->setIcon("map_empty").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mSkull->setIcon("skull_skeleton").setCategory(CreativeItemCategory::Decorations);
// 	Item::mCarrotOnAStick->setIcon("carrot_on_a_stick").setCategory(CreativeItemCategory::Miscellaneous);
// 
// 	Item::mNetherStar->setIcon("nether_star").setCategory(CreativeItemCategory::Miscellaneous).setExplodable(false).setIsGlint(true).setShouldDespawn(false);
// 	Item::mEnchanted_book->setIcon("book_enchanted").setCategory(CreativeItemCategory::Miscellaneous);
// 
// 	Item::mComparator->setIcon("comparator").setCategory(CreativeItemCategory::Tools);
// 	Item::mNetherbrick->setIcon("netherbrick").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mNetherQuartz->setIcon("quartz").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mTNTMinecart->setIcon("minecart_tnt").setCategory(CreativeItemCategory::Tools);
// 	Item::mHopperMinecart->setIcon("minecart_hopper").setCategory(CreativeItemCategory::Tools);
// 
// 	Item::mHopper->setIcon("hopper").setCategory(CreativeItemCategory::Tools);
// 
// 	Item::mRabbitFoot->setIcon("rabbit_foot").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mRabbitHide->setIcon("rabbit_hide").setCategory(CreativeItemCategory::Miscellaneous);
// 
// 	Item::mLeatherHorseArmor->setIcon("leather_horse_armor").setCategory(CreativeItemCategory::Tools);
// 	Item::mIronHorseArmor->setIcon("iron_horse_armor").setCategory(CreativeItemCategory::Tools);
// 	Item::mGoldHorseArmor->setIcon("gold_horse_armor").setCategory(CreativeItemCategory::Tools);
// 	Item::mDiamondHorseArmor->setIcon("diamond_horse_armor").setCategory(CreativeItemCategory::Tools);
// 
// 	//Item::mRecord_01 = &registerItem<RecordingItem>( 2000, "13").setIcon(0,
// 	// 15).setCategory(ItemCategory::Decorations).setNameID("record");
// 	//Item::mRecord_02 = &registerItem<RecordingItem>( 2001, "cat").setIcon(1,
// 	// 15).setCategory(ItemCategory::Decorations).setNameID("record");
// 	// Not implemented, using egg icon for now
// 
// 	Item::mLead->setIcon("lead").setCategory(CreativeItemCategory::Tools);
// 	Item::mNameTag->setIcon("name_tag").setCategory(CreativeItemCategory::Tools);
// 	Item::mEndCrystal->setIcon("end_crystal").setCategory(CreativeItemCategory::Decorations);
// 	Item::mDoor_spruce->setIcon("spruce_door").setCategory(CreativeItemCategory::Decorations);
// 	Item::mDoor_birch->setIcon("birch_door").setCategory(CreativeItemCategory::Decorations);
// 	Item::mDoor_jungle->setIcon("jungle_door").setCategory(CreativeItemCategory::Decorations);
// 	Item::mDoor_acacia->setIcon("acacia_door").setCategory(CreativeItemCategory::Decorations);
// 	Item::mDoor_darkoak->setIcon("dark_oak_door").setCategory(CreativeItemCategory::Decorations);
// 
// 	Item::mChorusFruit->setIcon("chorus_fruit").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mChorusFruitPopped->setIcon("chorus_fruit_popped").setCategory(CreativeItemCategory::Miscellaneous);
// 
// 	Item::mDragon_breath->setIcon("dragon_breath").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mSplash_potion->setIcon("potion_bottle_splash").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mLingering_potion->setIcon("potion_bottle_lingering").setCategory(CreativeItemCategory::Miscellaneous);
// 
// 	Item::mPrismarineShard->setIcon("prismarine_shard").setCategory(CreativeItemCategory::Miscellaneous);
// 	Item::mPrismarineCrystals->setIcon("prismarine_crystals").setCategory(CreativeItemCategory::Miscellaneous);
// 
// 	Item::mElytra->setIcon("elytra").setCategory(CreativeItemCategory::Tools);
// 
// 	Item::mShulkerShell->setIcon("shulker_shell").setCategory(CreativeItemCategory::Miscellaneous);
// 
// #ifdef MCPE_EDU
// 	// EDU Items
// 	Item::mChalkboard->setIcon("chalkboard").setCategory(CreativeItemCategory::Decorations);
// 	Item::mPortfolioBook->setIcon("book_portfolio").setCategory(CreativeItemCategory::Tools);
// #endif
// 
// 	// ------------------------------------------------------------------------------
// 	// Load Items from Json
// 	// ------------------------------------------------------------------------------
// 	Json::Value list;
// 	Json::Reader reader;
// 	std::string nameId;
// 	std::string fileContent;
// 
// 	JSON_SCOPE("items_client.json");
// 	if (Resource::load(ResourceLocation("items_client.json"), fileContent)) {
// 		if (reader.parse(fileContent, list)) {
// 			Json::Value root = list["items"];
// 			JSON_SCOPE("items");
// 
// 			if (!root.isNull()) {
// 				Json::Value offsetsRoot;
// 				if (Resource::load(ResourceLocation("items_offsets_client.json"), fileContent)) {
// 					reader.parse(fileContent, offsetsRoot);
// 				}
// 				Json::Value offsetsList = offsetsRoot["render_offsets"];
// 
// 				for (int itemIndex = 0; itemIndex < MAX_ITEMS; ++itemIndex) {
// 					if (mItems[itemIndex]) {
// 						Item* item = mItems[itemIndex];
// 						Json::Value itemJson = root[item->getRawNameId()];
// 						if (!itemJson.isNull() && itemJson.isObject()) {
// 							JSON_SCOPE(item->getRawNameId());
// 							item->initClient(itemJson, offsetsList);
// 						}
// 					}
// 				}
// 			}
// 		}
// 	}
}

void Item::addCreativeItem(short item, short auxValue) {
	addCreativeItem(ItemInstance(item, 1, auxValue));
}

void Item::addCreativeItem(const Block* block, short auxValue) {
	addCreativeItem(ItemInstance(block->mID, 1, auxValue));
}

void Item::addCreativeItem(Item* item, short auxValue) {
	addCreativeItem(ItemInstance(item->getId(), 1, auxValue));
}

void Item::addCreativeItem(const ItemInstance& itemInstance) {
	mCreativeList.emplace_back(itemInstance);
}

void Item::addBlockItems() {

	//
	// Special case for certain items since they can have different icons
	// @note: Make sure those different items are handled in ItemInHandRenderer::renderItem
	//
// 	registerItem<AuxDataBlockItem>("stone", Block::mStone->mID - 256, Block::mStone).setCategory(CreativeItemCategory::BuildingBlocks);
// 	registerItem<AuxDataBlockItem>("dirt", Block::mDirt->mID - 256, Block::mDirt).setCategory(CreativeItemCategory::BuildingBlocks);
// 	registerItem<AuxDataBlockItem>("sand", Block::mSand->mID - 256, Block::mSand).setCategory(CreativeItemCategory::BuildingBlocks);
// 	registerItem<AuxDataBlockItem>("stained_hardened_clay", Block::mStainedClay->mID - 256, Block::mStainedClay).setCategory(CreativeItemCategory::BuildingBlocks);
// 	registerItem<ClothBlockItem>("wool", Block::mWool->mID - 256, Block::mWool).setCategory(CreativeItemCategory::Decorations);
// 	registerItem<ClothBlockItem>("carpet", Block::mWoolCarpet->mID - 256, Block::mWoolCarpet).setCategory(CreativeItemCategory::Decorations);
// 	registerItem<AuxDataBlockItem>("log", Block::mLog->mID - 256, Block::mLog).setCategory(CreativeItemCategory::BuildingBlocks);
// 	registerItem<AuxDataBlockItem>("fence", Block::mFence->mID - 256, Block::mFence).setCategory(CreativeItemCategory::Decorations);
// 	registerItem<AuxDataBlockItem>("stonebrick", Block::mStoneBrick->mID - 256, Block::mStoneBrick).setCategory(CreativeItemCategory::BuildingBlocks);
// 	registerItem<StoneSlabBlockItem>("double_stone_slab", Block::mStoneSlab->mID - 256).setCategory(CreativeItemCategory::BuildingBlocks);
// 	registerItem<StoneSlabBlockItem>("double_stone_slab2", Block::mStoneSlab2->mID - 256).setCategory(CreativeItemCategory::BuildingBlocks);
// 	registerItem<SaplingBlockItem>("sapling", Block::mSapling->mID - 256).setCategory(CreativeItemCategory::Decorations);
// 	registerItem<LeafBlockItem>("leaves", Block::mLeaves->mID - 256, Block::mLeaves).setCategory(CreativeItemCategory::Decorations);
// 	registerItem<LeafBlockItem>("leaves2", Block::mLeaves2->mID - 256, Block::mLeaves2).setCategory(CreativeItemCategory::Decorations);
// 	registerItem<AuxDataBlockItem>("sandstone", Block::mSandStone->mID - 256, Block::mSandStone).setCategory(CreativeItemCategory::BuildingBlocks);
// 	registerItem<AuxDataBlockItem>("red_sandstone", Block::mRedSandstone->mID - 256, Block::mRedSandstone).setCategory(CreativeItemCategory::BuildingBlocks);
// 	registerItem<WoodSlabBlock::Item>("wooden_slab", Block::mWoodenSlab->mID - 256).setCategory(CreativeItemCategory::BuildingBlocks);
// 	registerItem<AuxDataBlockItem>("yellow_flower", Block::mYellowFlower->mID - 256, Block::mYellowFlower).setCategory(CreativeItemCategory::Decorations);
// 	registerItem<AuxDataBlockItem>("red_flower", Block::mRedFlower->mID - 256, Block::mRedFlower).setCategory(CreativeItemCategory::Decorations);
// 	registerItem<AuxDataBlockItem>("double_plant", Block::mDoublePlant->mID - 256, Block::mDoublePlant).setCategory(CreativeItemCategory::Decorations);
// 	registerItem<WaterLilyBlockItem>("waterlily", Block::mWaterlily->mID - 256).setCategory(CreativeItemCategory::Decorations);
// 
// 	registerItem<AuxDataBlockItem>("sponge", Block::mSponge->mID - 256, Block::mSponge).setCategory(CreativeItemCategory::Decorations);
// 
// 	registerItem<TopSnowBlockItem>("snow_layer", Block::mTopSnow->mID - 256).setCategory(CreativeItemCategory::Decorations);
// 
// 	registerItem<AuxDataBlockItem>("planks", Block::mWoodPlanks->mID - 256, Block::mWoodPlanks).setCategory(CreativeItemCategory::BuildingBlocks);
// 	registerItem<AuxDataBlockItem>("quartz_block", Block::mQuartzBlock->mID - 256, Block::mQuartzBlock).setCategory(CreativeItemCategory::BuildingBlocks);
// 	registerItem<AuxDataBlockItem>("purpur_block", Block::mPurpurBlock->mID - 256, Block::mPurpurBlock).setCategory(CreativeItemCategory::BuildingBlocks);
// 
// 	registerItem<AuxDataBlockItem>("cobblestone_wall", Block::mCobblestoneWall->mID - 256, Block::mCobblestoneWall).setCategory(CreativeItemCategory::Decorations);
// 
// 	registerItem<AuxDataBlockItem>("tallgrass", Block::mTallgrass->mID - 256, Block::mTallgrass).setCategory(CreativeItemCategory::Decorations);
// 	registerItem<AuxDataBlockItem>("brown_mushroom_block", Block::mBrownMushroomBlock->mID - 256, Block::mBrownMushroomBlock).setCategory(CreativeItemCategory::Decorations);
// 	registerItem<AuxDataBlockItem>("red_mushroom_block", Block::mRedMushroomBlock->mID - 256, Block::mRedMushroomBlock).setCategory(CreativeItemCategory::Decorations);
// 	registerItem<AuxDataBlockItem>("log2", Block::mLog2->mID - 256, Block::mLog2).setCategory(CreativeItemCategory::BuildingBlocks);
// 	registerItem<AuxDataBlockItem>("end_portal_frame", Block::mEndPortalFrame->mID - 256, Block::mEndPortalFrame).setCategory(CreativeItemCategory::Decorations);
// 	registerItem<AuxDataBlockItem>("anvil", Block::mAnvil->mID - 256, Block::mAnvil).setCategory(CreativeItemCategory::Decorations);
// 
// 	registerItem<AuxDataBlockItem>("monster_egg", Block::mMonsterStoneEgg->mID - 256, Block::mMonsterStoneEgg).setCategory(CreativeItemCategory::Decorations);
// 
// 	registerItem<BlockItem>("brewingStandBlock", Block::mBrewingStand->mID - 256).setCategory(CreativeItemCategory::Decorations);
// 	registerItem<BlockItem>("beacon", Block::mBeacon->mID - 256).setCategory(CreativeItemCategory::Miscellaneous);
// 
// 	registerItem<AuxDataBlockItem>("prismarine", Block::mPrismarine->mID - 256, Block::mPrismarine).setCategory(CreativeItemCategory::BuildingBlocks);
// 	registerItem<BlockItem>("seaLantern", Block::mSeaLantern->mID - 256).setCategory(CreativeItemCategory::Decorations);
// 
// 	registerItem<AuxDataBlockItem>("stained_glass", Block::mStainedGlass->mID - 256, Block::mStainedGlass).setCategory(CreativeItemCategory::Decorations);
// 	registerItem<AuxDataBlockItem>("stained_glass_pane", Block::mStainedGlassPane->mID - 256, Block::mStainedGlassPane).setCategory(CreativeItemCategory::Decorations);
// 
// 	// Start at 1 as we don't want to add the air block as an item.
// 	for (int i = 1; i < 256; i++) {
// 		if (Block::mBlocks[i] != nullptr && Block::mBlocks[i]->getDescriptionId() != "") {
// 			if (Item::mItems[i] == nullptr) {
// 				registerItem<BlockItem>(Block::mBlocks[i]->getDescriptionId(), i - 256).setRequiresWorldBuilder(Block::mBlocks[i]->hasProperty(BlockProperty::RequiresWorldBuilder)).mCreativeCategory = Block::mBlocks[i]->getCreativeCategory();
// 			}
// 		}
// 	}
}

void Item::initCreativeItems() {
// 	mCreativeList.clear();
// 
// 	addCreativeItem(Block::mBeacon);
// 
// 	addCreativeItem(Block::mRail);
// 	addCreativeItem(Block::mGoldenRail);
// 	addCreativeItem(Block::mDetectorRail);
// 	addCreativeItem(Block::mActivatorRail);
// 
// 	addCreativeItem(Block::mCobblestone);
// 	addCreativeItem(Block::mStoneBrick, 0);
// 	addCreativeItem(Block::mStoneBrick, 1);
// 	addCreativeItem(Block::mStoneBrick, 2);
// 	addCreativeItem(Block::mStoneBrick, 3);
// 	addCreativeItem(Block::mMossyCobblestone);
// 	addCreativeItem(Block::mWoodPlanks);
// 	addCreativeItem(Block::mWoodPlanks, (int)WoodBlockType::Spruce);
// 	addCreativeItem(Block::mWoodPlanks, (int)WoodBlockType::Birch);
// 	addCreativeItem(Block::mWoodPlanks, (int)WoodBlockType::Jungle);
// 	addCreativeItem(Block::mWoodPlanks, (int)WoodBlockType::Acacia);
// 	addCreativeItem(Block::mWoodPlanks, (int)WoodBlockType::Big_Oak);
// 	addCreativeItem(Block::mBrick);
// 
// 	for(auto a : range(7)) {
// 		addCreativeItem(Block::mStone, (short) a);
// 	}
// 
// 	addCreativeItem(Block::mDirt);
// 	addCreativeItem(Block::mPodzol);
// 	addCreativeItem(Block::mGrass);
// 	addCreativeItem(Block::mMycelium);
// 	addCreativeItem(Block::mClay);
// 	addCreativeItem(Block::mHardenedClay);
// 
// 	for(auto a : range(16)) {
// 		addCreativeItem(Block::mStainedClay, (short) a);
// 	}
// 	addCreativeItem(Block::mSandStone, 0);
// 	addCreativeItem(Block::mSandStone, 1);
// 	addCreativeItem(Block::mSandStone, 2);
// 	addCreativeItem(Block::mRedSandstone, 0);
// 	addCreativeItem(Block::mRedSandstone, 1);
// 	addCreativeItem(Block::mRedSandstone, 2);
// 	addCreativeItem(Block::mSand, enum_cast(SandBlock::SandType::Normal));
// 	addCreativeItem(Block::mSand, enum_cast(SandBlock::SandType::Red));
// 	addCreativeItem(Block::mGravel);
// 	addCreativeItem(Block::mCobblestoneWall);
// 	addCreativeItem(Block::mCobblestoneWall, WallBlock::TYPE_MOSSY);
// 
// 	//add all tree trunks
// 	addCreativeItem(Block::mLog, enum_cast(OldLogBlock::LogType::Oak));
// 	addCreativeItem(Block::mLog, enum_cast(OldLogBlock::LogType::Spruce));
// 	addCreativeItem(Block::mLog, enum_cast(OldLogBlock::LogType::Birch));
// 	addCreativeItem(Block::mLog, enum_cast(OldLogBlock::LogType::Jungle));
// 	addCreativeItem(Block::mLog2, NewLogBlock::ACACIA_TRUNK);
// 	addCreativeItem(Block::mLog2, NewLogBlock::BIG_OAK_TRUNK);
// 
// 	addCreativeItem(Block::mNetherBrick);
// 	addCreativeItem(Block::mNetherrack);
// 	addCreativeItem(Block::mSoulSand);
// 	addCreativeItem(Block::mBedrock);
// 	addCreativeItem(Block::mStoneStairs);
// 	addCreativeItem(Block::mOakStairs);
// 	addCreativeItem(Block::mSpruceStairs);
// 	addCreativeItem(Block::mBirchStairs);
// 	addCreativeItem(Block::mJungleStairs);
// 	addCreativeItem(Block::mAcaciaStairs);
// 	addCreativeItem(Block::mDarkOakStairs);
// 	addCreativeItem(Block::mBrickStairs);
// 	addCreativeItem(Block::mSandstoneStairs);
// 	addCreativeItem(Block::mRedSandstoneStairs);
// 	addCreativeItem(Block::mStoneBrickStairs);
// 	addCreativeItem(Block::mNetherBrickStairs);
// 	addCreativeItem(Block::mQuartzStairs);
// 	addCreativeItem(Block::mPurpurStairs);
// 	addCreativeItem(Block::mStoneSlab, enum_cast(StoneSlabBlock::StoneSlabType::Stone));
// 	addCreativeItem(Block::mStoneSlab, enum_cast(StoneSlabBlock::StoneSlabType::Cobblestone));
// 
// 	addCreativeItem(Block::mWaterlily);
// 
// 	//add all wood slabs
// 	for(auto i : range((int)WoodBlockType::_count)) {
// 		addCreativeItem(Block::mWoodenSlab, (short) i);
// 	}
// 
// 	addCreativeItem(Block::mStoneSlab, enum_cast(StoneSlabBlock::StoneSlabType::Brick));
// 	addCreativeItem(Block::mStoneSlab, enum_cast(StoneSlabBlock::StoneSlabType::Sandstone));
// 	addCreativeItem(Block::mStoneSlab2, enum_cast(StoneSlabBlock2::StoneSlabType::RedSandstone));
// 	addCreativeItem(Block::mStoneSlab, enum_cast(StoneSlabBlock::StoneSlabType::StoneBrick));
// 	addCreativeItem(Block::mStoneSlab, enum_cast(StoneSlabBlock::StoneSlabType::Netherbrick));
// 	addCreativeItem(Block::mStoneSlab, enum_cast(StoneSlabBlock::StoneSlabType::Quartz));
// 	addCreativeItem(Block::mStoneSlab2, enum_cast(StoneSlabBlock2::StoneSlabType::Purpur));
// 
// 	addCreativeItem(Block::mQuartzBlock, enum_cast(QuartzBlockBlock::Type::Default));
// 	addCreativeItem(Block::mQuartzBlock, enum_cast(QuartzBlockBlock::Type::Lines));
// 	addCreativeItem(Block::mQuartzBlock, enum_cast(QuartzBlockBlock::Type::Chiseled));
// 
// 	addCreativeItem(Block::mPrismarine, enum_cast(PrismarineBlock::PrismarineType::Prismarine));
// 	addCreativeItem(Block::mPrismarine, enum_cast(PrismarineBlock::PrismarineType::PrismarineBricks));
// 	addCreativeItem(Block::mPrismarine, enum_cast(PrismarineBlock::PrismarineType::PrismarineDark));
// 	addCreativeItem(Block::mSeaLantern);
// 
// 	addCreativeItem(Block::mPurpurBlock, enum_cast(QuartzBlockBlock::Type::Default));
// 	addCreativeItem(Block::mPurpurBlock, enum_cast(QuartzBlockBlock::Type::Lines));
// 
// 	addCreativeItem(Block::mChorusPlantBlock);
// 	addCreativeItem(Block::mChorusFlowerBlock);
// 
// 	// Materials
// // 	addCreativeItem(Item::mCoal, CoalItem::STONE_COAL);
// // 	addCreativeItem(Item::mCoal, CoalItem::CHAR_COAL);
// 	addCreativeItem(Item::mDiamond);
// 	addCreativeItem(Item::mIronIngot);
// 	addCreativeItem(Item::mGoldIngot);
// 	addCreativeItem(Item::mEmerald);
// 	addCreativeItem(Item::mStick);
// 	addCreativeItem(Item::mBowl);
// 	addCreativeItem(Item::mString);
// 	addCreativeItem(Item::mFeather);
// 	addCreativeItem(Item::mFlint);
// 	addCreativeItem(Item::mLeather);
// 	addCreativeItem(Item::mRabbitHide);
// 	addCreativeItem(Item::mClay);
// 	addCreativeItem(Item::mSugar);
// 
// 	// Bricks
// 	addCreativeItem(Item::mBrick);
// 	addCreativeItem(Item::mNetherbrick);
// 
// 	// Ores
// 	addCreativeItem(Block::mCoalOre);
// 	addCreativeItem(Block::mIronOre);
// 	addCreativeItem(Block::mGoldOre);
// 	addCreativeItem(Block::mDiamondOre);
// 	addCreativeItem(Block::mLapisOre);
// 	addCreativeItem(Block::mRedStoneOre);
// 	addCreativeItem(Block::mEmeraldOre);
// 	addCreativeItem(Block::mQuartzOre);
// 
// 	addCreativeItem(Block::mGoldBlock);
// 	addCreativeItem(Block::mIronBlock);
// 	addCreativeItem(Block::mDiamondBlock);
// 	addCreativeItem(Block::mLapisBlock);
// 	addCreativeItem(Block::mCoalBlock);
// 	addCreativeItem(Block::mEmeraldBlock);
// 	addCreativeItem(Block::mRedstoneBlock);
// 	addCreativeItem(Item::mNetherQuartz);
// 
// 	addCreativeItem(Block::mObsidian);
// 	addCreativeItem(Block::mIce);
// 	addCreativeItem(Block::mPackedIce);
// 	addCreativeItem(Block::mSnow);
// 	addCreativeItem(Block::mTopSnow);
// 	addCreativeItem(Block::mGlass);
// 	addCreativeItem(Block::mGlowStone);
// 	addCreativeItem(Block::mVine);
// 
// 	addCreativeItem(Block::mLadder);
// 
// 	addCreativeItem(Block::mSponge, enum_cast(SpongeBlock::SpongeType::Dry));
// 	addCreativeItem(Block::mSponge, enum_cast(SpongeBlock::SpongeType::Wet));
// 	addCreativeItem(Block::mTorch);
// 	addCreativeItem(Block::mGlassPane);
// 
// 	addCreativeItem(Item::mBucket, 0);
// 	addCreativeItem(Item::mBucket, 1);	// got milk
// 	addCreativeItem(Item::mBucket, Block::mFlowingWater->mID);
// 	addCreativeItem(Item::mBucket, Block::mFlowingLava->mID);
// 	addCreativeItem(Item::mPaper);
// 	addCreativeItem(Item::mBook);
// 	addCreativeItem(Item::mArrow);
// 	addCreativeItem(Item::mBone);
// 
// 	addCreativeItem(Item::mDoor_wood);
// 	addCreativeItem(Item::mDoor_spruce);
// 	addCreativeItem(Item::mDoor_birch);
// 	addCreativeItem(Item::mDoor_jungle);
// 	addCreativeItem(Item::mDoor_acacia);
// 	addCreativeItem(Item::mDoor_darkoak);
// 	addCreativeItem(Item::mDoor_iron);
// 	
// 	addCreativeItem(Item::mEmptyMap);
// 
// 	addCreativeItem(Block::mTrapdoor);
// 	addCreativeItem(Block::mIronTrapdoor);
// 	addCreativeItem(Block::mFence, (int)WoodBlockType::Oak);
// 	addCreativeItem(Block::mFence, (int)WoodBlockType::Spruce);
// 	addCreativeItem(Block::mFence, (int)WoodBlockType::Birch);
// 	addCreativeItem(Block::mFence, (int)WoodBlockType::Jungle);
// 	addCreativeItem(Block::mFence, (int)WoodBlockType::Acacia);
// 	addCreativeItem(Block::mFence, (int)WoodBlockType::Big_Oak);
// 	addCreativeItem(Block::mNetherFence);
// 	addCreativeItem(Block::mFenceGateOak);
// 	addCreativeItem(Block::mSpuceFenceGate);
// 	addCreativeItem(Block::mBirchFenceGate);
// 	addCreativeItem(Block::mJungleFenceGate);
// 	addCreativeItem(Block::mAcaciaFenceGate);
// 	addCreativeItem(Block::mDarkOakFenceGate);
// 	addCreativeItem(Block::mIronFence);
// 
// 	addCreativeItem(Item::mBed);
// 	addCreativeItem(Block::mBookshelf);
// 	addCreativeItem(Item::mSign);
// 	addCreativeItem(Item::mPainting);
// 	addCreativeItem(Item::mItemFrame);
// 	addCreativeItem(Block::mWorkBench);
// 	addCreativeItem(Block::mStonecutterBench);
// 	addCreativeItem(Block::mChest);
// 	addCreativeItem(Block::mTrappedChest);
// 	addCreativeItem(Block::mFurnace);
// 	addCreativeItem(Item::mBrewing_stand);
// 	addCreativeItem(Item::mCauldron);
// 	addCreativeItem(Block::mNote);
// 	addCreativeItem(Block::mTNT);
// 	addCreativeItem(Block::mEndBrick);
// 	addCreativeItem(Block::mEndStone);
// 	addCreativeItem(Block::mEndRod);
// 	addCreativeItem(Block::mEndPortalFrame);
// 	//TODO Patole: Add Ender Pearl for real
// 	//addCreativeItem(Item::mEnderpearl);
// 
// 	addCreativeItem(Block::mAnvil, enum_cast(AnvilType::Undamaged));
// 	addCreativeItem(Block::mAnvil, enum_cast(AnvilType::SlightlyDamaged));
// 	addCreativeItem(Block::mAnvil, enum_cast(AnvilType::VeryDamaged));
// 
// 	// We do not add the ones with the eyes because it causes a lot of confusion with our players
// 	//addCreativeItem(Block::end_portal_frame, 1, 4);
// 
// 	addCreativeItem(Block::mYellowFlower);
// 
// 	for(auto a : range(9)) {
// 		addCreativeItem(Block::mRedFlower, (short) a);
// 	}
// 
// 	for(auto a : range(6)) {
// 		addCreativeItem(Block::mDoublePlant, (short) a);
// 	}
// 
// 	addCreativeItem(Block::mBrownMushroom);
// 	addCreativeItem(Block::mRedMushroom);
// 
// 	addCreativeItem(Block::mBrownMushroomBlock, 14);
// 	addCreativeItem(Block::mRedMushroomBlock, 14);
// 	addCreativeItem(Block::mBrownMushroomBlock);
// 	addCreativeItem(Block::mBrownMushroomBlock, 15);
// 
// 	addCreativeItem(Block::mCactus);
// 	addCreativeItem(Block::mMelon);
// 	addCreativeItem(Block::mPumpkin);
// 	addCreativeItem(Block::mLitPumpkin);
// 	addCreativeItem(Block::mWeb);
// 	addCreativeItem(Block::mHayBlock);
// 	addCreativeItem(Item::mReeds);
// 	addCreativeItem(Item::mWheat);
// 	addCreativeItem(Block::mTallgrass, enum_cast(TallGrassType::Tall));
// 	addCreativeItem(Block::mTallgrass, enum_cast(TallGrassType::Fern));
// 	addCreativeItem(Block::mDeadBush);
// 
// 	for(auto i : range(6)) {
// 		addCreativeItem(Block::mSapling, (short) i);
// 	}
// 
// 	//add all old leaves
// 	for(auto i : range(4)) {
// 		addCreativeItem(Block::mLeaves, (short) i);
// 	}
// 
// 	//add all old leaves
// 	for(auto i : range(2)) {
// 		addCreativeItem(Block::mLeaves2, (short) i);
// 	}
// 
// 	//add all tipped arrows to creative menu
// 	//We start at 5 to avoid creating tipped arrows for water bottle, awkward, etc.
// 	for (int i = 5; i < Potion::getPotionCount(); ++i) {
// 		addCreativeItem(Item::mArrow, (short)(i + 1));
// 	}
// 
// 	addCreativeItem(Item::mSeeds_wheat);
// 	addCreativeItem(Item::mSeeds_pumpkin);
// 	addCreativeItem(Item::mSeeds_melon);
// 	addCreativeItem(Item::mSeeds_beetroot);
// 	addCreativeItem(Item::mEgg);
// 
// 	// Food
// 	addCreativeItem(Item::mApple);
// 	addCreativeItem(Item::mApple_gold);
// 	addCreativeItem(Item::mApple_enchanted);
// 
// 	addCreativeItem(Item::mFish_raw_cod);
// 	addCreativeItem(Item::mFish_raw_salmon);
// 	addCreativeItem(Item::mFish_raw_clownfish);
// 	addCreativeItem(Item::mFish_raw_pufferfish);
// 	addCreativeItem(Item::mFish_cooked_cod);
// 	addCreativeItem(Item::mFish_cooked_salmon);
// 
// 	addCreativeItem(Item::mRotten_flesh);
// 	addCreativeItem(Item::mMushroomStew);
// 	addCreativeItem(Item::mBread);
// 	addCreativeItem(Item::mPorkChop_raw);
// 	addCreativeItem(Item::mPorkChop_cooked);
// 	addCreativeItem(Item::mChicken_raw);
// 	addCreativeItem(Item::mChicken_cooked);
// 	addCreativeItem(Item::mMutton_raw);
// 	addCreativeItem(Item::mMutton_cooked);
// 	addCreativeItem(Item::mBeef_raw);
// 	addCreativeItem(Item::mBeef_cooked);
// 	addCreativeItem(Item::mMelon);
// 	addCreativeItem(Item::mCarrot);
// 	addCreativeItem(Item::mPotato);
// 	addCreativeItem(Item::mPotatoBaked);
// 	addCreativeItem(Item::mPoisonous_potato);
// 	addCreativeItem(Item::mBeetroot);
// 	addCreativeItem(Item::mBeetrootSoup);
// 	addCreativeItem(Item::mCake);
// 	addCreativeItem(Item::mCookie);
// 	addCreativeItem(Item::mPumpkinPie);
// 	addCreativeItem(Item::mRabbitRaw);
// 	addCreativeItem(Item::mRabbitCooked);
// 	addCreativeItem(Item::mRabbitStew);
// 	addCreativeItem(Item::mChorusFruit);
// 	addCreativeItem(Item::mChorusFruitPopped);
// 
// 	addCreativeItem(Item::mNetherStar);
// 
// 	addCreativeItem(Item::mLead);
// 	addCreativeItem(Item::mNameTag);
// 
// 	// Brewing/Nether Items
// 	addCreativeItem(Item::mMagma_cream);
// 	addCreativeItem(Item::mBlazeRod);
// 	addCreativeItem(Item::mGold_nugget);
// 	addCreativeItem(Item::mGoldenCarrot);
// 	addCreativeItem(Item::mSpeckled_melon);
// 	addCreativeItem(Item::mRabbitFoot);
// 	addCreativeItem(Item::mGhast_tear);
// 	addCreativeItem(Item::mSlimeBall);
// 	addCreativeItem(Item::mBlazePowder);
// 	addCreativeItem(Item::mNether_wart);
// 	addCreativeItem(Item::mSulphur);
// 	addCreativeItem(Item::mRedStone);
// 	addCreativeItem(Item::mYellowDust);
// 	addCreativeItem(Item::mSpider_eye);
// 	addCreativeItem(Item::mFermented_spider_eye);
// 	addCreativeItem(Item::mDragon_breath);
// 
// 	// skulls
// 	addCreativeItem(Item::mSkull, enum_cast(SkullBlockEntity::SkullType::SKELETON));
// 	addCreativeItem(Item::mSkull, enum_cast(SkullBlockEntity::SkullType::WITHER));
// 	addCreativeItem(Item::mSkull, enum_cast(SkullBlockEntity::SkullType::ZOMBIE));
// 	addCreativeItem(Item::mSkull, enum_cast(SkullBlockEntity::SkullType::CHAR));
// 	addCreativeItem(Item::mSkull, enum_cast(SkullBlockEntity::SkullType::CREEPER));
// 	addCreativeItem(Item::mSkull, enum_cast(SkullBlockEntity::SkullType::DRAGON));
// 
// 	addCreativeItem(Item::mBow);
// 	addCreativeItem(Item::mFishingRod);
// 	addCreativeItem(Item::mFlintAndSteel);
// 	addCreativeItem(Item::mShears);
// 	addCreativeItem(Item::mClock);
// 	addCreativeItem(Item::mCompass);
// 	addCreativeItem(Item::mCarrotOnAStick);
// 	addCreativeItem(Item::mMinecart);
// 	addCreativeItem(Item::mChestMinecart);
// 	addCreativeItem(Item::mHopperMinecart);
// 	addCreativeItem(Item::mTNTMinecart);
// 
// 	addCreativeItem(Item::mBoat);
// 	addCreativeItem(Item::mBoat, 1);
// 	addCreativeItem(Item::mBoat, 2);
// 	addCreativeItem(Item::mBoat, 3);
// 	addCreativeItem(Item::mBoat, 4);
// 	addCreativeItem(Item::mBoat, 5);
// 
// 	addCreativeItem(Item::mSaddle);
// 	addCreativeItem(Item::mLeatherHorseArmor);
// 	addCreativeItem(Item::mIronHorseArmor);
// 	addCreativeItem(Item::mGoldHorseArmor);
// 	addCreativeItem(Item::mDiamondHorseArmor);
// 
// 	addCreativeItem(Item::mFlowerPot);
// 
// 	addCreativeItem(Item::mMobPlacer, (short) EntityClassTree::getEntityTypeIdLegacy(EntityType::Villager));
// 	addCreativeItem(Item::mMobPlacer, (short) EntityClassTree::getEntityTypeIdLegacy(EntityType::Chicken));
// 	addCreativeItem(Item::mMobPlacer, (short) EntityClassTree::getEntityTypeIdLegacy(EntityType::Cow));
// 	addCreativeItem(Item::mMobPlacer, (short) EntityClassTree::getEntityTypeIdLegacy(EntityType::Pig));
// 	addCreativeItem(Item::mMobPlacer, (short) EntityClassTree::getEntityTypeIdLegacy(EntityType::Sheep));
// 	addCreativeItem(Item::mMobPlacer, (short) EntityClassTree::getEntityTypeIdLegacy(EntityType::Wolf));
// 	addCreativeItem(Item::mMobPlacer, (short) EntityClassTree::getEntityTypeIdLegacy(EntityType::PolarBear));
// 	addCreativeItem(Item::mMobPlacer, (short) EntityClassTree::getEntityTypeIdLegacy(EntityType::Ocelot));
// 	addCreativeItem(Item::mMobPlacer, (short) EntityClassTree::getEntityTypeIdLegacy(EntityType::MushroomCow));
// 	addCreativeItem(Item::mMobPlacer, (short) EntityClassTree::getEntityTypeIdLegacy(EntityType::Bat));
// 	addCreativeItem(Item::mMobPlacer, (short) EntityClassTree::getEntityTypeIdLegacy(EntityType::Rabbit));
// 	addCreativeItem(Item::mMobPlacer, (short) EntityClassTree::getEntityTypeIdLegacy(EntityType::Horse));
// 	addCreativeItem(Item::mMobPlacer, (short)EntityClassTree::getEntityTypeIdLegacy(EntityType::Donkey));
// 	addCreativeItem(Item::mMobPlacer, (short)EntityClassTree::getEntityTypeIdLegacy(EntityType::Mule));
// 	addCreativeItem(Item::mMobPlacer, (short)EntityClassTree::getEntityTypeIdLegacy(EntityType::SkeletonHorse));
// 	addCreativeItem(Item::mMobPlacer, (short)EntityClassTree::getEntityTypeIdLegacy(EntityType::ZombieHorse));
// 
// 	addCreativeItem(Item::mMobPlacer, (short) EntityClassTree::getEntityTypeIdLegacy(EntityType::Creeper));
// 	addCreativeItem(Item::mMobPlacer, (short) EntityClassTree::getEntityTypeIdLegacy(EntityType::EnderMan));
// 	addCreativeItem(Item::mMobPlacer, (short) EntityClassTree::getEntityTypeIdLegacy(EntityType::Silverfish));
// 	addCreativeItem(Item::mMobPlacer, (short) EntityClassTree::getEntityTypeIdLegacy(EntityType::Skeleton));
// 	addCreativeItem(Item::mMobPlacer, (short)EntityClassTree::getEntityTypeIdLegacy(EntityType::WitherSkeleton));
// 	addCreativeItem(Item::mMobPlacer, (short) EntityClassTree::getEntityTypeIdLegacy(EntityType::Stray));
// 	addCreativeItem(Item::mMobPlacer, (short) EntityClassTree::getEntityTypeIdLegacy(EntityType::Slime));
// 	addCreativeItem(Item::mMobPlacer, (short) EntityClassTree::getEntityTypeIdLegacy(EntityType::Spider));
// 	addCreativeItem(Item::mMobPlacer, (short) EntityClassTree::getEntityTypeIdLegacy(EntityType::Zombie));
// 	addCreativeItem(Item::mMobPlacer, (short) EntityClassTree::getEntityTypeIdLegacy(EntityType::PigZombie));
// 	addCreativeItem(Item::mMobPlacer, (short) EntityClassTree::getEntityTypeIdLegacy(EntityType::Husk)); 
// 	addCreativeItem(Item::mMobPlacer, (short) EntityClassTree::getEntityTypeIdLegacy(EntityType::Squid));
// 	addCreativeItem(Item::mMobPlacer, (short) EntityClassTree::getEntityTypeIdLegacy(EntityType::CaveSpider));
// 	addCreativeItem(Item::mMobPlacer, (short) EntityClassTree::getEntityTypeIdLegacy(EntityType::Witch));
// 	addCreativeItem(Item::mMobPlacer, (short) EntityClassTree::getEntityTypeIdLegacy(EntityType::Guardian));
// 	addCreativeItem(Item::mMobPlacer, (short) EntityClassTree::getEntityTypeIdLegacy(EntityType::ElderGuardian));
// 	addCreativeItem(Item::mMobPlacer, (short) EntityClassTree::getEntityTypeIdLegacy(EntityType::Endermite));
// 	
// 	addCreativeItem(Item::mMobPlacer, (short) EntityClassTree::getEntityTypeIdLegacy(EntityType::LavaSlime));
// 	addCreativeItem(Item::mMobPlacer, (short) EntityClassTree::getEntityTypeIdLegacy(EntityType::Ghast));
// 	addCreativeItem(Item::mMobPlacer, (short) EntityClassTree::getEntityTypeIdLegacy(EntityType::Blaze));
// 	addCreativeItem(Item::mMobPlacer, (short)EntityClassTree::getEntityTypeIdLegacy(EntityType::Shulker));
// 
// 	addCreativeItem(Block::mMonsterStoneEgg, (short)MonsterEggBlock::HostBlockType::Stone);
// 	addCreativeItem(Block::mMonsterStoneEgg, (short)MonsterEggBlock::HostBlockType::Cobblestone);
// 	addCreativeItem(Block::mMonsterStoneEgg, (short)MonsterEggBlock::HostBlockType::StoneBrick);
// 	addCreativeItem(Block::mMonsterStoneEgg, (short)MonsterEggBlock::HostBlockType::MossyStoneBrick);
// 	addCreativeItem(Block::mMonsterStoneEgg, (short)MonsterEggBlock::HostBlockType::CrackedStoneBrick);
// 	addCreativeItem(Block::mMonsterStoneEgg, (short)MonsterEggBlock::HostBlockType::ChiseledStoneBrick);
// 
// #ifdef MCPE_EDU
// 	addCreativeItem(Item::mMobPlacer, (short)EntityClassTree::getEntityTypeIdLegacy(EntityType::Npc));
// #endif//MCPE_EDU
// 
// 	addCreativeItem(Item::mExperiencePotionItem);
// 
// 	addCreativeItem(Item::mFireCharge);
// 
// 	addCreativeItem(Item::mSword_wood);
// 	addCreativeItem(Item::mHoe_wood);
// 	addCreativeItem(Item::mShovel_wood);
// 	addCreativeItem(Item::mPickAxe_wood);
// 	addCreativeItem(Item::mHatchet_wood);
// 
// 	addCreativeItem(Item::mSword_stone);
// 	addCreativeItem(Item::mHoe_stone);
// 	addCreativeItem(Item::mShovel_stone);
// 	addCreativeItem(Item::mPickAxe_stone);
// 	addCreativeItem(Item::mHatchet_stone);
// 
// 	addCreativeItem(Item::mSword_iron);
// 	addCreativeItem(Item::mHoe_iron);
// 	addCreativeItem(Item::mShovel_iron);
// 	addCreativeItem(Item::mPickAxe_iron);
// 	addCreativeItem(Item::mHatchet_iron);
// 
// 	addCreativeItem(Item::mSword_diamond);
// 	addCreativeItem(Item::mHoe_diamond);
// 	addCreativeItem(Item::mShovel_diamond);
// 	addCreativeItem(Item::mPickAxe_diamond);
// 	addCreativeItem(Item::mHatchet_diamond);
// 
// 	addCreativeItem(Item::mSword_gold);
// 	addCreativeItem(Item::mHoe_gold);
// 	addCreativeItem(Item::mShovel_gold);
// 	addCreativeItem(Item::mPickAxe_gold);
// 	addCreativeItem(Item::mHatchet_gold);
// 
// 	addCreativeItem(Item::mHelmet_cloth);
// 	addCreativeItem(Item::mChestplate_cloth);
// 	addCreativeItem(Item::mLeggings_cloth);
// 	addCreativeItem(Item::mBoots_cloth);
// 
// 	addCreativeItem(Item::mHelmet_chain);
// 	addCreativeItem(Item::mChestplate_chain);
// 	addCreativeItem(Item::mLeggings_chain);
// 	addCreativeItem(Item::mBoots_chain);
// 
// 	addCreativeItem(Item::mHelmet_iron);
// 	addCreativeItem(Item::mChestplate_iron);
// 	addCreativeItem(Item::mLeggings_iron);
// 	addCreativeItem(Item::mBoots_iron);
// 
// 	addCreativeItem(Item::mHelmet_diamond);
// 	addCreativeItem(Item::mChestplate_diamond);
// 	addCreativeItem(Item::mLeggings_diamond);
// 	addCreativeItem(Item::mBoots_diamond);
// 
// 	addCreativeItem(Item::mHelmet_gold);
// 	addCreativeItem(Item::mChestplate_gold);
// 	addCreativeItem(Item::mLeggings_gold);
// 	addCreativeItem(Item::mBoots_gold);
// 
// 	addCreativeItem(Item::mElytra);
// 
// 	addCreativeItem(Item::mShulkerShell);
// 
// 	// Redstone
// 	addCreativeItem(Block::mLever);
// 	addCreativeItem(Block::mUnlitRedStoneLamp);
// 	addCreativeItem(Block::mLitRedStoneTorch);
// 	addCreativeItem(Block::mWoodPressurePlate);
// 	addCreativeItem(Block::mStonePressurePlate);
// 	addCreativeItem(Block::mLightWeightedPressurePlate);
// 	addCreativeItem(Block::mHeavyWeightedPressurePlate);
// 	addCreativeItem(Block::mWoodButton, Facing::EAST);
// 	addCreativeItem(Block::mStoneButton, Facing::EAST);
// 	addCreativeItem(Block::mDaylightDetector);
// 	addCreativeItem(Block::mTripwireHook);
// 	addCreativeItem(Item::mRepeater);
// 	addCreativeItem(Item::mComparator);
// 	addCreativeItem(Block::mDispenser, Facing::SOUTH);
// 	addCreativeItem(Block::mDropper, Facing::SOUTH);
// 	addCreativeItem(Block::mPiston, Facing::UP);
// 	addCreativeItem(Block::mStickyPiston, Facing::UP);
// 	addCreativeItem(Block::mObserver);
// 
// 	addCreativeItem(Block::mDragonEgg);
// 
// 	addCreativeItem(Item::mHopper);
// 
// 	addCreativeItem(Item::mSnowBall);
// 	addCreativeItem(Item::mEnderpearl);
// 	addCreativeItem(Item::mEnderEye);
// 	addCreativeItem(Item::mEndCrystal);
// 
// 	addCreativeItem(Item::mPrismarineShard);
// 	addCreativeItem(Item::mPrismarineCrystals);
// 
// 	addCreativeItem(Block::mMobSpawner);
// 	addCreativeItem(Block::mEnchantingTable);
// 
// 	addCreativeItem(Block::mSlimeBlock);
// 	addCreativeItem(Block::mEnderChest);
// 
// #ifdef MCPE_EDU
// 	addCreativeItem(Block::mAllow);
// 	addCreativeItem(Block::mDeny);
// 	addCreativeItem(Block::mBorder);
// 	addCreativeItem(Item::mChalkboard, enum_cast(ChalkboardSize::OnebyOne));
// 	addCreativeItem(Item::mChalkboard, enum_cast(ChalkboardSize::TwobyOne));
// 	addCreativeItem(Item::mChalkboard, enum_cast(ChalkboardSize::ThreebyTwo));
// 
// 	addCreativeItem(Item::mCamera);
// 	addCreativeItem(Item::mPortfolioBook);
// #endif//MCPE_EDU
// 
// 	for (int i = 0; i < Enchant::Type::_num_enchants; ++i) {
// 		for (int j = Enchant::mEnchants[i]->getMinLevel(); j <= Enchant::mEnchants[i]->getMaxLevel(); ++j) {
// 			addCreativeItem(EnchantUtils::generateEnchantedBook(EnchantmentInstance(i, j)));
// 		}
// 	}
// 
// 	static const int rainbow[] = {
// 		0, 8, 7, 15, 12, 14, 1, 4, 5, 13, 9, 3, 11, 10, 2, 6
// 	};
// 
// 	for (auto&& color : rainbow) {
// 		addCreativeItem(Block::mWool, (short) color);
// 	}
// 
// 	for (auto&& color : rainbow) {
// 		addCreativeItem(Block::mWoolCarpet, (short) color);
// 	}
// 
// 	for (auto&& color : rainbow) {
// 		addCreativeItem(Item::mDye_powder, (short) color);
// 	}
// 
// 	addCreativeItem(Item::mGlass_bottle);
// 
// 	for(unsigned i = 0; Potion::getPotion(i); ++i) {
// 		addCreativeItem(Item::mPotion, Potion::getPotion(i)->getPotionId());
// 	}
// 
// 	for(unsigned i = 0; Potion::getPotion(i); ++i) {
// 		addCreativeItem(Item::mSplash_potion, Potion::getPotion(i)->getPotionId());
// 	}
// 
// 	for (unsigned i = 0; Potion::getPotion(i); ++i) {
// 		addCreativeItem(Item::mLingering_potion, Potion::getPotion(i)->getPotionId());
// 	}
// 
// 	//for (auto&& color : rainbow) {
// 	//	addCreativeItem(Block::mStainedGlass, (short)color);
// 	//}
// 
// 	//for (auto&& color : rainbow) {
// 	//	addCreativeItem(Block::mStainedGlassPane, (short)color);
// 	//}
}

/*static*/
void Item::teardownItems() {
	for (int i = 0; i < MAX_ITEMS; ++i) {
		Item::mItems[i] = nullptr;
	}
	Item::mItemLookupMap.clear();
}

float Item::destroySpeedBonus(const ItemInstance* inst) const {
	if (!inst) {
		return 1.0f;
	}

// 	int efficiency = EnchantUtils::getEnchantLevel(Enchant::Type::MINING_EFFICIENCY, *inst);
	int efficiency = 0;
	if (efficiency <= 0) {
		return 0.0f;
	}

	return (float)efficiency * efficiency + 1;
}

//
// Item impl.
//
Item::Item(const std::string & nameId, short id)
	: mId(256 + id)
	, mDescriptionId(ICON_DESCRIPTION_PREFIX + nameId)
	, mRawNameId(nameId)
	, mCraftingRemainingItem(nullptr)
	, mMaxDamage(0)
	, mIconTexture(nullptr)
	, mIconAtlas(nullptr)
	, mCreativeCategory(CreativeItemCategory::Miscellaneous)
	, mHandEquipped(false)
	, mIsStackedByData(false)
	, mIsGlint(false)
	, mShouldDespawn(true)
	, mIsMirroredArt(false)
	, mRequiresWorldBuilder(false)
	, mMaxUseDuration(0)
	, mBlockId(BlockID::AIR)
// 	, mUseAnim(UseAnimation::None)
	, mHoverTextColorFormat(nullptr)
	, mExplodable(true)
	, mUsesRenderingAdjustment(false)
	, mRenderingAdjTrans(0)
	, mRenderingAdjRot(0)
	, mRenderingAdjScale(1.0f) {

	DEBUG_ASSERT(!nameId.empty(),"missing a name for this item");
}

byte Item::getMaxStackSize(const ItemInstance* item) {
	return m_maxStackSize;
}

void Item::inventoryTick(ItemInstance& itemInstance, Level& level, Entity& owner, int slot, bool selected) {
}

void Item::onCraftedBy(ItemInstance& itemInstance, Level& level, Player& player) {
}

CooldownType Item::getCooldownType() const {
	return CooldownType::None;
}

int Item::getCooldownTime() const {
	return 0;
}

std::string Item::getInteractText(const Player& player) const {
	std::string ret = "";
// 	if (player.getBoostableComponent()) {
// 		if (player.getBoostableComponent()->itemUseText(player, ret)) {
// 			return ret;
// 		}
// 	}

	return ret;
}

Item& Item::setMaxStackSize(byte max) {
	m_maxStackSize = max;
	return *this;
}

Item& Item::setRequiresWorldBuilder(bool value) {
	mRequiresWorldBuilder = value;
	return *this;
}

bool Item::canBeDepleted() {
	return mMaxDamage > 0 && !mIsStackedByData;
}

const TextureUVCoordinateSet& Item::getIcon(int auxValue, int frame, bool inInventoryPane) const {

	// Prefer UVTextureItem over mIcon if set
	if (mIconAtlas != nullptr)
	{
		int clampedValue = Math::clamp(auxValue, 0, 15);
		return (*mIconAtlas)[clampedValue];
	}

	DEBUG_ASSERT(mIconTexture,"Item doesn't have an icon atlas so it must have a single icon texture. If not we got bad data baby, bad data.");
	return *mIconTexture;
}

Item& Item::setIcon(TextureUVCoordinateSet const& icon) {
	mIconTexture = &icon;
	return *this;
}

Item& Item::setIcon(const std::string& name, int id) {

	mIconTexture = &mItemTextureAtlas->getTextureItem(name)[id];
	return *this;
}

BlockShape Item::getBlockShape() const {
	return BlockShape::INVISIBLE;
}

bool Item::isMirroredArt() const {
	return mIsMirroredArt;
}

Item& Item::setIsMirroredArt(bool val) {
	mIsMirroredArt = val;
	return *this;
}

ItemInstance& Item::use(ItemInstance& itemInstance, Player& player) {
	
// 	if (mFoodComponent) {
// 		mFoodComponent->use(itemInstance, player);
// 	}
// 	
// 	if (mCameraComponent) {
// 		mCameraComponent->use(itemInstance, player);
// 	}
// 
// 	//rideable component check
// 	if (player.isRiding() && player.mRiding->getBoostableComponent()) {
// 		player.mRiding->getBoostableComponent()->onItemInteract(itemInstance, player);
// 	}
// 	itemInstance.startCoolDown(&player);
	return itemInstance;
}

bool Item::useOn(ItemInstance& instance, Entity& entity, int x, int y, int z, FacingID face, float clickX, float clickY, float clickZ) {

	bool ret = false;

// 	if (mFoodComponent && mFoodComponent->useOn(instance, entity, BlockPos(x, y, z), face, Vec3(clickX, clickY, clickZ))) {
// 		ret = true;
// 	}
// 
// 	if (mSeedComponent && mSeedComponent->useOn(instance, entity, BlockPos(x, y, z), face, Vec3(clickX, clickY, clickZ))) {
// 		ret = true;
// 	}
// 	
// 	if (mCameraComponent && mCameraComponent->useOn(instance, entity, BlockPos(x, y, z), face, Vec3(clickX, clickY, clickZ))) {
// 		ret = true;
// 	}

	return ret;
}

bool Item::dispense(BlockSource& region, Container& container, int slot, const Vec3& pos, FacingID face) {
	return false;
}

void Item::useTimeDepleted(ItemInstance* inoutInstance, Level* level, Player* player) {
	
	// Its possible that useTimeDepleted() will return a new item that we need to instantiate.
// 	if (mFoodComponent) {
// 		if (Item* newItem = mFoodComponent->useTimeDepleted(*inoutInstance, *player, *level))
// 			*inoutInstance = ItemInstance(newItem);
// 	}
}

void Item::releaseUsing(ItemInstance* itemInstance, Player* player, int durationLeft) {
// 	if (mCameraComponent) {
// 		mCameraComponent->releaseUsing(*itemInstance, *player, durationLeft);
// 	}
}

float Item::getDestroySpeed(ItemInstance* itemInstance, const Block* block) {
	return 1;
}

bool Item::canDestroySpecial(const Block* block) const {
	return false;
}

int Item::getLevelDataForAuxValue(int auxValue) const {
	return 0;
}

bool Item::isStackedByData() const {
	return mIsStackedByData;
}

Item& Item::setCategory(CreativeItemCategory creativeCategory) {
	mCreativeCategory = creativeCategory;
	return *this;
}

short Item::getMaxDamage() {
	return mMaxDamage;
}

int Item::getAttackDamage() {
	return 0;
}

void Item::hurtEnemy(ItemInstance* itemInstance, Mob* mob, Mob* attacker) {
	//unless overrided, all tools take 2 damage on durability( not on proper uses )
// 	itemInstance->hurtAndBreak(2, attacker);
}

bool Item::interactEnemy(ItemInstance* itemInstance, Mob* mob, Player* player) {
	return false;
}

bool Item::mineBlock(ItemInstance* itemInstance, BlockID blockId, int x, int y, int z, Entity* owner) {
	
	//unless overrided, all tools take 2 damage on durability( not on proper uses )
	const Block* block = Block::mBlocks[blockId];
	if (block && block->canHurtAndBreakItem()) {
		itemInstance->hurtAndBreak(2, owner);
	}

	return true; //??Never used
}

Item& Item::setHandEquipped() {
	mHandEquipped = true;
	return *this;
}

bool Item::isHandEquipped() const {
	return mHandEquipped;
}

bool Item::isArmor() const {
	return false;
}

bool Item::isDye() const {
	return false;
}

bool Item::isGlint(const ItemInstance* itemInstance) const {
	
	if (itemInstance && itemInstance->isEnchanted())
		return true;

	return mIsGlint;
}

bool Item::isThrowable() const {
	return false;
}

bool Item::canDestroyInCreative() const {
	return true;
}

std::string Item::buildDescriptionName(const ItemInstance& itemInstance) const {
	
	std::string out;
	out = I18n::get(mDescriptionId + ".name");
	return out;
}

std::string Item::buildEffectDescriptionName(const ItemInstance& instance) const {

	std::string format;
// 	if (instance.isEnchanted()) {
// 		auto enchantingText = instance.getEnchantsFromUserData().getEnchantNames();
// 		format += ColorFormat::GRAY;
// 
// 		for (const auto& str : enchantingText) {
// 			format += str + '\n';
// 		}
// 		format += ColorFormat::RESET;
// 	}
	return format;
}

void Item::appendFormattedHovertext(const ItemInstance& item, Level& level, std::string& hovertext, const bool advancedToolTops) const {
	
// 	bool enchanted = item.isEnchanted();
// 
// 	// Figure out hover text color format
// 	std::string format;
// 	if (mHoverTextColorFormat != nullptr) {
// 		format = *mHoverTextColorFormat;
// 	}
// 	if (item.isEnchantingBook()) {
// 		format = ColorFormat::YELLOW;
// 	}
// 	else if (enchanted /*or is a music disk*/) {
// 		format = ColorFormat::AQUA;
// 	}
// 	else {
// 		format = ColorFormat::WHITE;
// 	}
// 
// 	hovertext += format + item.getHoverName();
// 
// 	if (enchanted) {
// 		auto enchantingText = item.getEnchantsFromUserData().getEnchantNames();
// 		hovertext += ColorFormat::GRAY;
// 
// 		for (const auto& str : enchantingText) {
// 			hovertext += Util::NEW_LINE + str;
// 		}
// 		hovertext += ColorFormat::RESET;
// 	}
}

void Item::readUserData(ItemInstance* itemInstance, IDataInput& input) const {
	unsigned short size;

	size = input.readShort();

	if(size > 0) {
		std::string inputStream;
		inputStream.resize(size);
		input.readBytes((char*)inputStream.data(), size);
		StringByteInput buffer(inputStream);
		
		if(auto et = NbtIo::read(buffer)) {
			itemInstance->setUserData(std::move(et));
		}
	}
}

void Item::writeUserData(const ItemInstance* itemInstance, IDataOutput& output) const {
	if(!itemInstance->getUserData()) {
		output.writeShort(0);
		return;
	}

	std::string outputStream;
	StringByteOutput buffer(outputStream);

	const auto& tags = itemInstance->getUserData();
	NbtIo::write(tags.get(), buffer);

	output.writeShort(outputStream.length());
	output.writeBytes(outputStream.data(), outputStream.length());
}

Item& Item::setStackedByData(bool isStackedByData) {
	mIsStackedByData = isStackedByData;
	return *this;
}

Item& Item::setMaxDamage(int maxDamage) {
	mMaxDamage = maxDamage;
	return *this;
}

TextureUVCoordinateSet Item::getTextureUVCoordinateSet(const std::string& name, int id) {
	return mItemTextureAtlas->getTextureItem(name)[id];
}

const TextureAtlasItem& Item::getTextureItem(const std::string& name) {
	return mItemTextureAtlas->getTextureItem(name);
}

int Item::getEnchantSlot(void) const {
// 	return (mId == Item::mBook->getId() || mId == Item::mEnchanted_book->getId()) ? Enchant::Slot::All : Enchant::Slot::NONE;
	return 0;
}

// an already enchanted book cannot be randomly enchanted using an enchanting table
int Item::getEnchantValue(void) const {
	return (mId == Item::mBook->getId()) ? 1 : 0;
}

bool Item::isComplex() const {
	return false;
}

int Item::getDamageChance(int unbreaking) const {
	return 100 / (unbreaking + 1);
}

Item* Item::lookupByName(const std::string& name, bool caseInsensitive) {
	DEBUG_ASSERT(!name.empty(),"must pass in a non empty string for item Name");

	// All names in map are lower case (so input must always be lower case)
	std::string testName = caseInsensitive ? Util::toLower(name) : name;

	auto itr = mItemLookupMap.find(testName);
	if (itr != mItemLookupMap.end()) {
		return itr->second.get();
	}
	
	// See if we need to pre pend "item." (happens with text coming from console commands)
	if (testName.find("item.") == -1) {
		testName = "item." + testName;
		itr = mItemLookupMap.find(testName);
		if (itr != mItemLookupMap.end()) {
			return itr->second.get();
		}
	}

	//TODO adors
	//hack for updating to new format
	if (name.find("minecraft:") == 0) {
		testName = name.substr(std::string("minecraft:").length());
		itr = mItemLookupMap.find(testName);
		if (itr != mItemLookupMap.end()) {
			return itr->second.get();
		}
	}

	return nullptr;
}

bool Item::_textMatch(const std::string& n1, const std::string& n2, bool ignoreCaseFor1) {
	return ignoreCaseFor1 ? Util::toLower(n1) == n2 : n1 == n2;
}
