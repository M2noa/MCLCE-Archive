#pragma once

#include "./RigidBody.h"

namespace Esteem
{
	inline RigidBody::RigidBody()
		: Collider(cgc::strong_ptr<Entity>(), Type::RIGID_BODY)
		, rigidBody(0.f, nullptr, nullptr)
	{ }

	inline RigidBody::RigidBody(const cgc::strong_ptr<Entity>& entity)
		: Collider(entity, Type::RIGID_BODY)
		, rigidBody(0.f, nullptr, nullptr)
	{ }

	inline void RigidBody::SetOffset(const Vector3 & offset)
	{
		this->offset = offset;
	}

	inline void RigidBody::SetSize(const Vector3 & size)
	{
		this->size = size;
	}

	inline void RigidBody::SetLinearVelocity(const Vector3& velocity)
	{
		rigidBody.setLinearVelocity(reinterpret_cast<const btVector3&>(velocity));
	}

	inline void RigidBody::ApplyForce(const Vector3& velocity, const Vector3& relativePosition)
	{
		rigidBody.applyForce(reinterpret_cast<const btVector3&>(velocity), reinterpret_cast<const btVector3&>(relativePosition));
	}

	inline void RigidBody::ApplyImpulse(const Vector3& velocity, const Vector3& relativePosition)
	{ 
		rigidBody.applyImpulse(reinterpret_cast<const btVector3&>(velocity), reinterpret_cast<const btVector3&>(relativePosition));
	}

	inline void RigidBody::ApplyDamping(float timeStep)
	{
		rigidBody.applyDamping(timeStep);
	}

	inline void RigidBody::ApplyTorque(const Vector3& torque)
	{
		rigidBody.applyTorque(reinterpret_cast<const btVector3&>(torque));
	}

	inline void RigidBody::ApplyTorqueImpulse(const Vector3& torque)
	{
		rigidBody.applyTorqueImpulse(reinterpret_cast<const btVector3&>(torque));
	}
	
	inline void RigidBody::SetDamping(float linear, float angular)
	{
		rigidBody.setDamping(linear, angular);
	}

	inline void RigidBody::SetGravity(const Vector3& gravity)
	{
		rigidBody.setGravity(reinterpret_cast<const btVector3&>(gravity));
	}

	inline const Vector3& RigidBody::GetLinearVelocity() const
	{
		return reinterpret_cast<const Vector3&>(rigidBody.getLinearVelocity());
	}

	inline const Vector3& RigidBody::GetAngularVelocity() const
	{
		return reinterpret_cast<const Vector3&>(rigidBody.getAngularVelocity());
	}
}