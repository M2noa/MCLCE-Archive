#pragma once

#include <vector>
#include "./Mouse.h"
#include "./Keyboard.h"
#include "GameController.h"

namespace Esteem
{
	class Input
	{
	private:
		static bool enabled;
		static Mouse mouse;
		static Keyboard keyboard;
		static std::vector<GameController> controllers;
		static std::vector<IInputSystem*> inputSystems;

	public:
		static void Update();

		static void Enable();
		static void Disable();

		static Mouse& GetMouse() { return mouse; }		
		static Keyboard& GetKeyBoard() { return keyboard; }

		static std::vector<GameController>& GetGameControllers() { return controllers; }
		static GameController& GetGameController(int index) { return controllers[index]; }

		static std::vector<IInputSystem*>& GetInputSystems() { return inputSystems; }
		static IInputSystem* GetInputSystem(int index) { return inputSystems[index]; }
	};
}