#pragma once

#include "UIControl_Base.h"

class UIControl_Cursor : public UIControl_Base
{
public:
	UIControl_Cursor();

	virtual bool setupControl(UIScene *scene, IggyValuePath *parent, const string &controlName);
};