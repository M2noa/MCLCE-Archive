/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "client/game/ExternalServersFile.h"
//#include "network/NetworkIdentifier.h"


#include "Core/Utility/Filepath.h"
#if !defined(_WIN32)
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

#include "util/StringUtils.h"

ExternalServer::ExternalServer() 
	: mId(-1)
	, mPort(0)
	, mProtocol(0)
	, mPlayers(0)
	, mMaxPlayers(0)
	, mName("")
	, mTitle("")
	, mVersion("")
	, mAddress("") {
}

ExternalServer::ExternalServer(const int id, const std::string& name, const std::string& address, int port)
	: mId(id)
	, mPort(port)
	, mProtocol(0)
	, mPlayers(0)
	, mMaxPlayers(0)
	, mName(name)
	, mTitle("")
	, mVersion("")
	, mAddress(address)	{
	resolve();
}

void ExternalServer::resolve() {
	// TODO: rherlitz

	//mFutureIP = std::async(std::launch::async, [this] () -> std::string {
	//	if (Util::isValidIP(mAddress)) {
	//		return mAddress;
	//	}

	//	struct addrinfo* result;
	//	struct addrinfo* res;
	//	int error;
	//	error = getaddrinfo(mAddress.c_str(), nullptr, nullptr, &result);
	//	if (error == 0) {
	//		std::string ip;

	//		for (res = result; res != nullptr; res = res->ai_next) {
	//			if(res->ai_family == AF_INET6){
	//				ip = NetworkIdentifier(*(struct sockaddr_in6*)res->ai_addr).getAddress();
	//			}else{
	//				ip = NetworkIdentifier(*(struct sockaddr_in*)res->ai_addr).getAddress();
	//			}
	//		}
	//		freeaddrinfo(result);

	//		return ip;
	//	}
	//	else {
	//		return Util::EMPTY_STRING;
	//	}
	//});
}

static const auto maxWait = (std::chrono::milliseconds)1;

const std::string& ExternalServer::getIP() const {
	if (mFutureIP.wait_for(maxWait) != std::future_status::timeout) {
		return mFutureIP.get();
	}
	else {
		return Util::EMPTY_STRING;
	}
}

const int ExternalServer::getId() const {
	return mId;
}

const int ExternalServer::getProtocol() const {
	return mProtocol;
}

void ExternalServer::setProtocol(int protocal) {
	mProtocol = protocal;
}

const int ExternalServer::getPlayers() const {
	return mPlayers;
}

void ExternalServer::setPlayers(int players) {
	mPlayers = players;
}

const int ExternalServer::getMaxPlayers() const {
	return mMaxPlayers;
}

void ExternalServer::setMaxPlayers(int maxPlayers) {
	mMaxPlayers = maxPlayers;
}

const int ExternalServer::getPort() const {
	return mPort;
}

void ExternalServer::setPort(int port) {
	mPort = port;
}

const std::string& ExternalServer::getName() const{
	return mName;
}

void ExternalServer::setName(const std::string& name) {
	mName = name;
}

const std::string& ExternalServer::getTitle() const {
	return mTitle;
}

void ExternalServer::setTitle(const std::string& title){
	mTitle = title;
}

const std::string& ExternalServer::getVersion() const{
	return mVersion;
}

void ExternalServer::setVersion(const std::string& version) {
	mVersion = version;
}

const std::string& ExternalServer::getAddress() const{
	return mAddress;
}

void ExternalServer::setAddress(const std::string& address) {
	mAddress = address;
}

ExternalServerFile::ExternalServerFile(const std::string& folderPath) {
	mExternalServersFilePath = folderPath + "/external_servers.txt";
}

void ExternalServerFile::load() {
	//TODO should we replace this mess with a JSON file?
	mExternalServers.clear();
	FILE* pFile = port::fopen_mb(mExternalServersFilePath.c_str(), _FILE_STR("r"));
	if (pFile != nullptr) {
		char lineBuff[128];

		while (fgets(lineBuff, sizeof lineBuff, pFile)) {
			if (strlen(lineBuff) > 2) {
				std::stringstream stringParser(lineBuff);
				std::vector<std::string> parameters;

				while (!stringParser.eof()) {
					std::string destString;
					std::getline(stringParser, destString, ':');
					destString.erase(destString.find_last_not_of(" \n\r\t") + 1);
					if (destString == "") {
						break;
					}
					parameters.push_back(destString);
				}
				if (parameters.size() == 4) {
					int id = strtol(parameters[0].c_str(), nullptr, 0);
					std::string name = parameters[1];
					std::string address = parameters[2];
					int port = strtol(parameters[3].c_str(), nullptr, 0);
					if (port != 0 && id != 0) {
						mExternalServers[id] = make_unique<ExternalServer>(id, name, address, port);
					}
				}
			}
		}
		fclose(pFile);
	}
}

void ExternalServerFile::save() {
	FILE* pFile = port::fopen_mb(port::toFilePath(mExternalServersFilePath).c_str(), _FILE_STR("w"));
	if (pFile != nullptr) {
		for(auto&& server : mExternalServers) {
			fprintf(pFile, "%d:%s:%s:%d\n", server.second->getId(), server.second->getName().c_str(), server.second->getAddress().c_str(), server.second->getPort());
		}
		fclose(pFile);
	}
}

const std::unordered_map<int, Unique<ExternalServer> >& ExternalServerFile::getExternalServers() {
	return mExternalServers;
}

bool ExternalServerFile::addServer(const std::string& name, const std::string& address, int port) {
	int id = 1;

	for (; id < 60000; ++id) {
		auto server = mExternalServers.find(id);
		if (server == mExternalServers.end()) {
			break;
		}
		else if (server->second->getAddress() == address && server->second->getPort() == port) {
			return false;
		}
	}
	mExternalServers[id] = make_unique<ExternalServer>(id, name, address, port);
	save();
	return true;
}

void ExternalServerFile::editServer(int id, const std::string& name, const std::string& address, int port) {
	auto server = mExternalServers.find(id);
	if (server != mExternalServers.end()) {
		server->second->setName(name);
		server->second->setAddress(address);
		server->second->setPort(port);
	}
	save();
}

void ExternalServerFile::removeServer(int id) {
	auto server = mExternalServers.find(id);
	if (server != mExternalServers.end()) {
		mExternalServers.erase(server);
		save();
	}
}

std::tuple<std::string, std::string, int> ExternalServerFile::getServerInfo(int id) {

	auto server = mExternalServers.find(id);
	if (server != mExternalServers.end()) {
		return std::make_tuple(server->second->getName(), server->second->getAddress(), server->second->getPort());
	}
	else {
		return std::make_tuple("", "", 0);
	}
}
