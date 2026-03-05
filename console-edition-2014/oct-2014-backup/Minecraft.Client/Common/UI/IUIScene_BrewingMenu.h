#pragma once
#include "IUIScene_AbstractContainerMenu.h"

// The 0-indexed slot in the inventory list that lines up with the result slot
#define BREWING_SCENE_INGREDIENT_SLOT_UP_OFFSET 5
#define BREWING_SCENE_INGREDIENT_SLOT_DOWN_OFFSET 5
#define BREWING_SCENE_BOTTLE1_SLOT_UP_OFFSET 3
#define BREWING_SCENE_BOTTLE1_SLOT_DOWN_OFFSET 3
#define BREWING_SCENE_BOTTLE2_SLOT_UP_OFFSET 4
#define BREWING_SCENE_BOTTLE2_SLOT_DOWN_OFFSET 4
#define BREWING_SCENE_BOTTLE3_SLOT_UP_OFFSET 5
#define BREWING_SCENE_BOTTLE3_SLOT_DOWN_OFFSET 5

class IUIScene_BrewingMenu : public virtual IUIScene_AbstractContainerMenu
{
protected:
	virtual ESceneSection GetSectionAndSlotInDirection( ESceneSection eSection, ETapState eTapDirection, int *piTargetX, int *piTargetY );
	int getSectionStartOffset(ESceneSection eSection);
};