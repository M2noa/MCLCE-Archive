/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/GrassBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/level/material/Material.h"
#include "world/Facing.h"
#include "world/level/block/TallGrass.h"
#include "world/level/block/DirtBlock.h"
#include "world/entity/Mob.h"
#include "world/level/biome/Biome.h"
#include "world/level/FoliageColor.h"

// REFACTOR: Just need to know about ParsedAtlasNode. Yuck. 
#include "client/renderer/texture/TextureAtlas.h"

const Brightness GrassBlock::MIN_BRIGHTNESS(4);

GrassBlock::GrassBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Dirt))
	, sideColors({
	Color::fromARGB(0x79c05a),
	Color::fromARGB(0x8ab689),
	Color::fromARGB(0xbfb755),
	Color::fromARGB(0x59c93c),
	Color::fromARGB(0x55c93f),
	Color::fromARGB(0x88bb66),
	Color::fromARGB(0x86b87f),
	Color::fromARGB(0x64c73f),
	Color::fromARGB(0x86b783),
	Color::fromARGB(0x83b593),
	Color::fromARGB(0x80b497),
	Color::fromARGB(0x91bd59),
	Color::fromARGB(0x90814d),
	Color::fromARGB(0x8eb971),
	Color::fromARGB(0x6a7039),
	Color::fromARGB(0x507a32)
}) {
	setTicking(true);

	mRenderLayer = RENDERLAYER_OPAQUE;
}

int GrassBlock::getColor(int auxdata) const {
	return 0x87cd49;
}

int GrassBlock::getColor(BlockSource& region, const BlockPos& pos, DataID data) const {
	return region.getGrassColor(pos);
}

DataID GrassBlock::calcVariant(BlockSource& region, const BlockPos& pos, DataID data) const {
	const Material& above = region.getMaterial(pos.above());
	if (above.isType(MaterialType::TopSnow) || above.isType(MaterialType::Snow)) {
		return static_cast<DataID>(sideColors.size()-1);
	}

	return static_cast<DataID>(_getGrassSide(region.getGrassColor(pos)));
}

void GrassBlock::tick(BlockSource& region, const BlockPos& pos, Random& random ) const{
	if (region.getLevel().isClientSide()) {
		return;
	}

	BlockPos above = pos.above();

	auto topBrightness = region.getRawBrightness(above);
	if (topBrightness < MIN_BRIGHTNESS) {
		auto& aboveMat = region.getMaterial(above);
		if (aboveMat.getTranslucency() < 0.2f && random.nextInt(4) == 0) {
			region.setBlockAndData(pos, Block::mDirt->mID, Block::UPDATE_CLIENTS);
		}
	} else {
		if (topBrightness >= Brightness::MAX - 6) {

			int randZ = pos.z + random.nextInt(3) - 1;
			int randY = pos.y + random.nextInt(5) - 3;
			int randX = pos.x + random.nextInt(3) - 1;
			BlockPos t = BlockPos(randX, randY, randZ);

			auto block = region.getBlockAndData(t);
			if (block.id == Block::mDirt->mID) {
				int type = Block::mDirt->getBlockState(BlockState::MappedType).get<int>(block.data);
				if (type == DirtBlock::TYPE_NORMAL) {
					auto aboveBlock = region.getBlockAndData(t.above());
					auto& material = region.getMaterial(t.above());
					if (region.getRawBrightness(t.above()) >= MIN_BRIGHTNESS && !Block::mBlocks[aboveBlock.id]->isSolid() && !material.isLiquid()) {
						region.setBlockAndData(t, Block::mGrass->mID, Block::UPDATE_CLIENTS);
					}
				}
			}
		}
	}
}

int GrassBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Block::mDirt->getResource(random, 0, bonusLootLevel);
}

bool GrassBlock::_randomWalk(BlockSource& region, BlockPos& pos, int j) const {
	auto& random = region.getLevel().getRandom();

	for (int i = 0; i < j / 16; i++) {
		pos.x += random.nextInt(3) - 1;
		int randY0 = random.nextInt(3) - 1;
		pos.y += randY0 * random.nextInt(3) / 2;
		pos.z += random.nextInt(3) - 1;

		if (!region.getBlock(pos.below()).isType(Block::mGrass) || region.isSolidBlockingBlock(pos)) {
			return false;
		}
	}

	return j / 16 > 0;
}

int GrassBlock::_getGrassSide(int grassColorBytes) const {
	Color grassColor = Color::fromARGB(grassColorBytes);
	float minDistance = 1000;
	int shortestIndex = 0;
	int index = 0;

	for (const Color& color : sideColors) {
		float curMinDistance = color.distanceFrom(grassColor);
		if (curMinDistance < minDistance) {
			minDistance = curMinDistance;
			shortestIndex = index;
		}

		index++;
	}

	return shortestIndex;
}

bool GrassBlock::onFertilized(BlockSource& region, const BlockPos& pos, Entity* entity) const {
	auto& random = region.getLevel().getRandom();

	for (int j = 16; j < 64; j++) {
		BlockPos abovePos = pos.above();

		//see if there is a path to this block
		if ( !_randomWalk(region, abovePos, j )) {
			continue;
		}

		auto block = region.getBlockID(abovePos);
		if (block == BlockID::AIR) {
			FullBlock toPlant = FullBlock::AIR;

			int randomInt = random.nextInt(8);
			if (randomInt == 0) {
				toPlant = region.getBiome(abovePos).getRandomFlowerTypeAndData(random, abovePos);
			}
			else {
				toPlant.id = Block::mTallgrass->mID;
				int tallGrassType = (randomInt == 2) ? enum_cast(TallGrassType::Fern) : enum_cast(TallGrassType::Tall);
				Block::mTallgrass->getBlockState(BlockState::MappedType).set(toPlant.data, tallGrassType);
			}

			DEBUG_ASSERT(Block::mBlocks[toPlant.id] != nullptr, "Invalid flower/grass block returned!");

			//try to plant it
			if (Block::mBlocks[toPlant.id]->canSurvive(region, abovePos)) {
				region.setBlockAndData(abovePos, toPlant, Block::UPDATE_ALL);
			}
		}
	}

	return true;
}

const MobSpawnerData* GrassBlock::getMobToSpawn(BlockSource& region, const BlockPos& pos) const {
	auto above = pos.above();
	if (region.getRawBrightness(above) > 8 && region.canSeeSky(above)) {
		return getTypeToSpawn(region, EntityType::Animal, pos);
	} else {
		return Block::getMobToSpawn(region, pos);
	}
}

Color GrassBlock::getMapColor(BlockSource& region, const BlockPos& pos) const {
	return Color::fromARGB(FoliageColor::getMapGrassColor(region, pos));
}

#ifndef MCPE_HEADLESS
// This should really be in block graphics, but all the side texture selection is currently here.
// As part of the continual separation between client and server we need to find a way to fully data drive this
void GrassBlock::initSideColorsFromAtlas(const TextureAtlas& atlas, const TextureAtlasItem& atlasItem) {
	sideColors.clear();
	const ParsedAtlasNode& node= atlas.getParsedNode(atlasItem.getParsedNodeIndex());
	for (auto& entry : node.elements) {
		sideColors.emplace_back(entry.overlay);
	}
}
#endif
