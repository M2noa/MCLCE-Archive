#include "stdafx.h"
#include <iostream>
#include "InputOutputStream.h"
#include "net.minecraft.world.item.h"
#include "PacketListener.h"
#include "ServerSettingsChangedPacket.h"



const int ServerSettingsChangedPacket::HOST_DIFFICULTY	= 0;
const int ServerSettingsChangedPacket::HOST_OPTIONS		= 1;
const int ServerSettingsChangedPacket::HOST_IN_GAME_SETTINGS = 2;

ServerSettingsChangedPacket::~ServerSettingsChangedPacket()
{
}

ServerSettingsChangedPacket::ServerSettingsChangedPacket()
{
	action	= 	HOST_DIFFICULTY;
	data	=	1;
}

ServerSettingsChangedPacket::ServerSettingsChangedPacket(char action, unsigned int data)
{
	this->action = action;
	this->data = data;
	
	//app.DebugPrintf("ServerSettingsChangedPacket - Difficulty = %d",difficulty);
}

void ServerSettingsChangedPacket::handle(PacketListener *listener)
{
	listener->handleServerSettingsChanged(shared_from_this());
}

void ServerSettingsChangedPacket::read(DataInputStream *dis) //throws IOException
{
	action = dis->read();
	data = dis->readInt();
}

void ServerSettingsChangedPacket::write(DataOutputStream *dos) // throws IOException
{
	dos->write(action);
	dos->writeInt(data);
}

int ServerSettingsChangedPacket::getEstimatedSize() 
{
	return 2;
}
