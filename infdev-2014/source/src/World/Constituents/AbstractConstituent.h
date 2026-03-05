#pragma once

#include "stdafx.h"

#include <memory>
#include <cppu/cgc/pointers.h>

namespace Esteem
{
	class WorldController;
	class Entity;
	class World;

	class AbstractConstituent
	{
	friend class Entity;

	private:
		cgc::weak_ptr<Entity> entity;

	public:
		AbstractConstituent(const cgc::strong_ptr<Entity>& entity)
			: entity(entity)
		{ }
		
		inline const cgc::weak_ptr<Entity>& GetEntity() const
		{
			return entity;
		}
	};
}
