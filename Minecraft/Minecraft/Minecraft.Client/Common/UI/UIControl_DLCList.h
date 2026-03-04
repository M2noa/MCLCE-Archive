#pragma once

#include "UIControl_ButtonList.h"

class UIControl_DLCList : public UIControl_ButtonList
{
private:
	IggyName m_funcShowTick;

public:
	virtual bool setupControl(UIScene *scene, IggyValuePath *parent, const string &controlName);

	using UIControl_ButtonList::addItem;
	void addItem(const string &label, bool showTick, int iId);
	void addItem(const wstring &label, bool showTick, int iId);
	void showTick(int iId, bool showTick);
};
