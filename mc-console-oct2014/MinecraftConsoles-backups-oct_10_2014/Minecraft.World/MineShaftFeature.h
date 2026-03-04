#pragma once

#include "StructureFeature.h"

class MineShaftFeature : public StructureFeature
{
protected:
	virtual bool isFeatureChunk(int x, int z, bool bIsSuperflat=false);
	virtual StructureStart *createStructureStart(int x, int z);
};