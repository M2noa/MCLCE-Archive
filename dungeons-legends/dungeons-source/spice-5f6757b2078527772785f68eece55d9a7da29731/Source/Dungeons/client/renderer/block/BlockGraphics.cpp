/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "client/renderer/block/BlockGraphics.h"
#include "world/level/block/Block.h"
#include "world/level/block/BlockInclude.h"
#include "world/level/block/LeafBlock.h"
#include "world/level/BlockSource.h"
#include "world/phys/Vec3.h"
#include "client/renderer/texture/TextureAtlas.h"
#include "world/level/biome/Biome.h"
#include "world/Facing.h"
// #include "platform/AppPlatform.h"
#include "Core/Debug/Log.h"
#include "util/JSONReadVerifier.h"

//------------------------------------------------------------------------------
BlockSoundType BlockSoundTypeFromString(const std::string& type) {
	static std::unordered_map<std::string, BlockSoundType> mapping = {
		{ "normal",		BlockSoundType::Normal },
		{ "gravel",		BlockSoundType::Gravel },
		{ "wood",		BlockSoundType::Wood },
		{ "grass",		BlockSoundType::Grass },
		{ "metal",		BlockSoundType::Metal },
		{ "stone",		BlockSoundType::Stone },
		{ "cloth",		BlockSoundType::Cloth },
		{ "glass",		BlockSoundType::Glass },
		{ "sand",		BlockSoundType::Sand },
		{ "snow",		BlockSoundType::Snow },
		{ "ladder",		BlockSoundType::Ladder },
		{ "anvil",		BlockSoundType::Anvil },
		{ "slime",		BlockSoundType::Slime },
		{ "silent",		BlockSoundType::Silent },
		{ "default",	BlockSoundType::Default }
	};
	auto res = mapping.find(Util::toLower(type));
	if (res != mapping.end()) {
		return res->second;
	}
	else {
		return BlockSoundType::Undefined;
	}
}
std::string BlockSoundTypeToString(BlockSoundType type) {
	static std::map<BlockSoundType, std::string> typeToString = {
		{ BlockSoundType::Normal,	"normal" },
		{ BlockSoundType::Gravel,	"gravel" },
		{ BlockSoundType::Wood,		"wood" },
		{ BlockSoundType::Grass,	"grass" },
		{ BlockSoundType::Metal,	"metal" },
		{ BlockSoundType::Stone,	"stone" },
		{ BlockSoundType::Cloth,	"cloth" },
		{ BlockSoundType::Glass,	"glass" },
		{ BlockSoundType::Sand,		"sand" },
		{ BlockSoundType::Snow,		"snow" },
		{ BlockSoundType::Ladder,	"ladder" },
		{ BlockSoundType::Anvil,	"anvil" },
		{ BlockSoundType::Slime,	"slime" },
		{ BlockSoundType::Silent,	"silent" },
		{ BlockSoundType::Default,	"default" },
		{ BlockSoundType::Undefined,"undefined" }
	};
	return typeToString[type];
}

//------------------------------------------------------------------------------

const float BlockGraphics::SIZE_OFFSET = 0.0001f;

std::shared_ptr<TextureAtlas> BlockGraphics::mTerrainTextureAtlas;

BlockGraphics* BlockGraphics::mBlocks[NUM_BLOCK_TYPES];
std::vector<Unique<BlockGraphics> > BlockGraphics::mOwnedBlocks;
std::unordered_map<std::string, BlockGraphics* > BlockGraphics::mBlockLookupMap;

void BlockGraphics::setTextureAtlas(std::shared_ptr<TextureAtlas> terrainAtlas) {
	mTerrainTextureAtlas = terrainAtlas;
}

bool BlockGraphics::setTextures(BlockGraphics& block, const Json::Value& textureData){
	if (textureData.isNull())
		return true;

	if (textureData.isString()) {
		block.setTextureItem(textureData.asString());
		return true;
	}

	if (textureData.isObject()) {
		auto textureNames = textureData.getMemberNames();
		int textureNum = textureNames.size();
		if ((textureNum == 3) && (textureData["up"].isString() && textureData["down"].isString() && textureData["side"].isString())) {
			block.setTextureItem(textureData["up"].asString(), textureData["down"].asString(), textureData["side"].asString());
			return true;
		}
		if ((textureNum == 6) &&
			(textureData["up"].isString() && textureData["down"].isString() && textureData["north"].isString() &&
				textureData["south"].isString() && textureData["west"].isString() && textureData["east"].isString())) {
			block.setTextureItem(textureData["up"].asString(), textureData["down"].asString(), textureData["north"].asString(),
				textureData["south"].asString(), textureData["west"].asString(), textureData["east"].asString());
			return true;
		}
	}
	return false;
}

bool BlockGraphics::setCarriedTextures(BlockGraphics& block, const Json::Value& textureData)
{
	if (textureData.isNull())
		return true;

	if (textureData.isString()) {
		block.setCarriedTextureItem(textureData.asString());
		return true;
	}

	if (textureData.isObject()) {
		auto textureNames = textureData.getMemberNames();
		int textureNum = textureNames.size();
		if ((textureNum == 3) && (textureData["up"].isString() && textureData["down"].isString() && textureData["side"].isString())) {
			block.setCarriedTextureItem(textureData["up"].asString(), textureData["down"].asString(), textureData["side"].asString());
			return true;
		}
		if ((textureNum == 6) &&
			(textureData["up"].isString() && textureData["down"].isString() && textureData["north"].isString() &&
				textureData["south"].isString() && textureData["west"].isString() && textureData["east"].isString())) {
			block.setCarriedTextureItem(textureData["up"].asString(), textureData["down"].asString(), textureData["north"].asString(),
				textureData["south"].asString(), textureData["west"].asString(), textureData["east"].asString());
			return true;
		}
	}
	return false;
}


bool BlockGraphics::setTextureIsotropic(BlockGraphics& block, const Json::Value& isotropicData)
{
	if (isotropicData.isNull())
		return true;

	if (isotropicData.isBool() && isotropicData.asBool() == true) {
		block.setAllFacesIsotropic();
		return true;
	}

	if (isotropicData.isObject()) {
		static std::map<std::string, int> faceIsotropicMap_3 = {
			{ "up",			1 << Facing::UP },
			{ "down",		1 << Facing::DOWN },
			{ "side",		1 << Facing::NORTH | 1 << Facing::SOUTH | 1 << Facing::WEST | 1 << Facing::EAST }
		};
		static std::map<std::string, int> faceIsotropicMap_6 = {
			{ "up",			1 << Facing::UP },
			{ "down",		1 << Facing::DOWN },
			{ "north",		1 << Facing::NORTH },
			{ "south",		1 << Facing::SOUTH },
			{ "west",		1 << Facing::WEST },
			{ "east",		1 << Facing::EAST }
		};

		int isotropicValue = 0x00;
		int count = isotropicData.getMemberNames().size();

		for (auto& a : isotropicData["side"].isNull() ? faceIsotropicMap_6 : faceIsotropicMap_3) {
			if (isotropicData[a.first].isBool()) {
				count--;
				if (isotropicData[a.first].asBool() == true) {
					isotropicValue |= a.second;
				}
			}
		}

		block.setTextureIsotropic(isotropicValue);
		return count == 0;
	}
	return false;
}

bool BlockGraphics::setBlockShape(BlockGraphics& block, const Json::Value& blockShapeData) {
	static const std::map<std::string, BlockShape> nameToBlockShape = {
		{ "invisible",			BlockShape::INVISIBLE },
		{ "block",				BlockShape::BLOCK },
		{ "cross_texture",		BlockShape::CROSS_TEXTURE },
		{ "torch",				BlockShape::TORCH },
		{ "fire",				BlockShape::FIRE },
		{ "water",				BlockShape::WATER },
		{ "red_dust",			BlockShape::RED_DUST },
		{ "rows",				BlockShape::ROWS },
		{ "door",				BlockShape::DOOR },
		{ "ladder",				BlockShape::LADDER },
		{ "rail",				BlockShape::RAIL },
		{ "stairs",				BlockShape::STAIRS },
		{ "fence",				BlockShape::FENCE },
		{ "lever",				BlockShape::LEVER },
		{ "cactus",				BlockShape::CACTUS },
		{ "bed",				BlockShape::BED },
		{ "diode",				BlockShape::DIODE },
		{ "iron_fence",			BlockShape::IRON_FENCE },
		{ "stem",				BlockShape::STEM },
		{ "vine",				BlockShape::VINE },
		{ "fence_gate",			BlockShape::FENCE_GATE },
		{ "chest",				BlockShape::CHEST },
		{ "lilypad",			BlockShape::LILYPAD },
		{ "brewing_stand",		BlockShape::BREWING_STAND },
		{ "portal_frame",		BlockShape::PORTAL_FRAME },
		{ "cocoa",				BlockShape::COCOA },
		{ "tree",				BlockShape::TREE },
		{ "wall",				BlockShape::WALL },
		{ "double_plant",		BlockShape::DOUBLE_PLANT },
		{ "flower_pot",			BlockShape::FLOWER_POT },
		{ "anvil",				BlockShape::ANVIL },
		{ "cross_texture_poly",	BlockShape::CROSS_TEXTURE_POLY },
		{ "double_plant_poly",	BlockShape::DOUBLE_PLANT_POLY },
		{ "block_half",			BlockShape::BLOCK_HALF },
		{ "top_snow",			BlockShape::TOP_SNOW },
		{ "tripwire",			BlockShape::TRIPWIRE },
		{ "tripwire_hook",		BlockShape::TRIPWIRE_HOOK },
		{ "cauldron",			BlockShape::CAULDRON },
		{ "repeater",			BlockShape::REPEATER },
		{ "comparator",			BlockShape::COMPARATOR },
		{ "hopper",				BlockShape::HOPPER },
		{ "slime_block",		BlockShape::SLIME_BLOCK },
		{ "piston",				BlockShape::PISTON },
		{ "beacon",				BlockShape::BEACON},
		{ "chorus_plant",		BlockShape::CHORUS_PLANT },
		{ "chorus_flower",		BlockShape::CHORUS_FLOWER },
		{ "dragon_egg",		    BlockShape::DRAGON_EGG },
		{ "end_portal",			BlockShape::END_PORTAL },
		{ "end_rod",            BlockShape::END_ROD },
		{ "structure_void",	    BlockShape::STRUCTURE_VOID }
	};

	if (blockShapeData.isNull()) {
		return true;
	}

	if (blockShapeData.isString()) {
		auto elem = nameToBlockShape.find(blockShapeData.asString());
		if (elem != nameToBlockShape.end()) {
			block.setBlockShape(elem->second);
			return true;
		}
	}
	return false;
}

bool BlockGraphics::setBrightnessGamma(BlockGraphics& block, const Json::Value& data) {
	if (data.isNull())
		return true;
	if (data.isNumeric()) {
		block.setBrightnessGamma(data.asFloat());
		return true;

	}
	return false;
}

bool BlockGraphics::setSoundType(BlockGraphics& block, const Json::Value& data) {
	if (!data.isNull()) {
		BlockSoundType type = BlockSoundTypeFromString(data.asString("normal"));
		if (type != BlockSoundType::Undefined) {
			block.setSoundType(type);
		}
	}
	return true;
}

BlockGraphics& registerBlockGraphics(const Json::Value& list, const std::string& blockName) {
	auto block = make_unique<BlockGraphics>(blockName);

	auto actualID = block->mID;
	DEBUG_ASSERT(actualID >= 0 && actualID <= 0xff, "Invalid ID value");

	auto ptr = block.get();

	BlockGraphics::mOwnedBlocks.emplace_back(std::move(block));
	BlockGraphics::mBlocks[actualID] = ptr;

	Json::Value blockData = list[blockName];
	if (false == BlockGraphics::setTextures(*ptr, blockData["textures"])) {
		LOGE("Loading block data from Json :  \"%s\" : fail to set textures\n", blockName.c_str());
	}
	if (false == BlockGraphics::setCarriedTextures(*ptr, blockData["carried_textures"])) {
		LOGE("Loading block data from Json :  \"%s\" : fail to set carried textures\n", blockName.c_str());
	}
	if (false == BlockGraphics::setTextureIsotropic(*ptr, blockData["isotropic"])) {
		LOGE("Loading block data from Json :  \"%s\" : Error in setting texture isotropic\n", blockName.c_str());
	}
	if (false == BlockGraphics::setBlockShape(*ptr, blockData["blockshape"])) {
		LOGE("Loading block data from Json :  \"%s\" : fail to set block shape\n", blockName.c_str());
	}
	if (false == BlockGraphics::setBrightnessGamma(*ptr, blockData["brightness_gamma"])) {
		LOGE("Loading block data from Json :  \"%s\" : fail to set brightness gamma\n", blockName.c_str());
	}
	if (false == BlockGraphics::setSoundType(*ptr, blockData["sound"])) {
		LOGE("Loading block data from Json :  \"%s\" : fail to set sounds \n", blockName.c_str());
	}
	return *ptr;
}

/*static*/
void BlockGraphics::initBlocks() {
	mOwnedBlocks.clear();
	memset(mBlocks, 0, sizeof(mBlocks));//null all the blocks

	Json::Value json;
	Json::Reader reader;
// 	std::string fileContent;

	// TODO: rherlitz
// 	Resource::load(ResourceLocation("blocks.json"), fileContent);
	FString path = FPaths::GameDir() + "Content/data/resourcepacks/vanilla/blocks.json";
	std::ifstream fileContent(*path);
	JSONReadVerifier::verifyRead("blocks.json", reader, reader.parse(fileContent, json));

	registerBlockGraphics(json, "air");
	registerBlockGraphics(json, "stone");
	registerBlockGraphics(json, "grass").setMapColor(Color::fromARGB(0x7FB238));
	registerBlockGraphics(json, "dirt").setMapColor(Color::fromARGB(0x976D4D));
	registerBlockGraphics(json, "cobblestone");
	registerBlockGraphics(json, "planks");
	registerBlockGraphics(json, "sapling");
	registerBlockGraphics(json, "bedrock");
	registerBlockGraphics(json, "flowing_water");
	registerBlockGraphics(json, "water");
	registerBlockGraphics(json, "flowing_lava");
	registerBlockGraphics(json, "lava");
	registerBlockGraphics(json, "sand");
	registerBlockGraphics(json, "gravel");
	registerBlockGraphics(json, "gold_ore");
	registerBlockGraphics(json, "iron_ore");
	registerBlockGraphics(json, "coal_ore");
	registerBlockGraphics(json, "log");
	registerBlockGraphics(json, "leaves");

#ifdef MCPE_EDU
	registerBlockGraphics(json, "allow");
	registerBlockGraphics(json, "deny");
	registerBlockGraphics(json, "border_block");
	registerBlockGraphics(json, "chalkboard");
	registerBlockGraphics(json, "camera");
#endif

	registerBlockGraphics(json, "sponge");
	registerBlockGraphics(json, "glass");
	registerBlockGraphics(json, "lapis_ore");
	registerBlockGraphics(json, "lapis_block").setMapColor(Color::fromARGB(0x4A80FF));

	registerBlockGraphics(json, "dispenser");

	registerBlockGraphics(json, "sandstone");
	registerBlockGraphics(json, "noteblock");
	registerBlockGraphics(json, "bed");
	registerBlockGraphics(json, "golden_rail");
	registerBlockGraphics(json, "detector_rail");
	registerBlockGraphics(json, "sticky_piston");
	registerBlockGraphics(json, "web");
	registerBlockGraphics(json, "tallgrass");
	registerBlockGraphics(json, "deadbush");
	registerBlockGraphics(json, "piston");
	registerBlockGraphics(json, "pistonArmCollision");

	registerBlockGraphics(json, "wool");

	registerBlockGraphics(json, "yellow_flower");
	registerBlockGraphics(json, "red_flower");
	registerBlockGraphics(json, "brown_mushroom");
	registerBlockGraphics(json, "red_mushroom");
	registerBlockGraphics(json, "gold_block").setMapColor(Color::fromARGB(0xFAEE4D));
	registerBlockGraphics(json, "iron_block").setMapColor(Color::fromARGB(0xA7A7A7));
	registerBlockGraphics(json, "double_stone_slab");
	registerBlockGraphics(json, "stone_slab");
	registerBlockGraphics(json, "brick_block").setMapColor(Color::fromARGB(0x993333));
	registerBlockGraphics(json, "tnt");
	registerBlockGraphics(json, "bookshelf");
	registerBlockGraphics(json, "mossy_cobblestone");
	registerBlockGraphics(json, "obsidian");
	registerBlockGraphics(json, "torch");

	registerBlockGraphics(json, "mob_spawner");

	registerBlockGraphics(json, "oak_stairs");
	registerBlockGraphics(json, "chest");
	registerBlockGraphics(json, "redstone_wire");

	registerBlockGraphics(json, "diamond_ore");
	registerBlockGraphics(json, "diamond_block").setMapColor(Color::fromARGB(0x5CDBD5));
	registerBlockGraphics(json, "crafting_table");
	registerBlockGraphics(json, "wheat");
	registerBlockGraphics(json, "farmland");
	registerBlockGraphics(json, "furnace");
	registerBlockGraphics(json, "lit_furnace");
	registerBlockGraphics(json, "standing_sign");
	registerBlockGraphics(json, "wooden_door");
	registerBlockGraphics(json, "ladder");
	registerBlockGraphics(json, "rail");
	registerBlockGraphics(json, "stone_stairs");
	registerBlockGraphics(json, "wall_sign");
	registerBlockGraphics(json, "lever");
	registerBlockGraphics(json, "stone_pressure_plate").setVisualShape(Vec3(0.4), Vec3(0.6)); //HACK hardcode a small shape for now

	registerBlockGraphics(json, "iron_door");

	registerBlockGraphics(json, "wooden_pressure_plate").setVisualShape(Vec3(0.4), Vec3(0.6)); //HACK hardcode a small shape for now

	registerBlockGraphics(json, "redstone_ore");
	registerBlockGraphics(json, "lit_redstone_ore");
	registerBlockGraphics(json, "unlit_redstone_torch");
	registerBlockGraphics(json, "redstone_torch");
	registerBlockGraphics(json, "stone_button").setVisualShape(Vec3(0.4), Vec3(0.6)); //HACK hardcode a small shape for now

	registerBlockGraphics(json, "snow_layer");
	registerBlockGraphics(json, "ice");
	registerBlockGraphics(json, "snow");
	registerBlockGraphics(json, "cactus");
	registerBlockGraphics(json, "clay");
	registerBlockGraphics(json, "reeds");
	registerBlockGraphics(json, "fence").setMapColor(WoodBlock::getWoodMaterialColor(WoodBlockType::Oak));
	registerBlockGraphics(json, "nether_brick_fence").setMapColor(Color::fromARGB(0x700200));

	registerBlockGraphics(json, "pumpkin");
	registerBlockGraphics(json, "netherrack");
	registerBlockGraphics(json, "soul_sand").setMapColor(Color::fromARGB(0x664C33));

	registerBlockGraphics(json, "glowstone");
	registerBlockGraphics(json, "portal");
	registerBlockGraphics(json, "lit_pumpkin");
	registerBlockGraphics(json, "cake").setVisualShape(Vec3(0.0f), Vec3(1.0f, 0.5f, 1.0f)); // Hack hard coded shape for now

	registerBlockGraphics(json, "unpowered_repeater");
	registerBlockGraphics(json, "powered_repeater");

	registerBlockGraphics(json, "invisibleBedrock");
	registerBlockGraphics(json, "trapdoor");

	registerBlockGraphics(json, "monster_egg");
	registerBlockGraphics(json, "stonebrick");
	registerBlockGraphics(json, "brown_mushroom_block").setMapColor(Color::fromARGB(0x976D4D));
	registerBlockGraphics(json, "red_mushroom_block").setMapColor(Color::fromARGB(0x993333));

	// This should be called fenceIron but the iron part is cut out of the crafting screens
	registerBlockGraphics(json, "iron_bars");
	registerBlockGraphics(json, "glass_pane");
	registerBlockGraphics(json, "melon_block").setMapColor(Color::fromARGB(0x7FCC19));
	registerBlockGraphics(json, "pumpkin_stem");
	registerBlockGraphics(json, "melon_stem");
	registerBlockGraphics(json, "vine");
	registerBlockGraphics(json, "fence_gate").setMapColor(WoodBlock::getWoodMaterialColor(WoodBlockType::Oak));
	registerBlockGraphics(json, "brick_stairs");
	registerBlockGraphics(json, "mycelium").setMapColor(Color::fromARGB(0x7F3FB2));
	registerBlockGraphics(json, "waterlily");

	registerBlockGraphics(json, "brewing_stand");
	registerBlockGraphics(json, "cauldron");

	registerBlockGraphics(json, "end_portal").setBlockShape(BlockShape::END_PORTAL);
	registerBlockGraphics(json, "end_portal_frame").setMapColor(Color::fromARGB(0x667F33));

	registerBlockGraphics(json, "end_bricks");
	registerBlockGraphics(json, "end_stone");
	registerBlockGraphics(json, "end_rod");
	registerBlockGraphics(json, "redstone_lamp");
	registerBlockGraphics(json, "lit_redstone_lamp");
	registerBlockGraphics(json, "cocoa");

	registerBlockGraphics(json, "emerald_ore");
	registerBlockGraphics(json, "emerald_block").setMapColor(Color::fromARGB(0x00D93A));

	registerBlockGraphics(json, "spruce_stairs");
	registerBlockGraphics(json, "birch_stairs");
	registerBlockGraphics(json, "jungle_stairs");

	registerBlockGraphics(json, "beacon");

	registerBlockGraphics(json, "wooden_button").setVisualShape(Vec3(0.4), Vec3(0.6)); //HACK hardcode a small shape for now

	registerBlockGraphics(json, "stone_brick_stairs");
	registerBlockGraphics(json, "nether_brick").setBrightnessGamma(2.f);
	registerBlockGraphics(json, "nether_brick_stairs");
	registerBlockGraphics(json, "nether_wart").setMapColor(Color::fromARGB(0x993333));
	registerBlockGraphics(json, "enchanting_table").setMapColor(Color::fromARGB(0x993333));

	// Switched places from PC Minecraft
	registerBlockGraphics(json, "dropper");
	registerBlockGraphics(json, "activator_rail");
	registerBlockGraphics(json, "sandstone_stairs");
	registerBlockGraphics(json, "ender_chest");

	registerBlockGraphics(json, "tripwire_hook");
	registerBlockGraphics(json, "tripWire");

	registerBlockGraphics(json, "cobblestone_wall");
	registerBlockGraphics(json, "flower_pot");
	registerBlockGraphics(json, "carrots");
	registerBlockGraphics(json, "potatoes");

	registerBlockGraphics(json, "skull");
	registerBlockGraphics(json, "anvil");
	registerBlockGraphics(json, "trapped_chest");

	registerBlockGraphics(json, "light_weighted_pressure_plate").setVisualShape(Vec3(0.4), Vec3(0.6)); //HACK hardcode a small shape for now
	registerBlockGraphics(json, "heavy_weighted_pressure_plate").setVisualShape(Vec3(0.4), Vec3(0.6)); //HACK hardcode a small shape for now

	registerBlockGraphics(json, "unpowered_comparator");
	registerBlockGraphics(json, "powered_comparator");

	registerBlockGraphics(json, "daylight_detector").setVisualShape(Vec3(0.0f), Vec3(1.0f, 6.0f / 16.0f, 1.0f));
	registerBlockGraphics(json, "redstone_block").setMapColor(Color::fromARGB(0xff0000));
	registerBlockGraphics(json, "quartz_ore").setMapColor(Color::fromARGB(0x700200));

	registerBlockGraphics(json, "hopper");

	registerBlockGraphics(json, "quartz_block");
	registerBlockGraphics(json, "quartz_stairs");

	registerBlockGraphics(json, "purpur_block");
	registerBlockGraphics(json, "purpur_stairs");

	registerBlockGraphics(json, "chorus_plant");
	registerBlockGraphics(json, "chorus_flower");

	registerBlockGraphics(json, "double_wooden_slab");
	registerBlockGraphics(json, "wooden_slab");
	registerBlockGraphics(json, "stained_hardened_clay");

	registerBlockGraphics(json, "leaves2");
	registerBlockGraphics(json, "log2");

	registerBlockGraphics(json, "acacia_stairs");
	registerBlockGraphics(json, "dark_oak_stairs");
	registerBlockGraphics(json, "slime");

	registerBlockGraphics(json, "iron_trapdoor");

	registerBlockGraphics(json, "hay_block").setMapColor(Color::fromARGB(0xE5E533));
	registerBlockGraphics(json, "carpet").setVisualShape(Vec3(0.4), Vec3(0.6)); //HACK hardcode a small shape for now;

	registerBlockGraphics(json, "hardened_clay");
	registerBlockGraphics(json, "coal_block").setMapColor(Color::fromARGB(0x191919));
	registerBlockGraphics(json, "packed_ice");
	registerBlockGraphics(json, "double_plant").setBlockShape(BlockShape::DOUBLE_PLANT);

	registerBlockGraphics(json, "daylight_detector_inverted").setVisualShape(Vec3(0.0f), Vec3(1.0f, 6.0f / 16.0f, 1.0f));
	registerBlockGraphics(json, "red_sandstone");
	registerBlockGraphics(json, "red_sandstone_stairs");
	registerBlockGraphics(json, "double_stone_slab2");
	registerBlockGraphics(json, "stone_slab2");

	registerBlockGraphics(json, "spruce_fence_gate");
	registerBlockGraphics(json, "birch_fence_gate");
	registerBlockGraphics(json, "jungle_fence_gate");
	registerBlockGraphics(json, "dark_oak_fence_gate");
	registerBlockGraphics(json, "acacia_fence_gate");

	registerBlockGraphics(json, "spruce_door");
	registerBlockGraphics(json, "birch_door");
	registerBlockGraphics(json, "jungle_door");
	registerBlockGraphics(json, "acacia_door");
	registerBlockGraphics(json, "dark_oak_door");

	registerBlockGraphics(json, "dragon_egg");

	registerBlockGraphics(json, "grass_path");

	registerBlockGraphics(json, "frame");
	registerBlockGraphics(json, "end_gateway").setBlockShape(BlockShape::END_GATEWAY);

	registerBlockGraphics(json, "prismarine");
	registerBlockGraphics(json, "seaLantern");

	//registerBlockGraphics(json, "structure_block");
	//registerBlockGraphics(json, "structure_void");

	//
	// Special blocks for Pocket Edition is placed at high IDs
	//
	registerBlockGraphics(json, "podzol").setMapColor(Color::fromARGB(0x976D4D));

	
	registerBlockGraphics(json, "beetroot");
	registerBlockGraphics(json, "stonecutter");
	registerBlockGraphics(json, "glowingobsidian");
	registerBlockGraphics(json, "netherreactor");
	registerBlockGraphics(json, "info_update");
	registerBlockGraphics(json, "info_update2");
	registerBlockGraphics(json, "movingBlock");
	registerBlockGraphics(json, "observer");

	registerBlockGraphics(json, "stained_glass");
	registerBlockGraphics(json, "stained_glass_pane");

	//leaves_carried = (const LeafBlock*) (new LeafBlock>(254, "leaves_oak").setDestroyTime(0.2f).setLightBlock(1).setSoundType(SOUND_GRASS).setCategory(ItemCategory::Structures).setNameId("leaves");
	registerBlockGraphics(json, "reserved6"); //BG_todo: not used anywhere

	//
	// Stuff that need to be initialized in a specific order (i.e. after the other blocks have been created)
	//
	registerBlockGraphics(json, "fire");

	//add missing blocks
	//TODO start at 0, add a air block!
	for (int i = 1; i < 256; i++) {
        bool missingBlock = false;
        if(mBlocks[i] == nullptr) {
            missingBlock = true;
            // NOTE (venvious): Using [i] as the name because having an empty name is not allowed
            // (Also having the same name is not allowed either)
            registerBlockGraphics(json, Util::toString(i));
        }
        else {
            for(int j=0; j<6 && !missingBlock; ++j) {
                if(mBlocks[i]->mUVItem[j].getName().empty()) {
                    missingBlock = true;
                }
            }
        }
		if (missingBlock) {
			mBlocks[i]->setTextureItem("missing_tile");
		}
	}
}

/*static*/
void BlockGraphics::teardownBlocks() {
	for (int i = 0; i < 256; ++i) {
		mBlocks[i] = nullptr;
	}

	mOwnedBlocks.clear();
	mBlockLookupMap.clear();
}

void BlockGraphics::reloadBlockUVs(TextureAtlas& changed) {
	if (&changed != mTerrainTextureAtlas.get()) {
		return; //not us
	}

	//reload all block UVs
	for (auto&& block : mBlocks) {
		if (block) {
			for (auto& facing : Facing::DIRECTIONS) {
				auto i = enum_cast(facing);

				if (block->mUVItemName[i].size() > 0) {
					block->mUVItem[i] = getTextureItem(block->mUVItemName[i]);
				}

				if (block->mUVCarriedName[i].size() > 0) {
					block->mUVCarried[i] = getTextureItem(block->mUVCarriedName[i]);
				}
			}
		}
	}
}

const Block* BlockGraphics::getBlock() const {
	return mBlock;
}

BlockGraphics::BlockGraphics(const std::string& nameId)
	: mID(0)
	, mSoundType(BlockSoundType::Normal)
	, mBlockShape(BlockShape::BLOCK) {

	mBlock = Block::lookupByName(nameId);
	DEBUG_ASSERT(mBlock != nullptr, "Invalid Block Graphics name");
	mID = mBlock->mID;
	DEBUG_ASSERT(!BlockGraphics::mBlocks[mID], "Slot is already occupied");
}

BlockGraphics::~BlockGraphics() {
}

Color BlockGraphics::getMapColor(const FullBlock& block) const {
	return mMapColor;
}

Color BlockGraphics::getMapColor() const {
	return mMapColor;
}

void BlockGraphics::animateTick(BlockSource& region, const BlockPos& pos, Random& random) {
}

bool BlockGraphics::shouldRenderFace(BlockSource& region, const BlockPos& pos, FacingID face, const AABB& shape) const {
	// Common
	if ((face == Facing::DOWN && shape.min.y > 0) ||
		(face == Facing::UP && shape.max.y < 1) ||
		(face == Facing::NORTH && shape.min.z > 0) ||
		(face == Facing::SOUTH && shape.max.z < 1) ||
		(face == Facing::WEST && shape.min.x > 0) ||
		(face == Facing::EAST && shape.max.x < 1)) {
		return true;
	}

	const auto& fullBlockData = region.getBlockAndData(pos);
	BlockGraphics* bg = BlockGraphics::mBlocks[fullBlockData.id];
	const Block* block = Block::mBlocks[fullBlockData.id];

	if (bg == nullptr || block == nullptr) {
		return true;
	}

	if (block == Block::mInvisibleBedrock) {	//this hack is needed because unbreakable is not solid but we don't want it to render
		return false;	//we should really have two different flags for "logically solid" and "fully opaque", leaves are not solid pls
	}

	//HACK this code most definitely doesn't belong here
	if (block->hasProperty(BlockProperty::Leaf)) {
		return !LeafBlock::isDeepLeafBlock(region, pos);
	}

	return (face == Facing::UP && (block == Block::mTopSnow || block == Block::mWoolCarpet)) ? false : !block->isSolid();
}

const TextureUVCoordinateSet& BlockGraphics::getTexture(FacingID face, int variant) const {
	if (variant < 0 || variant >= (int)mUVItem[face].uvCount()) {
		variant = 0;
	}
	return mUVItem[face][variant];
}

const TextureAtlasItem& BlockGraphics::getAtlasItem(FacingID face) const {
	return mUVItem[face];
}

const TextureUVCoordinateSet& BlockGraphics::getTexture(FacingID face) const {
	return mUVItem[face][0];
}

const TextureUVCoordinateSet& BlockGraphics::getCarriedTexture(FacingID face, int variant) const {
	if (mUVCarried[face].uvCount() > 0) {
		if (variant < 0 || variant >= (int)mUVCarried[face].uvCount()) {
			variant = 0;
		}
		return mUVCarried[face][variant];
	}
	else {
		return getTexture(face, variant);
	}
}

bool BlockGraphics::isTextureIsotropic(FacingID face) const {
	return ((mFaceTextureIsotropic >> face) & 0x1) == 1;
}

Vec3 BlockGraphics::randomlyModifyPosition(const BlockPos& pos, int32_t& seed) const {
	seed = 0;
	return randomlyModifyPosition(pos);
}

Vec3 BlockGraphics::randomlyModifyPosition(const BlockPos& pos) const {
	return pos;
}

int BlockGraphics::getColor(int auxData) const {
	return 0xffffffff;
}

int BlockGraphics::getColor(BlockSource& region, const BlockPos& pos) const {
	return 0xffffffff;
}

int BlockGraphics::getColorForParticle(BlockSource& region, const BlockPos& pos, int auxData) const {
	return getColor(region, pos);
}

bool BlockGraphics::isSeasonTinted(BlockSource& region, const BlockPos& p) const {
	return false;
}

BlockGraphics& BlockGraphics::setSoundType(BlockSoundType type) {
	mSoundType = type;
	return *this;
}

BlockGraphics& BlockGraphics::setMapColor(const Color& color) {
	mMapColor = color;
	return *this;
}

BlockGraphics& BlockGraphics::setTextureItem(const std::string& name) {
	return setTextureItem(name, name, name, name, name, name);
}

BlockGraphics& BlockGraphics::setCarriedTextureItem(const std::string& name) {
	return setCarriedTextureItem(name, name, name);
}


BlockGraphics& BlockGraphics::setTextureItem(const std::string& nameUp, const std::string& nameDown, const std::string& nameSide) {
	return setTextureItem(nameUp, nameDown, nameSide, nameSide, nameSide, nameSide);
}

BlockGraphics& BlockGraphics::setTextureItem(const std::string& nameUp, const std::string& nameDown, const std::string& nameNorth, const std::string& nameSouth, const std::string& nameWest, const std::string& nameEast) {
	mUVItem[Facing::UP] = getTextureItem(mUVItemName[Facing::UP] = nameUp);
	mUVItem[Facing::DOWN] = getTextureItem(mUVItemName[Facing::DOWN] = nameDown);
	mUVItem[Facing::NORTH] = getTextureItem(mUVItemName[Facing::NORTH] = nameNorth);
	mUVItem[Facing::SOUTH] = getTextureItem(mUVItemName[Facing::SOUTH] = nameSouth);
	mUVItem[Facing::WEST] = getTextureItem(mUVItemName[Facing::WEST] = nameWest);
	mUVItem[Facing::EAST] = getTextureItem(mUVItemName[Facing::EAST] = nameEast);
	return *this;
}

BlockGraphics& BlockGraphics::setCarriedTextureItem(const std::string& nameUp, const std::string& nameDown, const std::string& nameSide) {
	return setCarriedTextureItem(nameUp, nameDown, nameSide, nameSide, nameSide, nameSide);
}

BlockGraphics& BlockGraphics::setCarriedTextureItem(const std::string& nameUp, const std::string& nameDown, const std::string& nameNorth, const std::string& nameSouth, const std::string& nameWest, const std::string& nameEast) {
	mUVCarried[Facing::UP] = getTextureItem(mUVCarriedName[Facing::UP] = nameUp);
	mUVCarried[Facing::DOWN] = getTextureItem(mUVCarriedName[Facing::DOWN] = nameDown);
	mUVCarried[Facing::NORTH] = getTextureItem(mUVCarriedName[Facing::NORTH] = nameNorth);
	mUVCarried[Facing::SOUTH] = getTextureItem(mUVCarriedName[Facing::SOUTH] = nameSouth);
	mUVCarried[Facing::WEST] = getTextureItem(mUVCarriedName[Facing::WEST] = nameWest);
	mUVCarried[Facing::EAST] = getTextureItem(mUVCarriedName[Facing::EAST] = nameEast);
	return *this;
}
void BlockGraphics::setVisualShape(const Vec3& min, const Vec3& max) {
	mVisualShape.set(min, max);
}

void BlockGraphics::setVisualShape(const AABB& shape) {
	mVisualShape.set(shape);
}

void BlockGraphics::onGraphicsModeChanged(bool fancy, bool preferPolyTessellation) {
	mFancy = fancy;

	if (mID == Block::mTallgrass->mID) {
		if (preferPolyTessellation) {
			mBlockShape = BlockShape::CROSS_TEXTURE_POLY;
		}
		else {
			mBlockShape = BlockShape::CROSS_TEXTURE;
		}
	}
	else if (mID == Block::mDoublePlant->mID) {
		if (preferPolyTessellation) {
			mBlockShape = BlockShape::DOUBLE_PLANT_POLY;
		}
		else {
			mBlockShape = BlockShape::DOUBLE_PLANT;
		}
	}

}

TextureUVCoordinateSet BlockGraphics::getTextureUVCoordinateSet(const std::string& name, int id) {
	return BlockGraphics::mTerrainTextureAtlas->getTextureItem(name)[id];
}

const TextureAtlasItem& BlockGraphics::getTextureItem(const std::string& name) {
	return BlockGraphics::mTerrainTextureAtlas->getTextureItem(name);
}

BlockShape BlockGraphics::getBlockShape() const {
	return mBlockShape;
}

BlockGraphics& BlockGraphics::setBlockShape(BlockShape shape) {
	mBlockShape = shape;
	return *this;
}

BlockGraphics& BlockGraphics::setAllFacesIsotropic() {
	mFaceTextureIsotropic = 0xff;
	return *this;
}


int BlockGraphics::getIconYOffset() const {
	return 0;
}

bool BlockGraphics::isAlphaTested() {
	auto rl = mRenderLayer;
	return rl == RENDERLAYER_ALPHATEST || rl == RENDERLAYER_ALPHATEST_SINGLE_SIDE || (mFancy && rl == RENDERLAYER_OPTIONAL_ALPHATEST);
}

BlockGraphics::BlockRenderLayer BlockGraphics::getRenderLayer(BlockSource& region, const BlockPos& pos) const {
	return static_cast<BlockGraphics::BlockRenderLayer>(getBlock()->getRenderLayer());
	// REFACTOR TODO: Replace with line below when BlockGraphics BlockRenderLayer handling is finished
	// return mRenderLayer;
}

BlockGraphics::BlockRenderLayer BlockGraphics::getRenderLayer() const {
	return static_cast<BlockGraphics::BlockRenderLayer>(getBlock()->getRenderLayer());
	// REFACTOR TODO: Replace with line below when BlockGraphics BlockRenderLayer handling is finished
	// return mRenderLayer;
}

int BlockGraphics::getExtraRenderLayers() {
	return 0;
}

bool BlockGraphics::isFullAndOpaque() {
	if ((getBlockShape() == BlockShape::BLOCK || getBlockShape() == BlockShape::TREE) && (getRenderLayer() == BlockRenderLayer::RENDERLAYER_OPAQUE || getRenderLayer() == BlockRenderLayer::RENDERLAYER_SEASONS_OPAQUE)) {
		//use the visual shape to make sure that this block is actually full
		//TODO do *not* use getBlockShape() for this. Only the actual Visual Shape should decide this.
		AABB bbb;
		bbb = getVisualShape(0, bbb);
		return (bbb.getBounds() == Vec3::ONE);
	}

	return false;
}

// static
bool BlockGraphics::isFullAndOpaque(const Block& block) {
	BlockGraphics *graphics = mBlocks[block.getId()];
	DEBUG_ASSERT(graphics != nullptr, "Invalid BlockGraphics for block");
	return graphics->isFullAndOpaque();
}

const AABB& BlockGraphics::getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping) {
	return getVisualShape(region.getData(pos), bufferAABB, false);
}

const AABB& BlockGraphics::getVisualShape(DataID data, AABB& bufferAABB, bool isClipping	/* = false*/) {
	return mVisualShape;
}

unsigned int BlockGraphics::getFaceTextureIsotropic() {
	return mFaceTextureIsotropic;
}

BlockGraphics& BlockGraphics::setTextureIsotropic(unsigned int faces) {
	mFaceTextureIsotropic = faces;
	return *this;
}

BlockSoundType  BlockGraphics::getSoundType() const {
	return mSoundType;
}


BlockGraphics* BlockGraphics::lookupByName(const std::string& name, bool caseInsensitive)
{
	// If name is empty, always return null.
	if (name.empty())
		return nullptr;

	// All names in map are lower case (so input must always be lower case)
	std::string testName = caseInsensitive ? Util::toLower(name) : name;

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

	//LOGE("Block::LookupByName() Failed to find block[%s]", name.c_str());

	return nullptr;
}