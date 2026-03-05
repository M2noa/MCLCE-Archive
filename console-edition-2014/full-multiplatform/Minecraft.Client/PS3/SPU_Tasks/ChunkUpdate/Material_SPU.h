#pragma once

#include <stdint.h>

class ChunkRebuildData;
class Material_SPU
{
	friend class ChunkRebuildData;
public:
	static const int air_Id = 0;
	static const int grass_Id = 1;
	static const int dirt_Id = 2;
	static const int wood_Id = 3;
	static const int stone_Id = 4;
	static const int metal_Id = 5;
	static const int water_Id = 6;
	static const int lava_Id = 7;
	static const int leaves_Id = 8;
	static const int plant_Id = 9;
	static const int replaceable_plant_Id = 10;
	static const int sponge_Id = 11;
	static const int cloth_Id = 12;
	static const int fire_Id = 13;
	static const int sand_Id = 14;
	static const int decoration_Id = 15;
	static const int glass_Id = 16;
	static const int explosive_Id = 17;
	static const int coral_Id = 18;
	static const int ice_Id = 19;
	static const int topSnow_Id = 20;
	static const int snow_Id = 21;
	static const int cactus_Id = 22;
	static const int clay_Id = 23;
	static const int vegetable_Id = 24;
	static const int egg_Id = 25;
	static const int portal_Id = 26;
	static const int cake_Id = 27;
	static const int web_Id = 28;
	static const int piston_Id = 29;
	static const int buildable_glass_Id = 30;
	static const int heavyMetal_Id = 31;
	static const int clothDecoration_Id = 32;
	static const int num_Ids = 32;

	enum Flags
	{
		e_flammable = 0x01,
		e_replaceable = 0x02,
		e_neverBuildable = 0x04,
		e_isSolid = 0x08,
		e_isLiquid = 0x10, 
		e_blocksLight = 0x20,
		e_blocksMotion = 0x40
	};

private:
	uint16_t id;
	uint16_t flags;

public:
	int color;

public:
	int getID() { return id;}
	bool isLiquid() { return (flags & e_isLiquid) != 0; }
	bool letsWaterThrough() { return (!isLiquid() && !isSolid()); }
    bool isSolid() { return (flags & e_isSolid) != 0;}
    bool blocksLight() { return (flags & e_blocksLight) != 0; }
    bool blocksMotion() { return (flags & e_blocksMotion) != 0; }
	bool isFlammable() { return (flags & e_flammable) != 0; }
	bool isReplaceable(){ return (flags & e_replaceable) != 0; }
	bool isSolidBlocking(){   if (flags & e_neverBuildable) return false;	return blocksMotion(); }
};

