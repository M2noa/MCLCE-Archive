#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include <cppu/cgc/pointers.h>

#include "PhysicsFactory.h"

class btDiscreteDynamicsWorld;
class btDispatcher;
class btCollisionConfiguration;
class btBroadphaseInterface;
class btConstraintSolver;
class btSoftBodySolver;

// collision callbacks / trigger
class btManifoldPoint;
struct btCollisionObjectWrapper;
class btPersistentManifold;

namespace Esteem
{
	class World;
	class KinematicBody;
	struct RayCastInfo;

	class Physics
	{
	private:
		// interpolation
		float renderingTime;
		std::atomic<bool> running;

		std::deque<std::tuple<btCollisionObject*, int, int>> addObjects;
		std::deque<btCollisionObject*> deleteObjects;

		std::deque<std::tuple<btRigidBody*, int, int>> addRigidBodies;
		std::deque<btRigidBody*> deleteRigidBodies;

		std::deque<KinematicBody*> addCharacters;
		std::deque<KinematicBody*> deleteCharacters;

		std::vector<KinematicBody*> characters;

		/// \brief every physical object go to the world
		btDiscreteDynamicsWorld* bulletWorld;
		/// \brief what collision algorithm to use for the collision between 2 certain objects
		btDispatcher* dispatcher;
		/// \brief configuration 
		btCollisionConfiguration* collisionConfig;
		/// \brief should Bullet examine every object, or just what is close to each other
		btBroadphaseInterface* broadPhaseAlgorithm;
		/// \brief Solve collisions, apply forces, impulses
		btConstraintSolver* constraintSolver;
		/// \brief Solve collisions with soft bodies
		btSoftBodySolver* softBodySolver;

#pragma region Callbacks / Triggers
		// ContactAddedCallback gContactAddedCallback = 0;
		static bool ContactAddedCallBack(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1);

		// ContactDestroyedCallback gContactDestroyedCallback = 0;
		static bool ContactDestroyedCallback(void* userPersistentData);

		// ContactProcessedCallback gContactProcessedCallback = 0;
		static bool ContactProcessedCallback(btManifoldPoint& cp, void* body0, void* body1);

		// ContactStartedCallback gContactStartedCallback = 0;
		static void ContactStartedCallback(btPersistentManifold* const& manifold);

		// ContactEndedCallback gContactEndedCallback = 0;
		static void ContactEndedCallback(btPersistentManifold* const& manifold);
#pragma endregion

	public:
		/// \brief Construct the Bullet Physics Engine for use
		/// \param fixedSizeWorld does this world has a fixed size? will determine the physics broadphase algortihm
		/// \param enableSoftBody should our world enable Soft Bodies? (will enable soft body solvers)
		Physics(bool fixedSizeWorld, bool enableSoftBody);
		~Physics();

		/// \brief Update (advance) the world with the time of given deltaTime
		/// \param deltaTime time is millisecond
		void UpdateWorld(float deltaTime);

		void DirtyCleanUp();

		/// \brief Add a physics object to the world
		/// \param physicsObject to add
		void AddPhysicsObject(btCollisionObject& object, int filterGroup, int filterMask);
		/// \brief Remove a physics object from the world
		/// \param physicsObject to remove
		void RemovePhysicsObject(btCollisionObject& object);

		/// \brief Add a physics rigid body to the world
		/// \param physics rigid body to add
		void AddPhysicsRigidBody(btRigidBody& rigidbody, int filterGroup, int filterMask);
		/// \brief Remove a physics rigid body from the world
		/// \param physicsO rigid body remove
		void RemovePhysicsRigidBody(btRigidBody& rigidbody);

		/// \brief Add a physics character to the world
		/// \param physicsCharacter to add
		void AddPhysicsCharacter(KinematicBody& character);
		/// \brief Remove a physics character from the world
		/// \param physicsCharacter to remove
		void RemovePhysicsCharacter(KinematicBody& character);

		/// \brief ray cast through the physics world
		/// \param from ray cast from
		/// \param to ray cast to
		/// \param rayCastInfo output data of the hit
		void RayCast(const glm::vec3& from, const glm::vec3& to, RayCastInfo& rayCastInfo);
	};
}