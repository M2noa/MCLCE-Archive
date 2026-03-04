#pragma once

#include "./Collider.h"
#include <atomic>
#include <unordered_set>

#include <BulletDynamics/Dynamics/btRigidBody.h>
#include "BulletDynamics/Dynamics/btActionInterface.h"
#include "BulletCollision/BroadphaseCollision/btCollisionAlgorithm.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

#include "General/Vector3.h"

namespace Esteem
{
	class Physics;

	class KinematicBody final : public Collider, public btActionInterface
	{
	friend class cgc::constructor;
	friend class Physics;

	private:
		btRigidBody collisionObject;

		btVector3 up;
		btVector3 right;

		// states
		std::unordered_set<const btCollisionObject*> triggers;

		btVector3 velocity;
		btVector3 moveDirection;
		btVector3 hitNormal;

		btVector3 prevPosition;
		btVector3 currentPosition;
		btVector3 offset;
		btScalar margin;

		float stepHeight;
		float maxSlopeCosine;

		float verticalMovement;
		float verticalOffset;

		int filterGroup;
		int filterMask;

		bool grounded;
		bool swimming;
		bool onLadder;

		KinematicBody(const cgc::strong_ptr<Entity>& entity, const Vector3& position, const Vector3& up);

		bool RecoverFromPenetration(btCollisionWorld* collisionWorld);

		void Move(btCollisionWorld* collisionWorld, const btVector3& moveDirection);
		void StepUp(btCollisionWorld* collisionWorld, const btVector3& forward);
		bool StepDown(btCollisionWorld* collisionWorld);

		///btActionInterface interface
		void updateAction(btCollisionWorld* collisionWorld, btScalar deltaTime);
		void debugDraw(btIDebugDraw* debugDrawer) { };

	public:
		static cgc::strong_ptr<KinematicBody> Instantiate(const cgc::strong_ptr<Entity>& entity, World& world, const Vector3& position, const Vector3& up = Vector3::up);
		~KinematicBody();

		void SetEnabled(bool enable);
		bool GetEnabled();

		const Vector3& GetOffset() const;
		void SetOffset(const Vector3& offset);

		void SetType(Collision::ShapeType type);
		void SetSize(const Vector3& size);

		const Vector3& GetPosition() const;
		void SetPosition(const Vector3& position);

		Quaternion GetRotation() const;
		void SetRotation(const Quaternion& position);

		Vector3 GetOffsettedPosition() const;
		const Vector3& GetHitNormal() const;

		void SetCharacterState(uint32_t state);
		void UnsetCharacterState(uint32_t state);
		
		void SetFilterGroup(uint32_t filterGroup);
		void SetFilterMask(uint32_t filterMask);
		
		bool IsGrounded() const;
		bool IsSwimming() const;

		void Move(const Vector3& velocity);
		void AddForce(const Vector3& force);
		const Vector3& GetVelocity() const;

		void Interpolate(float delta);
	};
}

#include "./KinematicBody.inl"
