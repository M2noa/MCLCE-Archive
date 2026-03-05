/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

enum class ChunkState : unsigned char {
	Unloaded,
	Generating,
	Generated,
	PostProcessing,
	Loaded
};

enum class ChunkTerrainDataState : unsigned char {
	NoData, // nothing done to generate/load data
	NeedsFixup, // needs fixing up due to being an older level
	ReadyForGeneration, // ready for generation
	Generated, // terrain has been generated
	PostProcessed, // terrain has been postprocessed
	Ready, // terrain is ready for playing
};

enum class ChunkDebugDisplaySavedState : unsigned char {
	Generated, // chunk is generated, but not saved to the DB, will be generated on any future loads
	Saved, // chunk exists in the DB, will be loaded on any future loads
};

enum class ChunkCachedDataState : unsigned char {
	NotGenerated,
	Generating,
	Generated
};