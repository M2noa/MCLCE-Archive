/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "world/level/newbiome/Layer.h"

class IslandLayer : public Layer {
public:

	IslandLayer(RandomSeed seedMixup);
	virtual void fillArea(LayerData& layerData, int xo, int yo, int w, int h) override;

};
