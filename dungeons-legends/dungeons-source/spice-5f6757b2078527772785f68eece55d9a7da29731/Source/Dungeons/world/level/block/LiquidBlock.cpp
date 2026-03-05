/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "world/level/block/LiquidBlock.h"
#include "world/level/BlockSource.h"
#include "world/level/material/Material.h"
// #include "world/level/BlockTickingQueue.h"
#include "world/level/Level.h"
#include "world/entity/Mob.h"
#include "world/level/biome/Biome.h"
#include "world/level/dimension/Dimension.h"
#include "world/Facing.h"

LiquidBlock::LiquidBlock(const std::string& nameId, int id, const Material& material)
	: Block(nameId, id, material) {
	mCanBuildOver = true;
	setSolid(false);
	mRenderLayer = (material.isType(MaterialType::Water)) ? RENDERLAYER_WATER : RENDERLAYER_LAVA;
	mProperties = BlockProperty::BreakOnPush;
	mCanInstatick = true;
	mAnimatedTexture = true;
	mTranslucency[id] = std::max(0.8f, mMaterial.getTranslucency());
}

void LiquidBlock::trySpreadFire(BlockSource& region, const BlockPos& pos, Random& random) const {
// 	if (region.getTickQueue(pos)->isInstaticking()) {
// 		return;
// 	}

	const int r = 1;
	static const BlockPos offsets[] = {
		{ 1, 0, 0 },
		{-1, 0, 0 },
		{ 0, 1, 0 },
		{ 0, -1, 0 },
		{ 0, 0, 1 },
		{ 0, 0, -1 }
	};

	for (int i = 0; i < 10; ++i) {
		int tx = pos.x + random.nextInt(r * 2 + 1) - r;
		int ty = pos.y + random.nextInt(r * 2 + 1) - r;
		int tz = pos.z + random.nextInt(r * 2 + 1) - r;

		auto& block = Block::mBlocks[region.getBlockID(tx, ty, tz)];

		if (block && block->getMaterial().isFlammable()) {
			//check if any block adjacent to this one is air, place fire there
			for (auto& o : offsets) {
				if (region.getBlockID(tx + o.x, ty + o.y, tz + o.z) == BlockID::AIR) {
					region.setBlock(tx + o.x, ty + o.y, tz + o.z, Block::mFire->mID, Block::UPDATE_ALL);
					return;
				}
			}
		}
	}
}

float LiquidBlock::getHeightFromData(int data) {
	int depth = Block::mStillWater->getBlockState(BlockState::LiquidDepth).get<int>(data);
	if (depth >= 8) {
		depth = 0;
	}

	return (depth + 1) / 9.0f;
}

bool LiquidBlock::mayPick(BlockSource& region, int data, bool liquid) const {
	int depth = getBlockState(BlockState::LiquidDepth).get<int>(data);
	return liquid && depth == 0;
}

bool LiquidBlock::shouldRenderFace(BlockSource& region, const BlockPos& pos, FacingID face, const AABB& shape) const {
	const Material& m = region.getMaterial(pos);
	if (m == mMaterial) {
		return false;
	}

	if (m.isType(MaterialType::Ice)) {
		return false;
	}

	if (face == Facing::UP) {
		return true;
	}

	return Block::shouldRenderFace(region, pos, face, shape);
}

void LiquidBlock::handleEntityInside(BlockSource& region, const BlockPos& pos, Entity* entity, Vec3& current) const {
	Vec3 flow = _getFlow(region, pos);
	current.x += flow.x * .5f;
	current.y += flow.y * .5f;
	current.z += flow.z * .5f;
}

int LiquidBlock::getTickDelay(BlockSource& region) const {
	if (mMaterial.isType(MaterialType::Water)) {
		return 5;
	} else if (mMaterial.isType(MaterialType::Lava)) {
		return region.getDimensionConst().isUltraWarm() ? 10 : 30;
	} else {
		return 0;
	}
}

float LiquidBlock::getBrightness(BlockSource& region, const BlockPos& pos) {
	float a = region.getBrightness(pos);
	float b = region.getBrightness(pos.above());
	return a > b ? a : b;
}

void LiquidBlock::animateTick(BlockSource& region, const BlockPos& pos, Random& random) const {
// 	if (mMaterial.isType(MaterialType::Water) && random.nextInt(16) == 0) {
// 		int depth = getBlockState(BlockState::LiquidDepth).get<int>(region.getData(pos));
// 		if (depth > 0 && depth < 8) {
// 			region.getLevel().playSound(LevelSoundEvent::Water, Vec3(pos));
// 		}
// 	}
// 
// 	if (mMaterial.isType(MaterialType::Lava)) {
// 		BlockID aboveID = region.getBlockID(pos.above());
// 		if (aboveID == BlockID::AIR) {
// 			if (random.nextInt(100) == 0) {
// 				float posx = pos.x + random.nextFloat();
// 				float posz = pos.z + random.nextFloat();
// 				region.getLevel().addParticle(ParticleType::Lava, Vec3(posx, pos.y + 0.5f, posz), Vec3::ZERO);
// 				region.getLevel().playSound(LevelSoundEvent::LavaPop, Vec3(pos));
// 			}
// 			
// 			if (random.nextInt(50) == 0) {
// 				region.getLevel().playSound(LevelSoundEvent::Lava, Vec3(pos));
// 			}
// 		}
// 	}
// 
// 	AABB tmpAABB;
// 	const AABB& shape = getVisualShape(region, pos, tmpAABB, false);
// 	if (mMaterial.isType(MaterialType::Lava)) {
// 		if (region.getMaterial(pos.above()).isType(MaterialType::Air) && !region.isConsideredSolidBlock(pos.above())) {
// 			if (random.nextInt(100) == 0) {
// 				float posx = pos.x + random.nextFloat();
// 				float posz = pos.z + random.nextFloat();
// 				region.getLevel().addParticle(ParticleType::Lava, Vec3(posx, pos.y + shape.max.y, posz), Vec3(0.0f));
// 			}
// 		}
// 	}
// 
// 	if(random.nextInt(10) == 0) {
// 		BlockPos belowPos = pos.below();
// 		const Material& below2Material = region.getMaterial(pos.below(2));
// 		// TODO: Should we really use canProvideSupport here?
// 		if (region.canProvideSupport(belowPos, Facing::UP, BlockSupportType::Any) && (!below2Material.getBlocksMotion()) && !below2Material.isLiquid()) {
// 
// 			if (below2Material.isLiquid()) {
// 				return;
// 			}
// 
// 			float posx = pos.x + random.nextFloat();
// 			float posz = pos.z + random.nextFloat();
// 
// 			if(mMaterial.isType(MaterialType::Water)) {
// 				region.getLevel().addParticle(ParticleType::DripWater, Vec3(posx, pos.y - 1.05f, posz), Vec3(0.0f));
// 			} else {
// 				region.getLevel().addParticle(ParticleType::DripLava, Vec3(posx, pos.y - 1.05f, posz), Vec3(0.0f));
// 			}
// 		}
// 	}
}

float LiquidBlock::getSlopeAngle(BlockSource& region, const BlockPos& pos, const Material& m) {
	Vec3 flow;
	if (m.isType(MaterialType::Water)) {
		flow = static_cast<const LiquidBlock*>(Block::mFlowingWater)->_getFlow(region, pos);
	} else if (m.isType(MaterialType::Lava)) {
		flow = static_cast<const LiquidBlock*>(Block::mFlowingLava)->_getFlow(region, pos);
	}

	if (flow.x == 0 && flow.z == 0) {
		return -1000;
	}

	return atan2(flow.z, flow.x) - PI * 0.5f;
}

void LiquidBlock::emitFizzParticle(BlockSource& region, const BlockPos& p) const {
// 	if (region.getPublicSource()) {
// 		auto& level = region.getLevel();
// 		const float pitch = (2.6f + level.getRandom().nextGaussianFloat() * 0.8f) * 1000.0f;
// 		const Vec3 pos(p.x + 0.5f, p.y + 0.5f, p.z + 0.5f);
// 		level.broadcastDimensionEvent(region, LevelEvent::SoundFizz, pos, (int)pitch);
// 
// 		for (int i = 0; i < 8; i++) {
// 			auto ox = level.getRandom().nextFloat();
// 			auto oz = level.getRandom().nextFloat();
// 			Vec3 particlePos(
// 				p.x + ox,
// 				p.y + 1.2f,
// 				p.z + oz);
// 
// 			level.addParticle(ParticleType::Smoke, particlePos, Vec3(0.0f), 250);
// 		}
// 	}
}

int LiquidBlock::getDepth(BlockSource& region, const BlockPos& pos) const {
	if (region.getMaterial(pos) != mMaterial) {
		return -1;
	} else {
		return getBlockState(BlockState::LiquidDepth).get<int>(region.getData(pos));
	}
}

int LiquidBlock::getRenderedDepth(BlockSource& region, const BlockPos& pos) const {
	if (region.getMaterial(pos) != mMaterial) {
		return -1;
	}

	int depth = getBlockState(BlockState::LiquidDepth).get<int>(region.getData(pos));
	if (depth >= 8) {
		depth = 0;
	}

	return depth;
}

bool LiquidBlock::solidify(BlockSource& region, const BlockPos& pos, const BlockPos& changedNeighbor) const {
	// Don't solidify upwards
	if (changedNeighbor.y - pos.y >= 0 && mMaterial.isType(MaterialType::Lava) && region.getMaterial(changedNeighbor).isType(MaterialType::Water)) {
		int depth = getBlockState(BlockState::LiquidDepth).get<int>(region.getData(pos));
		if (depth == 0) {
			region.setBlockAndData(pos, Block::mObsidian->mID, Block::UPDATE_ALL);
		} else if (depth <= 4) {
			region.setBlockAndData(pos, Block::mCobblestone->mID, Block::UPDATE_ALL);
		} else {
			return false;
		}

// 		if (!region.getTickQueue(pos)->isInstaticking()) {
// 			emitFizzParticle(region, pos);
// 		}

		return true;
	}

	return false;
}

Vec3 LiquidBlock::_getFlow(BlockSource& region, const BlockPos& pos) const {
	Vec3 flow(0, 0, 0);
	int mid = getRenderedDepth(region, pos);

	for (int d = 0; d < 4; d++) {
		int xt = pos.x;
		int yt = pos.y;
		int zt = pos.z;

		if (d == 0) {
			xt--;
		}
		if (d == 1) {
			zt--;
		}
		if (d == 2) {
			xt++;
		}
		if (d == 3) {
			zt++;
		}

		int t = getRenderedDepth(region, BlockPos( xt, yt, zt ));
		if (t < 0) {
			if (!region.getMaterial(xt, yt, zt).getBlocksMotion()) {
				t = getRenderedDepth(region, BlockPos( xt, yt - 1, zt ));
				if (t >= 0) {
					int dir = t - (mid - 8);
					flow = flow.add((float)((xt - pos.x) * dir), (float)((yt - pos.y) * dir), (float)((zt - pos.z) * dir));
				}
			}
		} else {
			if (t >= 0) {
				int dir = t - mid;
				flow = flow.add((float)((xt - pos.x) * dir), (float)((yt - pos.y) * dir), (float)((zt - pos.z) * dir));
			}
		}
	}
	int depth = getBlockState(BlockState::LiquidDepth).get<int>(region.getData(pos));
	if (depth >= 8) {
		bool ok = false;
		AABB tmpAABB;
		const AABB& shape = getVisualShape(region, pos, tmpAABB);
		if(ok || shouldRenderFace(region, pos.north(), 2, shape)) {
			ok = true;
		}
		if(ok || shouldRenderFace(region, pos.south(), 3, shape)) {
			ok = true;
		}
		if(ok || shouldRenderFace(region, pos.west(), 4, shape)) {
			ok = true;
		}
		if(ok || shouldRenderFace(region, pos.east(), 5, shape)) {
			ok = true;
		}
		if(ok || shouldRenderFace(region, pos.offset(0, 1, -1), 2, shape)) {
			ok = true;
		}
		if(ok || shouldRenderFace(region, pos.offset(0, 1, 1), 3, shape)) {
			ok = true;
		}
		if(ok || shouldRenderFace(region, pos.offset(-1, 1, 0), 4, shape)) {
			ok = true;
		}
		if(ok || shouldRenderFace(region, pos.offset(1, 1, 0), 5, shape)) {
			ok = true;
		}
		if (ok) {
			flow = flow.normalized().add(0, -6, 0);
		}
	}

	flow = flow.normalized();
	return flow;
}

bool LiquidBlock::checkIsPathable(Entity& entity, const BlockPos& lastPathPos, const BlockPos& pathPos) const{
	return mMaterial != Material::getMaterial(MaterialType::Lava);
}

Color LiquidBlock::getMapColor(BlockSource& region, const BlockPos& pos) const {
	if (mMaterial.isType(MaterialType::Water)) {
		Color totalColor;

		float samples = 0.0f;
		for (auto oz : range_incl(-1, 1)) {
			for (auto ox : range_incl(-1, 1)) {
				// Only average colors from biomes that are loaded
				Biome* biome = region.tryGetBiome(pos.offset(ox, 0, oz));
				if(biome) {
					totalColor += biome->mWaterColor;
					++samples;
				}
			}
		}

		// If samples == 0, so do all colors
		return samples == 0.0f ? totalColor : Color(totalColor.r / samples, totalColor.g / samples, totalColor.b / samples, totalColor.a / samples) * Color::fromARGB(0x7184ff);
	}
	return Block::getMapColor(region, pos);
}

const MobSpawnerData* LiquidBlock::getMobToSpawn(BlockSource& region, const BlockPos& pos) const {
	return getTypeToSpawn(region, EntityType::WaterAnimal, pos);
}

void LiquidBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const {
	solidify(region, pos, neighborPos);
}

void LiquidBlock::onPlace(BlockSource& region, const BlockPos& pos) const {
	solidify(region, pos, pos.south()) ||
	solidify(region, pos, pos.north()) ||
	solidify(region, pos, pos.east()) ||
	solidify(region, pos, pos.west()) ||
	solidify(region, pos, pos.above());
}

int LiquidBlock::getColor(BlockSource& region, const BlockPos& pos) const {
	if(mMaterial.isType(MaterialType::Water)) {
		Color totalColor;

		for(auto oz : range_incl(-1, 1)) {
			for(auto ox : range_incl(-1, 1)) {
				totalColor += region.getBiome(pos.offset(ox, 0, oz)).mWaterColor;
			}
		}

		return Color(totalColor.r / 9.f, totalColor.g / 9.f, totalColor.b / 9.f, totalColor.a / 9.f).toARGB();
	}

	return 0xffffffff;
}

int LiquidBlock::getResourceCount(Random& random, int data, int bonusLootLevel) const {
	return 0;
}

int LiquidBlock::getResource(Random& random, int data, int bonusLootLevel) const {
	return 0;
}

const AABB& LiquidBlock::getAABB(BlockSource& region, const BlockPos& pos, AABB& bufferValue, int optionalData, bool isClipping, int clipData) const {
	if(isClipping) {
		return bufferValue.set(Vec3::ZERO, Vec3::ONE).move(Vec3(pos));
	}

	return AABB::EMPTY;
}
