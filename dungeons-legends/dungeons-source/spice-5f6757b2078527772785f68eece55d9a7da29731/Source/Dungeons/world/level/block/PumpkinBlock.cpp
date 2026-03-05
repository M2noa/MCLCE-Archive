/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/PumpkinBlock.h"
#include "world/level/material/Material.h"
#include "world/Facing.h"
#include "world/level/Level.h"
#include "world/level/BlockSource.h"
// #include "world/entity/animal/IronGolem.h"
// #include "world/entity/animal/SnowGolem.h"
// #include "Events/MinecraftEventing.h"
// #include "world/level/Spawner.h"
// #include "network/packet/EventPacket.h"
#include "world/Container.h"
#include "world/level/block/LevelEvent.h"
#include "world/level/BlockPatternBuilder.h"
// #include "world/entity/EntityFactory.h"

PumpkinBlock::PumpkinBlock(const std::string& nameId, int id, bool lit)
	: Block(nameId, id, Material::getMaterial(MaterialType::Vegetable))
	, mLit(lit) {
	mProperties = BlockProperty::CubeShaped | BlockProperty::BreakOnPush;
}

int PumpkinBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	int dir = Math::floor(by.getRotation().y * 4 / (360) + 2.5f) & 3;

	DataID data = 0;
	getBlockState(BlockState::Direction).set(data, dir);
	
	return data;
}

ItemInstance PumpkinBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(mLit ? Block::mLitPumpkin : Block::mPumpkin);
}

bool PumpkinBlock::dispense(BlockSource& region, Container& container, int slot, const Vec3& pos, FacingID face) const {
	bool success = false;

	if (region.getBlock(pos).isType(Block::mAir)) {
		if (_canDispense(region, pos, face)) {
			if (region.setBlockAndData(BlockPos(pos), Block::mPumpkin->mID, 0, Block::UPDATE_ALL)) {
					success = true;
					container.removeItem(slot, 1);
			}
		}
	}

// 	if (success) {
// 		region.getLevel().broadcastLevelEvent(LevelEvent::SoundClick, pos, 1000);
// 	} else {
// 		region.getLevel().broadcastLevelEvent(LevelEvent::SoundClickFail, Vec3(pos), 1200);
// 	}

	return true;
}

bool PumpkinBlock::_golemPumpkinTester(BlockSource& region, const BlockPos& pos, BlockID id) const {
	return id == Block::mPumpkin->mID || id == Block::mLitPumpkin->mID;
}

bool PumpkinBlock::_canSpawnGolem(BlockSource& region, const BlockPos& pos) const {
// 	Unique<BlockPatternBuilder> snowGolemBuilder = BlockPatternBuilder::start(region);
// 	snowGolemBuilder->aisle(3, "^", "#", "#")
// 		.define('^', Block::mPumpkin->mID, std::bind(&PumpkinBlock::_golemPumpkinTester, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
// 		.define('#', Block::mSnow->mID)
// 		.build();
// 
// 	if (snowGolemBuilder->isReadyForMatch()) {
// 		BuildMatch BuildMatch = snowGolemBuilder->match(pos, 0, 0);
// 		if (BuildMatch.mMatched) {
// 			Vec3 position = BuildMatch.mObjectPos + Vec3(0.5f, 0.05f, 0.5f);
// 			if (Facing::getStepX(BuildMatch.mUp) != 0 || Facing::getStepZ(BuildMatch.mUp) != 0 || Facing::getStepY(BuildMatch.mUp) > 0) {
// 				position.y += 2.0f;
// 			}
// 
// 			Unique<Entity> golem = EntityFactory::createSpawnedEntity(EntityType::SnowGolem, nullptr, position, Vec2::ZERO);
// 			if (golem) {
// 				snowGolemBuilder->replaceBlocks(' ', BuildMatch);
// 				region.getLevel().addEntity(region, std::move(golem));
// 				return true;
// 			}
// 		}
// 	}
// 
// 	Unique<BlockPatternBuilder> ironGolemBuilder = BlockPatternBuilder::start(region);
// 	ironGolemBuilder->aisle(3, "~^~", "###", "~#~")
// 		.define('~', Block::mAir->mID)
// 		.define('^', Block::mPumpkin->mID, std::bind(&PumpkinBlock::_golemPumpkinTester, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
// 		.define('#', Block::mIronBlock->mID)
// 		.build();
// 
// 	if (ironGolemBuilder->isReadyForMatch()) {
// 		BuildMatch BuildMatch = ironGolemBuilder->match(pos);
// 		if (BuildMatch.mMatched) {
// 			Vec3 position = BuildMatch.mObjectPos;
// 			if (Facing::getStepX(BuildMatch.mUp) != 0 || Facing::getStepZ(BuildMatch.mUp) != 0 || Facing::getStepY(BuildMatch.mUp) > 0) {
// 				position.y += 2.0f;
// 			}			
// 			Unique<Entity> golem = EntityFactory::createSpawnedEntity(EntityDefinitionIdentifier("minecraft", EntityTypeToString(EntityType::IronGolem), "minecraft:from_player"), nullptr, position, Vec2::ZERO);
// 			if (golem) {
// 				IronGolem *ironGolem = static_cast<IronGolem*>(golem.get());
// 				if (Facing::getStepZ(BuildMatch.mForward)) {
// 					ironGolem->mYBodyRot += 90.0f;
// 				}
// 				ironGolemBuilder->replaceBlocks(' ', BuildMatch);
// 				region.getLevel().addEntity(region, std::move(golem));
// 				return true;
// 			}
// 		}
// 	}

	return false;
}

int PumpkinBlock::getVariant(int data) const {
	return mLit ? 1 : 0;
}

FacingID PumpkinBlock::getMappedFace(FacingID face, int data) const {
	int dir = getBlockState(BlockState::Direction).get<int>(data);

	switch (face) {
	case Facing::UP:
		return Facing::UP;
	case Facing::DOWN:
		return Facing::UP;
	case Facing::NORTH:
		if (dir == DIR_NORTH) {
			return Facing::SOUTH;
		}
		break;
	case Facing::EAST:
		if (dir == DIR_EAST) {
			return Facing::SOUTH;
		}
		break;
	case Facing::SOUTH:
		if (dir == DIR_SOUTH) {
			return Facing::SOUTH;
		}
		break;
	case Facing::WEST:
		if (dir == DIR_WEST) {
			return Facing::SOUTH;
		}
		break;
	}

	return Facing::NORTH;
}

void PumpkinBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	Block::onPlace(region, pos);

	_canSpawnGolem(region, pos);
}

bool PumpkinBlock::mayPlace(BlockSource& region, const BlockPos& pos) const {
	BlockID t = region.getBlockID(pos);
	return (t == 0 || Block::mBlocks[t]->getMaterial().isReplaceable()) && region.canProvideSupport(pos.below(), Facing::UP, BlockSupportType::Any); // TODO: It should not care about canProvideSupport!
}

bool PumpkinBlock::_canDispense(BlockSource& region, const Vec3& pos, FacingID face) const {
	Unique<BlockPatternBuilder> snowGolemBuilder = BlockPatternBuilder::start(region);
	snowGolemBuilder->aisle(3, "~", "#", "#")
		.define('~', Block::mAir->mID)
		.define('#', Block::mSnow->mID)
		.build();

	if (snowGolemBuilder->isReadyForMatch()) {
		BuildMatch buildMatch = snowGolemBuilder->match(pos, 0, 0);
		if (buildMatch.mMatched) {
			return true;
		}
	}

	Unique<BlockPatternBuilder> ironGolemBuilder = BlockPatternBuilder::start(region);
	ironGolemBuilder->aisle(3, "~ ~", "###", "~#~")
		.define('~', Block::mAir->mID)
		.define('#', Block::mIronBlock->mID)
		.build();

	if (ironGolemBuilder->isReadyForMatch()) {
		BuildMatch buildMatch = ironGolemBuilder->match(pos);
		if (buildMatch.mMatched) {
			return true;
		}
	}

	return false;
}