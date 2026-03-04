#pragma once

#include "UIControl.h"

// This class maps to the FJ_Base class in actionscript
class UIControl_Base : public UIControl
{
protected:	
	IggyName m_initFunc;
	IggyName m_setLabelFunc;
	IggyName m_funcGetLabel;
	IggyName m_funcCheckLabelWidths;

	bool m_bLabelChanged;
	wstring m_label;
public:
	UIControl_Base();

	virtual bool setupControl(UIScene *scene, IggyValuePath *parent, const string &controlName);

	virtual void tick();

	virtual void setLabel(const wstring &label, bool instant = false, bool force = false);
	virtual void setLabel(const string &label);
	const wchar_t* getLabel();
	virtual void setAllPossibleLabels(int labelCount, wchar_t labels[][256]);
	int getId() { return m_id; }

	virtual bool hasFocus();
};
