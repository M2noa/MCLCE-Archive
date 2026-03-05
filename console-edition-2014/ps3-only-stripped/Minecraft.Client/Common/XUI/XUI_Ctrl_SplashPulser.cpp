#include "stdafx.h"
#include "..\..\Minecraft.h"
#include "..\..\ScreenSizeCalculator.h"
#include "..\..\Lighting.h"
#include "XUI_Ctrl_SplashPulser.h"
#include "..\..\Font.h"
#include "..\..\..\Minecraft.World\Mth.h"
#include "..\..\..\Minecraft.World\System.h"

//-----------------------------------------------------------------------------
//  CXuiCtrlSplashPulser class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
CXuiCtrlSplashPulser::CXuiCtrlSplashPulser() :
	m_bDirty(FALSE),
	m_fScale(1.0f),
	m_fAlpha(1.0f)
{
	Minecraft *pMinecraft=Minecraft::GetInstance();

	ScreenSizeCalculator ssc(pMinecraft->options, pMinecraft->width_phys, pMinecraft->height_phys);
	m_fScreenWidth=(float)pMinecraft->width_phys;
	m_fRawWidth=(float)ssc.rawWidth;
	m_fScreenHeight=(float)pMinecraft->height_phys;
	m_fRawHeight=(float)ssc.rawHeight;
}

//-----------------------------------------------------------------------------
HRESULT CXuiCtrlSplashPulser::OnInit(XUIMessageInit* pInitData, BOOL& rfHandled)
{
	HRESULT hr=S_OK;
	return hr;
}

HRESULT CXuiCtrlSplashPulser::OnRender(XUIMessageRender *pRenderData, BOOL &bHandled )
{
	return S_OK;
}



