#pragma once
#include "world/level/newbiome/Layer.h"

class SmoothLayer : public Layer {
public:

	SmoothLayer(RandomSeed seedMixup, LayerPtr& parent);
	virtual void fillArea(LayerData& layerData, int xo, int yo, int w, int h) override;

};
