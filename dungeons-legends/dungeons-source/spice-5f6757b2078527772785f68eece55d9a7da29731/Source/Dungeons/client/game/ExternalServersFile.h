/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once
#include "CommonTypes.h"

class ExternalServer {
public:
	ExternalServer();
	ExternalServer(const int id, const std::string& name, const std::string& address, int port);
	void resolve();

	const std::string& getIP() const;

	const int getId() const;

	const int getProtocol() const;
	void setProtocol(int protocal);

	const int getPlayers() const;
	void setPlayers(int players);

	const int getMaxPlayers() const;
	void setMaxPlayers(int maxPlayers);

	const int getPort() const;
	void setPort(int port);

	const std::string& getName() const;
	void setName(const std::string& name);

	const std::string& getTitle() const;
	void setTitle(const std::string& title);

	const std::string& getVersion() const;
	void setVersion(const std::string& version);

	const std::string& getAddress() const;
	void setAddress(const std::string& address);

protected:

	std::shared_future<std::string> mFutureIP;

private:

	const int mId;
	int mPort;
	int mProtocol;
	int mPlayers;
	int mMaxPlayers;
	std::string mName;
	std::string mTitle;
	std::string mVersion;
	std::string mAddress;

};

class ExternalServerFile {
public:

	ExternalServerFile(const std::string& folderPath);
	void load();
	void save();

	const std::unordered_map<int, Unique<ExternalServer> >& getExternalServers();
	bool addServer(const std::string& name, const std::string& address, int port);
	void editServer(int id, const std::string& name, const std::string& address, int port);
	void removeServer(int id);
	std::tuple<std::string, std::string, int> getServerInfo(int id);

protected:

	std::unordered_map<int, Unique<ExternalServer> > mExternalServers;
	std::string mExternalServersFilePath;
};
