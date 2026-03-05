#include "stdafx.h"

#include "Packet.h"
#include "MaterialColor.h"
#include "Material.h"
#include "Tile.h"
#include "HatchetItem.h"
#include "PickaxeItem.h"
#include "ShovelItem.h"
#include "BlockReplacements.h"
#include "Biome.h"
#include "Item.h"
#include "FurnaceRecipes.h"
#include "Recipes.h"
#include "Stats.h"
#include "Achievements.h"
#include "Skeleton.h"
#include "PigZombie.h"
#include "TileEntity.h"
#include "EntityIO.h"
#include "SharedConstants.h"
#include "MobCategory.h"
#include "LevelChunk.h"
#include "MineShaftPieces.h"
#include "StrongholdFeature.h"
#include "VillageFeature.h"
#include "LevelType.h"
#include "EnderMan.h"
#include "PotionBrewing.h"
#include "Enchantment.h"
#include "VillagePieces.h"
#include "RandomScatteredLargeFeature.h"

#include "Minecraft.World.h"
#include "..\Minecraft.Client\ServerLevel.h"
#include "SparseLightStorage.h"
#include "SparseDataStorage.h"
#include "McRegionChunkStorage.h"
#include "Villager.h"
#include "LevelSettings.h"

#include "CommonStats.h"

void MinecraftWorld_RunStaticCtors()
{
	// The ordering of these static ctors can be important. If they are within statement blocks then
	// DO NOT CHANGE the ordering - 4J Stu

	Packet::staticCtor();

	{
		MaterialColor::staticCtor();
		Material::staticCtor();
		Tile::staticCtor();
		HatchetItem::staticCtor();
		PickaxeItem::staticCtor();
		ShovelItem::staticCtor();
		BlockReplacements::staticCtor();
		Biome::staticCtor();	
		Item::staticCtor();
		FurnaceRecipes::staticCtor();
		Recipes::staticCtor();	
		GenericStats::setInstance(new CommonStats());
		Stats::staticCtor();
		//Achievements::staticCtor(); // 4J Stu - This is now called from within the Stats::staticCtor()
		Skeleton::staticCtor();
		PigZombie::staticCtor();
		TileEntity::staticCtor();
		EntityIO::staticCtor();
		MobCategory::staticCtor();

		Item::staticInit();
		LevelChunk::staticCtor();

		LevelType::staticCtor();

		MineShaftPieces::staticCtor();
		StrongholdFeature::staticCtor();
		VillagePieces::Smithy::staticCtor();
		VillageFeature::staticCtor();
		RandomScatteredLargeFeature::staticCtor();
	}
	EnderMan::staticCtor();
	PotionBrewing::staticCtor();
	Enchantment::staticCtor();

	SharedConstants::staticCtor();

	ServerLevel::staticCtor();
	SparseLightStorage::staticCtor();
	CompressedTileStorage::staticCtor();
	SparseDataStorage::staticCtor();
	McRegionChunkStorage::staticCtor();
	Villager::staticCtor();
	GameType::staticCtor();
}
