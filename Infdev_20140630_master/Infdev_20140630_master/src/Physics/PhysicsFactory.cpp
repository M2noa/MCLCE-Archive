#include "PhysicsFactory.h"

#include <cppu/cgc/constructor.h>

#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletCollision/CollisionShapes/btCylinderShape.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <BulletCollision/CollisionShapes/btConeShape.h>
#include <BulletCollision/CollisionShapes/btMultiSphereShape.h>
#include <BulletCollision/CollisionShapes/btStaticPlaneShape.h>
#include <BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btConvexHullShape.h>
#include <BulletCollision/CollisionShapes/btShapeHull.h>

#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>

#include "Utils/Debug.h"

#include "PhysicsSettings.h"

namespace Esteem
{

	std::unordered_map<const AbstractMesh*, btBvhTriangleMeshShape*> PhysicsFactory::cachedMeshShapes;
	std::unordered_map<const AbstractMesh*, btConvexShape*> PhysicsFactory::cachedConvexShapes;

	void PhysicsFactory::SetTransform(btCollisionObject& collisionObject, const glm::vec3& position)
	{
		collisionObject.getWorldTransform().getBasis().setIdentity();
		collisionObject.getWorldTransform().setOrigin(reinterpret_cast<const btVector3&>(position));
	}

	void PhysicsFactory::SetTransform(btCollisionObject& collisionObject, const glm::vec3& position, const glm::mat3& rotation)
	{
		reinterpret_cast<glm::mat3x4&>(collisionObject.getWorldTransform().getBasis()) = glm::transpose(rotation);
		collisionObject.getWorldTransform().setOrigin(reinterpret_cast<const btVector3&>(position));
	}

	void PhysicsFactory::SetCollisionCallback(btCollisionObject& object, Collidable* collisionCallback)
	{
		object.setUserPointer(collisionCallback);
	}

	void PhysicsFactory::CreatePlane(btCollisionObject& object, const glm::vec4& plane)
	{
		SetShape(object, new btStaticPlaneShape(reinterpret_cast<const btVector3&>(plane), plane.w));
	}

	void PhysicsFactory::CreateEllipsoid(btCollisionObject& object, const glm::vec3& size)
	{
		btVector3 zero(0, 0, 0);
		SetShape(object, new btMultiSphereShape(&zero, &size.x, 1));
	}

	void PhysicsFactory::CreateEllipsoid(btRigidBody& rigidBody, const glm::vec3& size, float mass)
	{
		btVector3 zero(0, 0, 0);
		SetShape(rigidBody, new btMultiSphereShape(&zero, &size.x, 1), mass);
	}

	void PhysicsFactory::CreateCone(btCollisionObject& object, const glm::vec2& size)
	{
		SetShape(object, new btConeShape(size.x, size.y));
	}

	void PhysicsFactory::CreateCone(btRigidBody& rigidBody, const glm::vec2& size, float mass)
	{
		SetShape(rigidBody, new btConeShape(size.x, size.y), mass);
	}

	void PhysicsFactory::CreateCapsule(btCollisionObject& object, const glm::vec2& size)
	{
		SetShape(object, new btCapsuleShape(size.x, size.y));
	}

	void PhysicsFactory::CreateCapsule(btRigidBody& rigidBody, const glm::vec2& size, float mass)
	{
		SetShape(rigidBody, new btCapsuleShape(size.x, size.y), mass);
	}

	void PhysicsFactory::CreateCube(btCollisionObject& object, const glm::vec3& size)
	{
		SetShape(object, new btBoxShape(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f)));
	}

	void PhysicsFactory::CreateCube(btRigidBody& rigidBody, const glm::vec3& size, float mass)
	{
		SetShape(rigidBody, new btBoxShape(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f)), mass);
	}

	void PhysicsFactory::CreateSphere(btCollisionObject& object, float radial)
	{
		SetShape(object, new btSphereShape(radial));
	}

	void PhysicsFactory::CreateSphere(btRigidBody& rigidBody, float radial, float mass)
	{
		SetShape(rigidBody, new btSphereShape(radial), mass);
	}

	void PhysicsFactory::CreateCylinder(btCollisionObject& object, const glm::vec3& size)
	{
		SetShape(object, new btCylinderShape(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f)));
	}

	void PhysicsFactory::CreateCylinder(btRigidBody& rigidBody, const glm::vec3& size, float mass)
	{
		SetShape(rigidBody, new btCylinderShape(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f)), mass);
	}

	void PhysicsFactory::CreateStaticMeshCollider(btCollisionObject& object, const cgc::strong_ptr<AbstractMesh>& mesh, const glm::vec3& scale)
	{
		SetShape(object, GetOrCreateConcaveMeshShape(mesh, scale));
	}

	void PhysicsFactory::CreateConvexHullCollider(btCollisionObject& object, const cgc::strong_ptr<AbstractMesh>& mesh, const glm::vec3& scale)
	{
		SetShape(object, GetOrCreateConvexHullShape(mesh, scale));
	}

	void PhysicsFactory::CreateConvexHullCollider(btRigidBody& rigidBody, const cgc::strong_ptr<AbstractMesh>& mesh, const glm::vec3& scale, float mass)
	{
		SetShape(rigidBody, GetOrCreateConvexHullShape(mesh, scale), mass);
	}

	void PhysicsFactory::CreateTerrain(btCollisionObject& object, btHeightfieldTerrainShape* terrainShape)
	{
		SetShape(object, terrainShape);
	}

	void PhysicsFactory::CreateCustomConcaveCollider(btCollisionObject& object, const CustomConcaveCollider& concaveCollider)
	{
		PhysicsSettings::customStaticConstructor(object, concaveCollider);
	}

	void PhysicsFactory::CreateCustomConcaveCollider(btRigidBody& rigidBody, const CustomConcaveCollider& concaveCollider, float mass)
	{
		PhysicsSettings::customRigidConstructor(rigidBody, concaveCollider, mass);
	}

	void PhysicsFactory::SetShape(btCollisionObject& object, const btCollisionShape* shape)
	{
		object.setCollisionShape(const_cast<btCollisionShape*>(shape));
	}

	void PhysicsFactory::SetShape(btRigidBody& rigidBody, const btCollisionShape* shape, float mass)
	{
		btVector3 inertia = btVector3(0, 0, 0);
		if (mass != 0.f)
			shape->calculateLocalInertia(mass, inertia);

		rigidBody.setCollisionShape(const_cast<btCollisionShape*>(shape));
		rigidBody.setMassProps(mass, inertia);
	}

	btConcaveShape* PhysicsFactory::GetOrCreateConcaveMeshShape(const cgc::strong_ptr<AbstractMesh>& mesh, const glm::vec3& scale)
	{
		btBvhTriangleMeshShape* triangleMeshShape = nullptr;

		auto found = cachedMeshShapes.find(mesh.ptr());
		if (found != cachedMeshShapes.end())
			triangleMeshShape = found->second;
		else
		{
			btTriangleIndexVertexArray* meshInterface = new btTriangleIndexVertexArray();
			btIndexedMesh part;
			const cgc::strong_ptr<IMeshData>& meshData = mesh->GetMeshData();

			mm::array_view vertexData = meshData->GetVertexMemInfo();
			if (!vertexData.empty())
			{
				part.m_vertexBase = (unsigned char*)vertexData.data();
				part.m_vertexStride = sizeof(vertexData.type_size());
				part.m_numVertices = vertexData.size();
				part.m_vertexType = PHY_FLOAT;
			}

			mm::array_view indexData = meshData->GetIndexMemInfo();
			if (!indexData.empty())
			{
				part.m_triangleIndexBase = (unsigned char*)indexData.data();
				part.m_triangleIndexStride = indexData.type_size() * 3;
				part.m_numTriangles = indexData.size() / 3;
				part.m_indexType = PHY_INTEGER;
			}

			meshInterface->addIndexedMesh(part, PHY_INTEGER);

			bool useQuantizedAabbCompression = true;

			triangleMeshShape = new btBvhTriangleMeshShape(meshInterface, useQuantizedAabbCompression);
			cachedMeshShapes.emplace(std::piecewise_construct, std::forward_as_tuple(mesh.ptr()), std::forward_as_tuple(triangleMeshShape));
		}
		
		return scale == glm::vec3(1.f)
			? triangleMeshShape
			: static_cast<btConcaveShape*>(new btScaledBvhTriangleMeshShape(triangleMeshShape, reinterpret_cast<const btVector3&>(scale)));
	}

	btConvexShape* PhysicsFactory::GetOrCreateConvexHullShape(const cgc::strong_ptr<AbstractMesh>& mesh, const glm::vec3& scale)
	{

		auto found = cachedConvexShapes.find(mesh.ptr());
		if (found != cachedConvexShapes.end())
			return found->second;

		mm::array_view vertexData = mesh->GetMeshData()->GetVertexMemInfo();

		btConvexHullShape tempShape(static_cast<const btScalar*>(vertexData.data()), vertexData.size(), vertexData.type_size());
		btShapeHull hull(&tempShape);
		hull.buildHull(tempShape.getMargin());

		btConvexHullShape* shape = new btConvexHullShape(&hull.getVertexPointer()->x(), hull.numVertices());
		shape->setLocalScaling(reinterpret_cast<const btVector3&>(scale));

		cachedConvexShapes.emplace(std::piecewise_construct, std::forward_as_tuple(mesh.ptr()), std::forward_as_tuple(shape));

		return shape;
	}
}