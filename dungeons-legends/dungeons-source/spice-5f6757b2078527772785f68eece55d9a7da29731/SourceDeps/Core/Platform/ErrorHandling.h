#pragma once

//#include "Core/Debug/DebugUtils.h"
#include <iostream>

#if defined(_WIN32) && !defined(PUBLISH)
#define OUTPUT_ERROR_FUNC(msg) OutputDebugStringA(msg); std::cout << msg << std::endl;
#elif defined(MCPE_PLATFORM_ANDROID) && !defined(PUBLISH)
#define OUTPUT_ERROR_FUNC(msg) __android_log_print( ANDROID_LOG_WARN, "MinecraftPE", "%s", msg );
#elif defined(__APPLE__) && !defined(PUBLISH)
#define OUTPUT_ERROR_FUNC(msg) std::cout << (msg);
#else
#define OUTPUT_ERROR_FUNC(msg) {}
#endif


#define MCECONSTRUCTDEBUGSTRING( message ) \
	std::ostringstream _message;    \
	_message << message << " @ " << __FUNCTION__ << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl;    \

#define MCENOSCOPEPRINT( message ) \
	MCECONSTRUCTDEBUGSTRING( message ) \
	OUTPUT_ERROR_FUNC(_message.str().c_str());

#define MCEPRINT( message ) { MCENOSCOPEPRINT( message ) }

#ifdef _DEBUG
#define MCEINFO( message ) MCEPRINT( message )
#else
#define MCEINFO( message ) {}
#endif

#if defined(_DEBUG) || defined(DEBUG)
#define MCEASSERTMESSAGE( boolAssertIfFalse, message ) MCECHECK( boolAssertIfFalse, message )
#define MCEASSERT( boolAssertIfFalse ) MCECHECK( boolAssertIfFalse, #boolAssertIfFalse )
#define MCEASSERTMESSAGEONLY( message ) MCEFAIL( message )

#define MCEGOTOASSERTHANDLER( message ) { \
		MCECONSTRUCTDEBUGSTRING( message ); \
		gp_assert_handler( _message.str().c_str(), "", "", __LINE__, __FILE__, "" ); }

#define MCECHECK( boolAssertIfFalse, message ) { \
		if( !(boolAssertIfFalse)) { \
			MCEGOTOASSERTHANDLER( message ) } }

#define MCEFAIL( message ) { \
		MCEGOTOASSERTHANDLER( message ) }

#define MCEWARN( message ) \
	MCEPRINT( message );

#define MCEASSERTWARNMESSAGE( boolPrintIfFalse, message ) { \
		if( !(boolPrintIfFalse)) { \
			MCEPRINT( message ); } }

#elif defined(PUBLISH)
#define MCEASSERTMESSAGE( boolPrintIfFalse, message ) (boolPrintIfFalse)
#define MCEASSERT( boolPrintIfFalse ) (boolPrintIfFalse)
#define MCEASSERTMESSAGEONLY( message )

#define MCEGOTOASSERTHANDLER( message )
#define MCECHECK( boolAssertIfFalse, message ) (boolAssertIfFalse)
#define MCEFAIL( message )
#define MCEWARN( message )
#define MCEASSERTWARNMESSAGE( boolPrintIfFalse, message ) (boolPrintIfFalse)
#else
#define MCEASSERTMESSAGE( boolPrintIfFalse, message ) MCEASSERTWARNMESSAGE( boolPrintIfFalse, message )
#define MCEASSERT( boolPrintIfFalse ) MCEASSERTWARNMESSAGE( boolPrintIfFalse, #boolPrintIfFalse )
#define MCEASSERTMESSAGEONLY( message ) MCEPRINT( message )

#define MCEGOTOASSERTHANDLER( message ) MCEASSERTWARNMESSAGE( message );
#define MCECHECK( boolAssertIfFalse, message ) { if( !(boolAssertIfFalse)) { MCEPRINT( message ); } \
}
#define MCEFAIL( message ) { MCEPRINT( message ); }
#define MCEWARN( message ) { MCEPRINT( message ); }
#define MCEASSERTWARNMESSAGE( boolPrintIfFalse, message ) { if( !(boolPrintIfFalse)) { MCEPRINT( message ); } \
}
#endif

#define MCEUNKOWNFAIL( UnknownThing ) MCEFAIL( "Unknown " #UnknownThing ": " << enum_cast( UnknownThing ))
#define MCEUNKNOWNWARN( UnknownThing ) MCEPRINT( "Unknown " #UnknownThing ": " << enum_cast( UnknownThing ))
