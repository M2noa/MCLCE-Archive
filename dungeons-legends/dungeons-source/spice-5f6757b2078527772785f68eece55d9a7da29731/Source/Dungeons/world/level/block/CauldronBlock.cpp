/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/CauldronBlock.h"
// #include "world/level/block/entity/CauldronBlockEntity.h"
#include "world/level/material/Material.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
#include "world/level/block/LevelEvent.h"
#include "world/entity/Entity.h"
// #include "world/entity/item/ItemEntity.h"
#include "world/entity/player/Player.h"
// #include "world/entity/player/PlayerInventoryProxy.h"
#include "world/Facing.h"
#include "world/item/Item.h"
#include "world/item/ItemInstance.h"
// #include "world/item/ArmorItem.h"
// #include "world/item/HorseArmorItem.h"
// #include "world/item/DyePowderItem.h"
// #include "world/item/PotionItem.h"
// #include "world/entity/ParticleType.h"
// #include "network/packet/EventPacket.h"
// #include "network/PacketSender.h"
// #include "network/packet/ReplaceItemInSlotPacket.h"
// #include "network/packet/ContainerSetContentPacket.h"
// #include "network/packet/AddItemPacket.h"
// #include "network/packet/DropItemPacket.h"

int CauldronBlock::MIN_FILL_LEVEL = 0;
int CauldronBlock::MAX_FILL_LEVEL = 6;
int CauldronBlock::BASE_WATER_PIXEL = 5;
int CauldronBlock::PIXEL_PER_LEVEL = 2;

CauldronBlock::CauldronBlock(const std::string& nameId, int id)
	: EntityBlock(nameId, id, Material::getMaterial(MaterialType::Metal)) {
	mRenderLayer = RENDERLAYER_OPAQUE;
	setSolid(false);
	setTicking(false);
	mProperties = BlockProperty::Unspecified;

	mAnimatedTexture = true;
	mBlockEntityType = BlockEntityType::Cauldron;

	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

bool CauldronBlock::addCollisionShapes(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes, Entity* entity) const {
	addAABBs(region, pos, intersectTestBox, inoutBoxes);
	return true;
}

void CauldronBlock::addAABBs(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes) const {
	AABB shape(Vec3::ZERO, Vec3(1, 5.0f / 16.0f, 1));
	Block::addAABB(shape.move(pos), intersectTestBox, inoutBoxes);
	float thickness = 2.0f / 16.0f;

	shape.set(Vec3::ZERO, Vec3(thickness, 1, 1));
	Block::addAABB(shape.move(pos), intersectTestBox, inoutBoxes);
	shape.set(Vec3::ZERO, Vec3(1, 1, thickness));
	Block::addAABB(shape.move(pos), intersectTestBox, inoutBoxes);
	shape.set(Vec3(1 - thickness, 0, 0), Vec3::ONE);
	Block::addAABB(shape.move(pos), intersectTestBox, inoutBoxes);
	shape.set(Vec3(0, 0, 1 - thickness), Vec3::ONE);
	Block::addAABB(shape.move(pos), intersectTestBox, inoutBoxes);
}

void CauldronBlock::updateShape(BlockSource& region, const BlockPos& pos) {
	setVisualShape(Vec3::ZERO, Vec3::ONE);
}

bool CauldronBlock::canProvideSupport(BlockSource& region, const BlockPos& pos, FacingID face, BlockSupportType type) const {
	return face == Facing::UP && (type == BlockSupportType::Any || type == BlockSupportType::Edge);
}

void CauldronBlock::handleEntityInside(BlockSource& region, const BlockPos& pos, Entity* entity, Vec3& current) const {
	int fillLevel = getBlockState(BlockState::FillLevel).get<int>(region.getData(pos));
	float waterLevel = pos.y + ((float)BASE_WATER_PIXEL + PIXEL_PER_LEVEL * fillLevel) / 16.0f;

	if (!region.getLevel().isClientSide() && entity->isOnFire() && fillLevel > 0 && entity->mBB.min.y <= waterLevel) {
		current.y = 0.1f;
		setWaterLevel(region, pos, fillLevel, fillLevel - 1, entity);
	}
}

bool CauldronBlock::use(Player& player, const BlockPos& pos) const {
	//Looks like cauldrons do not work at all over the network

	BlockSource& region = player.getRegion();
	if (region.getLevel().isClientSide()) {
		return true;
	}

// 	ItemInstance *itemInstance = player.getSelectedItem();
// 
// 	CauldronBlockEntity *blockEntity = static_cast<CauldronBlockEntity*>(region.getBlockEntity(pos));
// 
// 	// should never happen, but just in case....
// 	DEBUG_ASSERT((blockEntity != nullptr), "Attempting to use a Cauldron that doesn't exist!!");
// 	if (blockEntity == nullptr) {
// 		return false;
// 	}
// 
// 	DataID data = region.getData(pos);
// 	int fillLevel = getBlockState(BlockState::FillLevel).get<int>(data);
// 	bool isEmpty = fillLevel == 0;
// 	bool isFull = fillLevel >= MAX_FILL_LEVEL;
// 	bool isPotion = !isEmpty && blockEntity != nullptr && blockEntity->getPotionId() >= 0;
// 	bool isWater = !isEmpty && !isPotion;
// 	bool isCleanWater = isWater && blockEntity != nullptr && !blockEntity->hasCustomColor();
// 
// 	// send CauldronUsed event
// 	short contentsType;
// 	if (blockEntity->hasCustomColor()) {
// 		contentsType = -2;  //  -2 is dye
// 	}
// 	else {
// 		contentsType = blockEntity->getPotionId(); // returns -1 if its water or empty, otherwise the id of the potion type
// 	}
// 
// 	uint32_t contentsColor = blockEntity->getCustomColor().toARGB();
// 
// 	EventPacket cauldronUsedPacket(&player, contentsType, contentsColor, fillLevel);
// 	player.sendEventPacket(cauldronUsedPacket);
// 
// 	if (itemInstance != nullptr) {
// 		const Item* item = itemInstance->getItem();
// 		if (item == Item::mBucket && itemInstance->getAuxValue() == Block::mFlowingWater->mID) {
// 			ItemInstance emptyBucket(Item::mBucket, 1);
// 			if (isWater || isEmpty) {
// 				_useInventory(player, itemInstance, emptyBucket);
// 
// 				blockEntity->setPotionId(-1);
// 				blockEntity->setCustomColor(Color::NIL);
// 				setWaterLevel(region, pos, data, MAX_FILL_LEVEL - 1, &player);	// Force a refresh, in case we were already full.
// 				setWaterLevel(region, pos, data, MAX_FILL_LEVEL, &player);
// 
// 				if (!isFull || !isCleanWater) {
// 					_spawnCauldronEvent(region, pos, LevelEvent::CauldronFillWater);
// 				}
// 				isFull = true;
// 
// 				return true;
// 			}
// 			else if (isPotion) {
// 				_useInventory(player, itemInstance, emptyBucket);
// 				_explodeCauldronContents(region, pos, data);
// 				return true;
// 			}
// 		}
// 		else if (item == Item::mBucket && itemInstance->getAuxValue() == 0) {
// 			if (isFull && isWater) {
// 				ItemInstance waterBucket(Item::mBucket, 1, Block::mFlowingWater->mID);
// 				_useInventory(player, itemInstance, waterBucket);
// 
// 				isFull = false;
// 				isEmpty = true;
// 				isWater = false;
// 				blockEntity->setPotionId(-1);
// 				blockEntity->setCustomColor(Color::NIL);
// 				setWaterLevel(region, pos, data, MIN_FILL_LEVEL, &player);
// 
// 				_spawnCauldronEvent(region, pos, LevelEvent::CauldronTakeWater);
// 				return true;
// 			}
// 		}
// 		else if (item == Item::mGlass_bottle && fillLevel >= 2) {
// 			if (isWater) {
// 				ItemInstance potion(Item::mPotion, 1, 0);
// 				_useInventory(player, itemInstance, potion);
// 				_spawnCauldronEvent(region, pos, LevelEvent::CauldronTakeWater);
// 			}
// 			else if (isPotion) { // Shouldn't be possible to not be true
// 				// Scoop some potion out of the cauldron
// 				ItemInstance potion(Potion::getBasePotion(blockEntity->getPotionType()), 1, blockEntity->getPotionId());
// 				_useInventory(player, itemInstance, potion);
// 				_spawnCauldronEvent(region, pos, LevelEvent::CauldronTakePotion);
// 			}
// 
// 			isEmpty = fillLevel <= 2;
// 			isWater = isWater && !isEmpty;
// 			isCleanWater = isCleanWater && !isEmpty;
// 			isPotion = isPotion && !isEmpty;
// 
// 			if (isEmpty) {
// 				blockEntity->setPotionId(-1);
// 				blockEntity->setCustomColor(Color::NIL);
// 			}
// 			setWaterLevel(region, pos, data, fillLevel - 2, &player);
// 			return true;
// 		}
// 		else if (item == Item::mArrow && itemInstance->getAuxValue() == 0 && !isEmpty && isPotion && blockEntity->getPotionId() >= 5) {
// 			//This is where we create tipped arrows
// 			//Each quarter of the cauldron will give up to 16 arrows
// 			auto tippedAuxId = blockEntity->getPotionId();
// 
// 			int fill = std::max(fillLevel > 2 ? fillLevel - 2 : 1, 0);
// 			int maxConvertCount = fill * 16;
// 			int convertCount = std::min(maxConvertCount, (int)itemInstance->getStackSize());
// 			int fillChange = (convertCount + 15) / 16;
// 			int fillRes = fillLevel - fillChange;
// 
// 			ItemInstance tippedArrows(Item::mArrow, convertCount, tippedAuxId + 1);
// 			_useInventory(player, itemInstance, tippedArrows, convertCount);
// 
// 			setWaterLevel(region, pos, data, fillRes > 2 ? fillRes : 0, &player);
// 
// 			return true;
// 		}
// 		else if (item == Item::mPotion || item == Item::mSplash_potion || item == Item::mLingering_potion) {
// 			short potionId = itemInstance->getAuxValue();
// 			auto potion = static_cast<const PotionItem*>(item);
// 
// 			if (potionId > 0) {
// 				bool matchingPotion = isPotion && (blockEntity->getPotionId() == potionId);
// 				if (isEmpty || (!isFull && matchingPotion)) {
// 					blockEntity->setPotionId(potionId);
// 					blockEntity->setPotionType(potion->getPotionType());
// 
// 					setWaterLevel(region, pos, data, fillLevel + 2, &player);
// 					isPotion = true;
// 					isFull = fillLevel + 2 >= MAX_FILL_LEVEL;
// 
// 					ItemInstance bottle(Item::mGlass_bottle, 1, 0);
// 					_useInventory(player, itemInstance, bottle);
// 					_spawnCauldronEvent(region, pos, LevelEvent::CauldronFillPotion);
// 					return true;
// 				}
// 				else if (!isEmpty && !matchingPotion) {
// 					isFull = false;
// 					isEmpty = true;
// 					isPotion = false;
// 					isWater = false;
// 
// 					ItemInstance bottle(Item::mGlass_bottle, 1, 0);
// 					_useInventory(player, itemInstance, bottle);
// 					_explodeCauldronContents(region, pos, data);
// 					blockEntity->setPotionId(-1);
// 					blockEntity->setCustomColor(Color::NIL);
// 					return true;
// 				}
// 			}
// 			else if (potionId == 0) {	// water bottle
// 				ItemInstance emptyBottle(Item::mGlass_bottle, 1);
// 				if (isWater || isEmpty) {
// 					_useInventory(player, itemInstance, emptyBottle);
// 
// 					isWater = true;
// 					isFull = data + 2 >= MAX_FILL_LEVEL;
// 					blockEntity->setPotionId(-1);
// 					blockEntity->setCustomColor(Color::NIL);
// 					setWaterLevel(region, pos, data, data + 2, &player);
// 
// 					_spawnCauldronEvent(region, pos, LevelEvent::CauldronFillWater);
// 					return true;
// 				}
// 				else if (isPotion) {
// 					_useInventory(player, itemInstance, emptyBottle);
// 					_explodeCauldronContents(region, pos, data);
// 					return true;
// 				}
// 			}
// 		}
// 		else if (!isEmpty && isWater && ItemInstance::isArmorItem(itemInstance)) {
// 			ArmorItem& armor = static_cast<ArmorItem&>(*itemInstance->getItem());
// 			if (armor.mModelIndex == 0) {
// 				if (!isCleanWater) {
// 					if (armor.mModelIndex == 0) {
// 						armor.setColor(*itemInstance, blockEntity->getCustomColor());
// 						setWaterLevel(region, pos, data, fillLevel - 1, &player);
// 						isEmpty = fillLevel <= 1;
// 						_setSelectedItemOrAdd(player, *itemInstance);
// 						_spawnCauldronEvent(region, pos, LevelEvent::CauldronDyeArmor);
// 					}
// 				}
// 				else {
// 					armor.clearColor(*itemInstance);
// 					setWaterLevel(region, pos, data, fillLevel - 1, &player);
// 					isEmpty = fillLevel <= 1;
// 					_setSelectedItemOrAdd(player, *itemInstance);
// 					_spawnCauldronEvent(region, pos, LevelEvent::CauldronCleanArmor);
// 				}
// 
// 				if (isEmpty) {
// 					blockEntity->setPotionId(-1);
// 					blockEntity->setCustomColor(Color::NIL);
// 				}
// 				return true;
// 			}
// 		}
// 		else if (!isEmpty && isWater && ItemInstance::isHorseArmorItem(itemInstance)) {
// 			HorseArmorItem& armor = static_cast<HorseArmorItem&>(*itemInstance->getItem());
// 			if (!isCleanWater) {
// 					armor.setColor(*itemInstance, blockEntity->getCustomColor());
// 					setWaterLevel(region, pos, data, fillLevel - 1, &player);
// 					isEmpty = fillLevel <= 1;
// 					_setSelectedItemOrAdd(player, *itemInstance);
// 					_spawnCauldronEvent(region, pos, LevelEvent::CauldronDyeArmor);
// 			}
// 			else {
// 				armor.clearColor(*itemInstance);
// 				setWaterLevel(region, pos, data, fillLevel - 1, &player);
// 				isEmpty = fillLevel <= 1;
// 				_setSelectedItemOrAdd(player, *itemInstance);
// 				_spawnCauldronEvent(region, pos, LevelEvent::CauldronCleanArmor);
// 			}
// 
// 			if (isEmpty) {
// 				blockEntity->setPotionId(-1);
// 				blockEntity->setCustomColor(Color::NIL);
// 			}
// 			return true;
// 		}
// 		else if (!isEmpty && isWater && itemInstance->getItem() == Item::mDye_powder) {
// 			ItemInstance singleDye(*itemInstance);
// 
// 			if (!player.isInCreativeMode()) {
// 				_decrementStack(player, itemInstance);
// 			}
// 			singleDye.setStackSize(1);
// 			blockEntity->setItem(CauldronBlockEntity::SLOT_DYES, &singleDye);
// 			blockEntity->mixDyes();
// 			// This forces the block to retessellate :(
// 			setWaterLevel(region, pos, data, fillLevel - 1, &player);
// 			setWaterLevel(region, pos, data, fillLevel, &player);
// 			_spawnCauldronEvent(region, pos, LevelEvent::CauldronAddDye);
// 		}
// 	}

	return true;
}

void CauldronBlock::setWaterLevel(BlockSource& region, const BlockPos& pos, DataID data, int waterLevel, Entity* setter) const {
	getBlockState(BlockState::FillLevel).set(data, Math::clamp(waterLevel, MIN_FILL_LEVEL, MAX_FILL_LEVEL));
	region.setBlockAndData(pos, {mID, data}, Block::UPDATE_ALL, setter);
}

int CauldronBlock::getWaterLevel(DataID data) {
	int waterLevel = data;
	return Math::clamp(waterLevel, MIN_FILL_LEVEL, MAX_FILL_LEVEL);
}

void CauldronBlock::handleRain(BlockSource& region, const BlockPos& pos, float downfallAmount) const {
	if (region.getLevel().getRandom().nextFloat() * 0.5f > downfallAmount) {
		return;
	}

	DataID data = region.getData(pos);
	int fillLevel = data;

	// Don't fill up dirty water cauldrons or potion cauldrons from getting rained on.
// 	CauldronBlockEntity *blockEntity = static_cast<CauldronBlockEntity*>(region.getBlockEntity(pos));
// 	if (blockEntity != nullptr && fillLevel > 0) {
// 		if (blockEntity->getPotionId() >= 0 ||
// 			blockEntity->hasCustomColor()) {
// 			return;
// 		}
// 	}

	if (data < MAX_FILL_LEVEL) {
		region.setBlockAndData(pos, FullBlock(mID, data + 1), Block::UPDATE_ALL);
	}
}

int CauldronBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return Item::mCauldron->getId();
}

ItemInstance CauldronBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance(Item::mCauldron, 1);
}

bool CauldronBlock::hasComparatorSignal() const {
	return true;
}

int CauldronBlock::getComparatorSignal(BlockSource& region, const BlockPos& pos, FacingID dir, int data) const {
	int fillLevel = getBlockState(BlockState::FillLevel).get<int>(data);
	return (fillLevel + 1) / 2;
}

ItemInstance CauldronBlock::getSilkTouchItemInstance(DataID data) const {
	return ItemInstance(Item::mCauldron);
}

void CauldronBlock::_useInventory(Player& player, ItemInstance* current, ItemInstance& replaceWith, int useCount) const {
// 	//Dont' remove the players in hand item if we're in instabuild mode
// 	if (!player.mAbilities.mInstabuild) {
// 		current->remove(useCount);
// 		if (current->isEmptyStack()) {
// 			*current = replaceWith;
// 			if (!player.isLocalPlayer()) {
// 				player.sendInventory();
// 			}
// 			return;
// 		}
// 	}
// 
// 	if (player.isLocalPlayer()) {
// 		if (!player.getSupplies().add(replaceWith)) {
// 			player.drop(replaceWith, false);
// 		}
// 	}
// 	else {
// 		player.sendInventory();
// 		AddItemPacket packet(&replaceWith);
// 		player.getRegion().getLevel().getPacketSender()->send(player.mOwner, packet);
// 	}
}

void CauldronBlock::_setSelectedItemOrAdd(Player& player, ItemInstance& newSelected) const {
// 	ItemInstance *currentItem = player.getSelectedItem();
// 	if (currentItem != nullptr) {
// 		*currentItem = newSelected;
// 	}
// 
	//if (!player.isLocalPlayer()) {
	//	ReplaceItemInSlotPacket packet(newSelected, player.getSupplies().getSelectedSlot().mSlot);
	//	player.getRegion().getLevel().getPacketSender()->send(player.mOwner, packet);
	//}
}

void CauldronBlock::_decrementStack(Player& player, ItemInstance* current) const {
	current->remove(1);
// 	if (!player.isLocalPlayer()) {
// 		ReplaceItemInSlotPacket packet(*current, player.getSupplies().getSelectedSlot().mSlot);
// 		player.getRegion().getLevel().getPacketSender()->send(player.mOwner, packet);
// 	}
}

void CauldronBlock::_explodeCauldronContents(BlockSource& region, const BlockPos& pos, DataID data) const {
	setWaterLevel(region, pos, data, MIN_FILL_LEVEL, nullptr);
	_spawnCauldronEvent(region, pos, LevelEvent::CauldronExplode);
}

void CauldronBlock::_spawnCauldronEvent(BlockSource& region, const BlockPos& pos, LevelEvent levelEvent) const {
// 	CauldronBlockEntity* blockEntity = static_cast<CauldronBlockEntity*>(region.getBlockEntity(pos));
// 	int fillLevel = getBlockState(BlockState::FillLevel).get<int>(region.getData(pos));
// 	if (blockEntity != nullptr && blockEntity->getType() == BlockEntityType::Cauldron) {
// 		float height = 6.0f / 16.0f + fillLevel * 2.0f / 16.0f;
// 		Level& level = region.getLevel();
// 		Vec3 eventPos(pos);
// 		eventPos += Vec3(0.5f, height, 0.5f);
// 		level.broadcastLevelEvent(levelEvent, eventPos, blockEntity->getColor().toARGB());
// 	}
}

void CauldronBlock::animateTick(BlockSource& region, const BlockPos& pos, Random& random) const {
// 	CauldronBlockEntity* blockEntity = static_cast<CauldronBlockEntity*>(region.getBlockEntity(pos));
// 	if (blockEntity != nullptr && blockEntity->getType() == BlockEntityType::Cauldron) {
// 		int fillLevel = getBlockState(BlockState::FillLevel).get<int>(region.getData(pos));
// 
// 		if (fillLevel > MIN_FILL_LEVEL && blockEntity != nullptr && blockEntity->getPotionId() >= 0) {
// 			float height = 6.0f / 16.0f + fillLevel * 2.0f / 16.0f;
// 			Color potionColor = blockEntity->getPotionColor();
// 
// 			spawnPotionParticles(region.getLevel(), Vec3(pos.x + 0.5f, pos.y + height, pos.z + 0.5f), random, potionColor.toARGB(), 1);
// 		}
// 	}
}

void CauldronBlock::spawnPotionParticles(Level& level, const Vec3& pos, Random& random, int color, int count) {
// 	for (int i = 0; i < count; ++i) {
// 		float offx = level.getRandom().nextFloat(-0.5f, 0.5f);
// 		float offz = level.getRandom().nextFloat(-0.5f, 0.5f);
// 		level.addParticle(ParticleType::MobSpell, pos + Vec3(offx, 0.0f, offz), Vec3::ZERO, color);
// 	}
}

void CauldronBlock::spawnSplashParticles(Level& level, const Vec3& pos, Random& random, int color, int count) {
// 	auto c = Color::fromARGB(color).brighter(0.3f).clamped().toARGB();
// 	for (int i = 0; i < count; ++i) {
// 		float offx = level.getRandom().nextFloat(-0.25f, 0.25f);
// 		float offz = level.getRandom().nextFloat(-0.25f, 0.25f);
// 		level.addParticle(ParticleType::Bubble, pos + Vec3(offx, -0.25f, offz), Vec3::ZERO, c);
// 	}
}

void CauldronBlock::spawnBubbleParticles(Level& level, const Vec3& pos, Random& random, int color, int count) {
// 	for (int i = 0; i < count; ++i) {
// 		float offx = level.getRandom().nextFloat(-0.35f, 0.35f);
// 		float offz = level.getRandom().nextFloat(-0.35f, 0.35f);
// 
// 		level.addParticle(ParticleType::Bubble, pos + Vec3(offx, -0.15f, offz), Vec3::ZERO);
// 	}
}

int CauldronBlock::getExtraRenderLayers() const {
	return (1 << RENDERLAYER_BLEND);
}
