#include "stdafx.h"
#include "UI.h"
#include "UIControl_Cursor.h"

UIControl_Cursor::UIControl_Cursor()
{
}

bool UIControl_Cursor::setupControl(UIScene *scene, IggyValuePath *parent, const string &controlName)
{
	UIControl::setControlType(UIControl::eCursor);
	bool success = UIControl_Base::setupControl(scene,parent,controlName);

	//Label specific initialisers

	return success;
}
