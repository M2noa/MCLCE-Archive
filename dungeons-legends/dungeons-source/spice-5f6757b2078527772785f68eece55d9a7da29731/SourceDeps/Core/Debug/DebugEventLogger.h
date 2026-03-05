/********************************************************
*   (C) Mojang. All rights reserved                       *
*   (C) Microsoft. All rights reserved.                   *
*********************************************************/

#pragma once

#define LOG_PRIORITY_VERBOSE 1 << 0
#define LOG_PRIORITY_INFO    1 << 1
#define LOG_PRIORITY_WARN    1 << 2
#define LOG_PRIORITY_ERROR   1 << 3
#define LOG_PRIORITY_UNKNOWN 1 << 30
#define LOG_PRIORITY_ALL     0xFFFFFFFF

#define LOG_AREA_PLATFORM    1 << 0
#define LOG_AREA_ENTITY      1 << 1
#define LOG_AREA_DATABASE    1 << 2
#define LOG_AREA_GUI         1 << 3
#define LOG_AREA_SYSTEM      1 << 4
#define LOG_AREA_NETWORK     1 << 5
#define LOG_AREA_RENDER      1 << 6
#define LOG_AREA_MEMORY      1 << 7
#define LOG_AREA_ANIMATION   1 << 8
#define LOG_AREA_INPUT       1 << 9
#define LOG_AREA_LEVEL       1 << 10
#define LOG_AREA_SERVER      1 << 11
#define LOG_AREA_CONTAINER   1 << 12
#define LOG_AREA_PHYSICS     1 << 13
#define LOG_AREA_FILE        1 << 14
#define LOG_AREA_STORAGE     1 << 15
#define LOG_AREA_REALMS      1 << 16
#define LOG_AREA_REALMSAPI   1 << 17
#define LOG_AREA_XBOXLIVE    1 << 18
#define LOG_AREA_USERMANAGER 1 << 19
#define LOG_AREA_XSAPI       1 << 20
#define LOG_AREA_PERF        1 << 21
#define LOG_AREA_TELEMETRY   1 << 22
#define LOG_AREA_BLOCKS      1 << 23
#define LOG_AREA_RAKNET      1 << 24
#define LOG_AREA_UNUSED4     1 << 25
#define LOG_AREA_UNUSED5     1 << 26
#define LOG_AREA_UNUSED6     1 << 27
#define LOG_AREA_UNUSED7     1 << 28
#define LOG_AREA_UNUSED8     1 << 29
#define LOG_AREA_UNKNOWN     1 << 30
#define LOG_AREA_ALL         0xFFFFFFFF

class LogSettingsUpdater;

namespace DebugLog {

	void createLog(const std::string& _logFilePath, bool _flushImmediate = false, bool _timestamp = false,
		bool _trace = false,  bool _append = false, unsigned int _priorityFilter = 0xFFFFFFFF, unsigned int _areaFilter = 0xFFFFFFFF);

	void flushLog();
	void closeLog();

	void log(unsigned int _area, unsigned int _priority, const char* _function, int line, const char * pszFormat, ...);
	void log_va(unsigned int _area, unsigned int _priority, const char* _function, int line, const char * pszFormat, va_list args);

	bool filterLog(unsigned int _area, unsigned int _priority);

	void updateLogSetting(const std::string& setting, bool newValue);
	bool updateLogSetting(LogSettingsUpdater& options, const std::string& setting);
	void updateLogFilter(std::unique_ptr<LogSettingsUpdater> options, const std::string& filterType, const std::vector<std::string>& filters, std::string& result);

	unsigned int priority(const std::string& filterString);
	unsigned int area(const std::string& filterString);
};
