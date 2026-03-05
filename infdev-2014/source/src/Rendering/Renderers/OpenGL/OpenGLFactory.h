#pragma once

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include <map>
#include <unordered_map>
#include <deque>
#include <list>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <mpmc_queue.h>
#include <cppu/stor/lock/deque.h>
#include <cppu/cgc/ref/unordered_map.h>

#include <rapidjson/document.h>

#include "./OpenGLRecipes.h"
#include "../../RenderingFactory.h"

#include "General/Command.h"
#include "Memory/array_view.h"

#include <SFML/Window/Context.hpp>

#include "../../Objects/Buffer.h"
#include "../../Objects/Texture1D.h"
#include "../../Objects/Texture2D.h"
#include "../../Objects/Texture3D.h"
#include "../../Objects/TextureCube.h"

#include "./Objects/OpenGLBoneMatrices.h"
#include "./Objects/OpenGLRenderObject.h"
#include "./Objects/OpenGLBoneMatrices.h"
#include "./Objects/OpenGLShader.h"
#include "./Objects/OpenGLSubShader.h"
#include "./Objects/OpenGLMaterial.h"
#include "./Objects/UBO.h"
#include "./Objects/VAO.h"
#include "./Objects/FBO.h"


namespace Esteem
{
	class LightObject;
	class World;

	namespace OpenGL
	{
		const std::string VERTEX_SHADER_EXTENSION = ".glvs";
		const std::string FRAGMENT_SHADER_EXTENSION = ".glfs";
		const std::string COMPUTE_SHADER_EXTENSION = ".glcs";
		const std::string GEOMETRY_SHADER_EXTENSION = ".glgs";
		const std::string TESSELATION_CONTROL_SHADER_EXTENSION = ".gltcs";
		const std::string TESSELATION_EVALUATION_SHADER_EXTENSION = ".gltes";
		const std::string INCLUDE_SHADER_EXTENSION = ".glinc";

		class OpenGLFactory : public RenderingFactory
		{
		private:
			std::thread::id threadID;
			std::size_t allocatedMemory;

			// OpenGL specific storage
			std::vector<LightData> lightsData;
			std::vector<glm::mat4> lightMatrices;

			uint defaultTextureId;
			cgc::m_array<Buffer> buffers;
			cgc::m_array<Texture1D> texture1Ds;
			std::unordered_map<hash_t, cgc::strong_ptr<Texture1D>> originalTexture1Ds;
			cgc::m_array<Texture2D, cgc::SIZE_32, cgc::CLEAN_PROC::MANUAL> texture2Ds;
			std::unordered_map<hash_t, cgc::strong_ptr<Texture2D>> originalTexture2Ds;
			cgc::ref::unordered_map<hash_t, Texture3D, cgc::CLEAN_PROC::MANUAL> texture3Ds;
			cgc::ref::unordered_map<hash_t, TextureCube, cgc::CLEAN_PROC::MANUAL> texturesCubes;

			std::unordered_map<hash_t, uint> uboIndices;

			cgc::ref::unordered_map<uint, VAO, cgc::CLEAN_PROC::MANUAL> vaos;
			cgc::ref::unordered_map<uint, VBO, cgc::CLEAN_PROC::MANUAL> vbos;
			cgc::ref::unordered_map<uint, EBO, cgc::CLEAN_PROC::MANUAL> ebos;
			cgc::m_array<UBO, cgc::SIZE_32, cgc::CLEAN_PROC::MANUAL> ubos;
			cgc::ref::unordered_map<hash_t, FBO, cgc::CLEAN_PROC::MANUAL> fbos;
			cgc::ref::unordered_map<uint, RBO, cgc::CLEAN_PROC::MANUAL> rbos;

			std::list<std::vector<OpenGLBoneMatrices::value_type>> boneMatricesList;
			cgc::m_array<OpenGLBoneMatrices, cgc::SIZE_32, cgc::CLEAN_PROC::MANUAL> boneMatrixObjects;
			cgc::m_array<OpenGLRenderObject, cgc::SIZE_32, cgc::CLEAN_PROC::MANUAL> renderObjects;
			
			cgc::ref::unordered_map<hash_t, OpenGLShader, cgc::CLEAN_PROC::MANUAL> shaders;
			cgc::ref::unordered_map<hash_t, OpenGLSubShader, cgc::CLEAN_PROC::MANUAL> subShaders;
			cgc::ref::unordered_map<hash_t, OpenGLMaterial, cgc::CLEAN_PROC::MANUAL> materials;

			stor::mpmc_queue<uint> swapBuffers;
			stor::mpmc_queue<std::pair<cgc::strong_ptr<IVBO>, uint>> updateVBOId;
			stor::mpmc_queue<std::pair<cgc::strong_ptr<IEBO>, uint>> updateEBOId;

			stor::mpmc_queue<Recipe::RenderObject*> renderObjectsRecipesQueue;
			stor::mpmc_queue<Recipe::MeshUpdate*> meshUpdateQueue;
			stor::mpmc_queue<Recipe::BufferUpdate*> bufferUpdateQueue;
			stor::mpmc_queue<Recipe::Texture1DUpdate*> Texture1DUpdateQueue;
			stor::mpmc_queue<Recipe::Texture2DUpdate*> Texture2DUpdateQueue;

			stor::lock::deque<Recipe::Buffer> bufferRecipesQueue;
			stor::lock::deque<Recipe::Texture1D> textures1DRecipesQueue;
			stor::lock::deque<Recipe::Texture2D> textures2DRecipesQueue;
			stor::lock::deque<Recipe::TextureCube> texturesCubeRecipesQueue;
			stor::lock::deque<Recipe::Shader> shadersRecipesQueue;
			stor::lock::deque<Recipe::BoneMatrices> boneMatricesRecipesQueue;


			// overall defines for loading shaders
			std::unordered_map<std::string, std::string> defines;
			
			// co-uploader
			bool coUploaderRunning;
			std::thread coUploader;
			sf::Context coUploaderContext;
			std::thread::id coUploaderThreadID;
			std::mutex coUploadThreadLock;
			std::condition_variable coUploadThreadWait;

			void AddAllocatedMemory(std::size_t append);
			void RemoveAllocatedMemory(std::size_t subtract);

		public:
			OpenGLFactory();
			virtual ~OpenGLFactory();

			virtual void Initialize() {}
			virtual void ReInitialize();

			virtual std::size_t GetAllocatedMemory() const { return allocatedMemory; }

			void Command(const std::string& command, const std::string& value);

			void SetThreadID(const std::thread::id& id);
			virtual bool EqualsSearch(std::string_view search) { return false; }

			/// \brief Load a default shader with a vertex and fragment opengl shader.
			/// \param path		path and filename (relative path to RESOURCES_PATH + SHADER_PATH folder) will look for: path + ".glvs" and path + ".glfs" or else if VERTEX_SHADER_EXTENSION or VERTEX_FRAGMENT_EXTENSION are changed.
			/// \returns		OpenGLShader* to a new created or found shader (with the same opengl shaders).
			virtual cgc::strong_ptr<IShader> LoadShader(std::string_view path);
			cgc::strong_ptr<OpenGLShader> LoadOpenGLShader(std::string_view path);
			/// \brief Load a shader with different opengl shaders.
			/// \param paths	path and filenames (relative path to RESOURCES_PATH + SHADER_PATH folder).
			/// \param types	shader types,  GL_COMPUTE_SHADER, GL_VERTEX_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER or NULL if the shader is not supported.
			/// \returns		OpenGLShader* to a new created or found shader (with the same opengl shaders).
			cgc::strong_ptr<OpenGLShader> LoadShader(std::vector<std::string>& paths, const std::vector<GLenum>& type);

			/// \brief Load an opengl shader, for convenient reasons called: SubShader.
			/// \param path		path and filename (relative path to RESOURCES_PATH + SHADER_PATH folder) will look for: path + ".glvs" and path + ".glfs" or else if VERTEX_SHADER_EXTENSION or VERTEX_FRAGMENT_EXTENSION are changed.
			/// \param type		shader type,  GL_COMPUTE_SHADER, GL_VERTEX_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER or NULL if the shader is not supported.
			/// \returns		OpenGLShader* to a new created or found shader (with the same opengl shaders).
			cgc::strong_ptr<OpenGLSubShader> LoadSubShader(std::string_view path, GLenum type);

			void ParseToShaderPath(std::string& path, const GLenum& type);

			/// \brief Load a material.
			/// \param path		path and filename (relative path to RESOURCES_PATH + MATERIALS_PATH folder) will look for: path + ".mat" files.
			/// \returns		OpenGLMaterial* to a new created or found material (with the same path).
			virtual cgc::strong_ptr<Material> LoadMaterial(std::string_view path, TEXTURE_FILTER textureFlter = Settings::textureFlter);
			virtual cgc::strong_ptr<Material> CreateMaterial(std::string_view path, const rapidjson::Document& json,
				const std::vector<cgc::strong_ptr<Material>>& baseMaterials, TEXTURE_FILTER textureFlter = Settings::textureFlter);
			virtual cgc::strong_ptr<Material> GetMaterial(std::string_view path);

			virtual cgc::strong_ptr<Buffer> LoadBuffer(const ConstructSettings::Buffer& settings, const std::string& name, void* data, std::size_t offset, std::size_t size);
			virtual cgc::strong_ptr<Texture1D> LoadTexture1D(const ConstructSettings::Texture1D& settings, const cgc::strong_ptr<Image>& image = cgc::strong_ptr<Image>());
			virtual cgc::strong_ptr<Texture1D> LoadTexture1D(const ConstructSettings::Texture1D& settings, const std::string& name);
			virtual cgc::strong_ptr<Texture2D> LoadTexture2D(const ConstructSettings::Texture2D& settings, const cgc::strong_ptr<Image>& image = cgc::strong_ptr<Image>());
			virtual cgc::strong_ptr<Texture2D> LoadTexture2D(const ConstructSettings::Texture2D& settings, const std::string& name);
			virtual cgc::strong_ptr<Texture3D> LoadTexture3D(const ConstructSettings::Texture3D& settings, const std::string& name, const std::vector<cgc::strong_ptr<Image>>& images = std::vector<cgc::strong_ptr<Image>>());
			virtual cgc::strong_ptr<TextureCube> LoadTextureCube(const ConstructSettings::TextureCube& settings, const std::string& name, const std::vector<cgc::strong_ptr<Image>>& images = std::vector<cgc::strong_ptr<Image>>());

			virtual cgc::strong_ptr<Texture2D> GetTexture2D(const std::string& name);

			virtual cgc::strong_ptr<RenderObject> LoadRenderObject(const cgc::strong_ptr<AbstractMesh>& mesh, const cgc::strong_ptr<Material>& material, UPLOAD_TYPE uploadType = UPLOAD_TYPE::STATIC, TRIANGLE_TYPE triangleType = TRIANGLE_TYPE::TRIANGLES);
			virtual cgc::strong_ptr<RenderObject> LoadRenderObject(const cgc::strong_ptr<RenderObject>& copy);

			virtual void UpdateBuffer(const cgc::strong_ptr<Buffer>& texture, const void* data, std::size_t offset, std::size_t size, bool enlarge = false);
			virtual void UpdateTexture1D(const cgc::strong_ptr<Texture1D>& texture, const cgc::strong_ptr<Image>& image, uint offset = 0);
			virtual void UpdateTexture2D(const cgc::strong_ptr<Texture2D>& texture, const cgc::strong_ptr<Image>& image, const glm::uvec2& offset = glm::uvec2(0, 0));
			virtual void UpdateMesh(const cgc::strong_ptr<AbstractMesh>& mesh, const cgc::strong_ptr<IMeshData>& meshData);

			virtual LightRenderData LoadLight();

			virtual cgc::strong_ptr<BoneMatrices> LoadBoneMatrices(uint matrixCount);

			cgc::strong_ptr<VAO> LoadVAO();
			cgc::strong_ptr<VBO> LoadVBO(const mm::array_view& vertices, const GLenum drawType = GL_STATIC_DRAW);
			cgc::strong_ptr<EBO> LoadEBO(const mm::array_view& indices, const GLenum bufferType = GL_STATIC_DRAW, TRIANGLE_TYPE triangleType = TRIANGLE_TYPE::TRIANGLES);
			virtual cgc::strong_ptr<IUBO> LoadUBO(void* data, int bufferSize, std::string_view bindingName, UPLOAD_TYPE usageType = UPLOAD_TYPE::STATIC) override;
			cgc::strong_ptr<FBO> LoadFBO(const std::string& name);
			cgc::strong_ptr<RBO> LoadRBO();

			
			const std::vector<LightData>& GetLightsData() const { return lightsData; }

			void CoHandleQueue();
			virtual void HandleQueue();

			void SetShaderDefinition(const std::string key, const std::string value)
			{
				this->defines[key] = value;
			}

			void RemoveShaderDefinition(const std::string& key)
			{
				auto found = this->defines.find(key);
				if (found != this->defines.end())
					this->defines.erase(found);
			}

			std::unique_lock<std::mutex> GetLock()
			{
				coUploadThreadWait.notify_all();
				return std::unique_lock<std::mutex>(coUploadThreadLock);
			}

		private:
				
				void ThreadSafeCreateRenderObject(Recipe::RenderObject* renderObjectRecipe);
				void ThreadSafeCreateBuffer(const Recipe::Buffer& bufferRecipe);
				void ThreadSafeCreateTexture1D(const Recipe::Texture1D& textureRecipe);
				void ThreadSafeCreateTexture2D(const Recipe::Texture2D& textureRecipe);
				void ThreadSafeCreateTextureCube(const Recipe::TextureCube& textureRecipe);
				void ThreadSafeCreateShader(Recipe::Shader& shaderRecipe);
				void ThreadSafeCreateBoneMatrices(const Recipe::BoneMatrices& boneMatricesRecipe);

				void InitializeMaterialUniforms(OpenGLMaterial* material);
		};
	}
}