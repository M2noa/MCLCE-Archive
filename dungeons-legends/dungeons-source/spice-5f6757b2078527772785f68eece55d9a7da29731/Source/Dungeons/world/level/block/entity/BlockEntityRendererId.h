/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/

#pragma once

enum BlockEntityRendererId {
	TR_DEFAULT_RENDERER,
	TR_CHEST_RENDERER,
	TR_SIGN_RENDERER,
	TR_MOBSPAWNER_RENDERER,
	TR_SKULL_RENDERER,
	TR_ENCHANTER_RENDERER,
	TR_PISTONARM_RENDERER,
	TR_ITEMFRAME_RENDERER,
	TR_MOVINGBLOCK_RENDERER,
	TR_CHALKBOARD_RENDERER,
	TR_BEACON_RENDERER,
	TR_ENDGATEWAY_RENDERER,
	TR_ENDERCHEST_RENDERER,
	TR_STRUCTUREBLOCK_RENDERER,
};

enum class BlockEntityType {
	None = 0,
	Furnace = 1,
	Chest = 2,
	NetherReactor = 3,
	Sign = 4,
	MobSpawner = 5,
	Skull = 6,
	FlowerPot = 7,
	BrewingStand = 8,
	EnchantingTable = 9,
	DaylightDetector = 10,
	Music = 11,
	Comparator = 12,
	Dispenser = 13,
	Dropper = 14,
	Hopper = 15,
	Cauldron = 16,
	ItemFrame = 17,
	PistonArm = 18,
	MovingBlock = 19,
	Chalkboard = 20,
	Beacon = 21,
	EndPortal = 22,
	EnderChest = 23,
	EndGateway = 24,
	StructureBlock = 32,
};
