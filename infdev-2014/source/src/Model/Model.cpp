#include "./Model.h"
#include "./ModelFactory.h"
#include "Utils/Data.h"

namespace Esteem
{
	Model::Model(const std::string& path, const std::vector<cgc::strong_ptr<Mesh<ModelVertexDataA>>>& meshes, const std::vector<cgc::strong_ptr<Material>>& materials,
		const std::vector<BoneData>& boneData, const cgc::strong_ptr<AnimationCollection>& animationCollection)
		: path(path)
		, meshes(meshes)
		, materials(materials)
		, boneMap()
		, boneData(boneData)
		, animationCollection(animationCollection)
		, center(0, 0, 0)
		, volume(0, 0, 0)
	{
		
	}

	Model::Model(const std::string& path, ModelGenerateSettings settings, std::string_view materialsDirectory)
		: path(path)
		, center(0, 0, 0)
		, volume(0, 0, 0)
		, importSettings(settings)
		, importMaterialsDirectory(materialsDirectory)
	{
		
	}

	Model::~Model()
	{
		
	}
}