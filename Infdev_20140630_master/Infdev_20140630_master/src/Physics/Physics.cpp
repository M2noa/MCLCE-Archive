#include "Physics.h"

#include "Utils/Debug.h"
//#include "World/WorldController.h"
//#include "World/World.h"
#include "Utils/Time.h"
#include "General/Settings.h"
#include "PhysicsSettings.h"

// bullet includes
#include <btBulletDynamicsCommon.h>
#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>
#include <BulletSoftBody/btDefaultSoftBodySolver.h>
#include <BulletSoftBody/btSoftBodyHelpers.h>
#include <BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h>

#include "Collidable.h"
#include "RayCast.h"
#include "World/Constituents/Collider.h"
#include "World/Constituents/RigidBody.h"
#include "World/Constituents/KinematicBody.h"

#include "World/Objects/Entity.h"

namespace Esteem
{
	Physics::Physics(bool fixedSizeWorld, bool enableSoftBody)
	{
		collisionConfig = PhysicsSettings::overrideBulletPhysicsConfiguration;
		if (collisionConfig == nullptr)
			collisionConfig = enableSoftBody ? new btSoftBodyRigidBodyCollisionConfiguration() : new btDefaultCollisionConfiguration();

		dispatcher = new btCollisionDispatcher(collisionConfig);
		broadPhaseAlgorithm = fixedSizeWorld
			? static_cast<btBroadphaseInterface*>(new bt32BitAxisSweep3(btVector3(-1000, -1000, -1000), btVector3(1000, 1000, 1000)))
			: new btDbvtBroadphase();

		constraintSolver = new btSequentialImpulseConstraintSolver();
		softBodySolver = enableSoftBody ? new btDefaultSoftBodySolver() : nullptr;
		bulletWorld = PhysicsSettings::customWorldCreator(dispatcher, broadPhaseAlgorithm, constraintSolver, collisionConfig, softBodySolver);

		gContactAddedCallback = &ContactAddedCallBack;
		gContactDestroyedCallback = &ContactDestroyedCallback;
		gContactProcessedCallback = &ContactProcessedCallback;
		gContactStartedCallback = &ContactStartedCallback;
		gContactEndedCallback = &ContactEndedCallback;

		bulletWorld->setGravity(btVector3(0, -9.78033f, 0));
		bulletWorld->setLatencyMotionStateInterpolation(false);
	}

	void Physics::UpdateWorld(float deltaTime)
	{
		constexpr btScalar FIXED_STEPS_PER_SECOND = btScalar(30.);
		constexpr btScalar FIXED_TIME_STEP = btScalar(1.) / FIXED_STEPS_PER_SECOND;

		running = true;
		int simulationSteps = bulletWorld->stepSimulation(deltaTime, 7, FIXED_TIME_STEP);

		renderingTime = (renderingTime + deltaTime) - simulationSteps * FIXED_TIME_STEP;
		float delta = renderingTime * FIXED_STEPS_PER_SECOND;

		for (auto& character : characters)
			character->Interpolate(delta);

		running = false;
	}

	void Physics::AddPhysicsObject(btCollisionObject& object, int filterGroup, int filterMask)
	{
		if (!running)
			bulletWorld->addCollisionObject(&object, filterGroup, filterMask);
		else
			addObjects.emplace_back(&object, filterGroup, filterMask);
	}

	void Physics::RemovePhysicsObject(btCollisionObject& object)
	{
		if (!running)
			bulletWorld->removeCollisionObject(&object);
		else
		{
			object.setUserPointer(nullptr);
			deleteObjects.emplace_back(&object);
		}
	}

	void Physics::AddPhysicsRigidBody(btRigidBody& rigidbody, int filterGroup, int filterMask)
	{
		if (!running)
			bulletWorld->addRigidBody(&rigidbody, filterGroup, filterMask);
		else
			addRigidBodies.emplace_back(&rigidbody, filterGroup, filterMask);
	}

	void Physics::RemovePhysicsRigidBody(btRigidBody& rigidbody)
	{
		if (!running)
			bulletWorld->removeRigidBody(&rigidbody);
		else
		{
			rigidbody.setUserPointer(nullptr);
			deleteRigidBodies.emplace_back(&rigidbody);
		}
	}

	void Physics::AddPhysicsCharacter(KinematicBody& character)
	{
		if (!running)
		{
			btRigidBody& rigidBody = character.collisionObject;
			rigidBody.setCollisionFlags(rigidBody.getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
			rigidBody.setActivationState(DISABLE_DEACTIVATION);

			bulletWorld->addRigidBody(&rigidBody, character.GetFilterGroup(), character.GetFilterMask());
			bulletWorld->addAction(&character);

			characters.push_back(&character);
		}
		else
			addCharacters.push_back(&character);
	}

	void Physics::RemovePhysicsCharacter(KinematicBody& character)
	{
		if (!running)
		{
			bulletWorld->removeAction(&character);
			bulletWorld->removeRigidBody(&character.collisionObject);

			auto found = std::find(characters.begin(), characters.end(), &character);
			if (found != characters.end())
				characters.erase(found);
		}
		else
			deleteCharacters.push_back(&character);
	}


	void Physics::RayCast(const glm::vec3& from, const glm::vec3& to, RayCastInfo& rayCastInfo)
	{
		btVector3 f(from.x, from.y, from.z);
		btVector3 t(to.x, to.y, to.z);
		CustomRayResultInfo<btCollisionWorld::ClosestRayResultCallback> rayCallback(f, t);
		rayCallback.m_collisionFilterMask = rayCastInfo.filterMask;
		rayCallback.m_flags = rayCastInfo.flags;

		float distance = rayCastInfo.distanceSquared;

		// Perform raycast
		bulletWorld->rayTest(f, t, rayCallback);

		rayCastInfo.hasHit = rayCallback.hasHit();
		rayCastInfo.hitVoxel = rayCallback.objPosition;
		rayCastInfo.hitPoint = glm::vec3(rayCallback.m_hitPointWorld.x(), rayCallback.m_hitPointWorld.y(), rayCallback.m_hitPointWorld.z());
		rayCastInfo.hitNormal = glm::vec3(rayCallback.m_hitNormalWorld.x(), rayCallback.m_hitNormalWorld.y(), rayCallback.m_hitNormalWorld.z());
		rayCastInfo.distanceSquared = rayCallback.m_closestHitFraction * distance;
		rayCastInfo.hitObject = rayCallback.m_collisionObject ? static_cast<Collider*>(rayCallback.m_collisionObject->getUserPointer()) : nullptr;

	}

	void Physics::DirtyCleanUp()
	{
		while (!addObjects.empty())
		{
			auto& object = addObjects.front();
			AddPhysicsObject(*std::get<0>(object), std::get<1>(object), std::get<2>(object));
			addObjects.pop_front();
		}

		while (!addRigidBodies.empty())
		{
			auto& object = addRigidBodies.front();
			AddPhysicsRigidBody(*std::get<0>(object), std::get<1>(object), std::get<2>(object));
			addRigidBodies.pop_front();
		}

		while (!addCharacters.empty())
		{
			AddPhysicsCharacter(*addCharacters.front());
			addCharacters.pop_front();
		}

		while (!deleteObjects.empty())
		{
			RemovePhysicsObject(*deleteObjects.front());
			deleteObjects.pop_front();
		}

		while (!deleteRigidBodies.empty())
		{
			RemovePhysicsRigidBody(*deleteRigidBodies.front());
			deleteRigidBodies.pop_front();
		}

		while (!deleteCharacters.empty())
		{
			RemovePhysicsCharacter(*deleteCharacters.front());
			deleteCharacters.pop_front();
		}
	}

#pragma region Callbacks / Triggers
	// ContactAddedCallback gContactAddedCallback = 0;
	bool Physics::ContactAddedCallBack(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1)
	{
		Debug::Log("Added CallBack");
		return false;
	}

	// ContactDestroyedCallback	gContactDestroyedCallback = 0;
	bool Physics::ContactDestroyedCallback(void* userPersistentData)
	{
		Debug::Log("Destroyed Callback");
		return false;
	}

	// ContactProcessedCallback gContactProcessedCallback = 0;
	bool Physics::ContactProcessedCallback(btManifoldPoint& cp, void* iBody0, void* iBody1)
	{
		const btCollisionObject* body0 = static_cast<btCollisionObject*>(iBody0);
		const btCollisionObject* body1 = static_cast<btCollisionObject*>(iBody1);

		bool body1IsTrigger = static_cast<btCollisionObject*>(iBody1)->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE;
		if (body1IsTrigger) // swap or not
			std::swap(body0, body1);

		if (body0->getCollisionFlags() & Collision::CFE_LIQUID)
		{
			Collider* collider = static_cast<Collider*>(body1->getUserPointer());
			if (collider)
			{
				switch (collider->GetCollidableType())
				{
				case Collidable::Type::RIGID_BODY:
					static_cast<RigidBody*>(collider)->ApplyForce(Vector3(0.f, 0.05f, 0.f), Vector3(0.f, 0.f, 0.f));
					break;
				}
			}
		}

		return false;
	}

	// ContactStartedCallback gContactStartedCallback = 0;
	void Physics::ContactStartedCallback(btPersistentManifold* const& manifold)
	{
		const btCollisionObject* body0 = manifold->getBody0();
		const btCollisionObject* body1 = manifold->getBody1();

		// make sure we actually need to do something with this collision
		const int enableProcessing = Collision::CFE_ENABLE_CALLBACK | Collision::CFE_LIQUID | Collision::CFE_CHARACTER_STATE;
		if ((body0->getCollisionFlags() | body1->getCollisionFlags()) & enableProcessing)
		{

			// if one these are true then use the second body as main
			const int dominantFlags = Collision::CFE_ENABLE_CALLBACK | Collision::CFE_PROGRAMMABLE_MATERIAL | Collision::CFE_LIQUID | Collision::CFE_CHARACTER_STATE;
			if (body1->getCollisionFlags() & dominantFlags)
				std::swap(body0, body1);

			// trigger like, i.e.: not a solid physics object?
			int body0CollisionFlags = body0->getCollisionFlags();

			if (body0CollisionFlags & Collision::CFE_LIQUID) // apply liquid settings to other body
			{
				if (Collidable* collider = static_cast<Collidable*>(body1->getUserPointer()))
				{
					switch (collider->GetCollidableType())
					{
					case Collidable::Type::RIGID_BODY:
						static_cast<RigidBody*>(collider)->SetDamping(0.95f, 0.95f);
						static_cast<RigidBody*>(collider)->SetGravity(Vector3(0, 4, 0));
						break;
					case Collidable::Type::CHARACTER:
						static_cast<KinematicBody*>(collider)->SetCharacterState(CharacterState::CS_IN_LIQUID);
						break;
					}
				}
			}
			else if (body0CollisionFlags & Collision::CFE_CHARACTER_STATE) // set state to character
			{

			}
			else if (body0CollisionFlags & Collision::CFE_NOT_SOLID) // trigger
			{
				if (Collidable* collider = static_cast<Collidable*>(body0->getUserPointer()))
					collider->TriggerStarted(static_cast<Collidable*>(body1->getUserPointer()));
			}
			else // it's a solid, collision feedback
			{
				if (Collidable* collider = static_cast<Collidable*>(body0->getUserPointer()))
					collider->CollisionStarted(static_cast<Collidable*>(body1->getUserPointer()), *manifold);
			}
		}
	}

	// ContactEndedCallback gContactEndedCallback = 0;
	void Physics::ContactEndedCallback(btPersistentManifold* const& manifold)
	{
		const btCollisionObject* body0 = manifold->getBody0();
		const btCollisionObject* body1 = manifold->getBody1();

		// make sure we actually need to do something with this collision
		const int enableProcessing = Collision::CFE_ENABLE_CALLBACK | Collision::CFE_LIQUID | Collision::CFE_CHARACTER_STATE;
		if ((body0->getCollisionFlags() | body1->getCollisionFlags()) & enableProcessing)
		{

			// if one these are true then swap and use the second body as main
			const int dominantFlags = Collision::CFE_ENABLE_CALLBACK | Collision::CFE_PROGRAMMABLE_MATERIAL | Collision::CFE_LIQUID | Collision::CFE_CHARACTER_STATE;
			if (body1->getCollisionFlags() & dominantFlags)
				std::swap(body0, body1);

			// trigger like, i.e.: non-solid object
			int body0CollisionFlags = body0->getCollisionFlags();

			if (body0CollisionFlags & Collision::CFE_LIQUID) // apply liquid settings to other body
			{
				if (Collidable* collider = static_cast<Collidable*>(body1->getUserPointer()))
				{
					switch (collider->GetCollidableType())
					{
					case Collidable::Type::RIGID_BODY:
						static_cast<RigidBody*>(collider)->SetDamping(0.0f, 0.0f);
						static_cast<RigidBody*>(collider)->SetGravity(Vector3(0.f, -9.8f, 0.f));
						break;
					case Collidable::Type::CHARACTER:
						static_cast<KinematicBody*>(collider)->UnsetCharacterState(CharacterState::CS_IN_LIQUID);
						break;
					}
				}
			}
			else if (body0CollisionFlags & Collision::CFE_CHARACTER_STATE) // set state to character
			{

			}
			else if (body0CollisionFlags & Collision::CFE_NOT_SOLID) // trigger
			{
				if (Collidable* collider = static_cast<Collidable*>(body0->getUserPointer()))
					collider->TriggerEnded(static_cast<Collidable*>(body1->getUserPointer()));
			}
			else // it's a solid, collision feedback
			{
				if (Collidable* collider = static_cast<Collidable*>(body0->getUserPointer()))
					collider->CollisionStarted(static_cast<Collidable*>(body1->getUserPointer()), *manifold);
			}
		}
	}
#pragma endregion

	Physics::~Physics()
	{
		delete dispatcher;
		delete constraintSolver;
		delete softBodySolver;
		delete broadPhaseAlgorithm;

		if (PhysicsSettings::overrideBulletPhysicsConfiguration == nullptr)
			delete collisionConfig;
	}
}