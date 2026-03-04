#include "stdafx.h"
#include <iostream>
#include "InputOutputStream.h"
#include "PacketListener.h"
#include "KickPlayerPacket.h"



KickPlayerPacket::KickPlayerPacket()
{
	m_networkSmallId = 0;
}

KickPlayerPacket::KickPlayerPacket(BYTE networkSmallId)
{
	m_networkSmallId = networkSmallId;
}

void KickPlayerPacket::handle(PacketListener *listener)
{
	listener->handleKickPlayer(shared_from_this());
}

void KickPlayerPacket::read(DataInputStream *dis) //throws IOException 
{
	m_networkSmallId = dis->readByte();
}

void KickPlayerPacket::write(DataOutputStream *dos) //throws IOException
{
	dos->writeByte(m_networkSmallId);
}

int KickPlayerPacket::getEstimatedSize() 
{
	return 1;
}
