#include "Dungeons.h"

#include "PistonArmBlock.h"
#include "world/level/block/PistonBlock.h"
// #include "world/level/block/entity/PistonBlockEntity.h"
#include "world/level/BlockSource.h"
#include "world/Facing.h"
#include "world/level/material/Material.h"

PistonArmBlock::PistonArmBlock(const std::string& nameId, int id)
	: Block(nameId, id, Material::getMaterial(MaterialType::Piston))
{
	setSolid(false);

	setVisualShape(Vec3::ZERO, Vec3::ONE);

	mProperties = BlockProperty::Power_NO | BlockProperty::Immovable;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

void PistonArmBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
// 	BlockPos pistonBasePos;
// 	if(!getSecondPart(region, pos, pistonBasePos)) {
// 		// TODO: if cant find base, we should probably remove the head!
// 		DEBUG_ASSERT(false, "piston arm without base!");
// 		return;
// 	}
// 
// 	PistonBlockEntity* pistonArmEntity = static_cast<PistonBlockEntity*>(region.getBlockEntity(pistonBasePos));
// 	if(!pistonArmEntity) {
// 		region.setBlock(pos, Block::mAir->mID, UPDATE_ALL);
// 	}
}

bool PistonArmBlock::getCollisionShape(AABB& outAABB, BlockSource& region, const BlockPos& pos, Entity* entity) const {
	return false;
}

const float size = 16.0f;
const float baseLeft = 5.0f / size;
const float baseRight = 11.0f / size;
const float middle = (size*0.5f) / size;
const float armLeft = 6.0f / size;
const float armRight = 10.0f / size;
const float armOffset = 4.0f / size;

// Order: DOWN, UP, NORTH, SOUTH, WEST, EAST
const AABB gBaseArm[] = {
	AABB(Vec3(baseLeft, middle+armOffset, baseLeft), Vec3(baseRight, 1.0f+armOffset, baseRight)), 
	AABB(Vec3(baseLeft, -armOffset, baseLeft), Vec3(baseRight, middle-armOffset, baseRight)),
	AABB(Vec3(baseLeft, baseLeft, -armOffset), Vec3(baseRight, baseRight, middle-armOffset)),
	AABB(Vec3(baseLeft, baseLeft, middle+armOffset), Vec3(baseRight, baseRight, 1.0f+armOffset)),
	AABB(Vec3(-armOffset, baseLeft, baseLeft), Vec3(middle-armOffset, baseRight, baseRight)),
	AABB(Vec3(middle+armOffset, baseLeft, baseLeft), Vec3(1.0f+armOffset, baseRight, baseRight))
};

const AABB gArm[] = {
	AABB(Vec3(armLeft, armOffset, armLeft), Vec3(armRight, middle+armOffset, armRight)),
	AABB(Vec3(armLeft, middle-armOffset, armLeft), Vec3(armRight, 1.0f-armOffset, armRight)),
	AABB(Vec3(armLeft, armLeft, middle-armOffset), Vec3(armRight, armRight, 1.0f-armOffset)),
	AABB(Vec3(armLeft, armLeft, armOffset), Vec3(armRight, armRight, middle+armOffset)),
	AABB(Vec3(middle-armOffset, armLeft, armLeft), Vec3(1.0f-armOffset, armRight, armRight)),
	AABB(Vec3(armOffset, armLeft, armLeft), Vec3(middle+armOffset, armRight, armRight))
};

const AABB gFront[] = {
	AABB(Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, armOffset, 1.0f)),
	AABB(Vec3(0.0f, 12.0f / size, 0.0f), Vec3(1.0f, 1.0f, 1.0f)),
	AABB(Vec3(0.0f, 0.0f, 12.0f / size), Vec3(1.0f, 1.0f, 1.0f)),
	AABB(Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 1.0f, 4.0f / size)),
	AABB(Vec3(12.0f / size, 0.0f, 0.0f), Vec3(1.0f, 1.0f, 1.0f)),
	AABB(Vec3(0.0f, 0.0f, 0.0f), Vec3(4.0f / size, 1.0f, 1.0f))
};

bool PistonArmBlock::addCollisionShapes(BlockSource& region, const BlockPos& pos, const AABB* intersectTestBox, std::vector<AABB>& inoutBoxes, Entity* entity) const {
// 	int data = region.getData(pos);
// 	FacingID pistonFacing = PistonBlock::getFacing(data);
// 		
// 	AABB armBaseAabb = gBaseArm[pistonFacing];
// 	AABB armAabb = gArm[pistonFacing];
// 	AABB frontAabb = gFront[pistonFacing];
// 
// 	// get piston entity
// 	BlockPos pistonBasePos;
// 	if(!getSecondPart(region, pos, pistonBasePos)) {
// 		return false;
// 	}
// 
// 	PistonBlockEntity* pistonArmEntity = static_cast<PistonBlockEntity*>(region.getBlockEntity(pistonBasePos));
// 	if(!pistonArmEntity) {
// 		return false; 
// 	}
// 	
// 	const BlockPos& pistonDir = PistonBlock::getFacingDir(pistonFacing);
// 	Vec3 progressPos = Vec3(pistonBasePos) + (Vec3(pistonDir) * pistonArmEntity->getProgress(1.0f));
// 
// 	Block::addAABB(armBaseAabb.translated(progressPos), intersectTestBox, inoutBoxes);
// 	Block::addAABB(armAabb.translated(progressPos), intersectTestBox, inoutBoxes);
// 	Block::addAABB(frontAabb.translated(progressPos), intersectTestBox, inoutBoxes);
	return true;
}

bool PistonArmBlock::canProvideSupport(BlockSource& region, const BlockPos& pos, FacingID face, BlockSupportType type) const {
	int data = region.getData(pos);
	FacingID pistonFacing = PistonBlock::getFacing(data);

	if (pistonFacing != Facing::UP && pistonFacing != Facing::DOWN) {
		pistonFacing = Facing::OPPOSITE_FACING[pistonFacing];
	}

	return pistonFacing == face;
}

bool PistonArmBlock::canBeSilkTouched() const {
	return false;
}

ItemInstance PistonArmBlock::asItemInstance(BlockSource& region, const BlockPos& pos, int blockData) const {
	BlockPos pistonBasePos;

	if(region.hasChunksAt(pos, 1)) {
		int data = region.getData(pos);
		pistonBasePos = pos + -PistonBlock::getFacingDir(data);

		return region.getBlock(pistonBasePos).asItemInstance(region, pos, blockData);
	}
	else {
		DEBUG_ASSERT(false, "piston arm without base!");
		return ItemInstance();
	}
}

//	Let the Piston Base handle It's spawning of the piston item.
void PistonArmBlock::spawnResources(BlockSource& region, const BlockPos& pos, int data, float odds, int bonusLootLevel) const {
}

bool PistonArmBlock::getSecondPart(BlockSource& region, const BlockPos& pos, BlockPos& out) const {
	if(region.hasChunksAt(pos, 1)) {
		int data = region.getData(pos);
		out = pos - PistonBlock::getFacingDir(data);
		return true;
	}

	return false;
}
