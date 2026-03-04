#include "stdafx.h"

//--------------------------------------------------------------------------------------
// Name: DebugSpewV()
// Desc: Internal helper function
//--------------------------------------------------------------------------------------
#ifndef _CONTENT_PACKAGE
static VOID DebugSpewV( const CHAR* strFormat, const va_list pArgList )
{
#if defined __PS3__
	assert(0);
#else
	CHAR str[2048];
	// Use the secure CRT to avoid buffer overruns. Specify a count of
	// _TRUNCATE so that too long strings will be silently truncated
	// rather than triggering an error.
	_vsnprintf_s( str, _TRUNCATE, strFormat, pArgList );
	OutputDebugStringA( str );
#endif
}
#endif

//--------------------------------------------------------------------------------------
// Name: DebugSpew()
// Desc: Prints formatted debug spew
//--------------------------------------------------------------------------------------
#ifdef  _Printf_format_string_  // VC++ 2008 and later support this annotation
VOID CDECL DebugSpew( _In_z_ _Printf_format_string_ const CHAR* strFormat, ... )
#else
VOID CDECL DebugPrintf( const CHAR* strFormat, ... )
#endif
{
#ifndef _CONTENT_PACKAGE
	va_list pArgList;
	va_start( pArgList, strFormat );
	DebugSpewV( strFormat, pArgList );
	va_end( pArgList );
#endif
}

