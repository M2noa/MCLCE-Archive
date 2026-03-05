#include "stdafx.h"
#include <iostream>
#include "InputOutputStream.h"
#include "PacketListener.h"
#include "TakeItemEntityPacket.h"



TakeItemEntityPacket::TakeItemEntityPacket() 
{
	itemId = -1;
	playerId = -1;
}

TakeItemEntityPacket::TakeItemEntityPacket(int itemId, int playerId)
{
	this->itemId = itemId;
	this->playerId = playerId;
}

void TakeItemEntityPacket::read(DataInputStream *dis) //throws IOException 
{
	itemId = dis->readInt();
	playerId = dis->readInt();
}

void TakeItemEntityPacket::write(DataOutputStream *dos) //throws IOException 
{
	dos->writeInt(itemId);
	dos->writeInt(playerId);
}

void TakeItemEntityPacket::handle(PacketListener *listener) 
{
	listener->handleTakeItemEntity(shared_from_this());
}

int TakeItemEntityPacket::getEstimatedSize()
{
	return 8;
}
