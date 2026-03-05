#pragma once

namespace Esteem
{
	class World;

	class IWorldObject
	{
	public:
		virtual ~IWorldObject() {};

		virtual void Update() = 0;

		virtual void LateUpdate() = 0;

		virtual World* GetWorld() = 0;

		virtual void DirtyCleanUp() {};
	};
}