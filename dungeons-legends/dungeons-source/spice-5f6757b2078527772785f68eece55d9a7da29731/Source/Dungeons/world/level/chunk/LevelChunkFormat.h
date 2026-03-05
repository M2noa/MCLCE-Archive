/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

enum class LevelChunkFormat : char {
	v9_00,
	v9_02,	//added to fix the grass color being corrupted
	v9_05,	//make sure that biomes are not corrupted
	v17_0,  //switch to a key per subchunk + 2D data
};
