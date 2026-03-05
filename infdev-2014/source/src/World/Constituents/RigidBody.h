#pragma once

#include "./Collider.h"

#include <BulletDynamics/Dynamics/btRigidBody.h>

#include "General/Vector3.h"
#include "Model/Model.h"
#include "../Objects/Transform.h"

namespace Esteem
{
	class RigidBody final : public Collider
	{
	private:
		btRigidBody rigidBody;
		Vector3 offset;
		cgc::strong_ptr<const Model> model;

	public:
		RigidBody();
		RigidBody(const cgc::strong_ptr<Entity>& entity);

		static cgc::strong_ptr<RigidBody> Instantiate(const cgc::strong_ptr<Entity>& entity);

		virtual void Start();

		virtual void Update();
		virtual void LateUpdate();

		virtual void SetEnabled(bool enable);

		virtual void OnDestroy();
				
		void SetLinearVelocity(const Vector3& velocity);
		void ApplyForce(const Vector3& velocity, const Vector3& relativePosition);
		void ApplyImpulse(const Vector3& velocity, const Vector3& relativePosition);
		void ApplyDamping(const float timeStep);
		void ApplyTorque(const Vector3& torque);
		void ApplyTorqueImpulse(const Vector3& torque);

		void SetDamping(float linear, float angular);
		void SetGravity(const Vector3& gravity);

		const Vector3& GetLinearVelocity() const;
		const Vector3& GetAngularVelocity() const;

		void SetType(Collision::ShapeType type);
		void SetSize(const Vector3& size);
		void SetOffset(const Vector3& offset);
	};
}

#include "./RigidBody.inl"