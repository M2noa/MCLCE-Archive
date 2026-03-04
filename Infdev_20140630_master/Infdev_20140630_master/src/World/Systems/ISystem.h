#pragma once

namespace Esteem
{
	class ISystem
	{
	public:
		virtual void Update() = 0;
		virtual void LateUpdate() {}
	};
}