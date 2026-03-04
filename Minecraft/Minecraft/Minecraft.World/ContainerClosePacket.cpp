#include "stdafx.h"
#include <iostream>
#include "InputOutputStream.h"
#include "PacketListener.h"
#include "ContainerClosePacket.h"



ContainerClosePacket::ContainerClosePacket()
{
	containerId = 0;
}

ContainerClosePacket::ContainerClosePacket(int containerId) 
{
	this->containerId = containerId;
}

void ContainerClosePacket::handle(PacketListener *listener) 
{
	listener->handleContainerClose(shared_from_this());
}

void ContainerClosePacket::read(DataInputStream *dis) //throws IOException
{
	containerId = dis->readByte();
}

void ContainerClosePacket::write(DataOutputStream *dos) //throws IOException 
{
	dos->writeByte(containerId);
}

int ContainerClosePacket::getEstimatedSize() 
{
	return 1;
}
