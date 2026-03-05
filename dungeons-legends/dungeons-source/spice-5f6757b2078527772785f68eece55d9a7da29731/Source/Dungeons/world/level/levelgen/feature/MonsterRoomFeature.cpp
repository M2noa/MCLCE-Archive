#include "Dungeons.h"

#include "world/level/levelgen/feature/MonsterRoomFeature.h"
#include "world/level/BlockSource.h"
#include "world/level/material/Material.h"
#include "world/level/block/Block.h"
//#include "world/level/block/entity/ChestBlockEntity.h"
//#include "world/level/block/entity/MobSpawnerBlockEntity.h"
#include "world/entity/EntityTypes.h"
#include "world/item/Item.h"
#include "world/Facing.h"

#include "util/LootTableUtil.h"
#include "util/Random.h"

MonsterRoomFeature::MonsterRoomFeature()
	: Feature(nullptr) {

}

bool MonsterRoomFeature::place(BlockSource& region, const BlockPos& pos, Random& random) const {
	int hr = 3;
	int xr = random.nextInt(2) + 2;
	int zr = random.nextInt(2) + 2;

	int holeCount = 0;

	for (int xx = pos.x - xr - 1; xx <= pos.x + xr + 1; xx++) {
		for (int yy = pos.y - 1; yy <= pos.y + hr + 1; yy++) {
			for (int zz = pos.z - zr - 1; zz <= pos.z + zr + 1; zz++) {
				BlockPos blockPos(xx, yy, zz);
				const Material& m = region.getMaterial(blockPos);
				if (yy == pos.y - 1 && !m.isSolid()) {
					return false;
				}
				if (yy == pos.y + hr + 1 && !m.isSolid()) {
					return false;
				}

				if (xx == pos.x - xr - 1 || xx == pos.x + xr + 1 || zz == pos.z - zr - 1 || zz == pos.z + zr + 1) {
					if (yy == pos.y && region.isEmptyBlock(blockPos) && region.isEmptyBlock(blockPos.above())) {
						holeCount++;
					}
				}
			}
		}
	}

	if (holeCount < 1 || holeCount > 5) {
		return false;
	}

	for (auto xx : range_incl(pos.x - xr - 1, pos.x + xr + 1)) {
		for (int yy = pos.y + hr; yy >= pos.y - 1; yy--) {
			for (int zz = pos.z - zr - 1; zz <= pos.z + zr + 1; zz++) {
				BlockPos blockPos(xx, yy, zz);
				if (xx == pos.x - xr - 1 || yy == pos.y - 1 || zz == pos.z - zr - 1 || xx == pos.x + xr + 1 || yy == pos.y + hr + 1 || zz == pos.z + zr + 1) {
					if (yy >= 0 && !region.getMaterial(blockPos.below()).isSolid()) {
						region.removeBlock(blockPos.x, blockPos.y, blockPos.z);
					}
					else if (region.getMaterial(blockPos).isSolid()) {
						if (yy == pos.y - 1 && random.nextInt(4) != 0) {
							_setBlockAndData(region, blockPos, FullBlock(Block::mMossyCobblestone->mID));
						}
						else {
							_setBlockAndData(region, blockPos, FullBlock(Block::mCobblestone->mID));
						}
					}
				}
				else {
					region.removeBlock(blockPos.x, blockPos.y, blockPos.z);
				}
			}
		}
	}

	for (int cc = 0; cc < 2; cc++) {
		for (int i = 0; i < 3; i++) {
			int randZ = random.nextInt(zr * 2 + 1) - zr;
			int randX = random.nextInt(xr * 2 + 1) - xr;
			BlockPos blockPos = pos + BlockPos(randX, 0, randZ);
			if (!region.isEmptyBlock(blockPos)) {
				continue;
			}

			int count = 0;
			if (region.getMaterial(blockPos.west()).isSolid()) {
				count++;
			}
			if (region.getMaterial(blockPos.east()).isSolid()) {
				count++;
			}
			if (region.getMaterial(blockPos.north()).isSolid()) {
				count++;
			}
			if (region.getMaterial(blockPos.south()).isSolid()) {
				count++;
			}

			if (count != 1) {
				continue;
			}

			// Look for a wall next to us, and if we find one, face the chest away from it please.
			DataID facingId = _getFacingDataId(region, blockPos, randX, randZ);

			_setBlockAndData(region, blockPos, FullBlock(Block::mChest->mID, facingId));
			
			//ChestBlockEntity* chest = (ChestBlockEntity*) region.getBlockEntity(blockPos);
			//if (chest != nullptr) {
			//	Util::LootTableUtils::fillChest(region.getLevel(), *chest, random, "loot_tables/chests/simple_dungeon.json");
			//}

			break;
		}
	}

	// TODO: rherlitz

	_setBlockAndData(region, pos, FullBlock(Block::mMobSpawner->mID));
	//MobSpawnerBlockEntity* entity = (MobSpawnerBlockEntity*) region.getBlockEntity(pos);
	//if (entity != nullptr) {
	//	entity->getSpawner().setEntityId(_getRandomEntityId(random));
	//}

	return true;
}

EntityType MonsterRoomFeature::_getRandomEntityId(Random& random) const {
	int id = random.nextInt(4);
	switch (id) {
	case 0:
		return EntityType::Skeleton;
	case 1:
	case 2:
		return EntityType::Zombie;
	case 3:
		return EntityType::Spider;
	default:
		return EntityType::Pig;
	}
}

DataID MonsterRoomFeature::_getFacingDataId(BlockSource& region, const BlockPos & blockPos, int offsetX, int offsetZ ) const {

	// Look for a wall next to us, and if we find one, face the chest away from it please.
	if (offsetZ > 0 && !region.isEmptyBlock(blockPos + BlockPos(0, 0, 1))) {
		return Facing::NORTH;
	}
	else if (offsetZ < 0 && !region.isEmptyBlock(blockPos + BlockPos(0, 0, -1))) {
		return Facing::SOUTH;
	}
	
	if (offsetX > 0 && !region.isEmptyBlock(blockPos + BlockPos(1, 0, 0))) {
		return Facing::WEST;
	}
	else if (offsetX < 0 && !region.isEmptyBlock(blockPos + BlockPos(-1, 0, 0))) {
		return Facing::EAST;
	}

	// Can't find a wall? if so lets just pick one based on offset from center.
	if (offsetZ > 0) {
		return Facing::NORTH;
	}
	else if (offsetZ < 0) {
		return Facing::SOUTH;
	}

	if (offsetX > 0) {
		return Facing::WEST;
	}
	else if (offsetX < 0) {
		return Facing::EAST;
	}

	return Facing::NORTH;
}