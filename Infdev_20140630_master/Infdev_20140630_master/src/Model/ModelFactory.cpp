#include "./ModelFactory.h"

#include "General/Command.h"
#include "Utils/Data.h"
#include "Utils/Debug.h"

namespace Esteem
{
	ModelFactory::ModelFactory()
	{
		modelLoaders.push_back(new AssimpModelLoader());
	}

	ModelFactory::~ModelFactory()
	{
		/*for (auto it = models.begin(); it != models.end();)
		{
			Model* ptr = it->second;
			it = models.erase(it);
			delete ptr;
		}*/
	}

	void ModelFactory::Initialize()
	{
		Command::RegisterListener("reload", DELEGATE(&ModelFactory::Command, this));		
	}

	void ModelFactory::Command(const std::string& command, const std::string& value)
	{
		if (value == "models")
		{
			Debug::Log("reloading all models");

			for (auto it = models.begin(); it != models.end(); ++it)
			{
				Model& model = *it->second;

				uint dotPosition = model.GetPath().find_last_of('.');
				std::string extension = model.GetPath().substr(dotPosition + 1);

				uint i = 0;
				for (; i < modelLoaders.size(); ++i)
				{
					if (modelLoaders[i]->EqualsSearch(extension))
					{
						modelLoaders[i]->LoadModel(model.GetPath(), model, model.importMaterialsDirectory, model.importSettings);
						break;
					}
				}

				if (i == modelLoaders.size())
					Debug::LogError("ModelFactory: No suitable model loader found for " + model.GetPath());
			}

			Debug::Log("all models are reloaded, triangle physics objects need to be reset...");

			Command::ExecuteCommand("reload", "physicsobjects");
			Command::ExecuteCommand("reload", "renderobjects");
		}
	}

	cgc::strong_ptr<const Model> ModelFactory::LoadModel(std::string_view path, std::string_view modelTexturesFolder, Model::ModelGenerateSettings settings)
	{
		std::string filePath = (RESOURCES_PATH + MODELS_PATH).append(path);

		auto foundModel = models.find(filePath);
		if (foundModel != models.end())
			return foundModel->second;

		std::string_view extension = path.substr(path.rfind('.') + 1);

		cgc::strong_ptr<Model> model = models.emplace(std::string(path), filePath, settings, modelTexturesFolder);
		for (auto* modelLoader : modelLoaders)
		{
			if (modelLoader->EqualsSearch(extension))
			{
				modelLoader->LoadModel(filePath, *model, modelTexturesFolder, settings);
				return model;
			}
		}

		Debug::LogError("ModelFactory: No suitable model loader found for ", path);

		return nullptr;
	}

	cgc::strong_ptr<AnimationCollection> ModelFactory::LoadAnimations(std::string_view path) const
	{
		std::string filePath = (RESOURCES_PATH + MODELS_PATH).append(path);

		auto foundModel = animationCollections.find(filePath);
		if (foundModel != animationCollections.end())
			return foundModel->second;

		return cgc::strong_ptr<AnimationCollection>();
	}

	void ModelFactory::Register(const cgc::strong_ptr<AnimationCollection>& collection)
	{
		if (animationCollections.find(collection->GetPath()) == animationCollections.end())
			animationCollections[collection->GetPath()] = collection;
	}

	void ModelFactory::UnRegister(const cgc::strong_ptr<const AnimationCollection>&collection)
	{
		auto found = animationCollections.find(collection->GetPath());
		if (found != animationCollections.end())
			animationCollections.erase(found);
	}
	
	bool ModelFactory::EqualsSearch(std::string_view search)
	{
		// all supported file formats
		return std::string("models;model").find(search) != std::string::npos;
	}
}