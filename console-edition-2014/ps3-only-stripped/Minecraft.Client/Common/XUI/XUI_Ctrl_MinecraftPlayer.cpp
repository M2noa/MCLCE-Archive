#include "stdafx.h"
#include "..\..\Minecraft.h"
#include "..\..\ScreenSizeCalculator.h"
#include "..\..\EntityRenderDispatcher.h"
#include "..\..\Lighting.h"
#include "..\..\MultiplayerLocalPlayer.h"
#include "XUI_Ctrl_MinecraftPlayer.h"
#include "XUI_Scene_AbstractContainer.h"
#include "XUI_Scene_Inventory.h"
#include "..\..\Options.h"

//-----------------------------------------------------------------------------
//  CXuiCtrlMinecraftPlayer class
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
CXuiCtrlMinecraftPlayer::CXuiCtrlMinecraftPlayer() :
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
HRESULT CXuiCtrlMinecraftPlayer::OnInit(XUIMessageInit* pInitData, BOOL& rfHandled)
{
	HRESULT hr=S_OK;

	HXUIOBJ parent = m_hObj;
	HXUICLASS hcInventoryClass = XuiFindClass( L"CXuiSceneInventory" );
	HXUICLASS currentClass;

	do
	{
		XuiElementGetParent(parent,&parent);
		currentClass = XuiGetObjectClass( parent );
	} while (parent != NULL && !XuiClassDerivesFrom( currentClass, hcInventoryClass ) );

	assert( parent != NULL );

	VOID *pObj;
	XuiObjectFromHandle( parent, &pObj );
	m_containerScene = (CXuiSceneInventory *)pObj;

	m_iPad = m_containerScene->getPad();

	return hr;
}

HRESULT CXuiCtrlMinecraftPlayer::OnRender(XUIMessageRender *pRenderData, BOOL &bHandled )
{
	return S_OK;

}



