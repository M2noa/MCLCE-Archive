#include "Dungeons.h"

#include "world/level/levelgen/v1/LargeHellCaveFeature.h"

#include "world/level/Level.h"
#include "world/level/block/Block.h"
#include "world/level/chunk/LevelChunk.h"
#include "world/level/dimension/Dimension.h"
#include "util/Random.h"
#include "util/Math.h"
#include "HellRandomLevelSource.h"
#include "world/level/block/BlockVolume.h"

void LargeHellCaveFeature::addRoom(BlockVolume& blocks, Random& random, LevelChunk& lc, const Vec3& room) {
	addTunnel(blocks, random, lc, room, 1 + random.nextFloat() * 6, 0, 0, -1, -1, 0.5);
}

void LargeHellCaveFeature::addTunnel(BlockVolume& blocks, Random& _random, LevelChunk& lc, const Vec3& startPos, float thickness, float yRot, float xRot, int step, int dist, float yScale) {
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
		float rad = 1.5f + (Math::sin(step * PI / dist) * thickness) * 1;
		float yRad = rad * yScale;

		float xc = Math::cos(xRot);
		float xs = Math::sin(xRot);
		cave.x += Math::cos(yRot) * xc;
		cave.y += xs;
		cave.z += Math::sin(yRot) * xc;

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

		int x0 = Math::floor(cave.x - rad) - lcPosition.x * 16 - 1;
		int x1 = Math::floor(cave.x + rad) - lcPosition.x * 16 + 1;

		int y0 = Math::floor(cave.y - yRad) - 1;
		int y1 = Math::floor(cave.y + yRad) + 1;

		int z0 = Math::floor(cave.z - rad) - lcPosition.z * 16 - 1;
		int z1 = Math::floor(cave.z + rad) - lcPosition.z * 16 + 1;

		if (x0 < 0) {
			x0 = 0;
		}
		if (x1 > 16) {
			x1 = 16;
		}

		if (y0 < 1) {
			y0 = 1;
		}
		if (y1 > HellGen::GEN_DEPTH - 8) {
			y1 = HellGen::GEN_DEPTH - 8;
		}

		if (z0 < 0) {
			z0 = 0;
		}
		if (z1 > 16) {
			z1 = 16;
		}

		bool detectedWater = false;

		for (int xx = x0; !detectedWater && xx < x1; xx++) {
			for (int zz = z0; !detectedWater && zz < z1; zz++) {
				for (int yy = y1 + 1; !detectedWater && yy >= y0 - 1; yy--) {
					if (yy < 0 || yy >= HellGen::GEN_DEPTH) {
						continue;
					}

					int p = (xx * 16 + zz) * HellGen::GEN_DEPTH + yy;
					if (blocks.block(p) == Block::mFlowingLava->mID || blocks.block(p) == Block::mStillLava->mID) {
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
			double xd = ((xx + lcPosition.x * 16 + 0.5) - cave.x) / rad;

			for (int zz = z0; zz < z1; zz++) {
				double zd = ((zz + lcPosition.z * 16 + 0.5) - cave.z) / rad;
				int p = (xx * 16 + zz) * HellGen::GEN_DEPTH + y1;

				for (int yy = y1 - 1; yy >= y0; yy--) {
					double yd = (yy + 0.5 - cave.y) / yRad;
					if (yd > -0.7 && xd * xd + yd * yd + zd * zd < 1) {
						if (blocks.block(p) == Block::mNetherrack->mID || blocks.block(p) == Block::mDirt->mID || blocks.block(p) == Block::mGrass->mID) {
							blocks.setBlock(p, BlockID::AIR);
						}
					}
					p--;
				}
			}
		}
		if (singleStep) {
			break;
		}
	}
}

void LargeHellCaveFeature::addFeature( BlockVolume& blocks, LevelChunk& lc, Dimension& generator, Random& random, int x, int z ) {
	int rand1 = random.nextInt(10) + 1;
	int rand2 = random.nextInt(rand1) + 1;
	int caves = random.nextInt(rand2);
	if (random.nextInt(5) != 0) {
		caves = 0;
	}

	for (int cave = 0; cave < caves; cave++) {
			const int randZ = (z * 16 + random.nextInt(16));
			const int randY = random.nextInt(HellGen::GEN_DEPTH);
			const int randX = (x * 16 + random.nextInt(16));
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

			addTunnel(blocks, random, lc, cavePos, thickness * 2, yRot, xRot, 0, 0, 0.5f);
		}
	}
}

void LargeHellCaveFeature::apply(BlockVolume& blocks, LevelChunk& lc, Dimension& biomeSource, Random& random) {
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
