/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "world/level/newbiome/Layer.h"

class ShoreLayer : public Layer {
public:

	ShoreLayer(RandomSeed seedMixup, LayerPtr& parent);
	virtual void fillArea(LayerData& layerData, int xo, int yo, int w, int h) override;

private:

	void _replaceIfNeighborOcean(LayerData& layerData, int x, int y, int w, int old, int replacement);
	bool _isJungleCompatible(int id);
	bool _isMesa(int id);

};
