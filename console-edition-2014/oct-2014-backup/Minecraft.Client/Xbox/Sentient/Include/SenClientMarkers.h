/********************************************************
*                                                       *
* Copyright (C) Microsoft. All rights reserved.         *
*                                                       *
********************************************************/

// Sentient Client UGC API
//
// Include this to get access to Markers.

#pragma once

#include "SenClientSys.h"


namespace Sentient
{
	/************************
	 ***** Marker Types *****
	 ************************/

	struct SenMarker
	{
		float		x, y, z;
		PlayerUID		user;
		int			userData;
		int			userData2;
	};


	/****************************
	 ***** Marker Functions *****
	 ****************************/

	HRESULT SenMarkerAdd(
		int userIndex,
		SenSysTitleID titleID,
		SenUGCID ugcID,
		const SenMarker *marker );

	HRESULT SenMarkerGetWithinArea(
		SenSysTitleID titleID,
		SenUGCID ugcID,
		float xMin, float yMin, float zMin,
		float xMax, float yMax, float zMax,
		int maxCount,
		PlayerUID friendsOf,
		int minTag,
		int maxTag,
		int tagMask,
		SenMarker **out_buffer,
		SenSysCompletedCallback userCallback,
		void *userCallbackData );

} // namespace Sentient
