#pragma once

#include "SenClientCulture.h"

namespace Sentient
{

//enum
//{
//	// Note that these include a terminating nul.
//	SenCulture_DescSizeMax = 80,
//	SenCulture_NameSizeMax = 12,
//};

struct SenCultureInfo
{
	wchar_t name[12]; // e.g. { L"en", L"en-CA", L"en-GB", L"" }
	wchar_t desc[80]; // e.g. L"English (United States)"
	size_t  nameCount;                    // e.g. 1
	const SenCultureInfo *parent;
};

static SenCultureInfo s_defaultculture;

__declspec(deprecated("This function is deprecated, and should not be used"))  
__inline const wchar_t *SenCultureGetSystemLanguageString()
{
	return L"en";
}

__declspec(deprecated("This function is deprecated, and should not be used"))  
__inline const wchar_t *SenCultureGetSystemLocaleString()
{
	return L"US";
}

__declspec(deprecated("This function is deprecated, and should not be used"))  
const wchar_t *SenCultureGetSystemCultureString();
//{
//	return L"en-US";
//}

__declspec(deprecated("This function is deprecated, and should not be used"))  
__inline const SenCultureInfo *SenCultureFind( const wchar_t *name )
{
	return &s_defaultculture;
}

__declspec(deprecated("This function is deprecated, and should not be used"))  
__inline const SenCultureInfo *SenCultureGetParent( const SenCultureInfo *culture )
{
	return &s_defaultculture;
}

__declspec(deprecated("This function is deprecated, and should not be used"))  
__inline HRESULT SenCultureSetCurrent( const wchar_t name[] )
{
	return E_FAIL;
}

__declspec(deprecated("This function is deprecated, and should not be used"))  
__inline const SenCultureInfo *SenCultureGetCurrent()
{
	return &s_defaultculture;
}


__declspec(deprecated("This function is deprecated, and should not be used"))  
__inline HRESULT SenCultureSetDefault( const wchar_t name[] )
{
	return E_FAIL;
}


__declspec(deprecated("This function is deprecated, and should not be used"))  
__inline const SenCultureInfo *SenCultureGetDefault()
{
	return &s_defaultculture;
}

}