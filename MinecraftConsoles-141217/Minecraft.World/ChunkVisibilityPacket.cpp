#include "stdafx.h"
#include <iostream>
#include "InputOutputStream.h"
#include "PacketListener.h"
#include "ChunkVisibilityPacket.h"



ChunkVisibilityPacket::ChunkVisibilityPacket()
{
	this->shouldDelay = false;
	x = 0;
	z = 0;
	visible = false;
}

ChunkVisibilityPacket::ChunkVisibilityPacket(int x, int z, bool visible) 
{
	this->shouldDelay = false;
	this->x = x;
	this->z = z;
	this->visible = visible;
}

void ChunkVisibilityPacket::read(DataInputStream *dis) //throws IOException 
{
	x = dis->readInt();
	z = dis->readInt();
	visible = dis->read() != 0;
}

void ChunkVisibilityPacket::write(DataOutputStream *dos) //throws IOException 
{
	dos->writeInt(x);
	dos->writeInt(z);
	dos->write(visible ? 1 : 0);
}

void ChunkVisibilityPacket::handle(PacketListener *listener)
{
	listener->handleChunkVisibility(shared_from_this());
}

int ChunkVisibilityPacket::getEstimatedSize()
{
	return 9;
}
