#pragma once

#include "Physics/Collidable.h"

namespace Esteem
{
	class WaterTrigger : public Collidable
	{
	public:
		WaterTrigger()
			: Collidable(Collidable::Type::NONE)
		{ }

		virtual void TriggerStarted(Collider* other);
		virtual void TriggerActive(Collider* other);
		virtual void TriggerEnded(Collider* other);
	};
}