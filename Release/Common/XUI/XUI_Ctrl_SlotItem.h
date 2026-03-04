#pragma once
#include "XUI_Ctrl_SlotItemCtrlBase.h"

class CXuiCtrlSlotItem : public CXuiControlImpl, public CXuiCtrlSlotItemCtrlBase
{
public:
	// Define the class. The class name must match the ClassOverride property
	// set for the scene in the UI Authoring tool.
	XUI_IMPLEMENT_CLASS( CXuiCtrlSlotItem, L"CXuiCtrlSlotItem", XUI_CLASS_CONTROL )

	XUI_BEGIN_MSG_MAP()
		XUI_ON_XM_INIT( OnInit )
		XUI_ON_XM_DESTROY(OnDestroy)
		XUI_ON_XM_GETSLOTITEM_MESSAGE(OnCustomMessage_GetSlotItem)

		// 4J WESTY : Pointer Prototype : Added to support prototype only.
		XUI_ON_XM_CONTROL_NAVIGATE( OnControlNavigate )

		XUI_ON_XM_KEYDOWN( OnKeyDown )
	XUI_END_MSG_MAP()
	
	using CXuiCtrlSlotItemCtrlBase::OnInit;
	HRESULT OnInit(XUIMessageInit* pInitData, BOOL& bHandled) { return this->OnInit( m_hObj, pInitData, bHandled ); };
	
	using CXuiCtrlSlotItemCtrlBase::OnDestroy;
	HRESULT OnDestroy() { return this->OnDestroy( m_hObj ); };

	using CXuiCtrlSlotItemCtrlBase::OnCustomMessage_GetSlotItem;
	HRESULT OnCustomMessage_GetSlotItem(CustomMessage_GetSlotItem_Struct *pData, BOOL& bHandled) { return this->OnCustomMessage_GetSlotItem( m_hObj, pData, bHandled ); };

	// 4J WESTY : Pointer Prototype : Added to support prototype only.
	using CXuiCtrlSlotItemCtrlBase::OnControlNavigate;
	HRESULT OnControlNavigate(XUIMessageControlNavigate *pControlNavigateData, BOOL& bHandled) { return this->OnControlNavigate( m_hObj, pControlNavigateData, bHandled ); };

	using CXuiCtrlSlotItemCtrlBase::OnKeyDown;
	HRESULT OnKeyDown(XUIMessageInput *pInputData, BOOL& bHandled) { return this->OnKeyDown( m_hObj, pInputData, bHandled ); };
};