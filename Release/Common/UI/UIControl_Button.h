#pragma once

#include "UIControl_Base.h"

class UIControl_Button : public UIControl_Base
{
private:
	IggyName m_funcEnableButton;

public:
	UIControl_Button();

	virtual bool setupControl(UIScene *scene, IggyValuePath *parent, const string &controlName);

	void init(const wstring &label, int id);
	virtual void ReInit();

	void setEnable(bool enable);
};