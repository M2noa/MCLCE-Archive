#pragma once

#include "UIControl_Base.h"

class UIControl_Label : public UIControl_Base
{
public:
	UIControl_Label();

	virtual bool setupControl(UIScene *scene, IggyValuePath *parent, const string &controlName);

	void init(const wstring &label);
	void init(const string &label);
	virtual void ReInit();
};