#pragma once
using namespace std;
class InputStream;

// This file aims to provide functionality to mimic the subset of java's Class class that we require. Classes that require this
// functionality derive from BaseObject, and each contain a unique nested class definition Class. There are #defines here to
// simplify declaring classes with this added functionality.


// 4J Stu - This Enum can be used as a more lightweight version of the above, without having do dynamic casts
// 4J-PB - for replacement of instanceof
enum eINSTANCEOF
{
	eTYPE_NOTSET=0,

	// 4J-RR arranging these pathfinder types in a bitfield fashion so that a single and can determine whether they are derived from
	// the 3 subclasses of pathfinders (water animals, animals, and monsters) that the mob spawner uses
	eTYPE_WATERANIMAL = 0x100,
	eTYPE_SQUID		  = 0x101,

	eTYPE_ANIMAL	  = 0x200,

	// 4J Stu - These have the ANIMAL, AGABLE_MOB and ANIMALS_SPAWN_LIMIT_CHECK bits set
	eTYPE_COW         = 0x82201,
	eTYPE_SHEEP       = 0x82202,
	eTYPE_PIG         = 0x82203,
	eTYPE_SNOWMAN	  = 0x82204,
	eTYPE_OZELOT	  = 0x82205,

	// 4J Stu - When adding new categories, please also update ConsoleSchematicFile::generateSchematicFile so these can be saved out to schematics
	// 4J Stu- These have the ANIMAL and AGABLE_MOB bits set, but NOT ANIMALS_SPAWN_LIMIT_CHECK 
	eTYPE_CHICKEN     = 0x2206,
	eTYPE_WOLF        = 0x2207,
	eTYPE_MUSHROOMCOW = 0x2208,

	// 4J Stu - If you add new hostile mobs here you should also update the string lookup function at CConsoleMinecraftApp::getEntityName
	eTYPE_MONSTER     = 0x400,
	eTYPE_ENEMY       = 0x800,
	eTYPE_CREEPER     = 0xC01,
	eTYPE_GIANT       = 0xC02,
	eTYPE_SKELETON    = 0xC03,
	eTYPE_SPIDER      = 0xC04,
	eTYPE_ZOMBIE      = 0xC05,
	eTYPE_PIGZOMBIE   = 0xC06,
	eTYPE_ENDERMAN	  = 0xC07,
	eTYPE_SILVERFISH  = 0xC08,
	eTYPE_CAVESPIDER  = 0xC09,
	eTYPE_BLAZE		  = 0xC0A,

	eTYPE_GHAST		  = 0xC0B,		// Now considering as a monster even though class inheritance doesn't work like this - but otherwise breaks mob spawning
	eTYPE_SLIME		  = 0xC0C,		// Now considering as a monster even though class inheritance doesn't work like this - but otherwise breaks mob spawning
	eTYPE_LAVASLIME   = 0xC0D,

	eTYPE_VILLAGERGOLEM = 0x1000,

	eTYPE_AGABLE_MOB = 0x2000,

	eTYPE_PLAYER	  = 0x8000,
	eTYPE_SERVERPLAYER= 0x8001,

	// Include AGABLE_MOB
	eTYPE_VILLAGER    = 0x12000,

	eTYPE_PROJECTILE		= 0x40000,
	eTYPE_ARROW				= 0x40001,
	eTYPE_FIREBALL			= 0x40002,
	eTYPE_FISHINGHOOK		= 0x40003,
	eTYPE_SNOWBALL			= 0x40004,
	eTYPE_THROWNEGG			= 0x40005,
	eTYPE_EYEOFENDERSIGNAL	= 0x40006,
	eTYPE_SMALL_FIREBALL	= 0x40007,
	eTYPE_THROWNENDERPEARL	= 0x40008,
	eTYPE_THROWNPOTION		= 0x40009,
	eTYPE_THROWNEXPBOTTLE	= 0x4000A,

	eTYPE_ANIMALS_SPAWN_LIMIT_CHECK = 0x80000,

	// Never used, exists to ensure all later entities don't match the bitmasks above
	eTYPE_OTHERS      = 0x100000,

	eTYPE_NETHER_SPHERE,
	eTYPE_ENDER_CRYSTAL,
	eTYPE_ENDERDRAGON,
	eTYPE_BOSS_MOB_PART,

	eTYPE_ENTITY,

	eTYPE_MOB,

	eTYPE_LIGHTNINGBOLT,

	eTYPE_PAINTING,
	eTYPE_ITEMENTITY,
	eTYPE_FALLINGTILE,
	eTYPE_BOAT,
	eTYPE_MINECART,
	eTYPE_PRIMEDTNT,

	eTYPE_TILEENTITY,
	eTYPE_CHESTTILEENTITY,
	eTYPE_DISPENSERTILEENTITY,
	eTYPE_MOBSPAWNERTILEENTITY,
	eTYPE_FURNACETILEENTITY,
	eTYPE_SIGNTILEENTITY,
	eTYPE_MUSICTILEENTITY,
	eTYPE_RECORDPLAYERTILE,
	eTYPE_PISTONPIECEENTITY,
	eTYPE_BREWINGSTANDTILEENTITY,
	eTYPE_ENCHANTMENTTABLEENTITY,
	eTYPE_THEENDPORTALTILEENTITY,
	eTYPE_SKULLTILEENTITY,
	eTYPE_ENDERCHESTTILEENTITY,

	eType_NODE,

	eType_ITEM,
	eType_ITEMINSTANCE,
	eType_MAPITEM,
	eType_TILE,
	eType_FIRETILE,

	eType_BREAKINGITEMPARTICLE,
	eType_BUBBLEPARTICLE,
	eType_EXPLODEPARTICLE,
	eType_FLAMEPARTICLE,
	eType_FOOTSTEPPARTICLE,
	eType_HEARTPARTICLE,
	eType_LAVAPARTICLE,
	eType_NOTEPARTICLE,
	eType_NETHERPORTALPARTICLE,
	eType_REDDUSTPARTICLE,
	eType_SMOKEPARTICLE,
	eType_SNOWSHOVELPARTICLE,
	eType_SPLASHPARTICLE,
	eType_TAKEANIMATIONPARTICLE,
	eType_TERRAINPARTICLE,
	eType_WATERDROPPARTICLE,

	// 1.8.2
	eTYPE_DELAYEDRELEASE,
	eTYPE_EXPERIENCEORB,
	eType_CRITPARTICLE,
	eType_CRITPARTICLE2,
	eType_HUGEEXPLOSIONPARTICLE,
	eType_HUGEEXPLOSIONSEEDPARTICLE,
	eType_PLAYERCLOUDPARTICLEPARTICLE,
	eType_SUSPENDEDPARTICLE,
	eType_SUSPENDEDTOWNPARTICLE,

	//1.0.1
	eTYPE_DRIPPARTICLE,
	eTYPE_ENCHANTMENTTABLEPARTICLE,
	eTYPE_SPELLPARTICLE,

	//TU9
	eTYPE_HANGING_ENTITY,
	eTYPE_ITEM_FRAME,
	eTYPE_DRAGONBREATHPARTICLE,
	eTYPE_DRAGON_FIREBALL,
	
	eType_ENDERPARTICLE,
};
