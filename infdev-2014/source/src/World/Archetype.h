#pragma once

#include <array>

namespace Esteem
{
	template <typename... Components>
	class Archetype
	{
	public:
		typedef Components Components;
	};

	template<size_t N, typename Component>
	struct ComponentArray : std::array<Component, N> { };

	template<size_t N, typename Component, typename... Components>
	struct ComponentArray : ComponentArray<N, Component>, ComponentArray<N, Components...> { };

	template <size_t N, typename... Components>
	class ArchetypeBucket
	{
	public:
		typedef Archetype<Components...> Archetype;

	private:
		ComponentArray<N, Components...> components;

	public:
		template<typename T>
		ComponentArray<N, T>& GetComponents()
		{
			return static_cast<ComponentArray<N, T>>(components);
		}
	};
}