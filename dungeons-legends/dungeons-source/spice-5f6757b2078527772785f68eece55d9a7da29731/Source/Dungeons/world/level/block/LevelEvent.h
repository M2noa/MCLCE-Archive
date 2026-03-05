/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

enum class LevelEvent : short {
	SoundClick = 1000,
	SoundClickFail = 1001,
	SoundLaunch = 1002,
	SoundOpenDoor = 1003,
	SoundFizz = 1004,
	SoundFuse = 1005,
	SoundPlayRecording = 1006,	//NYI

	SoundGhastWarning = 1007,	//NYI
	SoundGhastFireball = 1008,	//NYI
	SoundBlazeFireball = 1009,	//NYI

	SoundZombieWoodenDoor = 1010,
	SoundZombieDoorCrash = 1012,

	SoundZombieInfected = 1016,
	SoundZombieConverted = 1017,
	SoundEndermanTeleport = 1018,

	SoundAnvilBroken = 1020,
	SoundAnvilUsed = 1021,
	SoundAnvilLand = 1022,

	SoundInfinityArrowPickup = 1030,
	SoundTeleportEnderPearl = 1032,

	SoundItemFrameAddItem = 1040,
	SoundItemFrameBreak = 1041,
	SoundItemFramePlace = 1042,
	SoundItemFrameRemoveItem = 1043,
	SoundItemFrameRotateItem = 1044,

	SoundTakePicture = 1050,

	SoundExperienceOrbPickup = 1051,
	

	ParticlesShoot = 2000,
	ParticlesDestroyBlock = 2001,
	ParticlesPotionSplash = 2002,
	ParticlesEyeOfEnderDeath = 2003,
	ParticlesMobBlockSpawn = 2004,
	ParticleCropGrowth = 2005,
	ParticleSoundGuardianGhost = 2006,
	ParticleDeathSmoke = 2007,
	ParticleDenyBlock = 2008,
	ParticleGenericSpawn = 2009,
	ParticlesDragonEgg = 2010,

	StartRaining = 3001,
	StartThunderstorm = 3002,
	StopRaining = 3003,
	StopThunderstorm = 3004,
	GlobalPause = 3005,

	ActivateBlock = 3500,
	CauldronExplode = 3501,
	CauldronDyeArmor = 3502,
	CauldronCleanArmor = 3503,
	CauldronFillPotion = 3504,
	CauldronTakePotion = 3505,
	CauldronFillWater = 3506,
	CauldronTakeWater = 3507,
	CauldronAddDye = 3508,

	UpdateExtraData = 4000,

	AllPlayersSleeping = 9800,

	GlobalEvent = 0x4000
};


enum class LevelSoundEvent : unsigned char {
	ItemUseOn,
	Hit,
	Step,
	Jump,
	Break,
	Place,
	HeavyStep,
	Gallop,

	Fall,

	Ambient,
	AmbientBaby,
	AmbientInWater,
	Breathe,
	Death,
	DeathInWater,
	DeathToZombie,
	Hurt,
	HurtInWater,
	Mad,
	Boost,

	Bow,
	SquishBig,
	SquishSmall,
	FallBig,
	FallSmall,
	Splash,
	Fizz,
	Flap,
	Swim,
	Drink,
	Eat,
	Takeoff,
	Shake,
	Plop,
	Land,
	Saddle,
	Armor,
	AddChest,
	Throw,
	Attack,
	AttackNoDamage,
	Warn, 
	Shear,
	Milk,

	Thunder,
	Explode,
	Fire,
	Ignite,
	Fuse,
	Stare,
	Spawn,
	Shoot,
	BreakBlock,

	Remedy,
	Unfect,
	LevelUp,
	BowHit,
	BulletHit,
	ExtinguishFire,
	ItemFizz,
	ChestOpen,
	ChestClosed,
	PowerOn, 
	PowerOff,
	Attach,
	Detach,
	Deny,
	Tripod,
	Pop,
	DropSlot,
	
	Note,
	Thorns,
	PistonIn,
	PistonOut,
	Portal,
	Water,
	LavaPop,
	Lava,
	Burp,
	BucketFillWater,
	BucketFillLava,
	BucketEmptyWater,
	BucketEmptyLava,
	GuardianFlop,
	GuardianCurse,
	MobWarning,
	MobWarningBaby,

	Teleport,
	ShulkerOpen,
	ShulkerClose,

	Default,
	Undefined
};

LevelSoundEvent soundEventFromString(const std::string& type);