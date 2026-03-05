/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

// #include "network/packet/EventPacket.h"
#include "world/level/block/ItemFrameBlock.h"
// #include "world/level/block/entity/ItemFrameBlockEntity.h"
#include "world/entity/player/Player.h"
// #include "world/entity/player/PlayerInventoryProxy.h"
#include "world/item/ItemInstance.h"
// #include "world/item/MapItem.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/level/block/LevelEvent.h"
#include "world/Facing.h"
#include "world/level/material/Material.h"
// #include "world/level/saveddata/maps/MapItemSavedData.h"
#include "world/redstone/Redstone.h"
// #include "network/packet/ReplaceItemInSlotPacket.h"
// #include "network/PacketSender.h"

static const int MAP_GRID_ACHIEVEMENT_SIZE = 3;
// Total detection grid size (for a 3x3 valid grid, we need to detect a total of 5x5 around said map)
static const int MAP_GRID_ACHIEVEMENT_DETECTION_SIZE = 2 * (3 - 1) + 1;

ItemFrameBlock::ItemFrameBlock(const std::string& nameId, int id)
	: EntityBlock(nameId, id, Material::getMaterial(MaterialType::Decoration)) {
	setSolid(false);

	mBlockEntityType = BlockEntityType::ItemFrame;
	mProperties = BlockProperty::BreakOnPush;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

int ItemFrameBlock::getDirection(int data) {
	return Block::mItemFrame->getBlockState(BlockState::Direction).get<int>((DataID)data);
}

int ItemFrameBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	DataID dir = 0;

	switch (face) {
	case Facing::NORTH:
		getBlockState(BlockState::Direction).set(dir, DIR_NORTH);
		break;
	case Facing::SOUTH:
		getBlockState(BlockState::Direction).set(dir, DIR_SOUTH);
		break;
	case Facing::WEST:
		getBlockState(BlockState::Direction).set(dir, DIR_WEST);
		break;
	case Facing::EAST:
		getBlockState(BlockState::Direction).set(dir, DIR_EAST);
		break;
	default:
		break;
	}

	return dir;
}

bool ItemFrameBlock::mayPlace(BlockSource& region, const BlockPos& pos, FacingID face) const {
	if (face == Facing::UP || face == Facing::DOWN) {
		return false;
	}
	
	BlockPos neighbor(pos.x + Facing::STEP_X[Facing::OPPOSITE_FACING[face]], pos.y, pos.z + Facing::STEP_Z[Facing::OPPOSITE_FACING[face]]);
	const Material& material = region.getMaterial(neighbor);
	if (!material.isSolid()) {
		return false;
	}

	return true;
}

void ItemFrameBlock::getShape(int face, AABB& bufferValue) const {
	float min = 2 / 16.0f;
	float max = 14 / 16.0f;
	float depth = 1 / 16.0f;

	bufferValue.set(0, min, min, depth, max, max);
	if (face == 1) {
		bufferValue.set(1 - depth, min, min, 1, max, max);
	}
	else if (face == 2) {
		bufferValue.set(min, min, 0, max, max, depth);
	}
	else if (face == 3) {
		bufferValue.set(min, min, 1 - depth, max, max, 1);
	}

}

const AABB& ItemFrameBlock::getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping) const {
	getShape(region.getData(pos), bufferAABB);
	return bufferAABB;
}

const AABB& ItemFrameBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	if (isClipping) {
		int face = getBlockState(BlockState::Direction).get<int>(region.getData(pos));
		getShape(face, bufferValue);
		return bufferValue.move(Vec3(pos));
	}

	return AABB::EMPTY;
}

bool ItemFrameBlock::isInteractiveBlock() const {
	return true;
}

void ItemFrameBlock::spawnResources(BlockSource& region, const BlockPos& pos, int data, float odds, int bonusLootLevel) const {
// 	if (auto itemFrame = _getItemFrame(region, pos)) {
// 		itemFrame->dropFramedItem(region);
// 	}

	EntityBlock::spawnResources(region, pos, data, 1.0f, bonusLootLevel);
}

int ItemFrameBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Item::mItemFrame->getId();
}

ItemInstance ItemFrameBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
// 	if (auto itemFrame = _getItemFrame(region, pos)) {
// 		auto& framedItem = itemFrame->getFramedItem();
// 		if (!framedItem.isNull()) {
// 			return ItemInstance(framedItem);
// 		}
// 	}
	
	return ItemInstance(Item::mItemFrame, 1, blockData);
}

bool ItemFrameBlock::canBeSilkTouched() const {
	return false;
}

bool ItemFrameBlock::playerWillDestroy(Player& player, const BlockPos& pos, int data) const {
// 	auto& region = player.getRegion();
// 	if (region.checkBlockPermissions(player, pos)) {
// 		if (auto itemFrame = _getItemFrame(region, pos)) {
// 			if (!itemFrame->getFramedItem().isNull()) {
// 				itemFrame->dropFramedItem(region, player.isSurvival());
// 				region.getLevel().broadcastLevelEvent(LevelEvent::SoundItemFrameRemoveItem, pos, 0);
// 				return true;
// 			}
// 		}
// 	}

	return EntityBlock::playerWillDestroy(player, pos, data);
}

bool ItemFrameBlock::attack(Player* player, const BlockPos& pos) const {
// 	auto& region = player->getRegion();
// 	if (auto itemFrame = _getItemFrame(region, pos)) { 
// 		if (!itemFrame->getFramedItem().isNull()) {
// 			itemFrame->dropFramedItem(region);
// 			region.getLevel().broadcastLevelEvent(LevelEvent::SoundItemFrameRemoveItem, pos, 0);
// 			return false;
// 		}
// 	}

	return EntityBlock::attack(player, pos);
}

void ItemFrameBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	region.getLevel().broadcastLevelEvent(LevelEvent::SoundItemFramePlace, pos, 0);

	EntityBlock::onPlace(region, pos);
}

void ItemFrameBlock::onRemove(BlockSource& region, const BlockPos& pos) const {
	EntityBlock::onRemove(region, pos);
}

void ItemFrameBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	int dir = getBlockState(BlockState::Direction).get<int>(region.getData(pos));
	dir = Facing::OPPOSITE_FACING[_getFacingID(dir)];
	BlockPos neighbor(pos.x + Facing::STEP_X[dir], pos.y, pos.z + Facing::STEP_Z[dir]);
	const Material& material = region.getMaterial(neighbor);
	if (!material.isSolid()) {
		spawnResources(region, pos, region.getData(pos), 1);
		region.setBlock(pos, BlockID::AIR, Block::UPDATE_ALL);
		region.getLevel().broadcastLevelEvent(LevelEvent::SoundItemFrameBreak, pos, 0);
		region.getLevel().broadcastLevelEvent(LevelEvent::ParticlesDestroyBlock, pos, getId(), 0);
	}
}

bool ItemFrameBlock::hasComparatorSignal() const {
	return true;
}

int ItemFrameBlock::getComparatorSignal(BlockSource& region, const BlockPos& pos, FacingID dir, int data) const {
// 	if (_getFacingID(getBlockState(BlockState::Direction).get<int>(data)) == dir) {
// 		if (auto itemFrame = _getItemFrame(region, pos)) {
// 			ItemInstance& framedItem = itemFrame->getFramedItem();
// 			if (!framedItem.isNull()) {
// 				return Math::min(itemFrame->getRotation() + 1, Redstone::SIGNAL_MAX);
// 			}
// 		}
// 	}

	return EntityBlock::getComparatorSignal(region, pos, dir, data);
}

bool ItemFrameBlock::use(Player& player, const BlockPos& pos) const {
// 	BlockSource& region = player.getRegion();
// 	
// 	if (!region.getLevel().isClientSide() && region.checkBlockPermissions(player, pos)) {
// 		if (auto itemFrame = _getItemFrame(region, pos)) {
// 			ItemInstance& framedItem = itemFrame->getFramedItem();
// 			// Place Item.
// 			if (framedItem.isNull()) {
// 				ItemInstance* heldItem = player.getSelectedItem();
// 				if (heldItem != nullptr) {
// 					itemFrame->setItem(region, *heldItem);
// 					region.getLevel().broadcastLevelEvent(LevelEvent::SoundItemFrameAddItem, pos, 0);
// 
// 					// Check for "Map Room" achievement
// 					_checkAchievements(player, pos);
// 
// 					if (!player.mAbilities.mInstabuild) {
// 						heldItem->remove(1);
// 						if (!player.isLocalPlayer()) {
// 							ReplaceItemInSlotPacket packet(*heldItem, player.getSupplies().getSelectedSlot().mSlot);
// 							player.getRegion().getLevel().getPacketSender()->send(player.mOwner, packet);
// 						}
// 					}
// 				}
// 			} else {
// 				itemFrame->rotateFramedItem();
// 				region.getLevel().broadcastLevelEvent(LevelEvent::SoundItemFrameRotateItem, pos, 0);
// 			}
// 		}
// 	}
	
	return true;
}

int ItemFrameBlock::_getFacingID(int data) const {
	int dir = getBlockState(BlockState::Direction).get<int>(data);
	switch (dir) {
	case DIR_NORTH:
		return Facing::NORTH;
	case DIR_SOUTH:
		return Facing::SOUTH;
	case DIR_WEST:
		return Facing::WEST;
	case DIR_EAST:
		return Facing::EAST;
	default:
		return 0;
	}
}

// Helper function for iterating over the ItemFrame map grid
static void _forEachFrame(const BlockPos& center, int facingDir, std::function<void(const BlockPos&)> callback) {
	BlockPos blockOffset = BlockPos(0, 1, 0);
	switch (facingDir) {
		case Facing::NORTH:
			blockOffset.x = -1;
			break;
		case Facing::EAST:
			blockOffset.z = -1;
			break;
		case Facing::SOUTH:
			blockOffset.x = 1;
			break;
		case Facing::WEST:
			blockOffset.z = 1;
			break;
	}

	for (int yOffset = (MAP_GRID_ACHIEVEMENT_DETECTION_SIZE / 2); yOffset >= -(MAP_GRID_ACHIEVEMENT_DETECTION_SIZE / 2); --yOffset) {
		for (int neighborOffset = -(MAP_GRID_ACHIEVEMENT_DETECTION_SIZE / 2); neighborOffset <= (MAP_GRID_ACHIEVEMENT_DETECTION_SIZE / 2); ++neighborOffset) {
			BlockPos neighborPos = center.offset(blockOffset.x * neighborOffset, blockOffset.y * yOffset, blockOffset.z * neighborOffset);
			callback(neighborPos);
		}
	}
}

int ItemFrameBlock::_addMapCollection(std::vector<MapItemSavedData*>& detectionGrid, const BlockPos& centerPos, BlockSource& region) const {
	int dir = _getFacingID(getBlockState(BlockState::Direction).get<int>(region.getData(centerPos)));
	int mapCount = 0;
	int addedMaps = 0;
// 	_forEachFrame(centerPos, dir, [&](const BlockPos& currentPos) {
// 		ItemFrameBlockEntity* neightborItemFrame = _getItemFrame(region, currentPos);
// 
// 		if (neightborItemFrame != nullptr) {
// 			ItemInstance neighborItem = neightborItemFrame->getFramedItem();
// 			if (neighborItem.getItem() == Item::mFilledMap) {
// 				MapItemSavedData& neighborData = MapItem::getSavedData(MapItem::getMapId(neighborItem), region.getLevel());
// 				detectionGrid[mapCount] = &neighborData;
// 				++addedMaps;
// 			}
// 		}
// 
// 		++mapCount;
// 	});

	return addedMaps;
}

void ItemFrameBlock::_checkAchievements(Player& player, const BlockPos& currentPos) const {
// 	ItemFrameBlockEntity* currentEntity = _getItemFrame(player.getRegion(), currentPos);
// 
// 	if (currentEntity != nullptr) {
// 		ItemInstance currentItem = currentEntity->getFramedItem();
// 
// 		if (currentItem.getItem() == Item::mFilledMap) {
// 			std::vector<MapItemSavedData*> detectionGrid;
// 			detectionGrid.resize(MAP_GRID_ACHIEVEMENT_DETECTION_SIZE * MAP_GRID_ACHIEVEMENT_DETECTION_SIZE);
// 			int addedMaps = _addMapCollection(detectionGrid, currentPos, player.getRegion());
// 
// 			if (addedMaps < (MAP_GRID_ACHIEVEMENT_SIZE * MAP_GRID_ACHIEVEMENT_SIZE)) {
// 				return;
// 			}
// 
// 			for (auto&& map : detectionGrid) {
// 				if (map != nullptr && map->isFullyExplored() == false) {
// 					map = nullptr;
// 				}
// 			}
// 
// 			// Check our worst-case grid for valid NxN configurations
// 			for (int y = 0; y < MAP_GRID_ACHIEVEMENT_SIZE; ++y) {
// 				int detectedAlongX = 0;
// 				for (int x = 0; x < MAP_GRID_ACHIEVEMENT_DETECTION_SIZE; ++x) {
// 					int detectedAlongY = 0;
// 					MapItemSavedData* currentMapData = detectionGrid[x + y * MAP_GRID_ACHIEVEMENT_DETECTION_SIZE];
// 					if (currentMapData != nullptr) {
// 
// 						// Check eastern neighbor if we're not the last column
// 						if ((x < (MAP_GRID_ACHIEVEMENT_DETECTION_SIZE - 1)) && (detectedAlongX < MAP_GRID_ACHIEVEMENT_SIZE - 1)) {
// 							MapItemSavedData* easternNeighbor = detectionGrid[x + 1 + y * MAP_GRID_ACHIEVEMENT_DETECTION_SIZE];
// 							if (easternNeighbor == nullptr || currentMapData->isAdjacent(*easternNeighbor, Facing::EAST) == false) {
// 								detectedAlongX = 0;
// 								continue;
// 							}
// 						}
// 
// 						// Check our current column to see if its valid in our grid
// 						for (int columnY = 0; columnY < MAP_GRID_ACHIEVEMENT_SIZE - 1; ++columnY) {
// 							MapItemSavedData* columnMapData = detectionGrid[x + (y + columnY) * MAP_GRID_ACHIEVEMENT_DETECTION_SIZE];
// 
// 							// Check southern neighbor
// 							MapItemSavedData* southernNeighbor = detectionGrid[x + (y + columnY + 1) * MAP_GRID_ACHIEVEMENT_DETECTION_SIZE];
// 							if (southernNeighbor == nullptr || columnMapData->isAdjacent(*southernNeighbor, Facing::SOUTH) == false) {
// 								detectedAlongX = 0;
// 								break;
// 							}
// 							++detectedAlongY;
// 						}
// 
// 						// We've found a valid map column
// 						if (detectedAlongY >= MAP_GRID_ACHIEVEMENT_SIZE - 1) {
// 							++detectedAlongX;
// 						}
// 					}
// 					else {
// 						detectedAlongX = 0;
// 					}
// 
// 					// If we've found enough valid columns in our grid, aware achievement
// 					if (detectedAlongX >= MAP_GRID_ACHIEVEMENT_SIZE) {
// 						// Award "Map Room" Achievement
// 						EventPacket packet(&player, MinecraftEventing::AchievementIds::MapRoom);
// 						player.sendEventPacket(packet);
// 
// 						return;
// 					}
// 				}
// 
// 			}
// 
// 		}
// 	}
}

ItemFrameBlockEntity* ItemFrameBlock::_getItemFrame(BlockSource& region, const BlockPos& pos) const {
// 	auto blockEntity = region.getBlockEntity(pos);
// 
// 	if (blockEntity != nullptr && blockEntity->isType(BlockEntityType::ItemFrame)) {
// 		return static_cast<ItemFrameBlockEntity*>(blockEntity);
// 	}

	return nullptr;
}
