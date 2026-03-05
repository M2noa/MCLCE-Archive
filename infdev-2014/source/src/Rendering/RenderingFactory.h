#pragma once

#include <string>
#include <rapidjson/document.h>

#include "World/Constituents/Light.h"

#include "General/IFactory.h"
#include "General/Settings.h"

#include "./ConstructSettings.h"
#include "./Objects/IShader.h"
#include "./Objects/IUBO.h"
#include "./Objects/IVBO.h"
#include "./rtypes.h"

#include <cppu/cgc/m_array.h>

namespace Esteem
{
	class Image;
	class Material;
	class Buffer;
	class Texture1D;
	class Texture2D;
	class Texture3D;
	class TextureCube;
	class LightObject;
	class RenderObject;
	class AbstractMesh;
	struct IMeshData;

	class Camera;
	class MeshRenderer;
	class BoneMatrices;
	
	class RenderingFactory : public IFactory
	{
	private:

	protected:
		static RenderingFactory* instance;

		cgc::m_array<Light> lights;

		void AlterBuffer(const cgc::strong_ptr<Buffer>& buffer, uint textrueId, uint bufferId, const ConstructSettings::Buffer* settings);
		void AlterTexture1D(const cgc::strong_ptr<Texture1D>& texture, uint id, const std::string& path, const ConstructSettings::Texture1D* settings, const cgc::strong_ptr<Image>& image);
		void AlterTexture2D(const cgc::strong_ptr<Texture2D>& texture, uint id, const std::string& path, const ConstructSettings::Texture2D* settings, const cgc::strong_ptr<Image>& image);
		void AlterTextureCube(const cgc::strong_ptr<TextureCube>& texture, uint id, const glm::uvec2& size);
		
	public:
		inline static RenderingFactory* Instance() { return instance; }
		
		virtual ~RenderingFactory() = 0;

		virtual std::size_t GetAllocatedMemory() const = 0;
		
		virtual cgc::strong_ptr<IShader> LoadShader(std::string_view path) = 0;

		virtual cgc::strong_ptr<Material> LoadMaterial(std::string_view path, TEXTURE_FILTER textureFlter = Settings::textureFlter) = 0;
		virtual cgc::strong_ptr<Material> CreateMaterial(std::string_view path, const rapidjson::Document& json, const std::vector<cgc::strong_ptr<Material>>& baseMaterials, TEXTURE_FILTER textureFlter = Settings::textureFlter) = 0;
		virtual cgc::strong_ptr<Material> GetMaterial(std::string_view path) = 0;

		virtual cgc::strong_ptr<Buffer> LoadBuffer(const ConstructSettings::Buffer& settings, const std::string& name, void* data, std::size_t offset, std::size_t size) = 0;
		virtual cgc::strong_ptr<Texture1D> LoadTexture1D(const ConstructSettings::Texture1D& settings, const cgc::strong_ptr<Image>& image = cgc::strong_ptr<Image>()) = 0;
		virtual cgc::strong_ptr<Texture1D> LoadTexture1D(const ConstructSettings::Texture1D& settings, const std::string& name) = 0;
		virtual cgc::strong_ptr<Texture2D> LoadTexture2D(const ConstructSettings::Texture2D& settings, const cgc::strong_ptr<Image>& image = cgc::strong_ptr<Image>()) = 0;
		virtual cgc::strong_ptr<Texture2D> LoadTexture2D(const ConstructSettings::Texture2D& settings, const std::string& name) = 0;
		virtual cgc::strong_ptr<Texture3D> LoadTexture3D(const ConstructSettings::Texture3D& settings, const std::string& name, const std::vector<cgc::strong_ptr<Image>>& images = std::vector<cgc::strong_ptr<Image>>()) = 0;
		virtual cgc::strong_ptr<TextureCube> LoadTextureCube(const ConstructSettings::TextureCube& settings, const std::string& name, const std::vector<cgc::strong_ptr<Image>>& images = std::vector<cgc::strong_ptr<Image>>()) = 0;

		virtual cgc::strong_ptr<Texture2D> GetTexture2D(const std::string& name) = 0;

		virtual void UpdateBuffer(const cgc::strong_ptr<Buffer>& texture, const void* data, std::size_t offset, std::size_t size, bool enlarge = false) = 0;
		virtual void UpdateTexture1D(const cgc::strong_ptr<Texture1D>& texture, const cgc::strong_ptr<Image>& image, uint offset = 0) = 0;
		virtual void UpdateTexture2D(const cgc::strong_ptr<Texture2D>& texture, const cgc::strong_ptr<Image>& image, const glm::uvec2& offset = glm::uvec2(0, 0)) = 0;

		virtual cgc::strong_ptr<RenderObject> LoadRenderObject(const cgc::strong_ptr<AbstractMesh>& mesh, const cgc::strong_ptr<Material>& material, UPLOAD_TYPE uploadType = UPLOAD_TYPE::STATIC, TRIANGLE_TYPE triangleType = TRIANGLE_TYPE::TRIANGLES) = 0;
		virtual cgc::strong_ptr<RenderObject> LoadRenderObject(const cgc::strong_ptr<RenderObject>& copy) = 0;
		virtual void UpdateMesh(const cgc::strong_ptr<AbstractMesh>& mesh, const cgc::strong_ptr<IMeshData>& meshData) = 0;

		virtual LightRenderData LoadLight() = 0;
		virtual cgc::strong_ptr<BoneMatrices> LoadBoneMatrices(uint matrixCount) = 0;

		virtual cgc::strong_ptr<IUBO> LoadUBO(void* data, int bufferSize, std::string_view bindingName, UPLOAD_TYPE usageType = UPLOAD_TYPE::STATIC) = 0;

		virtual void Initialize() = 0;
		virtual void ReInitialize() = 0;
		virtual bool EqualsSearch(std::string_view search) = 0;
		
		virtual void HandleQueue() = 0;

	};

	inline RenderingFactory::~RenderingFactory() {}
}