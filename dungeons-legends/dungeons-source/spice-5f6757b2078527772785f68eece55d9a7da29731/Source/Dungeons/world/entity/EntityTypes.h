/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

enum class EntityType : int {
	Undefined = 1,

	// Mobs up to 63, rest on top of that
	ItemEntity = 64,
	PrimedTnt = 65,
	FallingBlock = 66,
	MovingBlock = 67,
	ExperiencePotion = 68,
	Experience = 69,
	EyeOfEnder = 70,
	EnderCrystal = 71,
	ShulkerBullet = 76,
	FishingHook = 77,
	Chalkboard = 78, // Legacy, used for converting old Chalkboards to BlockEntities

	DragonFireball = 79,
	Arrow = 80,
	Snowball = 81,
	ThrownEgg = 82,
	Painting = 83,
	LargeFireball = 85,	//used to be "Fireball", which has pure virtual members, and thus could not exist in a savegame
	ThrownPotion = 86,
	Enderpearl = 87,
	LeashKnot = 88,
	WitherSkull = 89,

	// (NEW ID's) D5: I hope this isn't in use in the future!!!
	BoatRideable = 90,

	WitherSkullDangerous = 91,

	LightningBolt = 93,
	SmallFireball = 94,
	AreaEffectCloud = 95,

	//	@Note: Minecarts occupy 84 & 96 - 99

	LingeringPotion = 101,

	// Mob types (Category, maybe different Enum to avoid clashes)
	// NOTE: Because of legacy serialization, mob type values may not overlap (base type bits are ignored)
	TypeMask = 0x000000ff,	// legacy system only supports up to 255 different entity ids
	Mob = 0x00000100,	// safely casted to (Mob*)
	PathfinderMob = 0x00000200 | Mob,
	Monster = 0x00000800 | PathfinderMob,	// safely casted to (Monster*), NOTE: Different from Java
															// project (example Slime.java vs Slime.h)
	Animal = 0x00001000 | PathfinderMob,		// safely casted to (Animal*)
	WaterAnimal = 0x00002000 | PathfinderMob,
	TamableAnimal = 0x00004000 | Animal,
	Ambient = 0x00008000 | Mob,				// safely casted to (AmbientCreature*)
	UndeadMob = 0x00010000 | Monster,
	ZombieMonster = 0x00020000 | UndeadMob,
	Arthropod = 0x00040000 | Monster,

	Minecart = 0x00080000,

	SkeletonMonster = 0x00100000 | UndeadMob,
	EquineAnimal = 0x00200000 | TamableAnimal,	// horse type

	Chicken = 10 | Animal,
	Cow = 11 | Animal,
	Pig = 12 | Animal,
	Sheep = 13 | Animal,
	Wolf = 14 | TamableAnimal,
	Villager = 15 | PathfinderMob,
	MushroomCow = 16 | Animal,
	Squid = 17 | WaterAnimal,
	Rabbit = 18 | Animal,
	Bat = 19 | Ambient,
	IronGolem = 20 | PathfinderMob,
	SnowGolem = 21 | PathfinderMob,
	Ocelot = 22 | TamableAnimal,
	Horse = 23 | EquineAnimal,
	PolarBear = 28 | Animal,

	//These are temporary and only used for mob spawner items
	Donkey = 24 | EquineAnimal,
	Mule = 25 | EquineAnimal,
	SkeletonHorse = 26 | EquineAnimal | UndeadMob,
	ZombieHorse = 27 | EquineAnimal | UndeadMob,

	// note that type Monster == (Monster | PathfinderMob | Mob)
	Zombie = 32 | ZombieMonster,
	Creeper = 33 | Monster,
	Skeleton = 34 | SkeletonMonster,
	Spider = 35 | Arthropod,
	PigZombie = 36 | UndeadMob,
	Slime = 37 | Monster,
	EnderMan = 38 | Monster,
	Silverfish = 39 | Arthropod,
	CaveSpider = 40 | Arthropod,
	Ghast = 41 | Monster,
	LavaSlime = 42 | Monster,
	Blaze = 43 | Monster,
	ZombieVillager = 44 | ZombieMonster,
	Witch = 45 | Monster,

	//These are temporary and only used for mob spawner items
	Stray = 46 | SkeletonMonster,
	Husk = 47 | ZombieMonster,
	WitherSkeleton = 48 | SkeletonMonster,
	Guardian = 49 | Monster,
	ElderGuardian = 50 | Monster,
	
	Npc = 51 | Mob,
	WitherBoss = 52 | UndeadMob,
	Dragon = 53 | Monster,
	Shulker = 54 | Monster,
	Endermite = 55 | Arthropod,
	Agent = 56 | Mob,

	TripodCamera = 62 | Mob,
	Player = 63 | Mob,

	

	MinecartRideable = 84 | Minecart,
	MinecartHopper = 96 | Minecart,
	MinecartTNT = 97 | Minecart,
	MinecartChest = 98 | Minecart,
	MinecartFurnace = 99 | Minecart,
};

enum class EntityTypeNamespaceRules : int {
	ReturnWithoutNamespace,
	ReturnWithNamespace,
};

struct EntityMapping {
	std::string mNamespace;
	std::string mPrimaryName;
	std::string mAlternateName;

	EntityMapping(const std::string& primary, const std::string& alt = "")
		: EntityMapping("minecraft", primary, alt)
	{}

	EntityMapping(const std::string& space, const std::string& primary, const std::string& alt)
		: mNamespace(space)
		, mPrimaryName(primary)
		, mAlternateName(alt)
	{}

	bool operator==(const EntityMapping& other) {
		return mNamespace == other.mNamespace
			&& mPrimaryName == other.mPrimaryName
			&& mAlternateName == other.mAlternateName;
	}

	size_t operator()(EntityMapping const& key) const {
		size_t seed = 0;
		seed = Math::hash_accumulate(seed, key.mNamespace);
		seed = Math::hash_accumulate(seed, key.mPrimaryName);
		seed = Math::hash_accumulate(seed, key.mAlternateName);
		return seed;
	}

	inline std::string getMappingName(EntityTypeNamespaceRules namespaceRule) const {
		if (namespaceRule == EntityTypeNamespaceRules::ReturnWithNamespace) {
			return mNamespace + ":" + mPrimaryName;
		} else {
			return mPrimaryName;
		}
	}
};

enum_bitwise_operators(EntityType, int);

// String conversion
std::string EntityTypeToString(EntityType entityType, EntityTypeNamespaceRules namespaceRule = EntityTypeNamespaceRules::ReturnWithoutNamespace);
std::string EntityTypeIdWithoutCategories(EntityType entityType, EntityTypeNamespaceRules namespaceRule = EntityTypeNamespaceRules::ReturnWithoutNamespace);

EntityType EntityTypeFromString(const std::string& fromString);

std::string EntityTypeResolveAlias(const std::string& fromString, EntityTypeNamespaceRules namespaceRule = EntityTypeNamespaceRules::ReturnWithNamespace);
