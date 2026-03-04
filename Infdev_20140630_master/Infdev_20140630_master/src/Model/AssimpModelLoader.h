#pragma once

#include "./IModelLoader.h"
#include "./ModelFactory.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Esteem
{
	class AssimpModelLoader : public IModelLoader
	{
	private:
		//Assimp::Importer importer;

		static void ParseMesh(const aiScene& scene, Model& model, std::string_view modelTexturesFolder);

		static void ParseBonesRecursive(Model& model, const aiNode& node, const aiScene& scene, const glm::mat4& parentMatrix);
		static void ParseChildBone(Model& model, const aiNode& node, const aiScene& scene, int parentIndex, const glm::mat4& parentMatrix);

		cgc::strong_ptr<AnimationCollection> ParseAnimationCollection(const aiScene& scene, const Model& model, std::string_view path);
		cgc::strong_ptr<AnimationSequence> ParseAnimationSequence(const aiAnimation& animation, const Model& model);

		static int FindBoneIndex(const Model& model, hash_t name);

	public:
		AssimpModelLoader();
		//~AssimpModelLoader();
		
		virtual void LoadModel(std::string_view path, Model& model, std::string_view modelTexturesFolder, Model::ModelGenerateSettings settings);
		virtual cgc::strong_ptr<AnimationCollection> LoadAnimations(std::string_view path);
		
		/// \brief used by the Data class to enable factory search, put in own logic in here
		virtual bool EqualsSearch(std::string_view search);
	};
}