/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "world/level/newbiome/ZoomLayer.h"

class FuzzyZoomLayer : public ZoomLayer {
public:

	FuzzyZoomLayer(RandomSeed seedMixup, LayerPtr& parent);
	static LayerPtr zoom(RandomSeed seed, LayerPtr& sup, int count);

protected:

	virtual int _modeOrRandom(const int a, const int b, const int c, const int d) override;

};
