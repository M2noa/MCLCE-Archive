/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

class LogSettingsUpdater {
public:
	virtual ~LogSettingsUpdater() {}

	virtual void save() = 0;

	virtual bool getDevLogAppend() const = 0;
	virtual void setDevLogAppend(bool value) = 0;

	virtual bool getDevLogFlushImmediate() const = 0;
	virtual void setDevLogFlushImmediate(bool value) = 0;

	virtual bool getDevLogTimestamp() const = 0;
	virtual void setDevLogTimestamp(bool value) = 0;

	virtual bool getDevLogTrace() const = 0;
	virtual void setDevLogTrace(bool value) = 0;

	virtual void toggleDevLogAreaFilter(unsigned int filter) = 0;
	virtual void toggleDevLogPriorityFilter(unsigned int filter) = 0;
};
