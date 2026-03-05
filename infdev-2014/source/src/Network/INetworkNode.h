#pragma once

#include "stdafx.h"

namespace cppu
{
	namespace net
	{
		class Packet;
	}
}

namespace Esteem
{
	class NetworkSystem;

	class INetworkNode
	{
	public:
		virtual ~INetworkNode() {}

		virtual uint GetNodeId() = 0;
		virtual NetworkSystem& GetNetworkSystem() = 0;
		virtual void ReceivedData(const net::Packet& packet) = 0;
		//virtual void SendData(void* data, uint& size) = 0;

		virtual void Update() = 0;
	};
}