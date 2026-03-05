#pragma once

#include "UIScene.h"

class UIComponent_MenuBackground : public UIScene
{
private:
	bool m_bSplitscreen;
public:
	UIComponent_MenuBackground(int iPad, void *initData, UILayer *parentLayer);

protected:
	// TODO: This should be pure virtual in this class
	virtual wstring getMoviePath();

public:
	virtual EUIScene getSceneType() { return eUIComponent_MenuBackground;}

	// Returns true if this scene handles input
	virtual bool stealsFocus() { return false; }

	// Returns true if this scene has focus for the pad passed in
	virtual bool hasFocus(int iPad) { return false; }

	// Returns true if lower scenes in this scenes layer, or in any layer below this scenes layers should be hidden
	virtual bool hidesLowerScenes() { return false; }

	// RENDERING
	virtual void render(S32 width, S32 height, C4JRender::eViewportType viewport);
};