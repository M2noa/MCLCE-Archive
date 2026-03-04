#pragma once


class CXuiCtrl4JIcon : public CXuiControlImpl
{
public:

	XUI_IMPLEMENT_CLASS(CXuiCtrl4JIcon, L"CXuiCtrl4JIcon", XUI_CLASS_LABEL);
	HRESULT UseBrush(HXUIBRUSH hBrush);

protected:

	XUI_BEGIN_MSG_MAP()
		XUI_ON_XM_INIT(OnInit)
		XUI_ON_XM_GET_SOURCE_IMAGE(OnGetSourceDataImage)
		XUI_ON_XM_DESTROY( OnDestroy )
	XUI_END_MSG_MAP()


	HRESULT OnInit( XUIMessageInit* pInitData, BOOL& bHandled );
	HRESULT OnGetSourceDataImage(XUIMessageGetSourceImage *pGetSourceImageData,BOOL& bHandled);
	HRESULT OnDestroy();

	HXUIBRUSH m_hBrush;
};
