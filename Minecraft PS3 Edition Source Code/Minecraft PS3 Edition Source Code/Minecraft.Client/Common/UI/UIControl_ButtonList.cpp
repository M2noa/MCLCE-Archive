#include "stdafx.h"
#include "UI.h"
#include "UIControl_ButtonList.h"

UIControl_ButtonList::UIControl_ButtonList()
{
	m_itemCount = 0;
	m_iCurrentSelection = 0;
}

bool UIControl_ButtonList::setupControl(UIScene *scene, IggyValuePath *parent, const string &controlName)
{
	UIControl::setControlType(UIControl::eButtonList);
	bool success = UIControl_Base::setupControl(scene,parent,controlName);

	//SlotList specific initialisers
	m_addNewItemFunc = registerFastName(L"addNewItem");
	m_removeAllItemsFunc = registerFastName(L"removeAllItems");
	m_funcHighlightItem = registerFastName(L"HighlightItem");
	m_funcRemoveItem = registerFastName(L"RemoveItem");
	m_funcSetButtonLabel = registerFastName(L"SetButtonLabel");
	m_funcSetTouchFocus = registerFastName(L"SetTouchFocus");
	m_funcCanTouchTrigger = registerFastName(L"CanTouchTrigger");

	return success;
}

void UIControl_ButtonList::init(int id)
{
	m_id = id;

	IggyDataValue result;
	IggyDataValue value[1];
	value[0].type = IGGY_DATATYPE_number;
	value[0].number = id;
	IggyResult out = IggyPlayerCallMethodRS ( m_parentScene->getMovie() , &result, getIggyValuePath() , m_initFunc , 1 , value );

}

void UIControl_ButtonList::ReInit()
{
	UIControl_Base::ReInit();
	init(m_id);
	m_itemCount = 0;
	m_iCurrentSelection = 0;
}

void UIControl_ButtonList::clearList()
{
	IggyDataValue result;
	IggyResult out = IggyPlayerCallMethodRS ( m_parentScene->getMovie() , &result, getIggyValuePath(), m_removeAllItemsFunc , 0 , NULL );

	m_itemCount = 0;
}

void UIControl_ButtonList::addItem(const string &label)
{
	addItem(label, m_itemCount);
}

void UIControl_ButtonList::addItem(const wstring &label)
{
	addItem(label, m_itemCount);
}

void UIControl_ButtonList::addItem(const string &label, int data)
{
	IggyDataValue result;
	IggyDataValue value[2];

	IggyStringUTF8 stringVal;
	stringVal.string = (char*)label.c_str();
	stringVal.length = (S32)label.length();
	value[0].type = IGGY_DATATYPE_string_UTF8;
	value[0].string8 = stringVal;

	value[1].type = IGGY_DATATYPE_number;
	value[1].number = data;
	IggyResult out = IggyPlayerCallMethodRS ( m_parentScene->getMovie() , &result, getIggyValuePath(), m_addNewItemFunc , 2 , value );

	++m_itemCount;
}

void UIControl_ButtonList::addItem(const wstring &label, int data)
{
	IggyDataValue result;
	IggyDataValue value[2];

	IggyStringUTF16 stringVal;
	stringVal.string = (IggyUTF16*)label.c_str();
	stringVal.length = label.length();
	value[0].type = IGGY_DATATYPE_string_UTF16;
	value[0].string16 = stringVal;

	value[1].type = IGGY_DATATYPE_number;
	value[1].number = data;
	IggyResult out = IggyPlayerCallMethodRS ( m_parentScene->getMovie() , &result, getIggyValuePath(), m_addNewItemFunc , 2 , value );

	++m_itemCount;
}

void UIControl_ButtonList::removeItem(int index)
{
	IggyDataValue result;
	IggyDataValue value[1];

	value[0].type = IGGY_DATATYPE_number;
	value[0].number = index;
	IggyResult out = IggyPlayerCallMethodRS ( m_parentScene->getMovie() , &result, getIggyValuePath(), m_funcRemoveItem , 1 , value );

	--m_itemCount;
}

void UIControl_ButtonList::setCurrentSelection(int iSelection)
{
	IggyDataValue result;
	IggyDataValue value[1];

	value[0].type = IGGY_DATATYPE_number;
	value[0].number = iSelection;
	IggyResult out = IggyPlayerCallMethodRS ( m_parentScene->getMovie() , &result, getIggyValuePath(), m_funcHighlightItem , 1 , value );
}

int UIControl_ButtonList::getCurrentSelection()
{
	return m_iCurrentSelection;
}

void UIControl_ButtonList::updateChildFocus(int iChild)
{
	m_iCurrentSelection = iChild;
}

void UIControl_ButtonList::setButtonLabel(int iButtonId, const wstring &label)
{
	IggyDataValue result;
	IggyDataValue value[2];

	value[0].type = IGGY_DATATYPE_number;
	value[0].number = iButtonId;

	IggyStringUTF16 stringVal;
	stringVal.string = (IggyUTF16*)label.c_str();
	stringVal.length = label.length();
	value[1].type = IGGY_DATATYPE_string_UTF16;
	value[1].string16 = stringVal;
	IggyResult out = IggyPlayerCallMethodRS ( m_parentScene->getMovie(), &result, getIggyValuePath(), m_funcSetButtonLabel, 2 , value );
}

