#pragma once

class CXuiSceneEnchant;

class CXuiCtrlEnchantmentButtonText : public CXuiControlImpl
{
public:
	XUI_IMPLEMENT_CLASS(CXuiCtrlEnchantmentButtonText, L"CXuiCtrlEnchantmentButtonText", XUI_CLASS_CONTROL)

protected:

	XUI_BEGIN_MSG_MAP()
		XUI_ON_XM_INIT(OnInit)
		XUI_ON_XM_RENDER(OnRender)
	XUI_END_MSG_MAP()

	HRESULT OnInit(XUIMessageInit* pInitData, BOOL& rfHandled);
	HRESULT OnRender(XUIMessageRender *pRenderData, BOOL &rfHandled);

private:
	CXuiCtrlEnchantmentButton *m_parentControl;

	float m_fScreenWidth,m_fScreenHeight;
	float m_fRawWidth,m_fRawHeight;
	
	int m_lastCost;
	wstring m_enchantmentString;

	unsigned int m_textColour, m_textFocusColour, m_textDisabledColour;

	class EnchantmentNames
	{
	public:
		static EnchantmentNames instance;

	private:
		Random random;
		vector<wstring> words;

		EnchantmentNames();

	public:
		wstring getRandomName();
	};
};