#pragma once

#include <cppu/dtypes.h>

// Check windows
#if _WIN32 || _WIN64
#	if _WIN64
#		define ENVIRONMENT64
#	else
#		define ENVIRONMENT32
#	endif
#elif __GNUC__ // Check GCC
#	if __x86_64__ || __ppc64__
#		define ENVIRONMENT64
#	else
#		define ENVIRONMENT32
#	endif
#else
#	error "Not implemented"
#endif

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#	include <fcntl.h>
#	include <io.h>
#	define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#	define SET_BINARY_MODE(file)
#endif



#ifdef ESTEEM_EXPORT_DYNAMIC
#	define LIB_EXPORT __declspec(dllexport)
#elif ESTEEM_IMPORT_DYNAMIC
#	define LIB_EXPORT __declspec(dllimport)
#elif ESTEEM_EXPORT_STATIC
#	define LIB_EXPORT 
#else
#	define LIB_EXPORT 
#endif