#include "WaterTrigger.h"
#include "../Constituents/RigidBody.h"

namespace Esteem
{
	void WaterTrigger::TriggerStarted(Collider* other)
	{
		if (other->GetCollidableType() == Collider::Type::RIGID_BODY)
		{
			static_cast<RigidBody*>(other)->SetDamping(0.95f, 0.95f);
		}
	}

	void WaterTrigger::TriggerActive(Collider* other)
	{
		// is this a rigidbody?
		if (other->GetCollidableType() == Collider::Type::RIGID_BODY)
		{
			//static_cast<RigidBody*>(collider)->SetDamping(0.95f, 0.95f);
			static_cast<RigidBody*>(other)->ApplyForce(Vector3(0, 15, 0), Vector3(0, 0, 0));
		}
	}

	void WaterTrigger::TriggerEnded(Collider* other)
	{
		if (other->GetCollidableType() == Collider::Type::RIGID_BODY)
		{
			static_cast<RigidBody*>(other)->SetDamping(0.f, 0.f);
		}
	}
}