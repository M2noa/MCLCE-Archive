#include "Dungeons.h"

#include "LargeCaveFeature.h"

#include "world/level/Level.h"
#include "world/level/block/Block.h"
#include "world/level/block/SandBlock.h"
#include "world/level/chunk/LevelChunk.h"
#include "util/Random.h"
#include "util/Math.h"
#include "world/level/dimension/Dimension.h"
#include "world/level/biome/BiomeSource.h"
#include "world/level/block/BlockVolume.h"

void LargeCaveFeature::apply(BlockVolume& blocks, LevelChunk& lc, Dimension& biomeSource, Random& random) {
	random.setSeed(biomeSource.getBiomes().getSeed());
	int xScale = random.nextInt() / 2 * 2 + 1;
	int zScale = random.nextInt() / 2 * 2 + 1;

	const int r = 8;

	auto& lcPosition = lc.getPosition();
	for (int x = lcPosition.x - r; x <= lcPosition.x + r; x++) {
		for (int z = lcPosition.z - r; z <= lcPosition.z + r; z++) {
			random.setSeed((x * xScale + z * zScale) ^ biomeSource.getBiomes().getSeed());
			addFeature(blocks, lc, biomeSource, random, x, z);
		}
	}
}

void LargeCaveFeature::addRoom(BlockVolume& blocks, Random& random, LevelChunk& lc, const Vec3& room) {
	addTunnel(blocks, random, lc, room, 1 + random.nextFloat() * 6, 0, 0, -1, -1, 0.5);
}

bool LargeCaveFeature::_thinSand(buffer_span<BlockID> blocks, Height height, int y) {
	return y < height - 3 &&
		   blocks[1] == Block::mSand->mID &&
		   blocks[1] == blocks[2] &&
		   blocks[2] == blocks[3];
}

bool LargeCaveFeature::isDiggable(BlockID block, BlockID above) {
	return block == Block::mStone->mID ||
		   block == Block::mDirt->mID ||
		   block == Block::mGrass->mID ||
		   block == Block::mHardenedClay->mID ||
		   block == Block::mStainedClay->mID ||
		   block == Block::mSandStone->mID ||
		   block == Block::mRedSandstone->mID ||
		   block == Block::mMycelium->mID ||
		   block == Block::mPodzol->mID ||
		   (block == Block::mSand->mID && above != Block::mStillWater->mID);
}

void LargeCaveFeature::addTunnel(BlockVolume& blocks, Random& _random, LevelChunk& lc, const Vec3& startPos, float thickness, float yRot, float xRot, int step, int dist, float yScale) {
	auto& lcPosition = lc.getPosition();
	float xMid = (float)(lcPosition.x * 16 + 8);
	float zMid = (float)(lcPosition.z * 16 + 8);

	auto cave = startPos;

	float yRota = 0;
	float xRota = 0;
	RandomSeed seed = _random.nextInt();
	Random random(seed);

	const int RADIUS = 8;

	if (dist <= 0) {
		int max = RADIUS * 16 - 16;
		dist = max - random.nextInt(max / 4);
	}
	bool singleStep = false;

	if (step == -1) {
		step = dist / 2;
		singleStep = true;
	}

	int splitPoint = random.nextInt(dist / 2) + dist / 4;
	bool steep = random.nextInt(6) == 0;

	for (; step < dist; step++) {
		float rad = 1.5f + (sin(step * PI / dist) * thickness) * 1;
		float yRad = rad * yScale;

		float xc = cos(xRot);
		float xs = sin(xRot);
		cave.x += cos(yRot) * xc;
		cave.y += xs;
		cave.z += sin(yRot) * xc;

		if (steep) {
			xRot *= 0.92f;
		}
		else {
			xRot *= 0.7f;
		}
		xRot += xRota * 0.1f;
		yRot += yRota * 0.1f;

		xRota *= 0.90f;
		yRota *= 0.75f;

		float multa = random.nextGaussianFloat();
		xRota += multa * random.nextFloat() * 2;
		multa = random.nextGaussianFloat();
		yRota += multa * random.nextFloat() * 4;

		if (!singleStep && step == splitPoint && thickness > 1) {
			addTunnel(blocks, random, lc, cave, random.nextFloat() * 0.5f + 0.5f, yRot - PI / 2, xRot / 3, step, dist, 1.0);
			addTunnel(blocks, random, lc, cave, random.nextFloat() * 0.5f + 0.5f, yRot + PI / 2, xRot / 3, step, dist, 1.0);
			return;
		}
		if (!singleStep && random.nextInt(4) == 0) {
			continue;
		}

		{
			float xd = cave.x - xMid;
			float zd = cave.z - zMid;
			float remaining = (float)(dist - step);
			float rr = (thickness + 2) + 16;
			if (xd * xd + zd * zd - (remaining * remaining) > rr * rr) {
				return;
			}
		}

		if (cave.x < xMid - 16 - rad * 2 || cave.z < zMid - 16 - rad * 2 || cave.x > xMid + 16 + rad * 2 || cave.z > zMid + 16 + rad * 2) {
			continue;
		}

		int x0 = (int)floor(cave.x - rad) - lcPosition.x * 16 - 1;
		int x1 = (int)floor(cave.x + rad) - lcPosition.x * 16 + 1;

		int y0 = (int)floor(cave.y - yRad) - 1;
		int y1 = (int)floor(cave.y + yRad) + 1;

		int z0 = (int)floor(cave.z - rad) - lcPosition.z * 16 - 1;
		int z1 = (int)floor(cave.z + rad) - lcPosition.z * 16 + 1;

		if (x0 < 0) {
			x0 = 0;
		}
		if (x1 > 16) {
			x1 = 16;
		}

		y1 = Math::clamp(y1, 1, 120);
		y0 = Math::clamp(y0, 1, y1);

		if (z0 < 0) {
			z0 = 0;
		}
		if (z1 > 16) {
			z1 = 16;
		}

		bool detectedWater = false;

		// do not create tunnels through water
		for (int xx = x0; xx < x1; xx++) {
			for (int zz = z0; zz < z1; zz++) {
				for (int yy = y1 + 1; yy >= y0 - 1; yy--) {

					auto p = blocks.index(Pos{ xx, yy, zz });

					if (blocks.block(p) == Block::mFlowingWater->mID || blocks.block(p) == Block::mStillWater->mID) {
						// water might be next to a new tunnel, and should flow into it if possible
						blocks.setBlock(p, Block::mFlowingWater->mID);
						detectedWater = true;
					}
					if (yy != y0 - 1 && xx != x0 && xx != x1 - 1 && zz != z0 && zz != z1 - 1) {
						yy = y0;
					}
				}
			}
		}
		if (detectedWater) {
			continue;
		}

		for (int xx = x0; xx < x1; xx++) {
			float xd = ((xx + lcPosition.x * 16 + 0.5f) - cave.x) / rad;

			for (int zz = z0; zz < z1; zz++) {
				float zd = ((zz + lcPosition.z * 16 + 0.5f) - cave.z) / rad;	
				bool hasGrass = false;
				if (xd * xd + zd * zd < 1) {
					auto p = blocks.index(Pos{ xx, y1, zz });
					for (int yy = y1 - 1; yy >= y0; yy--) {
						float yd = (yy + 0.5f - cave.y) / yRad;
						if (yd > -0.7 && xd * xd + yd * yd + zd * zd < 1) {
							auto& block = blocks.block(p);
							auto& above = blocks.block(p + 1);
							if (block == Block::mGrass->mID) {
								hasGrass = true;
							}
							if (isDiggable(block, above)) {
								if (yy < 10) {
									block = Block::mStillLava->mID;
									lc.deferLightEmitter(lc.getMin() + BlockPos(xx, yy, zz));
								}
								else {
									if (_thinSand(blocks.blocks.tail(p), blocks.getHeight(), yy)) {//fix the sand
										above = Block::mSandStone->mID;
										if (blocks.getData(Pos(xx, yy + 1, zz)) == static_cast<DataID>(SandBlock::SandType::Red)) {
											above = Block::mRedSandstone->mID;
										}
									}
									block = BlockID::AIR;
									if (hasGrass && blocks.block(p - 1) == Block::mDirt->mID) {
										blocks.block(p - 1) = Block::mGrass->mID;
									}
								}
							}
						}
						p--;
					}
				}
			}
		}
		if (singleStep) {
			break;
		}
	}
}

void LargeCaveFeature::addFeature( BlockVolume& blocks, LevelChunk& lc, Dimension& generator, Random& random, int x, int z ) {
	int rand1 = random.nextInt(40) + 1;
	int rand2 = random.nextInt(rand1) + 1;
	int caves = random.nextInt(rand2);
	if (random.nextInt(15) != 0) {
		caves = 0;
	}

	for (int cave = 0; cave < caves; cave++) {
		int randZ = z * 16 + random.nextInt(16);
		int randYParam = random.nextInt(120) + 8;
		int randY = random.nextInt(randYParam);
		int randX = x * 16 + random.nextInt(16);
		Vec3 cavePos((float)randX, (float)randY, (float)randZ);

		int tunnels = 1;
		if (random.nextInt(4) == 0) {
			addRoom(blocks, random, lc, cavePos);
			tunnels += random.nextInt(4);
		}

		for (int i = 0; i < tunnels; i++) {

			float yRot = random.nextFloat() * PI * 2;
			float xRot = ((random.nextFloat() - 0.5f) * 2) / 8;
			float randThick = random.nextFloat();
			float thickness = randThick * 2 + random.nextFloat();

			addTunnel(blocks, random, lc, cavePos, thickness, yRot, xRot, 0, 0, 1.0);
		}
	}
}
