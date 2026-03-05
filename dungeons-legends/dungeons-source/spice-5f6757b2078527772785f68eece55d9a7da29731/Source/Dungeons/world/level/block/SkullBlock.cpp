/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/SkullBlock.h"
// #include "world/level/block/entity/SkullBlockEntity.h"
#include "world/level/material/Material.h"
#include "world/Facing.h"
#include "world/level/Level.h"
#include "world/entity/player/Player.h"
#include "world/level/BlockSource.h"
#include "world/item/Item.h"
#include "world/level/BlockPatternBuilder.h"
// #include "world/entity/boss/WitherBoss.h"
// #include "world/level/Spawner.h"
// #include "world/redstone/circuit/CircuitSystem.h"
// #include "world/redstone/circuit/components/ConsumerComponent.h"
#include "world/level/dimension/Dimension.h"

SkullBlock::SkullBlock(const std::string& nameId, int id)
	: EntityBlock(nameId, id, Material::getMaterial(MaterialType::Decoration)) {
	setVisualShape(Vec3(4.0f / 16.0f, 0, 4.0f / 16.0f), Vec3(12.0f / 16.0f, .5f, 12.0f / 16.0f));

	setSolid(false);
	mBlockEntityType = BlockEntityType::Skull;
	mProperties = BlockProperty::BreakOnPush;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

bool SkullBlock::mayPlace(BlockSource& region, const BlockPos& pos) const {
	return EntityBlock::mayPlace(region, pos);
}

std::string SkullBlock::getTypeDescriptionId(int data) {
	static const std::array<std::string, 6> SKULL_NAMES = {
		"skeleton", "wither", "zombie", "char", "creeper", "dragon"
	};

	int type = Block::mSkull->getBlockState(BlockState::MappedType).get<int>(data);

	if(type < 0 || type >= (int)SKULL_NAMES.size()) {
		type = 0;
	}

	return SKULL_NAMES[type];
}

ItemInstance SkullBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	SkullBlockEntity* entity = (SkullBlockEntity*)region.getBlockEntity(pos);
// 	return ItemInstance(Item::mSkull->getId(), 1, enum_cast(entity->getSkullType()));
	return ItemInstance(Item::mSkull->getId(), 1, 0);
}

const AABB& SkullBlock::getVisualShape(BlockSource& region, const BlockPos& pos, AABB& bufferAABB, bool isClipping) const {
	int dir = getBlockState(BlockState::FacingDirection).get<int>(region.getData(pos));

	switch (dir) {
	default:
	case Facing::UP:
		bufferAABB.set(4.0f / 16.0f, 0, 4.0f / 16.0f, 12.0f / 16.0f, .5f, 12.0f / 16.0f);
		break;
	case Facing::NORTH:
		bufferAABB.set(4.0f / 16.0f, 4.0f / 16.0f, .5f, 12.0f / 16.0f, 12.0f / 16.0f, 1);
		break;
	case Facing::SOUTH:
		bufferAABB.set(4.0f / 16.0f, 4.0f / 16.0f, 0, 12.0f / 16.0f, 12.0f / 16.0f, .5f);
		break;
	case Facing::WEST:
		bufferAABB.set(.5f, 4.0f / 16.0f, 4.0f / 16.0f, 1, 12.0f / 16.0f, 12.0f / 16.0f);
		break;
	case Facing::EAST:
		bufferAABB.set(0, 4.0f / 16.0f, 4.0f / 16.0f, .5f, 12.0f / 16.0f, 12.0f / 16.0f);
		break;
	}

	return bufferAABB;
}

bool SkullBlock::canBeSilkTouched() const {
	return false;
}

DataID SkullBlock::getSpawnResourcesAuxValue(DataID data) const {
	return data;
}

bool SkullBlock::_witherSkullTester(BlockSource& region, const BlockPos& pos, BlockID id) const {
// 	if (id == Block::mSkull->mID) {
// 		auto skull = static_cast<SkullBlockEntity*>(region.getBlockEntity(pos));
// 		if (skull != nullptr) {
// 			return skull->getSkullType() == SkullBlockEntity::SkullType::WITHER;
// 		}
// 	}

	return false;
}

void SkullBlock::spawnResources(BlockSource& region, const BlockPos& pos, int data, float odds, int bonusLootLevel) const {
	// do nothing, resource is popped by onRemove
	// ... because the block entity is removed prior to spawnResources
}

int SkullBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Item::mSkull->getId();
}

void SkullBlock::onLoaded(BlockSource& region, const BlockPos& pos) const {
	EntityBlock::onLoaded(region, pos);
	_updatedDragonCircuit(region, pos);
}

void SkullBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	EntityBlock::onPlace(region, pos);
	_updatedDragonCircuit(region, pos);
}

bool SkullBlock::playerWillDestroy(Player& player, const BlockPos& pos, int data) const {
	// prevent resource-dropping in creative mode
// 	if (player.mAbilities.mInstabuild) {
// 		BlockSource& region = player.getRegion();
// 		FullBlock t = region.getBlockAndData(pos);
// 		getBlockState(BlockState::NoDropBit).set(t.data, true);
// 		region.setBlockAndData(pos, t, Block::UPDATE_CLIENTS);
// 	}

	return EntityBlock::playerWillDestroy(player, pos, data);
}

void SkullBlock::onRemove(BlockSource& region, const BlockPos& pos) const {
	if (region.getLevel().isClientSide()) {
		return;
	}

	FullBlock t = region.getBlockAndData(pos);
	if(!getBlockState(BlockState::NoDropBit).getBool(t.data)) {
// 		SkullBlockEntity* entity = (SkullBlockEntity*)region.getBlockEntity(pos);
// 		ItemInstance item(Item::mSkull->getId(), 1, enum_cast(entity->getSkullType()));
		ItemInstance item(Item::mSkull->getId(), 1, 0);

		/*@Todo: make owner work
		  if (entity.getSkullType() == SkullBlockEntity.TYPE_CHAR && entity.getOwnerProfile() != null) {
		   item.setTag(new CompoundTag());
		   CompoundTag ownerTag = new CompoundTag();
		   NbtUtils.writeGameProfile(ownerTag, entity.getOwnerProfile());
		   item.getTag().put("SkullOwner", ownerTag);
		   }*/

		popResource(region, pos, item);
	}
// 	region.getDimension().getCircuitSystem().removeComponents(pos);

	EntityBlock::onRemove(region, pos);
}

void SkullBlock::checkMobSpawn(Level& level, BlockSource& region, const BlockPos& pos, SkullBlockEntity& placedSkull) const {
// 	if (placedSkull.getSkullType() != SkullBlockEntity::SkullType::WITHER || pos.y < 2 || level.getDifficulty() == Difficulty::Peaceful || level.isClientSide()) {
// 		return;
// 	}
// 	
// 	Unique<BlockPatternBuilder> witherBuilder = BlockPatternBuilder::start(region);
// 	witherBuilder->aisle(3, "^^^", "###", "~#~")
// 		.define('^', Block::mSkull->mID, std::bind(&SkullBlock::_witherSkullTester, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
// 		.define('#', Block::mSoulSand->mID)
// 		.define('~', Block::mAir->mID)
// 		.build();
// 
// 	if (witherBuilder->isReadyForMatch()) {
// 		BuildMatch buildMatch = witherBuilder->match(pos);
// 		if (buildMatch.mMatched) {
// 			Vec3 position = buildMatch.mObjectPos + Vec3(0.5f, 0.05f, 0.5f);
// 			Mob *mob = region.getLevel().getSpawner().spawnMob(region, EntityType::WitherBoss, nullptr, position);
// 			if (mob != nullptr) {
// 				auto witherBoss = static_cast<WitherBoss*>(mob);
// 				if (Facing::getStepZ(buildMatch.mForward)) {
// 					witherBoss->mYBodyRot += 90.0f;
// 				}
// 
// 				mob->resetAttributes();
// 				witherBuilder->replaceBlocks(' ', buildMatch, BlockID::AIR, true);
// 
// 				//All players within a 100.9 x 100.9 x 103.5 area centered here should get the achievement.
// 				witherBoss->awardSpawnWitherAchievement();
// 
// 				Random& random = level.getRandom();
// 
// 				for (int i = 0; i < 120; i++) {
// 					float offx = random.nextFloat();
// 					float offy = -2 + random.nextFloat() * 3.9f;
// 					float offz = random.nextFloat();
// 					level.addParticle(ParticleType::SnowballPoof, position + Vec3(offx, offy, offz), Vec3::ZERO);
// 				}
// 			}
// 		}
// 	}
}

std::string SkullBlock::buildDescriptionName(DataID data) const {
	// descriptionId is tile.*, must replace with item for localization lookup
	std::string itemId = mDescriptionId;
	itemId.replace(0, 4, "item");

	return I18n::get(itemId + "." + getTypeDescriptionId(data) + ".name");
}

int SkullBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	DataID dir = (Math::floor(by.getRotation().y * 4 / (360) + 2.5f)) & 3;
	bool bNoDrop = getBlockState(BlockState::NoDropBit).getBool(itemValue);

	DataID data = 0;
	getBlockState(BlockState::NoDropBit).set(data, bNoDrop);

	if (dir == 0) {
		getBlockState(BlockState::FacingDirection).set(data, Facing::NORTH);
	} else if (dir == 1) {
		getBlockState(BlockState::FacingDirection).set(data, Facing::EAST);
	} else if (dir == 2) {
		getBlockState(BlockState::FacingDirection).set(data, Facing::SOUTH);
	} else if (dir == 3) {
		getBlockState(BlockState::FacingDirection).set(data, Facing::WEST);
	}

	return data;
}

void SkullBlock::_updatedDragonCircuit(BlockSource& region, const BlockPos& pos) const {
// 	if (region.getLevel().isClientSide()) {
// 		return;
// 	}
// 
// 	auto &circuit = region.getDimension().getCircuitSystem();
// 	auto consumer = circuit.create<ConsumerComponent>(pos, &region, Facing::DOWN);
// 	if (consumer) {
// 		consumer->setAcceptHalfPulse(true);
// 	}
}
