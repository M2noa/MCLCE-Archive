#include "stdafx.h"
#include "net.minecraft.world.level.h"
#include "net.minecraft.world.level.biome.h"
#include "ScatteredFeaturePieces.h"
#include "RandomScatteredLargeFeature.h"

vector<Biome *> RandomScatteredLargeFeature::allowedBiomes;

void RandomScatteredLargeFeature::staticCtor()
{
	allowedBiomes.push_back( Biome::desert );
	allowedBiomes.push_back( Biome::desertHills );
	allowedBiomes.push_back( Biome::jungle );
}

RandomScatteredLargeFeature::RandomScatteredLargeFeature()
{
}

bool RandomScatteredLargeFeature::isFeatureChunk(int x, int z, bool bIsSuperflat)
{
	int featureSpacing = 32;
	int minFeatureSeparation = 8;

	int xx = x;
	int zz = z;
	if (x < 0) x -= featureSpacing - 1;
	if (z < 0) z -= featureSpacing - 1;

	int xCenterFeatureChunk = x / featureSpacing;
	int zCenterFeatureChunk = z / featureSpacing;
	Random *r = level->getRandomFor(xCenterFeatureChunk, zCenterFeatureChunk, 14357617);
	xCenterFeatureChunk *= featureSpacing;
	zCenterFeatureChunk *= featureSpacing;
	xCenterFeatureChunk += r->nextInt(featureSpacing - minFeatureSeparation);
	zCenterFeatureChunk += r->nextInt(featureSpacing - minFeatureSeparation);
	x = xx;
	z = zz;

	bool forcePlacement = false;
	LevelGenerationOptions *levelGenOptions = app.getLevelGenerationOptions();
	if( levelGenOptions != NULL )
	{
		forcePlacement = levelGenOptions->isFeatureChunk(x,z,eFeature_Temples);
	}

	if (forcePlacement || (x == xCenterFeatureChunk && z == zCenterFeatureChunk))
	{
		bool biomeOk = level->getBiomeSource()->containsOnly(x * 16 + 8, z * 16 + 8, 0, allowedBiomes);
		if (biomeOk)
		{
			//                System.out.println("feature at " + (x * 16) + " " + (z * 16));
			return true;
		}
	}

	return false;

}

StructureStart *RandomScatteredLargeFeature::createStructureStart(int x, int z)
{
	//        System.out.println("feature at " + (x * 16) + " " + (z * 16));
	return new ScatteredFeatureStart(level, random, x, z);
}


RandomScatteredLargeFeature::ScatteredFeatureStart::ScatteredFeatureStart(Level *level, Random *random, int chunkX, int chunkZ)
{
	if (level->getBiome(chunkX * 16 + 8, chunkZ * 16 + 8) == Biome::jungle)
	{
		ScatteredFeaturePieces::JunglePyramidPiece *startRoom = new ScatteredFeaturePieces::JunglePyramidPiece(random, chunkX * 16, chunkZ * 16);
		pieces.push_back(startRoom);
		//                System.out.println("jungle feature at " + (chunkX * 16) + " " + (chunkZ * 16));
	}
	else
	{
		ScatteredFeaturePieces::DesertPyramidPiece *startRoom = new ScatteredFeaturePieces::DesertPyramidPiece(random, chunkX * 16, chunkZ * 16);
		pieces.push_back(startRoom);
		//                System.out.println("desert feature at " + (chunkX * 16) + " " + (chunkZ * 16));
	}

	calculateBoundingBox();
}