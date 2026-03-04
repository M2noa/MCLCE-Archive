#pragma once

#include <cppu/cgc/m_array.h>

#include "Window/View.h"

#include "World/Constituents/Camera.h"
#include "World/Constituents/Light.h"
#include "World/Constituents/KinematicBody.h"
#include "World/Constituents/RigidBody.h"
#include "World/Constituents/StaticBody.h"
#include "World/Constituents/Animator.h"
#include "World/Constituents/MeshRenderer.h"

namespace Esteem
{
	template<class T>
	class WorldDataConstituentGroup
	{
	private:
		// Constituent
		cgc::m_array<T> constituents;

	public:
		void Update();

		void DirtyCleanUp();

		template<class... Args>
		inline cgc::strong_ptr<T> Create(Args&&... args);

		cgc::m_array<T>& GetArray();
		const cgc::m_array<T>& GetArray() const;
	};


	struct WorldDataConstituents
	{
	public:
		WorldDataConstituentGroup<Camera> cameras;
		WorldDataConstituentGroup<Light> lights;
		WorldDataConstituentGroup<StaticBody> staticBodies;
		WorldDataConstituentGroup<RigidBody> rigidBodies;
		WorldDataConstituentGroup<KinematicBody> characters;
		WorldDataConstituentGroup<Animator> animators;
		WorldDataConstituentGroup<MeshRenderer> meshRenderers;
	};

	template<class T>
	inline void WorldDataConstituentGroup<T>::Update()
	{
		std::unique_lock<std::mutex> lock(constituents.get_lock());
		for (auto& vector : constituents.get_arrays())
		{
			for (auto& elem : *vector)
				elem.Update();
		}
	}

	template<class T>
	inline void WorldDataConstituentGroup<T>::DirtyCleanUp()
	{
		std::unique_lock<std::mutex> lock(constituents.get_lock());

		for (auto& vector : constituents.get_arrays())
		{
			for (auto& elem : *vector)
				elem.DirtyCleanUp();
		}
	}

	template<class T>
	template<class... Args>
	inline cgc::strong_ptr<T> WorldDataConstituentGroup<T>::Create(Args&&... args)
	{
		return constituents.emplace(std::forward<Args>(args)...);
	}

	template<class T>
	cgc::m_array<T>& WorldDataConstituentGroup<T>::GetArray()
	{
		return constituents;
	}

	template<class T>
	const cgc::m_array<T>& WorldDataConstituentGroup<T>::GetArray() const
	{
		return constituents;
	}
}