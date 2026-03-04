#include "stdafx.h"
#include "PsPlusUpsellWrapper_Orbis.h"

PsPlusUpsellWrapper::PsPlusUpsellWrapper(int userIndex)
	: m_userIndex(userIndex)
{
	m_bHasResponse = false;
}

bool PsPlusUpsellWrapper::displayUpsell()
{
	app.DebugPrintf("<PsPlusUpsellWrapper> Bringing up system PsPlus upsell for Pad_%i.\n", m_userIndex);

	sceNpCommerceDialogInitialize();

	SceNpCommerceDialogParam param;
	sceNpCommerceDialogParamInitialize(&param);
	param.mode		= SCE_NP_COMMERCE_DIALOG_MODE_PLUS;
	param.features	= SCE_NP_PLUS_FEATURE_REALTIME_MULTIPLAY; 
	param.userId	= ProfileManager.getUserID(m_userIndex);

	sceNpCommerceDialogOpen(&param);

	return true;
}

bool PsPlusUpsellWrapper::hasResponse()
{
	if (m_bHasResponse)	return true;
	
	if (sceNpCommerceDialogUpdateStatus() == SCE_COMMON_DIALOG_STATUS_FINISHED)
	{
		app.DebugPrintf(
			"<PsPlusUpsellWrapper> Pad_%i %s an PsPlus upsell.\n", 
			m_userIndex, (m_result.authorized?"accepted":"rejected")
			);

		m_bHasResponse = true;

		sceNpCommerceDialogGetResult(&m_result);

		sceNpCommerceDialogTerminate();

#ifndef _CONTENT_PACKAGE
		// 4J-JEV: If HasPlayStationPlus is miraculously true now, 
		// we didn't give it enough time to update before bringing up the upsell
		assert( ProfileManager.HasPlayStationPlus(m_userIndex) == false );
#endif

		ProfileManager.PsPlusUpdate(m_userIndex, &m_result);
	}

	return false;
}
