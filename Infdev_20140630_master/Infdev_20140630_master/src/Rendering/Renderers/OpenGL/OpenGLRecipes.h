#pragma once

#include <queue>
#include <vector>
#include <glm/glm.hpp>
#include <cppu/cgc/pointers.h>

#include "../../ConstructSettings.h"

namespace Esteem
{
	class Image;
	class Buffer;
	class Texture1D;
	class Texture2D;
	class TextureCube;

	class AbstractMesh;
	struct IMeshData;
	
	namespace OpenGL
	{
		class OpenGLMaterial;
		class OpenGLShader;
		class OpenGLRenderObject;
		class OpenGLBoneMatrices;

		namespace Recipe
		{
			struct Buffer
			{
				ConstructSettings::Buffer settings;
				cgc::strong_ptr<Esteem::Buffer> buffer;
				const void* data;
				std::size_t offset;
				std::size_t size;

				Buffer()
				{}

				Buffer(const cgc::strong_ptr<Esteem::Buffer>& buffer, const ConstructSettings::Buffer& settings, const void* data, std::size_t offset, std::size_t size)
					: buffer(buffer)
					, settings(settings)
					, data(data)
					, offset(offset)
					, size(size)
				{}
			};

			struct Texture1D
			{
				ConstructSettings::Texture1D settings;
				cgc::strong_ptr<Esteem::Texture1D> texture;
				cgc::strong_ptr<Esteem::Image> image;

				Texture1D(const cgc::strong_ptr<Esteem::Texture1D>& texture, const ConstructSettings::Texture1D& settings, const cgc::strong_ptr<Esteem::Image>& image = cgc::strong_ptr<Esteem::Image>())
					: texture(texture)
					, settings(settings)
					, image(image)
				{}
			};

			struct Texture2D
			{
				ConstructSettings::Texture2D settings;
				cgc::strong_ptr<Esteem::Texture2D> texture;
				cgc::strong_ptr<Esteem::Image> image;

				Texture2D()
				{}

				Texture2D(const cgc::strong_ptr<Esteem::Texture2D>& texture, const ConstructSettings::Texture2D& settings, const cgc::strong_ptr<Esteem::Image>& image = cgc::strong_ptr<Esteem::Image>())
					: texture(texture)
					, settings(settings)
					, image(image)
				{}
			};

			struct TextureCube
			{
				ConstructSettings::TextureCube settings;
				cgc::strong_ptr<Esteem::TextureCube> texture;
				std::vector<cgc::strong_ptr<Esteem::Image>> images;

				TextureCube()
				{}

				TextureCube(const cgc::strong_ptr<Esteem::TextureCube>& texture, const ConstructSettings::TextureCube& settings, const std::vector<cgc::strong_ptr<Esteem::Image>>& images)
					: texture(texture)
					, settings(settings)
					, images(images)
				{}
			};

			struct Shader
			{
				cgc::strong_ptr<OpenGLShader> shader;
				std::queue<cgc::strong_ptr<OpenGLMaterial>> materials;
				std::vector<std::string> paths;
				std::vector<uint> types;

				Shader(const cgc::strong_ptr<OpenGLShader>& shader, const std::vector<std::string>& paths, const std::vector<GLenum>& types)
					: shader(shader)
					, paths(paths)
					, types(types)
				{}

				Shader()
				{};
			};

			struct RenderObject
			{
				cgc::strong_ptr<OpenGLRenderObject> renderObject;
				cgc::strong_ptr<IMeshData> meshData;
				TRIANGLE_TYPE triangleType;
				UPLOAD_TYPE uploadType;

				RenderObject(const cgc::strong_ptr<OpenGLRenderObject>& renderObject, const cgc::strong_ptr<IMeshData>& meshData, UPLOAD_TYPE uploadType = UPLOAD_TYPE::STATIC, TRIANGLE_TYPE triangleType = TRIANGLE_TYPE::TRIANGLES)
					: renderObject(renderObject)
					, meshData(meshData)
					, uploadType(uploadType)
					, triangleType(triangleType)
				{}
			};

			struct BoneMatrices
			{
				cgc::strong_ptr<OpenGLBoneMatrices> boneMatrices;

				BoneMatrices()
				{}

				BoneMatrices(const cgc::strong_ptr<OpenGLBoneMatrices>& boneMatrices)
					: boneMatrices(boneMatrices)
				{}
			};

			struct MeshUpdate
			{
				cgc::strong_ptr<AbstractMesh> mesh;
				cgc::strong_ptr<IMeshData> meshData;

				MeshUpdate(const cgc::strong_ptr<AbstractMesh>& mesh, const cgc::strong_ptr<IMeshData>& meshData)
					: mesh(mesh)
					, meshData(meshData)
				{}
			};

			struct BufferUpdate
			{
				cgc::strong_ptr<Esteem::Buffer> buffer;
				const void* data;
				std::size_t offset;
				std::size_t size;
				bool enlarge;

				BufferUpdate(const cgc::strong_ptr<Esteem::Buffer>& buffer, const void* data, std::size_t offset, std::size_t size, bool enlarge)
					: buffer(buffer)
					, data(data)
					, offset(offset)
					, size(size)
					, enlarge(enlarge)
				{}
			};

			struct Texture1DUpdate
			{
				cgc::strong_ptr<Esteem::Texture1D> texture;
				cgc::strong_ptr<Esteem::Image> image;
				uint offset;

				Texture1DUpdate(const cgc::strong_ptr<Esteem::Texture1D>& texture, const cgc::strong_ptr<Esteem::Image>& image, uint offset)
					: texture(texture)
					, image(image)
					, offset(offset)
				{}
			};

			struct Texture2DUpdate
			{
				cgc::strong_ptr<Esteem::Texture2D> texture;
				cgc::strong_ptr<Esteem::Image> image;
				glm::uvec2 offset;

				Texture2DUpdate(const cgc::strong_ptr<Esteem::Texture2D>& texture, const cgc::strong_ptr<Esteem::Image>& image, const glm::uvec2& offset)
					: texture(texture)
					, image(image)
					, offset(offset)
				{}
			};
		}
	}
}