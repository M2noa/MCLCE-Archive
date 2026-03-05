/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "FlowerBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/item/ItemInstance.h"
#include "locale/I18n.h"

FlowerBlock::FlowerBlock(const std::string& nameId, int id, Type type) :
	BushBlock(nameId, id, Material::getMaterial(MaterialType::Plant))
	, mType(type) {
	mRenderLayer = RENDERLAYER_ALPHATEST;
}

bool FlowerBlock::onFertilized(BlockSource& region, const BlockPos& pos, Entity* entity) const {
	auto& random = region.getLevel().getRandom();

	for (int j = 16; j < 64; j++) {
		BlockPos abovePos = pos.above();

		//see if there is a path to this block
		if (!_randomWalk(region, abovePos.x, abovePos.y, abovePos.z, j)) {
			continue;
		}

		auto block = region.getBlockID(abovePos);
		if (block == 0) {

			//place tall grass or flowers
			int id = random.nextInt(8);
			const Block* toPlant;

			if (id == 0) {
				toPlant = (this == Block::mRedFlower) ? Block::mYellowFlower : Block::mRedFlower;
			} else{
				toPlant = (this == Block::mRedFlower) ? Block::mRedFlower : Block::mYellowFlower;
			}

			//try to plant it
			if (toPlant != nullptr) {
				if (toPlant->canSurvive(region, abovePos)) {
					region.setBlockAndData(abovePos, toPlant->mID, Block::UPDATE_ALL);
				}
			}
		}
	}

	return true;
}

bool FlowerBlock::_randomWalk(BlockSource& region, int& xx, int& yy, int& zz, int j) const {
	auto& random = region.getLevel().getRandom();

	for (int i = 0; i < j / 16; i++) {
		xx += random.nextInt(3) - 1;
		const int rand0 = random.nextInt(3) - 1;
		yy += rand0 * random.nextInt(3) / 2;
		zz += random.nextInt(3) - 1;

		if (region.getBlockID(xx, yy - 1, zz) != Block::mGrass->mID || region.isSolidBlockingBlock(xx, yy, zz)) {
			return false;
		}
	}

	return j / 16 > 0;
}

DataID FlowerBlock::getSpawnResourcesAuxValue(DataID data) const {
	return data;
}

Vec3 FlowerBlock::randomlyModifyPosition(const BlockPos& pos) const {
	Vec3 v = pos;

	float randomScale = 0.35f;
	int32_t seed = (pos.x * 3129871) ^ (pos.z * 116129781l) ^ (pos.y);
	seed = seed * seed * 42317861 + seed * 11;

	v.x += ((((seed >> 16) & 0xf) / 15.0f) - 0.5f) * randomScale;
	v.y += ((((seed >> 20) & 0xf) / 15.0f) - 1.0f) * (Block::mYellowFlower ? 0.1f : randomScale);	//	Flowers are too short to move them that much.
	v.z += ((((seed >> 24) & 0xf) / 15.0f) - 0.5f) * randomScale;

	return v;
}

const AABB& FlowerBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	if (isClipping) {
		const auto& shape = getVisualShape(region, pos, bufferValue, isClipping);
		return bufferValue.set(shape.min, shape.max).move(randomlyModifyPosition(pos));
	}

	return AABB::EMPTY;
}

std::string FlowerBlock::buildDescriptionName(DataID data) const {
	int mappedType = getBlockState(BlockState::MappedType).get<int>(data);
	if (mappedType < 0 || mappedType >= 9) {
		mappedType = 0;
	}

	static std::string FLOWER_NAMES[2][9] = {
		{"dandelion"},
		{"poppy", "blueOrchid", "allium", "houstonia", "tulipRed", "tulipOrange", "tulipWhite", "tulipPink", "oxeyeDaisy"}
	};
	return I18n::get(mDescriptionId + "." + FLOWER_NAMES[enum_cast(mType)][mappedType] + ".name");
}
