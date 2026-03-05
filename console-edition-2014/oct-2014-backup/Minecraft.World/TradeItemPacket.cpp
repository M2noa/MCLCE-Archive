#include "stdafx.h"
#include <iostream>
#include "InputOutputStream.h"
#include "PacketListener.h"
#include "TradeItemPacket.h"



TradeItemPacket::TradeItemPacket()
{
	containerId = 0;
	offer = 0;
}

TradeItemPacket::TradeItemPacket(int containerId, int offer)
{
	this->containerId = containerId;
	this->offer = offer;
}

void TradeItemPacket::handle(PacketListener *listener)
{
	listener->handleTradeItem(shared_from_this());
}

void TradeItemPacket::read(DataInputStream *dis) //throws IOException 
{
	containerId = dis->readInt();
	offer = dis->readInt();
}

void TradeItemPacket::write(DataOutputStream *dos) //throws IOException
{
	dos->writeInt(containerId);
	dos->writeInt(offer);
}

int TradeItemPacket::getEstimatedSize() 
{
	return 8;
}
