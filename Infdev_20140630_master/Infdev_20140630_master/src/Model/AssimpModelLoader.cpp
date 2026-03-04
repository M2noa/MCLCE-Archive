#include "AssimpModelLoader.h"

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <rapidjson/istreamwrapper.h>

#include "Utils/Data.h"
#include "Utils/Debug.h"
#include "Utils/StringParser.h"
#include "Math/Math.h"

#include "Rendering/Objects/AnimationCollection.h"
#include "Rendering/Objects/AnimationSequence.h"

#include "AssimpIOHandler.h"


namespace Esteem
{
	AssimpModelLoader::AssimpModelLoader()
	{
		//Command::RegisterListener("reload", DELEGATE(&AssimpModelLoader::Command, this));

	}

	void AssimpModelLoader::LoadModel(std::string_view path, Model& model, std::string_view modelTexturesFolder, Model::ModelGenerateSettings settings)
	{
		aiPostProcessSteps processSteps = (aiPostProcessSteps)0;
		if (settings & Model::ModelGenerateSettings::GENERATE_NORMALS)
		{
			if (settings & Model::ModelGenerateSettings::SMOOTH_NORMALS)
				processSteps = aiProcess_GenSmoothNormals;
			else
				processSteps = aiProcess_GenNormals;
		}

		Assimp::Importer importer;
		importer.SetIOHandler(new AssimpIOHandler());
		const aiScene* scene = importer.ReadFile(path.data(),
			processSteps

			// general
			| aiProcess_SortByPType
			| aiProcess_CalcTangentSpace
			| aiProcess_GenBoundingBoxes
			| aiProcess_FindInvalidData

			// materials / textures
			| aiProcess_FlipUVs
			| aiProcess_RemoveRedundantMaterials

			// mesh / vertices
			| aiProcess_Triangulate
			| aiProcess_JoinIdenticalVertices
			| aiProcess_OptimizeMeshes
			| aiProcess_ImproveCacheLocality
			| aiProcess_LimitBoneWeights

			// nodes / bones
			//| aiProcess_OptimizeGraph				// crashes bone lookups
			//| aiProcess_Debone					// somehow meshes are missing with this
		);

		if (scene == nullptr)
			Debug::LogError("AssimpModelLoader: Could not load ", path, " error code: ", importer.GetErrorString());
		else if (scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			Debug::LogError("AssimpModelLoader: The file wasn't successfully opened or doesn't contain anything useful: ", path);
		else
		{
			std::vector<Model::BoneData>& boneData = model.boneData;

			scene->mRootNode->mName = "Assimp";
			aiNode* root = scene->mRootNode; //->FindNode("RootNode");

			// find model's root bone, wish this could be easier
			for (size_t i = 0; i < scene->mRootNode->mNumChildren; ++i)
			{
				aiNode* node = scene->mRootNode->mChildren[i];
				if (strcmp(node->mName.C_Str(), "Hips") || strcmp(node->mName.C_Str(), "Armature"))
				{
					root = node;
					break;
				}
				else if (node->mNumMeshes == 0)
					root = node;
			}

			// find model's spine bone
			aiNode* spine = nullptr;
			for (size_t i = 0; i < root->mNumChildren; ++i)
			{
				aiNode* node = root->mChildren[i];
				if (node->mName.length >= 5 && std::string_view(node->mName.data, 5) == "Spine")
				{
					spine = node;
					break;
				}
			}

			boneData.clear();

			glm::mat4 rootMatrix = reinterpret_cast<const glm::mat4&>(root->mTransformation);
			Model::BoneData& rootBone = model.boneData.emplace_back(0, root->mNumChildren, -1,
				RT_HASH(std::string_view(root->mName.data, root->mName.length)), glm::inverse(rootMatrix));

			model.boneMap[rootBone.hash] = 0;

			// process from the spine and up
			if (spine)
				ParseChildBone(model, *spine, *scene, 0, rootMatrix);

			size_t boneUpperCount = model.boneData.size();

			// process child bones recursive
			for (uint i = 0; i < root->mNumChildren; ++i)
			{
				// if it has children then there's a big chance it's a bone or used as one,
				// if it has meshes, then there's a chance it's not used as a bone/node
				aiNode* child = root->mChildren[i];
				if ((child->mNumChildren > 0 || child->mNumMeshes == 0) && child != spine)
					ParseChildBone(model, *child, *scene, 0, rootMatrix);
			}

			model.boneUpperEnd = model.boneData.cbegin() + boneUpperCount;
			model.boneMatrices.resize(boneData.size());

			ParseMesh(*scene, model, modelTexturesFolder);
			//LoadBoneMatricesRecursive(model, boneMatrices, &(*bonesData)[0], boneMatrices[0]);

			cgc::strong_ptr<AnimationCollection> collection = ParseAnimationCollection(*scene, model, path);
			if (collection)
			{
				Data::GetModelFactory().Register(collection);
				model.SetBoneAnimationCollection(collection);
			}

			// recalculate center and volume with min and max values (yes this processor stores them in here)
			model.volume = (model.volume - model.center) * 0.5f;
			model.center += model.volume;
		}
	}

	cgc::strong_ptr<AnimationCollection> AssimpModelLoader::LoadAnimations(std::string_view path)
	{
		static_assert(true, "AssimpModelLoader::LoadAnimations(): not yet implemented");
		return cgc::strong_ptr<AnimationCollection>();
	}

	void AssimpModelLoader::ParseMesh(const aiScene& scene, Model& model, std::string_view modelTexturesFolder)
	{
		for (uint i = 0; i < scene.mNumMeshes; ++i)
		{
			const aiMesh& mesh = *scene.mMeshes[i];

			std::vector<ModelVertexDataA> vertices;
			std::vector<uint> indices;

			aiMaterial* mat = scene.mMaterials[mesh.mMaterialIndex];

			// Get default color
			glm::vec4 defaultColor(1.f);
			aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor4D*>(&defaultColor));


			model.center = glm::min(model.center, reinterpret_cast<const glm::vec3&>(mesh.mAABB.mMin));
			model.volume = glm::max(model.volume, reinterpret_cast<const glm::vec3&>(mesh.mAABB.mMax));

			vertices.resize(mesh.mNumVertices);
			for (uint i = 0; i < mesh.mNumVertices; i++)
			{
				ModelVertexDataA& vertexData = vertices[i];

				// position
				memcpy(&vertexData.position, &mesh.mVertices[i], sizeof(float) * 3);

				// normals
				if (mesh.HasNormals())
				{
					memcpy(&vertexData.normal, &mesh.mNormals[i].x, sizeof(float) * 3);
					memcpy(&vertexData.tangent, &mesh.mTangents[i].x, sizeof(float) * 3);
				}
				else
				{
					vertexData.normal = glm::vec3(1.f);
					vertexData.tangent = glm::vec3(1.f, 0.f, 0.f);
				}

				// colors
				if (mesh.mColors[0])
					memcpy(&vertexData.color, &mesh.mColors[0][i].r, sizeof(float) * 3);
				else
					vertexData.color = defaultColor;

				// uv data
				if (mesh.mTextureCoords[0])
					memcpy(&vertexData.uv, &mesh.mTextureCoords[0][i].x, sizeof(float) * 2);
				else
					vertexData.uv = glm::vec2(0.f);
			}

			// get bone weights
			if (mesh.HasBones())
			{
				for (uint b = 0; b < mesh.mNumBones; ++b)
				{
					aiBone* pBone = mesh.mBones[b];
					uint boneIndex = FindBoneIndex(model, RT_HASH(std::string_view(pBone->mName.data, pBone->mName.length)));

					for (uint i = 0; i < pBone->mNumWeights; ++i)
					{
						// add bone and weight to the list
						auto& vertex = vertices[pBone->mWeights[i].mVertexId];
						float weight = pBone->mWeights[i].mWeight;

						for (uint i = 0; i < 4; i++)
						{
							if (vertex.weights[i] == 0.0f)
							{
								vertex.bones[i] = boneIndex;
								vertex.weights[i] = weight;
								break;
							}
						}
					}
				}
			}

			// get indices
			for (uint faceIndex = 0; faceIndex < mesh.mNumFaces; faceIndex++)
			{
				aiFace& face = mesh.mFaces[faceIndex];
				for (uint index = 0; index < face.mNumIndices; index++)
					indices.emplace_back(face.mIndices[index]);
			}

			// get material
			auto& materials = model.materials;

			aiString aiMaterialName = mat->GetName();
			std::string materialPath(modelTexturesFolder);
			materialPath += '/';
			materialPath.append(aiMaterialName.data, aiMaterialName.length);

			if (auto ptr = RenderingFactory::Instance()->GetMaterial(materialPath))
			{
				materials.emplace_back(ptr);
			}
			else if (Data::AssetExists(materialPath + ".mat"))
			{
				materials.emplace_back(RenderingFactory::Instance()->LoadMaterial(materialPath));
			}
			else
			{
				std::stringstream fileContents;
				fileContents << "{\n\t\"name\": \"" << materialPath << "\",\n\t\"shader\": \"diffuse\",\n\t\"alpha\": false,\n\t\"textures\": {";
				char delimeter = ' ';

				auto GetTexture = [&](aiTextureType type, std::string_view name)
				{
					aiString str;
					if (mat->GetTextureCount(type) > 0
						&& mat->GetTexture(type, 0, &str) == aiReturn::aiReturn_SUCCESS)
					{
						std::string_view p(str.data, str.length);
						fileContents << delimeter << "\n\t\t\"" << name << "\": \"" << modelTexturesFolder << '/' << p.substr(p.find_last_of("/\\") + 1) << "\"";
						Debug::Log(p.substr(p.find_last_of("/\\") + 1));
						delimeter = ',';
					}
				};

				GetTexture(aiTextureType::aiTextureType_DIFFUSE, "diffuse");
				GetTexture(aiTextureType::aiTextureType_NORMALS, "normal");
				GetTexture(aiTextureType::aiTextureType_SPECULAR, "specular");

				fileContents << "\t}\n}";

				rapidjson::Document json;
				std::string file = fileContents.str();
				json.Parse(file.c_str());

				if (json.HasParseError())
				{
					Debug::LogError("Couldn't JSON parse: given contents - ", std::to_string((unsigned)json.GetErrorOffset()), "\n",
						file.substr(json.GetErrorOffset(), 20), (json.GetErrorOffset() + 19 < file.size() ? "..." : ""), "\n^");
				}

				materials.emplace_back(RenderingFactory::Instance()->CreateMaterial(materialPath, json, {}));
			}


			model.meshes.emplace_back(cgc::construct_new<Mesh<ModelVertexDataA>>(vertices, indices, true));
		}
	}

#pragma region Animations and Bones
	void AssimpModelLoader::ParseBonesRecursive(Model& model, const aiNode& node, const aiScene& scene, const glm::mat4& parentMatrix)
	{
		uint parentIndex = model.boneData.back().index;

		// process child bones recursive
		for (uint i = 0; i < node.mNumChildren; ++i)
		{
			// if it has children then there's a big chance it's a bone or used as one,
			// if it has meshes, then there's a chance it's not used as a bone/node
			aiNode& child = *node.mChildren[i];
			if (child.mNumChildren > 0 || child.mNumMeshes == 0)
				ParseChildBone(model, child, scene, parentIndex, parentMatrix);
		}
	}

	void AssimpModelLoader::ParseChildBone(Model& model, const aiNode& node, const aiScene& scene, int parentIndex, const glm::mat4& parentMatrix)
	{
		const glm::mat4& mTransformation = reinterpret_cast<const glm::mat4&>(node.mTransformation);
		glm::mat4 matrix = mTransformation * parentMatrix;
		hash_t hash = RT_HASH(std::string_view(node.mName.data, node.mName.length));
		
		uint index = model.boneData.size();
		model.boneMap[hash] = index;
		auto& bone = model.boneData.emplace_back(index, node.mNumChildren, parentIndex, hash, glm::inverse(matrix));
		bone.defaults.translation = matrix[3];
		bone.physicsIK = hash == CT_HASH("LeftUpLeg") || hash == CT_HASH("RightUpLeg");

		glm::vec4 dummy1;
		glm::vec3 dummy2;
		glm::decompose(glm::transpose(mTransformation), bone.defaults.scale, bone.defaults.rotation, bone.defaults.translation, dummy2, dummy1);
		bone.defaults.rotation = glm::conjugate(bone.defaults.rotation);

		ParseBonesRecursive(model, node, scene, matrix);
	}

	int AssimpModelLoader::FindBoneIndex(const Model& model, hash_t name)
	{
		auto found = model.boneMap.find(name);
		return found != model.boneMap.end() ? found->second : -1;
	}

	cgc::strong_ptr<AnimationCollection> AssimpModelLoader::ParseAnimationCollection(const aiScene& scene, const Model& model, std::string_view path)
	{
		if (scene.HasAnimations())
		{
			std::unordered_map<hash_t, cgc::strong_ptr<const AnimationSequence>> animationSequences;
			for (uint i = 0; i < scene.mNumAnimations; ++i)
				animationSequences.emplace(RT_HASH(path.substr(path.rfind('/') + 1)), ParseAnimationSequence(*scene.mAnimations[i], model));

			return cgc::construct_new<AnimationCollection>(path, std::move(animationSequences));
		}

		return nullptr;
	}

	cgc::strong_ptr<AnimationSequence> AssimpModelLoader::ParseAnimationSequence(const aiAnimation& animation, const Model& model)
	{
		float ticksPerSecond = animation.mTicksPerSecond > 0.0 ? float(animation.mTicksPerSecond) : 25.0f;
		float animationTimeScale = 1.f / ticksPerSecond;

		cgc::strong_ptr<AnimationSequence> sequence = cgc::construct_new<AnimationSequence>(std::string(animation.mName.data, animation.mName.length),
			float(animation.mDuration * animationTimeScale), ticksPerSecond);
		std::unordered_map<hash_t, AnimationChannelData>& channelData = const_cast<std::unordered_map<hash_t, AnimationChannelData>&>(sequence->GetChannelData());
		//channelData.resize(animation.mNumChannels);

		for (uint c = 0; c < animation.mNumChannels; c++)
		{
			aiNodeAnim* assimpChannel = animation.mChannels[c];

			std::string_view boneName(assimpChannel->mNodeName.data, assimpChannel->mNodeName.length);
			hash_t hash = RT_HASH(boneName);
			//if (boneName.length() == 0)
			//	continue;

			auto foundBoneIndex = model.boneMap.find(hash); // FindBoneIndex(model, boneName);
			if (foundBoneIndex != model.boneMap.end())
			{
				AnimationChannelData& channel = channelData[hash];
				channel.boneName = boneName;
				channel.boneIndex = foundBoneIndex->second;

				for (uint positionIndex = 0; positionIndex < assimpChannel->mNumPositionKeys; ++positionIndex)
				{
					aiVectorKey* assimpKey = &assimpChannel->mPositionKeys[positionIndex];
					channel.positionKeys.emplace_back((float)assimpKey->mTime * animationTimeScale, reinterpret_cast<glm::vec3&>(assimpKey->mValue));
				}

				for (uint rotationIndex = 0; rotationIndex < assimpChannel->mNumRotationKeys; ++rotationIndex)
				{
					aiQuatKey* assimpKey = &assimpChannel->mRotationKeys[rotationIndex];
					channel.rotationKeys.emplace_back((float)assimpKey->mTime * animationTimeScale, glm::quat(assimpKey->mValue.w, assimpKey->mValue.x, assimpKey->mValue.y, assimpKey->mValue.z));
				}

				/*for (uint scaleIndex = 0; scaleIndex < assimpChannel->mNumScalingKeys; ++scaleIndex)
				{
					aiVectorKey* assimpKey = &assimpChannel->mScalingKeys[scaleIndex];
					channel.scaleKeys.emplace_back((float)assimpKey->mTime * animationTimeScale, reinterpret_cast<glm::vec3&>(assimpKey->mValue));
				}*/
			}
		}

		return sequence;
	}
#pragma endregion

	bool AssimpModelLoader::EqualsSearch(std::string_view search)
	{
		// all supported file formats
		const char list32[] = {
			"fbx\0"
			"dae\0"
			"3ds\0"
			"ase\0"
			"obj\0"
			"ifc\0"
			"xgl\0"
			"zgl\0"
			"ply\0"
			"dxf\0"
			"lwo\0"
			"lws\0"
			"lxo\0"
			"stl\0"
			"x\0\0\0"
			"ac\0\0"
			"ms3d"
			"cob\0"
			"scn\0"
			"bvh\0"
			"csm\0"
			"xml\0"
			"irr\0"
			"mdl\0"
			"md2\0"
			"md3\0"
			"pk3\0"
			"mdc\0"
			"md5\0"
			"smd\0"
			"vta\0"
			"m3\0\0"
			"3d\0\0"
			"b3d\0"
			"q3d\0"
			"q3s\0"
			"nff\0"
			"off\0"
			"raw\0"
			"ter\0"
			"hmp\0"
			"ndo"
		};

		const char list64[] = {
			"blend\0\0\0"
			"irrmesh\0"
			"md5mesh\0"
			"md5anim"
		};

		if ((search.size() & ~0x3) == 0x4)
		{
			constexpr size_t size = std::size(list64) / sizeof(uint64_t);
			const uint64_t* list = reinterpret_cast<const uint64_t*>(list64);

			uint64_t value = 0;
			memcpy(&value, search.data(), search.size());

			const uint64_t* cur = list;
			const uint64_t* end = list + size;
			for (; cur != end; ++cur)
			{
				if (*cur == value)
					return true;
			}
		}
		else if (search.size() <= 4)
		{
			constexpr size_t size = std::size(list32) / sizeof(uint32_t);
			const uint32_t* list = reinterpret_cast<const uint32_t*>(list32);

			uint32_t value = 0;
			memcpy(&value, search.data(), search.size());

			const uint32_t* cur = list;
			const uint32_t* end = list + size;
			for (; cur != end; ++cur)
			{
				if (*cur == value)
					return true;
			}
		}

		return false;
	}
}
