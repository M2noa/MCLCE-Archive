#include "NativeModelLoader.h"

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Utils/Data.h"
#include "Utils/Debug.h"
#include "Utils/StringParser.h"
#include "Math/Math.h"

#include "Rendering/Objects/AnimationCollection.h"
#include "Rendering/Objects/AnimationSequence.h"


namespace Esteem
{
	NativeModelLoader::NativeModelLoader()
	{
		//Command::RegisterListener("reload", DELEGATE(&AssimpModelLoader::Command, this));		
	}

	void NativeModelLoader::LoadModel(const std::string& path, Model& model, const std::string& modelTexturesFolder, Model::ModelGenerateSettings settings)
	{
		if (Data::AssetExists(path))
		{
			std::size_t size;
			cgc::strong_ptr<std::istream> data = Esteem::Data::StreamAsset(path, size);
			if (data)
			{
				::details::zipfilebuffer zipFileBuffer(data, size);				
				
				// header data
				uint8 version[4] = { 0, 0, 0, 0 };

				if (data->gcount() != size)
				{
					// get header info
					char name[6];
					zipFileBuffer.sgetn(name, 6);
					if (strncmp(name, "Esteem", 6) == 0)
					{
						zipFileBuffer.sgetn(reinterpret_cast<char*>(version), 4);
						if (version[0] == 0 && version[1] == 1 && version[3] == 0)
							LoadModel_0_1_0_0(zipFileBuffer, model, modelTexturesFolder, settings);
						else
							Debug::LogError("Cant load ", path, ", unsupported version");
					}
					else
						Debug::LogError("Cant load ", path, ", unsupported model file");
				}
				else
					Debug::LogError("Cant load ", path, ", Invalid model file");
			}

		}
		else
			Debug::LogError("Could not load .mdl ", path);
	}

	bool NativeModelLoader::LoadModel_0_1_0_0(std::streambuf& streamBuffer, Model& model, const std::string& modelTexturesFolder, Model::ModelGenerateSettings settings)
	{
		// meta data
		streamBuffer.sgetn(reinterpret_cast<char*>(&model.volume), sizeof(glm::vec3));
		streamBuffer.sgetn(reinterpret_cast<char*>(&model.center), sizeof(glm::vec3));

		uint16 meshCount;
		streamBuffer.sgetn(reinterpret_cast<char*>(&meshCount), sizeof(uint16));

		for (uint16 m = 0; m < meshCount; ++m)
		{
			// vertices
			uint16 vertexType;
			uint16 vertexCount;
			std::vector<ModelVertexDataA> vertices;

			streamBuffer.sgetn(reinterpret_cast<char*>(&vertexType), sizeof(uint16));
			streamBuffer.sgetn(reinterpret_cast<char*>(&vertexCount), sizeof(uint16));

			assert(vertexType == 1);
			vertices.resize(vertexCount);
			streamBuffer.sgetn(reinterpret_cast<char*>(vertices.data()), sizeof(ModelVertexDataA) * vertexCount);

			// indices
			uint16 indexType;
			uint16 indexCount;
			std::vector<uint> indices;

			streamBuffer.sgetn(reinterpret_cast<char*>(&indexType), sizeof(uint16));
			streamBuffer.sgetn(reinterpret_cast<char*>(&indexCount), sizeof(uint16));

			assert(indexType == sizeof(uint));
			indices.resize(indexCount);
			streamBuffer.sgetn(reinterpret_cast<char*>(indices.data()), sizeof(uint) * indexCount);

			// Add mesh
			cgc::strong_ptr<Mesh<ModelVertexDataA, uint>> mesh = cgc::construct_new<Mesh<ModelVertexDataA, uint>>(vertices, indices, false);
			model.meshes.push_back(mesh);
		}

		// bones
		uint16 boneType;
		uint16 boneCount;
		std::vector<Model::BoneData>& bones = model.boneData;

		streamBuffer.sgetn(reinterpret_cast<char*>(&boneType), sizeof(uint16));
		streamBuffer.sgetn(reinterpret_cast<char*>(&boneCount), sizeof(uint16));

		assert(boneType == 1);
		bones.resize(boneCount);
		streamBuffer.sgetn(reinterpret_cast<char*>(bones.data()), sizeof(Model::BoneData) * boneCount);

		// materials
		uint16 materialCount;
		std::vector<cgc::strong_ptr<Material>>& materials = model.materials;

		streamBuffer.sgetn(reinterpret_cast<char*>(&materialCount), sizeof(uint16));

		materials.resize(materialCount);
		for (uint16 i = 0; i < materialCount; ++i)
		{
			uint16 materialSize;
			streamBuffer.sgetn(reinterpret_cast<char*>(&materialSize), sizeof(uint16));

			assert(false, "look at this");

			/*cgc::strong_ptr<Material> material = materials[i];
			material.resize(modelTexturesFolder.size() + materialSize + 1);

			memcpy(material.data(), modelTexturesFolder.data(), modelTexturesFolder.size());
			material[modelTexturesFolder.size()] = '/';
			streamBuffer.sgetn(reinterpret_cast<char*>(material.data() + modelTexturesFolder.size()), materialSize);*/
		}

		return true;
	}

	bool NativeModelLoader::SaveModel(std::ostream& streamBuffer, Model& model, const std::string& modelTexturesFolder)
	{
		// meta data
		streamBuffer.write(reinterpret_cast<const char*>(&model.volume), sizeof(glm::vec3));
		streamBuffer.write(reinterpret_cast<const char*>(&model.center), sizeof(glm::vec3));

		auto& meshes = model.meshes;
		for (uint16 m = 0; m < meshes.size(); ++m)
		{
			auto& mesh = *meshes[m].ptr();

			// vertices
			mm::array_view vertexInfo = mesh.GetMeshData()->GetVertexMemInfo();
			uint16 vertexSize = vertexInfo.type_size();
			uint16 vertexCount = vertexInfo.size();

			streamBuffer.write(reinterpret_cast<const char*>(&vertexSize), sizeof(glm::uint16));
			streamBuffer.write(reinterpret_cast<const char*>(&vertexCount), sizeof(glm::uint16));
			streamBuffer.write(reinterpret_cast<const char*>(vertexInfo.data()), std::streamsize(vertexCount) * vertexSize);

			// indices
			mm::array_view indexInfo = mesh.GetMeshData()->GetIndexMemInfo();
			uint16 indexSize = indexInfo.type_size();
			uint16 indexCount = indexInfo.size();

			streamBuffer.write(reinterpret_cast<const char*>(&indexSize), sizeof(uint16));
			streamBuffer.write(reinterpret_cast<const char*>(&indexCount), sizeof(uint16));
			streamBuffer.write(reinterpret_cast<const char*>(indexInfo.data()), std::streamsize(indexCount) * indexSize);
		}

		// bones
		std::vector<Model::BoneData>& bones = model.boneData;
		uint16 boneType = 1;
		uint16 boneCount = bones.size();

		streamBuffer.write(reinterpret_cast<const char*>(&boneType), sizeof(uint16));
		streamBuffer.write(reinterpret_cast<const char*>(&boneCount), sizeof(uint16));
		streamBuffer.write(reinterpret_cast<const char*>(bones.data()), std::streamsize(sizeof(Model::BoneData)) * boneCount);

		assert(false, "look at this");

		// materials
		/*std::vector<std::string>& materials = model.materials;
		uint16 materialCount = materials.size();

		streamBuffer.write(reinterpret_cast<char*>(&materialCount), sizeof(uint16));

		std::string modelTexturesFolderSlash = modelTexturesFolder + '/';
		for (uint16 i = 0; i < materialCount; ++i)
		{
			std::string material = materials[i];
			size_t pos = material.find(modelTexturesFolderSlash);
			if (pos != std::string::npos)
				material.erase(pos, modelTexturesFolderSlash.size());

			uint16 materialSize = material.size();
			streamBuffer.write(reinterpret_cast<char*>(&materialSize), sizeof(uint16));
			streamBuffer.write(reinterpret_cast<char*>(material.data()), materialSize);
		}*/

		return true;
	}

	cgc::strong_ptr<AnimationCollection> NativeModelLoader::LoadAnimations(const std::string& path)
	{
		return cgc::strong_ptr<AnimationCollection>();
	}

	bool NativeModelLoader::EqualsSearch(std::string_view search)
	{
		// all supported file formats
		return std::string("mdl").find(search) != std::string::npos;
	}
}
