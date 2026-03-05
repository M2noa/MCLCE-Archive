/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "world/level/newbiome/Layer.h"

class AddEdgeLayer : public Layer {
public:

	static const int OFFSET = 1;
	enum class Mode {
		COOL_WARM,
		HEAT_ICE,
		SPECIAL
	};

	AddEdgeLayer(RandomSeed seedMixup, LayerPtr& parent, Mode mode);
	virtual void fillArea(LayerData& layerData, int originX, int originY, int width, int height) override;
	void fillCoolWarm(LayerData& layerData, int originX, int originY, int width, int height);
	void fillHeatIce(LayerData& layerData, int originX, int originY, int width, int height);
	void fillIntroduceSpecial(LayerData& layerData, int originX, int originY, int width, int height);

private:

	const Mode mMode;

};
