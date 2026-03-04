#include "stdafx.h"
#include <iostream>
#include "InputOutputStream.h"
#include "PacketListener.h"
#include "PlayerInputPacket.h"



PlayerInputPacket::PlayerInputPacket()
{
	xa = 0.0f;
	ya = 0.0f;
	isJumpingVar = false;
	isSneakingVar = false;
	xRot = 0.0f;
	yRot = 0.0f;
}

PlayerInputPacket::PlayerInputPacket(float xa, float ya, bool isJumpingVar, bool isSneakingVar, float xRot, float yRot) 
{
	this->xa = xa;
	this->ya = ya;
	this->isJumpingVar = isJumpingVar;
	this->isSneakingVar = isSneakingVar;
	this->xRot = xRot;
	this->yRot = yRot;
}

void PlayerInputPacket::read(DataInputStream *dis) //throws IOException 
{
	xa = dis->readFloat();
	ya = dis->readFloat();
	xRot = dis->readFloat();
	yRot = dis->readFloat();
	isJumpingVar = dis->readBoolean();
	isSneakingVar = dis->readBoolean();
}

void PlayerInputPacket::write(DataOutputStream *dos) //throws IOException 
{
	dos->writeFloat(xa);
	dos->writeFloat(ya);
	dos->writeFloat(xRot);
	dos->writeFloat(yRot);
	dos->writeBoolean(isJumpingVar);
	dos->writeBoolean(isSneakingVar);
}

void PlayerInputPacket::handle(PacketListener *listener)
{
	listener->handlePlayerInput(shared_from_this());
}

int PlayerInputPacket::getEstimatedSize()
{
	return 18;
}

float PlayerInputPacket::getXa()
{
	return xa;
}

float PlayerInputPacket::getXRot()
{
	return xRot;
}

float PlayerInputPacket::getYa() 
{
	return ya;
}

float PlayerInputPacket::getYRot()
{
	return yRot;
}

bool PlayerInputPacket::isJumping() 
{
	return isJumpingVar;
}

bool PlayerInputPacket::isSneaking() 
{
	return isSneakingVar;
}
