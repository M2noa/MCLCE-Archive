/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "BlockRenderLayer.h"
#include "BlockState.h"
#include "Core/Math/Color.h"
#include "world/level/Rect2D.h"
#include "world/item/ItemCategory.h"
#include "world/level/block/entity/BlockEntityRendererId.h"
#include "world/phys/AABB.h"

class BlockPos;
class Entity;
class Mob;
class Player;
class BlockSource;
class Container;
class Material;
class Random;
class ItemInstance;
class TextureAtlas;
class Material;
class MobSpawnerData;
class TextureAtlasItem;
enum class EntityType : int;
enum class CreativeItemCategory : int;

enum class BlockProperty : uint32_t {
	Unspecified = 0,
	Stair = (1 << 0),
	HalfSlab = (1 << 1),
	Hopper = (1 << 2),
	TopSnow = (1 << 3),
	FenceGate = (1 << 4),
	Leaf = (1 << 5),
	ThinConnects2D = (1 << 6),
	Connects2D = (1 << 7),
	Carpet = (1 << 8),

	Door = (1 << 10),
	Portal = (1 << 11),
	Heavy = (1 << 12),
	Snow = (1 << 13),
	Trap = (1 << 14),
	Sign = (1 << 15),
	Walkable = (1 << 16),
	PressurePlate = (1 << 17),
	Slime = (1 << 18),

	TopSolidBlocking = (1 << 19),
	SolidBlocking = (1 << 20),
	CubeShaped = (1 << 21),

	Power_NO = (1 << 22),
	Power_BlockDown = (1 << 23),

	// piston behavior
	Immovable = (1 << 24),
	BreakOnPush = (1 << 25),
	Piston = (1 << 26),

	//blocks marked Infiniburn let fire on top of them never decay
	InfiniBurn = (1 << 27),
	
	RequiresWorldBuilder = (1 << 28),
	Placeholder = (1 << 29)
};

enum class BlockSupportType {
	Center,
	Edge,
	Any
};

enum class PistonResponse : unsigned char {
	Movable,
	BreakOnPush,
	Immovable
};
enum_bitwise_operators(BlockProperty, uint32_t);


//typedef int BlockRenderLayer;

// @Note: Got a memory leak in initBlocks? You probably didn't call
//        Block::init after constructing the block!
class Block {
public:

	static const float SIZE_OFFSET;

	// block update flags
	// neighbors: notify neighbors the block changed
	const static int UPDATE_NEIGHBORS = (1 << 0);
	// clients: send block update over network connections
	const static int UPDATE_CLIENTS = (1 << 1);
	// invisible: this update is invisible, so don't rebuild graphics
	const static int UPDATE_INVISIBLE = (1 << 2);
	// clients: something in the underlying item data changed, which needs to force an update
	const static int UPDATE_ITEM_DATA = (1 << 4);

	const static int UPDATE_NONE = UPDATE_INVISIBLE;
	const static int UPDATE_ALL = UPDATE_NEIGHBORS | UPDATE_CLIENTS;

	const static int TILE_NUM_SHIFT = 12;


	static const int NUM_BLOCK_TYPES = 256;

	static const Block* mBlocks[NUM_BLOCK_TYPES];
	static std::unordered_map<std::string, const Block*> mBlockLookupMap;

	static bool mShouldTick[NUM_BLOCK_TYPES];
	static bool mSolid[NUM_BLOCK_TYPES];
	static bool mPushesOutItems[NUM_BLOCK_TYPES];
	static float mTranslucency[NUM_BLOCK_TYPES];
	static Brightness mLightBlock[NUM_BLOCK_TYPES];
	static Brightness mLightEmission[NUM_BLOCK_TYPES];

	static const Block* mAir;
	static const Block* mStone;
	static Block* mGrass;	// Keep this non-const for now as we need to move the side color stuff to block graphics but that's too much refactor now
	static const Block* mDirt;
	static const Block* mCobblestone;
	static const Block* mWoodPlanks;
	static const Block* mSapling;
	static const Block* mBedrock;
	static const Block* mFlowingWater;
	static const Block* mStillWater;
	static const Block* mFlowingLava;
	static const Block* mStillLava;
	static const Block* mSand;
	static const Block* mGravel;
	static const Block* mGoldOre;
	static const Block* mIronOre;
	static const Block* mCoalOre;
	static const Block* mLog;
	static const Block* mLeaves;

	static const Block* mBorder;
	static const Block* mAllow;
	static const Block* mDeny;

	static const Block* mSponge;
	static const Block* mGlass;
	static const Block* mLapisOre;
	static const Block* mLapisBlock;
	static const Block* mDispenser;
	static const Block* mSandStone;
	static const Block* mNote;
	static const Block* mBed;
	static const Block* mGoldenRail;
	static const Block* mDetectorRail;
	static const Block* mWeb;
	static const Block* mTallgrass;
	static const Block* mDeadBush;
	static const Block* mPiston;
	static const Block* mStickyPiston;
	static const Block* mPistonArm;
	static const Block* mMovingBlock;
	static const Block* mObserver;
	static const Block* mWool;

	static const Block* mYellowFlower;
	static const Block* mRedFlower;
	static const Block* mBrownMushroom;
	static const Block* mRedMushroom;
	static const Block* mGoldBlock;
	static const Block* mIronBlock;
	static const Block* mDoubleStoneSlab;
	static const Block* mStoneSlab;
	static const Block* mBrick;
	static const Block* mTNT;
	static const Block* mBookshelf;
	static const Block* mMossyCobblestone;
	static const Block* mObsidian;
	static const Block* mTorch;

	static const Block* mMobSpawner;

	static const Block* mOakStairs;
	static const Block* mChest;
	static const Block* mRedStoneDust;

	static const Block* mDiamondOre;
	static const Block* mDiamondBlock;
	static const Block* mWorkBench;
	static const Block* mWheatCrop;
	static const Block* mFarmland;
	static const Block* mFurnace;
	static const Block* mLitFurnace;
	static const Block* mSign;
	static const Block* mWoodenDoor;
	static const Block* mLadder;
	static const Block* mRail;
	static const Block* mStoneStairs;
	static const Block* mWallSign;
	static const Block* mLever;
	static const Block* mStonePressurePlate;

	static const Block* mIronDoor;

	static const Block* mWoodPressurePlate;

	static const Block* mRedStoneOre;
	static const Block* mLitRedStoneOre;
	static const Block* mUnlitRedStoneTorch;
	static const Block* mLitRedStoneTorch;
	static const Block* mStoneButton;

	static const Block* mTopSnow;
	static const Block* mIce;
	static const Block* mSnow;
	static const Block* mCactus;
	static const Block* mClay;
	static const Block* mReeds;
	static const Block* mFence;
	static const Block* mNetherFence;

	static const Block* mPumpkin;
	static const Block* mNetherrack;
	static const Block* mSoulSand;

	static const Block* mGlowStone;
	static const Block* mPortal;
	static const Block* mLitPumpkin;
	static const Block* mCake;
	static const Block* mUnpoweredRepeater;
	static const Block* mPoweredRepeater;
	static const Block* mInvisibleBedrock;
	static const Block* mTrapdoor;

	static const Block* mMonsterStoneEgg;
	static const Block* mStoneBrick;
	static const Block* mBrownMushroomBlock;
	static const Block* mRedMushroomBlock;

	static const Block* mIronFence;
	static const Block* mGlassPane;
	static const Block* mMelon;
	static const Block* mPumpkinStem;
	static const Block* mMelonStem;
	static const Block* mVine;
	static const Block* mFenceGateOak;
	static const Block* mBrickStairs;
	static const Block* mMycelium;
	static const Block* mWaterlily;

	static const Block* mBrewingStand;
	static const Block* mCauldron;

	static const Block* mEndPortal;
	static const Block* mEndPortalFrame;

	static const Block* mEndBrick;
	static const Block* mEndStone;
	static const Block* mEndRod;
	static const Block* mUnlitRedStoneLamp;
	static const Block* mLitRedStoneLamp;
	static const Block* mCocoa;

	static const Block* mEmeraldOre;
	static const Block* mEmeraldBlock;

	static const Block* mSpruceStairs;
	static const Block* mBirchStairs;
	static const Block* mJungleStairs;

	static const Block* mBeacon;

	static const Block* mWoodButton;

	static const Block* mAcaciaStairs;
	static const Block* mDarkOakStairs;

	static const Block* mStoneBrickStairs;
	static const Block* mNetherBrick;
	static const Block* mNetherBrickStairs;
	static const Block* mNetherWart;
	static const Block* mEnchantingTable;

	static const Block* mDropper;
	static const Block* mActivatorRail;

	static const Block* mSandstoneStairs;

	static const Block* mTripwireHook;
	static const Block* mTripwire;

	static const Block* mCobblestoneWall;
	static const Block* mFlowerPot;
	static const Block* mCarrotCrop;
	static const Block* mPotatoCrop;

	static const Block* mSkull;
	static const Block* mAnvil;
	static const Block* mTrappedChest;

	static const Block* mLightWeightedPressurePlate;
	static const Block* mHeavyWeightedPressurePlate;

	static const Block* mUnpoweredComparator;
	static const Block* mPoweredComparator;

	static const Block* mDaylightDetector;
	static const Block* mRedstoneBlock;
	static const Block* mQuartzOre;
	static const Block* mHopper;
	static const Block* mQuartzBlock;
	static const Block* mQuartzStairs;

	static const Block* mPurpurBlock;
	static const Block* mPurpurStairs;

	static const Block* mChorusPlantBlock;
	static const Block* mChorusFlowerBlock;

	static const Block* mDoubleWoodenSlab;
	static const Block* mWoodenSlab;
	static const Block* mStainedClay;

	static const Block* mLeaves2;
	static const Block* mLog2;

	static const Block* mSlimeBlock;

	static const Block* mIronTrapdoor;

	static const Block* mHayBlock;
	static const Block* mWoolCarpet;
	static const Block* mCoalBlock;

	static const Block* mHardenedClay;
	static const Block* mPackedIce;
	static const Block* mDoublePlant;

	static const Block* mDaylightDetectorInverted;
	static const Block* mRedSandstone;
	static const Block* mRedSandstoneStairs;
	static const Block* mDoubleStoneSlab2;
	static const Block* mStoneSlab2;

	static const Block* mSpuceFenceGate;
	static const Block* mBirchFenceGate;
	static const Block* mJungleFenceGate;
	static const Block* mDarkOakFenceGate;
	static const Block* mAcaciaFenceGate;

	static const Block* mWoodenDoorSpruce;
	static const Block* mWoodenDoorBirch;
	static const Block* mWoodenDoorJungle;
	static const Block* mWoodenDoorAcacia;
	static const Block* mWoodenDoorDarkOak;

	static const Block* mGrassPathBlock;

	static const Block* mDragonEgg;

	static const Block* mItemFrame;

	static const Block* mStructureBlock;
	static const Block* mStructureVoid;

	static const Block* mPodzol;
	static const Block* mBeetrootCrop;
	static const Block* mStonecutterBench;
	static const Block* mGlowingObsidian;
	static const Block* mNetherReactor;
	static const Block* mInfoUpdateGame1;
	static const Block* mInfoUpdateGame2;
	static const Block* mInfoReserved6;

	static const Block* mFire;

	static const Block* mEnderChest;
	static const Block* mEndGateway;
	static const Block* mStainedGlass;
	static const Block* mStainedGlassPane;
	
	static const Block* mPrismarine;
	static const Block* mSeaLantern;

	static const Block* mCameraBlock;
	static const Block* mChalkboard;

	const BlockID mID;

	static Brightness getLightEmission(const BlockID id);

	static void initBlocks();

	const BlockState& getBlockState(BlockState::BlockStates stateType) const;

	static void teardownBlocks();

	static BlockID transformToValidBlockId(BlockID blockId);
	static BlockID transformToValidBlockId(BlockID blockId, const BlockPos& pos);

	Block(const std::string& nameId, int id, const Material& material);
	virtual ~Block();

	bool isType(const Block* compare) const;
	bool hasProperty(BlockProperty type) const;
	BlockProperty getProperties() const;
	virtual void tick(BlockSource& region, const BlockPos& pos, Random& random) const;
	
	BlockID getId() const { return mID; }

	// Collision Shape
	virtual bool getCollisionShape(AABB& outAABB, BlockSource& region, const BlockPos& pos, Entity* entity) const;

	virtual bool isObstructingChests(BlockSource& region, const BlockPos& pos) const;
	
	virtual Vec3 randomlyModifyPosition(const BlockPos& pos, int32_t& seed) const;
	virtual Vec3 randomlyModifyPosition(const BlockPos& pos) const;

	bool addAABB(const AABB& shape, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes) const;
	virtual void addAABBs(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes) const;
	// The bufferValue is not always the returned value, always use the reference returned.
	virtual const AABB& getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData = 0, bool isClipping = false, int clipData = 0) const;
	virtual bool addCollisionShapes(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes, Entity* entity) const;

	bool isSolid() const;
	void setSolid(bool solid);

	bool pushesOutItems() const;
	void setPushesOutItems(bool pushesOutItems);

	bool isUnbreakable() const;
	bool isHeavy() const;

	// If this block can provide support for another block (torch, rail, etc)
	virtual bool canProvideSupport(BlockSource& region, const BlockPos& pos, FacingID face, BlockSupportType type) const;

	bool isSolidBlockingBlock() const;

	virtual bool isInfiniburnBlock(int data) const;
	virtual bool isCropBlock() const;
	virtual bool isContainerBlock() const;
	virtual bool isCraftingBlock() const;
	virtual bool isInteractiveBlock() const;
	virtual bool isWaterBlocking() const;
	virtual bool isHurtableBlock() const;
	virtual bool isFenceBlock() const;
	virtual bool isStairBlock() const;
	virtual bool isRailBlock() const;
	virtual bool canHurtAndBreakItem() const;
	virtual bool isSignalSource() const;
	virtual bool isValidAuxValue(int value) const {
		UNUSED_PARAMETER(value);
		return true;
	}
	virtual int getDirectSignal(BlockSource& region, const BlockPos& pos, int dir) const;
	virtual bool waterSpreadCausesSpawn() const {
		return true;
	}

	virtual bool shouldConnectToRedstone(BlockSource& region, const BlockPos& pos, int direction) const {
		UNUSED_PARAMETER(region, pos, direction);
		return false;  
	}

	virtual void handleRain(BlockSource& region, const BlockPos& pos, float downfallAmount) const {
		UNUSED_PARAMETER(region, pos, downfallAmount);
	}

	BlockEntityType getBlockEntityType() const;

	bool hasBlockEntity() const;
	
	virtual float getThickness() const;

	virtual bool checkIsPathable(Entity& entity, const BlockPos& lastPathPos, const BlockPos& pathPos) const;

	virtual bool dispense(BlockSource& region, Container& container, int slot, const Vec3& pos, FacingID face) const;
	virtual void onPlace(BlockSource& region, const BlockPos& pos) const;
	virtual void onRemove(BlockSource& region, const BlockPos& pos) const;
	virtual void onExploded(BlockSource& region, const BlockPos& pos, Entity* entitySource) const;
	virtual void onStepOn(Entity& entity, const BlockPos& pos) const;
	virtual void onFallOn(BlockSource& region, const BlockPos& pos, Entity* entity, float fallDistance) const;
	virtual void onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const {
		UNUSED_PARAMETER(region, pos, strength, isFirstTime);
	}
	// Called before being moved
	virtual void onMove(BlockSource& region, const BlockPos& from, const BlockPos& to) const {
		UNUSED_PARAMETER(region, from, to);
	}
	// Returns if this object will separate from its getSecondPart if moved by a piston
	virtual bool detachesOnPistonMove(BlockSource& region, const BlockPos& pos) const {
		UNUSED_PARAMETER(region, pos);
		return false;
	}
	virtual void onLoaded(BlockSource& region, const BlockPos& pos) const {
		UNUSED_PARAMETER(region, pos);
	}
	virtual BlockProperty getRedstoneProperty(BlockSource& region, const BlockPos& pos) const { 
		UNUSED_PARAMETER(region, pos);
		return mProperties; 
	}
	virtual void updateEntityAfterFallOn(Entity& entity) const;

	virtual bool onFertilized(BlockSource& region, const BlockPos& pos, Entity* entity) const;
	virtual bool mayConsumeFertilizer(BlockSource& region) const;

	virtual bool mayPick() const;
	virtual bool mayPick(BlockSource& region, int data, bool liquid) const;
	virtual bool mayPlace(BlockSource& region, const BlockPos& pos, FacingID face) const;
	virtual bool mayPlace(BlockSource& region, const BlockPos& pos) const;
	virtual bool mayPlaceOn(const Block& block) const;
	virtual bool tryToPlace(BlockSource& region, const BlockPos& pos, DataID data = 0) const;
	virtual bool breaksFallingBlocks(int data) const;

	virtual void destroy(BlockSource& region, const BlockPos& pos, int data, Entity* entitySource) const;
	virtual bool playerWillDestroy(Player& player, const BlockPos& pos, int data) const;

	virtual void neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const;

	virtual bool getSecondPart(BlockSource& region, const BlockPos& pos, BlockPos& out) const;
	
	virtual int getResource(Random& random, int data, int bonusLootLevel = 0) const;
	virtual int getResourceCount(Random& random, int data, int bonusLootLevel = 0) const;

	virtual ItemInstance asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const;

	virtual void spawnResources(BlockSource& region, const BlockPos& pos, int data, float odds = 1, int bonusLootLevel = 0) const;
	virtual bool spawnBurnResources(BlockSource& region, float x, float y, float z);
	void popResource(BlockSource& region, const BlockPos& pos, const ItemInstance& itemInstance) const;

	virtual float getExplosionResistance(Entity* entity = nullptr) const;

	virtual HitResult clip(BlockSource& region, const BlockPos& pos, const Vec3& A, const Vec3& B, bool isClipping = false, int clipData = 0) const;
	HitResult clip(BlockSource& region, const BlockPos& pos, const Vec3& A, const Vec3& B, bool isClipping, int clipData, const AABB& shape) const;

	virtual bool use(Player& player, const BlockPos& pos) const;

	virtual int getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const;
	virtual DataID calcVariant(BlockSource& region, const BlockPos& pos, DataID data) const;
	virtual bool isAttachedTo(BlockSource& region, const BlockPos& pos, BlockPos& outAttachedTo) const;
	
	virtual bool attack(Player* player, const BlockPos& pos) const;

	virtual void handleEntityInside(BlockSource& region, const BlockPos& pos, Entity* entity, Vec3& current) const;
	
	virtual void entityInside(BlockSource& region, const BlockPos& pos, Entity& entity) const;

	virtual void playerDestroy(Player* player, const BlockPos& pos, int data) const;

	virtual bool canSurvive(BlockSource& region, const BlockPos& pos) const;

	virtual int getExperienceDrop(Random& random) const;

	virtual bool canBeBuiltOver(BlockSource& region, const BlockPos& pos) const;

	Block& setNameId(const std::string& id);
	
	virtual void triggerEvent(BlockSource& region, const BlockPos& pos, int b0, int b1) const;

	virtual const MobSpawnerData* getMobToSpawn(BlockSource& region, const BlockPos& pos) const;

	const MobSpawnerData* getMobToSpawn(BlockSource& region, const BlockPos& pos, std::map<EntityType, int> priorityMobs, bool& surpassLimit) const;

	const Material& getMaterial() const;
	virtual Color getMapColor(BlockSource& region, const BlockPos& pos) const;
	virtual Color getMapColor() const;
	
	virtual bool shouldStopFalling(Entity& entity) const;

	virtual float calcGroundFriction(Mob& mob, const BlockPos& pos) const;

	virtual bool canHaveExtraData() const;
	
	virtual bool hasComparatorSignal() const;
	virtual int getComparatorSignal(BlockSource& region, const BlockPos& pos, FacingID dir, int data) const;

	bool canSlide() const;
	bool canInstatick() const;

	float getGravity() const;
	float getFriction() const;
	CreativeItemCategory getCreativeCategory() const;
	float getDestroySpeed() const;

	static const Block* lookupByName(const std::string& name, bool caseInsensitive = true);

	//CLIENT SIDE PROPERTIES

	//TODO split in one or more components: eg Graphics / Sound / Text / Seasons

	virtual bool shouldRenderFace(BlockSource& region, const BlockPos& pos, FacingID face, const AABB& shape) const;

	virtual int getIconYOffset() const;

	BlockRenderLayer getRenderLayer() const;

	const std::string & getDescriptionId() const;
	const std::string& getRawNameId() const { return mRawNameId; }

	virtual std::string buildDescriptionName(DataID data) const;

	virtual int getColor(int auxData) const;
	virtual int getColor(BlockSource& region, const BlockPos& pos) const;
	virtual int getColor(BlockSource& region, const BlockPos& pos, DataID data) const;
	virtual int getColorForParticle(BlockSource& region, const BlockPos& pos, int auxData) const;
	virtual bool isSeasonTinted(BlockSource& region, const BlockPos& p) const;
	virtual void onGraphicsModeChanged(bool fancy, bool preferPolyTessellation, bool transparentLeaves);
	static void DEPRECATEDcallOnGraphicsModeChanged(bool fancy, bool preferPolyTessellation, bool transparentLeaves);
	virtual BlockRenderLayer getRenderLayer(BlockSource& region, const BlockPos& pos) const;
	virtual int getExtraRenderLayers() const;
	bool isAlphaTested() const;
	float getBrightnessGamma() const {
		return mBrightnessGamma;
	}

	Block& setBrightnessGamma(float g) {
		mBrightnessGamma = g;
		return *this;
	}

	float getShadeBrightness() const;

	// Visual Shape
	virtual const AABB& getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping = false) const;
	virtual const AABB& getVisualShape(DataID data, AABB& bufferAABB, bool isClipping = false) const;

	bool isTextureAnimated() const {
		return mAnimatedTexture;
	}

	virtual int getVariant(int data) const;
	virtual FacingID getMappedFace(FacingID face, int data) const;

	virtual void animateTick(BlockSource& region, const BlockPos& pos, Random& random) const;

	void getDebugText(std::vector<std::string>& outputInfo) const;

	bool canGrowChorus() const;    // Only End Stone and Chorus Plant Blocks can grow Chorus

protected:
	virtual Block& init();

	const MobSpawnerData* getTypeToSpawn(BlockSource& region, EntityType category, const BlockPos& pos) const;

	virtual bool canBeSilkTouched() const;
	virtual ItemInstance getSilkTouchItemInstance(DataID data) const;

	virtual void setVisualShape(const AABB& shape);
	virtual void setVisualShape(const Vec3& min, const Vec3& max);

	virtual Block& setLightBlock(Brightness i);
	virtual Block& setLightEmission(float f);

	virtual Block& setExplodeable(float explosionResistance);
	virtual Block& setDestroyTime(float destroySpeed);
	virtual Block& setFriction(float f = 0.6f);
	virtual Block& setBlockProperty(BlockProperty property);
	virtual void setTicking(bool tick);
	virtual Block& setMapColor(const Color& color);

	virtual Block& addProperty(BlockProperty type);

	virtual Block& addBlockState(BlockState::BlockStates state, int numBits);
	virtual Block& resetBitsUsed();

	/*** Returns the item instance's auxValue when a BlockItem is spawned from this Block. */
	virtual DataID getSpawnResourcesAuxValue(DataID data) const;

	static FacingID getPlacementFacingAll(Entity& entity, const BlockPos& pos, float yRotOffsetDegree);
	static FacingID getPlacementFacingAllExceptAxisY(Entity& entity, const BlockPos& pos, float yRotOffsetDegree);

	static const std::string BLOCK_DESCRIPTION_PREFIX;


	std::string mDescriptionId;
	std::string mRawNameId;

	bool mFancy;
	bool mCanBuildOver = false;

	BlockRenderLayer mRenderLayer = RENDERLAYER_OPAQUE;
	BlockProperty mProperties = BlockProperty::Unspecified;
	BlockEntityType mBlockEntityType = BlockEntityType::None;
	bool mAnimatedTexture = false;
	float mBrightnessGamma = 1.f;

	float mThickness = 0;
	bool mCanSlide = false;
	bool mCanInstatick = false;

	float mGravity = 1.0f;
	const Material& mMaterial;
	Color mMapColor;
	float mFriction = 0.6f;
	bool mHeavy = false;

	float mDestroySpeed = 0;
	float mExplosionResistance = 0.0f;
	CreativeItemCategory mCreativeCategory = CreativeItemCategory::All;

private:
	friend class ContainerModelTests;
	friend class ContainerControllerTests;

	Block& setCategory(CreativeItemCategory creativeCategory);

	template<class T, class ... Args>
	static T& registerBlock(Args&& ... args);
	static std::vector<Unique<Block> > mOwnedBlocks;

	// The shape of the object
	// (this can be overridden in block renderer, just want a default that makes sense)
	// Defaults to 0 0 0, 1 1 1
	AABB mVisualShape;

	// registered block states
	unsigned int mBitsUsed;
	BlockState mBlockStates[BlockState::NumBlockStates];
};
