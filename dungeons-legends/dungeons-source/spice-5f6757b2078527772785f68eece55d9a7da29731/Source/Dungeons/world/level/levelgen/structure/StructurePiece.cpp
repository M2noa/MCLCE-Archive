#include "Dungeons.h"

#include "world/level/levelgen/structure/StructurePiece.h"

#include "world/Facing.h"
#include "world/level/ChunkPos.h"
#include "world/level/Level.h"
#include "world/level/BlockSource.h"
#include "world/level/material/Material.h"
#include "world/level/block/Block.h"
#include "world/level/block/BaseRailBlock.h"
#include "world/level/block/TorchBlock.h"
// #include "world/level/block/entity/ChestBlockEntity.h"
// #include "world/level/block/entity/DispenserBlockEntity.h"
// #include "world/item/DoorItem.h"
#include "nbt/CompoundTag.h"
#include "nbt/IntArrayTag.h"
#include "world/level/dimension/Dimension.h"

#include "util/LootTableUtil.h"

bool StructurePiece::isInChunk(const ChunkPos& pos) const {
	int cx = (pos.x << 4);
	int cz = (pos.z << 4);

	return boundingBox.intersects(cx, cz, cx + 15, cz + 15);
}

StructurePiece* StructurePiece::findCollisionPiece(const PieceList& pieces, const BoundingBox& box){
	for (unsigned int i = 0; i < pieces.size(); i++) {
		if (pieces[i]->getBoundingBox().intersects(box)) {
			return pieces[i].get();
		}
	}
	return nullptr;
}

BlockPos StructurePiece::getLocatorPosition() const {
	return BlockPos(boundingBox.getXCenter(), boundingBox.getYCenter(), boundingBox.getZCenter());
}

void StructurePiece::postProcessMobsAt(BlockSource* level, Random& random, const BoundingBox& chunkBB){
}

void StructurePiece::addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random){

}

bool StructurePiece::edgesLiquid(BlockSource* level, const BoundingBox& chunkBB) {

	int x0 = std::max(boundingBox.x0 - 1, chunkBB.x0);
	int y0 = std::max(boundingBox.y0 - 1, chunkBB.y0);
	int z0 = std::max(boundingBox.z0 - 1, chunkBB.z0);
	int x1 = Math::min(boundingBox.x1 + 1, chunkBB.x1);
	int y1 = Math::min(boundingBox.y1 + 1, chunkBB.y1);
	int z1 = Math::min(boundingBox.z1 + 1, chunkBB.z1);

	// roof and floor
	for (int x = x0; x <= x1; x++) {
		for (int z = z0; z <= z1; z++) {
			if (level->getMaterial(x, y0, z).isLiquid()) {
				return true;
			}
			if (level->getMaterial(x, y1, z).isLiquid()) {
				return true;
			}
		}
	}

	// north and south
	for (int x = x0; x <= x1; x++) {
		for (int y = y0; y <= y1; y++) {
			auto& m1 = level->getMaterial(x, y, z0);
			auto& m2 = level->getMaterial(x, y, z1);
			if (m1.isLiquid()) {
				return true;
			}
			if (m2.isLiquid()) {
				return true;
			}
		}
	}

	// east and west
	for (int z = z0; z <= z1; z++) {
		for (int y = y0; y <= y1; y++) {
			if (level->getMaterial(x0, y, z).isLiquid()) {
				return true;
			}
			if (level->getMaterial(x1, y, z).isLiquid()) {
				return true;
			}
		}
	}
	return false;
}

int StructurePiece::getWorldX(int x, int z) {
	switch (orientation) {
	case Direction::NORTH:
	case Direction::SOUTH:
		return boundingBox.x0 + x;
	case Direction::WEST:
		return boundingBox.x1 - z;
	case Direction::EAST:
		return boundingBox.x0 + z;
	default:
		return x;
	}
}

int StructurePiece::getWorldY(int y) {
	if (orientation == Direction::UNDEFINED) {
		return y;
	}
	return y + boundingBox.y0;
}

int StructurePiece::getWorldZ(int x, int z) {
	switch (orientation) {
	case Direction::NORTH:
		return boundingBox.z1 - z;
	case Direction::SOUTH:
		return boundingBox.z0 + z;
	case Direction::WEST:
	case Direction::EAST:
		return boundingBox.z0 + x;
	default:
		return z;
	}
}

DataID StructurePiece::getOrientationData(const Block* block, DataID data) {
	if (block == Block::mRail) {
		if (orientation == Direction::WEST || orientation == Direction::EAST) {
			if (data == BaseRailBlock::DIR_FLAT_X) {
				return BaseRailBlock::DIR_FLAT_Z;
			}
			else {
				return BaseRailBlock::DIR_FLAT_X;
			}
		}
	}
	else if (block->hasProperty(BlockProperty::Door)) {
		if (orientation == Direction::SOUTH) {
			if (data == 0) {
				return 2;
			}
			if (data == 2) {
				return 0;
			}
		}
		else if (orientation == Direction::WEST) {
			// 0 = 1
			// 1 = 2
			// 2 = 3
			// 3 = 0
			return (data + 1) & 3;
		}
		else if (orientation == Direction::EAST) {
			// 0 = 3
			// 1 = 0
			// 2 = 1
			// 3 = 2
			return (data + 3) & 3;
		}
	}
	else if (block->isStairBlock()) {
		if (orientation == Direction::SOUTH) {
			if (data == 2) {
				return 3;
			}
			if (data == 3) {
				return 2;
			}
		}
		else if (orientation == Direction::WEST) {
			if (data == 0) {
				return 2;
			}
			if (data == 1) {
				return 3;
			}
			if (data == 2) {
				return 0;
			}
			if (data == 3) {
				return 1;
			}
		}
		else if (orientation == Direction::EAST) {
			if (data == 0) {
				return 2;
			}
			if (data == 1) {
				return 3;
			}
			if (data == 2) {
				return 1;
			}
			if (data == 3) {
				return 0;
			}
		}
	}
	else if (block == Block::mLadder) {
		if (orientation == Direction::SOUTH) {
			if (data == Facing::NORTH) {
				return Facing::SOUTH;
			}
			if (data == Facing::SOUTH) {
				return Facing::NORTH;
			}
		}
		else if (orientation == Direction::WEST) {
			if (data == Facing::NORTH) {
				return Facing::WEST;
			}
			if (data == Facing::SOUTH) {
				return Facing::EAST;
			}
			if (data == Facing::WEST) {
				return Facing::NORTH;
			}
			if (data == Facing::EAST) {
				return Facing::SOUTH;
			}
		}
		else if (orientation == Direction::EAST) {
			if (data == Facing::NORTH) {
				return Facing::EAST;
			}
			if (data == Facing::SOUTH) {
				return Facing::WEST;
			}
			if (data == Facing::WEST) {
				return Facing::NORTH;
			}
			if (data == Facing::EAST) {
				return Facing::SOUTH;
			}
		}

	}
	else if (block == Block::mWoodButton ||
			 block == Block::mStoneButton) {
		if (orientation == Direction::SOUTH) {
			if (data == 3) {
				return 4;
			}
			if (data == 4) {
				return 3;
			}
		}
		else if (orientation == Direction::WEST) {
			if (data == 3) {
				return 1;
			}
			if (data == 4) {
				return 2;
			}
			if (data == 2) {
				return 3;
			}
			if (data == 1) {
				return 4;
			}
		}
		else if (orientation == Direction::EAST) {
			if (data == 3) {
				return 2;
			}
			if (data == 4) {
				return 1;
			}
			if (data == 2) {
				return 3;
			}
			if (data == 1) {
				return 4;
			}
		}
	}
	else if (block == Block::mChest) {
		// the structure generator is using Direction, but chests use Facing
		if (orientation == Direction::NORTH) {
			return Direction::DIRECTION_FACING[data];
		}
		else if (orientation == Direction::SOUTH) {
			if (data == Direction::SOUTH || data == Direction::NORTH) {
				return Direction::DIRECTION_FACING[Direction::DIRECTION_OPPOSITE[data]];
			}
			return Direction::DIRECTION_FACING[data];
		}
		else if (orientation == Direction::WEST) {
			if (data == Direction::NORTH) {
				return Facing::WEST;
			}
			if (data == Direction::SOUTH) {
				return Facing::EAST;
			}
			if (data == Direction::WEST) {
				return Facing::NORTH;
			}
			if (data == Direction::EAST) {
				return Facing::SOUTH;
			}
		}
		else if (orientation == Direction::EAST) {
			if (data == Direction::NORTH) {
				return Facing::EAST;
			}
			if (data == Direction::SOUTH) {
				return Facing::WEST;
			}
			if (data == Direction::WEST) {
				return Facing::NORTH;
			}
			if (data == Direction::EAST) {
				return Facing::SOUTH;
			}
		}
	}
	/*
	   // directional blocks
	   else if ( || block instanceof DirectionalBlock ) {
		if (orientation == Direction::SOUTH) {
			if (data == Direction::SOUTH || data == Direction::NORTH) {
				return Direction::DIRECTION_OPPOSITE[data];
			}
		}
		else if (orientation == Direction::WEST) {
			if (data == Direction::NORTH) {
				return Direction::WEST;
			}
			if (data == Direction::SOUTH) {
				return Direction::EAST;
			}
			if (data == Direction::WEST) {
				return Direction::NORTH;
			}
			if (data == Direction::EAST) {
				return Direction::SOUTH;
			}
		}
		else if (orientation == Direction::EAST) {
			if (data == Direction::NORTH) {
				return Direction::EAST;
			}
			if (data == Direction::SOUTH) {
				return Direction::WEST;
			}
			if (data == Direction::WEST) {
				return Direction::NORTH;
			}
			if (data == Direction::EAST) {
				return Direction::SOUTH;
			}
		}
	   }
	 */
	/*
	   else if (block == Blocks.PISTON || block == Blocks.STICKY_PISTON || block == Blocks.LEVER || block ==
		  Blocks.DISPENSER) {
		if (orientation == Direction::SOUTH) {
			if (data == Facing::NORTH || data == Facing::SOUTH) {
				return Facing::OPPOSITE_FACING[data];
			}
		}
		else if (orientation == Direction::WEST) {
			if (data == Facing::NORTH) {
				return Facing::WEST;
			}
			if (data == Facing::SOUTH) {
				return Facing::EAST;
			}
			if (data == Facing::WEST) {
				return Facing::NORTH;
			}
			if (data == Facing::EAST) {
				return Facing::SOUTH;
			}
		}
		else if (orientation == Direction::EAST) {
			if (data == Facing::NORTH) {
				return Facing::EAST;
			}
			if (data == Facing::SOUTH) {
				return Facing::WEST;
			}
			if (data == Facing::WEST) {
				return Facing::NORTH;
			}
			if (data == Facing::EAST) {
				return Facing::SOUTH;
			}
		}
	   }
	 */
	else if (block == Block::mTorch) {
		return getTorchData(data);
	}
	return data;
}

DataID StructurePiece::getTorchData(DataID torchOrientation){
	if (torchOrientation == Direction::UNDEFINED) {
		// normal standing torch
		return TorchBlock::TORCH_TOP;
	}
	else if (orientation == Direction::NORTH) {
		switch (torchOrientation) {
		case Direction::NORTH:
			return TorchBlock::TORCH_NORTH;
		case Direction::SOUTH:
			return TorchBlock::TORCH_SOUTH;
		case Direction::WEST:
			return TorchBlock::TORCH_WEST;
		case Direction::EAST:
			return TorchBlock::TORCH_EAST;
		}
	}
	else if (orientation == Direction::SOUTH) {
		// flip Z
		switch (torchOrientation) {
		case Direction::NORTH:
			return TorchBlock::TORCH_SOUTH;
		case Direction::SOUTH:
			return TorchBlock::TORCH_NORTH;
		case Direction::WEST:
			return TorchBlock::TORCH_WEST;
		case Direction::EAST:
			return TorchBlock::TORCH_EAST;
		}
	}
	else if (orientation == Direction::WEST) {
		switch (torchOrientation) {
		case Direction::NORTH:
			return TorchBlock::TORCH_WEST;
		case Direction::SOUTH:
			return TorchBlock::TORCH_EAST;
		case Direction::WEST:
			return TorchBlock::TORCH_NORTH;
		case Direction::EAST:
			return TorchBlock::TORCH_SOUTH;
		}
	}
	else if (orientation == Direction::EAST) {
		switch (torchOrientation) {
		case Direction::NORTH:
			return TorchBlock::TORCH_EAST;
		case Direction::SOUTH:
			return TorchBlock::TORCH_WEST;
		case Direction::WEST:
			return TorchBlock::TORCH_NORTH;
		case Direction::EAST:
			return TorchBlock::TORCH_SOUTH;
		}
	}
	return TorchBlock::TORCH_TOP;
}

void StructurePiece::placeBlock(BlockSource* level, FullBlock t, int x, int y, int z, const BoundingBox& chunkBB){
	BlockPos world = _getWorldPos(x, y, z);

	if (!chunkBB.isInside(world)) {
		return;
	}

	if(level->setBlockAndData(world, t, Block::UPDATE_NONE) && (t == Block::mFlowingWater->mID || t == Block::mFlowingLava->mID)) {
		Random random(world.hashCode());
		Block::mBlocks[t.id]->tick(*level, world, random);
	}
}

bool StructurePiece::isAir(BlockSource* level, int x, int y, int z, const BoundingBox& chunkBB) {
	BlockPos world = _getWorldPos(x, y, z);
	if (!chunkBB.isInside(world.x, world.y, world.z)) {
		return false;
	}

	return level->getBlockID(world) == 0;
}

/**
 * The purpose of this method is to wrap the getBlock call on Level, in order
 * to prevent the level from generating chunks that shouldn't be loaded yet.
 * Returns 0 if the call is out of bounds.
 *
 * @param level
 * @param x
 * @param y
 * @param z
 * @param chunkBB
 * @return
 */
BlockID StructurePiece::getBlock(BlockSource* level, int x, int y, int z, const BoundingBox& chunkBB) {
	BlockPos world = _getWorldPos(x, y, z);
	if (!chunkBB.isInside(world.x, world.y, world.z)) {
		return BlockID::AIR;
	}

	return level->getBlockID(world);
}

void StructurePiece::generateAirBox(BlockSource* level, const BoundingBox& chunkBB, int x0, int y0, int z0, int x1, int y1, int z1) {
	for (int y = y0; y <= y1; y++) {
		for (int x = x0; x <= x1; x++) {
			for (int z = z0; z <= z1; z++) {
				placeBlock(level, FullBlock::AIR, x, y, z, chunkBB);
			}
		}
	}
}

void StructurePiece::generateBox(BlockSource* level, const BoundingBox& chunkBB, int x0, int y0, int z0, int x1, int y1, int z1, FullBlock edgeBlock, FullBlock fillBlock, bool skipAir) {
	for (int y = y0; y <= y1; y++) {
		for (int x = x0; x <= x1; x++) {
			for (int z = z0; z <= z1; z++) {

				if (skipAir && getBlock(level, x, y, z, chunkBB) == BlockID::AIR) {
					continue;
				}

				bool edge = (y == y0 || y == y1 || x == x0 || x == x1 || z == z0 || z == z1);
				placeBlock(level, edge ? edgeBlock : fillBlock, x, y, z, chunkBB);
			}
		}
	}
}

void StructurePiece::generateBox(BlockSource* level, const BoundingBox& chunkBB, int x0, int y0, int z0, int x1, int y1, int z1, BlockID edgeBlock, DataID edgeData, BlockID fillBlock, DataID fillData, bool skipAir) {
	generateBox(level, chunkBB, x0, y0, z0, x1, y1, z1, FullBlock(edgeBlock, edgeData), FullBlock(fillBlock, fillData), skipAir);
}

void StructurePiece::generateBox(BlockSource* level, const BoundingBox& chunkBB, const BoundingBox& boxBB, BlockID edgeBlock, BlockID fillBlock, bool skipAir) {
	generateBox(level, chunkBB, boxBB.x0, boxBB.y0, boxBB.z0, boxBB.x1, boxBB.y1, boxBB.z1, edgeBlock, fillBlock, skipAir);
}

void StructurePiece::generateBox(BlockSource* level, const BoundingBox& chunkBB, int x0, int y0, int z0, int x1, int y1, int z1, bool skipAir, Random& random, BlockSelector* selector) {
	for (int y = y0; y <= y1; y++) {
		for (int x = x0; x <= x1; x++) {
			for (int z = z0; z <= z1; z++) {

				if (skipAir && getBlock(level, x, y, z, chunkBB) == 0) {
					continue;
				}
				selector->next(random, x, y, z, y == y0 || y == y1 || x == x0 || x == x1 || z == z0 || z == z1);
				placeBlock(level, selector->getNext(), x, y, z, chunkBB);

			}
		}
	}
}

void StructurePiece::generateBox(BlockSource* level, const BoundingBox& chunkBB, const BoundingBox& boxBB, bool skipAir, Random& random, BlockSelector* selector) {
	generateBox(level, chunkBB, boxBB.x0, boxBB.y0, boxBB.z0, boxBB.x1, boxBB.y1, boxBB.z1, skipAir, random, selector);
}

Brightness StructurePiece::getBrightness(int x0, int y1, int z, BlockSource* level) {
// 	BlockPos pos = _getWorldPos(x0, y1 + 1, z);
// 	return level->getBrightness(level->getDimension().getSkyLightLayer(), pos.above());
	return Brightness();
}

void StructurePiece::generateMaybeBox(BlockSource* level, const BoundingBox& chunkBB, Random& random, float probability, int x0, int y0, int z0, int x1, int y1, int z1, BlockID edgeBlock, BlockID fillBlock, bool skipAir, Brightness maxBrightness) {
	for (int y = y0; y <= y1; y++) {
		for (int x = x0; x <= x1; x++) {
			for (int z = z0; z <= z1; z++) {

				if (random.nextFloat() > probability) {
					continue;
				}
				if (skipAir && getBlock(level, x, y, z, chunkBB) == 0) {
					continue;
				}

				if (maxBrightness && getBrightness(x, y, z, level) >= maxBrightness) {
					continue;
				}

				if (y == y0 || y == y1 || x == x0 || x == x1 || z == z0 || z == z1) {
					placeBlock(level, edgeBlock, x, y, z, chunkBB);
				}
				else {
					placeBlock(level, fillBlock, x, y, z, chunkBB);
				}

			}
		}
	}
}

void StructurePiece::maybeGenerateBlock(BlockSource* level, const BoundingBox& chunkBB, Random& random, float probability, int x, int y, int z, BlockID block, DataID data) {
	if (random.nextFloat() < probability) {
		placeBlock(level, FullBlock(block, data), x, y, z, chunkBB);
	}
}

void StructurePiece::generateUpperHalfSphere(BlockSource* level, const BoundingBox& chunkBB, int x0, int y0, int z0, int x1, int y1, int z1, BlockID fillBlock, bool skipAir) {
	float diagX = (float)(x1 - x0 + 1);
	float diagY = (float) (y1 - y0 + 1);
	float diagZ = (float) (z1 - z0 + 1);
	float cx = x0 + diagX / 2;
	float cz = z0 + diagZ / 2;

	for (int y = y0; y <= y1; y++) {
		float normalizedYDistance = (float) (y - y0) / diagY;

		for (int x = x0; x <= x1; x++) {
			float normalizedXDistance = (x - cx) / (diagX * .5f);

			for (int z = z0; z <= z1; z++) {
				float normalizedZDistance = (z - cz) / (diagZ * .5f);

				if (skipAir && getBlock(level, x, y, z, chunkBB) == 0) {
					continue;
				}

				float dist = (normalizedXDistance * normalizedXDistance) + (normalizedYDistance * normalizedYDistance) + (normalizedZDistance * normalizedZDistance);

				if (dist <= 1.05f) {
					placeBlock(level, fillBlock, x, y, z, chunkBB);
				}

			}
		}
	}
}

void StructurePiece::generateAirColumnUp(BlockSource* level, int x, int startY, int z, const BoundingBox& chunkBB) {

	int worldX = getWorldX(x, z);
	int worldY = getWorldY(startY);
	int worldZ = getWorldZ(x, z);

	if (!chunkBB.isInside(worldX, worldY, worldZ)) {
		return;
	}

	while (!level->isEmptyBlock(worldX, worldY, worldZ) && worldY < level->getMaxHeight() - 1) {
		level->setBlockAndData(worldX, worldY, worldZ, FullBlock(), Block::UPDATE_NONE);
		worldY++;
	}
}

void StructurePiece::fillColumnDown(BlockSource* level, FullBlock block, int x, int startY, int z, const BoundingBox& chunkBB) {

	BlockPos world = _getWorldPos(x, startY, z);

	if (!chunkBB.isInside(world.x, world.y, world.z)) {
		return;
	}

	while ((level->isEmptyBlock(world) || level->getMaterial(world).isLiquid()) && world.y > 1) {
		level->setBlockAndData(world, block, Block::UPDATE_NONE);
		world.y--;
	}
	// if topmost block is grass, replace it with dirt in order to prevent lots of chunk updates when grass ticks
	if (level->getBlockID(world) == Block::mGrass->mID) {
		level->setBlockAndData(world, Block::mDirt->mID, Block::UPDATE_NONE);
	}
}

int StructurePiece::getTotalWeight(const PieceWeightList& pieceWeights){
	bool hasAnyPieces = false;
	int totalWeight = 0;

	for (unsigned int i = 0; i < pieceWeights.size(); i++) {
		const PieceWeight& piece = pieceWeights[i];
		if (piece.maxPlaceCount > 0 && piece.placeCount < piece.maxPlaceCount) {
			hasAnyPieces = true;
		}
		totalWeight += piece.weight;
	}
	return hasAnyPieces ? totalWeight : -1;
}

bool StructurePiece::createChest(BlockSource* level, const BoundingBox& chunkBB, Random& random, int x, int y, int z, int direction, const std::string& lootTable) {

	int worldX = getWorldX(x, z);
	int worldY = getWorldY(y);
	int worldZ = getWorldZ(x, z);
	DataID data = getOrientationData(Block::mChest, direction);

// 	if (chunkBB.isInside(worldX, worldY, worldZ)) {
// 		if (level->getBlockID(worldX, worldY, worldZ) != Block::mChest->mID) {
// 			level->setBlockAndData(worldX, worldY, worldZ, Block::mChest->mID, data, Block::UPDATE_NONE);
// 			ChestBlockEntity* chest = (ChestBlockEntity*) level->getBlockEntity(worldX, worldY, worldZ);
// 			if (chest != nullptr) {
// 				Util::LootTableUtils::fillChest(level->getLevel(), *chest, random, lootTable);
// 			}
// 			//LOGE("\nCreated chest at (%d, %d, %d)\n", worldX, worldY, worldZ);
// 			return true;
// 		}
// 	}
	return false;
}

bool StructurePiece::createDispenser(BlockSource* level, const BoundingBox& chunkBB, Random& random, int x, int y, int z, int facing, const std::string& lootTable) {
	int worldX = getWorldX(x, z);
	int worldY = getWorldY(y);
	int worldZ = getWorldZ(x, z);

	if (chunkBB.isInside(worldX, worldY, worldZ)) {
		if (level->getBlockID(worldX, worldY, worldZ) != Block::mDispenser->mID) {
			placeBlock(level, FullBlock(Block::mDispenser->mID, facing), x, y, z, chunkBB);
// 			DispenserBlockEntity *dispenser = (DispenserBlockEntity*) level->getBlockEntity(worldX, worldY, worldZ);
// 			if(dispenser != nullptr) {
// 				Util::LootTableUtils::fillChest(level->getLevel(), *dispenser, random, lootTable);
// 			}
			return true;
		}
	}
	return false;
}

void StructurePiece::createDoor(BlockSource* level, const BoundingBox& chunkBB, Random& random, int x, int y, int z, int orientation) {
	BlockPos world = _getWorldPos(x, y, z);

// 	if (chunkBB.isInside(world)) {
// 		DoorItem::place(level, world.x, world.y, world.z, orientation, Block::mWoodenDoor);
// 	}

}

BlockPos StructurePiece::_getWorldPos(int x, int y, int z) {
	return BlockPos(getWorldX(x, z), getWorldY(y), getWorldZ(x, z));
}

Unique<CompoundTag> StructurePiece::createTag() const {
	auto tag = make_unique<CompoundTag>();

	tag->putInt("ID", static_cast<int>(getType()));
	tag->put("BB", boundingBox.createTag());
	tag->putInt("orientation", orientation);
	tag->putInt("gendepth", genDepth);

	addAdditionalSaveData(*tag);

	return tag;
}

void StructurePiece::loadFromTag(const CompoundTag& tag) {
	if (tag.contains("BB")) {
		boundingBox = BoundingBox((int*)tag.getIntArray("BB").data());
	}

	orientation = tag.getInt("orientation");
	genDepth = tag.getInt("genDepth");

	readAdditionalSaveData(tag);
}
