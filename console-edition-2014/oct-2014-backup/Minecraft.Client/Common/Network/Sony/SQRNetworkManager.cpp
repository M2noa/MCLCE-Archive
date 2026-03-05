#include "stdafx.h"

#include "SQRNetworkManager.h"

bool SQRNetworkManager::s_safeToRespondToGameBootInvite = false;

void SQRNetworkManager::SafeToRespondToGameBootInvite()
{
	s_safeToRespondToGameBootInvite = true;
}
