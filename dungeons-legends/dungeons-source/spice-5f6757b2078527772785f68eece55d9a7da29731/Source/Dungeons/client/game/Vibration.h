/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

class Vibration {
public:

	virtual ~Vibration() {
	}

	virtual void vibrate(int milliSeconds) = 0;
};
