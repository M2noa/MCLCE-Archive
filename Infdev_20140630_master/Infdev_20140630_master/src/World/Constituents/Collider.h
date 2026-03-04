#pragma once

#include "./AbstractConstituent.h"
#include "Physics/Collidable.h"

#include "General/Vector3.h"
#include "Model/Model.h"

#include "Physics/PhysicsTypes.h"

namespace Esteem
{
	class Physics;

	class Collider abstract : public AbstractConstituent, public Collidable
	{
	friend cgc::constructor;
	friend Physics;
	friend Collidable;

	protected:
		Vector3 size;
		float mass;

	protected:
		bool enabled;
		bool isTrigger;
		Collision::ShapeType collisionType;
		uint32_t filterGroup;
		uint32_t filterMask;
		uint32_t collisionFlags;
		
	public:
		Collider(Type type)
			: AbstractConstituent(nullptr)
			, Collidable(type)
			, collisionType(Collision::ShapeType::NONE)
			, filterGroup(Collision::StaticFilter)
			, filterMask(-3)
			, collisionFlags(0)
			, isTrigger(true)
			, enabled(false)
			, size(1.f)
			, mass(1.f)
		{ }

	protected:
		Collider(const cgc::strong_ptr<Entity>& entity, Type type, int filterGroup = Collision::StaticFilter, int filterMask = -3)
			: AbstractConstituent(entity)
			, Collidable(type)
			, collisionType(Collision::ShapeType::NONE)
			, filterGroup(filterGroup)
			, filterMask(filterMask)
			, collisionFlags(0)
			, isTrigger(true)
			, enabled(false)
			, size(1.f)
			, mass(1.f)
		{ }

	public:
		virtual bool GetEnabled() const { return enabled; }
		
		inline bool IsTrigger() const { return isTrigger; }

		uint32_t GetFilterGroup() const { return filterGroup; }
		uint32_t GetFilterMask() const { return filterMask; }

		virtual ~Collider() = default;
	};
}