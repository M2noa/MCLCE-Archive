/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "Vibration.h"

class NullVibration : public Vibration {
public:

	void vibrate(int milliSeconds) override {
	}
};
