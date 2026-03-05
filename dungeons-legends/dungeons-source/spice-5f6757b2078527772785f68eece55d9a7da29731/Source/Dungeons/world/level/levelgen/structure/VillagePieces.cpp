#include "Dungeons.h"

#include "world/level/levelgen/structure/VillagePieces.h"
#include "world/level/levelgen/structure/VillageFeature.h"

#include "nbt/CompoundTag.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/level/biome/BiomeSource.h"
#include "world/level/biome/Biome.h"
#include "world/level/block/SandStoneBlock.h"
#include "world/level/block/WoolCarpetBlock.h"
#include "world/level/block/NewLogBlock.h"
#include "world/level/block/OldLogBlock.h"
#include "world/level/block/WoodBlock.h"
#include "../SourceDeps/Core/Debug/Log.h"
// #include "world/item/trading/MerchantRecipeList.h"	// needed so android compiler doesn't fail on static_assert(sizeof(
// 													// Unique<MerchantRecipeList> )),
// #include "world/SimpleContainer.h"					//                                      or on static_assert(sizeof(
// 													// Unique<SimpleContainer> ))
// #include "world/entity/npc/Villager.h"
// #include "world/item/DyePowderItem.h"
// #include "world/level/material/Material.h"
// #include "world/item/DoorItem.h"
// #include "world/entity/monster/ZombieVillager.h"
// 
// #include "world/level/Spawner.h"

void MossyStoneSelector::next(Random& random, int worldX, int worldY, int worldZ, bool isEdge) {
	if (random.nextFloat() < .4f) {
		nextBlock = Block::mCobblestone->mID;
	}
	else {
		nextBlock = Block::mMossyCobblestone->mID;
	}
}

MossyStoneSelector VillagePiece::mossyStoneSelector;

VillagePiece::VillagePiece()
	: StructurePiece(0)
	, heightPosition(-1)
	, spawnedVillagerCount(0)
	, isDesertVillage(false)
	, isSavannahVillage(false)
	, isTaigaVillage(false)
	, isAbandoned(false) {
}

VillagePiece::VillagePiece(StartPiece* startPiece, int genDepth)
	: StructurePiece(genDepth)
	, heightPosition(-1)
	, spawnedVillagerCount(0)
	, isDesertVillage(false)
	, isSavannahVillage(false)
	, isTaigaVillage(false)
	, isAbandoned(false) {
	if (startPiece != nullptr) {
		this->isDesertVillage = startPiece->isDesertVillage;
		this->isSavannahVillage = startPiece->isSavannahVillage;
		this->isTaigaVillage = startPiece->isTaigaVillage;
		this->isAbandoned = startPiece->isAbandoned;
	}

	if (isAbandoned) {
		mCobbleSelector = &mossyStoneSelector;
	}
}

void VillagePiece::addAdditionalSaveData(CompoundTag& tag) const {
	tag.putInt("HPos", heightPosition);
	tag.putInt("VCount", spawnedVillagerCount);
	tag.putBoolean("Desert", isDesertVillage);
	tag.putBoolean("Savannah", isSavannahVillage);
	tag.putBoolean("Taiga", isTaigaVillage);
	tag.putBoolean("Abandoned", isAbandoned);
}

void VillagePiece::readAdditionalSaveData(const CompoundTag& tag) {
	heightPosition = tag.getInt("HPos");
	spawnedVillagerCount = tag.getInt("VCount");
	isDesertVillage = tag.getBoolean("Desert");
	isSavannahVillage = tag.getBoolean("Savannah");
	isTaigaVillage = tag.getBoolean("Taiga");
	isAbandoned = tag.getBoolean("Abandoned");

	if (isAbandoned) {
		mCobbleSelector = &mossyStoneSelector;
	}
}

void VillagePiece::spawnVillagers(BlockSource* level, const BoundingBox& chunkBB, int x, int y, int z, int count) {
 	if (spawnedVillagerCount >= count) {
		return;
	}

	for (int i = spawnedVillagerCount; i < count; i++) {
		Vec3 worldPos((float)getWorldX(x + i, z), (float)getWorldY(y), (float)getWorldZ(x + i, z));
		spawnedVillagerCount++;

		Unique<Entity> toSpawn;
		if (level->hasBlock(worldPos)) {

// 			if (isAbandoned) {
// 				ZombieVillager *zombie = static_cast<ZombieVillager*>(level->getLevel().getSpawner().spawnMob(*level, EntityDefinitionIdentifier("minecraft", EntityTypeToString(EntityType::ZombieVillager), "from_village"), nullptr, worldPos + Vec3(.5f, .0f, .5f)));
// 				if (zombie) {
// 					zombie->setPersistent();
// 					zombie->setSpawnedFromVillage();
// 				}
// 			}
// 			else {
// 
// 				LOGI("Villager profession %i\n", getVillagerProfession(0));
// 				std::string professionID = "minecraft:spawn_farmer";
// 				switch (getVillagerProfession(0)) {
// 				default:
// 				case 0:
// 					professionID = "minecraft:spawn_farmer";
// 					break;
// 				case 1:
// 					professionID = "minecraft:spawn_librarian";
// 					break;
// 				case 2:
// 					professionID = "minecraft:spawn_cleric";
// 					break;
// 				case 3:
// 					professionID = "minecraft:spawn_armorer";
// 					break;
// 				case 4:
// 					professionID = "minecraft:spawn_butcher";
// 					break;
// 				}
// 				LOGI("Villager ID %s\n", professionID.c_str());
// 
// 				Mob *result = level->getLevel().getSpawner().spawnMob(*level, EntityDefinitionIdentifier("minecraft", EntityTypeToString(EntityType::Villager), professionID), nullptr, worldPos + Vec3(.5f, .0f, .5f));
// 				if (result) {
// 					result->setPersistent();
// 				}
// 			}
		}
	}
}

 int VillagePiece::getVillagerProfession(int villagerNumber) {
 	return 0; // Profession::Farmer;
 }

FullBlock VillagePiece::biomeBlock(FullBlock block) {
	if (isDesertVillage) {
		if (block.id == Block::mLog->mID || block.id == Block::mLog2->mID) {
			return Block::mSandStone->mID;
		}
		else if (block.id == Block::mCobblestone->mID || block.id == Block::mMossyCobblestone->mID) {
			return FullBlock(Block::mSandStone->mID, enum_cast(SandStoneBlock::SandstoneType::Default));
		}
		else if (block.id == Block::mWoodPlanks->mID) {
			return FullBlock(Block::mSandStone->mID, enum_cast(SandStoneBlock::SandstoneType::Smoothside));
		}
		else if (block.id == Block::mOakStairs->mID) {
			return FullBlock(Block::mSandstoneStairs->mID, block.data);
		}
		else if (block.id == Block::mStoneStairs->mID) {
			return FullBlock(Block::mSandstoneStairs->mID, block.data);
		}
		else if (block.id == Block::mGrassPathBlock->mID) {
			return Block::mSandStone->mID;
		}
	}
	else if (isSavannahVillage) {
		if (block.id == Block::mLog->mID || block.id == Block::mLog2->mID) {
			return FullBlock(Block::mLog2->mID, NewLogBlock::ACACIA_TRUNK);
		}
		else if (block.id == Block::mWoodPlanks->mID) {
			return FullBlock(Block::mWoodPlanks->mID, (int)WoodBlockType::Acacia);
		}
		else if (block.id == Block::mOakStairs->mID) {
			return FullBlock(Block::mAcaciaStairs->mID, block.data);
		}
		else if (block.id == Block::mFence->mID) {
			return FullBlock(Block::mFence->mID, (int)WoodBlockType::Acacia);
		}
		else if (block.id == Block::mWoodenDoor->mID) {
			return FullBlock(Block::mWoodenDoorAcacia->mID, block.data);
		}
	}
	else if (isTaigaVillage) {
		if (block.id == Block::mLog->mID || block.id == Block::mLog2->mID) {
			return FullBlock(Block::mLog->mID, enum_cast(OldLogBlock::LogType::Spruce));
		}
		else if (block.id == Block::mWoodPlanks->mID) {
			return FullBlock(Block::mWoodPlanks->mID, (int)WoodBlockType::Spruce);
		}
		else if (block.id == Block::mOakStairs->mID) {
			return FullBlock(Block::mSpruceStairs->mID, block.data);
		}
		else if (block.id == Block::mFence->mID) {
			return FullBlock(Block::mFence->mID, (int)WoodBlockType::Spruce);
		}
		else if (block.id == Block::mWoodenDoor->mID) {
			return FullBlock(Block::mWoodenDoorSpruce->mID, block.data);
		}
	}
	return block;
}

VillagePiece* VillagePiece::generateHouseNorthernLeft(StartPiece* startPiece, PieceList& pieces, Random& random, int yOff, int zOff) {
	switch (orientation) {
	case Direction::NORTH:
		return generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 - 1, boundingBox.y0 + yOff, boundingBox.z0 + zOff, Direction::WEST, getGenDepth());
	case Direction::SOUTH:
		return generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 - 1, boundingBox.y0 + yOff, boundingBox.z0 + zOff, Direction::WEST, getGenDepth());
	case Direction::WEST:
		return generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 + zOff, boundingBox.y0 + yOff, boundingBox.z0 - 1, Direction::NORTH, getGenDepth());
	case Direction::EAST:
		return generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 + zOff, boundingBox.y0 + yOff, boundingBox.z0 - 1, Direction::NORTH, getGenDepth());
	}
	return nullptr;
}

VillagePiece* VillagePiece::generateHouseNorthernRight(StartPiece* startPiece, PieceList& pieces, Random& random, int yOff, int zOff) {
	switch (orientation) {
	case Direction::NORTH:
		return generateAndAddPiece(startPiece, pieces, random, boundingBox.x1 + 1, boundingBox.y0 + yOff, boundingBox.z0 + zOff, Direction::EAST, getGenDepth());
	case Direction::SOUTH:
		return generateAndAddPiece(startPiece, pieces, random, boundingBox.x1 + 1, boundingBox.y0 + yOff, boundingBox.z0 + zOff, Direction::EAST, getGenDepth());
	case Direction::WEST:
		return generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 + zOff, boundingBox.y0 + yOff, boundingBox.z1 + 1, Direction::SOUTH, getGenDepth());
	case Direction::EAST:
		return generateAndAddPiece(startPiece, pieces, random, boundingBox.x0 + zOff, boundingBox.y0 + yOff, boundingBox.z1 + 1, Direction::SOUTH, getGenDepth());
	}
	return nullptr;
}

VillagePiece* VillagePiece::generateAndAddRoadPiece(StartPiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int depth) {
	if (depth > BASE_ROAD_DEPTH + startPiece->villageSize) {
		return nullptr;
	}
	if (std::abs(footX - startPiece->getBoundingBox().x0) > 7 * 16 || std::abs(footZ - startPiece->getBoundingBox().z0) > 7 * 16) {
		return nullptr;
	}

	BoundingBox box = StraightRoad::findPieceBox(startPiece, pieces, random, footX, footY, footZ, direction);
	if (box.isValid() && box.y0 > LOWEST_Y_POSITION) {
		auto newPiece = make_unique<StraightRoad>(startPiece, depth, random, box, direction);
		//int x = (newPiece->getBoundingBox().x0 + newPiece->getBoundingBox().x1) / 2;
		//int z = (newPiece->getBoundingBox().z0 + newPiece->getBoundingBox().z1) / 2;
		//int xs = newPiece->getBoundingBox().x1 - newPiece->getBoundingBox().x0;
		//int zs = newPiece->getBoundingBox().z1 - newPiece->getBoundingBox().z0;
		//int r = xs > zs ? xs : zs;
		//if (startPiece->getBiomeSource()->containsOnly(x, z, r / 2 + 4, VillageFeature::allowedBiomes)) {
		{
			auto ptr = newPiece.get();
			pieces.push_back(std::move(newPiece));
			startPiece->pendingRoads.push_back(ptr);
			return ptr;
		}
	}

	return nullptr;
}

int VillagePiece::getAverageGroundHeight(BlockSource* level, const BoundingBox& chunkBB) {

	int total = 0;
	int count = 0;

	for (int z = boundingBox.z0; z <= boundingBox.z1; z++) {
		for (int x = boundingBox.x0; x <= boundingBox.x1; x++) {
			if (chunkBB.isInside(x, 64, z)) {
				total += std::max(level->getAboveTopSolidBlock(x, z), (Height)64);
				count++;
			}
		}
	}

	if (count == 0) {
		return -1;
	}
	return total / count;
}

bool VillagePiece::isOkBox(const BoundingBox& box) {
	return box.y0 > LOWEST_Y_POSITION;
}

void VillagePiece::placeBlock(BlockSource* level, FullBlock block, int x, int y, int z, const BoundingBox& chunkBB) {
	StructurePiece::placeBlock(level, biomeBlock(block), x, y, z, chunkBB);
}

void VillagePiece::placeBlock(BlockSource* level, FullBlock defaultBlock, Random random, BlockSelector* selector, int x, int y, int z, const BoundingBox& chunkBB) {
	if (selector) {
		selector->next(random, x, y, z, false);
		placeBlock(level, selector->getNext(), x, y, z, chunkBB);
	} else {
		placeBlock(level, defaultBlock, x, y, z, chunkBB);
	}
}

void VillagePiece::generateBoxPieces(BlockSource* level, const BoundingBox& chunkBB, int x0, int y0, int z0, int x1, int y1, int z1, FullBlock edgeBlock, FullBlock fillBlock, bool skipAir, float randomChance, FullBlock randomBlock){
	auto edge = biomeBlock(edgeBlock);
	auto fill = biomeBlock(fillBlock);
	StructurePiece::generateBox(level, chunkBB, x0, y0, z0, x1, y1, z1, edge, fill, skipAir);
}

void VillagePiece::generateBoxPieces(BlockSource* level, const BoundingBox& chunkBB, int x0, int y0, int z0, int x1, int y1, int z1, bool skipAir, FullBlock defaultBlock, Random& random, BlockSelector* selector) {
	if (selector) {
		StructurePiece::generateBox(level, chunkBB, x0, y0, z0, x1, y1, z1, skipAir, random, selector);
	}else {
		StructurePiece::generateBox(level, chunkBB, x0, y0, z0, x1, y1, z1, defaultBlock, Block::mCobblestone->mID, skipAir);
	}
}

void VillagePiece::fillColumnDown(BlockSource* level, FullBlock block, int x, int startY, int z, const BoundingBox& chunkBB) {

	StructurePiece::fillColumnDown(level, biomeBlock(block), x, startY, z, chunkBB);
}

void VillagePiece::createDoor(BlockSource* level, const BoundingBox& chunkBB, Random& random, int x, int y, int z, int orientation) {
	FullBlock block = biomeBlock(Block::mWoodenDoor->mID);

	BlockPos world = _getWorldPos(x, y, z);

	if (chunkBB.isInside(world)) {
// 		DoorItem::place(level, world.x, world.y, world.z, orientation, Block::mBlocks[block.id]);
	}
}

Unique<VillagePiece> VillagePiece::findAndCreatePieceFactory(StartPiece* startPiece, PieceWeight& piece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction,
	int depth) {

	if (piece.pieceClass == "SimpleHouse") {
		return SimpleHouse::createPiece(startPiece, pieces, random, footX, footY, footZ, direction, depth);
	}
	else if (piece.pieceClass == "SmallTemple") {
		return SmallTemple::createPiece(startPiece, pieces, random, footX, footY, footZ, direction, depth);
	}
	else if (piece.pieceClass == "BookHouse") {
		return BookHouse::createPiece(startPiece, pieces, random, footX, footY, footZ, direction, depth);
	}
	else if (piece.pieceClass == "SmallHut") {
		return SmallHut::createPiece(startPiece, pieces, random, footX, footY, footZ, direction, depth);
	}
	else if (piece.pieceClass == "PigHouse") {
		return PigHouse::createPiece(startPiece, pieces, random, footX, footY, footZ, direction, depth);
	}
	else if (piece.pieceClass == "DoubleFarmland") {
		return DoubleFarmland::createPiece(startPiece, pieces, random, footX, footY, footZ, direction, depth);
	}
	else if (piece.pieceClass == "Farmland") {
		return Farmland::createPiece(startPiece, pieces, random, footX, footY, footZ, direction, depth);
	}
	else if (piece.pieceClass == "Smithy") {
		return Smithy::createPiece(startPiece, pieces, random, footX, footY, footZ, direction, depth);
	}
	else if (piece.pieceClass == "TwoRoomHouse") {
		return TwoRoomHouse::createPiece(startPiece, pieces, random, footX, footY, footZ, direction, depth);
	}
	return nullptr;
}

Unique<VillagePiece> VillagePiece::generatePieceFromSmallDoor(StartPiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int depth) {

	int totalWeight = StructurePiece::getTotalWeight(startPiece->getPieceWeights());
	if (totalWeight <= 0) {
		return nullptr;
	}

	int numAttempts = 0;

	while (numAttempts < 5) {
		numAttempts++;

		int weightSelection = random.nextInt(totalWeight);
		PieceWeightList& pieceWeights = startPiece->getPieceWeights();
		PieceWeightList::iterator pieceIt = pieceWeights.begin();

		while (pieceIt != pieceWeights.end()) {
			weightSelection -= (*pieceIt).weight;
			if (weightSelection < 0) {

				if (!(*pieceIt).doPlace(depth) || ((*pieceIt).pieceClass == startPiece->previousPiece && pieceWeights.size() > 1)) {
					break;
				}

				auto villagePiece = findAndCreatePieceFactory(startPiece, (*pieceIt), pieces, random, footX, footY, footZ, direction, depth);
				if (villagePiece != nullptr) {
					(*pieceIt).placeCount++;
					startPiece->previousPiece = (*pieceIt).pieceClass;

					if (!(*pieceIt).isValid()) {
						pieceWeights.erase(pieceIt);
					}
					return villagePiece;
				}
			}
			++pieceIt;
		}
	}

	// attempt to place a light post instead
	BoundingBox box = LightPost::findPieceBox(startPiece, pieces, random, footX, footY, footZ, direction);
	if (box.isValid()) {
		return make_unique<LightPost>(startPiece, depth, random, box, direction);
	}

	return nullptr;
}

VillagePiece* VillagePiece::generateAndAddPiece(StartPiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int depth) {
	if (depth > MAX_DEPTH) {
		return nullptr;
	}
	if (std::abs(footX - startPiece->getBoundingBox().x0) > 7 * 16 || std::abs(footZ - startPiece->getBoundingBox().z0) > 7 * 16) {
		return nullptr;
	}

	if (auto newPiece = generatePieceFromSmallDoor(startPiece, pieces, random, footX, footY, footZ, direction, depth + 1)) {
			auto ptr = newPiece.get();
			startPiece->pendingHouses.push_back(newPiece.get());
			pieces.push_back(std::move(newPiece));
			return ptr;
		}
	return nullptr;
}

StartPiece::StartPiece(BiomeSource* biomeSource, int genDepth, Random& random, int west, int north, PieceWeightList& pieceSet, int villageSize, bool abandoned)
	: Well(nullptr, 0, random, west, north) {

	this->biomeSource = biomeSource;
	this->pieceWeights = pieceSet;
	this->villageSize = villageSize;
	this->isAbandoned = abandoned;

	Biome* biome = biomeSource->getBiome(west, north);
	isDesertVillage = biome == Biome::desert.get() || biome == Biome::desertHills.get();
	isSavannahVillage = biome == Biome::savanna.get() || biome == Biome::savannaMutated.get() || biome == Biome::savannaRock.get() || biome == Biome::savannaRockMutated.get();
	isTaigaVillage = biome == Biome::iceFlats.get() || biome == Biome::taiga.get() || biome == Biome::taigaCold.get() || biome == Biome::taigaColdHills.get() || biome == Biome::taigaColdMutated.get();
}

BiomeSource* StartPiece::getBiomeSource() {
	return biomeSource;
}

PieceWeightList& StartPiece::getPieceWeights() {
	return pieceWeights;
}

void StartPiece::addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) {
	generateAndAddRoadPiece((StartPiece*) startPiece, pieces, random, boundingBox.x0 - 1, boundingBox.y1 - 4, boundingBox.z0 + 1, Direction::WEST, getGenDepth());
	generateAndAddRoadPiece((StartPiece*) startPiece, pieces, random, boundingBox.x1 + 1, boundingBox.y1 - 4, boundingBox.z0 + 1, Direction::EAST, getGenDepth());
	generateAndAddRoadPiece((StartPiece*) startPiece, pieces, random, boundingBox.x0 + 1, boundingBox.y1 - 4, boundingBox.z0 - 1, Direction::NORTH, getGenDepth());
	generateAndAddRoadPiece((StartPiece*) startPiece, pieces, random, boundingBox.x0 + 1, boundingBox.y1 - 4, boundingBox.z1 + 1, Direction::SOUTH, getGenDepth());
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

Unique<VillagePiece> SimpleHouse::createPiece(StartPiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth) {
	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, 0, 0, 0, width, height, depth, direction);

	if (StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<SimpleHouse>(startPiece, genDepth, random, box, direction);
}

SimpleHouse::SimpleHouse() {

}

SimpleHouse::SimpleHouse(StartPiece* startPiece, int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: VillagePiece(startPiece, genDepth) {
	orientation = direction;
	boundingBox = stairsBox;
	hasTerrace = random.nextBoolean();
}

bool SimpleHouse::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	if (heightPosition < 0) {
		heightPosition = getAverageGroundHeight(level, chunkBB);
		if (heightPosition < 0) {
			return true;
		}
		boundingBox.move(0, heightPosition - boundingBox.y1 + height - 1, 0);
	}

	Random cobbleRandom(random.getSeed());

	// floor
	generateBoxPieces(level, chunkBB, 0, 0, 0, 4, 0, 4, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);
	// roof
	generateBox(level, chunkBB, 0, 4, 0, 4, 4, 4, Block::mLog->mID, Block::mLog->mID, false);
	generateBox(level, chunkBB, 1, 4, 1, 3, 4, 3, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);

	// window walls
	placeBlock(level, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector, 0, 1, 0, chunkBB);
	placeBlock(level, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector, 0, 2, 0, chunkBB);
	placeBlock(level, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector, 0, 3, 0, chunkBB);
	placeBlock(level, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector, 4, 1, 0, chunkBB);
	placeBlock(level, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector, 4, 2, 0, chunkBB);
	placeBlock(level, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector, 4, 3, 0, chunkBB);
	placeBlock(level, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector, 0, 1, 4, chunkBB);
	placeBlock(level, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector, 0, 2, 4, chunkBB);
	placeBlock(level, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector, 0, 3, 4, chunkBB);
	placeBlock(level, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector, 4, 1, 4, chunkBB);
	placeBlock(level, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector, 4, 2, 4, chunkBB);
	placeBlock(level, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector, 4, 3, 4, chunkBB);
	generateBox(level, chunkBB, 0, 1, 1, 0, 3, 3, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	generateBox(level, chunkBB, 4, 1, 1, 4, 3, 3, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	generateBox(level, chunkBB, 1, 1, 4, 3, 3, 4, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	placeBlock(level, Block::mGlassPane->mID, 0, 2, 2, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 2, 2, 4, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 4, 2, 2, chunkBB);

	// door wall
	placeBlock(level, Block::mWoodPlanks->mID, 1, 1, 0, chunkBB);
	placeBlock(level, Block::mWoodPlanks->mID, 1, 2, 0, chunkBB);
	placeBlock(level, Block::mWoodPlanks->mID, 1, 3, 0, chunkBB);
	placeBlock(level, Block::mWoodPlanks->mID, 2, 3, 0, chunkBB);
	placeBlock(level, Block::mWoodPlanks->mID, 3, 3, 0, chunkBB);
	placeBlock(level, Block::mWoodPlanks->mID, 3, 2, 0, chunkBB);
	placeBlock(level, Block::mWoodPlanks->mID, 3, 1, 0, chunkBB);
	if (getBlock(level, 2, 0, -1, chunkBB) == 0 && getBlock(level, 2, -1, -1, chunkBB) != 0) {
		placeBlock(level, FullBlock(Block::mStoneStairs->mID, getOrientationData(Block::mStoneStairs, 3)), 2, 0, -1, chunkBB);
	}

	// fill room with air
	generateBox(level, chunkBB, 1, 1, 1, 3, 3, 3, BlockID::AIR, BlockID::AIR, false);

	if (isAbandoned) {
		placeBlock(level, Block::mWeb->mID, random.nextInt(1, 3), 3, 3, chunkBB);
		placeBlock(level, Block::mWeb->mID, 1, 3, random.nextInt(1, 3), chunkBB);
	}

	// roof fence
	if (hasTerrace) {
		placeBlock(level, Block::mFence->mID, 0, 5, 0, chunkBB);
		placeBlock(level, Block::mFence->mID, 1, 5, 0, chunkBB);
		placeBlock(level, Block::mFence->mID, 2, 5, 0, chunkBB);
		placeBlock(level, Block::mFence->mID, 3, 5, 0, chunkBB);
		placeBlock(level, Block::mFence->mID, 4, 5, 0, chunkBB);
		placeBlock(level, Block::mFence->mID, 0, 5, 4, chunkBB);
		placeBlock(level, Block::mFence->mID, 1, 5, 4, chunkBB);
		placeBlock(level, Block::mFence->mID, 2, 5, 4, chunkBB);
		placeBlock(level, Block::mFence->mID, 3, 5, 4, chunkBB);
		placeBlock(level, Block::mFence->mID, 4, 5, 4, chunkBB);
		placeBlock(level, Block::mFence->mID, 4, 5, 1, chunkBB);
		placeBlock(level, Block::mFence->mID, 4, 5, 2, chunkBB);
		placeBlock(level, Block::mFence->mID, 4, 5, 3, chunkBB);
		placeBlock(level, Block::mFence->mID, 0, 5, 1, chunkBB);
		placeBlock(level, Block::mFence->mID, 0, 5, 2, chunkBB);
		placeBlock(level, Block::mFence->mID, 0, 5, 3, chunkBB);
	}

	// ladder
	if (hasTerrace) {
		int orientationData = getOrientationData(Block::mLadder, 3);
		placeBlock(level, FullBlock(Block::mLadder->mID, orientationData), 3, 1, 3, chunkBB);
		placeBlock(level, FullBlock(Block::mLadder->mID, orientationData), 3, 2, 3, chunkBB);
		placeBlock(level, FullBlock(Block::mLadder->mID, orientationData), 3, 3, 3, chunkBB);
		placeBlock(level, FullBlock(Block::mLadder->mID, orientationData), 3, 4, 3, chunkBB);
	}

	// torch
	if (!isAbandoned) {
		placeBlock(level, FullBlock(Block::mTorch->mID, getTorchData(Direction::SOUTH)), 2, 3, 1, chunkBB);
	}

	for (int z = 0; z < depth; z++) {
		for (int x = 0; x < width; x++) {
			generateAirColumnUp(level, x, height, z, chunkBB);
			fillColumnDown(level, Block::mCobblestone->mID, x, -1, z, chunkBB);
		}
	}

	return true;
}

void SimpleHouse::postProcessMobsAt(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	spawnVillagers(level, chunkBB, 1, 1, 2, 1);
}

void SimpleHouse::addAdditionalSaveData(CompoundTag& tag) const {
	VillagePiece::addAdditionalSaveData(tag);
	tag.putBoolean("Terrace", hasTerrace);
}

void SimpleHouse::readAdditionalSaveData(const CompoundTag& tag) {
	VillagePiece::readAdditionalSaveData(tag);
	hasTerrace = tag.getBoolean("Terrace");
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

Unique<VillagePiece> SmallTemple::createPiece(StartPiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth) {
	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, 0, 0, 0, width, height, depth, direction);

	if (StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<SmallTemple>(startPiece, genDepth, random, box, direction);
}

SmallTemple::SmallTemple() {

}

SmallTemple::SmallTemple(StartPiece* startPiece, int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: VillagePiece(startPiece, genDepth) {
	orientation = direction;
	boundingBox = stairsBox;
}

bool SmallTemple::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	if (heightPosition < 0) {
		heightPosition = getAverageGroundHeight(level, chunkBB);
		if (heightPosition < 0) {
			return true;
		}
		boundingBox.move(0, heightPosition - boundingBox.y1 + height - 1, 0);
	}

	// fill inside with air
	generateBox(level, chunkBB, 1, 1, 1, 3, 3, 7, BlockID::AIR, BlockID::AIR, false);
	generateBox(level, chunkBB, 1, 5, 1, 3, 9, 3, BlockID::AIR, BlockID::AIR, false);

	Random cobbleRandom(random.getSeed());

	// floor
	generateBoxPieces(level, chunkBB, 1, 0, 0, 3, 0, 8, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);

	// front wall
	generateBoxPieces(level, chunkBB, 1, 1, 0, 3, 10, 0, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);
	// left tall wall
	generateBoxPieces(level, chunkBB, 0, 1, 1, 0, 10, 3, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);
	// right tall wall
	generateBoxPieces(level, chunkBB, 4, 1, 1, 4, 10, 3, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);
	// left low wall
	generateBoxPieces(level, chunkBB, 0, 0, 4, 0, 4, 7, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);
	// right low wall
	generateBoxPieces(level, chunkBB, 4, 0, 4, 4, 4, 7, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);
	// far low wall
	generateBoxPieces(level, chunkBB, 1, 1, 8, 3, 4, 8, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);
	// far upper wall
	generateBoxPieces(level, chunkBB, 1, 5, 4, 3, 10, 4, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);

	// low roof
	generateBoxPieces(level, chunkBB, 1, 5, 5, 3, 5, 7, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);
	// high roof
	generateBoxPieces(level, chunkBB, 0, 9, 0, 4, 9, 4, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);

	// middle floor / roof
	generateBoxPieces(level, chunkBB, 0, 4, 0, 4, 4, 4, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);
	placeBlock(level, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector, 0, 11, 2, chunkBB);
	placeBlock(level, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector, 4, 11, 2, chunkBB);
	placeBlock(level, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector, 2, 11, 0, chunkBB);
	placeBlock(level, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector, 2, 11, 4, chunkBB);

	// altar pieces
	placeBlock(level, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector, 1, 1, 6, chunkBB);
	placeBlock(level, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector, 1, 1, 7, chunkBB);
	placeBlock(level, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector, 2, 1, 7, chunkBB);
	placeBlock(level, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector, 3, 1, 6, chunkBB);
	placeBlock(level, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector, 3, 1, 7, chunkBB);
	placeBlock(level, FullBlock(Block::mStoneStairs->mID, getOrientationData(Block::mStoneStairs, 3)), 1, 1, 5, chunkBB);
	placeBlock(level, FullBlock(Block::mStoneStairs->mID, getOrientationData(Block::mStoneStairs, 3)), 2, 1, 6, chunkBB);
	placeBlock(level, FullBlock(Block::mStoneStairs->mID, getOrientationData(Block::mStoneStairs, 3)), 3, 1, 5, chunkBB);
	placeBlock(level, FullBlock(Block::mStoneStairs->mID, getOrientationData(Block::mStoneStairs, 1)), 1, 2, 7, chunkBB);
	placeBlock(level, FullBlock(Block::mStoneStairs->mID, getOrientationData(Block::mStoneStairs, 0)), 3, 2, 7, chunkBB);

	// windows
	placeBlock(level, Block::mGlassPane->mID, 0, 2, 2, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 0, 3, 2, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 4, 2, 2, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 4, 3, 2, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 0, 6, 2, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 0, 7, 2, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 4, 6, 2, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 4, 7, 2, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 2, 6, 0, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 2, 7, 0, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 2, 6, 4, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 2, 7, 4, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 0, 3, 6, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 4, 3, 6, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 2, 3, 8, chunkBB);

	// torches
	if (!isAbandoned) {
		placeBlock(level, FullBlock(Block::mTorch->mID, getTorchData(Direction::NORTH)), 2, 4, 7, chunkBB);
		placeBlock(level, FullBlock(Block::mTorch->mID, getTorchData(Direction::WEST)), 1, 4, 6, chunkBB);
		placeBlock(level, FullBlock(Block::mTorch->mID, getTorchData(Direction::EAST)), 3, 4, 6, chunkBB);
		placeBlock(level, FullBlock(Block::mTorch->mID, getTorchData(Direction::SOUTH)), 2, 4, 5, chunkBB);
	}

	// ladder
	int orientationData = getOrientationData(Block::mLadder, 4);

	for (int y = 1; y <= 9; y++) {
		placeBlock(level, FullBlock(Block::mLadder->mID, orientationData), 3, y, 3, chunkBB);
	}

	// entrance
	placeBlock(level, FullBlock::AIR, 2, 1, 0, chunkBB);
	placeBlock(level, FullBlock::AIR, 2, 2, 0, chunkBB);
	if (!isAbandoned) {
		createDoor(level, chunkBB, random, 2, 1, 0, getOrientationData(Block::mWoodenDoor, 1));
	}
	if (getBlock(level, 2, 0, -1, chunkBB) == 0 && getBlock(level, 2, -1, -1, chunkBB) != 0) {
		placeBlock(level, FullBlock(Block::mStoneStairs->mID, getOrientationData(Block::mStoneStairs, 3)), 2, 0, -1, chunkBB);
	}

	for (int z = 0; z < depth; z++) {
		for (int x = 0; x < width; x++) {
			generateAirColumnUp(level, x, height, z, chunkBB);
			fillColumnDown(level, Block::mCobblestone->mID, x, -1, z, chunkBB);
		}
	}

	return true;
}

void SmallTemple::postProcessMobsAt(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	spawnVillagers(level, chunkBB, 2, 1, 2, 1);
}

void SmallTemple::addAdditionalSaveData(CompoundTag& tag) const {
	VillagePiece::addAdditionalSaveData(tag);
}

void SmallTemple::readAdditionalSaveData(const CompoundTag& tag) {
	VillagePiece::readAdditionalSaveData(tag);
}

int SmallTemple::getVillagerProfession(int villagerNumber) {
	return 2; // Profession::Priest;
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

Unique<VillagePiece> BookHouse::createPiece(StartPiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth) {
	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, 0, 0, 0, width, height, depth, direction);

	if (StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<BookHouse>(startPiece, genDepth, random, box, direction);
}

BookHouse::BookHouse() {

}

BookHouse::BookHouse(StartPiece* startPiece, int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: VillagePiece(startPiece, genDepth) {
	orientation = direction;
	boundingBox = stairsBox;
}

bool BookHouse::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	if (heightPosition < 0) {
		heightPosition = getAverageGroundHeight(level, chunkBB);
		if (heightPosition < 0) {
			return true;
		}
		boundingBox.move(0, heightPosition - boundingBox.y1 + height - 1, 0);
	}

	// fill inside with air
	generateBox(level, chunkBB, 1, 1, 1, 7, 5, 4, BlockID::AIR, BlockID::AIR, false);

	Random cobbleRandom(random.getSeed());

	// floor
	generateBoxPieces(level, chunkBB, 0, 0, 0, 8, 0, 5, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);
	// roof
	generateBoxPieces(level, chunkBB, 0, 5, 0, 8, 5, 5, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);
	generateBoxPieces(level, chunkBB, 0, 6, 1, 8, 6, 4, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);
	generateBoxPieces(level, chunkBB, 0, 7, 2, 8, 7, 3, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);

	int southStairs = getOrientationData(Block::mOakStairs, 3);
	int northStairs = getOrientationData(Block::mOakStairs, 2);

	for (int d = -1; d <= 2; d++) {
		for (int w = 0; w <= 8; w++) {
			placeBlock(level, FullBlock(Block::mOakStairs->mID, southStairs), w, 6 + d, d, chunkBB);
			placeBlock(level, FullBlock(Block::mOakStairs->mID, northStairs), w, 6 + d, 5 - d, chunkBB);
		}
	}
	if (isAbandoned) {
		placeBlock(level, Block::mWeb->mID, 4, 4, -1, chunkBB);
		placeBlock(level, Block::mWeb->mID, 2, 4,  6, chunkBB);
	}

	// rock supports
	generateBoxPieces(level, chunkBB, 0, 1, 0, 0, 1, 5, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);
	generateBoxPieces(level, chunkBB, 1, 1, 5, 8, 1, 5, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);
	generateBoxPieces(level, chunkBB, 8, 1, 0, 8, 1, 4, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);
	generateBoxPieces(level, chunkBB, 2, 1, 0, 7, 1, 0, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);
	generateBoxPieces(level, chunkBB, 0, 2, 0, 0, 4, 0, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);
	generateBoxPieces(level, chunkBB, 0, 2, 5, 0, 4, 5, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);
	generateBoxPieces(level, chunkBB, 8, 2, 5, 8, 4, 5, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);
	generateBoxPieces(level, chunkBB, 8, 2, 0, 8, 4, 0, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);

	// wooden walls
	generateBox(level, chunkBB, 0, 2, 1, 0, 4, 4, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	generateBox(level, chunkBB, 1, 2, 5, 7, 4, 5, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	generateBox(level, chunkBB, 8, 2, 1, 8, 4, 4, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	generateBox(level, chunkBB, 1, 2, 0, 7, 4, 0, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);

	// windows
	placeBlock(level, Block::mGlassPane->mID, 4, 2, 0, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 5, 2, 0, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 6, 2, 0, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 4, 3, 0, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 5, 3, 0, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 6, 3, 0, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 0, 2, 2, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 0, 2, 3, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 0, 3, 2, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 0, 3, 3, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 8, 2, 2, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 8, 2, 3, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 8, 3, 2, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 8, 3, 3, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 2, 2, 5, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 3, 2, 5, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 5, 2, 5, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 6, 2, 5, chunkBB);

	// roof inside and bookshelf
	generateBox(level, chunkBB, 1, 4, 1, 7, 4, 1, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	generateBox(level, chunkBB, 1, 4, 4, 7, 4, 4, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	generateBox(level, chunkBB, 1, 3, 4, 7, 3, 4, Block::mBookshelf->mID, Block::mBookshelf->mID, false);
	if (isAbandoned) {
		placeBlock(level, Block::mWeb->mID, 1, 4, 2, chunkBB);
		placeBlock(level, Block::mWeb->mID, 7, 4, 2, chunkBB);
	}


	// couch
	placeBlock(level, Block::mWoodPlanks->mID, 7, 1, 4, chunkBB);
	placeBlock(level, FullBlock(Block::mOakStairs->mID, getOrientationData(Block::mOakStairs, 0)), 7, 1, 3, chunkBB);
	int orientationData = getOrientationData(Block::mOakStairs, 3);
	placeBlock(level, FullBlock(Block::mOakStairs->mID, orientationData), 6, 1, 4, chunkBB);
	placeBlock(level, FullBlock(Block::mOakStairs->mID, orientationData), 5, 1, 4, chunkBB);
	placeBlock(level, FullBlock(Block::mOakStairs->mID, orientationData), 4, 1, 4, chunkBB);
	placeBlock(level, FullBlock(Block::mOakStairs->mID, orientationData), 3, 1, 4, chunkBB);

	// tables
	placeBlock(level, Block::mFence->mID, 6, 1, 3, chunkBB);
// 	placeBlock(level, FullBlock(Block::mWoolCarpet->mID, WoolCarpetBlock::getBlockDataForItemAuxValue(DyePowderItem::BROWN)), 6, 2, 3, chunkBB);
	//placeBlock(level, Blocks.WOODEN_PRESSURE_PLATE, 0, 6, 2, 3, chunkBB);
	placeBlock(level, Block::mFence->mID, 4, 1, 3, chunkBB);
// 	placeBlock(level, FullBlock(Block::mWoolCarpet->mID, WoolCarpetBlock::getBlockDataForItemAuxValue(DyePowderItem::BROWN)), 4, 2, 3, chunkBB);
	//placeBlock(level, Blocks.WOODEN_PRESSURE_PLATE, 0, 4, 2, 3, chunkBB);
	placeBlock(level, Block::mWorkBench->mID, 7, 1, 1, chunkBB);

	// entrance
	placeBlock(level, FullBlock::AIR, 1, 1, 0, chunkBB);
	placeBlock(level, FullBlock::AIR, 1, 2, 0, chunkBB);
	if (!isAbandoned) {
		createDoor(level, chunkBB, random, 1, 1, 0, getOrientationData(Block::mWoodenDoor, 1));
	}
	if (getBlock(level, 1, 0, -1, chunkBB) == 0 && getBlock(level, 1, -1, -1, chunkBB) != 0) {
		placeBlock(level, FullBlock(Block::mStoneStairs->mID, getOrientationData(Block::mStoneStairs, 3)), 1, 0, -1, chunkBB);
	}

	for (int z = 0; z < depth; z++) {
		for (int x = 0; x < width; x++) {
			generateAirColumnUp(level, x, height, z, chunkBB);
			fillColumnDown(level, Block::mCobblestone->mID, x, -1, z, chunkBB);
		}
	}

	return true;
}

void BookHouse::postProcessMobsAt(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	spawnVillagers(level, chunkBB, 2, 1, 2, 1);
}

void BookHouse::addAdditionalSaveData(CompoundTag& tag) const {
	VillagePiece::addAdditionalSaveData(tag);
}

void BookHouse::readAdditionalSaveData(const CompoundTag& tag) {
	VillagePiece::readAdditionalSaveData(tag);
}

int BookHouse::getVillagerProfession(int villagerNumber) {
	return 1; // Profession::Librarian;
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

Unique<VillagePiece> SmallHut::createPiece(StartPiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth) {
	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, 0, 0, 0, width, height, depth, direction);

	if (StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<SmallHut>(startPiece, genDepth, random, box, direction);
}

SmallHut::SmallHut() {

}

SmallHut::SmallHut(StartPiece* startPiece, int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: VillagePiece(startPiece, genDepth) {
	orientation = direction;
	boundingBox = stairsBox;

	lowCeiling = random.nextBoolean();
	tablePlacement = random.nextInt(3);
}

bool SmallHut::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	if (heightPosition < 0) {
		heightPosition = getAverageGroundHeight(level, chunkBB);
		if (heightPosition < 0) {
			return true;
		}
		boundingBox.move(0, heightPosition - boundingBox.y1 + height - 1, 0);
	}

	// fill inside with air
	generateBox(level, chunkBB, 1, 1, 1, 3, 5, 4, BlockID::AIR, BlockID::AIR, false);

	Random cobbleRandom(random.getSeed());

	// floor
	generateBoxPieces(level, chunkBB, 0, 0, 0, 3, 0, 4, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);
	generateBox(level, chunkBB, 1, 0, 1, 2, 0, 3, Block::mDirt->mID, Block::mDirt->mID, false);
	// roof
	if (lowCeiling) {
		generateBox(level, chunkBB, 1, 4, 1, 2, 4, 3, Block::mLog->mID, Block::mLog->mID, false);
		if (isAbandoned) {
			bool lowX = random.nextBoolean();
			bool lowZ = random.nextBoolean();
			placeBlock(level, Block::mWeb->mID, lowX ? 1 : 2, 3, lowZ ? 1 : 3, chunkBB);
		}
	}
	else {
		generateBox(level, chunkBB, 1, 5, 1, 2, 5, 3, Block::mLog->mID, Block::mLog->mID, false);
		if (isAbandoned) {
			bool lowX = random.nextBoolean();
			bool lowZ = random.nextBoolean();
			placeBlock(level, Block::mWeb->mID, lowX ? 1 : 2, 4, lowZ ? 1 : 3, chunkBB);
		}
	}
	placeBlock(level, Block::mLog->mID, 1, 4, 0, chunkBB);
	placeBlock(level, Block::mLog->mID, 2, 4, 0, chunkBB);
	placeBlock(level, Block::mLog->mID, 1, 4, 4, chunkBB);
	placeBlock(level, Block::mLog->mID, 2, 4, 4, chunkBB);
	placeBlock(level, Block::mLog->mID, 0, 4, 1, chunkBB);
	placeBlock(level, Block::mLog->mID, 0, 4, 2, chunkBB);
	placeBlock(level, Block::mLog->mID, 0, 4, 3, chunkBB);
	placeBlock(level, Block::mLog->mID, 3, 4, 1, chunkBB);
	placeBlock(level, Block::mLog->mID, 3, 4, 2, chunkBB);
	placeBlock(level, Block::mLog->mID, 3, 4, 3, chunkBB);

	// corners
	generateBox(level, chunkBB, 0, 1, 0, 0, 3, 0, Block::mLog->mID, Block::mLog->mID, false);
	generateBox(level, chunkBB, 3, 1, 0, 3, 3, 0, Block::mLog->mID, Block::mLog->mID, false);
	generateBox(level, chunkBB, 0, 1, 4, 0, 3, 4, Block::mLog->mID, Block::mLog->mID, false);
	generateBox(level, chunkBB, 3, 1, 4, 3, 3, 4, Block::mLog->mID, Block::mLog->mID, false);

	// wooden walls
	generateBox(level, chunkBB, 0, 1, 1, 0, 3, 3, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	generateBox(level, chunkBB, 3, 1, 1, 3, 3, 3, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	generateBox(level, chunkBB, 1, 1, 0, 2, 3, 0, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	generateBox(level, chunkBB, 1, 1, 4, 2, 3, 4, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);

	// windows
	placeBlock(level, Block::mGlassPane->mID, 0, 2, 2, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 3, 2, 2, chunkBB);

	// table
	if (tablePlacement > 0) {
		placeBlock(level, Block::mFence->mID, tablePlacement, 1, 3, chunkBB);
// 		placeBlock(level, FullBlock(Block::mWoolCarpet->mID, WoolCarpetBlock::getBlockDataForItemAuxValue(DyePowderItem::BROWN)), tablePlacement, 2, 3, chunkBB);
		//placeBlock(level, Blocks.WOODEN_PRESSURE_PLATE, 0, tablePlacement, 2, 3, chunkBB);
	}

	// entrance
	placeBlock(level, FullBlock::AIR, 1, 1, 0, chunkBB);
	placeBlock(level, FullBlock::AIR, 1, 2, 0, chunkBB);
	if (!isAbandoned) {
		createDoor(level, chunkBB, random, 1, 1, 0, getOrientationData(Block::mWoodenDoor, 1));
	}
	if (getBlock(level, 1, 0, -1, chunkBB) == 0 && getBlock(level, 1, -1, -1, chunkBB) != 0) {
		placeBlock(level, FullBlock(Block::mStoneStairs->mID, getOrientationData(Block::mStoneStairs, 3)), 1, 0, -1, chunkBB);
	}

	for (int z = 0; z < depth; z++) {
		for (int x = 0; x < width; x++) {
			generateAirColumnUp(level, x, height, z, chunkBB);
			fillColumnDown(level, Block::mCobblestone->mID, x, -1, z, chunkBB);
		}
	}

	return true;
}

void SmallHut::postProcessMobsAt(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	spawnVillagers(level, chunkBB, 1, 1, 2, 1);
}

void SmallHut::addAdditionalSaveData(CompoundTag& tag) const {
	VillagePiece::addAdditionalSaveData(tag);
	tag.putInt("T", tablePlacement);
	tag.putBoolean("C", lowCeiling);
}

void SmallHut::readAdditionalSaveData(const CompoundTag& tag) {
	VillagePiece::readAdditionalSaveData(tag);
	tablePlacement = tag.getInt("T");
	lowCeiling = tag.getBoolean("C");
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

Unique<VillagePiece> PigHouse::createPiece(StartPiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth) {
	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, 0, 0, 0, width, height, depth, direction);

	if (StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<PigHouse>(startPiece, genDepth, random, box, direction);
}

PigHouse::PigHouse() {

}

PigHouse::PigHouse(StartPiece* startPiece, int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: VillagePiece(startPiece, genDepth) {
	orientation = direction;
	boundingBox = stairsBox;
}

bool PigHouse::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	if (heightPosition < 0) {
		heightPosition = getAverageGroundHeight(level, chunkBB);
		if (heightPosition < 0) {
			return true;
		}
		boundingBox.move(0, heightPosition - boundingBox.y1 + height - 1, 0);
	}

	// fill inside with air
	generateBox(level, chunkBB, 1, 1, 1, 7, 4, 4, BlockID::AIR, BlockID::AIR, false);
	generateBox(level, chunkBB, 2, 1, 6, 8, 4, 10, BlockID::AIR, BlockID::AIR, false);

	Random cobbleRandom(random.getSeed());

	// pig floor
	generateBox(level, chunkBB, 2, 0, 6, 8, 0, 10, Block::mDirt->mID, Block::mDirt->mID, false);
	placeBlock(level, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector, 6, 0, 6, chunkBB);
	// pig fence
	generateBox(level, chunkBB, 2, 1, 6, 2, 1, 10, Block::mFence->mID, Block::mFence->mID, false);
	generateBox(level, chunkBB, 8, 1, 6, 8, 1, 10, Block::mFence->mID, Block::mFence->mID, false);
	generateBox(level, chunkBB, 3, 1, 10, 7, 1, 10, Block::mFence->mID, Block::mFence->mID, false);

	// floor
	generateBox(level, chunkBB, 1, 0, 1, 7, 0, 4, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	generateBoxPieces(level, chunkBB, 0, 0, 0, 0, 3, 5, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);
	generateBoxPieces(level, chunkBB, 8, 0, 0, 8, 3, 5, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);
	generateBoxPieces(level, chunkBB, 1, 0, 0, 7, 1, 0, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);
	generateBoxPieces(level, chunkBB, 1, 0, 5, 7, 1, 5, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);

	// roof
	generateBox(level, chunkBB, 1, 2, 0, 7, 3, 0, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	generateBox(level, chunkBB, 1, 2, 5, 7, 3, 5, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	generateBox(level, chunkBB, 0, 4, 1, 8, 4, 1, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	generateBox(level, chunkBB, 0, 4, 4, 8, 4, 4, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	generateBox(level, chunkBB, 0, 5, 2, 8, 5, 3, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	placeBlock(level, Block::mWoodPlanks->mID, 0, 4, 2, chunkBB);
	placeBlock(level, Block::mWoodPlanks->mID, 0, 4, 3, chunkBB);
	placeBlock(level, Block::mWoodPlanks->mID, 8, 4, 2, chunkBB);
	placeBlock(level, Block::mWoodPlanks->mID, 8, 4, 3, chunkBB);

	int southStairs = getOrientationData(Block::mOakStairs, 3);
	int northStairs = getOrientationData(Block::mOakStairs, 2);

	for (int d = -1; d <= 2; d++) {
		for (int w = 0; w <= 8; w++) {
			placeBlock(level, FullBlock(Block::mOakStairs->mID, southStairs), w, 4 + d, d, chunkBB);
			placeBlock(level, FullBlock(Block::mOakStairs->mID, northStairs), w, 4 + d, 5 - d, chunkBB);
		}
	}

	// windows etc
	placeBlock(level, Block::mLog->mID, 0, 2, 1, chunkBB);
	placeBlock(level, Block::mLog->mID, 0, 2, 4, chunkBB);
	placeBlock(level, Block::mLog->mID, 8, 2, 1, chunkBB);
	placeBlock(level, Block::mLog->mID, 8, 2, 4, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 0, 2, 2, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 0, 2, 3, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 8, 2, 2, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 8, 2, 3, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 2, 2, 5, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 3, 2, 5, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 5, 2, 0, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 6, 2, 5, chunkBB);

	// table
	placeBlock(level, Block::mFence->mID, 2, 1, 3, chunkBB);
// 	placeBlock(level, FullBlock(Block::mWoolCarpet->mID, WoolCarpetBlock::getBlockDataForItemAuxValue(DyePowderItem::BROWN)), 2, 2, 3, chunkBB);
	//placeBlock(level, Blocks.WOODEN_PRESSURE_PLATE, 0, 2, 2, 3, chunkBB);
	placeBlock(level, Block::mWoodPlanks->mID, 1, 1, 4, chunkBB);
	placeBlock(level, FullBlock(Block::mOakStairs->mID, getOrientationData(Block::mOakStairs, 3)), 2, 1, 4, chunkBB);
	placeBlock(level, FullBlock(Block::mOakStairs->mID, getOrientationData(Block::mOakStairs, 1)), 1, 1, 3, chunkBB);

	// butcher table
	generateBox(level, chunkBB, 5, 0, 1, 7, 0, 3, Block::mDoubleStoneSlab->mID, Block::mDoubleStoneSlab->mID, false);
	placeBlock(level, Block::mDoubleStoneSlab->mID, 6, 1, 1, chunkBB);
	placeBlock(level, Block::mDoubleStoneSlab->mID, 6, 1, 2, chunkBB);

	// entrance
	placeBlock(level, FullBlock::AIR, 2, 1, 0, chunkBB);
	placeBlock(level, FullBlock::AIR, 2, 2, 0, chunkBB);
	
	if (!isAbandoned) {
		placeBlock(level, FullBlock(Block::mTorch->mID, getTorchData(Direction::SOUTH)), 2, 3, 1, chunkBB);
		createDoor(level, chunkBB, random, 2, 1, 0, getOrientationData(Block::mWoodenDoor, 1));
	}
	if (getBlock(level, 2, 0, -1, chunkBB) == 0 && getBlock(level, 2, -1, -1, chunkBB) != 0) {
		placeBlock(level, FullBlock(Block::mStoneStairs->mID, getOrientationData(Block::mStoneStairs, 3)), 2, 0, -1, chunkBB);
	}

	// pig entrance
	placeBlock(level, FullBlock::AIR, 6, 1, 5, chunkBB);
	placeBlock(level, FullBlock::AIR, 6, 2, 5, chunkBB);
	if (!isAbandoned) {
		placeBlock(level, FullBlock(Block::mTorch->mID, getTorchData(Direction::NORTH)), 6, 3, 4, chunkBB);
		createDoor(level, chunkBB, random, 6, 1, 5, getOrientationData(Block::mWoodenDoor, 1));
	}

	for (int z = 0; z < 5; z++) {
		for (int x = 0; x < width; x++) {
			generateAirColumnUp(level, x, height, z, chunkBB);
			fillColumnDown(level, Block::mCobblestone->mID, x, -1, z, chunkBB);
		}
	}

	return true;
}

void PigHouse::postProcessMobsAt(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	spawnVillagers(level, chunkBB, 4, 1, 2, 2);
}

void PigHouse::addAdditionalSaveData(CompoundTag& tag) const {
	VillagePiece::addAdditionalSaveData(tag);
}

void PigHouse::readAdditionalSaveData(const CompoundTag& tag) {
	VillagePiece::readAdditionalSaveData(tag);
}

int PigHouse::getVillagerProfession(int villagerNumber) {
	if (villagerNumber == 0) {
		return 4; // Profession::Butcher;
	}
	return 0; // Profession::Farmer;
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

Unique<VillagePiece> DoubleFarmland::createPiece(StartPiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth) {
	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, 0, 0, 0, width, height, depth, direction);

	if (StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<DoubleFarmland>(startPiece, genDepth, random, box, direction);
}

DoubleFarmland::DoubleFarmland() :
	cropsA(BlockID::AIR)
	, cropsB(BlockID::AIR)
	, cropsC(BlockID::AIR)
	, cropsD(BlockID::AIR) {

}

DoubleFarmland::DoubleFarmland(StartPiece* startPiece, int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: VillagePiece(startPiece, genDepth)
	, cropsA(Farmland::selectCrops(random, *startPiece))
	, cropsB(Farmland::selectCrops(random, *startPiece))
	, cropsC(Farmland::selectCrops(random, *startPiece))
	, cropsD(Farmland::selectCrops(random, *startPiece)) {
	orientation = direction;
	boundingBox = stairsBox;

}

bool DoubleFarmland::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	if (heightPosition < 0) {
		heightPosition = getAverageGroundHeight(level, chunkBB);
		if (heightPosition < 0) {
			return true;
		}
		boundingBox.move(0, heightPosition - boundingBox.y1 + height - 1, 0);
	}

	// fill inside with air
	generateBox(level, chunkBB, 0, 1, 0, 12, 4, 8, BlockID::AIR, BlockID::AIR, false);

	// Change some stuff if we are in a snow covered biome
	BlockID farmland = Block::mFarmland->mID;
	Biome & biome = level->getBiome(BlockPos(chunkBB.getXCenter(), chunkBB.getYCenter(), chunkBB.getZCenter()));
	if (biome.isSnowCovered()) {
		farmland = Block::mDirt->mID;
	}

	// farmlands
	generateBox(level, chunkBB, 1, 0, 1, 2, 0, 7, farmland, Block::mFarmland->mID, false);
	generateBox(level, chunkBB, 4, 0, 1, 5, 0, 7, farmland, Block::mFarmland->mID, false);
	generateBox(level, chunkBB, 7, 0, 1, 8, 0, 7, farmland, Block::mFarmland->mID, false);
	generateBox(level, chunkBB, 10, 0, 1, 11, 0, 7, farmland, Block::mFarmland->mID, false);
	// walkpaths
	generateBox(level, chunkBB, 0, 0, 0, 0, 0, 8, Block::mLog->mID, Block::mLog->mID, false);
	generateBox(level, chunkBB, 6, 0, 0, 6, 0, 8, Block::mLog->mID, Block::mLog->mID, false);
	generateBox(level, chunkBB, 12, 0, 0, 12, 0, 8, Block::mLog->mID, Block::mLog->mID, false);
	generateBox(level, chunkBB, 1, 0, 0, 11, 0, 0, Block::mLog->mID, Block::mLog->mID, false);
	generateBox(level, chunkBB, 1, 0, 8, 11, 0, 8, Block::mLog->mID, Block::mLog->mID, false);

	// water
	generateBox(level, chunkBB, 3, 0, 1, 3, 0, 7, Block::mStillWater->mID, Block::mStillWater->mID, false);
	generateBox(level, chunkBB, 9, 0, 1, 9, 0, 7, Block::mStillWater->mID, Block::mStillWater->mID, false);

	// crops
	if (!biome.isSnowCovered()) {
		for (int d = 1; d <= 7; d++) {
			placeBlock(level, FullBlock(cropsA, Math::nextInt(random, 2, 7)), 1, 1, d, chunkBB);
			placeBlock(level, FullBlock(cropsA, Math::nextInt(random, 2, 7)), 2, 1, d, chunkBB);
			placeBlock(level, FullBlock(cropsB, Math::nextInt(random, 2, 7)), 4, 1, d, chunkBB);
			placeBlock(level, FullBlock(cropsB, Math::nextInt(random, 2, 7)), 5, 1, d, chunkBB);
			placeBlock(level, FullBlock(cropsC, Math::nextInt(random, 2, 7)), 7, 1, d, chunkBB);
			placeBlock(level, FullBlock(cropsC, Math::nextInt(random, 2, 7)), 8, 1, d, chunkBB);
			placeBlock(level, FullBlock(cropsD, Math::nextInt(random, 2, 7)), 10, 1, d, chunkBB);
			placeBlock(level, FullBlock(cropsD, Math::nextInt(random, 2, 7)), 11, 1, d, chunkBB);
		}
	}

	for (int z = 0; z < depth; z++) {
		for (int x = 0; x < width; x++) {
			generateAirColumnUp(level, x, height, z, chunkBB);
			fillColumnDown(level, Block::mDirt->mID, x, -1, z, chunkBB);
		}
	}

	return true;
}

void DoubleFarmland::addAdditionalSaveData(CompoundTag& tag) const {
	VillagePiece::addAdditionalSaveData(tag);

	tag.putInt("CA", cropsA);
	tag.putInt("CB", cropsB);
	tag.putInt("CC", cropsC);
	tag.putInt("CD", cropsD);
}

void DoubleFarmland::readAdditionalSaveData(const CompoundTag& tag) {
	VillagePiece::readAdditionalSaveData(tag);

	cropsA = (BlockID)tag.getInt("CA");
	cropsB = (BlockID)tag.getInt("CB");
	cropsC = (BlockID)tag.getInt("CC");
	cropsD = (BlockID)tag.getInt("CD");
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

Unique<VillagePiece> Farmland::createPiece(StartPiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth) {
	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, 0, 0, 0, width, height, depth, direction);

	if (StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<Farmland>(startPiece, genDepth, random, box, direction);
}

Farmland::Farmland() :
	cropsA(BlockID::AIR)
	, cropsB(BlockID::AIR) {

}

Farmland::Farmland(StartPiece* startPiece, int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: VillagePiece(startPiece, genDepth)
	, cropsA(Farmland::selectCrops(random, *startPiece))
	, cropsB(Farmland::selectCrops(random, *startPiece)) {
	orientation = direction;
	boundingBox = stairsBox;

}

bool Farmland::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	if (heightPosition < 0) {
		heightPosition = getAverageGroundHeight(level, chunkBB);
		if (heightPosition < 0) {
			return true;
		}
		boundingBox.move(0, heightPosition - boundingBox.y1 + height - 1, 0);
	}

	// Change some stuff if we are in a snow covered biome
	BlockID farmland = Block::mFarmland->mID;
	Biome & biome = level->getBiome(BlockPos(chunkBB.getXCenter(), chunkBB.getYCenter(), chunkBB.getZCenter()));
	if (biome.isSnowCovered()) {
		farmland = Block::mDirt->mID;
	}

	// fill inside with air
	generateBox(level, chunkBB, 0, 1, 0, 6, 4, 8, BlockID::AIR, BlockID::AIR, false);

	// farmlands
	generateBox(level, chunkBB, 1, 0, 1, 2, 0, 7, farmland, Block::mFarmland->mID, false);
	generateBox(level, chunkBB, 4, 0, 1, 5, 0, 7, farmland, Block::mFarmland->mID, false);

	// walkpaths
	generateBox(level, chunkBB, 0, 0, 0, 0, 0, 8, Block::mLog->mID, Block::mLog->mID, false);
	generateBox(level, chunkBB, 6, 0, 0, 6, 0, 8, Block::mLog->mID, Block::mLog->mID, false);
	generateBox(level, chunkBB, 1, 0, 0, 5, 0, 0, Block::mLog->mID, Block::mLog->mID, false);
	generateBox(level, chunkBB, 1, 0, 8, 5, 0, 8, Block::mLog->mID, Block::mLog->mID, false);

	// water
	generateBox(level, chunkBB, 3, 0, 1, 3, 0, 7, Block::mStillWater->mID, Block::mStillWater->mID, false);

	// crops
	if (!biome.isSnowCovered()) {
		for (int d = 1; d <= 7; d++) {
			placeBlock(level, FullBlock(cropsA, Math::nextInt(random, 2, 7)), 1, 1, d, chunkBB);
			placeBlock(level, FullBlock(cropsA, Math::nextInt(random, 2, 7)), 2, 1, d, chunkBB);
			placeBlock(level, FullBlock(cropsB, Math::nextInt(random, 2, 7)), 4, 1, d, chunkBB);
			placeBlock(level, FullBlock(cropsB, Math::nextInt(random, 2, 7)), 5, 1, d, chunkBB);
		}
	}

	for (int z = 0; z < depth; z++) {
		for (int x = 0; x < width; x++) {
			generateAirColumnUp(level, x, height, z, chunkBB);
			fillColumnDown(level, Block::mDirt->mID, x, -1, z, chunkBB);
		}
	}

	return true;
}

void Farmland::addAdditionalSaveData(CompoundTag& tag) const {
	VillagePiece::addAdditionalSaveData(tag);

	tag.putInt("CA", cropsA);
	tag.putInt("CB", cropsB);

}

void Farmland::readAdditionalSaveData(const CompoundTag& tag) {
	VillagePiece::readAdditionalSaveData(tag);

	cropsA = (BlockID)tag.getInt("CA");
	cropsB = (BlockID)tag.getInt("CB");

}

BlockID Farmland::selectCrops(Random& random, StartPiece& start) {
	switch (random.nextInt(3)) {
	default:
		return Block::mWheatCrop->mID;
	case 1:
		//pick the next one using the root position, so that it's always the same for a given village
		auto hash = start.getLocatorPosition().hashCode();

		switch ((hash >> 3) % 3) {
		case 0:
			return Block::mPotatoCrop->mID;
		case 1:
			return Block::mBeetrootCrop->mID;
		default:
			return Block::mCarrotCrop->mID;
		}
	}
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

Unique<VillagePiece> Smithy::createPiece(StartPiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth) {
	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, 0, 0, 0, width, height, depth, direction);

	if (StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<Smithy>(startPiece, genDepth, random, box, direction);
}

Smithy::Smithy() {

}

Smithy::Smithy(StartPiece* startPiece, int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: VillagePiece(startPiece, genDepth) {
	orientation = direction;
	boundingBox = stairsBox;

	hasPlacedChest = false;
}

bool Smithy::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	if (heightPosition < 0) {
		heightPosition = getAverageGroundHeight(level, chunkBB);
		if (heightPosition < 0) {
			return true;
		}
		boundingBox.move(0, heightPosition - boundingBox.y1 + height - 1, 0);
	}

	// fill inside with air
	generateBox(level, chunkBB, 0, 1, 0, 9, 4, 6, BlockID::AIR, BlockID::AIR, false);

	Random cobbleRandom(random.getSeed());

	// floor
	generateBoxPieces(level, chunkBB, 0, 0, 0, 9, 0, 6, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);

	// roof
	generateBoxPieces(level, chunkBB, 0, 4, 0, 9, 4, 6, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);
	generateBox(level, chunkBB, 0, 5, 0, 9, 5, 6, Block::mStoneSlab->mID, Block::mStoneSlab->mID, false);
	generateBox(level, chunkBB, 1, 5, 1, 8, 5, 5, BlockID::AIR, BlockID::AIR, false);

	// room walls
	generateBox(level, chunkBB, 1, 1, 0, 2, 3, 0, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	generateBox(level, chunkBB, 0, 1, 0, 0, 4, 0, Block::mLog->mID, Block::mLog->mID, false);
	generateBox(level, chunkBB, 3, 1, 0, 3, 4, 0, Block::mLog->mID, Block::mLog->mID, false);
	generateBox(level, chunkBB, 0, 1, 6, 0, 4, 6, Block::mLog->mID, Block::mLog->mID, false);
	placeBlock(level, Block::mWoodPlanks->mID, 3, 3, 1, chunkBB);
	generateBox(level, chunkBB, 3, 1, 2, 3, 3, 2, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	generateBox(level, chunkBB, 4, 1, 3, 5, 3, 3, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	generateBox(level, chunkBB, 0, 1, 1, 0, 3, 5, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	generateBox(level, chunkBB, 1, 1, 6, 5, 3, 6, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);

	// pillars
	generateBox(level, chunkBB, 5, 1, 0, 5, 3, 0, Block::mFence->mID, Block::mFence->mID, false);
	generateBox(level, chunkBB, 9, 1, 0, 9, 3, 0, Block::mFence->mID, Block::mFence->mID, false);

	// furnace
	generateBoxPieces(level, chunkBB, 6, 1, 4, 9, 4, 6, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);
	placeBlock(level, Block::mFlowingLava->mID, 7, 1, 5, chunkBB);
	placeBlock(level, Block::mFlowingLava->mID, 8, 1, 5, chunkBB);
	placeBlock(level, Block::mIronFence->mID, 9, 2, 5, chunkBB);
	placeBlock(level, Block::mIronFence->mID, 9, 2, 4, chunkBB);
	generateBox(level, chunkBB, 7, 2, 4, 8, 2, 5, BlockID::AIR, BlockID::AIR, false);
	placeBlock(level, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector, 6, 1, 3, chunkBB);
	placeBlock(level, FullBlock(Block::mFurnace->mID, Direction::DIRECTION_FACING[Direction::DIRECTION_OPPOSITE[orientation]]), 6, 2, 3, chunkBB);
	placeBlock(level, FullBlock(Block::mFurnace->mID, Direction::DIRECTION_FACING[Direction::DIRECTION_OPPOSITE[orientation]]), 6, 3, 3, chunkBB);
	placeBlock(level, Block::mDoubleStoneSlab->mID, 8, 1, 1, chunkBB);

	// windows etc
	placeBlock(level, Block::mGlassPane->mID, 0, 2, 2, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 0, 2, 4, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 2, 2, 6, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 4, 2, 6, chunkBB);

	// table
	placeBlock(level, Block::mFence->mID, 2, 1, 4, chunkBB);
// 	placeBlock(level, FullBlock(Block::mWoolCarpet->mID, WoolCarpetBlock::getBlockDataForItemAuxValue(DyePowderItem::BROWN)), 2, 2, 4, chunkBB);
	//placeBlock(level, Blocks.WOODEN_PRESSURE_PLATE, 0, 2, 2, 4, chunkBB);
	placeBlock(level, Block::mWoodPlanks->mID, 1, 1, 5, chunkBB);
	placeBlock(level, FullBlock(Block::mOakStairs->mID, getOrientationData(Block::mOakStairs, 3)), 2, 1, 5, chunkBB);
	placeBlock(level, FullBlock(Block::mOakStairs->mID, getOrientationData(Block::mOakStairs, 1)), 1, 1, 4, chunkBB);

	if (!hasPlacedChest) {
		int y = getWorldY(1);
		int x = getWorldX(5, 5), z = getWorldZ(5, 5);
		if (chunkBB.isInside(x, y, z)) {
			hasPlacedChest = true;
			createChest(level, chunkBB, random, 5, 1, 5, Direction::WEST, "loot_tables/chests/village_blacksmith.json");
		}
	}

	// entrance
	for (int x = 6; x <= 8; x++) {
		if (getBlock(level, x, 0, -1, chunkBB) == 0 && getBlock(level, x, -1, -1, chunkBB) != 0) {
			placeBlock(level, FullBlock(Block::mStoneStairs->mID, getOrientationData(Block::mStoneStairs, 3)), x, 0, -1, chunkBB);
		}
	}

	for (int z = 0; z < depth; z++) {
		for (int x = 0; x < width; x++) {
			generateAirColumnUp(level, x, height, z, chunkBB);
			fillColumnDown(level, Block::mCobblestone->mID, x, -1, z, chunkBB);
		}
	}

	return true;
}

void Smithy::postProcessMobsAt(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	spawnVillagers(level, chunkBB, 7, 1, 1, 1);
}

void Smithy::addAdditionalSaveData(CompoundTag& tag) const {
	VillagePiece::addAdditionalSaveData(tag);

	tag.putBoolean("Chest", hasPlacedChest);
}

void Smithy::readAdditionalSaveData(const CompoundTag& tag) {
	VillagePiece::readAdditionalSaveData(tag);

	hasPlacedChest = tag.getBoolean("Chest");
}

int Smithy::getVillagerProfession(int villagerNumber) {
	return 3;// Profession::Smith;
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

Unique<VillagePiece> TwoRoomHouse::createPiece(StartPiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction, int genDepth) {
	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, 0, 0, 0, width, height, depth, direction);

	if (StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return nullptr;
	}

	return make_unique<TwoRoomHouse>(startPiece, genDepth, random, box, direction);
}

TwoRoomHouse::TwoRoomHouse() {

}

TwoRoomHouse::TwoRoomHouse(StartPiece* startPiece, int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: VillagePiece(startPiece, genDepth) {
	orientation = direction;
	boundingBox = stairsBox;
}

bool TwoRoomHouse::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	if (heightPosition < 0) {
		heightPosition = getAverageGroundHeight(level, chunkBB);
		if (heightPosition < 0) {
			return true;
		}
		boundingBox.move(0, heightPosition - boundingBox.y1 + height - 1, 0);
	}

	Random cobbleRandom(random.getSeed());

	// fill inside with air
	generateBox(level, chunkBB, 1, 1, 1, 7, 4, 4, BlockID::AIR, BlockID::AIR, false);
	generateBox(level, chunkBB, 2, 1, 6, 8, 4, 10, BlockID::AIR, BlockID::AIR, false);

	// floor
	generateBox(level, chunkBB, 2, 0, 5, 8, 0, 10, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	generateBox(level, chunkBB, 1, 0, 1, 7, 0, 4, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	generateBoxPieces(level, chunkBB, 0, 0, 0, 0, 3, 5, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);
	generateBoxPieces(level, chunkBB, 8, 0, 0, 8, 3, 10, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);
	generateBoxPieces(level, chunkBB, 1, 0, 0, 7, 2, 0, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);
	generateBoxPieces(level, chunkBB, 1, 0, 5, 2, 1, 5, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);
	generateBoxPieces(level, chunkBB, 2, 0, 6, 2, 3, 10, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);
	generateBoxPieces(level, chunkBB, 3, 0, 10, 7, 3, 10, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);

	// room 1 roof
	generateBox(level, chunkBB, 1, 2, 0, 7, 3, 0, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	generateBox(level, chunkBB, 1, 2, 5, 2, 3, 5, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	generateBox(level, chunkBB, 0, 4, 1, 8, 4, 1, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	generateBox(level, chunkBB, 0, 4, 4, 3, 4, 4, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	generateBox(level, chunkBB, 0, 5, 2, 8, 5, 3, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	placeBlock(level, Block::mWoodPlanks->mID, 0, 4, 2, chunkBB);
	placeBlock(level, Block::mWoodPlanks->mID, 0, 4, 3, chunkBB);
	placeBlock(level, Block::mWoodPlanks->mID, 8, 4, 2, chunkBB);
	placeBlock(level, Block::mWoodPlanks->mID, 8, 4, 3, chunkBB);
	placeBlock(level, Block::mWoodPlanks->mID, 8, 4, 4, chunkBB);

	int southStairs = getOrientationData(Block::mOakStairs, 3);
	int northStairs = getOrientationData(Block::mOakStairs, 2);

	for (int d = -1; d <= 2; d++) {
		for (int w = 0; w <= 8; w++) {
			placeBlock(level, FullBlock(Block::mOakStairs->mID, southStairs), w, 4 + d, d, chunkBB);
			if ((d > -1 || w <= 1) && (d > 0 || w <= 3) && (d > 1 || w <= 4 || w >= 6)) {
				placeBlock(level, FullBlock(Block::mOakStairs->mID, northStairs), w, 4 + d, 5 - d, chunkBB);
			}
		}
	}

	// room 2 roof
	generateBox(level, chunkBB, 3, 4, 5, 3, 4, 10, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	generateBox(level, chunkBB, 7, 4, 2, 7, 4, 10, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	generateBox(level, chunkBB, 4, 5, 4, 4, 5, 10, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	generateBox(level, chunkBB, 6, 5, 4, 6, 5, 10, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	generateBox(level, chunkBB, 5, 6, 3, 5, 6, 10, Block::mWoodPlanks->mID, Block::mWoodPlanks->mID, false);
	int westStairs = getOrientationData(Block::mOakStairs, 0);

	if (isAbandoned) {
		placeBlock(level, Block::mWeb->mID, 5, 5, 3, chunkBB);
		placeBlock(level, Block::mWeb->mID, 5, 5, 4, chunkBB);
		placeBlock(level, Block::mWeb->mID, 5, 5, 10, chunkBB);
	}

	for(auto w : reverse_range(1, 5)) {
		placeBlock(level, Block::mWoodPlanks->mID, w, 2 + w, 7 - w, chunkBB);

		for (int d = 8 - w; d <= 10; d++) {
			placeBlock(level, FullBlock(Block::mOakStairs->mID, westStairs), w, 2 + w, d, chunkBB);
		}
	}
	int eastStairs = getOrientationData(Block::mOakStairs, 1);
	placeBlock(level, Block::mWoodPlanks->mID, 6, 6, 3, chunkBB);
	placeBlock(level, Block::mWoodPlanks->mID, 7, 5, 4, chunkBB);
	placeBlock(level, FullBlock(Block::mOakStairs->mID, eastStairs), 6, 6, 4, chunkBB);

	for (int w = 6; w <= 8; w++) {
		for (int d = 5; d <= 10; d++) {
			placeBlock(level, FullBlock(Block::mOakStairs->mID, eastStairs), w, 12 - w, d, chunkBB);
		}
	}

	// windows etc
	placeBlock(level, Block::mLog->mID, 0, 2, 1, chunkBB);
	placeBlock(level, Block::mLog->mID, 0, 2, 4, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 0, 2, 2, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 0, 2, 3, chunkBB);

	placeBlock(level, Block::mLog->mID, 4, 2, 0, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 5, 2, 0, chunkBB);
	placeBlock(level, Block::mLog->mID, 6, 2, 0, chunkBB);

	placeBlock(level, Block::mLog->mID, 8, 2, 1, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 8, 2, 2, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 8, 2, 3, chunkBB);
	placeBlock(level, Block::mLog->mID, 8, 2, 4, chunkBB);
	placeBlock(level, Block::mWoodPlanks->mID, 8, 2, 5, chunkBB);
	placeBlock(level, Block::mLog->mID, 8, 2, 6, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 8, 2, 7, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 8, 2, 8, chunkBB);
	placeBlock(level, Block::mLog->mID, 8, 2, 9, chunkBB);
	placeBlock(level, Block::mLog->mID, 2, 2, 6, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 2, 2, 7, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 2, 2, 8, chunkBB);
	placeBlock(level, Block::mLog->mID, 2, 2, 9, chunkBB);

	placeBlock(level, Block::mLog->mID, 4, 4, 10, chunkBB);
	placeBlock(level, Block::mGlassPane->mID, 5, 4, 10, chunkBB);
	placeBlock(level, Block::mLog->mID, 6, 4, 10, chunkBB);
	placeBlock(level, Block::mWoodPlanks->mID, 5, 5, 10, chunkBB);

	// entrance
	placeBlock(level, FullBlock::AIR, 2, 1, 0, chunkBB);
	placeBlock(level, FullBlock::AIR, 2, 2, 0, chunkBB);
	
	if (!isAbandoned) {
		placeBlock(level, FullBlock(Block::mTorch->mID, getTorchData(Direction::SOUTH)), 2, 3, 1, chunkBB);
		createDoor(level, chunkBB, random, 2, 1, 0, getOrientationData(Block::mWoodenDoor, 1));
	}
	generateBox(level, chunkBB, 1, 0, -1, 3, 2, -1, BlockID::AIR, BlockID::AIR, false);
	if (getBlock(level, 2, 0, -1, chunkBB) == 0 && getBlock(level, 2, -1, -1, chunkBB) != 0) {
		placeBlock(level, FullBlock(Block::mStoneStairs->mID, getOrientationData(Block::mStoneStairs, 3)), 2, 0, -1, chunkBB);
	}

	for (int z = 0; z < 5; z++) {
		for (int x = 0; x < width; x++) {
			generateAirColumnUp(level, x, height, z, chunkBB);
			fillColumnDown(level, Block::mCobblestone->mID, x, -1, z, chunkBB);
		}
	}

	for (int z = 5; z < depth - 1; z++) {
		for (int x = 2; x < width; x++) {
			generateAirColumnUp(level, x, height, z, chunkBB);
			fillColumnDown(level, Block::mCobblestone->mID, x, -1, z, chunkBB);
		}
	}

	Biome & biome = level->getBiome(BlockPos(chunkBB.getXCenter(), chunkBB.getYCenter(), chunkBB.getZCenter()));
	if (biome.isSnowCovered() && !hasPlacedChest) {
		int y = getWorldY(1);
		int x = getWorldX(5, 9), z = getWorldZ(5, 9);
		if (chunkBB.isInside(x, y, z)) {
			createChest(level, chunkBB, random, 5, 1, 9, Direction::SOUTH, "loot_tables/chests/village_two_room_house.json");
			hasPlacedChest = true;
		}
	}

	return true;
}

void TwoRoomHouse::postProcessMobsAt(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	spawnVillagers(level, chunkBB, 4, 1, 2, 2);
}

void TwoRoomHouse::addAdditionalSaveData(CompoundTag& tag) const {
	tag.putBoolean("Chest", hasPlacedChest);
	VillagePiece::addAdditionalSaveData(tag);
}

void TwoRoomHouse::readAdditionalSaveData(const CompoundTag& tag) {
	hasPlacedChest = tag.getBoolean("Chest");
	VillagePiece::readAdditionalSaveData(tag);
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

BoundingBox LightPost::findPieceBox(StartPiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction) {
	BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, 0, 0, 0, width, height, depth, direction);

	if (StructurePiece::findCollisionPiece(pieces, box) != nullptr) {
		return BoundingBox::getUnknownBox();
	}

	return box;
}

LightPost::LightPost() {

}

LightPost::LightPost(StartPiece* startPiece, int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: VillagePiece(startPiece, genDepth) {

	orientation = direction;
	boundingBox = stairsBox;

}

bool LightPost::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	if (heightPosition < 0) {
		heightPosition = getAverageGroundHeight(level, chunkBB);
		if (heightPosition < 0) {
			return true;
		}
		boundingBox.move(0, heightPosition - boundingBox.y1 + height - 1, 0);
	}

	// fill with air
	generateBox(level, chunkBB, 0, 0, 0, 2, 3, 1, BlockID::AIR, BlockID::AIR, false);

	// pillar
	placeBlock(level, Block::mFence->mID, 1, 0, 0, chunkBB);
	placeBlock(level, Block::mFence->mID, 1, 1, 0, chunkBB);
	placeBlock(level, Block::mFence->mID, 1, 2, 0, chunkBB);

	// head
// 	placeBlock(level, FullBlock(Block::mWool->mID, DyePowderItem::WHITE), 1, 3, 0, chunkBB);

	// torches
	if (!isAbandoned) {
		placeBlock(level, FullBlock(Block::mTorch->mID, getTorchData(Direction::EAST)), 0, 3, 0, chunkBB);
		placeBlock(level, FullBlock(Block::mTorch->mID, getTorchData(Direction::SOUTH)), 1, 3, 1, chunkBB);
		placeBlock(level, FullBlock(Block::mTorch->mID, getTorchData(Direction::WEST)), 2, 3, 0, chunkBB);
		placeBlock(level, FullBlock(Block::mTorch->mID, getTorchData(Direction::NORTH)), 1, 3, -1, chunkBB);
	}

	return true;
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

StraightRoad::StraightRoad() {
	// for reflection
}

StraightRoad::StraightRoad(StartPiece* startPiece, int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: VillagePiece(startPiece, genDepth) {
	orientation = direction;
	boundingBox = stairsBox;
	length = std::max(stairsBox.getXSpan(), stairsBox.getZSpan());
}

void StraightRoad::addAdditionalSaveData(CompoundTag& tag) const {
	VillagePiece::addAdditionalSaveData(tag);
	tag.putInt("Length", length);
}

void StraightRoad::readAdditionalSaveData(const CompoundTag& tag) {
	VillagePiece::readAdditionalSaveData(tag);
	length = tag.getInt("Length");
}

void StraightRoad::addChildren(StructurePiece* startPiece, PieceList& pieces, Random& random) {

	bool hasHouses = false;

	// place left houses
	int depth = random.nextInt(5);

	while (depth < length - 8) {
		VillagePiece* piece = generateHouseNorthernLeft((StartPiece*) startPiece, pieces, random, 0, depth);
		if (piece != nullptr) {
			depth += std::max(piece->getBoundingBox().getXSpan(), piece->getBoundingBox().getZSpan());
			hasHouses = true;
		}
		depth += 2 + random.nextInt(5);
	}

	// place right houses
	depth = random.nextInt(5);

	while (depth < length - 8) {
		VillagePiece* piece = generateHouseNorthernRight((StartPiece*) startPiece, pieces, random, 0, depth);
		if (piece != nullptr) {
			depth += std::max(piece->getBoundingBox().getXSpan(), piece->getBoundingBox().getZSpan());
			hasHouses = true;
		}
		depth += 2 + random.nextInt(5);
	}

	if (hasHouses && random.nextInt(3) > 0) {
		switch (orientation) {
		case Direction::NORTH:
			generateAndAddRoadPiece((StartPiece*) startPiece, pieces, random, boundingBox.x0 - 1, boundingBox.y0, boundingBox.z0, Direction::WEST, getGenDepth());
			break;
		case Direction::SOUTH:
			generateAndAddRoadPiece((StartPiece*) startPiece, pieces, random, boundingBox.x0 - 1, boundingBox.y0, boundingBox.z1 - 2, Direction::WEST, getGenDepth());
			break;
		case Direction::EAST:
			generateAndAddRoadPiece((StartPiece*) startPiece, pieces, random, boundingBox.x1 - 2, boundingBox.y0, boundingBox.z0 - 1, Direction::NORTH, getGenDepth());
			break;
		case Direction::WEST:
			generateAndAddRoadPiece((StartPiece*) startPiece, pieces, random, boundingBox.x0, boundingBox.y0, boundingBox.z0 - 1, Direction::NORTH, getGenDepth());
			break;
		}
	}
	if (hasHouses && random.nextInt(3) > 0) {
		switch (orientation) {
		case Direction::NORTH:
			generateAndAddRoadPiece((StartPiece*) startPiece, pieces, random, boundingBox.x1 + 1, boundingBox.y0, boundingBox.z0, Direction::EAST, getGenDepth());
			break;
		case Direction::SOUTH:
			generateAndAddRoadPiece((StartPiece*) startPiece, pieces, random, boundingBox.x1 + 1, boundingBox.y0, boundingBox.z1 - 2, Direction::EAST, getGenDepth());
			break;
		case Direction::EAST:
			generateAndAddRoadPiece((StartPiece*) startPiece, pieces, random, boundingBox.x1 - 2, boundingBox.y0, boundingBox.z1 + 1, Direction::SOUTH, getGenDepth());
			break;
		case Direction::WEST:
			generateAndAddRoadPiece((StartPiece*) startPiece, pieces, random, boundingBox.x0, boundingBox.y0, boundingBox.z1 + 1, Direction::SOUTH, getGenDepth());
			break;
		}
	}

}

BoundingBox StraightRoad::findPieceBox(StartPiece* startPiece, PieceList& pieces, Random& random, int footX, int footY, int footZ, int direction) {

	int length = 7 * (Math::nextInt(random, 3, 5));

	while (length >= 7) {
		BoundingBox box = BoundingBox::orientBox(footX, footY, footZ, 0, 0, 0, width, 3, length, direction);

		if (StructurePiece::findCollisionPiece(pieces, box) == nullptr) {
			return box;
		}
		length -= 7;
	}

	return BoundingBox::getUnknownBox();
}

bool StraightRoad::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	auto block = biomeBlock(Block::mGrassPathBlock->mID);
	auto filler = biomeBlock(Block::mWoodPlanks->mID);

	for (int x = boundingBox.x0; x <= boundingBox.x1; x++) {
		for (int z = boundingBox.z0; z <= boundingBox.z1; z++) {
			if (chunkBB.isInside(x, 64, z)) {
				int y = level->getAboveTopSolidBlock(x, z, true) - 1;
// 				if (level->getMaterial(x, y, z).isLiquid()) {
// 					level->setBlockAndData(x, y, z, filler, Block::UPDATE_NONE);
// 				}
// 				else{
// 					level->setBlockAndData(x, y, z, block, Block::UPDATE_NONE);
// 				}
			}
		}
	}

	return true;
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------
// |||-------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- --------|||
// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

Well::Well()
	: VillagePiece() {

}

Well::Well(StartPiece* startPiece, int genDepth, Random& random, const BoundingBox& stairsBox, int direction)
	: VillagePiece(startPiece, genDepth) {
	orientation = direction;
	boundingBox = stairsBox;

}

Well::Well(StartPiece* startPiece, int genDepth, Random& random, int west, int north) {
	orientation = random.nextInt(4);

	switch (orientation) {
	case Direction::NORTH:
	case Direction::SOUTH:
		boundingBox = BoundingBox(west, 64, north, west + width - 1, 64 + height - 1, north + depth - 1);
		break;
	default:
		boundingBox = BoundingBox(west, 64, north, west + depth - 1, 64 + height - 1, north + width - 1);
		break;
	}

}

bool Well::postProcess(BlockSource* level, Random& random, const BoundingBox& chunkBB) {
	if (heightPosition < 0) {
		heightPosition = getAverageGroundHeight(level, chunkBB);
		if (heightPosition < 0) {
			return true;
		}
		boundingBox.move(0, heightPosition - boundingBox.y1 + 3, 0);
	}

	FullBlock cobbleBlocks = Block::mCobblestone->mID;

	if (isAbandoned && random.nextFloat() > .4f) {
		cobbleBlocks = Block::mMossyCobblestone->mID;
	}

	generateBox(level, chunkBB, 1, 0, 1, 4, height - 3, 4, cobbleBlocks, Block::mStillWater->mID, false);
	placeBlock(level, FullBlock::AIR, 2, height - 3, 2, chunkBB);
	placeBlock(level, FullBlock::AIR, 3, height - 3, 2, chunkBB);
	placeBlock(level, FullBlock::AIR, 2, height - 3, 3, chunkBB);
	placeBlock(level, FullBlock::AIR, 3, height - 3, 3, chunkBB);

	placeBlock(level, Block::mFence->mID, 1, height - 2, 1, chunkBB);
	placeBlock(level, Block::mFence->mID, 1, height - 1, 1, chunkBB);
	placeBlock(level, Block::mFence->mID, 4, height - 2, 1, chunkBB);
	placeBlock(level, Block::mFence->mID, 4, height - 1, 1, chunkBB);
	placeBlock(level, Block::mFence->mID, 1, height - 2, 4, chunkBB);
	placeBlock(level, Block::mFence->mID, 1, height - 1, 4, chunkBB);
	placeBlock(level, Block::mFence->mID, 4, height - 2, 4, chunkBB);
	placeBlock(level, Block::mFence->mID, 4, height - 1, 4, chunkBB);

	Random cobbleRandom(random.getSeed());

	generateBoxPieces(level, chunkBB, 1, height, 1, 4, height, 4, false, Block::mCobblestone->mID, cobbleRandom, mCobbleSelector);

	for (int z = 0; z <= 5; z++) {
		for (int x = 0; x <= 5; x++) {
			// only do the frame
			if (x != 0 && x != 5 && z != 0 && z != 5) {
				continue;
			}
			placeBlock(level, Block::mGrassPathBlock->mID, x, height - 4, z, chunkBB);
			generateAirColumnUp(level, x, height - 3, z, chunkBB);
		}
	}

	return true;
}
