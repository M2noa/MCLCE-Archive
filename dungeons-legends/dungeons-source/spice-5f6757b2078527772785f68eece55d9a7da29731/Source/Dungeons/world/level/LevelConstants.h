/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

#include "CommonTypes.h"

const Height LEVEL_HEIGHT_DEPRECATED = 256;
const Height LEVEL_GEN_HEIGHT = 128;
const int CHUNK_WIDTH = 16;	// in blocks
const int CHUNK_DEPTH = CHUNK_WIDTH;
const int CHUNK_COLUMNS = CHUNK_WIDTH * CHUNK_DEPTH;

const int RENDERCHUNK_SIDE = 16;
const float SQRT_3 = 1.7320508075689;
const float RENDERCHUNK_DIAGONAL = RENDERCHUNK_SIDE * SQRT_3;
const float RENDERCHUNK_HALF_DIAGONAL = RENDERCHUNK_DIAGONAL / 2;
const int RENDERCHUNK_COLUMNS = RENDERCHUNK_SIDE * RENDERCHUNK_SIDE;
const int RENDERCHUNK_VOLUME = RENDERCHUNK_COLUMNS * RENDERCHUNK_SIDE;

// Used as an invalid max value. 
// Using INT16_MAX as this value goes through float
// operations that cause INT_MAX to overflow.
const int LEVEL_SPAWN_HEIGHT = INT16_MAX;
