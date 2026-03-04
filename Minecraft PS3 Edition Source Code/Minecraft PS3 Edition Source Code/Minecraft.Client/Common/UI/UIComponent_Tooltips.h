#pragma once

#include "UIScene.h"

class UIComponent_Tooltips : public UIScene
{
private:
	bool m_bSplitscreen;

protected:
	typedef struct _TooltipValues
	{
		bool show;	
		int iString;

		_TooltipValues()
		{
			show = false;
			iString = -1;
		}
	} TooltipValues;

	TooltipValues m_tooltipValues[eToolTipNumButtons];

	IggyName m_funcSetTooltip, m_funcSetOpacity, m_funcSetABSwap, m_funcUpdateLayout;


	UI_BEGIN_MAP_ELEMENTS_AND_NAMES(UIScene)
		UI_MAP_NAME( m_funcSetTooltip, L"SetToolTip")
		UI_MAP_NAME( m_funcSetOpacity, L"SetOpacity")
		UI_MAP_NAME( m_funcSetABSwap, L"SetABSwap")
		UI_MAP_NAME( m_funcUpdateLayout, L"UpdateLayout")
	UI_END_MAP_ELEMENTS_AND_NAMES()

	virtual wstring getMoviePath();
	
	virtual F64 getSafeZoneHalfWidth();

public:
	UIComponent_Tooltips(int iPad, void *initData, UILayer *parentLayer);

	virtual EUIScene getSceneType() { return eUIComponent_Tooltips;}

	// Returns true if this scene handles input
	virtual bool stealsFocus() { return false; }

	// Returns true if this scene has focus for the pad passed in
	virtual bool hasFocus(int iPad) { return false; }

	// Returns true if lower scenes in this scenes layer, or in any layer below this scenes layers should be hidden
	virtual bool hidesLowerScenes() { return false; }

	virtual void updateSafeZone();

	virtual void tick();

	// RENDERING
	virtual void render(S32 width, S32 height, C4JRender::eViewportType viewport);

	virtual void SetTooltipText( unsigned int tooltip, int iTextID );
	virtual void SetEnableTooltips( bool bVal );
	virtual void ShowTooltip( unsigned int tooltip, bool show );
	virtual void SetTooltips( int iA, int iB=-1, int iX=-1, int iY=-1 , int iLT=-1, int iRT=-1, int iLB=-1, int iRB=-1, int iLS=-1, bool forceUpdate = false);
	virtual void EnableTooltip( unsigned int tooltip, bool enable );

	virtual void handleReload();
	virtual void handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled);

	void overrideSFX(int iPad, int key, bool bVal);


private:
	bool _SetTooltip(unsigned int iToolTip, int iTextID);
	void _SetTooltip(unsigned int iToolTipId, const wstring &label, bool show, bool force = false);
	void _Relayout();

	bool m_overrideSFX[XUSER_MAX_COUNT][ACTION_MAX_MENU];

};