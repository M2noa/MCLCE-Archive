#include "RigidBody.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Utils/Time.h"
#include "Utils/Debug.h"
#include "Physics/Physics.h"
#include "Utils/Data.h"
#include "../Objects/Entity.h"

#include "World/World.h"
#include "Physics/PhysicsFactory.h"

namespace Esteem
{
	cgc::strong_ptr<RigidBody> RigidBody::Instantiate(const cgc::strong_ptr<Entity>& entity)
	{
		World* worldData = entity->GetWorld();

		return worldData->GetWorldConstituents().rigidBodies.Create(entity);
	}

#define entity GetEntity()

	void RigidBody::Start()
	{

	}

	void RigidBody::SetEnabled(bool enable)
	{
		if (this->enabled = enable && entity)
			entity->GetWorld()->Physics().AddPhysicsRigidBody(rigidBody, filterGroup, filterMask);
		else if (rigidBody.isInWorld())
			entity->GetWorld()->Physics().RemovePhysicsRigidBody(rigidBody);
	}
		
	void RigidBody::SetType(Collision::ShapeType type)
	{
		this->collisionType = type;

		if (enabled || rigidBody.isInWorld())
			entity->GetWorld()->Physics().RemovePhysicsRigidBody(rigidBody);

		switch (type)
		{
		case Collision::ShapeType::CUBE:
			PhysicsFactory::CreateCube(rigidBody, size, mass);
			break;
		case Collision::ShapeType::CAPSULE:
			PhysicsFactory::CreateCapsule(rigidBody, size, mass);
			break;
		case Collision::ShapeType::CYLINDER:
			PhysicsFactory::CreateCylinder(rigidBody, size, mass);
			break;
		case Collision::ShapeType::SPHERE:
			PhysicsFactory::CreateSphere(rigidBody, size.x, mass);
			break;
		case Collision::ShapeType::CONE:
			PhysicsFactory::CreateCone(rigidBody, size, mass);
			break;
		case Collision::ShapeType::ELLIPSOID:
			PhysicsFactory::CreateEllipsoid(rigidBody, size, mass);
			break;
		case Collision::ShapeType::TRIANGLES:
		case Collision::ShapeType::CONVEX_HULL:
			if (model != nullptr && !model->meshes.empty())
			{
				PhysicsFactory::CreateConvexHullCollider(rigidBody, model->meshes[0], entity->GetScale(), mass);

				/*physicsObjects.reserve(model->meshes.size());
				for (uint i = 0; i < model->meshes.size(); ++i)
				{
					physicsObjects.push_back(static_cast<IPhysicsRigidbody*>(Physics::GetFactory()->CreateConvexHullCollider(
						this, position, entity->GetRotation(), entity->GetScale(), model->meshes[i], entity->IsDynamic() ? mass : 0.f)));
				}*/
			}
			else
				Debug::LogError("RigidBody::SetType(): could not set to TRIANGLES or CONVEX_HULL because model has not been set");

			break;
		default:
			return;
		}

		PhysicsFactory::SetTransform(rigidBody, entity->GetPosition() + offset, entity->GetDirections());

		if (enabled)
			entity->GetWorld()->Physics().AddPhysicsRigidBody(rigidBody, filterGroup, filterMask);
	}

	void RigidBody::Update()
	{
		if (mass)
		{
			/*physicsObject->WriteModelMatrix(*swapMatrix);
			*swapMatrix = glm::scale(*swapMatrix, entity->GetScale());

			// Swap buffer
			glm::mat4* origMatrix = entity->GetMatrix();
			entity->SetMatrix(swapMatrix);
			swapMatrix = origMatrix;

			glm::mat4& matrix = *entity->GetMatrix();
			entity->SetForward	(Vector3(matrix[0][2], matrix[1][2], matrix[2][2]).Normalized());
			entity->SetUp		(Vector3(matrix[0][1], matrix[1][1], matrix[2][1]).Normalized());
			entity->SetRight	(Vector3(matrix[0][0], matrix[1][0], matrix[2][0]).Normalized());*/

			//deltaTime = 0;

			//entity->SetPosition(physicsObject->GetPosition());
			//position = physicsObject->GetPosition();
			//velocity = Vector3::zero;
			//velocity = physicsObject->GetLinearVelocity();
			//entity->SetRotation(physicsObject->GetRotation());

			//entity->SetDirty();
		}
	}

	void RigidBody::LateUpdate()
	{

	}

	void RigidBody::OnDestroy()
	{
		entity->GetWorld()->Physics().RemovePhysicsObject(rigidBody);
	}
}
#undef entity