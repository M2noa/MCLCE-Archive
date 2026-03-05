/********************************************************
*                                                       *
* Copyright (C) Microsoft. All rights reserved.         *
*                                                       *
********************************************************/

// Sentient Client Culture API
//
// Include this to get access to all Culture-related Sentient features.

#pragma once
#include "SenClientTypes.h"

namespace Sentient
{
	//=========================//
	//                         //
	//    Culture Functions    //
	//                         //
	//=========================//

	/// @brief           Set the current culture to the one specified, if possible.
	///
	/// @param[in]       dwLanguage
    ///                  the DWORD id as defined in xconfig.h (e.g. XC_LANGUAGE_ENGLISH) for the language to use

    /// @param[in]       dwLocale
    ///                  the DWORD id as defined in xconfig.h (e.g. XC_LOCALE_GREAT_BRITAIN) for the region to use
	///
	/// @return          Check SUCCEEDED( hresult ) or FAILED( hresult ) to determine success.  Specific values include:
	///                  SENTIENT_E_NOT_INITIALIZED: You did not call SentientInitialize() first.
	///                  E_FAIL: Failed to find the given culture.
	///                  S_OK: Current culture set successfully.
	///
	/// @details         Set the current culture to the one specified, if possible (if not english will be used).
    ///                  This should only be called during title launch if it is determined that the console is set to a lanuage unsupported by the title.
    ///                  This method should only be called once, right after calling SentientInitialize() 
	///                  This is the culture that will be used when any string routine is called.
	///                  By default, this is set to the system culture & region.
	///
    HRESULT SetCurrentCulture(
        __in DWORD dwLanguage,	
        __in DWORD dwLocale
        );

    /// @brief           Set the current culture to English (no region)
	///
	/// @return          Check SUCCEEDED( hresult ) or FAILED( hresult ) to determine success.  Specific values include:
	///                  SENTIENT_E_NOT_INITIALIZED: You did not call SentientInitialize() first.
	///                  E_FAIL: Failed to find the given culture.
	///                  S_OK: Current culture set successfully.
	///
	/// @details         Set the current culture English.
    ///                  This should only be called during title launch if it is determined that the console is set to a lanuage unsupported by the title.
    ///                  This method should only be called once, right after calling SentientInitialize() 
	///                  This is the culture that will be used when any string routine is called.
	///                  By default, Sentient uses the console's culture & region.
	///
    __inline HRESULT SetCurrentCultureEnglish() { return SetCurrentCulture(XC_LANGUAGE_ENGLISH, 0); }

} // namespace Sentient
