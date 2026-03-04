#include "KinematicBody.h"

#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include <algorithm>

#include "Utils/Time.h"
#include "Utils/Debug.h"
#include "Physics/Physics.h"
#include "Utils/Data.h"
#include "../Objects/Entity.h"

#include "World/World.h"

#include "Math/Math.h"
#include "Utils/Debug.h"
#include "Utils/Time.h"

#include "Physics/Physics.h"
#include "Physics/PhysicsFactory.h"
#include "Physics/PhysicsTypes.h"
#include "Physics/RayCast.h"

#include <BulletCollision/CollisionShapes/btConvexShape.h>
#include <BulletCollision/CollisionShapes/btMultiSphereShape.h>

#include <BulletCollision/BroadphaseCollision/btOverlappingPairCache.h>
#include <BulletCollision/BroadphaseCollision/btCollisionAlgorithm.h>
#include <BulletCollision/CollisionDispatch/btCollisionWorld.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>



struct KinematicClosestNotMeConvexResultCallback : public btCollisionWorld::ClosestConvexResultCallback
{
public:
	const btCollisionObject* me;
	btScalar minSlopeDot;
	btVector3 normal;
	std::vector<std::pair<float, const btCollisionObject*>> triggers;

	KinematicClosestNotMeConvexResultCallback(const btCollisionObject* exclude, const btVector3& normal, btScalar minSlopeDot)
		: btCollisionWorld::ClosestConvexResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0))
		, me(exclude)
		, normal(normal)
		, minSlopeDot(minSlopeDot)
	{}

	virtual btScalar addSingleResult(btCollisionWorld::LocalConvexResult& convexResult, bool normalInWorldSpace)
	{
		if (convexResult.m_hitCollisionObject == me)
			return btScalar(1.0);

		const btCollisionObject* collisionObject = convexResult.m_hitCollisionObject;
		if (!collisionObject->hasContactResponse())
		{
			triggers.emplace_back(convexResult.m_hitFraction, collisionObject);
			return btScalar(1.0);
		}

		btVector3 hitNormalWorld = normalInWorldSpace
			? convexResult.m_hitNormalLocal
			: convexResult.m_hitCollisionObject->getWorldTransform().getBasis() * convexResult.m_hitNormalLocal;

		btScalar dotUp = normal.dot(hitNormalWorld);
		if (dotUp < minSlopeDot)
			return btScalar(1.0);

		m_closestHitFraction = convexResult.m_hitFraction;
		m_hitCollisionObject = collisionObject;
		m_hitNormalWorld = hitNormalWorld;
		m_hitPointWorld = convexResult.m_hitPointLocal;

		return convexResult.m_hitFraction;
	}

	inline std::vector<std::pair<float, const btCollisionObject*>>& GetTriggers()
	{
		return triggers;
	}
};

namespace Esteem
{
	cgc::strong_ptr<KinematicBody> KinematicBody::Instantiate(const cgc::strong_ptr<Entity>& entity, World& world, const Vector3& position, const Vector3& up)
	{
		return world.GetWorldConstituents().characters.Create(entity, position, up);
	}

	KinematicBody::KinematicBody(const cgc::strong_ptr<Entity>& entity, const Vector3& position, const Vector3& up)
		: Collider(entity, Type::CHARACTER)
		, up(reinterpret_cast<const btVector3&>(up))
		, collisionObject(0.f, nullptr, nullptr)
		, prevPosition(reinterpret_cast<const btVector3&>(position))
		, currentPosition(reinterpret_cast<const btVector3&>(position))
		, moveDirection(btVector3(0.f, 0.f, 0.f))
		, stepHeight(0.6f)
		, maxSlopeCosine(cosf(Math::ToRadians(46.f)))
		, margin(0.001f)
		, verticalMovement(0.f)
		, verticalOffset(0.f)
		, grounded(false)
		, filterGroup(Collision::CharacterFilter)
		, filterMask(Collision::AllCollidable)
		, offset()
		, swimming(false)
		, onLadder(false)
	{
		filterGroup = Collision::CharacterFilter | Collision::KinematicFilter;
		collisionObject.setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
		PhysicsFactory::SetCollisionCallback(collisionObject, this);
	}

#define entity GetEntity()

	KinematicBody::~KinematicBody()
	{
		if(entity)
			entity->GetWorld()->Physics().RemovePhysicsCharacter(*this);
	}

	void KinematicBody::SetEnabled(bool enable)
	{
		if (this->enabled = enable)
		{
			if (!GetEnabled() && entity)
			{
				PhysicsFactory::SetTransform(collisionObject, entity->GetPosition() + reinterpret_cast<const glm::vec3&>(offset), entity->GetDirections());
				entity->GetWorld()->Physics().AddPhysicsCharacter(*this);
			}
		}
		else if (collisionObject.isInWorld())
			entity->GetWorld()->Physics().RemovePhysicsCharacter(*this);
	}

	void KinematicBody::SetType(Collision::ShapeType type)
	{
		this->collisionType = type;

		if (collisionObject.isInWorld())
			entity->GetWorld()->Physics().RemovePhysicsCharacter(*this);

		switch (type)
		{
		case Collision::ShapeType::CUBE:
			PhysicsFactory::CreateCube(collisionObject, size, mass);
			break;
		case Collision::ShapeType::CYLINDER:
			PhysicsFactory::CreateCylinder(collisionObject, size, mass);
			break;
		case Collision::ShapeType::SPHERE:
			PhysicsFactory::CreateSphere(collisionObject, size.x, mass);
			break;
		case Collision::ShapeType::CAPSULE:
			PhysicsFactory::CreateCapsule(collisionObject, size, mass);
			break;
		case Collision::ShapeType::CONE:
			PhysicsFactory::CreateCone(collisionObject, size, mass);
			break;
		case Collision::ShapeType::ELLIPSOID:
			PhysicsFactory::CreateEllipsoid(collisionObject, size, mass);
			break;
		default:
			return;
		}

		if (enabled)
			entity->GetWorld()->Physics().AddPhysicsCharacter(*this);
	}

	///btActionInterface interface
	void KinematicBody::updateAction(btCollisionWorld* collisionWorld, btScalar deltaTime)
	{
		prevPosition = currentPosition;

		btVector3 deltaMove = moveDirection * deltaTime;
		if (!deltaMove.fuzzyZero())
		{
			verticalMovement = up.dot(deltaMove);
			verticalOffset = verticalMovement * deltaTime;

			Move(collisionWorld, deltaMove);

			// if movement is too tiny, ignore it
			if (prevPosition.distance2(currentPosition) > 0.000001f)
			{
				// apply position
				collisionObject.getWorldTransform().setOrigin(currentPosition);
				velocity = (currentPosition - prevPosition) / deltaTime;
			}
			else
				currentPosition = prevPosition;
		}
		else
			velocity = btVector3(0, 0, 0);

	}

	bool KinematicBody::RecoverFromPenetration(btCollisionWorld* collisionWorld)
	{
		// Here we must refresh the overlapping paircache as the penetrating movement itself or the
		// previous recovery iteration might have used setWorldTransform and pushed us into an object
		// that is not in the previous cache contents from the last timestep, as will happen if we
		// are pushed into a new AABB overlap. If unhandled this means the next convex sweep gets stuck.
		//
		// Do this by calling the broadphase's setAabb with the moved AABB, this will update the broadphase
		// paircache and the ghostobject's internal paircache at the same time.    /BW

		btVector3 minAabb, maxAabb;
		collisionObject.getCollisionShape()->getAabb(collisionObject.getWorldTransform(), minAabb, maxAabb);
		collisionWorld->getBroadphase()->setAabb(collisionObject.getBroadphaseHandle(), minAabb, maxAabb, collisionWorld->getDispatcher());

		bool penetration = false;

		/*collisionWorld->getDispatcher()->dispatchAllCollisionPairs(collisionObject->getOverlappingPairCache(), collisionWorld->getDispatchInfo(), collisionWorld->getDispatcher());

		//currentPosition = ghostObject.getWorldTransform().getOrigin();

		btScalar maxPen = btScalar(0.0);
		for (int pairIndex = 0; pairIndex < ghostObject.getOverlappingPairCache()->getNumOverlappingPairs(); ++pairIndex)
		{
			manifoldArray.resize(0);

			btBroadphasePair& collisionPair = ghostObject.getOverlappingPairCache()->getOverlappingPairArray()[pairIndex];

			btCollisionObject* obj0 = static_cast<btCollisionObject*>(collisionPair.m_pProxy0->m_clientObject);
			btCollisionObject* obj1 = static_cast<btCollisionObject*>(collisionPair.m_pProxy1->m_clientObject);

			if ((obj0->getCollisionFlags() & obj1->getCollisionFlags()) & btCollisionObject::CF_NO_CONTACT_RESPONSE)
				continue;

			if (collisionPair.m_algorithm)
				collisionPair.m_algorithm->getAllContactManifolds(manifoldArray);

			for (int manifoldIndex = 0; manifoldIndex < manifoldArray.size(); ++manifoldIndex)
			{
				btPersistentManifold* manifold = manifoldArray[manifoldIndex];
				btScalar directionSign = manifold->getBody0() == &ghostObject ? btScalar(-1.0) : btScalar(1.0);
				for (int p = 0; p < manifold->getNumContacts(); p++)
				{
					const btManifoldPoint& pt = manifold->getContactPoint(p);

					btScalar dist = pt.getDistance();

					if (dist < 0.0)
					{
						if (dist < maxPen)
						{
							maxPen = dist;
							//touchingNormal = pt.normalWorldOnB * directionSign;//??

						}

						currentPosition += pt.m_normalWorldOnB * directionSign * dist * btScalar(0.2);
						penetration = true;
					}
				}

				//manifold->clearManifold();
			}
		}*/

		collisionObject.getWorldTransform().setOrigin(currentPosition);

		return penetration;
	}

	void KinematicBody::Move(btCollisionWorld* collisionWorld, const btVector3& moveDirection)
	{
		grounded = false;
		std::unordered_set<const btCollisionObject*> thisCycleTriggers;
		bool testTriggered = false;


		// TODO: apply gravity over time (after hitting the ground) take in account step down
		// TODO: reintegrate move up staircases (step up)
		// TODO: reintegrate move down staircases (step down) this somehow bugs the character when rigidbodies are in the world (symptom)
		// TODO: fix jittering with multiple walls

		// Set target direction and position
		btVector3 inverseMoveDirection(-moveDirection);
		btVector3 origMoveDirection = moveDirection;
		btVector3 curMoveDirection = moveDirection;

		//bool forwardInvalid = forward.fuzzyZero();

		// Transforms to set positions with to test against
		btTransform from(collisionObject.getWorldTransform());
		btTransform to(from);

		// Setup callback object to store hit data
		CustomRayResultInfo<KinematicClosestNotMeConvexResultCallback> callback(&collisionObject, -curMoveDirection, btScalar(0.0));
		//callback.m_collisionFilterGroup = ghostObject->getBroadphaseHandle()->m_collisionFilterGroup;
		//callback.m_collisionFilterMask = ghostObject->getBroadphaseHandle()->m_collisionFilterMask;

		callback.m_collisionFilterGroup = filterGroup; // btBroadphaseProxy::CollisionFilterGroups::CharacterFilter;
		callback.m_collisionFilterMask = filterMask; // btBroadphaseProxy::CollisionFilterGroups::AllFilter;

		for (int it = 0; it < 10 && !curMoveDirection.fuzzyZero(); ++it)
		{
			bool hittingGround = false;

			// reset callback
			callback.normal = -curMoveDirection;
			callback.m_closestHitFraction = btScalar(1.);
			from.setOrigin(currentPosition);
			to.setOrigin(currentPosition + curMoveDirection);

			// Physics test
			collisionWorld->convexSweepTest(static_cast<btConvexShape*>(collisionObject.getCollisionShape()), from, to, callback);

			// if we hit something process it!
			if (callback.hasHit())
			{
				const btVector3& hitNormal = callback.m_hitNormalWorld;
				const btScalar& hitFraction = callback.m_closestHitFraction;

				// set the character's position at the hit point
				currentPosition += curMoveDirection * hitFraction + hitNormal * margin;

				//origMoveDirection *= btScalar(1.) - hitFraction;
				curMoveDirection *= btScalar(1.) - hitFraction;
				//curMoveDirection -= curMoveDirection.dot(hitNormal) * hitNormal;


				btScalar upDot = up.dot(hitNormal);
				if (upDot > maxSlopeCosine)
				{
					hittingGround = true;
					this->hitNormal = hitNormal;

					btVector3 right = up.cross(curMoveDirection);
					if (!right.fuzzyZero())
					{
						btVector3 forward = right.cross(hitNormal).normalized();
						curMoveDirection = forward * forward.dot(curMoveDirection);
					}
					else
						curMoveDirection = btVector3(0, 0, 0);
				}
				else
				{
					hittingGround = false;

					std::cout << "sliding " << '\n';
					curMoveDirection -= curMoveDirection.dot(hitNormal) * hitNormal;
				}
			}
			else // we moved the whole way
			{
				hittingGround = false; // this step doesn't guarantee that we are on the ground so play it safe
				currentPosition = to.getOrigin(); // we are at the end point
				curMoveDirection = btVector3(0, 0, 0);
			}

			// only check when we're not on the ground and we have velocity downwards
			if (grounded && !hittingGround && verticalMovement < 0)
				hittingGround = StepDown(collisionWorld);

			// last check we were on the ground (it may change in the next iteration)
			grounded |= hittingGround;

			// handle triggers
			for (auto& [ distance, object ] : callback.GetTriggers())
			{
				if (distance <= callback.m_closestHitFraction)
				{
					thisCycleTriggers.emplace(object);
					triggers.erase(object);
				}
			}
		}

		for (auto& trigger : triggers)
		{
			btPersistentManifold manifold(&collisionObject, trigger, 0, 0, 0);
			gContactEndedCallback(&manifold);
		}

		triggers.swap(thisCycleTriggers);

		for (auto& trigger : triggers)
		{
			btPersistentManifold manifold(&collisionObject, trigger, 0, 0, 0);
			gContactStartedCallback(&manifold);
		}
	}

	void KinematicBody::StepUp(btCollisionWorld* world, const btVector3& forward)
	{
		// Transforms to set positions with to test against
		btTransform from;
		from.setIdentity();
		from.setOrigin(currentPosition + forward + up * stepHeight);

		btTransform to;
		to.setIdentity();
		to.setOrigin(currentPosition + forward);

		// Setup callback object to store hit data
		CustomRayResultInfo<KinematicClosestNotMeConvexResultCallback> callback(&collisionObject, up, maxSlopeCosine);
		callback.m_collisionFilterGroup = btBroadphaseProxy::CollisionFilterGroups::CharacterFilter;
		callback.m_collisionFilterMask = btBroadphaseProxy::CollisionFilterGroups::AllFilter;

		// Physics test
		world->convexSweepTest(static_cast<btConvexShape*>(collisionObject.getCollisionShape()), from, to, callback);
		if (callback.hasHit())
		{
			callback.normal = -forward;
			callback.minSlopeDot = btScalar(-0.0001); // make sure we also hit parallel surfaces

			to.getOrigin().setInterpolate3(to.getOrigin(), to.getOrigin(), callback.m_closestHitFraction);
			from.setOrigin(to.getOrigin() - forward);

			// Physics test
			world->convexSweepTest(static_cast<btConvexShape*>(collisionObject.getCollisionShape()), from, to, callback);
			if (callback.hasHit())
				currentPosition = from.getOrigin() - forward * callback.m_closestHitFraction;
		}
	}

	bool KinematicBody::StepDown(btCollisionWorld* collisionWorld)
	{
		// Transforms to set positions with to test against
		btTransform top;
		top.setIdentity();
		top.setOrigin(currentPosition);

		btVector3 delta = up * (stepHeight + margin);
		btTransform bottom;
		bottom.setIdentity();
		bottom.setOrigin(currentPosition - delta);

		// Setup callback objects to store hit data
		CustomRayResultInfo<KinematicClosestNotMeConvexResultCallback> callback(&collisionObject, up, maxSlopeCosine);
		callback.m_collisionFilterGroup = btBroadphaseProxy::CollisionFilterGroups::CharacterFilter;
		callback.m_collisionFilterMask = btBroadphaseProxy::CollisionFilterGroups::AllFilter;

		// Physics test
		collisionWorld->convexSweepTest(static_cast<btConvexShape*>(collisionObject.getCollisionShape()), top, bottom, callback);
		if (callback.hasHit())
		{
			currentPosition -= delta * callback.m_closestHitFraction + callback.m_hitNormalWorld * margin;
			this->hitNormal = callback.m_hitNormalWorld;

			return true;
		}

		return false;
	}
}

#undef entity