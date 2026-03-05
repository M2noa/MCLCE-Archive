/********************************************************
*   (C) Mojang. All rights reserved                       *
*   (C) Microsoft. All rights reserved.                   *
*********************************************************/

#include "pch_core.h"
#include "DebugEventLogger.h"
#include "Core/Utility/Filepath.h"
#include "Core/Debug/Log.h"
#include "Core/Debug/LogSettingsUpdater.h"
// #include "Core/Debug/DebugUtils.h"

void DEFAULT_ASSERT_HANDLER(const char* desc, const char* arg, const char* info, int line, const char* file, const char* function) {
	LOGE("Assertion failed: %s\n", desc);
	LOGE("Condition is false: %s\n", arg);

	if (info) {
		LOGE("with %s\n", info);
	}

	LOGE("Function: %s in %s @ %d\n", function, file, line);
}
AssertHandlerPtr gp_assert_handler = DEFAULT_ASSERT_HANDLER;


namespace DebugLog {

	std::string logFileName;
	std::ofstream logFile;
	bool flushImmediate = false;
	bool timestamp = false;
	bool trace = false;
	bool append = false;
	unsigned int priorityFilter = 0xFFFFFFFF;
	unsigned int areaFilter = 0xFFFFFFFF;
	std::map<std::string, unsigned int> areaFilterMap;
	std::map<std::string, unsigned int> priorityFilterMap;

	namespace {

		std::string toUpper(const std::string& str) {
			std::string l = str;
			std::transform(l.begin(), l.end(), l.begin(), ::toupper);
			return l;
		}
	}

	void setFlushImmediate(bool _flushImmediate) {
		flushImmediate = _flushImmediate;
	}
	void setTimestamp(bool _timestamp) {
		timestamp = _timestamp;
	}
	void setTrace(bool _trace) {
		trace = _trace;
	}
	void setAppend(bool _append) {
		append = _append;
	}
	void togglePriorityFilter(unsigned int _filter) {
		if (priorityFilter & _filter) {
			priorityFilter = priorityFilter & ~_filter;
		}
		else {
			priorityFilter = priorityFilter | _filter;
		}
	}
	void toggleAreaFilter(unsigned int _filter) {
		if (areaFilter & _filter) {
			areaFilter = areaFilter & ~_filter;
		}
		else {
			areaFilter = areaFilter | _filter;
		}
	}

	const char * _priorityFilterString(unsigned int _priority) {
		switch (_priority) {
			case LOG_PRIORITY_VERBOSE: return "VERBOSE";
			case LOG_PRIORITY_INFO: return "INFO";
			case LOG_PRIORITY_WARN: return "WARN";
			case LOG_PRIORITY_ERROR: return "ERROR";
			case LOG_PRIORITY_ALL: return "ALL";
			case LOG_PRIORITY_UNKNOWN:
			default: return "UNKNOWN";
		};
	}

	const char * _areaFilterString(unsigned int _area) {
		switch (_area) {
			case LOG_AREA_PLATFORM: return "PLATFORM";
			case LOG_AREA_ENTITY: return "ENTITY";
			case LOG_AREA_DATABASE: return "DATABASE";
			case LOG_AREA_GUI: return "GUI";
			case LOG_AREA_SYSTEM: return "SYSTEM";
			case LOG_AREA_NETWORK: return "NETWORK";
			case LOG_AREA_RENDER: return "RENDER";
			case LOG_AREA_MEMORY: return "MEMORY";
			case LOG_AREA_ANIMATION: return "ANIMATION";
			case LOG_AREA_INPUT: return "INPUT";
			case LOG_AREA_LEVEL: return "LEVEL";
			case LOG_AREA_SERVER: return "SERVER";
			case LOG_AREA_CONTAINER: return "CONTAINER";
			case LOG_AREA_PHYSICS: return "PHYSICS";
			case LOG_AREA_FILE: return "FILE";
			case LOG_AREA_STORAGE: return "STORAGE";
			case LOG_AREA_REALMS: return "REALMS";
			case LOG_AREA_REALMSAPI: return "REALMSAPI";
			case LOG_AREA_XBOXLIVE: return "XBOXLIVE";
			case LOG_AREA_USERMANAGER: return "USERMANAGER";
			case LOG_AREA_XSAPI: return "XSAPI";
			case LOG_AREA_PERF: return "PERF";
			case LOG_AREA_TELEMETRY: return "TELEMETRY";
			case LOG_AREA_RAKNET: return "RAKNET";
			case LOG_AREA_ALL: return "ALL";
			case LOG_AREA_UNKNOWN:
			default: return "UNKNOWN";
		};
	}

	void _initPriorityFilterMap() {
		priorityFilterMap[_priorityFilterString(LOG_PRIORITY_VERBOSE)] = LOG_PRIORITY_VERBOSE;
		priorityFilterMap[_priorityFilterString(LOG_PRIORITY_INFO)] = LOG_PRIORITY_INFO;
		priorityFilterMap[_priorityFilterString(LOG_PRIORITY_WARN)] = LOG_PRIORITY_WARN;
		priorityFilterMap[_priorityFilterString(LOG_PRIORITY_ERROR)] = LOG_PRIORITY_ERROR;
		priorityFilterMap[_priorityFilterString(LOG_PRIORITY_ALL)] = LOG_PRIORITY_ALL;
		priorityFilterMap[_priorityFilterString(LOG_PRIORITY_UNKNOWN)] = LOG_PRIORITY_UNKNOWN;
	}

	void _initAreaFilterMap() {
		areaFilterMap[_areaFilterString(LOG_AREA_PLATFORM)] = LOG_AREA_PLATFORM;
		areaFilterMap[_areaFilterString(LOG_AREA_ENTITY)] = LOG_AREA_ENTITY;
		areaFilterMap[_areaFilterString(LOG_AREA_DATABASE)] = LOG_AREA_DATABASE;
		areaFilterMap[_areaFilterString(LOG_AREA_GUI)] = LOG_AREA_GUI;
		areaFilterMap[_areaFilterString(LOG_AREA_SYSTEM)] = LOG_AREA_SYSTEM;
		areaFilterMap[_areaFilterString(LOG_AREA_NETWORK)] = LOG_AREA_NETWORK;
		areaFilterMap[_areaFilterString(LOG_AREA_RENDER)] = LOG_AREA_RENDER;
		areaFilterMap[_areaFilterString(LOG_AREA_MEMORY)] = LOG_AREA_MEMORY;
		areaFilterMap[_areaFilterString(LOG_AREA_ANIMATION)] = LOG_AREA_ANIMATION;
		areaFilterMap[_areaFilterString(LOG_AREA_INPUT)] = LOG_AREA_INPUT;
		areaFilterMap[_areaFilterString(LOG_AREA_LEVEL)] = LOG_AREA_LEVEL;
		areaFilterMap[_areaFilterString(LOG_AREA_SERVER)] = LOG_AREA_SERVER;
		areaFilterMap[_areaFilterString(LOG_AREA_CONTAINER)] = LOG_AREA_CONTAINER;
		areaFilterMap[_areaFilterString(LOG_AREA_PHYSICS)] = LOG_AREA_PHYSICS;
		areaFilterMap[_areaFilterString(LOG_AREA_FILE)] = LOG_AREA_FILE;
		areaFilterMap[_areaFilterString(LOG_AREA_STORAGE)] = LOG_AREA_STORAGE;
		areaFilterMap[_areaFilterString(LOG_AREA_REALMS)] = LOG_AREA_REALMS;
		areaFilterMap[_areaFilterString(LOG_AREA_REALMSAPI)] = LOG_AREA_REALMSAPI;
		areaFilterMap[_areaFilterString(LOG_AREA_XBOXLIVE)] = LOG_AREA_XBOXLIVE;
		areaFilterMap[_areaFilterString(LOG_AREA_USERMANAGER)] = LOG_AREA_USERMANAGER;
		areaFilterMap[_areaFilterString(LOG_AREA_XSAPI)] = LOG_AREA_XSAPI;
		areaFilterMap[_areaFilterString(LOG_AREA_PERF)] = LOG_AREA_PERF;
		areaFilterMap[_areaFilterString(LOG_AREA_TELEMETRY)] = LOG_AREA_TELEMETRY;
		areaFilterMap[_areaFilterString(LOG_AREA_RAKNET)] = LOG_AREA_RAKNET;
		areaFilterMap[_areaFilterString(LOG_AREA_ALL)] = LOG_AREA_ALL;
		areaFilterMap[_areaFilterString(LOG_AREA_UNKNOWN)] = LOG_AREA_UNKNOWN;
	}

	void _logToFile(const std::string& debugEvent) {
#ifndef PUBLISH
		if (!logFile.bad()) {
			if (logFile.rdbuf()->is_open()) {
				logFile << debugEvent.c_str();
				if (flushImmediate) {
					logFile.flush();
				}
			}
		}
#endif
	}

	unsigned int priority(const std::string& filterString) {
		std::map<std::string, unsigned int>::iterator iter = priorityFilterMap.find(filterString);
		if (iter != priorityFilterMap.end()) {
			return iter->second;
		}
		return LOG_PRIORITY_UNKNOWN;
	}

	unsigned int area(const std::string& filterString) {
		std::map<std::string, unsigned int>::iterator iter = areaFilterMap.find(filterString);
		if (iter != areaFilterMap.end()) {
			return iter->second;
		}
		return LOG_AREA_UNKNOWN;
	}

	void createLog(const std::string& _logFilePath, bool _flushImmediate, bool _timestamp,
		bool _trace, bool _append, unsigned int _priorityFilter, unsigned int _areaFilter) {
		_initAreaFilterMap();
		_initPriorityFilterMap();
#ifndef PUBLISH
		logFileName = _logFilePath + "/Debug_Log.txt";
		flushImmediate = _flushImmediate;
		timestamp = _timestamp;
		trace = _trace;
		append = _append;
		priorityFilter = _priorityFilter;
		areaFilter = _areaFilter;

		if ((logFile.bad()) || (!logFile.is_open())) {
			if (append) {
				logFile.open(port::toFilePath(logFileName), std::ofstream::app | std::ofstream::out);
			}
			else {
				logFile.open(port::toFilePath(logFileName), std::ofstream::trunc);
			}
			if (logFile.fail() || !logFile.is_open()) {
				ALOGE(LOG_AREA_SYSTEM, "FAILED to open log file %s %d\r\n", logFileName.c_str(), logFile.rdstate());
			}
			else {
				_logToFile(logFileName);
				ALOGV(LOG_AREA_SYSTEM, "opened log file %s\r\n", logFileName.c_str());
			}
		}
#endif
	}

	void flushLog() {
#ifndef PUBLISH
		if ((logFile.bad()) || (!logFile.is_open())) {
			return;
		}
		logFile.flush();
#endif
	}

	void closeLog() {
#ifndef PUBLISH
		if ((logFile.bad()) || (!logFile.is_open())) {
			return;
		}
		logFile.flush();
		logFile.close();
#endif
	}

	bool filterLog(unsigned int _area, unsigned int _priority) {
#ifndef PUBLISH
		if (logFile.bad() || !logFile.rdbuf()->is_open()) {
			return false;
		}
#endif
		if (!(priorityFilter&_priority)) {
			return false;
		}

		if (!(areaFilter&_area)) {
			return false;
		}

		return true;
	}

	void log(unsigned int _area, unsigned int _priority, const char* _function, int line, const char * pszFormat, ...) {

		if (!filterLog(_area, _priority)) {
			return;
		}

		va_list argptr;
// 		va_start(argptr, pszFormat);

		log_va(_area, _priority, _function, line, pszFormat, argptr);

// 		va_end(argptr);
	}

	void log_va(unsigned int _area, unsigned int _priority, const char* _function, int line, const char * pszFormat, va_list argptr) {
		char szBuf[1024];
		char szUser[1024];

#if defined(MCPE_PLATFORM_WIN32) && !defined(PUBLISH)
		if (_priority == LOG_PRIORITY_VERBOSE) {
			SetConsoleTextAttribute(hConsole, notsowhite);
		}
		else if (_priority == LOG_PRIORITY_INFO) {
			SetConsoleTextAttribute(hConsole, white);
		}
		else if (_priority == LOG_PRIORITY_WARN) {
			SetConsoleTextAttribute(hConsole, yellow);
		}
		else {
			SetConsoleTextAttribute(hConsole, red);
		}
#endif

#if defined(MCPE_PLATFORM_WINDOWS_FAMILY)
		sprintf_s(szBuf, sizeof(szBuf), "[%d:%d] - %s %s ",
			GetCurrentProcessId(),
			GetCurrentThreadId(),
			_priorityFilterString(_priority),
			_areaFilterString(_area));

		if (timestamp) {
			SYSTEMTIME st;

			GetSystemTime(&st);

			sprintf_s(szUser, sizeof(szUser),
				" [%04d/%02d/%02d-%02d:%02d:%02d.%03d]",
				st.wYear,
				st.wMonth,
				st.wDay,
				st.wHour,
				st.wMinute,
				st.wSecond,
				st.wMilliseconds);

			strcat_s(szBuf, sizeof(szBuf) - strlen(szBuf), szUser);
		}

		if (trace) {
			sprintf_s(szUser, sizeof(szUser), " %s #%d ", _function, line);
			strcat_s(szBuf, sizeof(szBuf) - strlen(szBuf), szUser);
		}

		std::string out(szBuf);
		// note: vsnprintf returns actual size that the string _could_ be.
		int len = vsnprintf_s(szUser, sizeof(szUser), _TRUNCATE, pszFormat, argptr);
		if (len != -1) {
			out += szUser;
		}
		else {
			//vsnprintf_s doesn't return the required length unlike vsnprintf so we need to get it the hard way
			int requiredLen = _vscprintf(pszFormat, argptr);
			// szUser is too small for result, enlarge buffer
			auto sz = out.size();
			out.append(requiredLen + 1, ' ');
			vsprintf_s(out._Myptr() + sz, requiredLen + 1, pszFormat, argptr);
		}
#if defined(MCPE_PLATFORM_STORE)
		DebugOut("%s", out.c_str());
#elif defined(MCPE_PLATFORM_WIN32)
		printf("%s", out.c_str());
#endif
#ifndef PUBLISH
		_logToFile(out);
#endif

#else // !(defined(MCPE_PLATFORM_WIN32) && !defined(PUBLISH))
		sprintf(szBuf, "%s %s ", _priorityFilterString(_priority), _areaFilterString(_area));

		if (timestamp) {
			time_t ltime;
			time(&ltime);

			sprintf(szUser, " [%s]", ctime(&ltime));

			strcat(szBuf, szUser);
		}

		if (trace) {
			sprintf(szUser, " %s #%d ", _function, line);
			strcat(szBuf, szUser);
		}

		std::string out(szBuf);
		// note: vsnprintf returns actual size that the string _could_ be.
		vsnprintf(szUser, sizeof(szUser), pszFormat, argptr);
		out += szUser; // note that this will clip the string if it is > szUser. Unfortunately the _Myptr() function isn't available everywhere
#if defined(ANDROID)
		char el[8] = "\r\n";
		out += el;

		if (_priority == LOG_PRIORITY_VERBOSE) {
			__android_log_print(ANDROID_LOG_VERBOSE, "MinecraftPE", "%s\r\n", out.c_str());
		}
		else if (_priority == LOG_PRIORITY_INFO) {
			__android_log_print(ANDROID_LOG_INFO, "MinecraftPE", "%s\r\n", out.c_str());
		}
		else if (_priority == LOG_PRIORITY_WARN) {
			__android_log_print(ANDROID_LOG_WARN, "MinecraftPE", "%s\r\n", out.c_str());
		}
		else {
			__android_log_print(ANDROID_LOG_WARN, "MinecraftPE", "%s\r\n", out.c_str());
		}
#elif defined(MCPE_PLATFORM_IOS)
		DebugOut(out.c_str());
#else
		printf("%s", out.c_str());
		// TODO: rherlitz
		std::cout << out << std::endl;
		//UE_LOG(LogTemp, Warning, TEXT("Running Constructor! AVoxel::AVoxel"));
#endif
#ifndef PUBLISH
		_logToFile(out);
#endif
#endif
	}

	void updateLogSetting(const std::string& setting, bool newValue) {
		if (strcmp(setting.c_str(), "append") == 0) {
			// toggles file append/trunc
			setAppend(newValue);
		}
		else if (strcmp(setting.c_str(), "flush") == 0) {
			setFlushImmediate(newValue);
		}
		else if (strcmp(setting.c_str(), "timestamp") == 0) {
			// adds timestamp
			setTimestamp(newValue);
		}
		else if (strcmp(setting.c_str(), "trace") == 0) {
			// adds line # and function name
			setTrace(newValue);
		}
	}

	bool updateLogSetting(LogSettingsUpdater& options, const std::string& setting) {
		if (strcmp(setting.c_str(), "append") == 0) {
			// toggles file append/trunc
			options.setDevLogAppend(!options.getDevLogAppend());
			bool newValue = options.getDevLogAppend();
			setAppend(newValue);
			options.save();
			return newValue;
		}
		else if (strcmp(setting.c_str(), "flush") == 0) {
			options.setDevLogFlushImmediate(!options.getDevLogFlushImmediate());
			bool newValue = options.getDevLogFlushImmediate();
			setFlushImmediate(newValue);
			options.save();
			return newValue;
		}
		else if (strcmp(setting.c_str(), "timestamp") == 0) {
			// adds timestamp
			options.setDevLogTimestamp(!options.getDevLogTimestamp());
			bool newValue = options.getDevLogTimestamp();
			setTimestamp(newValue);
			options.save();
			return newValue;
		}
		else if (strcmp(setting.c_str(), "trace") == 0) {
			// adds line # and function name
			options.setDevLogTrace(!options.getDevLogTrace());
			bool newValue = options.getDevLogTrace();
			setTrace(newValue);
			options.save();
			return newValue;
		}

		DEBUG_FAIL("Invalid log action specified.");
		return false;
	}

	void updateLogFilter(std::unique_ptr<LogSettingsUpdater> options, const std::string& filterType, const std::vector<std::string>& filters, std::string& result) {
		if (strcmp(filterType.c_str(), "area") == 0) {
			// development area filters
			for (auto&& filter : filters) {
				const std::string areaFilterStr = toUpper(filter);
				const unsigned int areaVal = DebugLog::area(areaFilterStr);
				if (areaVal != LOG_AREA_UNKNOWN) {
					DebugLog::toggleAreaFilter(areaVal);
					if (options != nullptr) {
						options->toggleDevLogAreaFilter(areaVal);
						options->save();
					}

					std::string tagE = "log area " + areaFilterStr + " enabled\n";
					std::string tagD = "log area " + areaFilterStr + " disabled\n";
					result.append((areaFilter & areaVal) ? tagE : tagD);
				}
			}
		}
		else if (strcmp(filterType.c_str(), "priority") == 0) {
			// priority filters
			for (auto&& filter : filters) {
				const std::string priFilter = toUpper(filter);
				const unsigned int pri = DebugLog::priority(priFilter);
				if (pri != LOG_PRIORITY_UNKNOWN) {
					DebugLog::togglePriorityFilter(pri);
					if (options != nullptr) {
						options->toggleDevLogPriorityFilter(pri);
						options->save();
					}

					std::string tagE = "log pri " + priFilter + " enabled\n";
					std::string tagD = "log pri " + priFilter + " disabled\n";
					result.append((priorityFilter & pri) ? tagE : tagD);
				}
			}
		}
	}
}
