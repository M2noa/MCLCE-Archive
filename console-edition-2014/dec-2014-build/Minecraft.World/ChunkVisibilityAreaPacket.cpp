#include "stdafx.h"
#include <iostream>
#include "InputOutputStream.h"
#include "net.minecraft.world.level.h"
#include "PacketListener.h"
#include "ChunkVisibilityAreaPacket.h"



ChunkVisibilityAreaPacket::ChunkVisibilityAreaPacket()
{
	m_minX = 0;
	m_maxX = 0;
	m_minZ = 0;
	m_maxZ = 0;
}

ChunkVisibilityAreaPacket::ChunkVisibilityAreaPacket(int minX, int maxX, int minZ, int maxZ)
{
	m_minX = minX;
	m_maxX = maxX;
	m_minZ = minZ;
	m_maxZ = maxZ;
}

void ChunkVisibilityAreaPacket::read(DataInputStream *dis) //throws IOException
{
	m_minX = dis->readInt();
	m_maxX = dis->readInt();
	m_minZ = dis->readInt();
	m_maxZ = dis->readInt();
}

void ChunkVisibilityAreaPacket::write(DataOutputStream *dos) // throws IOException
{
	dos->writeInt(m_minX);
	dos->writeInt(m_maxX);
	dos->writeInt(m_minZ);
	dos->writeInt(m_maxZ);
}

void ChunkVisibilityAreaPacket::handle(PacketListener *listener)
{
	listener->handleChunkVisibilityArea(shared_from_this());
}

int ChunkVisibilityAreaPacket::getEstimatedSize() 
{
	return 16;
}

