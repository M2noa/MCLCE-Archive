#pragma once

namespace Esteem
{
	class IInputSystem
	{
	public:
		virtual ~IInputSystem() = 0;
		virtual void Update() = 0;
	};

	inline IInputSystem::~IInputSystem() {};
}