#pragma once

#include "General/Delegate.h"
#include "Physics/PhysicsFactory.h"

#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>
#include <BulletSoftBody/btDefaultSoftBodySolver.h>

class btDispatcher;
class btBroadphaseInterface;
class btConstraintSolver;
class btDefaultSoftBodySolver;
class btCollisionConfiguration;

namespace Esteem
{
	typedef btDiscreteDynamicsWorld*(*CustomWorldCreatorFunc)(btDispatcher* dispatcher, btBroadphaseInterface* broadPhaseAlgorithm, btConstraintSolver* constraintSolver, btCollisionConfiguration* collisionConfig, btSoftBodySolver* softBodySolver);

	class PhysicsObject;

	class PhysicsSettings
	{
	public:
		static btCollisionConfiguration* overrideBulletPhysicsConfiguration;

		static CustomStaticConstructorFunc customStaticConstructor;
		static CustomRigidConstructorFunc customRigidConstructor;
		static CustomWorldCreatorFunc customWorldCreator;

		static void DummyStaticConstructor(btCollisionObject& object, const CustomConcaveCollider& concaveCollider)
		{
			static_assert("PhysicsSettings::customStaticShapeBuilder has not been set, custom shapes are not available.");
			throw("PhysicsSettings::customStaticShapeBuilder has not been set, custom shapes are not available.");
		}

		static void DummyRigidConstructor(btRigidBody& object, const CustomConcaveCollider& concaveCollider, float mass)
		{
			static_assert("PhysicsSettings::customRigidShapeBuilder has not been set, custom shapes are not available.");
			throw("PhysicsSettings::customRigidShapeBuilder has not been set, custom shapes are not available.");
		}

		static btDiscreteDynamicsWorld* CreateWorld(btDispatcher* dispatcher, btBroadphaseInterface* broadPhaseAlgorithm, btConstraintSolver* constraintSolver, btCollisionConfiguration* collisionConfig, btSoftBodySolver* softBodySolver = nullptr)
		{
			return softBodySolver != nullptr
				? new btSoftRigidDynamicsWorld(dispatcher, broadPhaseAlgorithm, constraintSolver, collisionConfig, softBodySolver)
				: new btDiscreteDynamicsWorld(dispatcher, broadPhaseAlgorithm, constraintSolver, collisionConfig);
		}
	};
}