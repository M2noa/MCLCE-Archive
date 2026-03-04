#pragma once

#include <deque>
#include <cppu/cgc/pointers.h>

#include "General/Vector3.h"
#include "./IWorldObject.h"

namespace Esteem
{
	class Transform;

	class WorldDataPart
	{
	friend class World;
	friend class WorldController;
	friend class WorldFactory;
	private:
		Vector3i location;

		bool staticMapExists;
		bool dynamicMapExists;

		std::deque<cgc::strong_ptr<Transform>> staticObjects;
		std::deque<cgc::strong_ptr<Transform>> dynamicObjects;

		WorldDataPart(const Vector3i& location)
			: location(location),
			staticMapExists(true),
			dynamicMapExists(true)
		{}

	public:
		WorldDataPart()
			: staticMapExists(true),
			dynamicMapExists(true)
		{}

		~WorldDataPart()
		{
		}

		inline const Vector3i& GetLocation() const { return location; }

		inline const std::deque<cgc::strong_ptr<Transform>>& GetStaticObjects() const { return staticObjects; }
		inline const std::deque<cgc::strong_ptr<Transform>>& GetDynamicObjects() const { return dynamicObjects; }

		static const int PARTS_SIZE = 256;
	};
}