/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/BedBlock.h"
#include "world/level/Level.h"
#include "world/entity/player/Player.h"
#include "world/Facing.h"
#include "world/Direction.h"
#include "world/level/BlockSource.h"
#include "world/level/biome/Biome.h"
#include "world/item/Item.h"
#include "util/ColorFormat.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/material/Material.h"

const int BedBlock::HEAD_DIRECTION_OFFSETS[4][2] = {
	{  0,  1 },
	{ -1,  0 },
	{  0, -1 },
	{  1,  0 }
};

const DataID BedBlock::HEAD_PIECE_DATA(0x8);
const DataID BedBlock::OCCUPIED_DATA(0x4);

BedBlock::BedBlock(const std::string& nameId, int id ) :
	Block(nameId, id, Material::getMaterial(MaterialType::Bed)) {
	setVisualShape(Vec3::ZERO, Vec3(1, 9.0f / 16.0f, 1));
	setSolid(false);
	setPushesOutItems(true);

	mProperties = BlockProperty::BreakOnPush;
	mRenderLayer = RENDERLAYER_DOUBLE_SIDED;
}

bool BedBlock::use(Player& player, const BlockPos& pos) const {
	if (player.getLevel().isClientSide()) {
		return true;
	}

	BlockPos offset(pos);

	auto& region = player.getRegion();
	auto data = region.getData(pos);

	if(!isHeadPiece(data)) {
		int direction = Block::mBed->getBlockState(BlockState::Direction).get<int>(data);
		offset.x += HEAD_DIRECTION_OFFSETS[direction][0];
		offset.z += HEAD_DIRECTION_OFFSETS[direction][1];		
		if (region.getBlockID(offset) != mID) {
			return true;
		}
		data = region.getData(offset);
	}

	if (!region.getDimension().mayRespawn() || region.getBiome(offset) == Biome::hell) {
		float xc = offset.x + 0.5f;
		float yc = offset.y + 0.5f;
		float zc = offset.z + 0.5f;
		region.setBlock(offset.x, offset.y, offset.z, BlockID::AIR, Block::UPDATE_ALL);
		int direction = Block::mBed->getBlockState(BlockState::Direction).get<int>(data);
		offset.x += HEAD_DIRECTION_OFFSETS[direction][0];
		offset.z += HEAD_DIRECTION_OFFSETS[direction][1];
		if(region.getBlockID(offset) == mID) {
			region.setBlock(offset.x, offset.y, offset.z, BlockID::AIR, Block::UPDATE_ALL);
			xc = (xc + offset.x + 0.5f) / 2;
			yc = (yc + offset.y + 0.5f) / 2;
			zc = (zc + offset.z + 0.5f) / 2;
		}

// 		player.getLevel().explode(region, nullptr, Vec3(xc, yc, zc), 5, true);
		return true;
	}

	// TODO: rherlitz
// 	if(isOccupied(data)) {
// 		Player* sleepingPlayer = nullptr;
// 
// 		player.getLevel().forEachPlayer([&sleepingPlayer, &offset](Player& player) {
// 			if(player.isSleeping()) {
// 				BlockPos bedPos = player.mBedPosition;
// 				if(bedPos.x == offset.x && bedPos.y == offset.y && bedPos.z == offset.z) {
// 					sleepingPlayer = &player;
// 					return false;
// 				}
// 			}
// 
// 			return true;
// 		});
// 
// 		if(sleepingPlayer == nullptr) {
// 			BedBlock::setOccupied(region, offset, false);
// 		} else {
// 			sleepingPlayer->displayLocalizableMessage(ColorFormat::GRAY + "%tile.bed.occupied");
// 			return true;
// 		}
// 	}
// 
// 	BedSleepingResult result = player.startSleepInBed(offset);
// 	if(result == BedSleepingResult::OK) {
// 		BedBlock::setOccupied(region, offset, true);
// 		return true;
// 	}
// 
// 	if(result == BedSleepingResult::NOT_POSSIBLE_NOW) {
// 		player.displayLocalizableMessage(ColorFormat::GRAY + "%tile.bed.noSleep");
// 	} else if(result == BedSleepingResult::NOT_SAFE) {
// 		player.displayLocalizableMessage(ColorFormat::GRAY + "%tile.bed.notSafe");
// 	}

	return true;
}

bool BedBlock::isInteractiveBlock() const {
	return true;
}

void BedBlock::setOccupied(BlockSource& region, const BlockPos& pos, bool occupied) {
	auto t = region.getBlockAndData(pos);
	if(occupied) {
		Block::mBed->getBlockState(BlockState::OccupiedBit).set(t.data, true);
	} else {
		Block::mBed->getBlockState(BlockState::OccupiedBit).set(t.data, false);
	}

	region.setBlockAndData(pos.x, pos.y, pos.z, t, Block::UPDATE_CLIENTS);
}

int BedBlock::getVariant(int data) const {
	return isHeadPiece(data) ? 1 : 0;
}

FacingID BedBlock::getMappedFace(FacingID face, int data) const {
	if (face == Facing::DOWN) {
		return Facing::DOWN;
	}

	int direction = Block::mBed->getBlockState(BlockState::Direction).get<int>(data);
	int blockFacing = Direction::RELATIVE_DIRECTION_FACING[direction][face];
	if (isHeadPiece(data)) {
		if (blockFacing == Facing::NORTH) {
			return Facing::NORTH;
		}
		if (blockFacing == Facing::EAST || blockFacing == Facing::WEST) {
			return Facing::WEST;
		}
		return Facing::UP;
	}
	else {
		if (blockFacing == Facing::SOUTH) {
			return Facing::SOUTH;
		}
		if (blockFacing == Facing::EAST || blockFacing == Facing::WEST) {
			return Facing::WEST;
		}
		return Facing::UP;
	}
}

bool BedBlock::playerWillDestroy(Player& player, const BlockPos& pos, int data) const {
	BlockPos secondPos;
	getSecondPart(player.getRegion(), pos, secondPos);
	player.getRegion().setBlockAndData(secondPos, FullBlock::AIR, Block::UPDATE_ALL);	// will be replaced by air

	return Block::playerWillDestroy(player, pos, data);
}
void BedBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	auto data = region.getData(pos);
	BlockPos secondPos;
	getSecondPart(region, pos, secondPos);

	if(isHeadPiece(data)) {
		int direction = Block::mBed->getBlockState(BlockState::Direction).get<int>(data);
		if(region.getBlockID(pos.x - HEAD_DIRECTION_OFFSETS[direction][0], pos.y, pos.z - HEAD_DIRECTION_OFFSETS[direction][1]) != mID) {
			region.setBlock(pos, BlockID::AIR, Block::UPDATE_ALL);
// 			region.getLevel().broadcastDimensionEvent(region, LevelEvent::ParticlesDestroyBlock, pos, mID + (data << Block::TILE_NUM_SHIFT));
		}
		else if (region.getBlockID(secondPos) != mID) {
			region.setBlock(pos, BlockID::AIR, Block::UPDATE_ALL);
// 			region.getLevel().broadcastDimensionEvent(region, LevelEvent::ParticlesDestroyBlock, pos, mID + (data << Block::TILE_NUM_SHIFT));
		}
	}
}

int BedBlock::getResource( Random& random, int data, int bonusLootLevel) const {
	return Item::mBed->getId();
}

bool BedBlock::findStandUpPosition(BlockSource& region, const BlockPos& pos, int skipCount, BlockPos& position){
	auto data = region.getData(pos);
	int direction = Block::mBed->getBlockState(BlockState::Direction).get<int>(data);

	for(auto step : range_incl(1)) {
		int startX = pos.x - BedBlock::HEAD_DIRECTION_OFFSETS[direction][0] * step - 1;
		int startZ = pos.z - BedBlock::HEAD_DIRECTION_OFFSETS[direction][1] * step - 1;
		int endX = startX + 2;
		int endZ = startZ + 2;

		for(auto standX : range_incl(startX, endX)) {
			for (int standZ = startZ; standZ <= endZ; ++standZ) {
				if (region.isSolidBlockingBlock(standX, pos.y - 1, standZ) && region.isEmptyBlock(standX, pos.y, standZ) && region.isEmptyBlock(standX, pos.y + 1, standZ)) {
					if (skipCount > 0) {
						skipCount--;
						continue;
					}

					position = BlockPos(standX, pos.y, standZ);
					return true;
				}
			}
		}
	}

	return false;
}

ItemInstance BedBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(Item::mBed);
}

bool BedBlock::isHeadPiece(int data) {
	return Block::mBed->getBlockState(BlockState::HeadPieceBit).getBool(data);
}

bool BedBlock::isOccupied( int data ) {
	return Block::mBed->getBlockState(BlockState::OccupiedBit).getBool(data);
}

bool BedBlock::getSecondPart(BlockSource& region, const BlockPos& pos, BlockPos& out) const {
	auto data = region.getData(pos);
	auto direction = getBlockState(BlockState::Direction).get<int>(data);

	if (isHeadPiece(data)) {
		out = BlockPos(pos.x - HEAD_DIRECTION_OFFSETS[direction][0], pos.y, pos.z - HEAD_DIRECTION_OFFSETS[direction][1]);
	} else {
		out = BlockPos(pos.x + HEAD_DIRECTION_OFFSETS[direction][0], pos.y, pos.z + HEAD_DIRECTION_OFFSETS[direction][1]);
	}

	return true;
}

bool BedBlock::canBeSilkTouched() const {
	return false;
}
