#include "Dungeons.h"

#include "world/level/block/EndGatewayBlock.h"
#include "world/level/material/Material.h"
#include "util/Random.h"
#include "world/level/BlockSource.h"
#include "world/level/Level.h"
// #include "world/entity/ParticleType.h"
#include "world/entity/Entity.h"
#include "world/level/block/entity/BlockEntity.h"
// #include "world/level/block/entity/EndGatewayBlockEntity.h"

EndGatewayBlock::EndGatewayBlock(const std::string& nameId, int id)
	: EntityBlock(nameId, id, Material::getMaterial(MaterialType::Portal)) {
	setSolid(false);
	setPushesOutItems(true);
	mProperties = BlockProperty::Portal | BlockProperty::Immovable;
	mBlockEntityType = BlockEntityType::EndGateway;
	setLightEmission(1.0f);
	setMapColor(Color::BLACK);
	mRenderLayer = BlockRenderLayer::RENDERLAYER_ENDPORTAL;
}

int EndGatewayBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 0;
}

bool EndGatewayBlock::addCollisionShapes(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes, Entity* entity) const {
	if (entity) {
		const BlockEntity* blockEntity = region.getBlockEntity(pos);
		if (!blockEntity || !(blockEntity->getType() == BlockEntityType::EndGateway)) {
			return false;
		}
// 		const EndGatewayBlockEntity* gateway = static_cast<const EndGatewayBlockEntity*>(blockEntity);
// 		if (!gateway->canTeleport(entity, region)) {
// 			return Block::addCollisionShapes(region, pos, intersectTestBox, inoutBoxes, entity);
// 		}
	}
	return false;
}

void EndGatewayBlock::animateTick(BlockSource& region, const BlockPos& pos, Random& random) const {
	const BlockEntity* blockEntity = region.getBlockEntity(pos);
	if (!blockEntity || !(blockEntity->getType() == BlockEntityType::EndGateway)) {
		return;
	}
	// TODO: rherlitz
// 	const int particleCount = ((EndGatewayBlockEntity*)blockEntity)->getParticleAmount(region, pos);
// 	for (int i = 0; i < particleCount; i++) {
// 		float x = pos.x + random.nextFloat();
// 		const float y = pos.y + random.nextFloat();
// 		float z = pos.z + random.nextFloat();
// 		float xa = (random.nextFloat() - 0.5f) * 0.5f;
// 		const float ya = (random.nextFloat() - 0.5f) * 0.5f;
// 		float za = (random.nextFloat() - 0.5f) * 0.5f;
// 	
// 		const int flip = random.nextInt(2) * 2 - 1;
// 		if (random.nextBoolean()) {
// 			z = pos.z + 0.5f + 0.25f * flip;
// 			za = random.nextFloat() * 2 * flip;
// 		}
// 		else {
// 			x = pos.x + 0.5f + 0.25f * flip;
// 			xa = random.nextFloat() * 2 * flip;
// 		}
// 	
// 		region.getLevel().addParticle(ParticleType::Portal, Vec3(x, y, z), Vec3(xa, ya, za));
// 	}
}

bool EndGatewayBlock::shouldRenderFace(BlockSource& region, const BlockPos& pos, FacingID face, const AABB& shape) const {
	const Block& block = region.getBlock(pos.relative(face));
	return !block.isSolid() && block.mID != Block::mEndGateway->mID;
}

bool EndGatewayBlock::isWaterBlocking() const {
	return true;
}

ItemInstance EndGatewayBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	return ItemInstance();
}
