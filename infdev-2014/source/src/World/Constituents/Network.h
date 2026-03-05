#pragma once

#include "stdafx.h"

#include "./AbstractConstituent.h"
#include "Network/NetworkSystem.h"
#include "Network/INetworkNode.h"

#include <cppu/net/Packet.h>

namespace Esteem
{
	class Network : public INetworkNode, public AbstractConstituent
	{
		friend class cgc::constructor;
	private:
		NetworkSystem& networkSystem;
		uint nodeId;
		bool enabled;
		bool receive;
		bool send;
		
		Network(const cgc::strong_ptr<Entity>& entity, NetworkSystem& networkSystem, uint nodeId)
			: AbstractConstituent(entity)
			, networkSystem(networkSystem)
			, nodeId(nodeId)
			, receive(true)
			, send(true)
		{
			Initialize();
		}

	public:
		static cgc::strong_ptr<Network> Instantiate(const cgc::strong_ptr<Entity>& entity, uint networkId);

		virtual void Initialize();

		virtual void Update();
		
		virtual uint GetNodeId() { return nodeId; }

		virtual NetworkSystem& GetNetworkSystem() { return networkSystem; }

		virtual void ReceivedData(const net::Packet& packet);

		virtual bool GetEnabled() const { return enabled; }
		virtual void SetEnabled(bool enable) { enabled = enable; }

		void EnableSend(bool enable) { send = enable; }
		void EnableReceive(bool enable) { receive = enable; }
	};
}