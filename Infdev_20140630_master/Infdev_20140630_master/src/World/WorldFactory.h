#pragma once

#include <vector>
#include <string>

#include "General/IFactory.h"
#include "./BSP/BSPFactory.h"
#include "./Objects/WorldDataPart.h"
#include "./Objects/Entity.h"

namespace Esteem
{
	const std::string WORLD_PART_DYNAMICS_EXTENSION = ".wpd";
	const std::string WORLD_PART_STATICS_EXTENSION = ".wps";

	class WorldFactory : public IFactory
	{
	private:
		BSPFactory* bspFactory; // TODO: make bsp factory function correctly

	public:
		static IWorldObject* CreateWorldObject();

		virtual void Initialize();

		static WorldDataPart LoadWorldPart(const std::string& world, const Vector3i& location);
		static void SaveWorldPart(WorldDataPart* worldDataPart, const std::string& world);
		
		/// \brief used by the Data class to enable factory search, put in own logic in here
		virtual bool EqualsSearch(std::string_view search) override;
	};
}
