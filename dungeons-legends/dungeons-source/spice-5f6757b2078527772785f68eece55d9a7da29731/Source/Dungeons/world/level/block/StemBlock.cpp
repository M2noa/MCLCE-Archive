/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/StemBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/item/ItemInstance.h"
#include "world/item/Item.h"

StemBlock::StemBlock(const std::string& nameId, int id, const Block& fruit)
	: BushBlock(nameId, id, Material::getMaterial(MaterialType::Plant))
	, mFruit(fruit) {
	setTicking(true);
	float ss = 0.125f;
	setVisualShape(Vec3(0.5f - ss, 0, 0.5f - ss), Vec3(0.5f + ss, 0.25f, 0.5f + ss));
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

bool StemBlock::mayPlaceOn( const Block& block ) const {
	return block.isType(Block::mFarmland);
}

void StemBlock::tick(BlockSource& region, const BlockPos& pos, Random& random) const {
	BushBlock::tick(region, pos, random);
	if (region.getRawBrightness( pos.above() ) >= Brightness::MAX - 6) {

		float growthSpeed = getGrowthSpeed(region, pos);

		if (random.nextInt((int) (25 / growthSpeed) + 1) == 0) {
			auto t = region.getBlockAndData(pos);
			int growth = getGrowth(t.data);
			if (growth < 7) {
				getBlockState(BlockState::Growth).set(t.data, growth + 1);
				region.setBlockAndData(pos, t, Block::UPDATE_ALL);
			} else {
				if ((region.getBlockID(pos.west() ) == mFruit.mID) ||
					(region.getBlockID(pos.east() ) == mFruit.mID) ||
					(region.getBlockID(pos.north() ) == mFruit.mID) ||
					(region.getBlockID(pos.south() ) == mFruit.mID)) {
					return;
				}

				for(auto a : range(4)) {
					int dir  = a;	//random.nextInt(4);
					BlockPos neighborPos(pos);

					if (dir == 0) {
						--neighborPos.x;
					}
					if (dir == 1) {
						++neighborPos.x;
					}
					if (dir == 2) {
						--neighborPos.z;
					}
					if (dir == 3) {
						++neighborPos.z;
					}

					BlockID below = region.getBlockID(neighborPos.below());
					if (region.getBlockID(neighborPos) == BlockID::AIR && (below == Block::mFarmland->mID || below == Block::mDirt->mID || below == Block::mGrass->mID)) {
						region.setBlock(neighborPos.x, neighborPos.y, neighborPos.z, mFruit.mID, Block::UPDATE_ALL);
						break;
					}
				}
			}
		}
	}
}

float StemBlock::getGrowthSpeed(BlockSource& region, const BlockPos& pos) const {
	float speed = 1;

	BlockID
		n = region.getBlockID(pos.north()),
		s = region.getBlockID(pos.south()),
		w = region.getBlockID(pos.west()),
		e = region.getBlockID(pos.east()),
		d0 = region.getBlockID(pos.x - 1, pos.y, pos.z - 1),
		d1 = region.getBlockID(pos.x + 1, pos.y, pos.z - 1),
		d2 = region.getBlockID(pos.x + 1, pos.y, pos.z + 1),
		d3 = region.getBlockID(pos.x - 1, pos.y, pos.z + 1);

	bool horizontal = w == mID || e == mID;
	bool vertical = n == mID || s == mID;
	bool diagonal = d0 == mID || d1 == mID || d2 == mID || d3 == mID;

	for (int xx = pos.x - 1; xx <= pos.x + 1; xx++) {
		for (int zz = pos.z - 1; zz <= pos.z + 1; zz++) {
			BlockID t = region.getBlockID(xx, pos.y - 1, zz);

			float blockSpeed = 0;
			if (t == Block::mFarmland->mID) {
				blockSpeed = 1;
				int moisturized = Block::mFarmland->getBlockState(BlockState::MoisturizedAmount).get<int>(region.getData(xx, pos.y - 1, zz));
				if (moisturized > 0) {
					blockSpeed = 3;
				}
			}

			if (xx != pos.x || zz != pos.z) {
				blockSpeed /= 4;
			}

			speed += blockSpeed;
		}
	}

	if (diagonal || (horizontal && vertical)) {
		speed /= 2;
	}

	return speed;
}

int StemBlock::getGrowth(const DataID& data) const {
	return getBlockState(BlockState::Growth).get<int>(data);
}

int StemBlock::getColor(int data) const {
	int growth = getGrowth(data);
	int r = growth * 32;
	int g = 255 - growth * 8;
	int b = growth * 4;
	return r << 16 | g << 8 | b;
}

int StemBlock::getColor( BlockSource& region, const BlockPos& pos) const {
	return getColor(region.getData(pos));
}

int StemBlock::getColor(BlockSource& region, const BlockPos& pos, DataID data) const {
	return getColor(data);
}

const AABB& StemBlock::getVisualShape(DataID data, AABB& bufferAABB, bool isClipping	/* = false*/) const {
	int growth = getGrowth(data);
	float yy1 = (growth * 2 + 2) / 16.0f;
	float ss = 0.125f;
	return bufferAABB.set(Vec3(0.5f - ss, 0, 0.5f - ss), Vec3(0.5f + ss, (float)yy1, 0.5f + ss));
}

int StemBlock::getConnectDir(BlockSource& region, const BlockPos& pos) const {
	int growth = getGrowth(region.getData(pos));
	if (growth < 7) {
		return -1;
	}
	if (region.getBlockID(pos.west()) == mFruit.mID) {
		return 0;
	}
	if (region.getBlockID(pos.east()) == mFruit.mID) {
		return 1;
	}
	if (region.getBlockID(pos.north()) == mFruit.mID) {
		return 2;
	}
	if (region.getBlockID(pos.south()) == mFruit.mID) {
		return 3;
	}

	return -1;
}

void StemBlock::spawnResources(BlockSource& region, const BlockPos& pos, int data, float odds, int bonusLootLevel) const {
	BushBlock::spawnResources(region, pos, data, odds);

	auto& level = region.getLevel();
	if (level.isClientSide()) {
		return;
	}

	Item* seed = nullptr;
	//if (fruit == Block::pumpkin) seed = Item::pumpkin_seeds;
	if (mFruit.isType(Block::mMelon)) {
		seed = Item::mSeeds_melon;
	} else{
		seed = Item::mSeeds_pumpkin;
	}

	for (int i = 0; i < 3; i++) {
		if (level.getRandom().nextInt(5 * 3) > data) {
			continue;
		}

		popResource(region, pos, ItemInstance(seed));
	}
}

int StemBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return -1;
}

int StemBlock::getResourceCount( Random& random, int data, int bonusLootLevel) const {
	return 1;
}

ItemInstance StemBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	if(mFruit.isType(Block::mPumpkin)) {
		return ItemInstance(Item::mSeeds_pumpkin);
	} else if (mFruit.isType(Block::mMelon)) {
		return ItemInstance(Item::mSeeds_melon);
	}

	return ItemInstance();
}


bool StemBlock::onFertilized(BlockSource& region, const BlockPos& pos, Entity* entity) const {
	if (region.getData(pos) >= MAX_GROWTH) {
		return false;
	}
	
	growCrops(region, pos);

	return true;
}

bool StemBlock::canBeSilkTouched() const {
	return false;
}
