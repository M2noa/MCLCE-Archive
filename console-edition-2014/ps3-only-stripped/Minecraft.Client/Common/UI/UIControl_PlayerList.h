#pragma once

#include "UIControl_ButtonList.h"

class UIControl_PlayerList : public UIControl_ButtonList
{
private:
	IggyName m_funcSetPlayerIcon, m_funcSetVOIPIcon;

public:
	virtual bool setupControl(UIScene *scene, IggyValuePath *parent, const string &controlName);

	using UIControl_ButtonList::addItem;
	void addItem(const wstring &label, int iPlayerIcon, int iVOIPIcon);
	void setPlayerIcon(int iId, int iPlayerIcon);
	void setVOIPIcon(int iId, int iVOIPIcon);
};