#pragma once

//#include "BuildConfig.h"
#include "Core/pch_core.h"

//#ifndef MCPE_HEADLESS
//#include "Renderer/pch_renderer.h"
//#endif

//do not warn for library files
#ifndef _MSC_VER
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wshadow"
#endif

#ifdef WIN32
#else
#include <errno.h>
#include <unistd.h>
#endif

#if defined(LINUX) || defined(ANDROID) || defined(__APPLE__) || defined(POSIX)
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#endif
#ifdef MACOSX
#include <CoreServices/CoreServices.h>
#include <unistd.h>
#endif

#ifdef ANDROID
#include <android/log.h>
#endif

#ifdef __cplusplus

#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <set>
#include <unordered_set>
#include <chrono>
#include <unordered_map>
#include <map>
#include <list>
#include <utility>
#include <sstream>
#include <thread>
#include <limits>
#include <mutex>
#include <queue>
#include <deque>
#include <condition_variable>
#include <fstream>
#include <atomic>
#include <array>
#include <bitset>
#include <cmath>
#include <limits>
#include <bitset>
#include <future>

#include <cassert>
#include <cctype>
#include <cstring>
#include <cfloat>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <ctime>
#include <cstdlib>

#include <json/json.h>
#include <json/writer.h>

#include "AllowWindowsPlatformTypes.h"
#include <raknet/MessageIdentifiers.h>
#include <raknet/RakNetTypes.h>
#include <raknet/BitStream.h>
#include <raknet/PacketPriority.h>
#include <raknet/RakPeerInterface.h>
#include <raknet/GetTime.h>
#include <raknet/RakString.h>
#include <raknet/PacketPriority.h>
#include "HideWindowsPlatformTypes.h"

#include "util/SmallSet.h"
#include "util/MovePriorityQueue.h"
#include "util/Math.h"
#include "util/StringUtils.h"
#include "util/StringHash.h"

#include "Core/Resource/ResourceUtil.h"
#include "Core/Utility/EnumCast.h"
#include "Core/Platform/ErrorHandling.h"

#endif

#define AUTO_VAR(_var, _val) auto _var = _val

#include <zlib/zlib.h>

//#include <glm/glm.hpp>
//#include "glm/gtc/type_ptr.hpp"
//#include "glm/gtc/matrix_transform.hpp"

//#ifdef WIN32
//#include <tchar.h>
//#include <shellapi.h>
//#include <direct.h>
//#include <io.h>
//#include <Psapi.h>
//
//// ppl
//#include <ppltasks.h>
//
//#ifdef MCPE_PLATFORM_WINRT
//// CX collections
//#include <collection.h>
//#endif
//#endif

#pragma warning(disable:4068) //allow unknown pragmas

#ifndef _MSC_VER
#pragma GCC diagnostic pop
#else
//enable additional pragmas on MSVC
#pragma warning(3:4062)	//incomplete switch
#pragma warning(3:4265)	//'class': class has virtual functions, but destructor is not virtual
#pragma warning(3:4296)	//expression is always false
#pragma warning(3:4701)	//use of uninitialized variable
#pragma warning(3:4457) //declaration of 'x' hides function parameter
#pragma warning(3:4456) //declaration of 'y' hides previous local declaration
//#pragma warning(3:4702) //unreachable code
#endif

//#include "Core/Debug/DebugUtils.h"

//#include "Renderer/HAL/Interface/ProfileSection.h"
