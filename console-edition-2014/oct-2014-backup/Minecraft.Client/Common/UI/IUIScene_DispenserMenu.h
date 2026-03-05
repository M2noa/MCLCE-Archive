#pragma once
#include "IUIScene_AbstractContainerMenu.h"

// The 0-indexed slot in the inventory list that lines up with the result slot
#define TRAP_SCENE_TRAP_SLOT_OFFSET 3

class IUIScene_DispenserMenu : public virtual IUIScene_AbstractContainerMenu
{
protected:
	virtual ESceneSection GetSectionAndSlotInDirection( ESceneSection eSection, ETapState eTapDirection, int *piTargetX, int *piTargetY );
	int getSectionStartOffset(ESceneSection eSection);
};