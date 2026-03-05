/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/Block.h"
#include "world/level/block/BlockInclude.h"
#include "world/level/Level.h"
#include "world/level/dimension/Dimension.h"
// #include "world/level/Spawner.h"
#include "world/entity/player/Player.h"
// #include "world/entity/item/ItemEntity.h"
// #include "world/entity/item/ExperienceOrb.h"
#include "world/item/Item.h"
#include "world/entity/Entity.h"
#include "world/level/BlockSource.h"
#include "world/level/material/Material.h"
#include "world/phys/HitResult.h"
#include "world/phys/Vec3.h"
#include "locale/I18n.h"
#include "world/item/ItemCategory.h"
#include "util/WeighedRandom.h"
#include "world/level/biome/Biome.h"
// #include "world/food/FoodConstants.h"
#include "world/Facing.h"
#include "world/level/LevelConstants.h"
// #include "world/redstone/circuit/CircuitSystem.h"
#include "world/redstone/Redstone.h"
#include "world/level/block/CameraBlock.h"
// #include "client/renderer/debug/DebugRenderer.h"
#include "world/level/block/PrismarineBlock.h"
#include "world/level/block/BorderBlock.h"
// #include "world/level/block/entity/ChalkboardBlockEntity.h"

#include "world/level/storage/GameRules.h"


//------------------------------------------------------------------------------

const float Block::SIZE_OFFSET = 0.0001f;

const Brightness Brightness::MAX(0xf), Brightness::MIN(0), Brightness::INVALID(0xff);

const BlockID BlockID::AIR(0);
const FullBlock FullBlock::AIR(BlockID(0), 0);

const std::string Block::BLOCK_DESCRIPTION_PREFIX("tile.");

const Block* Block::mBlocks[NUM_BLOCK_TYPES];
std::vector<Unique<Block> > Block::mOwnedBlocks;
std::unordered_map<std::string, const Block* > Block::mBlockLookupMap;
Brightness Block::mLightBlock[NUM_BLOCK_TYPES];
Brightness Block::mLightEmission[NUM_BLOCK_TYPES];

bool Block::mSolid[NUM_BLOCK_TYPES] = {
	false
};

bool Block::mPushesOutItems[NUM_BLOCK_TYPES]{
	true
};

float Block::mTranslucency[] = {
	0.f
};									// @trans: translucent, @trans "asbMax", some more like "*condition"

bool Block::mShouldTick[] = {
	false
};

const Block* Block::mAir= nullptr;
const Block* Block::mStone= nullptr;
Block* Block::mGrass= nullptr;
const Block* Block::mDirt= nullptr;
const Block* Block::mCobblestone= nullptr;
const Block* Block::mWoodPlanks= nullptr;
const Block* Block::mSapling= nullptr;
const Block* Block::mBedrock= nullptr;
const Block* Block::mFlowingWater= nullptr;
const Block* Block::mStillWater= nullptr;
const Block* Block::mFlowingLava= nullptr;
const Block* Block::mStillLava= nullptr;
const Block* Block::mSand= nullptr;
const Block* Block::mGravel= nullptr;
const Block* Block::mGoldOre= nullptr;
const Block* Block::mIronOre= nullptr;
const Block* Block::mCoalOre= nullptr;
const Block* Block::mLog= nullptr;
const Block* Block::mLeaves= nullptr;

const Block* Block::mBorder = nullptr;
const Block* Block::mAllow = nullptr;
const Block* Block::mDeny = nullptr;

const Block* Block::mSponge= nullptr;
const Block* Block::mGlass= nullptr;
const Block* Block::mLapisOre= nullptr;
const Block* Block::mLapisBlock= nullptr;
const Block* Block::mDispenser = nullptr;
const Block* Block::mSandStone= nullptr;
const Block* Block::mNote= nullptr;
const Block* Block::mBed= nullptr;
const Block* Block::mGoldenRail= nullptr;
const Block* Block::mDetectorRail = nullptr;
const Block* Block::mWeb= nullptr;
const Block* Block::mTallgrass= nullptr;
const Block* Block::mDeadBush= nullptr;
const Block* Block::mPiston = nullptr;
const Block* Block::mStickyPiston = nullptr;
const Block* Block::mPistonArm = nullptr;
const Block* Block::mMovingBlock = nullptr;
const Block* Block::mObserver = nullptr;
const Block* Block::mWool= nullptr;

const Block* Block::mYellowFlower= nullptr;
const Block* Block::mRedFlower= nullptr;
const Block* Block::mBrownMushroom= nullptr;
const Block* Block::mRedMushroom= nullptr;
const Block* Block::mGoldBlock= nullptr;
const Block* Block::mIronBlock= nullptr;
const Block* Block::mDoubleStoneSlab= nullptr;
const Block* Block::mStoneSlab= nullptr;
const Block* Block::mBrick= nullptr;
const Block* Block::mTNT= nullptr;
const Block* Block::mBookshelf= nullptr;
const Block* Block::mMossyCobblestone= nullptr;
const Block* Block::mObsidian= nullptr;
const Block* Block::mTorch= nullptr;

const Block* Block::mMobSpawner= nullptr;

const Block* Block::mOakStairs= nullptr;
const Block* Block::mChest= nullptr;
const Block* Block::mRedStoneDust= nullptr;

const Block* Block::mDiamondOre= nullptr;
const Block* Block::mDiamondBlock= nullptr;
const Block* Block::mWorkBench= nullptr;
const Block* Block::mWheatCrop= nullptr;
const Block* Block::mFarmland= nullptr;
const Block* Block::mFurnace= nullptr;
const Block* Block::mLitFurnace= nullptr;
const Block* Block::mSign= nullptr;
const Block* Block::mWoodenDoor= nullptr;
const Block* Block::mLadder= nullptr;
const Block* Block::mRail= nullptr;
const Block* Block::mStoneStairs= nullptr;
const Block* Block::mWallSign= nullptr;
const Block* Block::mLever= nullptr;
const Block* Block::mStonePressurePlate= nullptr;

const Block* Block::mIronDoor= nullptr;

const Block* Block::mWoodPressurePlate= nullptr;

const Block* Block::mRedStoneOre= nullptr;
const Block* Block::mLitRedStoneOre= nullptr;
const Block* Block::mUnlitRedStoneTorch= nullptr;
const Block* Block::mLitRedStoneTorch= nullptr;
const Block* Block::mStoneButton= nullptr;

const Block* Block::mTopSnow= nullptr;
const Block* Block::mIce= nullptr;
const Block* Block::mSnow= nullptr;
const Block* Block::mCactus= nullptr;
const Block* Block::mClay= nullptr;
const Block* Block::mReeds= nullptr;
const Block* Block::mFence= nullptr;
const Block* Block::mNetherFence= nullptr;

const Block* Block::mPumpkin= nullptr;
const Block* Block::mNetherrack= nullptr;
const Block* Block::mSoulSand= nullptr;

const Block* Block::mGlowStone= nullptr;
const Block* Block::mPortal= nullptr;
const Block* Block::mLitPumpkin= nullptr;
const Block* Block::mCake= nullptr;
const Block* Block::mUnpoweredRepeater = nullptr;
const Block* Block::mPoweredRepeater = nullptr;
const Block* Block::mInvisibleBedrock= nullptr;
const Block* Block::mTrapdoor= nullptr;

const Block* Block::mMonsterStoneEgg= nullptr;
const Block* Block::mStoneBrick= nullptr;
const Block* Block::mBrownMushroomBlock= nullptr;
const Block* Block::mRedMushroomBlock= nullptr;

const Block* Block::mIronFence= nullptr;
const Block* Block::mGlassPane= nullptr;
const Block* Block::mMelon= nullptr;
const Block* Block::mPumpkinStem= nullptr;
const Block* Block::mMelonStem= nullptr;
const Block* Block::mVine= nullptr;
const Block* Block::mFenceGateOak= nullptr;
const Block* Block::mBrickStairs= nullptr;
const Block* Block::mMycelium= nullptr;
const Block* Block::mWaterlily= nullptr;

const Block* Block::mBrewingStand= nullptr;
const Block* Block::mCauldron = nullptr;

const Block* Block::mEndPortal = nullptr;
const Block* Block::mEndPortalFrame= nullptr;

const Block* Block::mEndBrick = nullptr;
const Block* Block::mEndStone= nullptr;
const Block* Block::mEndRod = nullptr;
const Block* Block::mUnlitRedStoneLamp= nullptr;
const Block* Block::mLitRedStoneLamp= nullptr;
const Block* Block::mCocoa= nullptr;

const Block* Block::mEmeraldOre= nullptr;
const Block* Block::mEmeraldBlock= nullptr;

const Block* Block::mSpruceStairs= nullptr;
const Block* Block::mBirchStairs= nullptr;
const Block* Block::mJungleStairs= nullptr;

const Block* Block::mBeacon = nullptr;

const Block* Block::mWoodButton= nullptr;

const Block* Block::mAcaciaStairs= nullptr;
const Block* Block::mDarkOakStairs= nullptr;

const Block* Block::mStoneBrickStairs= nullptr;
const Block* Block::mNetherBrick= nullptr;
const Block* Block::mNetherBrickStairs= nullptr;
const Block* Block::mNetherWart= nullptr;
const Block* Block::mEnchantingTable= nullptr;

const Block* Block::mDropper = nullptr;
const Block* Block::mActivatorRail= nullptr;

const Block* Block::mSandstoneStairs= nullptr;
const Block* Block::mTripwireHook= nullptr;
const Block* Block::mTripwire= nullptr;

const Block* Block::mCobblestoneWall= nullptr;
const Block* Block::mFlowerPot= nullptr;
const Block* Block::mCarrotCrop= nullptr;
const Block* Block::mPotatoCrop= nullptr;

const Block* Block::mSkull= nullptr;
const Block* Block::mAnvil= nullptr;
const Block* Block::mTrappedChest = nullptr;

const Block* Block::mLightWeightedPressurePlate= nullptr;
const Block* Block::mHeavyWeightedPressurePlate= nullptr;

const Block* Block::mUnpoweredComparator = nullptr;
const Block* Block::mPoweredComparator = nullptr;

const Block* Block::mDaylightDetector= nullptr;
const Block* Block::mRedstoneBlock= nullptr;
const Block* Block::mQuartzOre= nullptr;
const Block* Block::mHopper = nullptr;
const Block* Block::mQuartzBlock= nullptr;
const Block* Block::mQuartzStairs= nullptr;

const Block* Block::mPurpurBlock = nullptr;
const Block* Block::mPurpurStairs = nullptr;

const Block* Block::mChorusPlantBlock = nullptr;
const Block* Block::mChorusFlowerBlock = nullptr;

const Block* Block::mWoodenDoorSpruce = nullptr;
const Block* Block::mWoodenDoorBirch = nullptr;
const Block* Block::mWoodenDoorJungle = nullptr;
const Block* Block::mWoodenDoorAcacia = nullptr;
const Block* Block::mWoodenDoorDarkOak = nullptr;

const Block* Block::mDoubleWoodenSlab= nullptr;
const Block* Block::mWoodenSlab= nullptr;
const Block* Block::mStainedClay= nullptr;

const Block* Block::mLeaves2= nullptr;
const Block* Block::mLog2= nullptr;

const Block* Block::mSlimeBlock = nullptr;

const Block* Block::mIronTrapdoor= nullptr;

const Block* Block::mHayBlock= nullptr;
const Block* Block::mWoolCarpet= nullptr;
const Block* Block::mCoalBlock= nullptr;

const Block* Block::mHardenedClay= nullptr;
const Block* Block::mPackedIce= nullptr;
const Block* Block::mDoublePlant= nullptr;

const Block* Block::mDaylightDetectorInverted = nullptr;
const Block* Block::mRedSandstone = nullptr;
const Block* Block::mRedSandstoneStairs = nullptr;
const Block* Block::mDoubleStoneSlab2 = nullptr;
const Block* Block::mStoneSlab2 = nullptr;

const Block* Block::mSpuceFenceGate= nullptr;
const Block* Block::mBirchFenceGate= nullptr;
const Block* Block::mJungleFenceGate= nullptr;
const Block* Block::mDarkOakFenceGate= nullptr;
const Block* Block::mAcaciaFenceGate= nullptr;

const Block* Block::mDragonEgg = nullptr;

const Block* Block::mGrassPathBlock = nullptr;
const Block* Block::mItemFrame = nullptr;
const Block* Block::mStructureBlock = nullptr;
const Block* Block::mStructureVoid = nullptr;

const Block* Block::mPodzol= nullptr;
const Block* Block::mBeetrootCrop= nullptr;
const Block* Block::mStonecutterBench= nullptr;
const Block* Block::mGlowingObsidian= nullptr;
const Block* Block::mNetherReactor= nullptr;
const Block* Block::mInfoUpdateGame1= nullptr;
const Block* Block::mInfoUpdateGame2= nullptr;
const Block* Block::mInfoReserved6= nullptr;
const Block* Block::mFire= nullptr;

const Block* Block::mEnderChest = nullptr;
const Block* Block::mEndGateway = nullptr;
const Block* Block::mStainedGlass = nullptr;
const Block* Block::mStainedGlassPane = nullptr;


const Block* Block::mPrismarine = nullptr;
const Block* Block::mSeaLantern = nullptr;
const Block* Block::mCameraBlock= nullptr;

const Block* Block::mChalkboard = nullptr;

template<class T, class ... Args>
T& Block::registerBlock(Args&& ... args) {
	auto block = make_unique<T>(std::forward<Args>(args) ...);

	auto actualID = block->mID;
	DEBUG_ASSERT(actualID >= 0 && actualID <= 0xff, "Invalid ID value");
	DEBUG_ASSERT(Item::mItems[actualID] == nullptr, "Slot already assigned");

	auto ptr = block.get();

	Block::mOwnedBlocks.emplace_back(std::move(block));
	Block::mBlocks[actualID] = ptr;

	// Register Name
	std::string descId = Util::toLower(ptr->getRawNameId());
	if (!descId.empty())
	{
		DEBUG_ASSERT(!ptr->getDescriptionId().empty(), "Block missing a name?! please fix");
		DEBUG_ASSERT(Block::mBlockLookupMap.find(descId) == Block::mBlockLookupMap.end(), "We already have a Block with that name registered?!");
		Block::mBlockLookupMap[descId] = ptr;
	}

	return static_cast<T&>(*ptr);
}

/*static*/
void Block::initBlocks() {

	mOwnedBlocks.clear();
	memset(mBlocks, 0, sizeof(mBlocks));//null all the blocks

	Block::mTranslucency[0] = 1.f;

	mAir = &registerBlock<AirBlock>("air", 0, Material::getMaterial(MaterialType::Air)).setDestroyTime(-1);
	mStone = &registerBlock<StoneBlock>("stone", 1).setDestroyTime(1.5f).setExplodeable(10).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::MappedType, 3);
	mGrass = &registerBlock<GrassBlock>("grass", 2).setMapColor(Color::fromARGB(0x7FB238)).setDestroyTime(0.6f).setCategory(CreativeItemCategory::BuildingBlocks);
	mDirt = &registerBlock<DirtBlock>("dirt", 3).setMapColor(Color::fromARGB(0x976D4D)).setDestroyTime(0.5f).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::MappedType, 1);
	mCobblestone = &registerBlock<Block>("cobblestone", 4, Material::getMaterial(MaterialType::Stone)).setDestroyTime(2.0f).setExplodeable(10).setCategory(CreativeItemCategory::BuildingBlocks);
	mWoodPlanks = &registerBlock<WoodBlock>("planks", 5).setDestroyTime(2.0f).setExplodeable(5).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::MappedType, 3);
	mSapling = &registerBlock<Sapling>("sapling", 6).setDestroyTime(0.0f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::MappedType, 3).addBlockState(BlockState::AgeBit, 1);
	mBedrock = &registerBlock<BedrockBlock>("bedrock", 7).setDestroyTime(-1).setExplodeable(6000000).addProperty(BlockProperty::Immovable).setCategory(CreativeItemCategory::BuildingBlocks);
	mFlowingWater = &registerBlock<LiquidBlockDynamic>("flowing_water", 8, Material::getMaterial(MaterialType::Water)).setDestroyTime(100.0f).setLightBlock(Brightness(3)).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::LiquidDepth, 4);
	mStillWater = &registerBlock<LiquidBlockStatic>("water", 9, mFlowingWater->mID, Material::getMaterial(MaterialType::Water)).setDestroyTime(100.0f).setLightBlock(Brightness(3)).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::LiquidDepth, 4);
	mFlowingLava = &registerBlock<LiquidBlockDynamic>("flowing_lava", 10, Material::getMaterial(MaterialType::Lava)).setDestroyTime(100.0f).setLightEmission(1.0f).setLightBlock(Brightness::MAX).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::LiquidDepth, 4);
	mStillLava = &registerBlock<LiquidBlockStatic>("lava", 11, mFlowingLava->mID, Material::getMaterial(MaterialType::Lava)).setDestroyTime(100.0f).setLightEmission(1.0f).setLightBlock(Brightness::MAX).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::LiquidDepth, 4);
	mSand = &registerBlock<SandBlock>("sand", 12).setDestroyTime(0.5f).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::MappedType, 1);
	mGravel = &registerBlock<GravelBlock>("gravel", 13).setDestroyTime(0.6f).setCategory(CreativeItemCategory::BuildingBlocks);
	mGoldOre = &registerBlock<OreBlock>("gold_ore", 14).setDestroyTime(3.0f).setExplodeable(5).setCategory(CreativeItemCategory::BuildingBlocks);
	mIronOre = &registerBlock<OreBlock>("iron_ore", 15).setDestroyTime(3.0f).setExplodeable(5).setCategory(CreativeItemCategory::BuildingBlocks);
	mCoalOre = &registerBlock<OreBlock>("coal_ore", 16).setDestroyTime(3.0f).setExplodeable(5).setCategory(CreativeItemCategory::BuildingBlocks);
	mLog = &registerBlock<OldLogBlock>("log", 17).setDestroyTime(2.0f).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::MappedType, 2).addBlockState(BlockState::Direction, 2);
	mLeaves = &registerBlock<OldLeafBlock>("leaves", 18).setDestroyTime(0.2f).setLightBlock(Brightness(1)).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::MappedType, 2).addBlockState(BlockState::UpdateBit, 1).addBlockState(BlockState::PersistentBit, 1);

#ifdef MCPE_EDU
	//Edu Blocks
	mBorder = &registerBlock<BorderBlock>("border_block", 212).setMapColor(Color::fromARGB(0xFF0000)).setDestroyTime(0.2f).setExplodeable(6000).setCategory(CreativeItemCategory::BuildingBlocks).setBlockProperty(BlockProperty::RequiresWorldBuilder | BlockProperty::Immovable);
	mAllow = &registerBlock<Block>("allow", 210, Material::getMaterial(MaterialType::Allow)).setDestroyTime(0.2f).setExplodeable(6000).setCategory(CreativeItemCategory::BuildingBlocks).setBlockProperty(BlockProperty::RequiresWorldBuilder | BlockProperty::Immovable);
	mDeny = &registerBlock<Block>("deny", 211, Material::getMaterial(MaterialType::Deny)).setDestroyTime(0.2f).setExplodeable(6000).setCategory(CreativeItemCategory::BuildingBlocks).setBlockProperty(BlockProperty::RequiresWorldBuilder | BlockProperty::Immovable);
	mChalkboard = &registerBlock<ChalkboardBlock>("chalkboard", 230).setDestroyTime(1.0f).setCategory(CreativeItemCategory::None).setBlockProperty(BlockProperty::RequiresWorldBuilder | BlockProperty::Immovable).addBlockState(BlockState::Direction, 2);
	mCameraBlock = &registerBlock<CameraBlock>("camera", 242).setCategory(CreativeItemCategory::Tools);
	///////////
#endif

	mSponge = &registerBlock<SpongeBlock>("sponge", 19, Material::getMaterial(MaterialType::Sponge)).setCategory(CreativeItemCategory::Decorations).setDestroyTime(0.6f).setExplodeable(1);
	mGlass = &registerBlock<GlassBlock>("glass", 20, Material::getMaterial(MaterialType::Glass), false).setDestroyTime(0.3f).setCategory(CreativeItemCategory::Decorations);
	mLapisOre = &registerBlock<OreBlock>("lapis_ore", 21).setDestroyTime(3.0f).setExplodeable(5).setCategory(CreativeItemCategory::BuildingBlocks);
	mLapisBlock = &registerBlock<Block>("lapis_block", 22, Material::getMaterial(MaterialType::Stone)).setMapColor(Color::fromARGB(0x4A80FF)).setDestroyTime(3.0f).setExplodeable(5).setCategory(CreativeItemCategory::Decorations);

	mDispenser = &registerBlock<DispenserBlock>("dispenser", 23).setDestroyTime(3.5f).setCategory(CreativeItemCategory::Tools).addBlockState(BlockState::FacingDirection, 3).addBlockState(BlockState::TriggeredBit, 1);

	mSandStone = &registerBlock<SandStoneBlock>("sandstone", 24).setDestroyTime(0.8f).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::MappedType, 2);
	mNote = &registerBlock<NoteBlock>("noteblock", 25).setDestroyTime(0.8f).setCategory(CreativeItemCategory::Decorations);
	mBed = &registerBlock<BedBlock>("bed", 26).setDestroyTime(0.2f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::OccupiedBit, 1).addBlockState(BlockState::HeadPieceBit, 1);
	mGoldenRail = &registerBlock<PoweredRailBlock>("golden_rail", 27).setDestroyTime(0.7f).setCategory(CreativeItemCategory::Tools).addBlockState(BlockState::RailDirection, 3).addBlockState(BlockState::RailDataBit, 1);
	mDetectorRail = &registerBlock<DetectorRailBlock>("detector_rail", 28).setDestroyTime(0.7f).setCategory(CreativeItemCategory::Tools).addBlockState(BlockState::RailDirection, 3).addBlockState(BlockState::RailDataBit, 1);
	mStickyPiston = &registerBlock<PistonBlock>("sticky_piston", 29, PistonBlock::Type::Sticky).setDestroyTime(0.5f).setCategory(CreativeItemCategory::Tools).addBlockState(BlockState::FacingDirection, 3);
	mWeb = &registerBlock<WebBlock>("web", 30).setLightBlock(Brightness(1)).setDestroyTime(4.0f).setCategory(CreativeItemCategory::Decorations);
	mTallgrass = &registerBlock<TallGrass>("tallgrass", 31).setDestroyTime(0.0f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::MappedType, 2);
	mDeadBush = &registerBlock<DeadBush>("deadbush", 32).setDestroyTime(0.0f).setCategory(CreativeItemCategory::Decorations);
	mPiston = &registerBlock<PistonBlock>("piston", 33).setDestroyTime(0.5f).setCategory(CreativeItemCategory::Tools).addBlockState(BlockState::FacingDirection, 3);
	mPistonArm = &registerBlock<PistonArmBlock>("pistonArmCollision", 34).setDestroyTime(0.5f).addBlockState(BlockState::FacingDirection, 3);
	mWool = &registerBlock<ClothBlock>("wool", 35).setDestroyTime(0.8f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::MappedType, 4);

	mYellowFlower = &registerBlock<FlowerBlock>("yellow_flower", 37, FlowerBlock::Type::Yellow).setDestroyTime(0.0f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::MappedType, 4);
	mRedFlower = &registerBlock<FlowerBlock>("red_flower", 38, FlowerBlock::Type::Red).setDestroyTime(0.0f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::MappedType, 4);
	mBrownMushroom = &registerBlock<MushroomBlock>("brown_mushroom", 39).setDestroyTime(0.0f).setLightEmission(2 / 16.0f).setCategory(CreativeItemCategory::Decorations);
	mRedMushroom = &registerBlock<MushroomBlock>("red_mushroom", 40).setDestroyTime(0.0f).setCategory(CreativeItemCategory::Decorations);
	mGoldBlock = &registerBlock<MetalBlock>("gold_block", 41).setMapColor(Color::fromARGB(0xFAEE4D)).setDestroyTime(3.0f).setExplodeable(10).setCategory(CreativeItemCategory::Decorations);
	mIronBlock = &registerBlock<MetalBlock>("iron_block", 42).setMapColor(Color::fromARGB(0xA7A7A7)).setDestroyTime(5.0f).setExplodeable(10).setCategory(CreativeItemCategory::Decorations);
	mBrick = &registerBlock<Block>("brick_block", 45, Material::getMaterial(MaterialType::Stone)).setMapColor(Color::fromARGB(0x993333)).setDestroyTime(2.0f).setExplodeable(10).setCategory(CreativeItemCategory::BuildingBlocks);
	mTNT = &registerBlock<TntBlock>("tnt", 46).setDestroyTime(0.0f).setCategory(CreativeItemCategory::Tools).addBlockState(BlockState::ExplodeBit, 1);
	mBookshelf = &registerBlock<BookshelfBlock>("bookshelf", 47).setDestroyTime(1.5f).setCategory(CreativeItemCategory::Decorations);
	mMossyCobblestone = &registerBlock<Block>("mossy_cobblestone", 48, Material::getMaterial(MaterialType::Stone)).setDestroyTime(2.0f).setExplodeable(10).setCategory(CreativeItemCategory::BuildingBlocks);
	mObsidian = &registerBlock<ObsidianBlock>("obsidian", 49, false).setDestroyTime(35.0f).setExplodeable(2000).setCategory(CreativeItemCategory::BuildingBlocks);
	mTorch = &registerBlock<TorchBlock>("torch", 50).setDestroyTime(0.0f).setLightEmission(15 / 16.0f).setCategory(CreativeItemCategory::Tools).addBlockState(BlockState::FacingDirection, 3);

	mMobSpawner = &registerBlock<MobSpawnerBlock>("mob_spawner", 52).setDestroyTime(5.0f).setLightBlock(Brightness::MIN).setCategory(CreativeItemCategory::Decorations);

	mOakStairs = &registerBlock<StairBlock>("oak_stairs", 53, *mWoodPlanks).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::UpsideDownBit, 1);
	mChest = &registerBlock<ChestBlock>("chest", 54).setCategory(CreativeItemCategory::Decorations).setDestroyTime(2.5f).addBlockState(BlockState::FacingDirection, 3);
	mRedStoneDust = &registerBlock<RedStoneWireBlock>("redstone_wire", 55).setDestroyTime(0.0f).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::RedstoneSignal, 4);

	mDiamondOre = &registerBlock<OreBlock>("diamond_ore", 56).setDestroyTime(3.0f).setExplodeable(5).setCategory(CreativeItemCategory::BuildingBlocks);
	mDiamondBlock = &registerBlock<MetalBlock>("diamond_block", 57).setMapColor(Color::fromARGB(0x5CDBD5)).setDestroyTime(5.0f).setExplodeable(10).setCategory(CreativeItemCategory::Decorations);
	mWorkBench = &registerBlock<WorkbenchBlock>("crafting_table", 58).setDestroyTime(2.5f).setCategory(CreativeItemCategory::Decorations);
	mWheatCrop = &registerBlock<CropBlock>("wheat", 59).setDestroyTime(0.0f).setCategory(CreativeItemCategory::Miscellaneous).addBlockState(BlockState::Growth, 3);
	mFarmland = &registerBlock<FarmBlock>("farmland", 60).setDestroyTime(0.6f).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::MoisturizedAmount, 3);
	mFurnace = &registerBlock<FurnaceBlock>("furnace", 61, false).setDestroyTime(3.5f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::FacingDirection, 3);
	mLitFurnace = &registerBlock<FurnaceBlock>("lit_furnace", 62, true).setDestroyTime(3.5f).setLightEmission(14 / 16.0f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::FacingDirection, 3);
	mSign = &registerBlock<SignBlock>("standing_sign", 63, true).setDestroyTime(1.0f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::FacingDirection, 3);
	mWoodenDoor = &registerBlock<DoorBlock>("wooden_door", 64, Material::getMaterial(MaterialType::Wood), DoorBlock::OAK).setDestroyTime(3.0f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::OpenBit, 1).addBlockState(BlockState::UpperBlockBit, 1).resetBitsUsed().addBlockState(BlockState::DoorHingeBit, 1);
	mLadder = &registerBlock<LadderBlock>("ladder", 65).setDestroyTime(0.4f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::FacingDirection, 3);
	mRail = &registerBlock<RailBlock>("rail", 66).setDestroyTime(0.7f).setCategory(CreativeItemCategory::Tools).addBlockState(BlockState::RailDirection, 3).addBlockState(BlockState::RailDataBit, 1);
	mStoneStairs = &registerBlock<StairBlock>("stone_stairs", 67, *mCobblestone).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::UpsideDownBit, 1);
	mWallSign = &registerBlock<SignBlock>("wall_sign", 68, false).setDestroyTime(1.0f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::FacingDirection, 3);
	mLever = &registerBlock<LeverBlock>("lever", 69).setDestroyTime(0.5f).setCategory(CreativeItemCategory::Tools).addBlockState(BlockState::FacingDirection, 3).addBlockState(BlockState::OpenBit, 1);
	mStonePressurePlate = &registerBlock<PressurePlateBlock>("stone_pressure_plate", 70, Material::getMaterial(MaterialType::Stone), PressurePlateBlock::Sensitivity::MOBS).setDestroyTime(0.5f).setCategory(CreativeItemCategory::Tools).addBlockState(BlockState::RedstoneSignal, 4);

	mIronDoor = &registerBlock<DoorBlock>("iron_door", 71, Material::getMaterial(MaterialType::Metal), DoorBlock::IRON).setDestroyTime(5.0f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::OpenBit, 1).addBlockState(BlockState::UpperBlockBit, 1).resetBitsUsed().addBlockState(BlockState::DoorHingeBit, 1);

	mWoodPressurePlate = &registerBlock<PressurePlateBlock>("wooden_pressure_plate", 72, Material::getMaterial(MaterialType::Wood), PressurePlateBlock::Sensitivity::EVERYTHING).setDestroyTime(0.5f).setCategory(CreativeItemCategory::Tools).addBlockState(BlockState::RedstoneSignal, 4);

	mRedStoneOre = &registerBlock<RedStoneOreBlock>("redstone_ore", 73, false).setDestroyTime(3.0f).setExplodeable(5).setCategory(CreativeItemCategory::BuildingBlocks);
	mLitRedStoneOre = &registerBlock<RedStoneOreBlock>("lit_redstone_ore", 74, true).setDestroyTime(3.0f).setLightEmission(10 / 16.0f).setExplodeable(5);
	mUnlitRedStoneTorch = &registerBlock<RedstoneTorchBlock>("unlit_redstone_torch", 75, false).setDestroyTime(0.0f).setCategory(CreativeItemCategory::Tools).addBlockState(BlockState::FacingDirection, 3);
	mLitRedStoneTorch = &registerBlock<RedstoneTorchBlock> ("redstone_torch", 76, true).setDestroyTime(0.0f).setCategory(CreativeItemCategory::Tools).setLightEmission(8.0f / 16.0f).addBlockState(BlockState::FacingDirection, 3);
	mStoneButton = &registerBlock<StoneButtonBlock>("stone_button", 77).setDestroyTime(0.5f).setCategory(CreativeItemCategory::Tools).addBlockState(BlockState::FacingDirection, 3).addBlockState(BlockState::ButtonPressedBit, 1);

	mTopSnow = &registerBlock<TopSnowBlock>("snow_layer", 78).setDestroyTime(0.1f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::Height, 3).addBlockState(BlockState::CoveredBit, 1);
	mIce = &registerBlock<IceBlock>("ice", 79, false).setDestroyTime(0.5f).setLightBlock(Brightness(3)).setCategory(CreativeItemCategory::BuildingBlocks);
	mSnow = &registerBlock<SnowBlock>("snow", 80).setDestroyTime(0.2f).setCategory(CreativeItemCategory::BuildingBlocks);
	mCactus = &registerBlock<CactusBlock>("cactus", 81).setDestroyTime(0.4f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::Age, 4);
	mClay = &registerBlock<ClayBlock>("clay", 82).setDestroyTime(0.6f).setCategory(CreativeItemCategory::BuildingBlocks);
	mReeds = &registerBlock<ReedBlock>("reeds", 83).setDestroyTime(0.0f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::Age, 4);
	mFence = &registerBlock<FenceBlock>("fence", 85, Material::getMaterial(MaterialType::Wood)).setMapColor(WoodBlock::getWoodMaterialColor(WoodBlockType::Oak)).setDestroyTime(2.0f).setExplodeable(5).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::MappedType, 3);
	mNetherFence = &registerBlock<FenceBlock>("nether_brick_fence", 113, Material::getMaterial(MaterialType::Stone)).setMapColor(Color::fromARGB(0x700200)).setDestroyTime(2.0f).setExplodeable(10).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::MappedType, 3);

	mPumpkin = &registerBlock<PumpkinBlock>("pumpkin", 86, false).setDestroyTime(1.0f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::Direction, 2);
	mNetherrack = &registerBlock<Block>("netherrack", 87, Material::getMaterial(MaterialType::Stone)).setDestroyTime(0.4f).setCategory(CreativeItemCategory::BuildingBlocks).addProperty(BlockProperty::InfiniBurn);
	mSoulSand = &registerBlock<SoulSandBlock>("soul_sand", 88).setMapColor(Color::fromARGB(0x664C33)).setDestroyTime(0.5f).setCategory(CreativeItemCategory::BuildingBlocks);

	mGlowStone = &registerBlock<LightGemBlock>("glowstone", 89, Material::getMaterial(MaterialType::Glass)).setDestroyTime(0.3f).setCategory(CreativeItemCategory::Decorations).setLightEmission(1.0f);
	mPortal = (PortalBlock*)&registerBlock<PortalBlock>("portal", 90).setDestroyTime(-1).setLightEmission(0.75f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::Axis, 2);
	mLitPumpkin = &registerBlock<PumpkinBlock>("lit_pumpkin", 91, true).setDestroyTime(1.0f).setLightEmission(1.0f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::Direction, 2);
	mCake = &registerBlock<CakeBlock>("cake", 92).setDestroyTime(.5f).setCategory(CreativeItemCategory::Miscellaneous).addBlockState(BlockState::BiteCounter, 3);
	
	mUnpoweredRepeater = &registerBlock<RepeaterBlock>("unpowered_repeater", 93, false).setDestroyTime(0.0f).setCategory(CreativeItemCategory::Tools).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::RepeaterDelay, 2);
	mPoweredRepeater = &registerBlock<RepeaterBlock>("powered_repeater", 94, true).setDestroyTime(0.0f).setCategory(CreativeItemCategory::Tools).setLightEmission(8.0f / 16.0f).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::RepeaterDelay, 2);

	mInvisibleBedrock = &registerBlock<InvisibleBlock>("invisibleBedrock", 95, Material::getMaterial(MaterialType::Stone)).setDestroyTime(-1).setExplodeable(6000000).setLightBlock(Brightness::MAX).addProperty(BlockProperty::Immovable).setCategory(CreativeItemCategory::None);
	mTrapdoor = &registerBlock<TrapDoorBlock>("trapdoor", 96, Material::getMaterial(MaterialType::Wood)).setDestroyTime(3.0f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::UpsideDownBit, 1).addBlockState(BlockState::OpenBit, 1);

	mMonsterStoneEgg = &registerBlock<MonsterEggBlock>("monster_egg", 97).setDestroyTime(.75f).addBlockState(BlockState::MappedType, 3).setCategory(CreativeItemCategory::Decorations);
	mStoneBrick = &registerBlock<MultiTextureBlock>("stonebrick", 98, Material::getMaterial(MaterialType::Stone)).setDestroyTime(1.5f).setExplodeable(10).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::MappedType, 3);
	mBrownMushroomBlock = &registerBlock<HugeMushroomBlock>("brown_mushroom_block", 99, Material::getMaterial(MaterialType::Wood), HugeMushroomBlock::Type::Brown).setMapColor(Color::fromARGB(0x976D4D)).setDestroyTime(0.2f).setCategory(CreativeItemCategory::Decorations);
	mRedMushroomBlock = &registerBlock<HugeMushroomBlock>("red_mushroom_block", 100, Material::getMaterial(MaterialType::Wood), HugeMushroomBlock::Type::Red).setMapColor(Color::fromARGB(0x993333)).setDestroyTime(0.2f).setCategory(CreativeItemCategory::Decorations);

	// This should be called iron_bars but the iron part is cut out of the crafting screens
	mIronFence = &registerBlock<ThinFenceBlock>("iron_bars", 101, Material::getMaterial(MaterialType::Metal), true).setDestroyTime(5.0f).setExplodeable(10).setCategory(CreativeItemCategory::Decorations);
	mGlassPane = &registerBlock<ThinFenceBlock>("glass_pane", 102, Material::getMaterial(MaterialType::Glass), false).setDestroyTime(0.3f).setCategory(CreativeItemCategory::Decorations);
	mMelon = &registerBlock<MelonBlock>("melon_block", 103).setDestroyTime(1.0f).setMapColor(Color::fromARGB(0x7FCC19)).setCategory(CreativeItemCategory::Decorations);
	mPumpkinStem = &registerBlock<StemBlock>("pumpkin_stem", 104, *Block::mPumpkin).setDestroyTime(0.0f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::Growth, 3);
	mMelonStem = &registerBlock<StemBlock>("melon_stem", 105, *Block::mMelon).setDestroyTime(0.0f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::Growth, 3);
	mVine = &registerBlock<VineBlock>("vine", 106).setDestroyTime(0.2f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::VineGrowth, 4);
	mFenceGateOak = &registerBlock<FenceGateBlock>("fence_gate", 107, WoodBlockType::Oak).setMapColor(WoodBlock::getWoodMaterialColor(WoodBlockType::Oak)).setDestroyTime(2.0f).setExplodeable(5).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::OpenBit, 1).addBlockState(BlockState::InWallBit, 1);
	mBrickStairs = &registerBlock<StairBlock>("brick_stairs", 108, *Block::mBrick).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::UpsideDownBit, 1);
	mMycelium = &registerBlock<MyceliumBlock>("mycelium", 110).setMapColor(Color::fromARGB(0x7F3FB2)).setDestroyTime(0.6f).setCategory(CreativeItemCategory::BuildingBlocks);
	mWaterlily = &registerBlock<WaterlilyBlock>("waterlily", 111).setDestroyTime(0.0f).setCategory(CreativeItemCategory::Decorations);

	mBrewingStand = &registerBlock<BrewingStandBlock>("brewing_stand", 117).setDestroyTime(0.5f).setLightBlock(Brightness(3));
	mCauldron = &registerBlock<CauldronBlock>("cauldron", 118).setDestroyTime(2.0f).setLightBlock(Brightness(3)).setCategory(CreativeItemCategory::Decorations).setNameId("cauldron").addBlockState(BlockState::FillLevel, 3);

	mEndPortal = &registerBlock<EndPortalBlock>("end_portal", 119).setDestroyTime(-1).setExplodeable(6000000).setCategory(CreativeItemCategory::None);
	mEndPortalFrame = &registerBlock<EndPortalFrameBlock>("end_portal_frame", 120).setMapColor(Color::fromARGB(0x667F33)).setDestroyTime(-1).setExplodeable(6000000).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::EndPortalEyeBit, 1);

	mEndStone = &registerBlock<Block>("end_stone", 121, Material::getMaterial(MaterialType::Stone)).setDestroyTime(3.0f).setExplodeable(15).setCategory(CreativeItemCategory::BuildingBlocks);
	mUnlitRedStoneLamp = &registerBlock<RedstoneLampBlock>("redstone_lamp", 123, false).setDestroyTime(0.3f).setCategory(CreativeItemCategory::Tools);
	mLitRedStoneLamp = &registerBlock<RedstoneLampBlock>("lit_redstone_lamp", 124, true).setDestroyTime(0.3f);
	mCocoa = &registerBlock<CocoaBlock>("cocoa", 127).setDestroyTime(0.2f).setExplodeable(5).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::Age, 2);

	mEmeraldOre = &registerBlock<OreBlock>("emerald_ore", 129).setCategory(CreativeItemCategory::BuildingBlocks).setDestroyTime(3.0f).setExplodeable(5);
	mEmeraldBlock = &registerBlock<MetalBlock>("emerald_block", 133).setMapColor(Color::fromARGB(0x00D93A)).setCategory(CreativeItemCategory::Decorations).setDestroyTime(5.0f).setExplodeable(10);

	mSpruceStairs = &registerBlock<StairBlock>("spruce_stairs", 134, *mWoodPlanks, 1).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::UpsideDownBit, 1);
	mBirchStairs = &registerBlock<StairBlock>("birch_stairs", 135, *mWoodPlanks, 2).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::UpsideDownBit, 1);
	mJungleStairs = &registerBlock<StairBlock>("jungle_stairs", 136, *mWoodPlanks, 3).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::UpsideDownBit, 1);

	mBeacon = &registerBlock<BeaconBlock>("beacon", 138).setDestroyTime(3.0f).setLightEmission(15.0f / 16.0f).setCategory(CreativeItemCategory::Decorations).addProperty(BlockProperty::Immovable);

	mWoodButton = &registerBlock<WoodButtonBlock>("wooden_button", 143).setDestroyTime(0.5f).setCategory(CreativeItemCategory::Tools).addBlockState(BlockState::FacingDirection, 3).addBlockState(BlockState::ButtonPressedBit, 1);
	
	mStoneBrickStairs = &registerBlock<StairBlock>("stone_brick_stairs", 109, *Block::mStoneBrick).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::UpsideDownBit, 1);
	mNetherBrick = &registerBlock<Block>("nether_brick", 112, Material::getMaterial(MaterialType::Stone)).setDestroyTime(2.0f).setExplodeable(10).setCategory(CreativeItemCategory::BuildingBlocks).setBrightnessGamma(2.f);
	mNetherBrickStairs = &registerBlock<StairBlock>("nether_brick_stairs", 114, *Block::mNetherBrick).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::UpsideDownBit, 1);
	mNetherWart = &registerBlock<NetherWartBlock>("nether_wart", 115).setMapColor(Color::fromARGB(0x993333)).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::Age, 2);
	mEnchantingTable = &registerBlock<EnchantingTableBlock>("enchanting_table", 116).setMapColor(Color::fromARGB(0x993333)).setExplodeable(2000).setCategory(CreativeItemCategory::Decorations);

	mDragonEgg = &registerBlock<DragonEggBlock>("dragon_egg", 122).setDestroyTime(3.0f).setCategory(CreativeItemCategory::Decorations).setLightEmission(2.0f / 16.0f);

	// Switched places from PC Minecraft
	mDropper = &registerBlock<DropperBlock>("dropper", 125).setDestroyTime(3.5f).setCategory(CreativeItemCategory::Tools).addBlockState(BlockState::FacingDirection, 3).addBlockState(BlockState::TriggeredBit, 1);
	mActivatorRail = &registerBlock<ActivatorRailBlock>("activator_rail", 126).setDestroyTime(.5f).setCategory(CreativeItemCategory::Tools).addBlockState(BlockState::RailDirection, 3).addBlockState(BlockState::RailDataBit, 1);
	mSandstoneStairs = &registerBlock<StairBlock>("sandstone_stairs", 128, *Block::mSandStone).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::UpsideDownBit, 1);

	mEnderChest = &registerBlock<EnderChestBlock>("ender_chest", 130).setDestroyTime(22.5f).setExplodeable(1000).setLightEmission(0.5f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::FacingDirection, 3);
	mTripwireHook = &registerBlock<TripWireHookBlock>("tripwire_hook", 131).setDestroyTime(0.0f).setCategory(CreativeItemCategory::Tools).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::AttachedBit, 1).addBlockState(BlockState::PoweredBit, 1);
	mTripwire = &registerBlock<TripWireBlock>("tripWire", 132).setDestroyTime(0.0f).setCategory(CreativeItemCategory::Tools).addBlockState(BlockState::PoweredBit, 1).addBlockState(BlockState::SuspendedBit, 1).addBlockState(BlockState::AttachedBit, 1).addBlockState(BlockState::DisarmedBit, 1);

	mCobblestoneWall = &registerBlock<WallBlock>("cobblestone_wall", 139, *mCobblestone).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::MappedType, 1);
	mFlowerPot = &registerBlock<FlowerPotBlock>("flower_pot", 140).setDestroyTime(0.0f).setCategory(CreativeItemCategory::Decorations);
	mCarrotCrop = &registerBlock<CarrotBlock>("carrots", 141).setDestroyTime(0.0f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::Growth, 3);
	mPotatoCrop = &registerBlock<PotatoBlock>("potatoes", 142).setDestroyTime(0.0f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::Growth, 3);

	mSkull = &registerBlock<SkullBlock>("skull", 144).setDestroyTime(1.0f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::FacingDirection, 3).addBlockState(BlockState::NoDropBit, 1).resetBitsUsed().addBlockState(BlockState::MappedType, 3);
	mAnvil = &registerBlock<AnvilBlock>("anvil", 145, Material::getMaterial(MaterialType::Metal)).setDestroyTime(5.0f).setLightBlock(Brightness(3)).setCategory(CreativeItemCategory::Decorations).setExplodeable(2000).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::Damage, 2);
	mTrappedChest = &registerBlock<ChestBlock>("trapped_chest", 146, ChestBlock::TYPE_TRAP).setCategory(CreativeItemCategory::Decorations).setDestroyTime(2.5f).addBlockState(BlockState::FacingDirection, 3);

	mLightWeightedPressurePlate = &registerBlock<WeightedPressurePlateBlock>("light_weighted_pressure_plate", 147, Material::getMaterial(MaterialType::Metal), 15).setDestroyTime(0.5f).setCategory(CreativeItemCategory::Tools).addBlockState(BlockState::RedstoneSignal, 4);
	mHeavyWeightedPressurePlate = &registerBlock<WeightedPressurePlateBlock>("heavy_weighted_pressure_plate", 148, Material::getMaterial(MaterialType::Metal), 15 * 10).setDestroyTime(0.5f).setCategory(CreativeItemCategory::Tools).addBlockState(BlockState::RedstoneSignal, 4);

	mUnpoweredComparator = &registerBlock<ComparatorBlock>("unpowered_comparator", 149, false).setDestroyTime(0.0f).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::OutputSubtractBit, 1).addBlockState(BlockState::OutputLitBit, 1);
	mPoweredComparator = &registerBlock<ComparatorBlock>("powered_comparator", 150, true).setDestroyTime(0.0f).setLightEmission(8.0f / 16.0f).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::OutputSubtractBit, 1).addBlockState(BlockState::OutputLitBit, 1);

	mDaylightDetector = &registerBlock<DaylightDetectorBlock>("daylight_detector", 151, false).setDestroyTime(0.2f).setCategory(CreativeItemCategory::Tools).addBlockState(BlockState::RedstoneSignal, 4);
	mRedstoneBlock = &registerBlock<RedstoneBlock>("redstone_block", 152).setMapColor(Color::fromARGB(0xff0000)).setCategory(CreativeItemCategory::Decorations).setDestroyTime(5.0f).setExplodeable(10);
	mQuartzOre = &registerBlock<OreBlock>("quartz_ore", 153).setMapColor(Color::fromARGB(0x700200)).setDestroyTime(3.0f).setExplodeable(5).setCategory(CreativeItemCategory::BuildingBlocks);
	
	mHopper = &registerBlock<HopperBlock>("hopper", 154).setDestroyTime(3.0f).setLightBlock(Brightness(3)).setExplodeable(8).setCategory(CreativeItemCategory::Tools).addBlockState(BlockState::FacingDirection, 3).addBlockState(BlockState::ToggleBit, 1);
	
	mQuartzBlock = &registerBlock<QuartzBlockBlock>("quartz_block", 155).setDestroyTime(0.8f).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::MappedType, 2).addBlockState(BlockState::Direction, 2);
	mQuartzStairs = &registerBlock<StairBlock>("quartz_stairs", 156, *Block::mQuartzBlock).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::UpsideDownBit, 1);
	mDoubleWoodenSlab = &registerBlock<WoodSlabBlock>("double_wooden_slab", 157, true).setDestroyTime(2.0f).setExplodeable(10).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::MappedType, 3).addBlockState(BlockState::TopSlotBit, 1);
	mWoodenSlab = &registerBlock<WoodSlabBlock>("wooden_slab", 158, false).setDestroyTime(2.0f).setExplodeable(10).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::MappedType, 3).addBlockState(BlockState::TopSlotBit, 1);
	mStainedClay = &registerBlock<ColoredBlock>("stained_hardened_clay", 159, Material::getMaterial(MaterialType::Stone)).setDestroyTime(1.25f).setExplodeable(7).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::MappedType, 4);
	
	// Had to assign a new id to stained_glass as invisibleBedrock used its original one
	//mStainedGlass = &registerBlock<StainedGlassBlock>("stained_glass", 241, Material::getMaterial(MaterialType::Glass), false).setDestroyTime(0.3f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::MappedType, 4);
	//mStainedGlassPane = &registerBlock<StainedGlassPaneBlock>("stained_glass_pane", 160, Material::getMaterial(MaterialType::Glass), false).setDestroyTime(0.3f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::MappedType, 4);
	mStainedGlass = &registerBlock<GlassBlock>("stained_glass", 241, Material::getMaterial(MaterialType::Glass), false).setDestroyTime(0.3f).setCategory(CreativeItemCategory::Decorations);
	mStainedGlassPane = &registerBlock<ThinFenceBlock>("stained_glass_pane", 160, Material::getMaterial(MaterialType::Glass), false).setDestroyTime(0.3f).setCategory(CreativeItemCategory::Decorations);

	mLeaves2 = &registerBlock<NewLeafBlock>("leaves2", 161).setDestroyTime(0.2f).setLightBlock(Brightness(1)).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::MappedType, 2).addBlockState(BlockState::UpdateBit, 1).addBlockState(BlockState::PersistentBit, 1);
	mLog2 = &registerBlock<NewLogBlock>("log2", 162).setDestroyTime(2.0f).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::MappedType, 2).addBlockState(BlockState::Direction, 2);

	mAcaciaStairs = &registerBlock<StairBlock>("acacia_stairs", 163, *mWoodPlanks, 4).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::UpsideDownBit, 1);
	mDarkOakStairs = &registerBlock<StairBlock>("dark_oak_stairs", 164, *mWoodPlanks, 5).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::UpsideDownBit, 1);
	mSlimeBlock = &registerBlock<SlimeBlock>("slime", 165, Material::getMaterial(MaterialType::Slime)).setLightBlock(Brightness::MIN).setCategory(CreativeItemCategory::Decorations);

	mIronTrapdoor = &registerBlock<TrapDoorBlock>("iron_trapdoor", 167, Material::getMaterial(MaterialType::Metal)).setDestroyTime(5.0f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::UpsideDownBit, 1).addBlockState(BlockState::OpenBit, 1);
	mPrismarine = &registerBlock<PrismarineBlock>("prismarine", 168).setDestroyTime(1.5f).setExplodeable(10).setCategory(CreativeItemCategory::BuildingBlocks);
	mSeaLantern = &registerBlock<SeaLanternBlock>("seaLantern", 169).setDestroyTime(0.3f).setExplodeable(0.5f).setCategory(CreativeItemCategory::Decorations).setLightEmission(1.0f);
	mHayBlock = &registerBlock<HayBlockBlock>("hay_block", 170).setMapColor(Color::fromARGB(0xE5E533)).setDestroyTime(0.5f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::MappedType, 2).addBlockState(BlockState::Direction, 2);
	mWoolCarpet = &registerBlock<WoolCarpetBlock>("carpet", 171).setDestroyTime(0.1f).setCategory(CreativeItemCategory::Decorations).setLightBlock(Brightness::MIN);

	mHardenedClay = &registerBlock<Block>("hardened_clay", 172, Material::getMaterial(MaterialType::Stone)).setMapColor(Color::fromARGB(0xD87F33)).setDestroyTime(1.25f).setExplodeable(7).setCategory(CreativeItemCategory::BuildingBlocks);
	mCoalBlock = &registerBlock<MetalBlock>("coal_block", 173).setMapColor(Color::fromARGB(0x191919)).setDestroyTime(5.0f).setExplodeable(10).setCategory(CreativeItemCategory::Decorations);
	mPackedIce = &registerBlock<IceBlock>("packed_ice", 174, true).setDestroyTime(0.5f)/*setLightBlock(Brightness(15).*/ .setCategory(CreativeItemCategory::BuildingBlocks);
	mDoublePlant = &registerBlock<DoublePlantBlock>("double_plant", 175).setDestroyTime(0).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::MappedType, 3).addBlockState(BlockState::UpperBlockBit, 1).resetBitsUsed().addBlockState(BlockState::Direction, 2);

	mDaylightDetectorInverted = &registerBlock<DaylightDetectorBlock>("daylight_detector_inverted", 178, true).setDestroyTime(0.2f).setCategory(CreativeItemCategory::Tools).addBlockState(BlockState::RedstoneSignal, 4);
	mRedSandstone = &registerBlock<SandStoneBlock>("red_sandstone", 179).setDestroyTime(0.8f).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::MappedType, 2);
	mRedSandstoneStairs = &registerBlock<StairBlock>("red_sandstone_stairs", 180, *Block::mRedSandstone).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::UpsideDownBit, 1);
	mDoubleStoneSlab = &registerBlock<StoneSlabBlock>("double_stone_slab", 43, true).setDestroyTime(2.0f).setExplodeable(10).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::MappedType, 3).addBlockState(BlockState::TopSlotBit, 1);
	mStoneSlab = &registerBlock<StoneSlabBlock>("stone_slab", 44, false).setDestroyTime(2.0f).setExplodeable(10).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::MappedType, 3).addBlockState(BlockState::TopSlotBit, 1);
	mDoubleStoneSlab2 = &registerBlock<StoneSlabBlock2>("double_stone_slab2", 181, true).setDestroyTime(2.0f).setExplodeable(10).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::MappedType, 3).addBlockState(BlockState::TopSlotBit, 1);
	mStoneSlab2 = &registerBlock<StoneSlabBlock2>("stone_slab2", 182, false).setDestroyTime(2.0f).setExplodeable(10).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::MappedType, 3).addBlockState(BlockState::TopSlotBit, 1);

	mSpuceFenceGate = &registerBlock<FenceGateBlock>("spruce_fence_gate", 183, WoodBlockType::Spruce).setMapColor(WoodBlock::getWoodMaterialColor(WoodBlockType::Spruce)).setDestroyTime(2.0f).setExplodeable(5).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::OpenBit, 1).addBlockState(BlockState::InWallBit, 1);
	mBirchFenceGate = &registerBlock<FenceGateBlock>("birch_fence_gate", 184, WoodBlockType::Birch).setMapColor(WoodBlock::getWoodMaterialColor(WoodBlockType::Birch)).setDestroyTime(2.0f).setExplodeable(5).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::OpenBit, 1).addBlockState(BlockState::InWallBit, 1);
	mJungleFenceGate = &registerBlock<FenceGateBlock>("jungle_fence_gate", 185, WoodBlockType::Jungle).setMapColor(WoodBlock::getWoodMaterialColor(WoodBlockType::Jungle)).setDestroyTime(2.0f).setExplodeable(5).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::OpenBit, 1).addBlockState(BlockState::InWallBit, 1);
	mDarkOakFenceGate = &registerBlock<FenceGateBlock>("dark_oak_fence_gate", 186, WoodBlockType::Big_Oak).setMapColor(WoodBlock::getWoodMaterialColor(WoodBlockType::Big_Oak)).setDestroyTime(2.0f).setExplodeable(5).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::OpenBit, 1).addBlockState(BlockState::InWallBit, 1);
	mAcaciaFenceGate = &registerBlock<FenceGateBlock>("acacia_fence_gate", 187, WoodBlockType::Acacia).setMapColor(WoodBlock::getWoodMaterialColor(WoodBlockType::Acacia)).setDestroyTime(2.0f).setExplodeable(5).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::OpenBit, 1).addBlockState(BlockState::InWallBit, 1);

	mWoodenDoorSpruce = &registerBlock<DoorBlock>("spruce_door", 193, Material::getMaterial(MaterialType::Wood), DoorBlock::SPRUCE).setDestroyTime(3.0f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::OpenBit, 1).addBlockState(BlockState::UpperBlockBit, 1).resetBitsUsed().addBlockState(BlockState::DoorHingeBit, 1);
	mWoodenDoorBirch = &registerBlock<DoorBlock>("birch_door", 194, Material::getMaterial(MaterialType::Wood), DoorBlock::BIRCH).setDestroyTime(3.0f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::OpenBit, 1).addBlockState(BlockState::UpperBlockBit, 1).resetBitsUsed().addBlockState(BlockState::DoorHingeBit, 1);
	mWoodenDoorJungle = &registerBlock<DoorBlock>("jungle_door", 195, Material::getMaterial(MaterialType::Wood), DoorBlock::JUNGLE).setDestroyTime(3.0f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::OpenBit, 1).addBlockState(BlockState::UpperBlockBit, 1).resetBitsUsed().addBlockState(BlockState::DoorHingeBit, 1);
	mWoodenDoorAcacia = &registerBlock<DoorBlock>("acacia_door", 196, Material::getMaterial(MaterialType::Wood), DoorBlock::ACACIA).setDestroyTime(3.0f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::OpenBit, 1).addBlockState(BlockState::UpperBlockBit, 1).resetBitsUsed().addBlockState(BlockState::DoorHingeBit, 1);
	mWoodenDoorDarkOak = &registerBlock<DoorBlock>("dark_oak_door", 197, Material::getMaterial(MaterialType::Wood), DoorBlock::DARKOAK).setDestroyTime(3.0f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::OpenBit, 1).addBlockState(BlockState::UpperBlockBit, 1).resetBitsUsed().addBlockState(BlockState::DoorHingeBit, 1);

	mGrassPathBlock = &registerBlock<GrassPathBlock>("grass_path", 198).setDestroyTime(0.65f);

	mItemFrame = &registerBlock<ItemFrameBlock>("frame", 199).setCategory(CreativeItemCategory::Decorations).setDestroyTime(0.25f).addBlockState(BlockState::Direction, 2);
	// I moved the chorus plant and it's whoever made mItemFrame's fault; with love - Venvious
	mChorusPlantBlock = &registerBlock<ChorusPlantBlock>("chorus_plant", 240).setDestroyTime(0.4f).setCategory(CreativeItemCategory::Decorations);
	mChorusFlowerBlock = &registerBlock<ChorusFlowerBlock>("chorus_flower", 200).setDestroyTime(0.4f).setCategory(CreativeItemCategory::Decorations); // A flower can't grow chorus, yes
	
	mPurpurBlock = &registerBlock<QuartzBlockBlock>("purpur_block", 201).setDestroyTime(1.5f).setCategory(CreativeItemCategory::BuildingBlocks).setExplodeable(10).addBlockState(BlockState::MappedType, 2).addBlockState(BlockState::Direction, 2);
	mPurpurStairs = &registerBlock<StairBlock>("purpur_stairs", 203, *Block::mPurpurBlock).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::Direction, 2).addBlockState(BlockState::UpsideDownBit, 1);

	mEndBrick = &registerBlock<Block>("end_bricks", 206, Material::getMaterial(MaterialType::Stone)).setDestroyTime(0.8f).setCategory(CreativeItemCategory::BuildingBlocks);
	// Was supposed to be 198 but grass_path used it. grass_path is 208 in Java so made an interchange.
	mEndRod = &registerBlock<EndRodBlock>("end_rod", 208).setDestroyTime(0.0f).setLightEmission(15.0f / 16.0f).setCategory(CreativeItemCategory::Decorations);
	mEndGateway = &registerBlock<EndGatewayBlock>("end_gateway", 209).setDestroyTime(-1).setExplodeable(6000000).setCategory(CreativeItemCategory::None);

	//mStructureVoid = &registerBlock<StructureVoid>("structure_void", 217).setCategory(CreativeItemCategory::Tools).setExplodeable(6000000);
	//mStructureBlock = &registerBlock<StructureBlock>("structure_block", 252).setCategory(CreativeItemCategory::Tools).setExplodeable(6000000);
	
	//
	// Special blocks for Pocket Edition is placed at high IDs
	//
	mPodzol = &registerBlock<PodzolBlock>("podzol", 243).setMapColor(Color::fromARGB(0x976D4D)).setDestroyTime(0.5f).setCategory(CreativeItemCategory::BuildingBlocks);
	
	mBeetrootCrop = &registerBlock<BeetrootBlock>("beetroot", 244).setDestroyTime(0.0f).setCategory(CreativeItemCategory::Decorations).addBlockState(BlockState::Growth, 3);
	mStonecutterBench = &registerBlock<StonecutterBlock>("stonecutter", 245).setDestroyTime(2.5f).setCategory(CreativeItemCategory::Decorations);
	mGlowingObsidian = &registerBlock<ObsidianBlock>("glowingobsidian", 246, true).setDestroyTime(10.0f).setLightEmission(14 / 16.0f).setExplodeable(2000).setCategory(CreativeItemCategory::None);
	mNetherReactor = &registerBlock<NetherReactorBlock>("netherreactor", 247, Material::getMaterial(MaterialType::Metal)).setDestroyTime(3.0f).setCategory(CreativeItemCategory::None);
	mInfoUpdateGame1 = &registerBlock<Block>("info_update", 248, Material::getMaterial(MaterialType::Dirt)).setDestroyTime(1.0f).setCategory(CreativeItemCategory::None);
	mInfoUpdateGame2 = &registerBlock<Block>("info_update2", 249, Material::getMaterial(MaterialType::Dirt)).setDestroyTime(1.0f).setCategory(CreativeItemCategory::None);
	mMovingBlock = &registerBlock<Block>("movingBlock", 250, Material::getMaterial(MaterialType::Dirt)).setDestroyTime(-1).setExplodeable(6000000).setCategory(CreativeItemCategory::None);
// 	mMovingBlock = &registerBlock<MovingBlock>("movingBlock", 250).setDestroyTime(-1).setExplodeable(6000000).setCategory(CreativeItemCategory::None);
	mObserver = &registerBlock<ObserverBlock>("observer", 251).setDestroyTime(3.0f).setCategory(CreativeItemCategory::Tools).addBlockState(BlockState::FacingDirection, 3);

	//leaves_carried = (const LeafBlock*) (new LeafBlock>(254, "leaves_oak").setDestroyTime(0.2f).setLightBlock(1).setCategory(ItemCategory::Structures).setNameId("leaves");
	mInfoReserved6  = &registerBlock<Block>("reserved6", 255, Material::getMaterial(MaterialType::Dirt)).setCategory(CreativeItemCategory::None);

	//
	// Stuff that need to be initialized in a specific order (i.e. after the other blocks have been created)
	//
	mFire = &registerBlock<FireBlock>("fire", 51).setDestroyTime(0.0f).setLightEmission(1.0f).setCategory(CreativeItemCategory::BuildingBlocks).addBlockState(BlockState::Age, 4);



	//add missing blocks
	//TODO start at 0, add a air block!
	for (int i = 1; i < 256; i++) {
		if(Block::mBlocks[i] == nullptr) {
			// NOTE (venvious): Using [i] as the name to match with the array in BlockGraphics.cpp
			registerBlock<Block>(Util::toString(i), i, Material::getMaterial(MaterialType::Air)).setDestroyTime(0.0f).setCategory(CreativeItemCategory::None).setBlockProperty(BlockProperty::Placeholder);;
		}
	}

	//initialize all blocks
	for (auto&& block : mOwnedBlocks) {
		block->init();
	}

	// Init TopSnowBlock Recoverable Table
	//TopSnowBlock::registerRecoverableBlock(Block::mDoublePlant->mID);
	TopSnowBlock::registerRecoverableBlock(Block::mTallgrass->mID);
	TopSnowBlock::registerRecoverableBlock(Block::mYellowFlower->mID);
	TopSnowBlock::registerRecoverableBlock(Block::mRedFlower->mID);
	TopSnowBlock::registerRecoverableBlock(Block::mBrownMushroom->mID);
	TopSnowBlock::registerRecoverableBlock(Block::mRedMushroom->mID);
}

const BlockState& Block::getBlockState(BlockState::BlockStates stateType) const {
	return mBlockStates[stateType];
}

/*static*/
void Block::teardownBlocks() {
	for (int i = 0; i < 256; ++i) {
		Block::mBlocks[i] = nullptr;
	}

	mOwnedBlocks.clear();

	mBlockLookupMap.clear();
}

BlockID Block::transformToValidBlockId(BlockID blockId) {
	return transformToValidBlockId(blockId, BlockPos(0, 0, 0));
}

BlockID Block::transformToValidBlockId(BlockID blockId, const BlockPos& pos) {
	if (blockId != 0 && Block::mBlocks[blockId] == nullptr) {
		return (((pos.x + pos.y + pos.z) & 1) == 1) ? Block::mInfoUpdateGame1->mID : Block::mInfoUpdateGame2->mID;
	}

	return blockId;
}

Block::Block(const std::string& nameId, int id, const Material& material)
	: mID(id)
	, mDescriptionId(nameId.empty() ? "" : BLOCK_DESCRIPTION_PREFIX + nameId)
	, mRawNameId(nameId)
	, mMaterial(material)
	, mMapColor(material.getColor())
	, mProperties(BlockProperty::CubeShaped)
	, mBitsUsed(0) {
	DEBUG_ASSERT(!Block::mBlocks[id], "Slot is already occupied");

	mTranslucency[id] = material.getTranslucency();
	setSolid(true);
}

Block::~Block() {
#ifdef ENABLE_DEBUG_RENDERING
	if (this == DebugRenderer::getDebugBlock()) {
		DebugRenderer::clearDebugBlock();
	}
#endif
}

bool Block::canSlide() const {
	return mCanSlide;
}

bool Block::canInstatick() const {
	return mCanInstatick;
}

CreativeItemCategory Block::getCreativeCategory() const {
	return mCreativeCategory;
}

float Block::getGravity() const {
	return mGravity;
}

const Material& Block::getMaterial() const {
	return mMaterial;
}

Color Block::getMapColor(BlockSource&, const BlockPos&) const {
	return mMapColor;
}

Color Block::getMapColor() const {
	return mMapColor;
}

float Block::getFriction() const {
	return mFriction;
}

float Block::getDestroySpeed() const {
	return mDestroySpeed;
}

void Block::tick(BlockSource& region, const BlockPos& pos, Random& random) const {
}

void Block::animateTick(BlockSource& region, const BlockPos& pos, Random& random) const {
}

bool Block::isType(const Block* compare) const {
	return this == compare;
}

Block& Block::setLightEmission(float f) {
	Block::mLightEmission[mID] = (Brightness)(int)(Brightness::MAX * f);
	return *this;
}

Block& Block::init() {
	auto& translucent = mTranslucency[mID];

	if (translucent < 0) {	//was not overridden in the constructor
		translucent = mMaterial.getTranslucency();
	}

	return *this;
}

HitResult Block::clip(BlockSource& region, const BlockPos& pos, const Vec3& A, const Vec3& B, bool isClipping, int clipData,const AABB& aaBB) const {
	Vec3 sub(pos);

	AABB shape =  aaBB.translated(-sub);

	Vec3 a = A - sub;//a.add((float)-xt, (float)-yt, (float)-zt);
	Vec3 b = B - sub;//b.add((float)-xt, (float)-yt, (float)-zt);

	Vec3 xh0, xh1, yh0, yh1, zh0, zh1;

	bool bxh0 = a.clipX(b, shape.min.x, xh0);
	bool bxh1 = a.clipX(b, shape.max.x, xh1);

	bool byh0 = a.clipY(b, shape.min.y, yh0);
	bool byh1 = a.clipY(b, shape.max.y, yh1);

	bool bzh0 = a.clipZ(b, shape.min.z, zh0);
	bool bzh1 = a.clipZ(b, shape.max.z, zh1);

	//if (!containsX(xh0)) xh0 = nullptr;
	if(!bxh0 || !shape.containsX(xh0)) {
		bxh0 = false;
	}
	if(!bxh1 || !shape.containsX(xh1)) {
		bxh1 = false;
	}
	if(!byh0 || !shape.containsY(yh0)) {
		byh0 = false;
	}
	if(!byh1 || !shape.containsY(yh1)) {
		byh1 = false;
	}
	if(!bzh0 || !shape.containsZ(zh0)) {
		bzh0 = false;
	}
	if(!bzh1 || !shape.containsZ(zh1)) {
		bzh1 = false;
	}
	Vec3* closest = nullptr;

	//if (xh0 != nullptr && (closest == nullptr || a.distanceToSqr(xh0) < a.distanceToSqr(closest))) closest = xh0;
	if(bxh0 && (closest == nullptr || a.distanceToSqr(xh0) < a.distanceToSqr(*closest))) {
		closest = &xh0;
	}
	if(bxh1 && (closest == nullptr || a.distanceToSqr(xh1) < a.distanceToSqr(*closest))) {
		closest = &xh1;
	}
	if(byh0 && (closest == nullptr || a.distanceToSqr(yh0) < a.distanceToSqr(*closest))) {
		closest = &yh0;
	}
	if(byh1 && (closest == nullptr || a.distanceToSqr(yh1) < a.distanceToSqr(*closest))) {
		closest = &yh1;
	}
	if(bzh0 && (closest == nullptr || a.distanceToSqr(zh0) < a.distanceToSqr(*closest))) {
		closest = &zh0;
	}
	if(bzh1 && (closest == nullptr || a.distanceToSqr(zh1) < a.distanceToSqr(*closest))) {
		closest = &zh1;
	}

	if(closest == nullptr) {
		return HitResult(B);
	}

	FacingID face = -1;

	if(closest == &xh0) {
		face = 4;
	}
	if(closest == &xh1) {
		face = 5;
	}
	if(closest == &yh0) {
		face = 0;
	}
	if(closest == &yh1) {
		face = 1;
	}
	if(closest == &zh0) {
		face = 2;
	}
	if(closest == &zh1) {
		face = 3;
	}

	return HitResult(pos, face, *closest + sub);
}

HitResult Block::clip(BlockSource& region, const BlockPos& pos, const Vec3& A, const Vec3& B, bool isClipping, int clipData) const {
	AABB tmpAABB;
	getAABB(region, pos, tmpAABB, clipData, true, clipData);

	return clip(region, pos, A, B, isClipping, clipData, tmpAABB);
}

void Block::spawnResources(BlockSource& region, const BlockPos& pos, int data, float odds, int bonusLootLevel) const {
// 	auto& level = region.getLevel();
// 	if (level.isClientSide()) {
// 		return;
// 	}
// 
// 	int count = getResourceCount(level.getRandom(), data, bonusLootLevel);
// 
// 	for (int i = 0; i < count; i++) {
// 		if (level.getRandom().nextFloat() > odds) {
// 			continue;
// 		}
// 		int type = getResource(level.getRandom(), data, bonusLootLevel);
// 		if (type <= 0) {
// 			continue;
// 		}
// 		const float s = 0.7f;
// 		const float xo = level.getRandom().nextFloat() * s + (1 - s) * 0.5f;
// 		const float yo = level.getRandom().nextFloat() * s + (1 - s) * 0.5f;
// 		const float zo = level.getRandom().nextFloat() * s + (1 - s) * 0.5f;
// 		level.getSpawner().spawnItem(region, ItemInstance(type, 1, getSpawnResourcesAuxValue(data)), nullptr, Vec3(pos.x + xo, pos.y + yo, pos.z + zo), 10);
// 	}
// 
// 	ExperienceOrb::spawnOrbs(region, Vec3(pos), getExperienceDrop(level.getRandom()));
}

void Block::popResource(BlockSource& region, const BlockPos& pos, const ItemInstance& itemInstance) const {
// 	auto& level = region.getLevel();
// 	// TODO: Check Game Rules for block drops once it is implemented
// 	if (level.isClientSide()) {
// 		return;
// 	}
// 
// 	const float s = 0.7f;
// 	const float xo = level.getRandom().nextFloat() * s + (1 - s) * 0.5f;
// 	const float yo = level.getRandom().nextFloat() * s + (1 - s) * 0.5f;
// 	const float zo = level.getRandom().nextFloat() * s + (1 - s) * 0.5f;
// 
// 	region.getLevel().getSpawner().spawnItem(region, itemInstance, nullptr, Vec3(pos.x + xo, pos.y + yo, pos.z + zo), 10);
}

bool Block::spawnBurnResources(BlockSource& region, float x, float y, float z){
	return false;
}

void Block::destroy( BlockSource& region, const BlockPos& pos, int data, Entity* entitySource) const {
}

bool Block::playerWillDestroy(Player& player, const BlockPos& pos, int data) const {
	player.getRegion().setBlockAndData(pos, FullBlock::AIR, Block::UPDATE_ALL, &player);	// and will be replaced by air
	return true;
}

bool Block::shouldRenderFace(BlockSource& region, const BlockPos& pos, FacingID face, const AABB& shape) const {
	// Common
	if ((face == Facing::DOWN && shape.min.y > 0) ||
		(face == Facing::UP && shape.max.y < 1) ||
		(face == Facing::NORTH && shape.min.z > 0) ||
		(face == Facing::SOUTH && shape.max.z < 1) ||
		(face == Facing::WEST && shape.min.x > 0) ||
		(face == Facing::EAST && shape.max.x < 1)) {
		return true;
	}

	const auto& fullBlockData = region.getBlockAndData( pos );
	const Block* t = Block::mBlocks[fullBlockData.id];
	if (!t) {
		return true;
	}

	if (t == mInvisibleBedrock) {	//this hack is needed because unbreakable is not solid but we don't want it to render
		return false;	//we should really have two different flags for "logically solid" and "fully opaque", leaves are not solid pls
	}

	//HACK this code most definitely doesn't belong here
	if (t->hasProperty(BlockProperty::Leaf)) {
		return !LeafBlock::isDeepLeafBlock(region, pos );
	}

	return (face == Facing::UP && (t == mTopSnow || t == mWoolCarpet)) ? false : !t->isSolid();
}

Vec3 Block::randomlyModifyPosition(const BlockPos& pos, int32_t& seed) const {
	seed = 0;
	return randomlyModifyPosition(pos);
}

Vec3 Block::randomlyModifyPosition(const BlockPos& pos) const {
	return pos;
}

bool Block::addCollisionShapes(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes, Entity* entity) const {
	AABB tmpAABB;
	getCollisionShape(tmpAABB, region, pos, entity);
	return addAABB(tmpAABB, intersectTestBox, inoutBoxes);
}

void Block::addAABBs( BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes ) const {
	AABB tmpAABB;
	addAABB(getAABB(region, pos, tmpAABB), intersectTestBox, inoutBoxes);
}

bool Block::addAABB(const AABB& shape, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes) const {
	if(!shape.isEmpty() && intersectTestBox->intersects(shape)) {
		inoutBoxes.push_back(shape);
		return true;
	}

	return false;
}

bool Block::isSolid() const {
	return mSolid[mID];
}

void Block::setSolid(bool solid) {
	mSolid[mID] = solid;
	mLightBlock[mID] = solid ? Brightness::MAX : Brightness::MIN;
	mPushesOutItems[mID] = solid;
}

bool Block::pushesOutItems() const {
	return mPushesOutItems[mID];
}

void Block::setPushesOutItems(bool pushesOutItems) {
	mPushesOutItems[mID] = pushesOutItems;
}

bool Block::isUnbreakable() const {
	return mDestroySpeed < 0;
}

bool Block::isHeavy() const {
	return mHeavy;
}

bool Block::canProvideSupport(BlockSource& region, const BlockPos& pos, FacingID face, BlockSupportType type) const {
	if (face == Facing::UP) {
		if (hasProperty(BlockProperty::TopSolidBlocking)) {
			return true;
		}
		if (getMaterial().isSolidBlocking() && isSolid()) {
			return true;
		}
		int data = region.getData(pos);
		if (hasProperty(BlockProperty::Stair)) {
			return (data& StairBlock::UPSIDEDOWN_BIT) == StairBlock::UPSIDEDOWN_BIT;
		}
		if (hasProperty(BlockProperty::HalfSlab)) {
			return (data& SlabBlock::TOP_SLOT_BIT) == SlabBlock::TOP_SLOT_BIT;
		}
		if (hasProperty(BlockProperty::Connects2D)) {
			return true;
		}
		if (isType(Block::mTopSnow)) {
			return TopSnowBlock::dataIDToHeight(data) == TopSnowBlock::MAX_HEIGHT;
		}
	} else {
		return getMaterial().isSolidBlocking() && (hasProperty(BlockProperty::CubeShaped) || hasProperty(BlockProperty::SolidBlocking));
	}
	return false;
}

const AABB& Block::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	bufferValue = getVisualShape(region, pos, bufferValue, false);
	return bufferValue.move(Vec3(pos));
}

bool Block::mayPick() const {
	return true;
}

bool Block::mayPick(BlockSource& region, int data, bool liquid) const {
	return mayPick();
}

int Block::getResourceCount( Random& random, int data, int bonusLootLevel) const{
	return 1;
}

ItemInstance Block::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(this, 1, blockData);
}

int Block::getResource(Random& random, int data, int bonusLootLevel) const{
	return mID;
}

float Block::getExplosionResistance( Entity* entity ) const{
	return mExplosionResistance;
}

bool Block::use(Player& player, const BlockPos& pos) const {
	return false;
}

int Block::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	return itemValue;
}

DataID Block::calcVariant(BlockSource& region, const BlockPos& pos, DataID data) const {
	return data;
}

bool Block::isAttachedTo(BlockSource& region, const BlockPos& pos, BlockPos& outAttachedTo) const {
	return false;
}

bool Block::attack(Player* player, const BlockPos& pos) const {
	return true;
}

void Block::handleEntityInside(BlockSource& region, const BlockPos& pos, Entity* entity, Vec3& current) const {
}

int Block::getColor(int auxData) const {
	return 0xffffffff;
}

int Block::getColor(BlockSource& region, const BlockPos& pos, DataID data) const {
	return 0xffffffff;
}

int Block::getColor(BlockSource& region, const BlockPos& pos) const {
	return 0xffffffff;
}

int Block::getColorForParticle(BlockSource& region, const BlockPos& pos, int auxData) const {
	return getColor(region, pos, static_cast<DataID>(auxData));
}

bool Block::isSeasonTinted(BlockSource& region, const BlockPos& p) const {
	return false;
}

bool Block::isSignalSource() const {
	return false;
}

int Block::getDirectSignal(BlockSource& region, const BlockPos& pos, int dir) const {
	return Redstone::SIGNAL_NONE;
}

void Block::entityInside(BlockSource& region, const BlockPos& pos, Entity& entity) const {
}

void Block::playerDestroy(Player* player, const BlockPos& pos, int data) const {
// 	ItemInstance* item = player->getSelectedItem();
// 	int bonusLootLevel = 0;
// 	bool spawned = false;
// 
// 	// if item has an enchant that might effect the resource drop
// 	if (item && item->isEnchanted()) {
// 
// 		// if the item is enchanted with silk touch, this will take precedence over other resource bonus enchants
// 		if (canBeSilkTouched() && EnchantUtils::hasEnchant(Enchant::Type::MINING_SILKTOUCH, *item)) {
// 			popResource(player->getRegion(), pos, getSilkTouchItemInstance(data));
// 			spawned = true;
// 		} else {
// 			// only consider a loot enchant if silk touch wasn't already present
// 			bonusLootLevel = EnchantUtils::getEnchantLevel(Enchant::Type::MINING_LOOT, *item);
// 		}
// 	}
// 
// 	// only if silk touch didn't already take effect
// 	if (!spawned) {
// 		spawnResources(player->getRegion(), pos, data, 1.0f, bonusLootLevel);
// 	}
// 
// 	player->causeFoodExhaustion(FoodConstants::EXHAUSTION_MINE);
// 	//player.awardStat(Stats.blockMined[id], 1);
}

ItemInstance Block::getSilkTouchItemInstance(DataID data) const {
	return ItemInstance(mID, 1, getSpawnResourcesAuxValue(data));
}

bool Block::canSurvive(BlockSource& region, const BlockPos& pos) const{
	return true;
}

int Block::getExperienceDrop(Random&) const {
	return 0;
}

bool Block::canBeBuiltOver(BlockSource& region, const BlockPos& pos) const {
	return mCanBuildOver;
}

Block& Block::setNameId(const std::string& id) {
	mDescriptionId = BLOCK_DESCRIPTION_PREFIX + id;
	return *this;
}

const std::string & Block::getDescriptionId() const
{
	return mDescriptionId;
}

std::string Block::buildDescriptionName(DataID data) const {
	return I18n::get(mDescriptionId + ".name");
}

void Block::triggerEvent(BlockSource& region, const BlockPos& pos, int b0, int b1) const {
}

Block& Block::setLightBlock( Brightness i ){
	mLightBlock[mID] = i;
	return *this;
}

Block& Block::setExplodeable( float explosionResistance ){
	mExplosionResistance = explosionResistance * 3;
	return *this;
}

Block& Block::setDestroyTime( float destroySpeed ){
	mDestroySpeed = destroySpeed;
	if (mExplosionResistance < destroySpeed * 5) {
		mExplosionResistance = destroySpeed * 5;
	}

	return *this;
}

Block& Block::setFriction(float f){
	mFriction = f;
	return *this;
}

Block& Block::setBlockProperty(BlockProperty property) {
	mProperties = mProperties | property;
	return *this;
}

void Block::setTicking(bool tick) {
	mShouldTick[mID] = tick;
}


Block& Block::setMapColor(const Color& color) {
	mMapColor = color;
	return *this;
}

Block& Block::addProperty(BlockProperty type) {
	if (!hasProperty(type)) {
		mProperties = mProperties | type;
	}
	return *this;
}

Block& Block::addBlockState(BlockState::BlockStates state, int numBits) {
	mBlockStates[state].initState(mBitsUsed, numBits);
	return *this;
}

Block& Block::resetBitsUsed() {
	mBitsUsed = 0;
	return *this;
}

int Block::getVariant(int data) const {
	return data;
}

FacingID Block::getMappedFace(FacingID face, int data) const{
	return face;
}

DataID Block::getSpawnResourcesAuxValue(DataID data) const {
	return 0;
}

void Block::setVisualShape(const Vec3& min, const Vec3& max) {
	mVisualShape.set(min, max);
}

void Block::setVisualShape(const AABB& shape) {
	mVisualShape.set(shape);
}

bool Block::mayPlaceOn(const Block& block) const {
	return true;
}

bool Block::mayPlace(BlockSource& region, const BlockPos& pos) const {
	return pos.y < region.getMaxHeight() && pos.y >= 0 && region.getMaterial(pos).isReplaceable() && mayPlaceOn(region.getBlock(pos.below()));
}

bool Block::mayPlace(BlockSource& region, const BlockPos& pos, FacingID face) const {
	return mayPlace(region, pos);
}

bool Block::tryToPlace(BlockSource& region, const BlockPos& pos, DataID data) const {
	if (region.mayPlace(mID, pos, 1, nullptr, true)) {
		region.setBlockAndData(pos, {mID, data}, Block::UPDATE_ALL);
		return true;
	}

	return false;
}

bool Block::breaksFallingBlocks(int data) const {
	return !isSolid();
}

Block& Block::setCategory(CreativeItemCategory creativeCategory) {
	mCreativeCategory = creativeCategory;
	return *this;
}

void Block::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
}


bool Block::getSecondPart(BlockSource& region, const BlockPos& pos, BlockPos& out) const {
	return false;
}

void Block::onGraphicsModeChanged(bool fancy, bool preferPolyTessellation, bool transparentLeaves) {
	mFancy = fancy;
}

void Block::DEPRECATEDcallOnGraphicsModeChanged(bool fancy, bool preferPolyTessellation, bool transparentLeaves) {
	// TODO: Calling non-const function this way is ugly, don't do it. This function should be moved to BlockGraphics
	for (auto&& block : Block::mOwnedBlocks) {
		if (block) {
			block->onGraphicsModeChanged(fancy, preferPolyTessellation, transparentLeaves);
		}
	}
}

bool Block::checkIsPathable(Entity& entity, const BlockPos& lastPathPos, const BlockPos& pathPos) const {
	return !mMaterial.getBlocksMotion();
}

bool Block::dispense(BlockSource& region, Container& container, int slot, const Vec3& pos, FacingID face) const {
	return false;
}

void Block::onPlace(BlockSource& region, const BlockPos& pos) const {
}

void Block::onRemove(BlockSource& region, const BlockPos& pos) const {
// 	if (!region.getLevel().isClientSide()) {
// 		region.getDimension().getCircuitSystem().removeComponents(pos);
// 	}
}

void Block::onExploded(BlockSource& region, const BlockPos& pos, Entity* entitySource) const {
}

void Block::onStepOn(Entity& entity, const BlockPos& pos) const {
}

void Block::onFallOn(BlockSource& region, const BlockPos& pos, Entity* entity, float fallDistance) const {
// 	if (region.getLevel().getGameRules().getBool(GameRules::FALL_DAMAGE)) {
// 		entity->causeFallDamage(fallDistance);
// 	}
}

void Block::updateEntityAfterFallOn(Entity& entity) const {
	entity.mPosDelta.y = 0;
}

bool Block::onFertilized(BlockSource& region, const BlockPos& pos, Entity* entity) const {
	return false;
}

bool Block::mayConsumeFertilizer(BlockSource& region) const {
	return true;
}

int Block::getIconYOffset() const {
	return 0;
}

const MobSpawnerData* Block::getTypeToSpawn(BlockSource& region, EntityType category, const BlockPos& pos) const {
	const MobList* mobList = nullptr;

	// first ask the ChunkSource for feature specific mobs, if none then default to Biome mobs.
	const MobList& mobs = region.getMobsAt(category, pos);
	if (mobs.size()) {
		mobList = &mobs;
	} else {
		mobList = &region.getBiome(pos).getMobs(category);
	}

	DEBUG_ASSERT(mobList, "MobList must be set.");
	return WeighedRandom::getRandomItem(&(region.getLevel().getRandom()), *mobList);
}

bool Block::canBeSilkTouched() const {
	return true;
}

const MobSpawnerData* Block::getMobToSpawn(BlockSource& region, const BlockPos& pos) const {
	return (region.getBrightness(pos) <= 8) ? getTypeToSpawn(region, EntityType::Monster, pos) : nullptr;
}

const MobSpawnerData* Block::getMobToSpawn(BlockSource& region, const BlockPos& pos, std::map<EntityType, int> priorityMobs, bool& surpassLimit) const {
	if (region.getBrightness(pos) <= 8) {
		const MobList& mobs = region.getMobsAt(EntityType::Monster, pos);
		surpassLimit = false;
		if (mobs.size()) {
			for (auto it = mobs.begin(); it != mobs.end(); ++it) {
				if (it->featureMob) {
					for (auto it2 = priorityMobs.begin(); it2 != priorityMobs.end(); ++it2) {
						if (it->mobClassId == it2->first && it2->second <= 0) {
							//we found a priorityMob and have none currently in the area
							//so in a special case we can make sure we have at least one spawned
							surpassLimit = true;
							return &(*it);
						}
					}
				}
			}
		}
		return getMobToSpawn(region, pos);
	}
	return nullptr;
}

bool Block::isAlphaTested() const {
	auto rl = mRenderLayer;
	return rl == RENDERLAYER_ALPHATEST || rl == RENDERLAYER_ALPHATEST_SINGLE_SIDE || (mFancy && rl == RENDERLAYER_OPTIONAL_ALPHATEST) || (mFancy && rl == RENDERLAYER_SEASONS_OPTIONAL_ALPHATEST);
}

bool Block::isSolidBlockingBlock() const {
	return mMaterial.isSolidBlocking() && (hasProperty(BlockProperty::CubeShaped) || hasProperty(BlockProperty::SolidBlocking));
}

bool Block::isInfiniburnBlock(int data) const {
	return hasProperty(BlockProperty::InfiniBurn);
}

bool Block::isCropBlock() const {
	return false;
}

bool Block::isContainerBlock() const {
	return false;
}

bool Block::isCraftingBlock() const {
	return false;
}

bool Block::isInteractiveBlock() const {
	return false;
}

bool Block::isWaterBlocking() const {
	if (getThickness() > 0) {	//blocks with a "thickness" are floor-covers and can be removed by water
		return false;
	}

	const Material& m = getMaterial();

	if (m.getBlocksMotion()) {
		return true;
	}

	if (m.isSolid()) {
		return true;
	}

	return false;
}

bool Block::isHurtableBlock() const {
	return false;
}

bool Block::isFenceBlock() const {
	return false;
}

bool Block::isStairBlock() const {
	return false;
}

bool Block::isRailBlock() const {
	return false;
}

bool Block::canHurtAndBreakItem() const {

	// Flowers and other 'weak' one hit blocks always have a destroy speed of 0.0 or less.
	// For 1 hit blocks, we should not hurt/break items.
	if (mDestroySpeed <= 0.0f) {
		return false;
	}
	return true;
}

BlockEntityType Block::getBlockEntityType() const {
	return mBlockEntityType;	//no block entity
}

bool Block::hasBlockEntity() const {
	return getBlockEntityType() != BlockEntityType::None;
}

BlockRenderLayer Block::getRenderLayer(BlockSource& region, const BlockPos& pos) const {
	return mRenderLayer;
}

BlockRenderLayer Block::getRenderLayer() const {
	return mRenderLayer;
}

int Block::getExtraRenderLayers() const {
	return 0;
}

float Block::getThickness() const {
	//for snow, etc
	return mThickness;
}

bool Block::isObstructingChests(BlockSource& region, const BlockPos& pos) const {
	return isSolid();
}

const AABB& Block::getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping) const {
	return getVisualShape(region.getData(pos), bufferAABB, false);
}

const AABB& Block::getVisualShape(DataID data, AABB& bufferAABB, bool isClipping	/* = false*/) const {
	return mVisualShape;
}

bool Block::getCollisionShape(AABB& outAABB, BlockSource& region, const BlockPos& pos, Entity* entity) const {
	outAABB = getAABB(region, pos, outAABB);

	// If AABB is empty then return false (we don't have a collision shape)
	return outAABB.min.y != outAABB.max.y;
}

float Block::calcGroundFriction(Mob& mob, const BlockPos& pos) const {
	return mFriction;
}

bool Block::canHaveExtraData() const {
	return false;// causes crashes when pushing, because we cant tesselate extra data for every transparant block
	// return !isSolid();
}

bool Block::hasComparatorSignal() const {
	return false;
}

int Block::getComparatorSignal(BlockSource& region, const BlockPos& pos, FacingID dir, int data) const {
	return Redstone::SIGNAL_NONE;
}

bool Block::shouldStopFalling(Entity& entity) const {
	return entity.mOnGround;
}

bool Block::hasProperty(BlockProperty type) const { 
	return (mProperties & type) != BlockProperty::Unspecified; 
}

BlockProperty Block::getProperties() const { 
	return mProperties; 
}

float Block::getShadeBrightness() const {
	return isSolid() || hasProperty(BlockProperty::Leaf) ? 0.2f : 1;
}

Brightness Block::getLightEmission(const BlockID id) {
	return mLightEmission[id];
}

const Block* Block::lookupByName(const std::string& name, bool caseInsensitive)
{
	// If name is empty, always return null.
	if (name.empty())
		return nullptr;

	// All names in map are lower case (so input must always be lower case)
	std::string testName = caseInsensitive ? Util::toLower(name) : name;
	std::size_t namespaceLoc;
	if ((namespaceLoc = testName.find(":")) != testName.npos) {
		testName = testName.substr(namespaceLoc + 1);
	}

	auto it = mBlockLookupMap.find(testName);
	if (it != mBlockLookupMap.end())
		return it->second;

	// See if we need to pre pend "tile." (happens with text coming from console commands)
	if (testName.find("tile.") == -1)
	{
		testName = "tile." + testName;
		it = mBlockLookupMap.find(testName);
		if (it != mBlockLookupMap.end())
			return it->second;
	}

	ALOGE(LOG_AREA_BLOCKS, "Block::LookupByName() Failed to find block[%s]", name.c_str());

	return nullptr;
}

void Block::getDebugText(std::vector<std::string>& outputInfo) const {
	
#ifdef ENABLE_DEBUG_RENDERING
	const BlockPos& bpos = DebugRenderer::getDebugBlockPosition();
	outputInfo.push_back(Util::format("Debug Block[ %s ] [%d, %d, %d]", mDescriptionId.c_str(), bpos.x, bpos.y, bpos.z));
#else
	outputInfo.push_back("Debug Block[ " + mDescriptionId + " ]");
#endif
	outputInfo.push_back("Block ID: " + Util::toString(mID));
}

bool Block::canGrowChorus() const {
	return mID == mEndStone->mID || mID == mChorusPlantBlock->mID;
}

FacingID Block::getPlacementFacingAll(Entity& entity, const BlockPos& pos, float yRotOffsetDegree) {
	
	Vec3 entityPos = entity.getPos();

	if (abs(static_cast<int>(entityPos.x) - pos.x) < 2 && abs(static_cast<int>(entityPos.z) - pos.z) < 2) {
		// If the entity is above the block, the slot is on the top
		if (entity.mBB.min.y > pos.y) {
			return Facing::UP;
		}

		// If the entity is below the block, the slot is on the bottom
		if (pos.y > entity.mBB.max.y) {
			return Facing::DOWN;
		}
	}
	
	// The slot is on the side	
	return getPlacementFacingAllExceptAxisY(entity, pos, yRotOffsetDegree);
}


FacingID Block::getPlacementFacingAllExceptAxisY(Entity& entity, const BlockPos& pos, float yRotOffsetDegree) {

	DataID dir = (Math::floor((entity.getRotation().y - yRotOffsetDegree) * 4.0f / 360.0f + 0.5f)) & 3;

	switch (dir) {
	case 0:
		return Facing::NORTH;
	case 1:
		return Facing::EAST;
	case 2:
		return Facing::SOUTH;
	case 3:
		return Facing::WEST;
	default:
		DEBUG_ASSERT(0, "What direction should this be?");
		return Facing::NOT_DEFINED;
	}
}