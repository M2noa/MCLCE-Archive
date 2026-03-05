/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "pch_core.h"

#include "Core/Utility/InitOnce.h"

InitOnce::InitOnce(const std::function<void()>& initFunction)
	: mInitialized(false) 
	, mInitFunction(initFunction) {
}

void InitOnce::setInitAction(const std::function<void()>& initFunction) {
	mInitFunction = initFunction;
}

void InitOnce::initOnce() {
	if (!mInitialized) {
		if (mInitFunction) {
			mInitFunction();
			mInitialized = true;
		}
	}
}