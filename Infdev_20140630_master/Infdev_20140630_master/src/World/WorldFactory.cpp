#include "WorldFactory.h"

#include <iostream>
#include <typeinfo>

#include "Utils/Debug.h"
#include "Utils/Data.h"
#include "rapidxml/rapidxml.hpp"

namespace Esteem
{
	void WorldFactory::Initialize()
	{

	}

	IWorldObject* WorldFactory::CreateWorldObject()
	{
		return 0;
	}

	WorldDataPart WorldFactory::LoadWorldPart(const std::string& world, const Vector3i& location)
	{
		std::string fileContents = Data::ReadAsset(RESOURCES_PATH + WORLD_PARTS_PATH + world + "/" + std::to_string(location.x) + "x" + std::to_string(location.y) + "x" + std::to_string(location.z) + WORLD_PART_STATICS_EXTENSION);
		if (fileContents.length() > 0)
		{
			rapidxml::xml_document<> doc;
			doc.parse<0>(&fileContents[0]);

			Debug::Log("Name of my first node is: " + std::string(doc.first_node()->name()));

			/*rapidxml::xml_node<>* node = doc.first_node()->first_node("Entity");
			cereal::XMLInputArchive iarchive(node->;


			Debug::Log("Node Entity has value " + std::string(node->value()));
			for (rapidxml::xml_attribute<>* attr = node->first_attribute(); attr; attr = attr->next_attribute())
			{
				Debug::Log(std::string(attr->name()) + " = " + std::string(attr->value()));
			}*/
		}

		return WorldDataPart(location); // std::vector<IWorldObject*>();
	}

	void WorldFactory::SaveWorldPart(WorldDataPart* worldDataPart, const std::string& world)
	{
		std::string location = std::to_string(worldDataPart->location.x) + "x" + std::to_string(worldDataPart->location.y) + "x" + std::to_string(worldDataPart->location.z);

		/*{
			const std::deque<IWorldObject*>& worldObjects = worldDataPart->GetStaticObjects();
			if (!worldObjects.empty())
			{
				std::ofstream ostream(RESOURCES_PATH + WORLD_PARTS_PATH + world + "/" + location + WORLD_PART_STATICS_EXTENSION);
				cereal::XMLOutputArchive oarchive(ostream);

				for (auto it = worldObjects.begin(); it != worldObjects.end(); ++it)
				{
					IWorldObject* worldObject = *it;
					if (strcmp(typeid(*worldObject).raw_name(), ".?AVEntity@Esteem@@") == 0)
						oarchive(cereal::make_nvp("Entity", *reinterpret_cast<Entity*>(worldObject)));
				}
			}
		}
		{
			const std::deque<IWorldObject*>& worldObjects = worldDataPart->GetDynamicObjects();
			if (!worldObjects.empty())
			{
				std::ofstream ostream(RESOURCES_PATH + WORLD_PARTS_PATH + world + "/" + location + WORLD_PART_DYNAMICS_EXTENSION);
				cereal::XMLOutputArchive oarchive(ostream);

				for (auto it = worldObjects.begin(); it != worldObjects.end(); ++it)
				{
					IWorldObject* worldObject = *it;
					if (strcmp(typeid(*worldObject).raw_name(), ".?AVEntity@Esteem@@") == 0)
						oarchive(cereal::make_nvp("Entity", *reinterpret_cast<Entity*>(worldObject)));
				}
			}
		}*/
	}

	bool WorldFactory::EqualsSearch(std::string_view search)
	{
		// all supported file formats
		return std::string("models;model").find(search) != std::string::npos;
	}
}