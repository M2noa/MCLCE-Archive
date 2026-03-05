/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "world/level/newbiome/Layer.h"

class ZoomLayer : public Layer {
public:

	static const int ZOOM_BITS = 1;
	static const int ZOOM_OFFSET = 1;
	static const int ZOOM_MASK = 1 << (ZOOM_BITS - 1);

	ZoomLayer(RandomSeed seedMixup, LayerPtr parent);
	virtual void fillArea(LayerData& layerData, int xo, int yo, int w, int h) override;
	static LayerPtr zoom(RandomSeed seed, LayerPtr& sup, int count);

};
