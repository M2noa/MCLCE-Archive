#include "NetworkSystem.h"

#include <algorithm>

#include "Utils/Time.h"
#include "Utils/Debug.h"
#include "Utils/Hash.h"
#include "General/Command.h"

#include "World/Constituents/Network.h"

#include "INetworkNode.h"

#include <cppu/net/Packet.h>

namespace Esteem
{
	NetworkSystem::NetworkSystem()
		: broadcast(net::IPAddress("255.255.255.255"), 2501)
		, threadLock()
		, threadWait()
		, running(true)
	{
		//Host("255.255.255.255", 2500);

		Command::RegisterListener("connect", DELEGATE(&NetworkSystem::OnCommand, this));
		Command::RegisterListener("host", DELEGATE(&NetworkSystem::OnCommand, this));
		
		thread = std::thread(&NetworkSystem::Update, this);

		//net::Start();
	}

	NetworkSystem::~NetworkSystem()
	{
		tcpListener.Close();
		for (auto it = clients.begin(); it != clients.end(); ++it)
			it->Close();

		udpSocket.Close();
		tcpClient.Close();

		running = false;
		threadWait.notify_all();
		thread.join();

		Stop();
	}

	void NetworkSystem::Stop()
	{
		//net::Stop();
	}

	void NetworkSystem::Connect(const std::string& ip, uint16 bindPort, uint16 sendPort)
	{
		hosting = false;

		this->ip = net::IPAddress(ip); // has implicit string to sf::IpAddress caster
		this->bindPort = bindPort;
		this->sendPort = sendPort;

		//udpSocket.(false);
		//tcpClient.setBlocking(false);

		if (udpSocket.Bind(net::EndPoint(this->bindPort)))
			Debug::Log("UDP: could not bind port: " + std::to_string(this->bindPort));
		else
			udpSocket.SetBlocking(false);
	}

	void NetworkSystem::Send(const net::Packet& packet)
	{
		udpSocket.Send(peer, packet.GetData(), packet.GetPacketSize());
	}

	bool NetworkSystem::Host(const std::string& ip, uint16 bindPort, uint16 sendPort)
	{
		hosting = true;

		host = net::EndPoint(net::IPAddress(ip), bindPort);

		this->ip = net::IPAddress(ip);
		this->bindPort = bindPort;
		this->sendPort = sendPort;

		net::ErrorCode error;
		if (error = tcpListener.Listen(host) != net::Error::NONE)
		{
			Debug::Log("TCP: could not bind port: " + std::to_string(host.GetPort()) + " for listening, error code: " + std::to_string(error));
			tcpListener.Close();
			return false;
		}
		else
			tcpListener.SetBlocking(false);

		if (error = udpSocket.Bind(host) != net::Error::NONE)
		{
			Debug::Log("UDP: could not bind port: " + std::to_string(host.GetPort()) + ", error code: " + std::to_string(error));
			udpSocket.Close();
			tcpListener.Close();
			return false;
		}
		else
			udpSocket.SetBlocking(false);

		udpBroadcast = true;
		return true;
	}

	void NetworkSystem::Register(INetworkNode* networkNode)
	{
		if (networkNode != nullptr)
		{
			uint requestId = networkNode->GetNodeId();

			auto found = networkNodes.find(requestId);
			if (found == networkNodes.end())
				networkNodes[requestId] = networkNode;
		}
	}

	void NetworkSystem::UnRegister(const INetworkNode* networkNode)
	{
		auto found = std::find_if(networkNodes.begin(), networkNodes.end(),
			[&networkNode](std::pair<const uint, INetworkNode*>& it) -> bool { return it.second == networkNode; });

		if (found != networkNodes.end())
			networkNodes.erase(found);
	}

	void NetworkSystem::UnRegister(uint networkNodeId)
	{
		auto found = networkNodes.find(networkNodeId);
		if (found != networkNodes.end())
			networkNodes.erase(found);
	}

	void NetworkSystem::UpdateAsync()
	{
		threadWait.notify_one();
	}

	void NetworkSystem::Update()
	{
		while (running)
		{
			{
				std::unique_lock<std::mutex> lk(threadLock);
				threadWait.wait(lk);
			}

			if (hosting)
			{
				if (udpBroadcast)
				{
					nextBroadcast -= Time::RenderDeltaTime();
					if (nextBroadcast <= 0)
					{
						nextBroadcast += 1.f;
						char message[8] = "hosting";
						//udpSocket->SendTo(message, 8, broadcast);

						net::TCPSocket socket;
						while (tcpListener.Accept(socket))
						{
							socket.SetBlocking(false);
							validatingClients.emplace_back(std::move(socket));
						}

						if (!validatingClients.empty())
						{
							for (auto it = validatingClients.begin(); it != validatingClients.end();)
							{
								net::TCPSocket& socket = *it;

								// receive data
								const std::size_t packetSize = 1024;
								std::vector<char> data;
								data.resize(packetSize);

								std::size_t actualPacketSize;
								if (actualPacketSize = socket.Receive(&data[0], packetSize))
								{
									if (actualPacketSize == 4)
									{
										this->connected = true;
										this->peer = socket.GetRemoteEndPoint();

										Debug::Log("Host: connected to " + (std::string)this->peer.GetAddress() + ":" + std::to_string(this->peer.GetPort()));

										clients.emplace_back(std::move(socket));
										it = validatingClients.erase(it);

										continue;
									}
								}
								++it;
							}
						}
					}
				}
			}
			else
			{
				if (!connected)
				{
					char message[8];
					uint receivedSize;
					//uint16 port;

					// auto connect enabled
					// TODO: remove this part, testing purposes only
					if (udpSocket.DataAvailable())
					{
						net::EndPoint endPoint;
						receivedSize = udpSocket.Receive(message, 8, endPoint);
						if (strcmp(message, "hosting") == 0)
						{
							peer = endPoint;
							if (tcpClient.Connect(peer) == net::Error::NONE)
							{
								Debug::Log("TEST");

								this->connected = tcpClient.IsOpen();
								if (this->connected)
									Debug::Log("Client: connected to " + (std::string)this->peer.GetAddress() + ":" + std::to_string(this->peer.GetPort()));
							}
						}
					}
				}
			}

			if (connected)
			{
				// send data;
				for (auto it = networkNodes.begin(); it != networkNodes.end(); ++it)
					it->second->Update();

				// receive data
				const uint16 packetSize = 1024;
				std::vector<char> data;
				data.resize(packetSize);

				size_t size;
				//uint16 port;

				while (size = udpSocket.Receive(data.data(), packetSize))
				{
					net::Packet packet(std::move(std::vector<char>(data.begin(), data.begin() + size)));

					uint id = packet.GetValue<uint>();
					if (networkNodes.count(id) > 0)
						networkNodes[id]->ReceivedData(packet);
				}
			}

			//std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}
	}

	void NetworkSystem::OnCommand(const std::string& command, const std::string& value)
	{
		hash_t hash = RT_HASH(command);
		switch (hash)
		{
		case CT_HASH("connect"):
		{
			hosting = false;
			std::size_t offset = value.find(':');
			if (offset != value.npos)
			{
				try
				{
					std::string address = value.substr(0, offset);
					std::string port = value.substr(offset + 1);

					peer = net::EndPoint(net::IPAddress(address), std::stoi(port));
					if (tcpClient.Connect(peer) == net::Error::NONE)
					{

						this->host = tcpClient.GetLocalEndPoint();
						if (udpSocket.Bind(this->host) != net::Error::NONE)
							Debug::Log("UDP: could not bind port: " + std::to_string(this->host.GetPort()));
						else
							udpSocket.SetBlocking(false);

						uint port = this->peer.GetPort();
						tcpClient.Send(&port, 4);
						tcpClient.SetBlocking(false);

						Debug::Log("Client: connected to " + (std::string)this->peer.GetAddress() + ":" + std::to_string(this->peer.GetPort()));
					}
					else
						Debug::Log("Client: could not connect to " + (std::string)this->peer.GetAddress() + ":" + std::to_string(this->peer.GetPort()));

					this->connected = tcpClient.IsOpen();

					/*tcpClient.ConnectAsync(peer, [this](net::TCPSocket* socket, net::ErrorCode error)
					{
						net::TCPSocket* tcpClient = static_cast<net::TCPSocket*>(socket);

						this->connected = socket->IsOpen();
						if (this->connected)
						{

							this->host = tcpClient->GetLocalEndPoint();
							if (!udpSocket.Bind(this->host))
								Debug::Log("UDP: could not bind port: " + std::to_string(this->host.GetPort()));

							uint port = this->peer.GetPort();
							tcpClient->Send(&port, 4);

							Debug::Log("Client: connected to " + (std::string)this->peer.GetAddress() + ":" + std::to_string(this->peer.GetPort()));
						}
						else
							Debug::Log("Client: could not connect to " + (std::string)this->peer.GetAddress() + ":" + std::to_string(this->peer.GetPort()));
					});*/
				}
				catch (std::exception e)
				{
					Debug::LogError(value + " contains an incorrect ip:port");
				}
			}
		}
		break;
		case CT_HASH("host"):
			try
			{
				hosting = true;
				this->sendPort = std::stoi(value);
				net::IPAddress address = net::IPAddress::GetLocalAddress();
				if (Host(address, this->sendPort, 2501))
					Debug::Log("Host: hosting on: " + (std::string)address + ":" + value);
			}
			catch (std::exception e)
			{
				Debug::LogError(value + " contains an incorrect port " + e.what());
			}
			break;
		}
	}

	cgc::strong_ptr<Network> NetworkSystem::CreateConstituent(const cgc::strong_ptr<Entity>& entity, uint networkId)
	{
		return networkConstituents.emplace(entity, *this, networkId);
	}
}