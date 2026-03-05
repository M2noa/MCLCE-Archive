#pragma once
using namespace std;

class ServerInterface
{
	virtual int getConfigInt(const wstring &name, int defaultValue) = 0;
	virtual wstring getConfigString(const wstring &name, const wstring &defaultValue) = 0;
	virtual bool getConfigBoolean(const wstring &name, bool defaultValue) = 0;
	virtual void setProperty(wstring &propertyName, void *value) = 0;
	virtual void configSave() = 0;
	virtual wstring getConfigPath() = 0;
	virtual wstring getServerIp() = 0;
	virtual int getServerPort() = 0;
	virtual wstring getServerName() = 0;
	virtual wstring getServerVersion() = 0;
	virtual int getPlayerCount() = 0;
	virtual int getMaxPlayers() = 0;
	virtual wstring[] getPlayerNames() = 0;
	virtual wstring getWorldName() = 0;
	virtual wstring getPluginNames() = 0;
	virtual void disablePlugin() = 0;
	virtual wstring runCommand(const wstring &command) = 0;
	virtual bool isDebugging() = 0;
	// Logging
	virtual void info(const wstring &string) = 0;
	virtual void warn(const wstring &string) = 0;
	virtual void error(const wstring &string) = 0;
	virtual void debug(const wstring &string) = 0;
};