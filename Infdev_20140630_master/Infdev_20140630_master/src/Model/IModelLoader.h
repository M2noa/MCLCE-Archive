#pragma once

#include <vector>
#include <string>
#include "Rendering/Objects/AnimationCollection.h"
#include "./Model.h"

namespace Esteem
{
	class IModelLoader
	{
	public:
		virtual ~IModelLoader() = 0;

		virtual void LoadModel(std::string_view path, Model& model, std::string_view modelTexturesFolder, Model::ModelGenerateSettings settings) = 0;
		virtual cgc::strong_ptr<AnimationCollection> LoadAnimations(std::string_view path) = 0;

		/// \brief used by the Data class to enable factory search, put in own logic in here
		virtual bool EqualsSearch(std::string_view search) = 0;

	};

	inline IModelLoader::~IModelLoader() {}
}