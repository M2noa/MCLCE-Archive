#include "stdafx.h"
#include <iostream>
#include "InputOutputStream.h"
#include "PacketListener.h"
#include "InteractPacket.h"



const int InteractPacket::INTERACT = 0;
const int InteractPacket::ATTACK = 1;

InteractPacket::InteractPacket()
{
	source = 0;
	target = 0;
	action = 0;
}

InteractPacket::InteractPacket(int source, int target, int action) 
{
	this->source = source;
	this->target = target;
	this->action = action;
}

void InteractPacket::read(DataInputStream *dis) //throws IOException
{
	source = dis->readInt();
	target = dis->readInt();
	action = dis->readByte();
}

void InteractPacket::write(DataOutputStream *dos) // throws IOException 
{
	dos->writeInt(source);
	dos->writeInt(target);
	dos->writeByte(action);
}

void InteractPacket::handle(PacketListener *listener) 
{
	listener->handleInteract(shared_from_this());
}

int InteractPacket::getEstimatedSize() 
{
	return 9;
}
