/********************************************************
*                                                       *
* Copyright (C) Microsoft. All rights reserved.         *
*                                                       *
********************************************************/

// Sentient Client System API
//
// Include this to get access to all System Sentient features.

#pragma once

#include "SenClientTypes.h"


namespace Sentient
{
	/************************
	 ***** System Types *****
	 ************************/

	// Standardized callback that all asynchronous functions call either on completion or on failure.
	typedef void  (*SenSysCompletedCallback)( HRESULT taskResult, void *userCallbackData );

	typedef void *(*SenSysMemAllocCallback)( size_t size );

	typedef void  (*SenSysMemFreeCallback)( void *ptr );

	typedef void  (*SenSysOutputCallback)( const wchar_t *outputString );


	/****************************
	 ***** System Functions *****
	 ****************************/

#ifdef __WINLIVE__
	// Call this to get the GFWL version number that Sentient was compiled against
	int SenSysGetGFWLSDKVersion();
#endif
	
	// Sentient calls this to get total allocated memory.
	// (this is only tracked in debug mode - in release mode, this will return 0)
	size_t SenSysMemAllocated();

	// Redirects all of Sentient's internal memory allocs to a custom allocator.
	void SenSysSetMemAllocCallback( SenSysMemAllocCallback callback );

	// Redirects all of Sentient's internal memory frees to a custom allocator.
	void SenSysSetMemFreeCallback( SenSysMemFreeCallback callback );

	// Redirects all of Sentient's internal output to a custom routine.
	void SenSysSetOutputCallback( SenSysOutputCallback callback );

} // namespace Sentient
