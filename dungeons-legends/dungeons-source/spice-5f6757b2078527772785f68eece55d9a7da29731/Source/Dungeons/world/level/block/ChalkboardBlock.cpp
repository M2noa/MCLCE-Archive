/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/ChalkboardBlock.h"

#include "world/Direction.h"
// #include "world/entity/item/ItemEntity.h"
#include "world/entity/player/Player.h"
#include "world/Facing.h"
#include "world/item/Item.h"
#include "world/item/ItemInstance.h"
#include "world/level/BlockSource.h"
// #include "world/level/block/entity/ChalkboardBlockEntity.h"
#include "world/level/material/Material.h"
#include "world/level/Level.h"
// #include "world/level/Spawner.h"

ChalkboardBlock::ChalkboardBlock(const std::string& nameId, int id)
	: EntityBlock(nameId, id, Material::getMaterial(MaterialType::Wood)) {

	float r = 4 / 16.0f;
	float h = 16 / 16.0f;

	setVisualShape(Vec3(0.5f - r, 0, 0.5f - r), Vec3(0.5f + r, h, 0.5f + r));
	setSolid(false);

	mBlockEntityType = BlockEntityType::Chalkboard;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

bool ChalkboardBlock::playerWillDestroy(Player& player, const BlockPos& pos, int data) const {
	BlockSource& region = player.getRegion();

	ChalkboardBlockEntity* blockEntity = getBlockEntity(region, pos);
	// Spawn by hand to deal with variable chalkboard size and only dropping for base. Can't use spawnresources or getresourcevalue because blockEntity with size info is gone by then
	if(!player.isCreative() && blockEntity) {
		auto& level = player.getLevel();
		if (!level.isClientSide()) {
			Random random;
			const float xo = random.nextFloat() * 0.8f + 0.1f;
			const float yo = random.nextFloat() * 0.8f + 0.1f;
			const float zo = random.nextFloat() * 0.8f + 0.1f;

// 			auto itemEntity = level.getSpawner().spawnItem(player.getRegion(), ItemInstance(Item::mChalkboard, 1, enum_cast(blockEntity->getChalkboardSize())), nullptr, Vec3(pos.x + xo, pos.y + yo, pos.z + zo), 10);
// 			static const float pow = 0.05f;
// 			itemEntity->mPosDelta.x = random.nextGaussian() * pow;
// 			itemEntity->mPosDelta.y = random.nextGaussian() * pow + 0.2f;
// 			itemEntity->mPosDelta.z = random.nextGaussian() * pow;
		}
	}
	return EntityBlock::playerWillDestroy(player, pos, data);
}

const AABB& ChalkboardBlock::getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping) const {
	ChalkboardBlockEntity* blockEntity = getBlockEntity(region, pos);
	DEBUG_ASSERT(blockEntity, "Chalkboard block with no BlockEntity");
// 	if (blockEntity && blockEntity->isBaseChalkboard()) {
// 		int dir = getBlockState(BlockState::Direction).get<int>(region.getData(pos));
// 		_getShape(blockEntity->getChalkboardSize(), blockEntity->isOnGround(), dir, bufferAABB);
// 	}

	return bufferAABB;
}

void ChalkboardBlock::_getShape(ChalkboardSize size, bool isOnGround, int dir, AABB& bufferValue) const {
	float heightBottom = 4.f / 16.f;
	static const float heightTop = 1.f;
	static const float widthLeft = 0 / 16.0f;
	static const float widthRight = 16 / 16.0f;

	static const float depth = 2 / 16.0f;

	bufferValue.set(0, 0, 0, 1, 1, 1);

	if (!isOnGround) {
		// Make non-1x1 full block size for convenience
// 		if (size != ChalkboardSize::OnebyOne) {
// 			heightBottom = 0.f;
// 		}

		if (dir == Direction::NORTH) {
			bufferValue.set(widthLeft, heightBottom, 1 - depth, widthRight, heightTop, 1);
		}
		if (dir == Direction::SOUTH) {
			bufferValue.set(widthLeft, heightBottom, 0, widthRight, heightTop, depth);
		}
		if (dir == Direction::WEST) {
			bufferValue.set(1 - depth, heightBottom, widthLeft, 1, heightTop, widthRight);
		}
		if (dir == Direction::EAST) {
			bufferValue.set(0, heightBottom, widthLeft, depth, heightTop, widthRight);
		}
	}
}

const AABB& ChalkboardBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
// 	ChalkboardBlockEntity* blockEntity = getBlockEntity(region, pos);
// 	if (blockEntity != nullptr) {
// 		if (isClipping) {
// 			const bool isOnGround = blockEntity->isOnGround();
// 
// 			if (isOnGround) {
// 				bufferValue.set(Vec3::ZERO, Vec3::ONE);
// 			}
// 			else {
// 				int dir = getBlockState(BlockState::Direction).get<int>(region.getData(pos));
// 				_getShape(blockEntity->getChalkboardSize(), isOnGround, dir, bufferValue);
// 			}
// 
// 			return bufferValue.move(Vec3(pos));
// 		}
// 	}

	return AABB::EMPTY;
}

bool ChalkboardBlock::checkIsPathable(Entity& entity, const BlockPos& lastPathPos, const BlockPos& pathPos) const {
	return true;
}

ItemInstance ChalkboardBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int) const {
	//TODO: rherlitz
	int data = 0;
// 	int data = enum_cast(ChalkboardSize::OnebyOne);
// 	ChalkboardBlockEntity* blockEntity = getBlockEntity(region, pos);
// 	if (blockEntity != nullptr) {
// 		data = enum_cast(blockEntity->getChalkboardSize());
// 	}

	return ItemInstance(Item::mChalkboard, 1, data);
}

void ChalkboardBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	EntityBlock::neighborChanged(region, pos, neighborPos);
	ChalkboardBlockEntity* currentChalkboard = getBlockEntity(region, pos);

	bool remove = false;

	if (currentChalkboard == nullptr) {
		// Remove if our block entity is gone
		remove = true;
	}
// 	else {
// 		// Remove if our base was destroyed
// 		ChalkboardBlockEntity* baseEntity = currentChalkboard->getBaseChalkboard(region);
// 		if (baseEntity == nullptr) {
// 			remove = true;
// 		}
// 		else {
// 			// Remove if any of our siblings were destroyed
// 			const std::vector<BlockPos> siblings = ChalkboardBlockEntity::calculateAllBlocks(baseEntity->getPosition(), baseEntity->getChalkboardSize(), baseEntity->getRotation(region));
// 			for (const BlockPos& siblingPos : siblings) {
// 				if (getBlockEntity(region, siblingPos) == nullptr) {
// 					remove = true;
// 					break;
// 				}
// 			}
// 		}
// 	}

	if (remove) {
		region.setBlock(pos, BlockID::AIR, Block::UPDATE_ALL);
	}
}

int ChalkboardBlock::getVariant(int data) const {
	//the default getVariant returns data which in our case stores facing and is wrong
	return 0;
}

ChalkboardBlockEntity* ChalkboardBlock::getBlockEntity(BlockSource& region, const BlockPos& pos) {
// 	BlockEntity* currentBlockEntity = region.getBlockEntity(pos);
// 	if ((currentBlockEntity != nullptr) && (currentBlockEntity->getType() == BlockEntityType::Chalkboard)) {
// 		return static_cast<ChalkboardBlockEntity*>(currentBlockEntity);
// 	}
	return nullptr;
}

void ChalkboardBlock::spawnResources(BlockSource& region, const BlockPos& pos, int data, float odds, int bonusLootLevel) const {
	 // Do nothing, PlayerWillDestroy spawns this 
}

bool ChalkboardBlock::use(Player& player, const BlockPos& pos) const {
// 	BlockSource& region = player.getRegion();

// 	ChalkboardBlockEntity* blockEntity = getBlockEntity(region, pos);
// 	if (blockEntity != nullptr) {
// 		ChalkboardBlockEntity* baseEntity = blockEntity->getBaseChalkboard(region);
// 		if (baseEntity != nullptr) {
// 			player.openChalkboard(*baseEntity);
// 		}
// 	}

	return true;
}

bool ChalkboardBlock::canBeSilkTouched() const {
	return false;
}
