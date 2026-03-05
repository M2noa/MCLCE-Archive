/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

enum class StorageVersion : int {
	Unknown,
	OldV1,
	OldV2,
	OldV3,
	LevelDB1,
	LevelDBSubChunks,
};
