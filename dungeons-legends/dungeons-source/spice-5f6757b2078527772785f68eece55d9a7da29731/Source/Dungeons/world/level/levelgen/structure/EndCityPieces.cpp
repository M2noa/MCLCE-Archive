#include "Dungeons.h"

#include "world/level/levelgen/structure/EndCityPieces.h"

// #include "platform/AppPlatform.h"
// #include "world/entity/monster/Shulker.h"
#include "world/level/block/entity/BlockEntity.h"
// #include "world/level/block/entity/ChestBlockEntity.h"
// #include "world/level/block/entity/ItemFrameBlockEntity.h"
// #include "world/level/block/entity/StructureBlockEntity.h"
#include "world/level/block/ItemFrameBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
// #include "world/level/Spawner.h"
#include "world/level/levelgen/structure/StructureManager.h"
#include "world/level/levelgen/structure/StructureTemplate.h"
// #include "world/entity/EntityFactory.h"

#include "util/LootTableUtil.h"

StructureSettings EndCityPieces::mOverwriteSettings;
StructureSettings EndCityPieces::mInsertSettings;

void EndCityPieces::init() {
	mOverwriteSettings.setIgnoreEntities(true);
	mInsertSettings.setIgnoreEntities(true);
	mInsertSettings.setIgnoreBlock(Block::mAir);
}

void EndCityPieces::startHouseTower(StructureManager& structureManager, const BlockPos& origin, Rotation rotation, PieceList& pieces, Random& random) {
	// Section Generators
	Generators generators;
	generators.emplace_back(std::make_unique<TowerGenerator>());
	generators.emplace_back(std::make_unique<FatTowerGenerator>());
	generators.emplace_back(std::make_unique<TowerBridgeGenerator>());
	generators.emplace_back(std::make_unique<HouseTowerGenerator>());

	// Base of the End City
	EndCityPiece* lastPiece = _addHelper(pieces, std::make_unique<EndCityPiece>(structureManager, "base_floor", origin, rotation, true));
	lastPiece = _addHelper(pieces, _createPiece(structureManager, lastPiece, BlockPos(-1, 0, -1), "second_floor", rotation, false));
	lastPiece = _addHelper(pieces, _createPiece(structureManager, lastPiece, BlockPos(-1, 4, -1), "third_floor", rotation, false));
	lastPiece = _addHelper(pieces, _createPiece(structureManager, lastPiece, BlockPos(-1, 8, -1), "third_roof", rotation, true));

	// Recursively add towers, bridges, houses, and (if lucky) a ship
	_recursiveChildren(structureManager, generators, SectionTower, 1, lastPiece, BlockPos::ZERO, pieces, random);
}

Unique<EndCityPieces::EndCityPiece> EndCityPieces::_createPiece(StructureManager& structureManager, EndCityPiece* parent, const BlockPos& offset, const std::string& templateName, Rotation rotation, bool overwrite) {
	auto child = std::make_unique<EndCityPiece>(structureManager, templateName, parent->getTemplatePosition(), rotation, overwrite);

	BlockPos origin = parent->getTemplate()->calculateConnectedPosition(parent->getSettings(), offset, child->getSettings(), BlockPos::ZERO);
	child->moveBoundingBox(origin.x, origin.y, origin.z);

	return child;
}

EndCityPieces::EndCityPiece* EndCityPieces::_addHelper(PieceList& pieces, Unique<EndCityPiece> piece) {
	pieces.emplace_back(std::move(piece));
	return static_cast<EndCityPiece*>(pieces.back().get());
}

bool EndCityPieces::_recursiveChildren(StructureManager& structureManager, const Generators& generators, SectionType type, int newGenDepth, EndCityPiece* parent, const BlockPos& offset, PieceList& pieces, Random& random) {
	if (newGenDepth > MAX_GEN_DEPTH) {
		return false;
	}

	PieceList childPieces;
	if (generators[type]->generate(structureManager, generators, newGenDepth, parent, offset, childPieces, random)) {
		// If any child piece collides with parent pieces, do not add to final city
		bool collision = false;
		int childTag = random.nextInt();

		for (auto& child : childPieces) {
			child->setGenDepth(childTag);

			StructurePiece* collisionPiece = StructurePiece::findCollisionPiece(pieces, child->getBoundingBox());
			if (collisionPiece != nullptr && collisionPiece->getGenDepth() != parent->getGenDepth()) {
				collision = true;
				break;
			}
		}

		if (!collision) {
			for (auto& child : childPieces) {
				pieces.emplace_back(std::move(child));
			}

			return true;
		}
	}

	return false;
}

/********************************************************************************************************/
/*                                           TOWER GENERATOR                                            */
/********************************************************************************************************/

bool EndCityPieces::TowerGenerator::generate(StructureManager& structureManager, const Generators& generators, int genDepth, EndCityPiece* parent, const BlockPos& offset, PieceList& pieces, Random& random) {
	Rotation rotation = parent->getSettings().getRotation();

	// Generate tower base
	const int offsetX = 3 + random.nextInt(2);
	const int offsetZ = 3 + random.nextInt(2);

	EndCityPiece* lastPiece = parent;
	lastPiece = _addHelper(pieces, _createPiece(structureManager, lastPiece, BlockPos(offsetX, -3, offsetZ), "tower_base", rotation, true));
	lastPiece = _addHelper(pieces, _createPiece(structureManager, lastPiece, BlockPos(0, 7, 0), "tower_piece", rotation, true));

	EndCityPiece* bridgePiece = random.nextInt(3) == 0 ? lastPiece : nullptr;

	int towerHeight = 1 + random.nextInt(3);
	for (int i = 0; i < towerHeight; ++i) {
		lastPiece = _addHelper(pieces, _createPiece(structureManager, lastPiece, BlockPos(0, 4, 0), "tower_piece", rotation, true));
		if (i < towerHeight - 1 && random.nextBoolean()) {
			bridgePiece = lastPiece;
		}
	}

	if (bridgePiece != nullptr) {
		// If randomly decided to build a bridge, build it
		const static std::vector<std::pair<Rotation, BlockPos>> towerBridges = {
			std::make_pair(Rotation::NONE, BlockPos(1, -1, 0)),
			std::make_pair(Rotation::ROTATE_90,   BlockPos(6, -1, 1)),
			std::make_pair(Rotation::ROTATE_270,  BlockPos(0, -1, 5)),
			std::make_pair(Rotation::ROTATE_180,  BlockPos(5, -1, 6))
		};

		// Randomly add up to 4 bridges
		for (auto& bridge : towerBridges) {
			if (random.nextBoolean()) {
				EndCityPiece* bridgeStart = _addHelper(pieces, _createPiece(structureManager, bridgePiece, bridge.second, "bridge_end", RotationUtil::getRotated(rotation, bridge.first), true));
				_recursiveChildren(structureManager, generators, SectionBridge, genDepth + 1, bridgeStart, BlockPos::ZERO, pieces, random);
			}
		}

		// Finish tower
		lastPiece = _addHelper(pieces, _createPiece(structureManager, lastPiece, BlockPos(-1, 4, -1), "tower_top", rotation, true));
	}
	else {
		// If no bridges...
		if (genDepth == MAX_GEN_DEPTH - 1) {
			// If at max depth, end tower.
			_addHelper(pieces, _createPiece(structureManager, lastPiece, BlockPos(-1, 4, -1), "tower_top", rotation, true));
		}
		else {
			// Otherwise, make it a fat tower.
			return _recursiveChildren(structureManager, generators, SectionFatTower, genDepth + 1, lastPiece, BlockPos::ZERO, pieces, random);
		}
	}

	return true;
}

/********************************************************************************************************/
/*                                         FAT TOWER GENERATOR                                          */
/********************************************************************************************************/

bool EndCityPieces::FatTowerGenerator::generate(StructureManager& structureManager, const Generators& generators, int genDepth, EndCityPiece* parent, const BlockPos& offset, PieceList& pieces, Random& random) {
	const static std::vector<std::pair<Rotation, BlockPos>> fatTowerBridges = {
		std::make_pair(Rotation::NONE, BlockPos(4,  -1, 0)),
		std::make_pair(Rotation::ROTATE_90,   BlockPos(12, -1, 4)),
		std::make_pair(Rotation::ROTATE_270,  BlockPos(0, -1, 8)),
		std::make_pair(Rotation::ROTATE_180,  BlockPos(8, -1, 12))
	};

	// Create fat tower base
	Rotation rotation = parent->getSettings().getRotation();
	EndCityPiece* lastPiece = _addHelper(pieces, _createPiece(structureManager, parent, BlockPos(-3, 4, -3), "fat_tower_base", rotation, true));
	lastPiece = _addHelper(pieces, _createPiece(structureManager, lastPiece, BlockPos(0, 4, 0), "fat_tower_middle", rotation, true));

	// Add up to 2 levels to the fat tower
	for (int i = 0; i < 2; ++i) {
		if (random.nextInt(3) == 0) {
			break;
		}
		lastPiece = _addHelper(pieces, _createPiece(structureManager, lastPiece, BlockPos(0, 8, 0), "fat_tower_middle", rotation, true));

		// Randomly add up to 4 bridges
		for (auto& bridge : fatTowerBridges) {
			if (random.nextBoolean()) {
				EndCityPiece* bridgeStart = _addHelper(pieces, _createPiece(structureManager, lastPiece, bridge.second, "bridge_end", RotationUtil::getRotated(rotation, bridge.first), true));
				_recursiveChildren(structureManager, generators, SectionBridge, genDepth + 1, bridgeStart, BlockPos::ZERO, pieces, random);
			}
		}
	}

	// Finish fat tower
	lastPiece = _addHelper(pieces, _createPiece(structureManager, lastPiece, BlockPos(-2, 8, -2), "fat_tower_top", rotation, true));
	return true;
}

/********************************************************************************************************/
/*                                        TOWER BRIDGE GENERATOR                                        */
/********************************************************************************************************/

EndCityPieces::TowerBridgeGenerator::TowerBridgeGenerator()
	: mShipCreated(false) {
}

bool EndCityPieces::TowerBridgeGenerator::generate(StructureManager& structureManager, const Generators& generators, int genDepth, EndCityPiece* parent, const BlockPos& offset, PieceList& pieces, Random& random) {
	Rotation rotation = parent->getSettings().getRotation();
	int bridgeLength = random.nextInt(4) + 1;

	// First bridge piece
	EndCityPiece* lastPiece = _addHelper(pieces, _createPiece(structureManager, parent, BlockPos(0, 0, -4), "bridge_piece", rotation, true));
	lastPiece->setGenDepth(-1);

	// Generate bridge pieces, each of a random type
	int nextY = 0;
	for (int i = 0; i < bridgeLength; ++i) {
		if (random.nextBoolean()) {
			lastPiece = _addHelper(pieces, _createPiece(structureManager, lastPiece, BlockPos(0, nextY, -4), "bridge_piece", rotation, true));
			nextY = 0;
		}
		else {
			if (random.nextBoolean()) {
				lastPiece = _addHelper(pieces, _createPiece(structureManager, lastPiece, BlockPos(0, nextY, -4), "bridge_steep_stairs", rotation, true));
			}
			else {
				lastPiece = _addHelper(pieces, _createPiece(structureManager, lastPiece, BlockPos(0, nextY, -8), "bridge_gentle_stairs", rotation, true));
			}
			nextY = 4;
		}
	}

	if (mShipCreated || random.nextInt(2 + MAX_GEN_DEPTH - genDepth) != 0) {
		// If there is already a ship or random allow us to, add a house at the last bridge piece built.
		if (!_recursiveChildren(structureManager, generators, SectionHouse, genDepth + 1, lastPiece, BlockPos(-3, nextY + 1, -11), pieces, random)) {
			return false;
		}
	}
	else {
		// If no ship has been created and a house isn't built, add an End Ship at the last bridge piece built.
		const int offsetX = -8 + random.nextInt(8);
		const int offsetZ = -70 + random.nextInt(10);
		_addHelper(pieces, _createPiece(structureManager, lastPiece, BlockPos(offsetX, nextY, offsetZ), "ship", rotation, true));
		mShipCreated = true;
	}

	// Finish up the bridge
	lastPiece = _addHelper(pieces, _createPiece(structureManager, lastPiece, BlockPos(4, nextY, 0), "bridge_end", RotationUtil::getRotated(rotation, Rotation::ROTATE_180), true));
	lastPiece->setGenDepth(-1);

	return true;
}

/********************************************************************************************************/
/*                                         HOUSE TOWER GENERATOR                                        */
/********************************************************************************************************/

bool EndCityPieces::HouseTowerGenerator::generate(StructureManager& structureManager, const Generators& generators, int genDepth, EndCityPiece* parent, const BlockPos& offset, PieceList& pieces, Random& random) {
	if (genDepth > MAX_GEN_DEPTH) {
		return false;
	}

	Rotation rotation = parent->getSettings().getRotation();
	EndCityPiece* lastPiece = _addHelper(pieces, _createPiece(structureManager, parent, offset, "base_floor", rotation, true));

	int numFloors = random.nextInt(3);
	if (numFloors == 0) {
		lastPiece = _addHelper(pieces, _createPiece(structureManager, lastPiece, BlockPos(-1, 4, -1), "base_roof", rotation, true));
	}
	else if (numFloors == 1) {
		lastPiece = _addHelper(pieces, _createPiece(structureManager, lastPiece, BlockPos(-1, 0, -1), "second_floor_2", rotation, false));
		lastPiece = _addHelper(pieces, _createPiece(structureManager, lastPiece, BlockPos(-1, 8, -1), "second_roof", rotation, false));

		_recursiveChildren(structureManager, generators, SectionTower, genDepth + 1, lastPiece, BlockPos::ZERO, pieces, random);
	}
	else if (numFloors == 2) {
		lastPiece = _addHelper(pieces, _createPiece(structureManager, lastPiece, BlockPos(-1, 0, -1), "second_floor_2", rotation, false));
		lastPiece = _addHelper(pieces, _createPiece(structureManager, lastPiece, BlockPos(-1, 4, -1), "third_floor_c", rotation, false));
		lastPiece = _addHelper(pieces, _createPiece(structureManager, lastPiece, BlockPos(-1, 8, -1), "third_roof", rotation, true));

		_recursiveChildren(structureManager, generators, SectionTower, genDepth + 1, lastPiece, BlockPos::ZERO, pieces, random);
	}

	return true;
}

/********************************************************************************************************/
/*                                            END CITY PIECE                                            */
/********************************************************************************************************/

EndCityPieces::EndCityPiece::EndCityPiece(StructureManager& structureManager)
	: TemplateStructurePiece(0)
	, mStructureManager(structureManager)
{
}

EndCityPieces::EndCityPiece::EndCityPiece(StructureManager& structureManager, std::string templateName, const BlockPos& origin, Rotation rotation, bool overwrite)
	: TemplateStructurePiece(0)
	, mStructureManager(structureManager)
	, mTemplateName(templateName)
	, mRotation(rotation)
	, mOverwrite(overwrite) {

	_loadAndSetup(origin);
}

void EndCityPieces::EndCityPiece::addAdditionalSaveData(CompoundTag& tag) const {
	TemplateStructurePiece::addAdditionalSaveData(tag);
	tag.putString("Template", mTemplateName);
	tag.putInt("Rotation", enum_cast(mRotation));
	tag.putBoolean("Overwrite", mOverwrite);
}

void EndCityPieces::EndCityPiece::readAdditionalSaveData(const CompoundTag& tag) {
	TemplateStructurePiece::readAdditionalSaveData(tag);

	mTemplateName = tag.getString("Template");
	mRotation = static_cast<Rotation>(tag.getInt("Rotation"));
	mOverwrite = tag.getBoolean("Overwrite");

	_loadAndSetup(mTemplatePosition);
}

void EndCityPieces::EndCityPiece::postProcessMobsAt(BlockSource* region, Random& random, const BoundingBox& chunkBB) {
	Level& level = region->getLevel();

	std::vector<BlockPos> notAdded;

// 	for (auto&& position : mMobPositions) {
// 		if (chunkBB.isInside(position)) {
// 			Mob* mob = level.getSpawner().spawnMob(*region, EntityTypeToString(EntityType::Shulker, EntityTypeNamespaceRules::ReturnWithNamespace), nullptr, position);
// 			if (mob != nullptr) {
// 				Shulker* shulker = static_cast<Shulker*>(mob);
// 				shulker->setAttachPos(position);
// 				shulker->setPersistent();
// 			}
// 		}
// 		else {
// 			notAdded.push_back(position);
// 		}
// 	}

	mMobPositions.swap(notAdded);
}

void EndCityPieces::EndCityPiece::_handleDataMarker(const std::string& markerId, const BlockPos& position, BlockSource& region, Random& random, const BoundingBox& chunkBB) {
	if (Util::startsWith(markerId, "Chest")) {
// 		BlockPos chestPosition = position.below();
// 
// 		if (chunkBB.isInside(chestPosition)) {
// 			BlockEntity* blockEntity = region.getBlockEntity(chestPosition);
// 			if (blockEntity != nullptr && blockEntity->isType(BlockEntityType::Chest)) {
// 				ChestBlockEntity* chest = static_cast<ChestBlockEntity*>(blockEntity);
// 				Util::LootTableUtils::fillChest(region.getLevel(), *chest, random, "loot_tables/chests/end_city_treasure.json");
// 			}
// 		}
	}
	else if (Util::startsWith(markerId, "Sentry")) {
		if (chunkBB.isInside(position)) {
			mMobPositions.push_back(position);
		}
	}
	else if (Util::startsWith(markerId, "Elytra")) {
		int dir = ItemFrameBlock::DIR_SOUTH;
		switch (mRotation) {
		case Rotation::ROTATE_90:
			dir = ItemFrameBlock::DIR_WEST;
			break;
		case Rotation::ROTATE_180:
			dir = ItemFrameBlock::DIR_NORTH;
			break;
		case Rotation::ROTATE_270:
			dir = ItemFrameBlock::DIR_EAST;
			break;
		default:
			break;
		}

// 		if (chunkBB.isInside(position) && region.setBlockAndData(position, Block::mItemFrame->mID, dir, Block::UPDATE_ALL)) {
// 			BlockEntity* blockEntity = region.getBlockEntity(position);
// 			if (blockEntity != nullptr && blockEntity->isType(BlockEntityType::ItemFrame)) {
// 				ItemFrameBlockEntity* itemFrame = static_cast<ItemFrameBlockEntity*>(blockEntity);
// 				ItemInstance elytra(Item::mElytra, 1);
// 				itemFrame->setItem(region, elytra);
// 			}
// 		}
	}
}

void EndCityPieces::EndCityPiece::_loadAndSetup(const BlockPos& position) {
	StructureTemplate& structTemplate = mStructureManager.getOrCreate("endcity/" + mTemplateName);
	StructureSettings settings = (mOverwrite) ? EndCityPieces::mOverwriteSettings : EndCityPieces::mInsertSettings;
	settings.setRotation(mRotation);

	_setup(structTemplate, settings, position);
}
