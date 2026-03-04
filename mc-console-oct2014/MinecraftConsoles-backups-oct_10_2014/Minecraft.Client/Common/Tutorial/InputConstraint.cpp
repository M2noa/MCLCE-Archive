#include "stdafx.h"
#include "InputConstraint.h"

bool InputConstraint::isMappingConstrained(int iPad, int mapping)
{
	// If it's a menu button, then we ignore all inputs
	if((m_inputMapping == mapping) || (mapping < ACTION_MAX_MENU))
	{
		return true;
	}
	
	// Otherwise see if they map to the same actual button
	unsigned char layoutMapping = InputManager.GetJoypadMapVal( iPad );

	// 4J HEG - Replaced the equivalance test with bitwise AND, important in some mapping configurations
	// (e.g. when comparing two action map values and one has extra buttons mapped)
	return (InputManager.GetGameJoypadMaps(layoutMapping,m_inputMapping) & InputManager.GetGameJoypadMaps(layoutMapping,mapping)) > 0;
}
