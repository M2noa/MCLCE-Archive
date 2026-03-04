#pragma once

#include "./Collider.h"

#include "General/Vector3.h"
#include "Model/Model.h"

#include <BulletCollision/CollisionDispatch/btCollisionObject.h>

namespace Esteem
{
	class StaticBody final : public Collider
	{
	friend cgc::constructor;

	private:
		btCollisionObject collisionObject;
		cgc::strong_ptr<const Model> model;
		
	public:
		StaticBody()
			: Collider(Type::STATIC_BODY)
		{ }

		StaticBody(const cgc::strong_ptr<Entity>& entity)
			: Collider(entity, Type::STATIC_BODY)
		{ }

	protected:
		StaticBody(const cgc::strong_ptr<Entity>& entity, Type type, int filterGroup = Collision::StaticFilter, int filterMask = -3)
			: Collider(entity, Type::STATIC_BODY)
		{ }

	public:
		~StaticBody();

		static cgc::strong_ptr<StaticBody> Instantiate(const cgc::strong_ptr<Entity>& entity);

		virtual void SetEnabled(bool enable);

		virtual void SetType(Collision::ShapeType type);
		void SetSize(const Vector3& size);
		void SetAsTrigger(bool enable);

		void LoadModel(const std::string& path);
		void SetModel(const cgc::strong_ptr<const Model>& model);
		const cgc::strong_ptr<const Model>& GetModel() const;

		void SetFilterGroup(uint32_t filterGroup);
		void SetFilterMask(uint32_t filterMask);
	};
}

#include "./StaticBody.inl"