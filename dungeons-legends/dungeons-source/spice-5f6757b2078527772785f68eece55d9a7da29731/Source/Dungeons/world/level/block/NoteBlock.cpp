/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "NoteBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/BlockPos.h"
#include "world/level/Level.h"
#include "world/level/dimension/Dimension.h"
// #include "world/level/block/entity/MusicBlockEntity.h"
#include "world/level/material/Material.h"
// #include "world/redstone/circuit/CircuitSystem.h"
// #include "world/redstone/circuit/components/ConsumerComponent.h"
#include "world/entity/player/Player.h"

static const std::vector<std::string> INSTRUMENTS = { "harp", "bd", "snare", "hat", "bassattack" };

NoteBlock::NoteBlock(const std::string& nameId, int id)
	: EntityBlock(nameId, id, Material::getMaterial(MaterialType::Wood)) {
	mBlockEntityType = BlockEntityType::Music;
}

void NoteBlock::onLoaded(BlockSource& region, const BlockPos& pos) const {

// 	if (!region.getLevel().isClientSide()) {
// 		auto &circuit = region.getDimension().getCircuitSystem();
// 		auto component = circuit.create<ConsumerComponent>(pos, &region, Facing::DOWN);
// 		if (component != nullptr) {
// 			component->setPropagatePower(true);
// 		}
// 	}
}

void NoteBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	EntityBlock::onPlace(region, pos);
	onLoaded(region, pos);
}

void NoteBlock::onRedstoneUpdate(BlockSource& region, const BlockPos& pos, int strength, bool isFirstTime) const {
	
	//don't change on the first time (init)
	if (isFirstTime == true) {
		return;
	}

// 	BlockEntity* blockEntity = region.getBlockEntity(pos);
// 	if (blockEntity != nullptr && blockEntity->getType() == BlockEntityType::Music) {
// 		MusicBlockEntity *musicBlockEntity = static_cast<MusicBlockEntity*>(blockEntity);
// 		bool shouldBeOn = strength > 0;
// 
// 		if (musicBlockEntity->mOn != shouldBeOn) {
// 			if (shouldBeOn) {
// 				musicBlockEntity->playNote(region, pos);
// 			}
// 			musicBlockEntity->mOn = shouldBeOn;
// 		}
// 	}
}

bool NoteBlock::use(Player& player, const BlockPos& pos) const {
	if (player.getRegion().getLevel().isClientSide()) {
		return true;
	}

// 	BlockEntity* blockEntity = player.getRegion().getBlockEntity(pos);
// 	if (blockEntity != nullptr && blockEntity->getType() == BlockEntityType::Music) {
// 		MusicBlockEntity *musicBlockEntity = static_cast<MusicBlockEntity*>(blockEntity);
// 
// 		musicBlockEntity->tune();
// 		musicBlockEntity->playNote(player.getRegion(), pos);
// 	}

	return true;
}

bool NoteBlock::attack(Player* player, const BlockPos& pos) const {
	if (player->getRegion().getLevel().isClientSide()) {
		return true;
	}

// 	BlockEntity* blockEntity = player->getRegion().getBlockEntity(pos);
// 	if (blockEntity != nullptr && blockEntity->getType() == BlockEntityType::Music) {
// 		MusicBlockEntity *musicBlockEntity = static_cast<MusicBlockEntity*>(blockEntity);
// 		musicBlockEntity->playNote(player->getRegion(), pos);
// 	}

	return true;
}

std::string NoteBlock::getSoundName(int index) const {
	if (index < 0 || index >= (int)INSTRUMENTS.size()) {
		index = 0;
	}

	return "note." + INSTRUMENTS[index];
}

float NoteBlock::getPitch(int note) const {
	return  powf(2, (note - 12) / 12.0f);
}
void NoteBlock::triggerEvent(BlockSource& region, const BlockPos& pos, int instrument, int note) const {

// 	Vec3 vPos(pos);
// 	Level& level = region.getLevel();
// 	level.broadcastSoundEvent(region, LevelSoundEvent::Note, vPos, static_cast<BlockID>(instrument), static_cast<EntityType>(note));
// 	level.addParticle(ParticleType::Note, vPos + Vec3(0.5f, 1.2f, 0.5f), Vec3(note / 24.0f, 0, 0));
}
