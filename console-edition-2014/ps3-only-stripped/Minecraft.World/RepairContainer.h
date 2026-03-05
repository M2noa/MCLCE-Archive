#pragma once

#include "SimpleContainer.h"

class RepairMenu;

class RepairContainer : public SimpleContainer, public enable_shared_from_this<RepairContainer>
{
private:
	RepairMenu *m_menu;

public:
	RepairContainer(RepairMenu *menu, int name, int size);
	void setChanged();
};