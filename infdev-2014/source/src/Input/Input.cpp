#include "Input.h"

namespace Esteem
{
	bool Input::enabled = true;
	Mouse Input::mouse = Mouse();
	Keyboard Input::keyboard = Keyboard();
	std::vector<GameController> Input::controllers = std::vector<GameController>();
	std::vector<IInputSystem*> Input::inputSystems = std::vector<IInputSystem*>();

	void Input::Update()
	{
		if (enabled)
		{
			keyboard.Update();
			mouse.Update();
		}
	}

	void Input::Enable()
	{
		enabled = true;
		mouse.Enable();
	}

	void Input::Disable()
	{
		enabled = false;
		mouse.Disable();
	}
}