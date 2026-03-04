#pragma once

#include "stdafx.h"
#include <list>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unordered_map>

#include <cppu/cgc/m_array.h>
#include <cppu/net/UDPSocket.h>
#include <cppu/net/TCPListener.h>
#include <cppu/net/Packet.h>

#include "../World/Constituents/Network.h"

namespace cppu
{
	namespace net
	{
		class Packet;
	}
}

namespace Esteem
{
	class Entity;
	class Network;
	class INetworkNode;

	class NetworkSystem
	{
		friend class Network;
	private:
		bool enabled;
		bool connected;
		bool hosting;
		bool running;

		net::UDPSocket udpSocket;
		net::TCPSocket tcpClient;
		net::TCPListener tcpListener;

		std::thread thread;
		std::condition_variable threadWait;
		std::mutex threadLock;

		std::list<net::TCPSocket> clients;
		std::deque<net::TCPSocket> validatingClients;

		net::IPAddress ip;
		uint16 bindPort;
		uint16 sendPort;

		net::EndPoint broadcast;
		net::EndPoint peer;
		net::EndPoint host;

		bool udpBroadcast;
		float nextBroadcast;

		cgc::m_array<Network> networkConstituents;
		std::unordered_map<uint, INetworkNode*> networkNodes;

	public:
		NetworkSystem();
		~NetworkSystem();

		void Stop();

		void Connect(const std::string& ip, uint16 bindPort, uint16 sendPort);
		bool Host(const std::string& ip, uint16 bindPort, uint16 sendPort);

		bool IsConnected() { return connected; }
		void Send(const net::Packet& packet);
		
		void Register(INetworkNode* networkNode);
		void UnRegister(const INetworkNode* networkNode);
		void UnRegister(uint netConstituentID);

		void OnCommand(const std::string& command, const std::string& value);

		cgc::strong_ptr<Network> CreateConstituent(const cgc::strong_ptr<Entity>& entity, uint networkId);
		
		void UpdateAsync();
		void Update();
	};
}