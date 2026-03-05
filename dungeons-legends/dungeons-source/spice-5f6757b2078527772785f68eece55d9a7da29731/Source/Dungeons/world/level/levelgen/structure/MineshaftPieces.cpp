#include "Dungeons.h"

#include "world/level/levelgen/structure/MineshaftPieces.h"

#include "nbt/CompoundTag.h"
#include "nbt/IntArrayTag.h"
#include "nbt/ListTag.h"
#include "world/entity/Mob.h"
#include "world/entity/EntityTypes.h"
#include "world/level/BlockSource.h"
#include "world/level/block/Block.h"
#include "world/item/Item.h"
// #include "world/item/DyePowderItem.h"
#include "world/level/Level.h"
#include "world/level/block/BaseRailBlock.h"
// #include "world/level/block/entity/MobSpawnerBlockEntity.h"
#include "world/level/block/WoodBlock.h"
#include "world/level/levelgen/structure/MineshaftFeature.h"
// #include "world/entity/item/MinecartChest.h"
// #include "world/entity/components/ContainerComponent.h"

// #include "world/entity/EntityFactory.h"
#include "util/LootTableUtil.h"

Unique<StructurePiece> MineshaftPiece::createRandomShaftPiece(MineshaftData& metadata, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth){
	int randomSelection = random.nextInt(100);
	if (randomSelection >= 80) {
		BoundingBox crossingBox = MineshaftCrossing::findCrossing(pieces, random, footX, footY, footZ, direction);
		if (crossingBox.isValid()) {
			return make_unique<MineshaftCrossing>(metadata, genDepth, random, crossingBox, direction);
		}
	}
	else if (randomSelection >= 70) {
		BoundingBox stairsBox = MineshaftStairs::findStairs(pieces, random, footX, footY, footZ, direction);
		if (stairsBox.isValid()) {
			return make_unique<MineshaftStairs>(metadata, genDepth, random, stairsBox, direction);
		}
	}
	else {
		BoundingBox corridorBox = MineshaftCorridor::findCorridorSize(pieces, random, footX, footY, footZ, direction);
		if (corridorBox.isValid()) {
			return make_unique<MineshaftCorridor>(metadata, genDepth, random, corridorBox, direction);
		}
	}

	return nullptr;
}

StructurePiece* MineshaftPiece::generateAndAddPiece(StructurePiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int depth){
	if (depth > MAX_DEPTH) {
		return nullptr;
	}
	if (std::abs(footX - startPiece->getBoundingBox().x0) > 5 * 16 || std::abs(footZ - startPiece->getBoundingBox().z0) > 5 * 16) {
		return nullptr;
	}

	auto newPiece = createRandomShaftPiece(metadata, pieces, random, footX, footY, footZ, direction, depth + 1);
	if (newPiece != nullptr) {
		auto ptr = newPiece.get();
		pieces.emplace_back(std::move(newPiece));
		ptr->addChildren(startPiece, pieces, random);
		return ptr;
	}
	return nullptr;
}

bool MineshaftPiece::createChest(BlockSource* level, const BoundingBox& chunkBB, Random& random, int x, int y, int z, int direction, const std::string& lootTable) {
	BlockPos pos(getWorldX(x, z), getWorldY(y), getWorldZ(x, z));
	if (chunkBB.isInside(pos)) {
		if (level->getBlockID(pos) == Block::mAir->mID && level->getBlockID(pos.below()) != Block::mAir->mID) {
			int data = random.nextBoolean() ? BaseRailBlock::DIR_FLAT_X : BaseRailBlock::DIR_FLAT_Z;
			level->setBlockAndData(pos, Block::mRail->mID, getOrientationData(Block::mRail, data), Block::UPDATE_CLIENTS);

// 			Unique<Entity> result = EntityFactory::createSpawnedEntity(EntityType::MinecartChest, nullptr, pos, Vec2::ZERO);
// 			if (result) {
// 				MinecartChest *cart = static_cast<MinecartChest*>(result.get());
// 				level->getLevel().addEntity(*level, std::move(result));
// 				if (cart != nullptr && cart->getContainerComponent()) {
// 					auto chest = cart->getContainerComponent()->mContainer.get();
// 					DEBUG_ASSERT(cart != nullptr, "Something went wrong! A Chest Minecart somehow doesn't have a container component!");
// 					if (chest != nullptr) {
// 						//	Should there actually we a chest component on this minecart (Should always be true), than we can go
// 						//	ahead and add it's loot and add it to the level.
// 						Util::LootTableUtils::fillChest(level->getLevel(), *chest, random, lootTable);
// 					}
// 				}
// 				return true;
// 			}
		}
	}
	return false;
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

MineshaftRoom::MineshaftRoom(MineshaftData& metadata)
	: MineshaftPiece(0, metadata){

}

MineshaftRoom::MineshaftRoom(MineshaftData& metadata, int genDepth, Random& random, int west, int north)
	: MineshaftPiece(genDepth, metadata){
	int randZ1 = north + 7 + random.nextInt(6);
	int randY1 = 54 + random.nextInt(6);
	int randX1 = west + 7 + random.nextInt(6);
	boundingBox = BoundingBox(west, 50, north, randX1, randY1, randZ1);
}

void MineshaftRoom::addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random){
	if (random.nextFloat() > metadata.roomChance) {
		return;
	}

	int depth = getGenDepth();

	int pos;

	int heightSpace = boundingBox.getYSpan() - DEFAULT_SHAFT_HEIGHT - 1;
	if (heightSpace <= 0) {
		heightSpace = 1;
	}

	// northern exits
	pos = 0;

	while (pos < boundingBox.getXSpan()) {
		pos += random.nextInt(boundingBox.getXSpan());
		if ((pos + DEFAULT_SHAFT_WIDTH) > boundingBox.getXSpan()) {
			break;
		}
		StructurePiece* child = generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 + pos, boundingBox.y0 + random.nextInt(heightSpace) + 1, boundingBox.z0 - 1, Direction::NORTH,
			depth);
		if (child != nullptr) {
			BoundingBox childBox = child->getBoundingBox();
			childEntranceBoxes.push_back(BoundingBox(childBox.x0, childBox.y0, boundingBox.z0, childBox.x1, childBox.y1, boundingBox.z0 + 1));
		}
		pos += DEFAULT_SHAFT_WIDTH + 1;
	}
	// southern exits
	pos = 0;

	while (pos < boundingBox.getXSpan()) {
		pos += random.nextInt(boundingBox.getXSpan());
		if ((pos + DEFAULT_SHAFT_WIDTH) > boundingBox.getXSpan()) {
			break;
		}
		StructurePiece* child = generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 + pos, boundingBox.y0 + random.nextInt(heightSpace) + 1, boundingBox.z1 + 1, Direction::SOUTH,
			depth);
		if (child != nullptr) {
			BoundingBox childBox = child->getBoundingBox();
			childEntranceBoxes.push_back(BoundingBox(childBox.x0, childBox.y0, boundingBox.z1 - 1, childBox.x1, childBox.y1, boundingBox.z1));
		}
		pos += DEFAULT_SHAFT_WIDTH + 1;
	}
	// western exits
	pos = 0;

	while (pos < boundingBox.getZSpan()) {
		pos += random.nextInt(boundingBox.getZSpan());
		if ((pos + DEFAULT_SHAFT_WIDTH) > boundingBox.getZSpan()) {
			break;
		}
		StructurePiece* child = generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 - 1, boundingBox.y0 + random.nextInt(heightSpace) + 1, boundingBox.z0 + pos, Direction::WEST,
			depth);
		if (child != nullptr) {
			BoundingBox childBox = child->getBoundingBox();
			childEntranceBoxes.push_back(BoundingBox(boundingBox.x0, childBox.y0, childBox.z0, boundingBox.x0 + 1, childBox.y1, childBox.z1));
		}
		pos += DEFAULT_SHAFT_WIDTH + 1;
	}
	// eastern exits
	pos = 0;

	while (pos < boundingBox.getZSpan()) {
		pos += random.nextInt(boundingBox.getZSpan());
		if ((pos + DEFAULT_SHAFT_WIDTH) > boundingBox.getZSpan()) {
			break;
		}
		StructurePiece* child = generateAndAddPiece(startPiece, pieces, random, boundingBox.x1 + 1, boundingBox.y0 + random.nextInt(heightSpace) + 1, boundingBox.z0 + pos, Direction::EAST,
			depth);
		if (child != nullptr) {
			BoundingBox childBox = child->getBoundingBox();
			childEntranceBoxes.push_back(BoundingBox(boundingBox.x1 - 1, childBox.y0, childBox.z0, boundingBox.x1, childBox.y1, childBox.z1));
		}
		pos += DEFAULT_SHAFT_WIDTH + 1;
	}
}

bool MineshaftRoom::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB){
	if (CHECK_WATER_EDGE && edgesLiquid(level, chunkBB)) {
		return false;
	}

	// floor
	generateBox(level, chunkBB, boundingBox.x0, boundingBox.y0, boundingBox.z0, boundingBox.x1, boundingBox.y0, boundingBox.z1, Block::mDirt->mID, BlockID::AIR, true);

	// room air
	generateBox(level, chunkBB, boundingBox.x0, boundingBox.y0 + 1, boundingBox.z0, boundingBox.x1, Math::min(boundingBox.y0 + 3, boundingBox.y1), boundingBox.z1, BlockID::AIR, BlockID::AIR,
		false);

	for (unsigned int i = 0; i < childEntranceBoxes.size(); i++) {
		const BoundingBox& entranceBox = childEntranceBoxes[i];
		generateBox(level, chunkBB, entranceBox.x0, entranceBox.y1 - (DEFAULT_SHAFT_HEIGHT - 1), entranceBox.z0, entranceBox.x1, entranceBox.y1, entranceBox.z1, BlockID::AIR, BlockID::AIR,
			false);
	}
	generateUpperHalfSphere(level, chunkBB, boundingBox.x0, boundingBox.y0 + 4, boundingBox.z0, boundingBox.x1, boundingBox.y1, boundingBox.z1, BlockID::AIR, false);

	return true;
}

void MineshaftRoom::addAdditionalSaveData(CompoundTag& tag) const {
	auto entrances = make_unique<ListTag>();

	for (unsigned int i = 0; i < childEntranceBoxes.size(); i++) {
		const BoundingBox& bb = childEntranceBoxes[i];
		entrances->add(bb.createTag());
	}
	tag.put("Entrances", std::move(entrances));
}

void MineshaftRoom::readAdditionalSaveData(const CompoundTag& tag) {
	if (auto entrances = tag.getList("Entrances")) {
		for (int i = 0; i < entrances->size(); i++) {
			// @todo: clean up casting
			childEntranceBoxes.push_back(BoundingBox((int*)((IntArrayTag*)entrances->get(i))->data.data()));
		}
	}
}

void MineshaftRoom::moveBoundingBox(int dx, int dy, int dz) {
	StructurePiece::moveBoundingBox(dx, dy, dz);
	for (auto& box : childEntranceBoxes) {
		box.move(dx, dy, dz);
	}
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

BoundingBox MineshaftCorridor::findCorridorSize(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction){
	BoundingBox box = BoundingBox(footX, footY, footZ, footX, footY + (DEFAULT_SHAFT_HEIGHT - 1), footZ);

	int corridorLength = random.nextInt(3) + 2;

	while (corridorLength > 0) {
		int blockLength = corridorLength * DEFAULT_SHAFT_LENGTH;

		switch (direction) {
		case Direction::NORTH:
			box.x1 = footX + (DEFAULT_SHAFT_WIDTH - 1);
			box.z0 = footZ - (blockLength - 1);
			break;
		case Direction::SOUTH:
			box.x1 = footX + (DEFAULT_SHAFT_WIDTH - 1);
			box.z1 = footZ + (blockLength - 1);
			break;
		case Direction::WEST:
			box.x0 = footX - (blockLength - 1);
			box.z1 = footZ + (DEFAULT_SHAFT_WIDTH - 1);
			break;
		case Direction::EAST:
			box.x1 = footX + (blockLength - 1);
			box.z1 = footZ + (DEFAULT_SHAFT_WIDTH - 1);
			break;
		}

		if (StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
			corridorLength--;
		}
		else {
			break;
		}
	}

	if (corridorLength > 0) {
		return box;
	}
	// unable to place corridor here
	return BoundingBox::getUnknownBox();
}

MineshaftCorridor::MineshaftCorridor(MineshaftData& metadata)
	: MineshaftPiece(0, metadata)
	, hasRails(false)
	, spiderCorridor(false)
	, hasPlacedSpider(false)
	, numSections(0){

}

MineshaftCorridor::MineshaftCorridor(MineshaftData& metadata, int genDepth, Random& random, const BoundingBox& corridorBox, int direction)
	: MineshaftPiece(genDepth, metadata)
	, hasRails(false)
	, spiderCorridor(false)
	, hasPlacedSpider(false)
	, numSections(0){
	orientation = direction;
	boundingBox = corridorBox;
	hasRails = random.nextInt(3) == 0;
	spiderCorridor = !hasRails && random.nextInt(23) == 0 && boundingBox.y1 < 50;

	if (orientation == Direction::NORTH || orientation == Direction::SOUTH) {
		numSections = corridorBox.getZSpan() / DEFAULT_SHAFT_LENGTH;
	}
	else {
		numSections = corridorBox.getXSpan() / DEFAULT_SHAFT_LENGTH;
	}
}

void MineshaftCorridor::addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random){
	int depth = getGenDepth();
	int endSelection = random.nextInt(4);

	switch (orientation) {
	case Direction::NORTH:
		if (endSelection <= 1) {
			generateAndAddPiece(startPiece, pieces, random, boundingBox.x0, boundingBox.y0 - 1 + random.nextInt(3), boundingBox.z0 - 1, orientation, depth);
		}
		else if (endSelection == 2) {
			generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 - 1, boundingBox.y0 - 1 + random.nextInt(3), boundingBox.z0, Direction::WEST, depth);
		}
		else {
			generateAndAddPiece(startPiece, pieces, random, boundingBox.x1 + 1, boundingBox.y0 - 1 + random.nextInt(3), boundingBox.z0, Direction::EAST, depth);
		}
		break;
	case Direction::SOUTH:
		if (endSelection <= 1) {
			generateAndAddPiece(startPiece, pieces, random, boundingBox.x0, boundingBox.y0 - 1 + random.nextInt(3), boundingBox.z1 + 1, orientation, depth);
		}
		else if (endSelection == 2) {
			generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 - 1, boundingBox.y0 - 1 + random.nextInt(3), boundingBox.z1 - DEFAULT_SHAFT_WIDTH, Direction::WEST, depth);
		}
		else {
			generateAndAddPiece(startPiece, pieces, random, boundingBox.x1 + 1, boundingBox.y0 - 1 + random.nextInt(3), boundingBox.z1 - DEFAULT_SHAFT_WIDTH, Direction::EAST, depth);
		}
		break;
	case Direction::WEST:
		if (endSelection <= 1) {
			generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 - 1, boundingBox.y0 - 1 + random.nextInt(3), boundingBox.z0, orientation, depth);
		}
		else if (endSelection == 2) {
			generateAndAddPiece(startPiece, pieces, random, boundingBox.x0, boundingBox.y0 - 1 + random.nextInt(3), boundingBox.z0 - 1, Direction::NORTH, depth);
		}
		else {
			generateAndAddPiece(startPiece, pieces, random, boundingBox.x0, boundingBox.y0 - 1 + random.nextInt(3), boundingBox.z1 + 1, Direction::SOUTH, depth);
		}
		break;
	case Direction::EAST:
		if (endSelection <= 1) {
			generateAndAddPiece(startPiece, pieces, random, boundingBox.x1 + 1, boundingBox.y0 - 1 + random.nextInt(3), boundingBox.z0, orientation, depth);
		}
		else if (endSelection == 2) {
			generateAndAddPiece(startPiece, pieces, random, boundingBox.x1 - DEFAULT_SHAFT_WIDTH, boundingBox.y0 - 1 + random.nextInt(3), boundingBox.z0 - 1, Direction::NORTH, depth);
		}
		else {
			generateAndAddPiece(startPiece, pieces, random, boundingBox.x1 - DEFAULT_SHAFT_WIDTH, boundingBox.y0 - 1 + random.nextInt(3), boundingBox.z1 + 1, Direction::SOUTH, depth);
		}
		break;
	}

	// generate cross sections using higher depth
	if (depth < MAX_DEPTH) {
		if (orientation == Direction::NORTH || orientation == Direction::SOUTH) {
			for (int z = boundingBox.z0 + 3; (z + DEFAULT_SHAFT_WIDTH) <= boundingBox.z1; z += DEFAULT_SHAFT_LENGTH) {
				int selection = random.nextInt(5);
				if (selection == 0) {
					generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 - 1, boundingBox.y0, z, Direction::WEST, depth + 1);
				}
				else if (selection == 1) {
					generateAndAddPiece(startPiece, pieces, random, boundingBox.x1 + 1, boundingBox.y0, z, Direction::EAST, depth + 1);
				}
			}
		}
		else {
			for (int x = boundingBox.x0 + 3; (x + DEFAULT_SHAFT_WIDTH) <= boundingBox.x1; x += DEFAULT_SHAFT_LENGTH) {
				int selection = random.nextInt(5);
				if (selection == 0) {
					generateAndAddPiece(startPiece, pieces, random, x, boundingBox.y0, boundingBox.z0 - 1, Direction::NORTH, depth + 1);
				}
				else if (selection == 1) {
					generateAndAddPiece(startPiece, pieces, random, x, boundingBox.y0, boundingBox.z1 + 1, Direction::SOUTH, depth + 1);
				}
			}
		}
	}

}

bool MineshaftPiece::_isSupportingBox(const int x0, const int x1, BlockSource* level, const int y1, int z) {
	BlockPos
		min = _getWorldPos(x0, y1 + 1, z),
		max = _getWorldPos(x1, y1 + 1, z);

	for (BlockPos p = min; p.x <= max.x; ++p.x) {
		for (p.z = min.z; p.z <= max.z; ++p.z) {
			if (level->getBlockID(p) == 0) {
				return false;
			}
		}
	}
	return true;
}

void MineshaftCorridor::_placeCobWeb(BlockSource* level, const BoundingBox& chunkBB, Random& random, float p, const int x0, const int y1, int z) {
	if (getBrightness(x0, y1, z, level) < 8) {
		maybeGenerateBlock(level, chunkBB, random, p, x0, y1, z - 1, Block::mWeb->mID, 0);
	}
}

void MineshaftCorridor::_placeSupport(BlockSource* level, const BoundingBox& chunkBB, const int x0, const int y0, int z, const int y1, const int x1, Random& random) {

	//check if there is nothing to support
	if (!_isSupportingBox(x0, x1, level, y1, z)) {
		return;
	}

	generateBox(level, chunkBB, x0, y0, z, x0, y1 - 1, z, metadata.fenceBlock, FullBlock::AIR, false);
	generateBox(level, chunkBB, x1, y0, z, x1, y1 - 1, z, metadata.fenceBlock, FullBlock::AIR, false);
	if (random.nextInt(4) == 0) {
		generateBox(level, chunkBB, x0, y1, z, x0, y1, z, metadata.woodBlock, FullBlock::AIR, false);
		generateBox(level, chunkBB, x1, y1, z, x1, y1, z, metadata.woodBlock, FullBlock::AIR, false);
	}
	else {
		generateBox(level, chunkBB, x0, y1, z, x1, y1, z, metadata.woodBlock, FullBlock::AIR, false);
		maybeGenerateBlock(level, chunkBB, random, .05f, x0 + 1, y1, z - 1, Block::mTorch->mID, getTorchData(Direction::NORTH));
		maybeGenerateBlock(level, chunkBB, random, .05f, x0 + 1, y1, z + 1, Block::mTorch->mID, getTorchData(Direction::SOUTH));
	}
}

void MineshaftCorridor::postProcessMobsAt(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	const int x0 = 0;
	const int x1 = DEFAULT_SHAFT_WIDTH - 1;
	const int y0 = 0;

	for (int section = 0; section < numSections; section++) {
		int z = 2 + section * DEFAULT_SHAFT_LENGTH;

		if (random.nextInt(100) == 0) {
			createChest(level, chunkBB, random, x1, y0, z - 1, Direction::WEST, "loot_tables/chests/abandoned_mineshaft.json");
		}
		if (random.nextInt(100) == 0) {
			createChest(level, chunkBB, random, x0, y0, z + 1, Direction::WEST, "loot_tables/chests/abandoned_mineshaft.json");
		}
	}
}

bool MineshaftCorridor::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	if (CHECK_WATER_EDGE && edgesLiquid(level, chunkBB)) {
		return false;
	}

	const int x0 = 0;
	const int x1 = DEFAULT_SHAFT_WIDTH - 1;
	const int y0 = 0;
	const int y1 = DEFAULT_SHAFT_HEIGHT - 1;
	const int length = (numSections * DEFAULT_SHAFT_LENGTH) - 1;

	// corridor air
	generateBox(level, chunkBB, x0, 0, y0, x1, y1 - 1, length, BlockID::AIR, BlockID::AIR, false);
	generateMaybeBox(level, chunkBB, random, .8f, x0, y1, y0, x1, y1, length, BlockID::AIR, BlockID::AIR, false);

	if (spiderCorridor) {
		generateMaybeBox(level, chunkBB, random, .6f, x0, 0, y0, x1, y1 - 1, length, Block::mWeb->mID, BlockID::AIR, false, Brightness(8));
	}

	// place a support in every section
	for (int section = 0; section < numSections; section++) {

		int z = 2 + section * DEFAULT_SHAFT_LENGTH;

		_placeSupport(level, chunkBB, x0, y0, z, y1, x1, random);

		_placeCobWeb(level, chunkBB, random, .1f, x0, y1, z);
		_placeCobWeb(level, chunkBB, random, .1f, x1, y1, z - 1);
		_placeCobWeb(level, chunkBB, random, .1f, x0, y1, z + 1);
		_placeCobWeb(level, chunkBB, random, .1f, x1, y1, z + 1);
		_placeCobWeb(level, chunkBB, random, .05f, x0, y1, z - 2);
		_placeCobWeb(level, chunkBB, random, .05f, x1, y1, z - 2);
		_placeCobWeb(level, chunkBB, random, .05f, x0, y1, z + 2);
		_placeCobWeb(level, chunkBB, random, .05f, x1, y1, z + 2);

		if (spiderCorridor && !hasPlacedSpider) {
			int y = getWorldY(y0), newZ = z - 1 + random.nextInt(3);
			int x = getWorldX(x0 + 1, newZ);
			newZ = getWorldZ(x0 + 1, newZ);
			if (chunkBB.isInside(x, y, newZ) && getBrightness(x, y, newZ, level) < 8) {
				hasPlacedSpider = true;
				level->setBlockAndData(x, y, newZ, Block::mMobSpawner->mID, Block::UPDATE_NONE);
// 				MobSpawnerBlockEntity* entity = (MobSpawnerBlockEntity*)level->getBlockEntity(x, y, newZ);
// 
// 				DEBUG_ASSERT(entity, "this is wrong");
// 
// 				entity->getSpawner().setEntityId(EntityType::CaveSpider);
			}
		}
	}

	// prevent air floating
	for (int x = x0; x <= x1; x++) {
		for (int z = 0; z <= length; z++) {
			int y2 = -1;
			if (getBlock(level, x, y2, z, chunkBB) == 0 && getBrightness(x, y2, z, level) < 8) {
				int y = -1;
				placeBlock(level, metadata.woodBlock, x, y, z, chunkBB);
			}
		}
	}

	if (hasRails) {
		for (int z = 0; z <= length; z++) {
			auto floor = getBlock(level, x0 + 1, y0 - 1, z, chunkBB);
			if (floor != 0 && Block::mSolid[floor]) {

				float probability = (getBrightness(x0 + 1, y0 - 1, z, level) > 8) ? 0.9f : 0.7f;

				maybeGenerateBlock(level, chunkBB, random, probability, x0 + 1, y0, z, Block::mRail->mID, getOrientationData(Block::mRail, BaseRailBlock::DIR_FLAT_Z));
			}
		}
	}

	return true;
}

void MineshaftCorridor::addAdditionalSaveData(CompoundTag& tag) const {
	tag.putBoolean("hr", hasRails);
	tag.putBoolean("sc", spiderCorridor);
	tag.putBoolean("hps", hasPlacedSpider);
	tag.putInt("Num", numSections);
}

void MineshaftCorridor::readAdditionalSaveData(const CompoundTag& tag) {
	hasRails = tag.getBoolean("hr");
	spiderCorridor = tag.getBoolean("sc");
	hasPlacedSpider = tag.getBoolean("hps");
	numSections = tag.getInt("Num");
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

BoundingBox MineshaftCrossing::findCrossing(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction){
	BoundingBox box = BoundingBox(footX, footY, footZ, footX, footY + (DEFAULT_SHAFT_HEIGHT - 1), footZ);

	if (random.nextInt(4) == 0) {
		box.y1 += DEFAULT_SHAFT_HEIGHT + 1;	// two-floored
	}

	switch (direction) {
	case Direction::NORTH:
		box.x0 = footX - 1;
		box.x1 = footX + DEFAULT_SHAFT_WIDTH;
		box.z0 = footZ - (DEFAULT_SHAFT_WIDTH + 1);
		break;
	case Direction::SOUTH:
		box.x0 = footX - 1;
		box.x1 = footX + DEFAULT_SHAFT_WIDTH;
		box.z1 = footZ + (DEFAULT_SHAFT_WIDTH + 1);
		break;
	case Direction::WEST:
		box.x0 = footX - (DEFAULT_SHAFT_WIDTH + 1);
		box.z0 = footZ - 1;
		box.z1 = footZ + DEFAULT_SHAFT_WIDTH;
		break;
	case Direction::EAST:
		box.x1 = footX + (DEFAULT_SHAFT_WIDTH + 1);
		box.z0 = footZ - 1;
		box.z1 = footZ + DEFAULT_SHAFT_WIDTH;
		break;
	}

	if (StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return BoundingBox::getUnknownBox();
	}

	return box;
}

MineshaftCrossing::MineshaftCrossing(MineshaftData& metadata)
	: MineshaftPiece(0, metadata){

}

MineshaftCrossing::MineshaftCrossing(MineshaftData& metadata, int genDepth, Random& random, const BoundingBox& crossingBox, int direction)
	: MineshaftPiece(genDepth, metadata){
	this->direction = direction;
	boundingBox = crossingBox;
	isTwoFloored = crossingBox.getYSpan() > DEFAULT_SHAFT_HEIGHT;
}

void MineshaftCrossing::addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random){
	int depth = getGenDepth();

	// crossings are coming from a direction and will generate children
	// in the remaining three directions
	switch (direction) {
	case Direction::NORTH:
		generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 + 1, boundingBox.y0, boundingBox.z0 - 1, Direction::NORTH, depth);
		generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 - 1, boundingBox.y0, boundingBox.z0 + 1, Direction::WEST, depth);
		generateAndAddPiece(startPiece, pieces, random, boundingBox.x1 + 1, boundingBox.y0, boundingBox.z0 + 1, Direction::EAST, depth);
		break;
	case Direction::SOUTH:
		generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 + 1, boundingBox.y0, boundingBox.z1 + 1, Direction::SOUTH, depth);
		generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 - 1, boundingBox.y0, boundingBox.z0 + 1, Direction::WEST, depth);
		generateAndAddPiece(startPiece, pieces, random, boundingBox.x1 + 1, boundingBox.y0, boundingBox.z0 + 1, Direction::EAST, depth);
		break;
	case Direction::WEST:
		generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 + 1, boundingBox.y0, boundingBox.z0 - 1, Direction::NORTH, depth);
		generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 + 1, boundingBox.y0, boundingBox.z1 + 1, Direction::SOUTH, depth);
		generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 - 1, boundingBox.y0, boundingBox.z0 + 1, Direction::WEST, depth);
		break;
	case Direction::EAST:
		generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 + 1, boundingBox.y0, boundingBox.z0 - 1, Direction::NORTH, depth);
		generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 + 1, boundingBox.y0, boundingBox.z1 + 1, Direction::SOUTH, depth);
		generateAndAddPiece(startPiece, pieces, random, boundingBox.x1 + 1, boundingBox.y0, boundingBox.z0 + 1, Direction::EAST, depth);
		break;
	}

	if (isTwoFloored) {
		if (random.nextBoolean()) {
			generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 + 1, boundingBox.y0 + DEFAULT_SHAFT_HEIGHT + 1, boundingBox.z0 - 1, Direction::NORTH, depth);
		}
		if (random.nextBoolean()) {
			generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 - 1, boundingBox.y0 + DEFAULT_SHAFT_HEIGHT + 1, boundingBox.z0 + 1, Direction::WEST, depth);
		}
		if (random.nextBoolean()) {
			generateAndAddPiece(startPiece, pieces, random, boundingBox.x1 + 1, boundingBox.y0 + DEFAULT_SHAFT_HEIGHT + 1, boundingBox.z0 + 1, Direction::EAST, depth);
		}
		if (random.nextBoolean()) {
			generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 + 1, boundingBox.y0 + DEFAULT_SHAFT_HEIGHT + 1, boundingBox.z1 + 1, Direction::SOUTH, depth);
		}
	}
}

void MineshaftCrossing::_placeSupportPillar(BlockSource* level, const BoundingBox& chunkBB, int x, int y0, int z, int y1) {

	if (getBlock(level, x, y1 + 1, z, chunkBB) != 0) {
		generateBox(level, chunkBB, x, y0, z, x, y1, z, metadata.woodBlock, BlockID::AIR, false);
	}
}

bool MineshaftCrossing::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB){
	if (CHECK_WATER_EDGE && edgesLiquid(level, chunkBB)) {
		return false;
	}

	// corridor air
	if (isTwoFloored) {
		generateBox(level, chunkBB, boundingBox.x0 + 1, boundingBox.y0, boundingBox.z0, boundingBox.x1 - 1, boundingBox.y0 + DEFAULT_SHAFT_HEIGHT - 1, boundingBox.z1, BlockID::AIR,
			BlockID::AIR, false);
		generateBox(level, chunkBB, boundingBox.x0, boundingBox.y0, boundingBox.z0 + 1, boundingBox.x1, boundingBox.y0 + DEFAULT_SHAFT_HEIGHT - 1, boundingBox.z1 - 1, BlockID::AIR,
			BlockID::AIR, false);
		generateBox(level, chunkBB, boundingBox.x0 + 1, boundingBox.y1 - (DEFAULT_SHAFT_HEIGHT - 1), boundingBox.z0, boundingBox.x1 - 1, boundingBox.y1, boundingBox.z1, BlockID::AIR,
			BlockID::AIR, false);
		generateBox(level, chunkBB, boundingBox.x0, boundingBox.y1 - (DEFAULT_SHAFT_HEIGHT - 1), boundingBox.z0 + 1, boundingBox.x1, boundingBox.y1, boundingBox.z1 - 1, BlockID::AIR,
			BlockID::AIR, false);
		generateBox(level, chunkBB, boundingBox.x0 + 1, boundingBox.y0 + DEFAULT_SHAFT_HEIGHT, boundingBox.z0 + 1, boundingBox.x1 - 1, boundingBox.y0 + DEFAULT_SHAFT_HEIGHT,
			boundingBox.z1 - 1, BlockID::AIR, BlockID::AIR, false);
	}
	else {
		generateBox(level, chunkBB, boundingBox.x0 + 1, boundingBox.y0, boundingBox.z0, boundingBox.x1 - 1, boundingBox.y1, boundingBox.z1, BlockID::AIR, BlockID::AIR, false);
		generateBox(level, chunkBB, boundingBox.x0, boundingBox.y0, boundingBox.z0 + 1, boundingBox.x1, boundingBox.y1, boundingBox.z1 - 1, BlockID::AIR, BlockID::AIR, false);
	}

	// support pillars
	_placeSupportPillar(level, chunkBB, boundingBox.x0 + 1, boundingBox.y0, boundingBox.z0 + 1, boundingBox.y1);
	_placeSupportPillar(level, chunkBB, boundingBox.x0 + 1, boundingBox.y0, boundingBox.z1 - 1, boundingBox.y1);
	_placeSupportPillar(level, chunkBB, boundingBox.x1 - 1, boundingBox.y0, boundingBox.z0 + 1, boundingBox.y1);
	_placeSupportPillar(level, chunkBB, boundingBox.x1 - 1, boundingBox.y0, boundingBox.z1 - 1, boundingBox.y1);

	// prevent air floating note: use world coordinates because the
	// corridor hasn't defined orientation
	for (int x = boundingBox.x0; x <= boundingBox.x1; x++) {
		for (int z = boundingBox.z0; z <= boundingBox.z1; z++) {
			if (isAir(level, x, boundingBox.y0 - 1, z, chunkBB) && getBrightness(x, boundingBox.y0 - 1, z, level) < 8) {
				placeBlock(level, metadata.woodBlock, x, boundingBox.y0 - 1, z, chunkBB);
			}
		}
	}

	return true;
}

void MineshaftCrossing::addAdditionalSaveData(CompoundTag& tag) const {
	tag.putBoolean("tf", isTwoFloored);
	tag.putInt("D", direction);
}

void MineshaftCrossing::readAdditionalSaveData(const CompoundTag& tag) {
	isTwoFloored = tag.getBoolean("tf");
	direction = tag.getInt("D");
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

BoundingBox MineshaftStairs::findStairs(PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction){
	// stairs are two steps in, 5x5 steps down, two steps out

	BoundingBox box = BoundingBox(footX, footY - 5, footZ, footX, footY + (DEFAULT_SHAFT_HEIGHT - 1), footZ);

	switch (direction) {
	case Direction::NORTH:
		box.x1 = footX + (DEFAULT_SHAFT_WIDTH - 1);
		box.z0 = footZ - 8;
		break;
	case Direction::SOUTH:
		box.x1 = footX + (DEFAULT_SHAFT_WIDTH - 1);
		box.z1 = footZ + 8;
		break;
	case Direction::WEST:
		box.x0 = footX - 8;
		box.z1 = footZ + (DEFAULT_SHAFT_WIDTH - 1);
		break;
	case Direction::EAST:
		box.x1 = footX + 8;
		box.z1 = footZ + (DEFAULT_SHAFT_WIDTH - 1);
		break;
	}

	if (StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return BoundingBox::getUnknownBox();
	}

	return box;
}

MineshaftStairs::MineshaftStairs(MineshaftData& metadata)
	: MineshaftPiece(0, metadata){

}

MineshaftStairs::MineshaftStairs(MineshaftData& metadata, int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: MineshaftPiece(genDepth, metadata){
	orientation = direction;
	boundingBox = stairsBox;
}

void MineshaftStairs::addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random){
	int depth = getGenDepth();

	// crossings are coming from a direction and will generate children
	// in the remaining three directions
	switch (orientation) {
	case Direction::NORTH:
		generateAndAddPiece(startPiece, pieces, random, boundingBox.x0, boundingBox.y0, boundingBox.z0 - 1, Direction::NORTH, depth);
		break;
	case Direction::SOUTH:
		generateAndAddPiece(startPiece, pieces, random, boundingBox.x0, boundingBox.y0, boundingBox.z1 + 1, Direction::SOUTH, depth);
		break;
	case Direction::WEST:
		generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 - 1, boundingBox.y0, boundingBox.z0, Direction::WEST, depth);
		break;
	case Direction::EAST:
		generateAndAddPiece(startPiece, pieces, random, boundingBox.x1 + 1, boundingBox.y0, boundingBox.z0, Direction::EAST, depth);
		break;
	}

}

bool MineshaftStairs::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB){
	if (CHECK_WATER_EDGE && edgesLiquid(level, chunkBB)) {
		return false;
	}

	// upper floor
	generateBox(level, chunkBB, 0, 5, 0, (DEFAULT_SHAFT_WIDTH - 1), 5 + (DEFAULT_SHAFT_HEIGHT - 1), 1, BlockID::AIR, BlockID::AIR, false);
	// lower floor
	generateBox(level, chunkBB, 0, 0, 7, (DEFAULT_SHAFT_WIDTH - 1), (DEFAULT_SHAFT_HEIGHT - 1), 8, BlockID::AIR, BlockID::AIR, false);

	// stairs
	for (int i = 0; i < 5; i++) {
		generateBox(level, chunkBB, 0, 5 - i - ((i < 4) ? 1 : 0), 2 + i, (DEFAULT_SHAFT_WIDTH - 1), 5 + (DEFAULT_SHAFT_HEIGHT - 1) - i, 2 + i, BlockID::AIR, BlockID::AIR, false);
	}

	return true;
}
