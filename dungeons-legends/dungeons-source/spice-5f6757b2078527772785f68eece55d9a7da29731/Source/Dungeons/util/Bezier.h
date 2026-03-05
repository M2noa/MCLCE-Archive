/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

namespace Util {
	float getPoint(float p1, float p2, float factor) {
		return p1 + ((p2 - p1) * factor);
	}

	float getBezier(float p1, float p2, float p3, float factor) {
		const float r1 = getPoint(p1, p2, factor);
		const float r2 = getPoint(p2, p3, factor);

		return getPoint(r1, r2, factor);
	}
}