/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
// Hullo MinceRaft programmer,
// This file is not that great, it is hard-coded mappings
// to make the Java block state convert to MC++ data values
// - With love, Venvious
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

#include "Dungeons.h"

#include "world/level/levelgen/structure/StructureTemplate.h"
// #include "world/level/block/entity/StructureBlockEntity.h"
#include "world/level/BlockSource.h"
#include "world/level/block/entity/BlockEntity.h"
#include "Core/Debug/Log.h"
#include "world/entity/Entity.h"
#include "world/level/Level.h"

// TODO: GET THESE OUT OF HERE A$0P
#include "world/level/block/QuartzBlockBlock.h" // Trash include
#include "world/level/block/StairBlock.h"       // Not the place for this
#include "world/level/block/SlabBlock.h"        // Do not want
#include "world/level/block/WoodBlock.h"        // Do not want x2
#include "world/level/block/WallBlock.h"        // Do not want x3
#include "world/level/block/TorchBlock.h"       // Literally the worst
#include "world/level/block/CauldronBlock.h"    // Swag
#include "world/level/block/BedBlock.h"         // I'm tired
#include "world/level/block/MonsterEggBlock.h"  // Please don't
#include "world/level/block/StoneSlabBlock.h"   // Ugh
#include "world/Facing.h"

// Cardinals that make sense <~*new and improved formula*~>
enum SensibleDirections { NORTH, EAST, SOUTH, WEST };

#define ROTATE_CONVERT(direction) \
if (isStairs) { \
	rotationData = StairBlock::DIR_##direction; \
} \
else if (currID == Block::mTorch->mID) { \
	rotationData = TorchBlock::TORCH_##direction; \
} \
else { \
	rotationData = Facing::Name::direction; \
}

DataID StructureTemplate::_mapToData(BlockID currID, StructureBlockInfo &blockInfo, const StructureSettings &settings) {
	DataID data = blockInfo.mBlock.data;
	DataID rotationData = 0; // Stores the rotation separate as to not affect the rest of the data
	bool ignoreExtraRotation = false;

	static const std::unordered_set<int> specialCaseBlocks = {
		Block::mLadder->mID,
		Block::mTorch->mID,
		Block::mEndRod->mID,
		Block::mWallSign->mID,
		Block::mChest->mID,
		Block::mEnderChest->mID,
		Block::mFurnace->mID,
		Block::mBed->mID,
		Block::mLitRedStoneTorch->mID,
		Block::mSkull->mID,
		Block::mBrewingStand->mID
	};

	bool isStairs = (blockInfo.mBlock.getBlock().isStairBlock() || currID == Block::mTrapdoor->mID);
	if (isStairs || specialCaseBlocks.find(currID) != specialCaseBlocks.end()) {
		// Switch to sensible values cuz it's better, yes this is ridiculous
		if (currID == Block::mTorch->mID || currID == Block::mLitRedStoneTorch->mID) {
			// Special case for torches 5 directions (first 3 bits)
			switch (data & 0x07) {
			case TorchBlock::TORCH_NORTH:
				rotationData = SensibleDirections::NORTH;
				break;
			case TorchBlock::TORCH_EAST:
				rotationData = SensibleDirections::EAST;
				break;
			case TorchBlock::TORCH_SOUTH:
				rotationData = SensibleDirections::SOUTH;
				break;
			case TorchBlock::TORCH_WEST:
				rotationData = SensibleDirections::WEST;
				break;
			case TorchBlock::TORCH_TOP:
				// We don't care about the up direction, RUDE!
				break;
			}

			// Zero out the rotation bits, ignoring the "up" direction (0xFF - 0x07 = 0xF8)
			if ((data & 0x07) != TorchBlock::TORCH_TOP) {
				data &= 0xF8;
			}
			else {
				ignoreExtraRotation = true;
			}
		}
		// End rods also like to be mirrored but for some reason they use Facing instead of TorchBlock
		else if (currID == Block::mEndRod->mID) {
			switch (data & 0x07) {
			case Facing::NORTH:
				rotationData = SensibleDirections::NORTH;
				break;
			case Facing::SOUTH:
				rotationData = SensibleDirections::SOUTH;
				break;
			case Facing::WEST:
				rotationData = SensibleDirections::WEST;
				break;
			case Facing::EAST:
				rotationData = SensibleDirections::EAST;
				break;
			default:
				// Ignore up and down
				break;
			}

			// Zero out the rotation bits, ignoring the "up" and "down" directions (0xFF - 0x07 = 0xF8)
			if ((data & 0x07) != Facing::UP && (data & 0x07) != Facing::DOWN) {
				data &= 0xF8;
			}
			else {
				ignoreExtraRotation = true;
			}
		}
		// This is not for stairs
		else if (!isStairs) {
			// First 3 bits because facing is weird
			switch (data & 0x07) {
			case Facing::NORTH:
				rotationData = SensibleDirections::NORTH;
				break;
			case Facing::EAST:
				rotationData = SensibleDirections::EAST;
				break;
			case Facing::SOUTH:
				rotationData = SensibleDirections::SOUTH;
				break;
			case Facing::WEST:
				rotationData = SensibleDirections::WEST;
				break;
			}

			// Zero out the rotation bits (0xFF - 0x07 = 0xF8)
			data &= 0xF8;
		}
		// This is for stairs (and trap doors)
		else {
			// Only count the first 2 bits for stair rotation
			switch (data & 0x03) {
			case StairBlock::DIR_NORTH:
				rotationData = SensibleDirections::NORTH;
				break;
			case StairBlock::DIR_EAST:
				rotationData = SensibleDirections::EAST;
				break;
			case StairBlock::DIR_SOUTH:
				rotationData = SensibleDirections::SOUTH;
				break;
			case StairBlock::DIR_WEST:
				rotationData = SensibleDirections::WEST;
				break;
			}

			// Zero out the rotation bits (0xFF - 0x03 = 0xFC)
			data &= 0xFC;
		}

		int rotation = static_cast<int>(settings.getRotation());
		if (currID == Block::mBed->mID) {
			// Brilliant! (only for beds? y?)
			switch (rotation) {
			case enum_cast(Rotation::NONE):
				rotation = enum_cast(Rotation::ROTATE_90);
				break;
			case enum_cast(Rotation::ROTATE_90):
				rotation = enum_cast(Rotation::ROTATE_270);
				break;
			case enum_cast(Rotation::ROTATE_270):
				rotation = enum_cast(Rotation::NONE);
				break;
			}
		}

		// This is pretty dumb
		switch ((rotationData + rotation) % enum_cast(Rotation::TOTAL)) {
		case SensibleDirections::NORTH:
			ROTATE_CONVERT(NORTH);
			break;
		case SensibleDirections::EAST:
			ROTATE_CONVERT(EAST);
			break;
		case SensibleDirections::SOUTH:
			ROTATE_CONVERT(SOUTH);
			break;
		case SensibleDirections::WEST:
			ROTATE_CONVERT(WEST);
			break;
		}
	}

	// Finalize the data
	return ignoreExtraRotation ? data : (data | rotationData);
}

// Terrible mapping function that takes the Java saved property strings and gives it in a data friendly form
int StructureTemplate::_mapToProperty(const std::string &propertyString, const std::string &valueString, const std::string &blockName) {
	if (propertyString == "variant") {
		if (valueString == "default") {
			// Kill me
		}
		else if (valueString == "chiseled") {
			return enum_cast(QuartzBlockBlock::Type::Chiseled);
		}
		else if (valueString == "lines") {
			return enum_cast(QuartzBlockBlock::Type::Lines);
		}

		else if (valueString == "chiseled_brick") {
			return enum_cast(MonsterEggBlock::HostBlockType::ChiseledStoneBrick);
		}
		else if (valueString == "mossy_brick") {
			return enum_cast(MonsterEggBlock::HostBlockType::MossyStoneBrick);
		}
		else if (valueString == "stone_brick") {
			return enum_cast(MonsterEggBlock::HostBlockType::StoneBrick);
		}

		else if (valueString == "mossy_stonebrick") {
			return WallBlock::TYPE_MOSSY;
		}
		else if (valueString == "smooth_andesite") {
			return 4;
		}
		else if (valueString == "chiseled_stonebrick") {
			return 3;
		}
		else if (valueString == "cracked_stonebrick") {
			return 2;
		}
		else if (valueString == "spruce") {
			return enum_cast(WoodBlockType::Spruce);
		}
	}
	else if (propertyString == "part") {
		if (valueString == "foot") {
			return 0;
		}
		else if (valueString == "head") {
			return BedBlock::HEAD_PIECE_DATA;
		}
	}
	else if (propertyString == "color") {
		if (valueString == "white") {
			return 0;
		}
		else if (valueString == "orange") {
			return 1;
		}
		else if (valueString == "magenta") {
			return 2;
		}
		else if (valueString == "lightBlue") {
			return 3;
		}
		else if (valueString == "yellow") {
			return 4;
		}
		else if (valueString == "lime") {
			return 5;
		}
		else if (valueString == "pink") {
			return 6;
		}
		else if (valueString == "gray") {
			return 7;
		}
		else if (valueString == "silver") {
			return 8;
		}
		else if (valueString == "cyan") {
			return 9;
		}
		else if (valueString == "purple") {
			return 10;
		}
		else if (valueString == "blue") {
			return 11;
		}
		else if (valueString == "brown") {
			return 12;
		}
		else if (valueString == "green") {
			return 13;
		}
		else if (valueString == "red") {
			return 14;
		}
		else if (valueString == "black") {
			return 15;
		}
	}
	else if (propertyString == "has_bottle_0") {
		if (valueString == "true") {
			// ???
		}
	}
	else if (propertyString == "has_bottle_1") {
		if (valueString == "true") {
			// ???
		}
	}
	else if (propertyString == "has_bottle_2") {
		if (valueString == "true") {
			// ???
		}
	}
	// Cauldron water level, yes, this one needs a comment...
	else if (propertyString == "level") {
		int unclamped;
		Util::toInt(valueString, unclamped);
		int clamped = Math::clamp(unclamped, CauldronBlock::MIN_FILL_LEVEL, CauldronBlock::MAX_FILL_LEVEL);
		float delta = float(clamped) / 3.0f;
		return CauldronBlock::MIN_FILL_LEVEL + int(delta * float(CauldronBlock::MAX_FILL_LEVEL - CauldronBlock::MIN_FILL_LEVEL));
	}
	else if (propertyString == "half") {
		bool isStairs = (blockName.find("stairs") != std::string::npos);
		bool isTrapdoor = (blockName == "trapdoor" || blockName == "iron_trapdoor");

		if (valueString == "top") {
			if (isStairs || isTrapdoor) {
				return StairBlock::UPSIDEDOWN_BIT;
			}
			else {
				return SlabBlock::TOP_SLOT_BIT;
			}
		}
	}
	else if (propertyString == "axis") {
		if (valueString == "x") {
			return RotatedPillarBlock::FACING_X;
		} else if (valueString == "y") {
			return RotatedPillarBlock::FACING_Y;
		} else if (valueString == "z") {
			return RotatedPillarBlock::FACING_Z;
		}
	}
	else if (propertyString == "facing") {
		// Torch can face 5 directions so it is special, with reverse h4xx
		if (blockName == "torch" || blockName == "redstone_torch") {
			if (valueString == "east") {
				return TorchBlock::TORCH_WEST;
			}
			else if (valueString == "west") {
				return TorchBlock::TORCH_EAST;
			}
			else if (valueString == "south") {
				return TorchBlock::TORCH_NORTH;
			}
			else if (valueString == "north") {
				return TorchBlock::TORCH_SOUTH;
			}
			else if (valueString == "up") {
				return TorchBlock::TORCH_TOP;
			}
		}
		// Flip directions for the end rod
		else if (blockName == "end_rod") {
			if (valueString == "east") {
				return Facing::WEST;
			}
			else if (valueString == "west") {
				return Facing::EAST;
			}
			else if (valueString == "south") {
				return Facing::NORTH;
			}
			else if (valueString == "north") {
				return Facing::SOUTH;
			}

			// Down and up are the same

			else if (valueString == "down") {
				return Facing::DOWN;
			}
			else if (valueString == "up") {
				return Facing::UP;
			}
		}
		// Special case for trapdoor, it is stairs???
		else if (blockName == "trapdoor" || blockName == "iron_trapdoor") {
			if (valueString == "east") {
				return StairBlock::DIR_EAST;
			}
			else if (valueString == "west") {
				return StairBlock::DIR_WEST;
			}
			else if (valueString == "south") {
				return StairBlock::DIR_SOUTH;
			}
			else if (valueString == "north") {
				return StairBlock::DIR_NORTH;
			}
		}
		else {
			bool isStairs = (blockName.find("stairs") != std::string::npos);

			if (valueString == "east") {
				return isStairs ? StairBlock::DIR_EAST : Facing::EAST;
			}
			else if (valueString == "west") {
				return isStairs ? StairBlock::DIR_WEST : Facing::WEST;
			}
			else if (valueString == "south") {
				return isStairs ? StairBlock::DIR_SOUTH : Facing::SOUTH;
			}
			else if (valueString == "north") {
				return isStairs ? StairBlock::DIR_NORTH : Facing::NORTH;
			}
			else if (!isStairs) {
				if (valueString == "down") {
					return Facing::DOWN;
				}
				else if (valueString == "up") {
					return Facing::UP;
				}
			}
		}
	}

	return 0;
}

void StructureTemplate::_mapToString(CompoundTag &paletteTag, DataID dataVariant) {
	CompoundTag propertiesTag;
	std::string variantString("default");

	if (dataVariant == enum_cast(QuartzBlockBlock::Type::Chiseled)) {
		variantString = "chiseled";
	}
	else if (dataVariant == enum_cast(QuartzBlockBlock::Type::Lines)) {
		variantString = "lines";
	}
	propertiesTag.putString("variant", variantString);

	//////////////////////////////////////////////////////////////////////////
	// Just for MC++, takes precedence over the JervaMC string states
	//////////////////////////////////////////////////////////////////////////
	propertiesTag.putInt("dataID", dataVariant);

	paletteTag.put("Properties", propertiesTag.copy());
}

void StructureTemplate::_mapToBlock(std::string& blockName, int& data) {
	if (blockName == "purpur_pillar") {
		blockName = "purpur_block";
		data |= static_cast<int>(QuartzBlockBlock::Type::Lines);
	}
	else if (blockName == "purpur_slab") {
		blockName = "stone_slab2";
		data |= static_cast<int>(StoneSlabBlock2::StoneSlabType::Purpur);
	}
}

Unique<CompoundTag> StructureTemplate::_mapTag(Unique<CompoundTag> originalTag) {
	// Horrible hack to transform potion items in Java to MCPE
// 	if (originalTag->contains("Items")) {
// 		auto items = originalTag->getList("Items");
// 		for (int i = 0; i < items->size(); ++i) {
// 			Tag* tag = items->get(i);
// 			if (tag->getId() == Tag::Type::Compound) {
// 				CompoundTag* itemTag = (CompoundTag*)tag;
// 				if (itemTag->contains("tag")) {
// 					const CompoundTag* potionTag = itemTag->getCompound("tag");
// 					auto id = itemTag->getString("id");
// 					if (id == "minecraft:potion") {
// 						itemTag->putShort("id", 373);
// 						if (potionTag->getString("Potion") == "minecraft:strong_healing") {
// 							itemTag->putShort("Damage", 22);
// 						}
// 						// Slots are also mapped differently
// 						auto slot = itemTag->getByte("Slot");
// 						++slot;
// 						if (slot < 4) {
// 							itemTag->putByte("Slot", slot);
// 						}
// 					}
// 					else if (id == "minecraft:splash_potion") {
// 						itemTag->putShort("id", 438);
// 						if (potionTag->getString("Potion") == "minecraft:weakness") {
// 							itemTag->putShort("Damage", 34);
// 						}
// 					}
// 				}
// 			}
// 		}
// 	}
// 	// Transforms cactus id for flower pots
// 	else if (originalTag->contains("Item")) {
// 		auto item = originalTag->getString("Item");
// 		if (item == "minecraft:cactus") {
// 			originalTag->putShort("item", 81);
// 		}
// 	}

	return originalTag;
}
