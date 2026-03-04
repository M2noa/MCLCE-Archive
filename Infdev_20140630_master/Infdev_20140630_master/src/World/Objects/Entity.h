#pragma once

#include "stdafx.h"

#include <typeindex>
#include <unordered_map>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>

#include "World/Objects/Transform.h"

#include <cppu/cgc/pointers.h>


namespace Esteem
{
	class AbstractConstituent;
	class AbstractComponent;

	class Entity : public Transform
	{
		friend class World;
		friend class cgc::constructor;

	private:
		std::unordered_map<size_t, cgc::strong_ptr<AbstractConstituent>> constituents;

		std::vector<cgc::strong_ptr<AbstractComponent>> components;
		//std::map<int, AbstractComponent> components;

		Entity(World* world);

	public:
		virtual ~Entity();

		virtual void Update();

		virtual void LateUpdate();

		virtual void DirtyCleanUp();

		template<class T>
		cgc::strong_ptr<T> GetSystemComponent() const;

		template <typename T>
		void AddSystemComponent(const cgc::strong_ptr<T>& constituent);

		template <typename T>
		void RemoveSystemComponent();

		template<class T>
		cgc::strong_ptr<T> GetComponent() const;

		static void AddComponent(const cgc::strong_ptr<Entity>& entity, const cgc::strong_ptr<AbstractComponent>& component);
		void RemoveComponent(const cgc::strong_ptr<AbstractComponent>& component);

		operator std::string() { return "Entity()"; }
	};
}

#include "./Entity.inl"