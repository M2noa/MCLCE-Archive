#include "stdafx.h"
#include "com.mojang.nbt.h"
#include "net.minecraft.world.level.h"
#include "net.minecraft.world.entity.item.h"
#include "net.minecraft.world.entity.player.h"
#include "net.minecraft.world.item.h"
#include "net.minecraft.world.entity.h"
#include "net.minecraft.world.phys.h"
#include "net.minecraft.network.packet.h"
#include "SignTileEntity.h"
#include <xuiapp.h>
#include "..\Minecraft.Client\ClientConnection.h"
#include "..\Minecraft.Client\Minecraft.h"
#include "..\Minecraft.Client\ServerLevel.h"
#include "..\Minecraft.World\Level.h"



const int SignTileEntity::MAX_LINE_LENGTH = 15;

SignTileEntity::SignTileEntity() : TileEntity()
{
	m_wsmessages[0] = L"";
	m_wsmessages[1] = L"";
	m_wsmessages[2] = L"";
	m_wsmessages[3] = L"";
	m_bVerified=true;
	m_bCensored=false;

	m_iSelectedLine = -1;

	_isEditable = true;
}

SignTileEntity::~SignTileEntity()
{
	// TODO ORBIS_STUBBED;
#ifndef __ORBIS__
	// 4J-PB - we don't need to verify strings anymore - InputManager.CancelQueuedVerifyStrings(&SignTileEntity::StringVerifyCallback,(LPVOID)this);
#endif
}

void SignTileEntity::save(CompoundTag *tag) 
{
	TileEntity::save(tag);
	tag->putString(L"Text1", m_wsmessages[0] );
	tag->putString(L"Text2", m_wsmessages[1] );
	tag->putString(L"Text3", m_wsmessages[2] );
	tag->putString(L"Text4", m_wsmessages[3] );
#ifndef _CONTENT_PACKAGE
	OutputDebugStringW(L"### - Saving a sign with text - \n");
	for(int i=0;i<4;i++)
	{
		OutputDebugStringW(m_wsmessages[i].c_str());
		OutputDebugStringW(L"\n");
	}
#endif
}

void SignTileEntity::load(CompoundTag *tag)
{
	_isEditable = false;
	TileEntity::load(tag);
	for (int i = 0; i < MAX_SIGN_LINES; i++) 
	{
		wchar_t *buf = new wchar_t[256];
		swprintf(buf, 256, L"Text%d", (i+1) );
		m_wsmessages[i] = tag->getString( buf );
		if (m_wsmessages[i].length() > MAX_LINE_LENGTH) m_wsmessages[i] = m_wsmessages[i].substr(0, MAX_LINE_LENGTH);
	}
#ifndef _CONTENT_PACKAGE
	OutputDebugStringW(L"### - Loaded a sign with text - \n");
	for(int i=0;i<4;i++)
	{
		OutputDebugStringW(m_wsmessages[i].c_str());
		OutputDebugStringW(L"\n");
	}
#endif

	// 4J Stu - Fix for #13531 - Bug: Signs do not Censor after loading a save
	// Set verified as false so that it can be re-verified
	m_bVerified=false;

	setChanged();
}

shared_ptr<Packet> SignTileEntity::getUpdatePacket()
{
	wstring copy[MAX_SIGN_LINES];
	for (int i = 0; i < MAX_SIGN_LINES; i++) 
	{
		copy[i] = m_wsmessages[i];
	}
	return shared_ptr<SignUpdatePacket>( new SignUpdatePacket(x, y, z, m_bVerified, m_bCensored, copy) );
}

bool SignTileEntity::isEditable() 
{
	return _isEditable;
}

void SignTileEntity::setEditable(bool isEditable)
{
	this->_isEditable = isEditable;
}

void SignTileEntity::setChanged()
{
	TileEntity::setChanged();

	// 4J - CENSORING DISABLED - Skip string verification entirely
	// Original code called InputManager.VerifyStrings() to check for profanity
	// Now we just mark as verified and not censored immediately
	
	m_bVerified = true;
	m_bCensored = false;
	
	// Send update immediately without waiting for verification callback
	if(!level->isClientSide)
	{
		ServerLevel *serverLevel = (ServerLevel *)level;
		serverLevel->queueSendTileUpdate(x, y, z);
	}
}


void SignTileEntity::SetMessage(int iIndex,wstring &wsText)
{ 
	m_wsmessages[iIndex]=wsText;

}

// 4J-PB - added for string verification
int SignTileEntity::StringVerifyCallback(LPVOID lpParam,STRING_VERIFY_RESPONSE *pResults)
{
	// 4J - CENSORING DISABLED - Free speech enabled, all signs display as written
	SignTileEntity *pClass=(SignTileEntity *)lpParam;

	pClass->m_bVerified=true;
	pClass->m_bCensored=false;  // Always false - no censoring
	
	// Original code checked pResults->pStringResult[i] for profanity
	// Now we skip all censoring checks

	if(!pClass->level->isClientSide)
	{
		ServerLevel *serverLevel = (ServerLevel *)pClass->level;
		// 4J Stu - This callback gets called on the main thread, but tried to access things on the server thread. Change to go through the protected method.
		//pClass->level->sendTileUpdated(pClass->x, pClass->y, pClass->z);
		serverLevel->queueSendTileUpdate(pClass->x, pClass->y, pClass->z);
	}

	return 0;
}

// 4J Added
shared_ptr<TileEntity> SignTileEntity::clone()
{
	shared_ptr<SignTileEntity> result = shared_ptr<SignTileEntity>( new SignTileEntity() );
	TileEntity::clone(result);

	result->m_wsmessages[0] = m_wsmessages[0];
	result->m_wsmessages[1] = m_wsmessages[1];
	result->m_wsmessages[2] = m_wsmessages[2];
	result->m_wsmessages[3] = m_wsmessages[3];
	result->m_bVerified = m_bVerified;
	result->m_bCensored = m_bCensored;
	return result;
}