/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/OldLeafBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/FoliageColor.h"
#include "world/item/ItemInstance.h"
#include "world/item/Item.h"
#include "world/level/biome/Biome.h"

OldLeafBlock::OldLeafBlock(const std::string& nameId, int id)
	: LeafBlock(nameId, id) {
}

int OldLeafBlock::getColor(BlockSource& region, const BlockPos& pos, DataID data) const {
	LeafType type = static_cast<LeafType>(getLeafType(data));
	if (isSeasonTinted(region, pos)) {
		Color c;
		switch (type) {
		case LeafType::Spruce:
			c = getSeasonsColor(region, pos, 1, 1);
			break;
		case LeafType::Birch:
			c = getSeasonsColor(region, pos, 2, 4);
			break;
		default:
			return LeafBlock::getColor(region, pos, data);
		}

		return c.toARGB();
	}
	else {
		int totalRed = 0;
		int totalGreen = 0;
		int totalBlue = 0;

		BlockPos offset;

		for (offset.x = -1; offset.x < 2; ++offset.x) {
			for (offset.z = -1; offset.z < 2; ++offset.z) {
				if (offset.x == 0 && offset.z == 0) {
					continue;
				}

				Biome& biome = region.getBiome(pos + offset * 4);
				int foliageColor = 0;
				switch (type) {
				case LeafType::Spruce:
					foliageColor = biome.getEvergreenFoliageColor();
					break;
				case LeafType::Birch:
					foliageColor = biome.getBirchFoliageColor();
					break;
				default:
					foliageColor = biome.getFoliageColor();
				}

				totalRed += (foliageColor & 0xff0000) >> 16;
				totalGreen += (foliageColor & 0xff00) >> 8;
				totalBlue += foliageColor & 0xff;
			}
		}

		return (((totalRed / 8) & 0xFF) << 16) | (((totalGreen / 8) & 0xFF) << 8) | (((totalBlue / 8) & 0xFF));
	}
}

Color OldLeafBlock::getMapColor(BlockSource& region, const BlockPos& pos) const {
		auto type = static_cast<LeafType>(getLeafType(region.getData(pos)));
	Color result = Color::fromARGB(0x8a8a8a);

	int totalRed = 0;
	int totalGreen = 0;
	int totalBlue = 0;

	BlockPos offset;

	for (offset.x = -1; offset.x < 2; ++offset.x) {
		for (offset.z = -1; offset.z < 2; ++offset.z) {
			if (offset.x == 0 && offset.z == 0) {
				continue;
			}

			Biome& biome = region.getBiome(pos + offset * 4);
			int foliageColor = 0;
			switch (type) {
			case LeafType::Spruce:
				foliageColor = biome.getMapEvergreenFoliageColor();
				break;
			case LeafType::Birch:
				foliageColor = biome.getMapBirchFoliageColor();
				break;
			default:
				foliageColor = biome.getMapFoliageColor();
			}
			totalRed += (foliageColor & 0xff0000) >> 16;
			totalGreen += (foliageColor & 0xff00) >> 8;
			totalBlue += foliageColor & 0xff;
		}
	}

	return result * Color::fromARGB((((totalRed / 8) & 0xFF) << 16) | (((totalGreen / 8) & 0xFF) << 8) | (((totalBlue / 8) & 0xFF)));
}
void OldLeafBlock::dropExtraLoot(BlockSource& region, const BlockPos& pos, int data) const {
	int leafType = getLeafType(data);
	if (leafType == NORMAL_LEAF) {
		popResource(region, pos, ItemInstance(Item::mApple, 1, 0));
	}
}
int OldLeafBlock::getVariant(int data) const {
	int leafType = getLeafType(data) % NUM_TYPES;
	int index = mHasTransparentLeaves ? leafType : leafType + NUM_TYPES;
	return index;
}

std::string OldLeafBlock::buildDescriptionName(DataID data) const {
	int leafType = getLeafType(data);
	static const std::string LEAF_NAMES[NUM_TYPES] = {
		"oak", "spruce", "birch", "jungle"
	};

	return I18n::get(mDescriptionId + "." + LEAF_NAMES[leafType % NUM_TYPES] + ".name");
}

DataID OldLeafBlock::getSpawnResourcesAuxValue(DataID data) const {
	return getLeafType(data);
}
