#include "stdafx.h"
#include "net.minecraft.world.level.levelgen.structure.h"
#include "JavaMath.h"

bool MineShaftFeature::isFeatureChunk(int x, int z, bool bIsSuperflat)
{
	bool forcePlacement = false;
	LevelGenerationOptions *levelGenOptions = app.getLevelGenerationOptions();
	if( levelGenOptions != NULL )
	{
		forcePlacement = levelGenOptions->isFeatureChunk(x,z,eFeature_Mineshaft);
	}

	return forcePlacement || (random->nextInt(100) == 0 && random->nextInt(80) < max(abs(x), abs(z)));
}

StructureStart *MineShaftFeature::createStructureStart(int x, int z)
{
	// 4J added
	app.AddTerrainFeaturePosition(eTerrainFeature_Mineshaft,x,z);

	return new MineShaftStart(level, random, x, z);
}