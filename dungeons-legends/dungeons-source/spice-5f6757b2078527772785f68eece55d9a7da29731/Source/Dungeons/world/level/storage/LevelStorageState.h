/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include <string>

enum class LevelStorageState {
	Open,
	Corrupted,
	NotFound,
	IOError,
	NotSupported,
	InvalidArguments,
	Unknown
};

class Level;

struct LevelStorageResult {
	LevelStorageState state;
	std::string extra;
};
