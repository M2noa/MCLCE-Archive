#pragma once

#include <vector>
#include <string>
#include <cppu/cgc/ref/unordered_map.h>

#include <cppu/cgc/m_array.h>

#include "General/IFactory.h"
#include "Rendering/Objects/AnimationCollection.h"
#include "./Model.h"
#include "./IModelLoader.h"

#include "./AssimpModelLoader.h"

namespace Esteem
{
	class ModelFactory : public IFactory
	{
	private:
		cgc::ref::unordered_map<std::string, Model> models;
		std::unordered_map<std::string, cgc::strong_ptr<AnimationCollection>> animationCollections;
		std::vector<std::vector<Mesh<ModelVertexDataA>>*> meshArray;


		std::vector<IModelLoader*> modelLoaders;

	public:
		ModelFactory();
		~ModelFactory();

		virtual void Initialize();
		void Command(const std::string& command, const std::string& value);

		virtual cgc::strong_ptr<const Model> LoadModel(std::string_view path, std::string_view modelTexturesFolder, Model::ModelGenerateSettings settings);
		virtual cgc::strong_ptr<AnimationCollection> LoadAnimations(std::string_view path) const;

		virtual void Register(const cgc::strong_ptr<AnimationCollection>& model);
		virtual void UnRegister(const cgc::strong_ptr<const AnimationCollection>& model);

		/// \brief used by the Data class to enable factory search, put in own logic in here
		virtual bool EqualsSearch(std::string_view search) override;

		static void EnlargeBounds(Model& model, const glm::vec3& vertex);

	};
}

#include "./ModelFactory.inl"