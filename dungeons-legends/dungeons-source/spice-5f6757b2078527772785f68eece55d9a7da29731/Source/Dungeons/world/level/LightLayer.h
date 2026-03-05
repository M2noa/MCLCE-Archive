/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "CommonTypes.h"

class LightLayer {
public:

	static const LightLayer BLOCK;	// (0)

	LightLayer(int id, Brightness surrounding);

	bool operator==(const LightLayer& layer) const;

	const Brightness getSurrounding() const { return mSurrounding; }
	void setSurrounding(Brightness surrounding) {
		mSurrounding = surrounding;
	}

	bool isSky() const {
		return mId == 0;
	}
private:

	Brightness mSurrounding;
	const int mId;
};
