#pragma once

#include "UIControl_Base.h"

class UIControl_DynamicLabel : public UIControl_Label
{
private:
	IggyName m_funcAddText, m_funcTouchScroll, m_funcGetRealWidth, m_funcGetRealHeight;

public:
	UIControl_DynamicLabel();

	virtual bool setupControl(UIScene *scene, IggyValuePath *parent, const string &controlName);

	virtual void addText(const wstring &text, bool bLastEntry);

	virtual void ReInit();

	virtual void SetupTouch();

	virtual void TouchScroll(S32 iY, bool bActive);

	S32 GetRealWidth();
	S32 GetRealHeight();
};
