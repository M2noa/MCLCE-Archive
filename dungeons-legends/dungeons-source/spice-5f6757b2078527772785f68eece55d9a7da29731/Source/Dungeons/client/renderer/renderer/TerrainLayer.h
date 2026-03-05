/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/

#pragma once

enum class TerrainLayer : int {
	Opaque,
	EndPortal,
	OpaqueSeasons,
	DoubleSide,
	Alpha,
	AlphaSingleSide,
	AlphaSeasons,
	Blend,
	Water,
	WaterStencil,
	Far,
	FarSeasons,
	FarSeasonsAlpha,

	Count
};
