#include "Entity.h"

#include "World/World.h"

#include "World/Constituents/AbstractComponent.h"
#include "World/Constituents/AbstractConstituent.h"

namespace Esteem
{
	Entity::Entity(World* world)
		: Transform(world)
		, components()
	{
		
	}

	Entity::~Entity()
	{
		components.clear();
		constituents.clear();

		GetWorld()->RemoveWorldObject(this);
	}

	void Entity::Update()
	{
		for (auto& component : components)
			component->Update();
	}

	void Entity::LateUpdate()
	{
		for (auto& component : components)
			component->LateUpdate();
	}

	void Entity::DirtyCleanUp()
	{
		Transform::DirtyCleanUp();

		for (auto& component : components)
			component->DirtyCleanUp();
	}	

	void Entity::AddComponent(const cgc::strong_ptr<Entity>& entity, const cgc::strong_ptr<AbstractComponent>& component)
	{
		if (entity && component)
		{
			auto& components = entity->components;
			if (std::find(components.begin(), components.end(), component) == components.end())
			{
				components.push_back(component);
				component->entity = entity;
				component->Initialize();
			}
		}
	}

	void Entity::RemoveComponent(const cgc::strong_ptr<AbstractComponent>& component)
	{
		(void)std::remove(components.begin(), components.end(), component);
	}
}