/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/AnvilBlock.h"
#include "world/level/material/Material.h"
#include "world/Facing.h"
#include "world/entity/Mob.h"
#include "world/item/ItemInstance.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/entity/player/Player.h"
#include "world/level/block/LevelEvent.h"
#include "locale/I18n.h"

AnvilBlock::AnvilBlock(const std::string& nameId, int id, const Material& material) :
	HeavyBlock(nameId, id, Material::getMaterial(MaterialType::Metal)){

	setSolid(false);
	setPushesOutItems(true);

	mProperties = BlockProperty::Heavy;
	mRenderLayer = BlockRenderLayer::RENDERLAYER_ALPHATEST;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

bool AnvilBlock::isCubeShaped(void) {
	return false;
}

bool AnvilBlock::isSolidRender(void) const {
	return false;
}

bool AnvilBlock::shouldRenderFace(BlockSource& region, const BlockPos& pos, FacingID face, const AABB& shape) const {
	return true;
}

Color AnvilBlock::getDustColor(DataID data) const {
	return Color::NIL;
}

void AnvilBlock::animateTick(BlockSource& region, const BlockPos& pos, Random& random) const {
	//Base class uses this to animate particles. Made this virtual to avoid it, since Anvils are falling blocks that
	// don't emit particles.
}

std::string AnvilBlock::buildDescriptionName(DataID data) const {
	AnvilType blockData = static_cast<AnvilType>(data);
	std::string name;

	switch (blockData) {
	case AnvilType::SlightlyDamaged:
		name = "tile.anvil.slightlyDamaged";
		break;

	case AnvilType::VeryDamaged:
		name = "tile.anvil.veryDamaged";
		break;

	case AnvilType::Undamaged:
	default:
		name = "tile.anvil.intact";
		break;
	}

	return I18n::get(name + ".name");
}

int AnvilBlock::getPlacementDataValue(Entity& by, const BlockPos& pos, FacingID face, const Vec3& clickPos, int itemValue) const {
	int dir = (Math::floor(by.getRotation().y * 4 / (360) + 0.5f)) & 3;
	dir = (dir + 1) % 4;//The anvil should be rotated 90 degrees from the player's facing.

	DataID data = 0;
	AnvilType auxVal = static_cast<AnvilType>(itemValue);
	Block::mAnvil->getBlockState(BlockState::Damage).set(data, auxVal);

	switch (dir) {
	case 0:
		Block::mAnvil->getBlockState(BlockState::Direction).set(data, Direction::SOUTH);
		break;
	case 1:
		Block::mAnvil->getBlockState(BlockState::Direction).set(data, Direction::WEST);
		break;
	case 2:
		Block::mAnvil->getBlockState(BlockState::Direction).set(data, Direction::NORTH);
		break;
	case 3:
		Block::mAnvil->getBlockState(BlockState::Direction).set(data, Direction::EAST);
		break;
	default:
		break;
	}

	return data;
}

DataID AnvilBlock::getSpawnResourcesAuxValue(DataID data) const {
	return Block::mAnvil->getBlockState(BlockState::Damage).get<DataID>(data);
}

ItemInstance AnvilBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	int auxVal = enum_cast(Block::mAnvil->getBlockState(BlockState::Damage).get<AnvilType>(blockData));
	return ItemInstance(Block::mAnvil, 1, auxVal);
}

int AnvilBlock::getVariant(int data) const {
	AnvilType damage = Block::mAnvil->getBlockState(BlockState::Damage).get<AnvilType>(data);
	switch (damage) {
	case AnvilType::Undamaged:
		return 0;
	case AnvilType::SlightlyDamaged:
		return 1;
	case AnvilType::VeryDamaged:
		return 2;
	default:
		return 3;
	}
}

int AnvilBlock::getDataForSide(int data) {
	// Only the lower two bits from data (the rotation), plus the higher two bits which ensures the side variation is rendered (3, check getVariant)
	return Block::mAnvil->getBlockState(BlockState::Direction).get<int>(data);
}

int AnvilBlock::getContainerSize() const {
	return 3;
}

bool AnvilBlock::isCraftingBlock() const {
	return true;
}

bool AnvilBlock::canBeSilkTouched() const {
	return false;
}

bool AnvilBlock::falling() const {
	return true;//HeavyBlocks use falling to determine if it does player damage.
}

void AnvilBlock::onLand(BlockSource& region, const BlockPos& pos) const {
// 	region.getLevel().broadcastDimensionEvent(region, LevelEvent::SoundAnvilLand, pos, 0);//On landing, a sound event
																						// needs to be played!
}

bool AnvilBlock::use(Player& player, const BlockPos& pos) const {
// 	if (!player.getLevel().isClientSide() ) {
// 		player.openAnvil(getContainerSize(), pos);
// 	}

	return true;
}
