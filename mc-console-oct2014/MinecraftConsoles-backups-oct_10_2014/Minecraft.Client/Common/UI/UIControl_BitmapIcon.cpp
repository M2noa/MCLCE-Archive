#include "stdafx.h"
#include "UI.h"
#include "UIControl_BitmapIcon.h"

bool UIControl_BitmapIcon::setupControl(UIScene *scene, IggyValuePath *parent, const string &controlName)
{
	UIControl::setControlType(UIControl::eBitmapIcon);
	bool success = UIControl::setupControl(scene,parent,controlName);

	//SlotList specific initialisers
	m_funcSetTextureName = registerFastName(L"SetTextureName");

	return success;
}

void UIControl_BitmapIcon::setTextureName(const wstring &iconName)
{
	IggyDataValue result;
	IggyDataValue value[1];

	IggyStringUTF16 stringVal;
	stringVal.string = (IggyUTF16*)iconName.c_str();
	stringVal.length = iconName.length();
	value[0].type = IGGY_DATATYPE_string_UTF16;
	value[0].string16 = stringVal;
	IggyResult out = IggyPlayerCallMethodRS ( m_parentScene->getMovie() , &result, getIggyValuePath(), m_funcSetTextureName , 1 , value );
}
