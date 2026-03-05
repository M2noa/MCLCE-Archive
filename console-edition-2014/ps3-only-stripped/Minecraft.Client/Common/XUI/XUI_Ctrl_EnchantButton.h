#pragma once

class CXuiSceneEnchant;

class CXuiCtrlEnchantmentButton : public CXuiControlImpl
{
	friend class CXuiCtrlEnchantmentButtonText;
public:
	XUI_IMPLEMENT_CLASS(CXuiCtrlEnchantmentButton, L"CXuiCtrlEnchantmentButton", XUI_CLASS_CONTROL)

protected:

	XUI_BEGIN_MSG_MAP()
		XUI_ON_XM_INIT(OnInit)
		XUI_ON_XM_GET_SOURCE_TEXT(OnGetSourceDataText)
	XUI_END_MSG_MAP()

	HRESULT OnInit(XUIMessageInit* pInitData, BOOL& rfHandled);
	HRESULT OnGetSourceDataText(XUIMessageGetSourceText *pGetSourceTextData, BOOL& bHandled);

public:
	void SetData(int iPad, int index);

private:
	int	m_iPad;
	int m_index;
	int m_lastCost;
	wstring m_costString;
	CXuiSceneEnchant *m_containerScene;

	float m_fScreenWidth,m_fScreenHeight;
	float m_fRawWidth,m_fRawHeight;
};