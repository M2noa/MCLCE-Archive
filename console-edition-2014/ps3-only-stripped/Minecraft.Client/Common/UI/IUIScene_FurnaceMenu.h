#pragma once
#include "IUIScene_AbstractContainerMenu.h"

// The 0-indexed slot in the inventory list that lines up with the result slot
#define FURNACE_SCENE_RESULT_SLOT_UP_OFFSET 6
#define FURNACE_SCENE_RESULT_SLOT_DOWN_OFFSET -7
#define FURNACE_SCENE_FUEL_SLOT_UP_OFFSET 0
#define FURNACE_SCENE_FUEL_SLOT_DOWN_OFFSET -3

class IUIScene_FurnaceMenu : public virtual IUIScene_AbstractContainerMenu
{
protected:
	virtual ESceneSection GetSectionAndSlotInDirection( ESceneSection eSection, ETapState eTapDirection, int *piTargetX, int *piTargetY );
	int getSectionStartOffset(ESceneSection eSection);
};