#include "stdafx.h"
#include "net.minecraft.world.inventory.h"
#include "EnchantmentContainer.h"

EnchantmentContainer::EnchantmentContainer(EnchantmentMenu *menu) : SimpleContainer(IDS_ENCHANT, 1), m_menu( menu )
{
}

int EnchantmentContainer::getMaxStackSize()
{
	return 1;
}

void EnchantmentContainer::setChanged()
{
	SimpleContainer::setChanged();
	m_menu->slotsChanged(); // Remove this param as it's not needed
}