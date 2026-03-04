#pragma once

#include "UIControl_Base.h"

class UIControl_Touch : public UIControl_Base
{
private:

public:
	UIControl_Touch();

	virtual bool setupControl(UIScene *scene, IggyValuePath *parent, const string &controlName);

	void init(int id);
	virtual void ReInit();
};