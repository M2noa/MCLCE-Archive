/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/LeafBlock.h"
#include "world/level/material/Material.h"
#include "world/item/Item.h"
#include "world/item/ItemInstance.h"
#include "world/level/FoliageColor.h"
#include "world/Facing.h"
#include "world/level/Level.h"
#include "world/entity/player/Player.h"
#include "world/level/biome/Biome.h"
// #include "world/entity/monster/Spider.h"
// #include "world/level/Spawner.h"
#include "world/level/levelgen/synth/PerlinSimplexNoise.h"
#include "world/level/BlockSource.h"

bool _istransparent(BlockSource& region, const BlockPos& pos) {
	const Block& block = region.getBlock(pos);
	return !block.hasProperty(BlockProperty::Leaf) && Block::mTranslucency[block.mID] > 0.f;
}

bool LeafBlock::isDeepLeafBlock(BlockSource& region, const BlockPos& pos) {
	if (_istransparent(region, pos.below())) {
		return false;
	}
	if (_istransparent(region, pos.north())) {
		return false;
	}
	if (_istransparent(region, pos.south())) {
		return false;
	}
	if (_istransparent(region, pos.west())) {
		return false;
	}
	if (_istransparent(region, pos.east())) {
		return false;
	}

	//"up" is an important case because we also want to exclude topSnow in forests
		const Block& block = region.getBlock(pos.above());
		if (!block.hasProperty(BlockProperty::TopSnow | BlockProperty::Leaf) && Block::mTranslucency[block.mID] > 0.f) {
		return false;
	}

	return true;
}

LeafBlock::LeafBlock(const std::string& nameId, int id) :
	TransparentBlock(nameId, id, Material::getMaterial(MaterialType::Leaves), false){
	mHasTransparentLeaves = false;
	setTicking(true);

	setSolid(false);
	setPushesOutItems(true);

	mRenderLayer = RENDERLAYER_SEASONS_OPTIONAL_ALPHATEST;
	mBrightnessGamma = 0.8f;

	mProperties = mProperties | BlockProperty::Leaf | BlockProperty::BreakOnPush;
}

LeafBlock::~LeafBlock(){
}

void LeafBlock::onRemove(BlockSource& region, const BlockPos& pos) const {
	runDecay(region, pos, 1);
	Block::onRemove(region, pos);
}

bool LeafBlock::isSeasonTinted(BlockSource& region, const BlockPos& p) const {
	BlockRenderLayer layer = getRenderLayer(region, p);
	return mHasTransparentLeaves && ((layer == RENDERLAYER_SEASONS_OPAQUE) || (layer == RENDERLAYER_SEASONS_OPTIONAL_ALPHATEST));
}

BlockRenderLayer LeafBlock::getRenderLayer(BlockSource& region, const BlockPos& pos) const {
	auto& b = region.getBiome(pos);
	if (b.canHaveSnowfall()) {
		return mRenderLayer;
	}

	return mHasTransparentLeaves ? RENDERLAYER_OPTIONAL_ALPHATEST : RENDERLAYER_OPAQUE;
}

int LeafBlock::getColor(BlockSource& region, const BlockPos& pos, DataID data) const{
	if (isSeasonTinted(region, pos)) {
		Color c(getSeasonsColor(region, pos, 5, 7));
		return c.toARGB();
	} else {
		int totalRed = 0;
		int totalGreen = 0;
		int totalBlue = 0;

		BlockPos offset;

		for (offset.x = -1; offset.x < 2; ++offset.x) {
			for (offset.z = -1; offset.z < 2; ++offset.z) {
				if (offset.x == 0 && offset.z == 0) {
					continue;
				}

				int foliageColor = region.getBiome(pos + offset * 4).getFoliageColor();
				totalRed += (foliageColor & 0xff0000) >> 16;
				totalGreen += (foliageColor & 0xff00) >> 8;
				totalBlue += foliageColor & 0xff;
			}
		}

		return (((totalRed / 8) & 0xFF) << 16) | (((totalGreen / 8) & 0xFF) << 8) | (((totalBlue / 8) & 0xFF));
	}
}

Color LeafBlock::getMapColor(BlockSource& region, const BlockPos& pos) const {
	Color result= Color::fromARGB( 0x8a8a8a );
	int totalRed = 0;
	int totalGreen = 0;
	int totalBlue = 0;

	BlockPos offset;

	for (offset.x = -1; offset.x < 2; ++offset.x) {
		for (offset.z = -1; offset.z < 2; ++offset.z) {
			if (offset.x == 0 && offset.z == 0) {
				continue;
			}

			int foliageColor = region.getBiome(pos + offset * 4).getMapFoliageColor();
			totalRed += (foliageColor & 0xff0000) >> 16;
			totalGreen += (foliageColor & 0xff00) >> 8;
			totalBlue += foliageColor & 0xff;
		}
	}

	return result * Color::fromARGB((((totalRed / 8) & 0xFF) << 16) | (((totalGreen / 8) & 0xFF) << 8) | (((totalBlue / 8) & 0xFF)));
}

static float fallScale = 0.01f;

Color LeafBlock::getSeasonsColor(BlockSource& region, const BlockPos& pos, int startColumn, int endColumn) const {
	BlockPos testPos(pos);
	Biome& b = region.getBiome(testPos);
	Color c(0.0f, b.mId * 1.0f / 255.0f, 0.0f, 1.0f);

	Height h = region.getHeightmap(pos);
	int columnOffset = 8;

	while (testPos.y < h) {
		BlockID id = region.getBlockID(testPos);
		if (!(id == BlockID::AIR || id.hasProperty(BlockProperty::Leaf | BlockProperty::TopSnow | BlockProperty::Snow))) {
			columnOffset = 0;
			break;
		}
		++testPos.y;
	}

	startColumn += columnOffset;
	endColumn += columnOffset;

	if (endColumn > startColumn) {
		static PerlinSimplexNoise noise(1234L, 1);

		float scale = (endColumn - startColumn) / 16.0f;
		float offset = startColumn / 16.0f + 1.0f / 32.0f;

		float val = noise.getValue(Vec3(pos) * fallScale) * 0.5f + 0.5f;
		val = Math::clamp(val, 0.0f, 1.0f);
		val = val * scale + offset;
		c.r = val;
	} else {
		c.r = startColumn * 1.0f / 16.0f + 1.0f / 32.0f;
	}

	return c;
}

void LeafBlock::tick(BlockSource& region, const BlockPos& pos, Random& random) const {
	if (region.getLevel().isClientSide()) {
		return;
	}

	DataID currentData = region.getData(pos);
	bool bUpdateBit = getBlockState(BlockState::UpdateBit).getBool(currentData);
	bool bPersistentBit = getBlockState(BlockState::PersistentBit).getBool(currentData);

	if (bUpdateBit && !bPersistentBit) {
		const int r = LeafBlock::REQUIRED_WOOD_RANGE;
		int r2 = r + 1;

		static const int WIDTH = 32;
		std::array<int, WIDTH * WIDTH * WIDTH> checkBuffer = {};

		const int WW = WIDTH * WIDTH;
		const int WO = WIDTH / 2;

		Vec3 vPos(pos);
		AABB aabb(vPos, vPos);
		if(region.hasChunksAt(aabb.grow(Vec3((float)r2)))) {
			for (int xo = -r; xo <= r; xo++) {
				for (int yo = -r; yo <= r; yo++) {
					for (int zo = -r; zo <= r; zo++) {
						const Block& block = region.getBlock(pos.x + xo, pos.y + yo, pos.z + zo);
						if (block.isType(Block::mLog) || block.isType(Block::mLog2)) {
							checkBuffer[(xo + WO) * WW + (yo + WO) * WIDTH + (zo + WO)] = 0;
						} else if (block.hasProperty(BlockProperty::Leaf)) {
							checkBuffer[(xo + WO) * WW + (yo + WO) * WIDTH + (zo + WO)] = -2;
						} else {
							checkBuffer[(xo + WO) * WW + (yo + WO) * WIDTH + (zo + WO)] = -1;
						}
					}
				}
			}

			for (int i = 1; i <= LeafBlock::REQUIRED_WOOD_RANGE; i++) {
				for (int xo = -r; xo <= r; xo++) {
					for (int yo = -r; yo <= r; yo++) {
						for (int zo = -r; zo <= r; zo++) {
							if (checkBuffer[(xo + WO) * WW + (yo + WO) * WIDTH + (zo + WO)] == i - 1) {
								if (checkBuffer[(xo + WO - 1) * WW + (yo + WO) * WIDTH + (zo + WO)] == -2) {
									checkBuffer[(xo + WO - 1) * WW + (yo + WO) * WIDTH + (zo + WO)] = i;
								}
								if (checkBuffer[(xo + WO + 1) * WW + (yo + WO) * WIDTH + (zo + WO)] == -2) {
									checkBuffer[(xo + WO + 1) * WW + (yo + WO) * WIDTH + (zo + WO)] = i;
								}
								if (checkBuffer[(xo + WO) * WW + (yo + WO - 1) * WIDTH + (zo + WO)] == -2) {
									checkBuffer[(xo + WO) * WW + (yo + WO - 1) * WIDTH + (zo + WO)] = i;
								}
								if (checkBuffer[(xo + WO) * WW + (yo + WO + 1) * WIDTH + (zo + WO)] == -2) {
									checkBuffer[(xo + WO) * WW + (yo + WO + 1) * WIDTH + (zo + WO)] = i;
								}
								if (checkBuffer[(xo + WO) * WW + (yo + WO) * WIDTH + (zo + WO - 1)] == -2) {
									checkBuffer[(xo + WO) * WW + (yo + WO) * WIDTH + (zo + WO - 1)] = i;
								}
								if (checkBuffer[(xo + WO) * WW + (yo + WO) * WIDTH + (zo + WO + 1)] == -2) {
									checkBuffer[(xo + WO) * WW + (yo + WO) * WIDTH + (zo + WO + 1)] = i;
								}
							}
						}
					}
				}
			}
		}

		int mid = checkBuffer[(WO)*WW + (WO)*WIDTH + (WO)];
		if (mid >= 0) {
			getBlockState(BlockState::UpdateBit).set(currentData, false);
			region.setBlockAndData(pos.x, pos.y, pos.z, mID, currentData, Block::UPDATE_NONE);
		} else {
			die(region, pos);
		}
	}
}

void LeafBlock::playerDestroy(Player* player, const BlockPos& pos, int data) const{
	auto& region = player->getRegion();
	if (!region.getLevel().isClientSide()) {
		ItemInstance* item = player->getSelectedItem();
		if (item && item->isInstance((Item*)Item::mShears)) {
			// drop leaf block instead of sapling
			popResource(region, pos, ItemInstance(mID, 1, getLeafType(data)));
			return;
		}
	}

	TransparentBlock::playerDestroy(player, pos, data);
}

int LeafBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const{
	return random.nextInt(20) == 0 ? 1 : 0;
}

int LeafBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Block::mSapling->mID;
}

void LeafBlock::spawnResources(BlockSource& region, const BlockPos& pos, int data, float odds, int bonusLootLevel) const {
	auto& level = region.getLevel();
	Random& random = level.getRandom();

	if (!level.isClientSide()) {
		int chance = 20;

		// basic loot drop: chance to drop sapling. Best possible chance is 1 in 10
		if (bonusLootLevel > 0) {
			chance -= 2 << bonusLootLevel;
			chance = std::max(10, chance);
		}

		if (random.nextInt(chance) == 0) {
			int type = getResource(level.getRandom(), data);
			popResource(region, pos, ItemInstance(type, 1, getSpawnResourcesAuxValue(data)));
		}

		// second, bonus loot drop: chance to drop apple.  Best possible chance is 1 in 120
		// Note - using console commands to illegally inflate the level of looting enchant, it is possible to hit the
		// hard cap of 1 in 40 odds
		chance = 200;
		if (bonusLootLevel > 0) {
			chance -= 10 << bonusLootLevel;
			chance = std::max(40, chance);
		}

		if (random.nextInt(chance) == 0) {
			dropExtraLoot(region, pos, getSpawnResourcesAuxValue(data));
		}
	}
}

void LeafBlock::onGraphicsModeChanged(bool fancy, bool preferPolyTessellation, bool transparentLeaves) {
	mAllowSame = transparentLeaves;
	mHasTransparentLeaves = transparentLeaves;

	mRenderLayer = mHasTransparentLeaves ? RENDERLAYER_SEASONS_OPTIONAL_ALPHATEST : RENDERLAYER_OPAQUE;

	setLightBlock(Brightness(1));

	TransparentBlock::onGraphicsModeChanged(fancy, preferPolyTessellation, transparentLeaves);
}
DataID LeafBlock::getSpawnResourcesAuxValue(DataID data) const {
	return getBlockState(BlockState::MappedType).get<DataID>(data);
}

void LeafBlock::die(BlockSource& region, const BlockPos& pos) const {
	spawnResources(region, pos, region.getData(pos), 0.f);
	region.setBlock(pos, BlockID::AIR, Block::UPDATE_ALL);
}

const MobSpawnerData* LeafBlock::getMobToSpawn(BlockSource& region, const BlockPos& pos) const {
	static const MobSpawnerData spiderData(EntityType::Spider, 100, 0, 1);

	// leaves create spiders, muahaha, hahahaha, ha
	if (region.getRawBrightness(pos) < 8) {
		return &spiderData;
	}

	return nullptr;
}

void LeafBlock::runDecay(BlockSource& region, const BlockPos& t, int range) {
	if (region.getLevel().isClientSide()) {
		return;
	}

	int r2 = range + 1;
	AABB aabb(t, t);
	if (region.hasChunksAt(aabb.grow(Vec3((float)r2)))) {
		BlockPos o;

		for (o.x = -range; o.x <= range; o.x++) {
			for (o.y = -range; o.y <= range; o.y++) {
				for (o.z = -range; o.z <= range; o.z++) {
					auto leaf = region.getBlockAndData(t + o);
					if (leaf.id.hasProperty(BlockProperty::Leaf)) {
						Block::mLeaves->getBlockState(BlockState::UpdateBit).set(leaf.data, true);
						region.setBlockAndData(t + o, leaf, Block::UPDATE_NONE);
					}
				}
			}
		}
	}
}

int LeafBlock::getLeafType(int data) const {
	return getBlockState(BlockState::MappedType).get<int>(data);
}
