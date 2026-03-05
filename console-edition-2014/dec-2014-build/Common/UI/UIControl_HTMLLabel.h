#pragma once

#include "UIControl_Base.h"

class UIControl_HTMLLabel : public UIControl_Label
{
private:
	IggyName m_funcStartAutoScroll, m_funcTouchScroll, m_funcGetRealWidth, m_funcGetRealHeight;

public:
	UIControl_HTMLLabel();

	virtual bool setupControl(UIScene *scene, IggyValuePath *parent, const string &controlName);

	void startAutoScroll();
	virtual void ReInit();

	using UIControl_Base::setLabel;
	void setLabel(const string &label);

	virtual void SetupTouch();

	virtual void TouchScroll(S32 iY, bool bActive);

	S32 GetRealWidth();
	S32 GetRealHeight();
};