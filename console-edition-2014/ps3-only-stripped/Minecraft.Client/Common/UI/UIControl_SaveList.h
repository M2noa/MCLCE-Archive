#pragma once

#include "UIControl_ButtonList.h"

class UIControl_SaveList : public UIControl_ButtonList
{
private:
	IggyName m_funcSetTextureName;

public:
	virtual bool setupControl(UIScene *scene, IggyValuePath *parent, const string &controlName);

	using UIControl_ButtonList::addItem;

	void addItem(const wstring &label);
	void addItem(const string &label);

	void addItem(const wstring &label, int data);
	void addItem(const string &label, int data);

	void addItem(const string &label, const wstring &iconName);
	void addItem(const wstring &label, const wstring &iconName);
	void setTextureName(int iId, const wstring &iconName);

private:
	void addItem(const string &label, const wstring &iconName, int data);
	void addItem(const wstring &label, const wstring &iconName, int data);

};