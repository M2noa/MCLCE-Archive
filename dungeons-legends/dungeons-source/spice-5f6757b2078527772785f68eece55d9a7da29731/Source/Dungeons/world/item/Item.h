/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

// Physical Dependencies
#include "locale/I18n.h"
#include "util/Random.h"
//#include "world/item/UseAnim.h"
#include "client/renderer/texture/TextureUVCoordinateSet.h"
#include "client/renderer/block/BlockGraphics.h"
#include "ItemCategory.h"
// #include "world/item/FoodItemComponent.h"
// #include "world/item/SeedItemComponent.h"
// #include "world/item/CameraItemComponent.h"
#include "Core/Math/Color.h"

// Logical Dependencies
class TextureAtlas;
class Level;
class Block;
class Entity;
class Mob;
class Player;
class Item;
class BlockPos;
class BlockSource;
class ItemInstance;
class Container;
class Vec3;
// class FoodItemComponent;
// class SeedItemComponent;
// class CameraItemComponent;
class TextureAtlasItem;
enum class UseAnimation : byte;
class IDataInput;
class IDataOutput;
class ResourcePackManager;

enum class CooldownType {
	None = -1,
	ChorusFruit = 0,
	EnderPearl = 1,

	Count
};

// [Constant Item Definition class]
class Item {
	static const byte MAX_STACK_SIZE = 64;	//Container::LARGE_MAX_STACK_SIZE;

public:
	static const int MAX_ITEMS = 512;//32000;
	static const int ICON_COLUMNS = 16;
	static const std::string ICON_DESCRIPTION_PREFIX;

	class Tier {
		const int mLevel;
		const int mUses;
		const float mSpeed;
		const int mDamage;
		const int mEnchantmentValue;
	public:
		static const Tier WOOD;
		static const Tier STONE;
		static const Tier IRON;
		static const Tier DIAMOND;
		static const Tier GOLD;

		Tier(int level, int uses, float speed, int damage, int enchant) :
			mLevel(level)
			, mUses(uses)
			, mSpeed(speed)
			, mDamage(damage)
			, mEnchantmentValue(enchant) {
		}

		int getUses() const { return mUses; }
		float getSpeed() const { return mSpeed; }
		int getAttackDamageBonus() const { return mDamage; }
		int getLevel() const { return mLevel; }
		int getEnchantmentValue() const { return mEnchantmentValue; }
		ItemInstance getTierItem(void) const;
	};

public:

	static Item* mItems[MAX_ITEMS];
	static std::unordered_map<std::string, Unique<Item>> mItemLookupMap;

	static Item* mShovel_iron;
	static Item* mPickAxe_iron;
	static Item* mHatchet_iron;
	static Item* mFlintAndSteel;
	static Item* mApple;
	static Item* mBow;
	static Item* mArrow;
	static Item* mCoal;
	static Item* mDiamond;
	static Item* mIronIngot;
	static Item* mGoldIngot;
	static Item* mSword_iron;

	static Item* mSword_wood;
	static Item* mShovel_wood;
	static Item* mPickAxe_wood;
	static Item* mHatchet_wood;

	static Item* mSword_stone;
	static Item* mShovel_stone;
	static Item* mPickAxe_stone;
	static Item* mHatchet_stone;

	static Item* mSword_diamond;
	static Item* mShovel_diamond;
	static Item* mPickAxe_diamond;
	static Item* mHatchet_diamond;

	static Item* mStick;
	static Item* mBowl;
	static Item* mMushroomStew;

	static Item* mSword_gold;
	static Item* mShovel_gold;
	static Item* mPickAxe_gold;
	static Item* mHatchet_gold;

	static Item* mString;
	static Item* mFeather;
	static Item* mSulphur;

	static Item* mHoe_wood;
	static Item* mHoe_stone;
	static Item* mHoe_iron;
	static Item* mHoe_diamond;
	static Item* mHoe_gold;

	static Item* mSeeds_wheat;
	static Item* mWheat;
	static Item* mBread;

	static Item* mHelmet_cloth;
	static Item* mChestplate_cloth;
	static Item* mLeggings_cloth;
	static Item* mBoots_cloth;

	static Item* mElytra;

	static Item* mHelmet_chain;
	static Item* mChestplate_chain;
	static Item* mLeggings_chain;
	static Item* mBoots_chain;

	static Item* mHelmet_iron;
	static Item* mChestplate_iron;
	static Item* mLeggings_iron;
	static Item* mBoots_iron;

	static Item* mHelmet_diamond;
	static Item* mChestplate_diamond;
	static Item* mLeggings_diamond;
	static Item* mBoots_diamond;

	static Item* mHelmet_gold;
	static Item* mChestplate_gold;
	static Item* mLeggings_gold;
	static Item* mBoots_gold;

	static Item* mFlint;
	static Item* mPorkChop_raw;
	static Item* mPorkChop_cooked;
	static Item* mPainting;

	static Item* mChorusFruit;
	static Item* mChorusFruitPopped;

	static Item* mApple_gold;
	static Item* mApple_enchanted;

	static Item* mSign;
	static Item* mDoor_wood;

	static Item* mBucket;
	//static Item* bucket_empty;
	//static Item* bucket_water;
	//static Item* bucket_lava;

	static Item* mMinecart;
	static Item* mSaddle;
	static Item* mDoor_iron;
	static Item* mRedStone;
	static Item* mSnowBall;

	static Item* mBoat;

	static Item* mLeather;
	static Item* mRabbitHide;

	//static Item* milk;
	static Item* mBrick;
	static Item* mClay;
	static Item* mReeds;
	static Item* mPaper;
	static Item* mBook;
	static Item* mSlimeBall;
	static Item* mChestMinecart;
	static Item* mMinecart_chest;
	static Item* mMinecart_furnace;
	static Item* mEgg;
	static Item* mCompass;
	static Item* mFishingRod;
	static Item* mClock;
	static Item* mYellowDust;
	static Item* mCarrotOnAStick;

	static Item* mFish_raw_cod;
	static Item* mFish_raw_salmon;
	static Item* mFish_raw_clownfish;
	static Item* mFish_raw_pufferfish;
	static Item* mFish_cooked_cod;
	static Item* mFish_cooked_salmon;

	static Item* mMelon;
	static Item* mSpeckled_melon;
	static Item* mSeeds_pumpkin;
	static Item* mSeeds_melon;

	static Item* mEnderpearl;
	static Item* mEnderEye;
	static Item* mEndCrystal;
	static Item* mBlazeRod;
	static Item* mNether_wart;
	static Item* mGold_nugget;
	static Item* mSpider_eye;
	static Item* mFermented_spider_eye;
	static Item* mBlazePowder;

	static Item* mMobPlacer;
	static Item* mExperiencePotionItem;

	static Item* mFireCharge;

	static Item* mDye_powder;
	static Item* mBone;
	static Item* mSugar;
	static Item* mCake;

	static Item* mBed;
	static Item* mRepeater;
	static Item* mCookie;

	static Item* mPumpkinPie;

	static Item* mPotato;
	static Item* mPotatoBaked;
	static Item* mPoisonous_potato;
	static Item* mCarrot;
	static Item* mGoldenCarrot;
	static Item* mBeetroot;
	static Item* mSeeds_beetroot;
	static Item* mBeetrootSoup;

	static Item* mSkull;

	static Item* mNetherStar;

	static Item* mDiode;

	static Item* mShears;

	static Item* mBeef_raw;
	static Item* mBeef_cooked;
	static Item* mChicken_raw;
	static Item* mChicken_cooked;
	static Item* mMutton_raw;
	static Item* mMutton_cooked;
	static Item* mRotten_flesh;

	static Item* mRabbitRaw;
	static Item* mRabbitCooked;
	static Item* mRabbitStew;
	static Item* mRabbitFoot;

	static Item* mLeatherHorseArmor;
	static Item* mIronHorseArmor;
	static Item* mGoldHorseArmor;
	static Item* mDiamondHorseArmor;
	static Item* mLead;
	static Item* mNameTag;

	static Item* mDoor_spruce;
	static Item* mDoor_birch;
	static Item* mDoor_jungle;
	static Item* mDoor_acacia;
	static Item* mDoor_darkoak;

	static Item* mGhast_tear;

	static Item* mMagma_cream;
	static Item* mBrewing_stand;
	static Item* mCauldron;

	static Item* mEnchanted_book;
	static Item* mComparator;
	static Item* mNetherbrick;
	static Item* mNetherQuartz;
	static Item* mTNTMinecart;

	static Item* mRecord_01;
	static Item* mRecord_02;

	static Item* mPrismarineShard;
	static Item* mPrismarineCrystals;
	
	static Item* mEmerald;
	static Item* mItemFrame;

	static Item* mFlowerPot;

	static Item* mPotion;
	static Item* mSplash_potion;
	static Item* mLingering_potion;
	static Item* mGlass_bottle;
	static Item* mDragon_breath;

	static Item* mHopperMinecart;
	static Item* mHopper;

	static Item* mFilledMap;
	static Item* mEmptyMap;

	static Item* mPortfolioBook;
	static Item* mChalkboard;
	static Item* mCamera;

	static Item* mShulkerShell;

	static void setTextureAtlas(std::shared_ptr<TextureAtlas> itemsTextureAtlas);

	static void registerItems();		// Create all the items.
// 	static void initServerData(ResourcePackManager& loader);		// Load in all the server overridden data
	static void initClientData();		// Load in all the client overridden data
	static void initCreativeItems();
	static void addCreativeItem(short id, short auxValue = 0);
	static void addCreativeItem(const Block* block, short auxValue = 0);
	static void addCreativeItem(Item* item, short auxValue = 0);
	static void addCreativeItem(const ItemInstance& instance);
	static void addBlockItems();
	static void teardownItems();

protected:

	//OVERRIDABLES
	//these members can (and should) be set to non-default values by implementing items
	byte m_maxStackSize = MAX_STACK_SIZE;
	std::string m_textureAtlasFile = "atlas.items";
	int m_frameCount = 1;
	bool m_animatesInToolbar = false;

	float destroySpeedBonus(const ItemInstance* inst) const;

public:

	// Construction/Destruction
	Item(const std::string& nameId, short id);
	virtual ~Item() { }

	// Field Access
	short getId() const { return mId; }
	const std::string & getDescriptionId() const { return mDescriptionId; }
	const std::string& getRawNameId() const { return mRawNameId; }
	const TextureUVCoordinateSet* getIconTexture() const { return mIconTexture; }
	const TextureAtlasItem* getIconAtlas() const { return mIconAtlas; }
	int getFrameCount() const { return m_frameCount; }
	bool isAnimatedInToolbar() const { return m_animatesInToolbar; }
	UseAnimation getUseAnimation() const { return mUseAnim; }
	int getMaxUseDuration() const { return mMaxUseDuration; }
	CreativeItemCategory getCreativeCategory() const { return mCreativeCategory; }
	BlockID getBlockId() const { return mBlockId; }
	bool isExplodable() const { return mExplodable; }
	bool shouldDespawn() const { return mShouldDespawn; }

	// Components Access
// 	bool isFood() const { return mFoodComponent.get() != nullptr; }
// 	bool isSeed() const { return mSeedComponent.get() != nullptr; }
// 	bool isCamera() const { return mCameraComponent.get() != nullptr; }

// 	FoodItemComponent* getFood() const { return mFoodComponent.get(); }
// 	SeedItemComponent* getSeed() const { return mSeedComponent.get(); }
// 	CameraItemComponent* getCamera() const { return mCameraComponent.get(); }

	// rendering adjustments access
	// currently, these only affect VR reality mode, but could be extended later for other situations
	bool usesRenderingAdjustment() const { return mUsesRenderingAdjustment; }
	Vec3 getRenderingPosAdjustment() const { return mRenderingAdjTrans; }
	Vec3 getRenderingRotAdjustment() const { return mRenderingAdjRot; }
	float getRenderingScaleAdjustment() const { return mRenderingAdjScale; }

	// Settings API
	virtual Item& setIcon(const std::string& name, int id = 0);
	virtual Item& setIcon(TextureUVCoordinateSet const& icon);
	virtual Item& setMaxStackSize(byte max);
	virtual Item& setCategory(CreativeItemCategory creativeCategory);
	virtual Item& setStackedByData(bool isStackedByData);
	virtual Item& setMaxDamage(int maxDamage);
	virtual Item& setHandEquipped();
	virtual Item& setUseAnimation(UseAnimation anim) { mUseAnim = anim; return *this; }
	virtual Item& setMaxUseDuration(int maxUseDuration) { mMaxUseDuration = maxUseDuration; return *this; }
	virtual Item& setRequiresWorldBuilder(bool value = true);
	virtual Item& setExplodable(bool boom = true) { mExplodable = boom; return *this; }
	virtual Item& setIsGlint(bool glint = false) { mIsGlint = glint; return *this; }
	virtual Item& setShouldDespawn(bool despawn = true) { mShouldDespawn = despawn; return *this; }
	virtual BlockShape getBlockShape() const;
	virtual bool canBeDepleted();
	virtual bool canDestroySpecial(const Block* block) const;
	virtual int getLevelDataForAuxValue(int auxValue) const;
	virtual bool isStackedByData() const;
	virtual short getMaxDamage();
	virtual int getAttackDamage();
	virtual bool isHandEquipped() const;
	virtual bool isArmor() const;
	virtual bool isDye() const;
	virtual bool isGlint(const ItemInstance* instance) const;
	virtual bool isThrowable() const;
	virtual bool canDestroyInCreative() const;
	virtual bool isLiquidClipItem(int auxValue) const {
		UNUSED_PARAMETER(auxValue);
		return false; 
	}
	virtual bool requiresInteract() const { return false; }
	virtual void appendFormattedHovertext(const ItemInstance& item, Level& level, std::string& hovertext, const bool advancedToolTops) const;
	virtual bool isValidRepairItem(const ItemInstance& source, const ItemInstance& repairItem) {
		UNUSED_PARAMETER(source,repairItem);
		return false; 
	}
	virtual int getEnchantSlot(void) const;
	virtual int getEnchantValue(void) const;
	virtual bool isComplex() const;
	virtual bool isValidAuxValue(int auxValue) const {
		UNUSED_PARAMETER(auxValue);
		return true; 
	}

	virtual int getDamageChance(int unbreaking) const;

	// Used to tell if Aux values are used to indicate a completely different version of an item such as potions
	virtual bool uniqueAuxValues() const { return false; }

	virtual Color getColor(const ItemInstance& instance) const {
		UNUSED_PARAMETER(instance);
		return Color::WHITE;
	}

	// Instance API
	virtual ItemInstance& use(ItemInstance& instance, Player& player);
	virtual bool useOn(ItemInstance& instance, Entity& entity, int x, int y, int z, FacingID face, float clickX, float clickY, float clickZ);
	virtual bool dispense(BlockSource& region, Container& container, int slot, const Vec3& pos, FacingID face);
	virtual void useTimeDepleted(ItemInstance* inoutInstance, Level* level, Player* player);
	virtual void releaseUsing(ItemInstance* instance, Player* player, int durationLeft);
	virtual float getDestroySpeed(ItemInstance* instance, const Block* block);
	virtual void hurtEnemy(ItemInstance* instance, Mob* mob, Mob* attacker);
	virtual bool interactEnemy(ItemInstance* instance, Mob* mob, Player* player);
	virtual bool mineBlock(ItemInstance* itemInstance, BlockID blockId, int x, int y, int z, Entity* owner);
	virtual std::string buildDescriptionName(const ItemInstance& instance) const;
	virtual std::string buildEffectDescriptionName(const ItemInstance& instance) const;
	virtual void readUserData(ItemInstance* itemInstance, IDataInput& input) const;
	virtual void writeUserData(const ItemInstance* itemInstance, IDataOutput& output) const;
	virtual byte getMaxStackSize(const ItemInstance* item);
	virtual void inventoryTick(ItemInstance& itemInstance, Level& level, Entity& owner, int slot, bool selected);
	virtual void onCraftedBy(ItemInstance& itemInstance, Level& level, Player& player);
	virtual CooldownType getCooldownType() const;
	virtual int getCooldownTime() const;

	//graphical component //TODO extract all these to ItemGraphics
	virtual std::string getInteractText(const Player& player) const;
	virtual int getAnimationFrameFor(Mob& holder) const {
		UNUSED_PARAMETER(holder);
		return 0; 
	}
	virtual bool isEmissive(int data) const {
		UNUSED_PARAMETER(data);
		return false; 
	}
	virtual const TextureUVCoordinateSet& getIcon(int auxValue, int frame, bool inInventoryPane = false) const;
	virtual int getIconYOffset() const { return 0; }
	virtual bool isMirroredArt() const;
	Item& setIsMirroredArt(bool val);
	//end graphical component

	static Item* lookupByName(const std::string& name, bool caseInsensitive = true);
	static bool _textMatch(const std::string& n1, const std::string& n2, bool ignoreCaseFor1);
	static TextureUVCoordinateSet getTextureUVCoordinateSet(const std::string& name, int id = 0);
	static const TextureAtlasItem& getTextureItem(const std::string& name);

protected:
	bool initServer(Json::Value & data);
	bool initClient(Json::Value & data, Json::Value & offsetsList);

	// Fields
	const short mId;
	std::string mDescriptionId;
	std::string mRawNameId;
	bool mIsMirroredArt;
	short mMaxDamage;
	bool mIsGlint;
	bool mHandEquipped;
	bool mIsStackedByData;
	bool mRequiresWorldBuilder:1;
	int mMaxUseDuration;
	bool mExplodable;
	bool mShouldDespawn;
	BlockID mBlockId;
	UseAnimation mUseAnim;
	CreativeItemCategory mCreativeCategory;
	Item* mCraftingRemainingItem;
	const std::string* mHoverTextColorFormat;
	const TextureUVCoordinateSet* mIconTexture;	// single uvs
	const TextureAtlasItem* mIconAtlas;	// multiple uvs

	// rendering adjustments
	bool mUsesRenderingAdjustment; // did the .json have a rendering adjustment?  Only apply adjustments if this is true

	// relative to the hand, X axis completes the right-handed frame
	// relative to the hand, if you hold a fist, the +Y axis is along the axis a sword would be pointing towards
	// relative to the hand, +Z is the direction pointed fingers would point in
	Vec3 mRenderingAdjTrans; 

	// rotation adjustments follow the right-handed rule:
	//  point thumb along axis, rotation towards the fingertips is positive angles
	//
	// note that these form Euler angles, and are applied in the following sequential order:
	// Y (rotates in the axis of the "thumb", think of the sword "twisting")
	// X (rotates in the plane of the thumb/fingers)
	// z (rotates along the fingers/forearm)
	Vec3 mRenderingAdjRot;

	// overall render scale adjustment, cumulative with other scale factors
	float mRenderingAdjScale;

	// Components
// 	std::unique_ptr<FoodItemComponent> mFoodComponent;
// 	std::unique_ptr<SeedItemComponent> mSeedComponent;
// 	std::unique_ptr<CameraItemComponent> mCameraComponent;

public:
	// Statics
	static std::vector<ItemInstance> mCreativeList;
	static std::shared_ptr<TextureAtlas> mItemTextureAtlas;
	static Random mRandom;
};

