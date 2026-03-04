#include "stdafx.h"
#include <iostream>
#include "InputOutputStream.h"
#include "PacketListener.h"
#include "UpdateProgressPacket.h"



UpdateProgressPacket::UpdateProgressPacket()
{
	this->m_percentage = 0;
}

UpdateProgressPacket::UpdateProgressPacket(int percentage)
{
	this->m_percentage = percentage;
}

void UpdateProgressPacket::read(DataInputStream *dis) //throws IOException 
{
	m_percentage = dis->readByte();
}

void UpdateProgressPacket::write(DataOutputStream *dos) //throws IOException 
{
	dos->writeByte(m_percentage);
}

void UpdateProgressPacket::handle(PacketListener *listener) 
{
	listener->handleUpdateProgress(shared_from_this());
}

int UpdateProgressPacket::getEstimatedSize()
{
	return 1;
}
