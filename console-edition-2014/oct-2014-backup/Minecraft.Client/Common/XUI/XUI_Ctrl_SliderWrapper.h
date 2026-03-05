#pragma once

class CXuiCtrlSliderWrapper : public CXuiSceneImpl
{
private:
	CXuiSlider *m_pSlider;
	CXuiControl *m_pFocusSink;
	bool m_sliderActive;
	bool m_bDisplayVal;
	bool m_bPlaySound;

protected:
	// Message map. Here we tie messages to message handlers.
	XUI_BEGIN_MSG_MAP()
		XUI_ON_XM_INIT( OnInit )
		XUI_ON_XM_TIMER( OnTimer )
		XUI_ON_XM_KEYDOWN(OnKeyDown)
		XUI_ON_XM_NOTIFY_VALUE_CHANGED(OnNotifyValueChanged)
	XUI_END_MSG_MAP()

	HRESULT OnInit( XUIMessageInit* pInitData, BOOL& bHandled );
	HRESULT OnKeyDown(XUIMessageInput* pInputData, BOOL& rfHandled);
	HRESULT OnNotifyValueChanged (HXUIOBJ hObjSource, XUINotifyValueChanged* pValueChangedData, BOOL& rfHandled);
	HRESULT OnTimer(XUIMessageTimer *pData,BOOL& rfHandled);
public:

	// Define the class. The class name must match the ClassOverride property
	// set for the scene in the UI Authoring tool.
	XUI_IMPLEMENT_CLASS( CXuiCtrlSliderWrapper, L"CXuiCtrlSliderWrapper", XUI_CLASS_SCENE )

	HRESULT SetValue( int nValue );
	HXUIOBJ GetSlider();
	HRESULT SetRange( int nRangeMin, int nRangeMax);
	LPCWSTR GetText(  );
	HRESULT SetText(LPCWSTR text, int iDataAssoc=0  );
	HRESULT SetValueDisplay( BOOL bShow );

};