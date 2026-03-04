#include "Network.h"
#include "../Objects/Entity.h"

#include "Utils/Debug.h"

#include "World/World.h"

namespace Esteem
{
	cgc::strong_ptr<Network> Network::Instantiate(const cgc::strong_ptr<Entity>& entity, uint networkId)
	{
		return entity->GetWorld()->GetNetworkSystem().CreateConstituent(entity, networkId);
	}

#define entity GetEntity()

	void Network::Initialize()
	{
		networkSystem.Register(this);
	}

	void Network::Update()
	{
		if (send)
		{
			net::Packet packet;
			packet.AddValue<uint>(this->nodeId);
			packet.AddValue<Vector3>(entity->GetPosition());
			packet.AddValue<Quaternion>(entity->GetRotation());

			networkSystem.Send(packet);
		}
	}

	void Network::ReceivedData(const net::Packet& packet)
	{
		if (receive)
		{
			entity->SetPosition(packet.GetValue<Vector3>());
			entity->SetRotation(packet.GetValue<Quaternion>());
			entity->SetDirty();
		}
	}
}

#undef entity