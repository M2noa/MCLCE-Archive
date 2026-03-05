#include "Dungeons.h"

#include "world/level/levelgen/feature/LakeFeature.h"
#include "world/level/Level.h"
#include "world/level/block/Block.h"
#include "world/level/BlockSource.h"
#include "world/level/material/Material.h"
#include "util/Random.h"
#include "world/level/dimension/Dimension.h"

LakeFeature::LakeFeature(BlockID block) :
	mBlock(block)
	, mEmptyBlock(BlockID::AIR) {
}

LakeFeature::LakeFeature(BlockID block, BlockID emptyBlock) :
	mBlock(block)
	, mEmptyBlock(emptyBlock) {
}

bool LakeFeature::place(BlockSource& region, const BlockPos& pos, Random& random) const {
	BlockPos blockPos(pos);
	blockPos.x -= 8;
	blockPos.z -= 8;

	while (blockPos.y > 0 && region.isEmptyBlock(blockPos)) {
		blockPos.y--;
	}
	blockPos.y -= 4;

	const int size = 16 * 16 * 8;
	bool grid[size];

	for (int i = 0; i < size; ++i) {
		grid[i] = false;
	}

	int spots = random.nextInt(4) + 4;

	for (int i = 0; i < spots; i++) {
		float rx = random.nextFloat() * 6 + 3;
		float ry = random.nextFloat() * 4 + 2;
		float rz = random.nextFloat() * 6 + 3;
		Vec3 r(rx, ry, rz);

		Vec3 p = (Vec3(16, 8, 16) - r - Vec3(2, 4, 2)) + Vec3(1, 2, 1) + r / 2;
		p.x *= random.nextFloat();
		p.y *= random.nextFloat();
		p.z *= random.nextFloat();

		for (int xx = 1; xx < 15; xx++) {
			for (int zz = 1; zz < 15; zz++) {
				for (int yy = 1; yy < 7; yy++) {
					float xd = ((xx - p.x) / (r.x / 2));
					float yd = ((yy - p.y) / (r.y / 2));
					float zd = ((zz - p.z) / (r.z / 2));
					float d = xd * xd + yd * yd + zd * zd;
					if (d < 1) {
						grid[((xx) * 16 + (zz)) * 8 + (yy)] = true;
					}
				}
			}
		}
	}

	for (int xx = 0; xx < 16; xx++) {
		for (int zz = 0; zz < 16; zz++) {
			for (int yy = 0; yy < 8; yy++) {
				if (_check(BlockPos(xx, yy, zz), grid)) {
					const Material& m = region.getMaterial(blockPos.offset(xx, yy, zz));
					if (yy >= 4 && m.isLiquid()) {
						return false;
					}
					if (yy < 4 && (!m.isSolid() && region.getBlockID(blockPos.offset(xx, yy, zz)) != mBlock)) {
						return false;
					}
				}
			}
		}
	}

	for (int xx = 0; xx < 16; xx++) {
		for (int zz = 0; zz < 16; zz++) {
			for (int yy = 0; yy < 8; yy++) {
				if (grid[((xx) * 16 + (zz)) * 8 + (yy)]) {
					_setBlockAndData( region, {blockPos.x + xx, blockPos.y + yy, blockPos.z + zz}, yy >= 4 ? mEmptyBlock : mBlock);
				}
			}
		}
	}

	// TODO: rherlitz
	//auto& skyLight = region.getDimension().getSkyLightLayer();
	//for (int xx = 0; xx < 16; xx++) {
	//	for (int zz = 0; zz < 16; zz++) {
	//		for (int yy = 4; yy < 8; yy++) {
	//			if (grid[((xx) * 16 + (zz)) * 8 + (yy)]) {
	//				if (region.getBlockID(blockPos.offset(xx, yy - 1, zz)) == Block::mDirt->mID && region.getBrightness(skyLight, blockPos.offset(xx, yy, zz)) > 0) {
	//					_setBlockAndData( region, {blockPos.x + xx, blockPos.y + yy - 1, blockPos.z + zz}, Block::mGrass->mID);
	//				}
	//			}
	//		}
	//	}
	//}

	if (Block::mBlocks[mBlock]->getMaterial().isType(MaterialType::Lava)) {
		for (int xx = 0; xx < 16; xx++) {
			for (int zz = 0; zz < 16; zz++) {
				for (int yy = 0; yy < 8; yy++) {
					if (_check(BlockPos(xx, yy, zz), grid)) {
						if ((yy < 4 || random.nextInt(2) != 0) && region.getMaterial(blockPos.offset(xx, yy, zz)).isSolid()) {
							_setBlockAndData( region, {blockPos.x + xx, blockPos.y + yy, blockPos.z + zz}, Block::mStone->mID);
						}
					}
				}
			}
		}
	}

	return true;
}

bool LakeFeature::_check(const BlockPos& pos, bool* grid) const
{
	return !grid[(pos.x * 16 + pos.z) * 8 + pos.y] &&
			  ((pos.x < 15 && grid[((pos.x + 1) * 16 + (pos.z)) * 8 + (pos.y)])
			|| (pos.x > 0 && grid[((pos.x - 1) * 16 + (pos.z)) * 8 + (pos.y)])
			|| (pos.z < 15 && grid[((pos.x) * 16 + (pos.z + 1)) * 8 + (pos.y)])
			|| (pos.z > 0 && grid[((pos.x) * 16 + (pos.z - 1)) * 8 + (pos.y)])
			|| (pos.y < 7 && grid[((pos.x) * 16 + (pos.z)) * 8 + (pos.y + 1)])
			|| (pos.y > 0 && grid[((pos.x) * 16 + (pos.z)) * 8 + (pos.y - 1)]));
}
