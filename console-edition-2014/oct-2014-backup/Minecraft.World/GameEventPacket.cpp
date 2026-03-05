#include "stdafx.h"
#include <iostream>
#include "InputOutputStream.h"
#include "PacketListener.h"
#include "GameEventPacket.h"



const int GameEventPacket::NO_RESPAWN_BED_AVAILABLE = 0;
const int GameEventPacket::START_RAINING = 1;
const int GameEventPacket::STOP_RAINING = 2;
const int GameEventPacket::CHANGE_GAME_MODE = 3; // 1.8.2
const int GameEventPacket::WIN_GAME = 4; // 1.0.1
const int GameEventPacket::DEMO_EVENT = 5;

const int GameEventPacket::DEMO_PARAM_INTRO = 0;
const int GameEventPacket::DEMO_PARAM_HINT_1 = 101;
const int GameEventPacket::DEMO_PARAM_HINT_2 = 102;
const int GameEventPacket::DEMO_PARAM_HINT_3 = 103;

// 4J Added
const int GameEventPacket::START_SAVING = 10;
const int GameEventPacket::STOP_SAVING = 11;

const int GameEventPacket::EVENT_LANGUAGE_ID[EVENT_LANGUAGE_ID_LENGTH] = { IDS_TILE_BED_NOT_VALID, -1, -1, IDS_GAME_MODE_CHANGED, -1, -1 };

GameEventPacket::GameEventPacket()
{
	this->_event = 0;
	this->param = 0;
}

GameEventPacket::GameEventPacket(int _event, int param)
{
	this->_event = _event;
	this->param = param;
}

void GameEventPacket::read(DataInputStream *dis) //throws IOException 
{
	_event = dis->readByte();
	param = dis->readByte();
}

void GameEventPacket::write(DataOutputStream *dos) //throws IOException 
{
	dos->writeByte(_event);
	dos->writeByte(param);
}

void GameEventPacket::handle(PacketListener *listener) 
{
	listener->handleGameEvent(shared_from_this());
}

int GameEventPacket::getEstimatedSize()
{
	return 2;
}
