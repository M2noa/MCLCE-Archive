#pragma once

#include <vector>
#include <string>
#include <cppu/hash.h>
#include <glm/mat4x4.hpp>

#include "Rendering/Objects/Mesh.h"
#include "Rendering/Objects/VertexData/ModelVertexDataA.h"
#include "Rendering/Objects/Material.h"
#include "Rendering/Objects/BoneMatrices.h"
#include "Rendering/Objects/AnimationCollection.h"

namespace Esteem
{
	class Model
	{
		friend class ModelFactory;
	public:
		enum ModelGenerateSettings
		{
			NONE = 0x0,
			GENERATE_NORMALS = 0x1,
			SMOOTH_NORMALS = 0x2
		};

		struct BoneData
		{
			uint16 index;
			uint16 parentIndex;

			hash_t hash;

			BoneMatrices::value_type invBindMatrix;

			//std::vector<uint> childBoneIndices;
			uint8_t childCount;
			bool physicsIK;

			struct
			{
				glm::vec3 translation;
				glm::vec3 scale;
				glm::quat rotation;

			} defaults;

			BoneData() = default;

			BoneData(uint index, uint8_t childCount, uint parentIndex, hash_t hash, BoneMatrices::value_type invBindMatrix)
				: index(index)
				, parentIndex(parentIndex)
				, hash(hash)
				, invBindMatrix(invBindMatrix)
				, childCount(childCount)
				, physicsIK(false)
				, defaults()
			{}
		};

	private:
		std::string path;

		ModelGenerateSettings importSettings;
		std::string importMaterialsDirectory;

		cgc::strong_ptr<const AnimationCollection> animationCollection;

	public:
		glm::vec3 center;
		glm::vec3 volume;

		std::vector<cgc::strong_ptr<Mesh<ModelVertexDataA>>> meshes;
		std::vector<cgc::strong_ptr<Material>> materials;
		std::unordered_map<hash_t, uint16> boneMap;
		std::vector<BoneData> boneData;
		std::vector<BoneData>::const_iterator boneUpperEnd;
		std::vector<glm::mat4> boneMatrices;

		Model(const std::string& path, const std::vector<cgc::strong_ptr<Mesh<ModelVertexDataA>>>& meshes, const std::vector<cgc::strong_ptr<Material>>& materials,
			const std::vector<BoneData>& boneData, const cgc::strong_ptr<AnimationCollection>& animationCollection = cgc::strong_ptr<AnimationCollection>());

		Model(const std::string& path, ModelGenerateSettings settings, std::string_view materialsDirectory);
		~Model();

		inline const std::string& GetPath() const { return path; }

		inline void SetBoneAnimationCollection(const cgc::strong_ptr<const AnimationCollection>& collection) { animationCollection = collection; }
		inline const cgc::strong_ptr<const AnimationCollection>& GetBoneAnimationCollection() const { return animationCollection; }

		operator std::string() { return "Model(" + path + ")"; }
	};
}