// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef _PCH_CORE_H_
#define _PCH_CORE_H_

//do not warn for library files
#ifndef _MSC_VER
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wshadow"
#endif

//#ifdef _MSC_VER
//#define VC_EXTRALEAN			// Exclude rarely-used stuff
//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
//#define NOMINMAX				//- Macros min(a,b) and max(a,b)
//
//#include <windows.h>
//#include <winapifamily.h>
//#include <winuser.h>
//
//// http
//#include <msxml6.h>
//
//// com
//#pragma warning(push)
//// wrl.h has some bugs in it, disable warnings for just wrl.h
//// 4265: 'class' : class has virtual functions, but destructor is not virtual
//// 4467: Usage of ATL attributes is deprecated
//#pragma warning(disable : 4265)
//#pragma warning(disable : 4467)
//#include <wrl.h>
//#pragma warning(pop)
//#include <wrl/client.h>
//
//#include <commdlg.h>
//#include <concrt.h>
//
//#endif

#if defined(__GNUC__) || defined(__clang__)
#define MCPE_COMPILER_GCC_CLANG
#endif

#if defined(_MSC_VER)
#define MCPE_COMPILER_MSVC
#endif

#if defined(ANDROID) || defined(__ANDROID__)

#include <jni.h>
#include <errno.h>

#include <string.h>
#include <unistd.h>
#include <sys/resource.h>

#include <android/sensor.h>

#include <android/log.h>
#ifndef MCPE_PLATFORM_ANDROID
#define MCPE_PLATFORM_ANDROID
#endif

#endif

//#include <atomic>
#include <algorithm>
#include <array>
#include <bitset>
#include <cctype>
#include <locale>

// GCC/Clang doesn't support codecvt yet??!?
#if !defined(MCPE_COMPILER_GCC_CLANG)
#include <codecvt>
#define MCPE_ALLOW_CODE_CVT
#endif


#ifdef MCPE_COMPILER_MSVC
#include <filesystem>
#endif
//#include <chrono>
//#include <condition_variable>
//#include <deque>
#include <iomanip>
#include <iostream>
#include <fstream>
//#include <list>
//#include <functional>
//#include <limits>
//#include <locale>
#include <map>
//#include <mutex>
//#include <memory>
//#include <queue>
//#include <regex>
//#include <utility>
//#include <stack>
#include <sstream>
//#include <set>
//#include <string>
#include <thread>
#include <unordered_set>
//#include <unordered_map>
#include <vector>
//
//#include <cassert>
//#include <cctype>
//#include <cstring>
//#include <cfloat>
//#include <cmath>
//#include <cstddef>
//#include <cstdio>
//#include <ctime>
//#include <cstdlib>

#define MCE_UNREFERENCED_VARIABLE( localVariable ) (void)localVariable;

// These are purely for suggesting to intellisense that these macros exist
// DO NOT DEFINE THESE YOURSELF!
// They are defined in project .props files
#if 0
#define MCPE_PLATFORM_WIN32
#define MCPE_PLATFORM_DESKTOP
#define MCPE_PLATFORM_WINSTORE
#define MCPE_PLATFORM_WIN10UAP
#define MCPE_PLATFORM_STORE
#define MCPE_PLATFORM_WINRT
#define MCPE_PLATFORM_ANDROID
#define MCPE_PLATFORM_IOS
#define MCPE_PLATFORM_OSX
#define MCPE_PLATFORM_APPLE
#define MCPE_ARCH_ARM
#define MCPE_PLATFORM_WINDOWS_FAMILY

#define MCPE_BIT_32
#define MCPE_BIT_64

#define MCPE_ARCH_X86	// this is x86 and x64, meaning its Intel/AMD based instruction sets
#define MCPE_ARCH_ARM

#define MCPE_DEBUG
#define MCPE_RELEASE
#define MCPE_PUBLISH

#define MCPE_COMPILER_GCC_CLANG
#define MCPE_COMPILER_MSVC
#endif

// TODO: Move these somewhere else
#if !defined(PUBLISH)
#define ENABLE_CHEATS
#endif

#if defined(MCPE_PLATFORM_STORE) && !defined(PUBLISH)
#define USE_STORE_SIMULATOR
#endif

#if defined(MCPE_PLATFORM_IOS) || defined(MCPE_PLATFORM_ANDROID)
#define MCPE_POCKET_EDITION
#endif

using std::make_unique;

//#include <glm/glm.hpp>
//#include <glm/gtc/type_ptr.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/epsilon.hpp>
//#include <glm/gtx/rotate_vector.hpp>

#include "Core/Utility/PrimitiveTypes.h"

#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif

#ifndef UNUSED_PARAMETER
#if defined(MCPE_COMPILER_MSVC)
#define UNUSED_PARAMETER(...) __VA_ARGS__;
#else
#define UNUSED_PARAMETER(...)
#endif
#endif

#define UNUSED1(a)                  (void)(a)
#define UNUSED2(a,b)                (void)(a),UNUSED1(b)
#define UNUSED3(a,b,c)              (void)(a),UNUSED2(b,c)
#define UNUSED4(a,b,c,d)            (void)(a),UNUSED3(b,c,d)
#define UNUSED5(a,b,c,d,e)          (void)(a),UNUSED4(b,c,d,e)
#define UNUSED6(a,b,c,d,e,f)        (void)(a),UNUSED5(b,c,d,e,f)

#define VA_NUM_ARGS_IMPL(_1,_2,_3,_4,_5, _6, N,...) N
#define VA_NUM_ARGS(...) VA_NUM_ARGS_IMPL(__VA_ARGS__, 6, 5, 4, 3, 2, 1)

#define ALL_UNUSED_IMPL_(nargs) UNUSED ## nargs
#define ALL_UNUSED_IMPL(nargs) ALL_UNUSED_IMPL_(nargs)
#define ALL_UNUSED(...)

#ifndef UNUSED_VARIABLE
#if defined(MCPE_COMPILER_MSVC) 
#define UNUSED_VARIABLE(...) __VA_ARGS__;
#elif defined (MCPE_PLATFORM_ANDROID)
#define UNUSED_VARIABLE(...) __VA_ARGS__;
#elif defined(MCPE_PLATFORM_APPLE)
#define UNUSED_VARIABLE(...) ALL_UNUSED_IMPL( VA_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__ );
#else
#define UNUSED_VARIABLE(...)
#endif
#endif

#endif

// TODO: rherlitz, from debugutils
///the assertion handler type
typedef void(*AssertHandlerPtr)(const char* desc, const char* arg, const char* info, int line, const char* file, const char* function);

///the globally define assertion handler pointer
extern AssertHandlerPtr gp_assert_handler;


#define REQUIRES_PC_CHUNK static_assert(CHUNK_WIDTH == 16 && CHUNK_DEPTH == 16, "This code breaks if chunks are not 16x16")

#ifndef PUBLISH

#define DEBUG_ASSERT_IMPL( T, MSG, INFO ) { \
	if (!(T)) { \
		gp_assert_handler( MSG, #T, INFO, __LINE__, __FILE__, __FUNCTION__ ); \
	} \
}
#define DEBUG_ASSERT_INFO( T, MSG, INFO )   DEBUG_ASSERT_IMPL( T, MSG, (INFO).ASCII().c_str())
#define DEBUG_ASSERT( T, MSG )              DEBUG_ASSERT_IMPL( T, MSG, nullptr )
#define DEBUG_ASSERT_N( T )                 DEBUG_ASSERT( T, "Internal error" )
// #define DEBUG_ASSERT_MAIN_THREAD            DEBUG_ASSERT( std::this_thread::get_id() == gMainThread, "This code should only be executed on the main thread" )
#define DEBUG_ASSERT_MAIN_THREAD {}

#define DEBUG_TODO DEBUG_ASSERT( false, "METHOD NOT IMPLEMENTED" )
#define DEBUG_FAIL( MSG ) DEBUG_ASSERT( false, MSG )

#define IF_DEBUG  if(true)

#define DEBUG_HACK {}

#else

//TODO turn these into warnings?

#define DEBUG_ASSERT_IMPL( T, MSG, INFO ) {}
#define DEBUG_ASSERT_INFO( T, MSG, INFO ) {}
#define DEBUG_ASSERT( T, MSG ) {}
#define DEBUG_ASSERT_N( T ) {}
#define DEBUG_TODO {}
#define DEBUG_FAIL( MSG ) {}
#define DEBUG_ASSERT_MAIN_THREAD {}

#define IF_DEBUG  if(false)

#define DEBUG_HACK static_assert(false, "Fix this hack dammit")

#endif
