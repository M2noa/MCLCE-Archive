#include "stdafx.h"
#include "..\..\..\Minecraft.World\ItemInstance.h"
#include "..\UI\UI.h"
#include "Tutorial.h"
#include "XuiCraftingTask.h"

bool XuiCraftingTask::isCompleted()
{
	// This doesn't seem to work
	//IUIScene_CraftingMenu *craftScene = reinterpret_cast<IUIScene_CraftingMenu *>(tutorial->getScene());
	UIScene_CraftingMenu *craftScene = reinterpret_cast<UIScene_CraftingMenu *>(tutorial->getScene());

	bool completed = false;

	switch(m_type)
	{
	case e_Crafting_SelectGroup:
		if(craftScene != NULL && craftScene->getCurrentGroup() == m_group)
		{
			completed = true;
		}
		break;
	case e_Crafting_SelectItem:
		if(craftScene != NULL && craftScene->isItemSelected(m_item))
		{
			completed = true;
		}
		break;
	}

	return completed;
}
