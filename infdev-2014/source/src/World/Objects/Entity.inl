#pragma once

#include "Entity.h"

namespace Esteem
{
	template<class T>
	inline cgc::strong_ptr<T> Entity::GetSystemComponent() const
	{
		auto found = constituents.find(typeid(T).hash_code());
		return found != constituents.end() ? cgc::static_pointer_cast<T>(found->second) : nullptr;
	}

	template <typename T>	
	inline void Entity::AddSystemComponent(const cgc::strong_ptr<T>& constituent)
	{
		if (constituent)
			constituents.try_emplace(typeid(T).hash_code(), constituent);
	}

	template <typename T>
	inline void Entity::RemoveSystemComponent()
	{
		constituents.erase(typeid(T).hash_code());
	}

	template<class T>
	inline cgc::strong_ptr<T> Entity::GetComponent() const
	{
		for (std::size_t i = 0; i < components.size(); ++i)
		{
			cgc::strong_ptr<T> component = cgc::dynamic_pointer_cast<T, AbstractComponent>(components[i]);
			if (component && typeid(T*) == typeid(component.ptr()))
				return component;
		}

		return nullptr;
	}
}