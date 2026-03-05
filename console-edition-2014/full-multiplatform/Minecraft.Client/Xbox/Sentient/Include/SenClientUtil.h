/********************************************************
*                                                       *
* Copyright (C) Microsoft. All rights reserved.         *
*                                                       *
********************************************************/

// Sentient Client Utility API
//
// Include this to get access to all Utility-related Sentient features.

#pragma once

#include "SenClientTypes.h"


namespace Sentient
{
	/*************************
	 ***** Utility Types *****
	 *************************/

	// This function should return the width (in pixels) of a single 
	// character, and is used to check how many characters can fit on a 
	// single line of the screen
	typedef unsigned int (*SenUtilGetCharacterWidthCallback)( wchar_t character );
		

	/*****************************
	 ***** Utility Functions *****
	 *****************************/

	// This function takes a multi-line text string, a width (in pixels) that
	// a text line can be, and a callback to measure the width of each character.
	// It will return the end of the current text line, and the beginning of 
	// the next text line. Use the beginning of the next text line as input next
	// time you call SenUtilWordWrapFindNextLine()
	void SenUtilWordWrapFindNextLine(
		const wchar_t *startOfThisLine,
		unsigned int maxWidth,
		SenUtilGetCharacterWidthCallback characterWidthCallback,
		const wchar_t **out_endOfThisLine,
		const wchar_t **out_beginningOfNextLine );

	// This returns a SYSTEMTIME set to its earliest possible value.
	SYSTEMTIME SenUtilDateTimeMin();

	// This returns a SYSTEMTIME set to its latest possible value.
	SYSTEMTIME SenUtilDateTimeMax();

} // namespace Sentient
