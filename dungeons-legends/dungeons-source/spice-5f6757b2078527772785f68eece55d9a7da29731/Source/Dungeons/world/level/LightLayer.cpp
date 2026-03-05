/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#include "Dungeons.h"

#include "world/level/LightLayer.h"

const LightLayer LightLayer::BLOCK(1, Brightness(0));

LightLayer::LightLayer(int id, Brightness surrounding) :
	mSurrounding(surrounding)
	, mId(id) {
}

bool LightLayer::operator==(const LightLayer& layer) const {
	return mId == layer.mId;
}
