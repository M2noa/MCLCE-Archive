#pragma once

class CXuiSceneBasePlayer : public CXuiSceneImpl
{

protected:
	HRESULT OnInit( XUIMessageInit* pInitData, BOOL& bHandled );

public:
	// Define the class. The class name must match the ClassOverride property
	// set for the scene in the UI Authoring tool.
	XUI_IMPLEMENT_CLASS( CXuiSceneBasePlayer, L"CXuiSceneBasePlayer", XUI_CLASS_SCENE )
};