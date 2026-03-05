/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "world/level/newbiome/Layer.h"

class VoronoiZoom : public Layer {
public:

	static const int ZOOM_BITS = 2;
	static const int ZOOM_OFFSET = 1;
	static const int ZOOM = 1 << ZOOM_BITS;
	static const int ZOOM_MASK = ZOOM - 1;

	VoronoiZoom(RandomSeed seedMixup, LayerPtr& parent);
	virtual void fillArea(LayerData& layerData, int xo, int yo, int w, int h) override;

};
