#pragma once

#include "stdafx.h"

#include <vector>
#include <unordered_map>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include "General/Delegate.h"

class btSoftRigidDynamicsWorld;
class btDiscreteDynamicsWorld;

class btHeightfieldTerrainShape;
class btBvhTriangleMeshShape;
class btCollisionShape;
class btConcaveShape;
class btConvexShape;

class btCollisionObject;
class btRigidBody;

namespace cppu::cgc
{
	template<class T>
	class strong_ptr;
}

namespace Esteem
{
	class Collidable;
	class RigidBody;
	class PhysicsCharacter;

	class AbstractMesh;

	struct CustomConcaveCollider
	{
		uint32_t type;
		void* object;

		CustomConcaveCollider() = default;

		CustomConcaveCollider(uint32_t type, void* object)
			: type(type)
			, object(object)
		{ }
	};

	typedef void(*CustomStaticConstructorFunc)(btCollisionObject& object, const CustomConcaveCollider& concaveCollider);
	typedef void(*CustomRigidConstructorFunc)(btRigidBody& rigidBody, const CustomConcaveCollider& concaveCollider, float mass);

	class PhysicsFactory
	{
	private:
		static std::unordered_map<const AbstractMesh*, btBvhTriangleMeshShape*> cachedMeshShapes;
		static std::unordered_map<const AbstractMesh*, btConvexShape*> cachedConvexShapes;

	public:
		static void SetCollisionCallback(btCollisionObject& object, Collidable* collisionCallback);

		static void SetTransform(btCollisionObject& collisionObject, const glm::vec3& position);
		static void SetTransform(btCollisionObject& collisionObject, const glm::vec3& position, const glm::mat3& rotation);

		static void CreatePlane(btCollisionObject& object, const glm::vec4& plane);

		static void CreateEllipsoid(btCollisionObject& object, const glm::vec3& size);
		static void CreateEllipsoid(btRigidBody& rigidBody, const glm::vec3& size, float mass);

		static void CreateCone(btCollisionObject& object, const glm::vec2& size);
		static void CreateCone(btRigidBody& rigidBody, const glm::vec2& size, float mass);

		static void CreateCapsule(btCollisionObject& object, const glm::vec2& size);
		static void CreateCapsule(btRigidBody& rigidBody, const glm::vec2& size, float mass);

		static void CreateCube(btCollisionObject& object, const glm::vec3& size);
		static void CreateCube(btRigidBody& rigidBody, const glm::vec3& size, float mass);

		static void CreateSphere(btCollisionObject& object, float radial);
		static void CreateSphere(btRigidBody& rigidBody, float radial, float mass);

		static void CreateCylinder(btCollisionObject& object, const glm::vec3& size);
		static void CreateCylinder(btRigidBody& rigidBody, const glm::vec3& size, float mass);

		static void CreateStaticMeshCollider(btCollisionObject& object, const cgc::strong_ptr<AbstractMesh>& mesh, const glm::vec3& scale);

		static void CreateConvexHullCollider(btCollisionObject& object, const cgc::strong_ptr<AbstractMesh>& mesh, const glm::vec3& scale);
		static void CreateConvexHullCollider(btRigidBody& rigidBody, const cgc::strong_ptr<AbstractMesh>& mesh, const glm::vec3& scale, float mass);

		static void CreateTerrain(btCollisionObject& object, btHeightfieldTerrainShape* terrainShape);

		// Custom
		static void CreateCustomConcaveCollider(btCollisionObject& object, const CustomConcaveCollider& concaveCollider);
		static void CreateCustomConcaveCollider(btRigidBody& rigidBody, const CustomConcaveCollider& concaveCollider, float mass);

	private:
		static void SetShape(btCollisionObject& object, const btCollisionShape* shape);
		static void SetShape(btRigidBody& rigidBody, const btCollisionShape* shape, float mass);

		static btConcaveShape* GetOrCreateConcaveMeshShape(const cgc::strong_ptr<AbstractMesh>& mesh, const glm::vec3& scale);
		static btConvexShape* GetOrCreateConvexHullShape(const cgc::strong_ptr<AbstractMesh>& mesh, const glm::vec3& scale);
	};
}