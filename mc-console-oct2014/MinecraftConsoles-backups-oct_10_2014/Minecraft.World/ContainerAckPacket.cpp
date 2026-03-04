#include "stdafx.h"
#include <iostream>
#include "InputOutputStream.h"
#include "PacketListener.h"
#include "ContainerAckPacket.h"



ContainerAckPacket::ContainerAckPacket()
{
	containerId = 0;
	uid = 0;
	accepted = 0;
}

ContainerAckPacket::ContainerAckPacket(int containerId, short uid, bool accepted)
{
	this->containerId = containerId;
	this->uid = uid;
	this->accepted = accepted;
}

void ContainerAckPacket::handle(PacketListener *listener)
{
	listener->handleContainerAck(shared_from_this());
}

void ContainerAckPacket::read(DataInputStream *dis) //throws IOException 
{
	containerId = dis->readByte();
	uid = dis->readShort();
	accepted = dis->readByte() != 0;
}

void ContainerAckPacket::write(DataOutputStream *dos) //throws IOException
{
	dos->writeByte(containerId);
	dos->writeShort(uid);
	dos->writeByte(accepted ? 1 : 0);
}

int ContainerAckPacket::getEstimatedSize() 
{
	return 4;
}
