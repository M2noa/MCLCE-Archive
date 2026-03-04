#include "stdafx.h"
#include <iostream>
#include "InputOutputStream.h"
#include "net.minecraft.world.item.h"
#include "PacketListener.h"
#include "CraftItemPacket.h"



CraftItemPacket::~CraftItemPacket()
{
}

CraftItemPacket::CraftItemPacket()
{
	recipe = -1;
	uid = 0;
}

CraftItemPacket::CraftItemPacket(int recipe, short uid)
{
	this->recipe = recipe;
	this->uid = uid;
}

void CraftItemPacket::handle(PacketListener *listener)
{
	listener->handleCraftItem(shared_from_this());
}

void CraftItemPacket::read(DataInputStream *dis) //throws IOException
{
	uid = dis->readShort();
	recipe = dis->readInt();
}

void CraftItemPacket::write(DataOutputStream *dos) // throws IOException
{
	dos->writeShort(uid);
	dos->writeInt(recipe);
}

int CraftItemPacket::getEstimatedSize() 
{
	return 2 + 4;
}
