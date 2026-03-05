#include "StaticBody.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Utils/Time.h"
#include "Utils/Debug.h"
#include "Physics/Physics.h"
#include "Physics/PhysicsFactory.h"
#include "Utils/Data.h"
#include "../Objects/Entity.h"

#include "World/World.h"

namespace Esteem
{
	cgc::strong_ptr<StaticBody> StaticBody::Instantiate(const cgc::strong_ptr<Entity>& entity)
	{
		World* worldData = entity->GetWorld();

		return worldData->GetWorldConstituents().staticBodies.Create(entity);
	}

#define entity GetEntity()

	void StaticBody::SetEnabled(bool enable)
	{
		if (enable)
		{
			if (!this->enabled && entity)
			{
				this->enabled = true;
				PhysicsFactory::SetTransform(collisionObject, entity->GetPosition(), entity->GetDirections());
				entity->GetWorld()->Physics().AddPhysicsObject(collisionObject, filterGroup, filterMask);
			}
		}
		else if (collisionObject.getBroadphaseHandle())
		{
			this->enabled = false;
			entity->GetWorld()->Physics().RemovePhysicsObject(collisionObject);
		}
	}

	void StaticBody::SetType(Collision::ShapeType type)
	{
		this->collisionType = type;

		if (collisionObject.getBroadphaseHandle())
			entity->GetWorld()->Physics().RemovePhysicsObject(collisionObject);

		switch (type)
		{
		case Collision::ShapeType::CUBE:
			PhysicsFactory::CreateCube(collisionObject, size);
			break;
		case Collision::ShapeType::CAPSULE:
			PhysicsFactory::CreateCapsule(collisionObject, size);
			break;
		case Collision::ShapeType::CYLINDER:
			PhysicsFactory::CreateCylinder(collisionObject, size);
			break;
		case Collision::ShapeType::SPHERE:
			PhysicsFactory::CreateSphere(collisionObject, size.x);
			break;
		case Collision::ShapeType::CONE:
			PhysicsFactory::CreateCone(collisionObject, size);
			break;
		case Collision::ShapeType::ELLIPSOID:
			PhysicsFactory::CreateEllipsoid(collisionObject, size);
			break;
		case Collision::ShapeType::PLANE:
			PhysicsFactory::CreatePlane(collisionObject, glm::vec4(entity->GetDirections() * Vector3::up, 0.f));
			break;
		case Collision::ShapeType::CONVEX_HULL:
			if (model != nullptr)
				PhysicsFactory::CreateConvexHullCollider(collisionObject, model->meshes[0], size);
			else
				Debug::LogError("StaticBody::SetType(): could not set to CONVEX_HULL because model has not been set");

			break;
		case Collision::ShapeType::TRIANGLES:
			if (model != nullptr)
				PhysicsFactory::CreateStaticMeshCollider(collisionObject, model->meshes[0], size);
			else
				Debug::LogError("StaticBody::SetType(): could not set to TRIANGLES because model has not been set");

			break;
		}

		if (enabled)
			entity->GetWorld()->Physics().AddPhysicsObject(collisionObject, filterGroup, filterMask);
	}

	void StaticBody::SetSize(const Vector3& size)
	{
		this->size = size;
		// TODO: update rigidbody
	}

	StaticBody::~StaticBody()
	{
		if (collisionObject.getBroadphaseHandle())
			entity->GetWorld()->Physics().RemovePhysicsObject(collisionObject);
	}

	void StaticBody::LoadModel(const std::string& path)
	{
		std::string extensionLess = path.substr(0, path.rfind('.'));
		model = Data::GetModelFactory().LoadModel(path, extensionLess, Model::ModelGenerateSettings::GENERATE_NORMALS);
	}

	void StaticBody::SetAsTrigger(bool enable)
	{
		Physics& physics = entity->GetWorld()->Physics();
		
		physics.RemovePhysicsObject(collisionObject);

		collisionObject.setCollisionFlags(enable
			? collisionObject.getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE
			: collisionObject.getCollisionFlags() ^ btCollisionObject::CF_NO_CONTACT_RESPONSE);

		physics.AddPhysicsObject(collisionObject, filterGroup, filterMask);
	}
}
#undef entity