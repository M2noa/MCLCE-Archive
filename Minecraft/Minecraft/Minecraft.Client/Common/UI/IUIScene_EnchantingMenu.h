#pragma once

#include "IUIScene_AbstractContainerMenu.h"

// The 0-indexed slot in the inventory list that lines up with the result slot
#define ENCHANT_SCENE_ENCHANT_BUTTONS_UP_OFFSET 3
#define ENCHANT_SCENE_ENCHANT_BUTTONS_DOWN_OFFSET -7
#define ENCHANT_SCENE_INGREDIENT_SLOT_UP_OFFSET 0
#define ENCHANT_SCENE_INGREDIENT_SLOT_DOWN_OFFSET 0

class EnchantmentMenu;

class IUIScene_EnchantingMenu : public virtual IUIScene_AbstractContainerMenu
{
protected:
	virtual ESceneSection GetSectionAndSlotInDirection( ESceneSection eSection, ETapState eTapDirection, int *piTargetX, int *piTargetY );
	virtual void handleOtherClicked(int iPad, ESceneSection eSection, int buttonNum, bool quickKey);
	int getSectionStartOffset(ESceneSection eSection);
	virtual bool IsSectionSlotList( ESceneSection eSection );

public:
	EnchantmentMenu *getMenu();
};