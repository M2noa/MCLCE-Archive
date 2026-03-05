#pragma once

#include "./KinematicBody.h"

#include "World/Objects/Entity.h"

namespace Esteem
{
	inline bool KinematicBody::GetEnabled()
	{
		return collisionObject.getBroadphaseProxy() != nullptr;
	}

	inline void KinematicBody::SetSize(const Vector3& size)
	{
		this->size = size;
	}

	inline void KinematicBody::SetOffset(const Vector3& offset)
	{
		this->offset = reinterpret_cast<const btVector3&>(offset);
	}
	
	inline void KinematicBody::SetPosition(const Vector3& position)
	{
		collisionObject.getWorldTransform().setOrigin(reinterpret_cast<const btVector3&>(position) + offset);
	}

	inline void KinematicBody::SetRotation(const Quaternion& rotation)
	{
		collisionObject.getWorldTransform().setRotation({ rotation.x, rotation.y, rotation.z, rotation.w });
	}

	inline const Vector3& KinematicBody::GetOffset() const
	{
		return reinterpret_cast<const Vector3&>(offset);
	}

	inline Vector3 KinematicBody::GetOffsettedPosition() const
	{
		btVector3 position = collisionObject.getWorldTransform() * offset;
		return reinterpret_cast<const Vector3&>(position);
	}

	inline const Vector3& KinematicBody::GetPosition() const
	{
		return reinterpret_cast<const Vector3&>(currentPosition);
	}

	inline Quaternion KinematicBody::GetRotation() const
	{
		btQuaternion quat = collisionObject.getOrientation();
		return { quat.w(), quat.x(), quat.y(), quat.z() };
	}

	inline void KinematicBody::Move(const Vector3& velocity)
	{
		this->moveDirection = btVector3(velocity.x, velocity.y, velocity.z);
	}

	inline void KinematicBody::AddForce(const Vector3& force)
	{
		//physicsCharacter->AddForce(force);
	}

	inline const Vector3& KinematicBody::GetVelocity() const
	{
		return reinterpret_cast<const Vector3&>(velocity);
	}

	inline const Vector3& KinematicBody::GetHitNormal() const
	{
		return reinterpret_cast<const Vector3&>(hitNormal);
	}

	inline bool KinematicBody::IsGrounded() const
	{
		return grounded;
	}

	inline bool KinematicBody::IsSwimming() const
	{
		return swimming;
	}

	inline void KinematicBody::SetCharacterState(uint32_t state)
	{
		swimming |= (state >> (CharacterState::CS_IN_LIQUID - 1)) & 1;
		onLadder |= (state >> (CharacterState::CS_ON_LADDER - 1)) & 1;
	}

	inline void KinematicBody::UnsetCharacterState(uint32_t state)
	{
		swimming ^= (state >> (CharacterState::CS_IN_LIQUID - 1)) & 1;
		onLadder ^= (state >> (CharacterState::CS_ON_LADDER - 1)) & 1;
	}

	inline void KinematicBody::SetFilterGroup(uint32_t filterGroup)
	{
		this->filterGroup = filterGroup;
	}

	inline void KinematicBody::SetFilterMask(uint32_t filterMask)
	{
		this->filterMask = filterMask;
	}

	inline void KinematicBody::Interpolate(float delta)
	{
		btVector3 interpolatedPosition = (prevPosition + (currentPosition - prevPosition) * delta) - offset;
		GetEntity()->SetPosition(reinterpret_cast<const glm::vec3&>(interpolatedPosition));
		GetEntity()->SetDirty();
	}
}