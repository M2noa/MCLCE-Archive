/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once
#include "DebugEventLogger.h"


#if defined(PUBLISH) && !defined(FORCE_LOGS_IN_PUBLISH)
#define __LOG_NULL(...) do { __VA_ARGS__; } while(0)
#define ALOGV(_area, fmt, ...) __LOG_NULL(__VA_ARGS__)
#define ALOGI(_area, fmt, ...) __LOG_NULL(__VA_ARGS__)
#define ALOGW(_area, fmt, ...) __LOG_NULL(__VA_ARGS__)
#define ALOGE(_area, fmt, ...) __LOG_NULL(__VA_ARGS__)

#define LOGV(fmt, ...) __LOG_NULL(__VA_ARGS__)
#define LOGI(fmt, ...) __LOG_NULL(__VA_ARGS__)
#define LOGW(fmt, ...) __LOG_NULL(__VA_ARGS__)
#define LOGE(fmt, ...) __LOG_NULL(__VA_ARGS__)
#if defined(ANDROID)
// @todo @fix; Obiously the tag shouldn't be hardcoded in here..
#define LOG_IN_PUBLISH(...) { ((void)__android_log_print( ANDROID_LOG_VERBOSE, "MinecraftPE", __VA_ARGS__ )); }
#define ALOG_IN_PUBLISH(_area, ...) { ((void)__android_log_print( ANDROID_LOG_VERBOSE, "MinecraftPE", __VA_ARGS__ )); }
#elif defined(MCPE_PLATFORM_STORE)
extern void DebugOut(LPCSTR pszFormat, ...);
#define LOG_IN_PUBLISH(...) {DebugOut(__VA_ARGS__); }
#define ALOG_IN_PUBLISH(_area, ...) {DebugOut(__VA_ARGS__); }
#elif defined(MCPE_PLATFORM_IOS)
extern void DebugOut(const char *message);
#define LOG_IN_PUBLISH(...) {(printf(__VA_ARGS__)); }
#define ALOG_IN_PUBLISH(_area, ...) {(printf(__VA_ARGS__)); }
#elif defined(WIN32)
#define LOG_IN_PUBLISH(...) { printf(__VA_ARGS__); }
#define ALOG_IN_PUBLISH(_area, ...) { printf(__VA_ARGS__); }
#else
#define LOG_IN_PUBLISH(...) {(printf(__VA_ARGS__)); }
#define ALOG_IN_PUBLISH(_area, ...) {(printf(__VA_ARGS__)); }
#endif

#else
#define LOG_IN_PUBLISH LOGV
#define ALOG_IN_PUBLISH ALOGV
#if defined(ANDROID)
#define printf LOGI
#elif defined(MCPE_PLATFORM_STORE)
extern void DebugOut(LPCSTR pszFormat, ...);
#elif defined(MCPE_PLATFORM_IOS)
extern void DebugOut(const char *message);
#elif defined(WIN32)
#include "AllowWindowsPlatformTypes.h"
const HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
const WORD white = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
const WORD notsowhite = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
const WORD yellow = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;
const WORD red = FOREGROUND_INTENSITY | FOREGROUND_RED;
#include "HideWindowsPlatformTypes.h"
#endif
#define ALOGV(_area, ...) {DebugLog::log(_area, LOG_PRIORITY_VERBOSE, __FUNCTION__, __LINE__, __VA_ARGS__);}
#define ALOGI(_area, ...) {DebugLog::log(_area, LOG_PRIORITY_INFO, __FUNCTION__, __LINE__, __VA_ARGS__);}
#define ALOGW(_area, ...) {DebugLog::log(_area, LOG_PRIORITY_WARN, __FUNCTION__, __LINE__, __VA_ARGS__);}
#define ALOGE(_area, ...) {DebugLog::log(_area, LOG_PRIORITY_ERROR, __FUNCTION__, __LINE__, __VA_ARGS__);}

#define LOGV(...) {DebugLog::log(LOG_AREA_UNKNOWN, LOG_PRIORITY_VERBOSE, __FUNCTION__, __LINE__, __VA_ARGS__);}
#define LOGI(...) {DebugLog::log(LOG_AREA_UNKNOWN, LOG_PRIORITY_INFO, __FUNCTION__, __LINE__, __VA_ARGS__);}
#define LOGW(...) {DebugLog::log(LOG_AREA_UNKNOWN, LOG_PRIORITY_WARN, __FUNCTION__, __LINE__, __VA_ARGS__);}
#define LOGE(...) {DebugLog::log(LOG_AREA_UNKNOWN, LOG_PRIORITY_ERROR, __FUNCTION__, __LINE__, __VA_ARGS__);}
#endif
