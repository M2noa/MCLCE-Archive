#pragma once

#include <glm/vec3.hpp>

class btPersistentManifold;

namespace Esteem
{
	class Collider;
	class StaticBody;
	class RigidBody;
	class KinematicBody;
	class Physics;

	class Collidable abstract
	{
	friend class Physics;

	public:
		enum class Type : uint8_t
		{
			NONE = 0,
			STATIC_BODY,
			RIGID_BODY,
			CHARACTER,
			VOXEL
		};

	private:
		Type collidableType;

	protected:
		Collidable(Type type);

		virtual void TriggerStarted(Collidable* other);
		virtual void TriggerContinuous(Collidable* other);
		virtual void TriggerEnded(Collidable* other);

		virtual void CollisionStarted(Collidable* other, const btPersistentManifold& collisionInfo);
		virtual void CollisionContinuous(Collidable* other, const btPersistentManifold& collisionInfo);
		virtual void CollisionEnded(Collidable* other, const btPersistentManifold& collisionInfo);

	public:
		Type GetCollidableType() const;

		virtual ~Collidable() = default;
	};

	inline Collidable::Collidable(Type type)
		: collidableType(type)
	{ }

	inline Collidable::Type Collidable::GetCollidableType() const { return collidableType; }

	inline void Collidable::TriggerStarted(Collidable* other) { }
	inline void Collidable::TriggerContinuous(Collidable* other) { }
	inline void Collidable::TriggerEnded(Collidable* other) { }

	inline void Collidable::CollisionStarted(Collidable* other, const btPersistentManifold& collisionInfo) { }
	inline void Collidable::CollisionContinuous(Collidable* other, const btPersistentManifold& collisionInfo) { }
	inline void Collidable::CollisionEnded(Collidable* other, const btPersistentManifold& collisionInfo) { }
}
/*
#if !__has_include(<StaticBody.h>)
#include "World/Constituents/StaticBody.h"
#endif

#if !__has_include(<RigidBody.h>)
#include "World/Constituents/RigidBody.h"
#endif

#if !__has_include(<KinematicBody.h>)
#include "World/Constituents/KinematicBody.h"
#endif
*/

namespace Esteem
{
	/*inline Collidable::operator StaticBody* () { return collidableType == Type::STATIC_BODY ? static_cast<StaticBody*>(this) : nullptr; }
	inline Collidable::operator RigidBody* () { return collidableType == Type::RIGID_BODY ? static_cast<RigidBody*>(this) : nullptr; }
	inline Collidable::operator KinematicBody* () { return collidableType == Type::CHARACTER ? static_cast<KinematicBody*>(this) : nullptr; }*/
}