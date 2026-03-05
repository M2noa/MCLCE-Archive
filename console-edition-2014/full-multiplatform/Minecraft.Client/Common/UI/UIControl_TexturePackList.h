#pragma once

#include "UIControl_Base.h"

class UIControl_TexturePackList : public UIControl_Base
{
private:
	IggyName m_addPackFunc, m_funcSelectSlot, m_funcSetTouchFocus, m_funcCanTouchTrigger, m_funcGetRealHeight,m_clearSlotsFunc;
	IggyName m_funcEnableSelector;

public:
	UIControl_TexturePackList();

	virtual bool setupControl(UIScene *scene, IggyValuePath *parent, const string &controlName);

	void init(const wstring &label, int id);

	void addPack(int id, const wstring &textureName);
	void selectSlot(int id);
	void clearSlots();

	virtual void setEnabled(bool enable);

	void SetTouchFocus(S32 iX, S32 iY, bool bRepeat);
	bool CanTouchTrigger(S32 iX, S32 iY);
	S32 GetRealHeight();
};
