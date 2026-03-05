#include "HTMLRendererOpenGL.h"
#include <RmlUi/Core/FileInterface.h>

#include <Rendering/Objects/Image.h>

namespace Esteem
{
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

	class HTMLRendererOpenGLGeometryHandler
	{
	public:
		GLuint vertexID, indexID;
		int verticesCount;
		Rml::TextureHandle texture;

		HTMLRendererOpenGLGeometryHandler()
			: vertexID(0)
			, indexID(0)
			, verticesCount(0)
			, texture(0)
		{
		};

		~HTMLRendererOpenGLGeometryHandler()
		{
			if (vertexID)
				glDeleteBuffers(1, &vertexID);

			if (indexID)
				glDeleteBuffers(1, &indexID);

			vertexID = indexID = 0;
		};
	};

	HTMLRendererOpenGL::HTMLRendererOpenGL(sf::RenderWindow* window)
		: window(window)
	{
	}

	void HTMLRendererOpenGL::SetWindow(sf::RenderWindow* Window)
	{
		window = Window;
	}

	sf::RenderWindow* HTMLRendererOpenGL::GetWindow()
	{
		return window;
	}

	// Called by RmlUi when it wants to render geometry that it does not wish to optimize.
	void HTMLRendererOpenGL::RenderGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, const Rml::TextureHandle texture, const Rml::Vector2f& translation)
	{
		window->pushGLStates();
		InitViewport();

		glTranslatef(translation.x, translation.y, 0);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glVertexPointer(2, GL_FLOAT, sizeof(Rml::Vertex), vertices + offsetof(Rml::Vertex, Rml::Vertex::position));
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Rml::Vertex), vertices + offsetof(Rml::Vertex, Rml::Vertex::color));
		glTexCoordPointer(2, GL_FLOAT, sizeof(Rml::Vertex), vertices + offsetof(Rml::Vertex, Rml::Vertex::tex_coord));

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		sf::Texture* sfTexture = (sf::Texture*)texture;

		if (sfTexture)
		{
			sf::Texture::bind(sfTexture);
		}
		else
		{
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glBindTexture(GL_TEXTURE_2D, 0);
		};

		glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, indices);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		glColor4f(1, 1, 1, 1);

		window->popGLStates();
	}

	// Called by RmlUi when it wants to compile geometry it believes will be static for the forseeable future.		
	Rml::CompiledGeometryHandle HTMLRendererOpenGL::CompileGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, const Rml::TextureHandle texture)
	{
		HTMLRendererOpenGLGeometryHandler* geometry = new HTMLRendererOpenGLGeometryHandler();
		geometry->verticesCount = num_indices;

		glGenBuffers(1, &geometry->vertexID);
		glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Rml::Vertex) * num_vertices, vertices,	GL_STATIC_DRAW);

		glGenBuffers(1, &geometry->indexID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->indexID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * num_indices, indices, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		geometry->texture = texture;

		return (Rml::CompiledGeometryHandle)geometry;
	}

	// Called by RmlUi when it wants to render application-compiled geometry.		
	void HTMLRendererOpenGL::RenderCompiledGeometry(Rml::CompiledGeometryHandle geometry, const Rml::Vector2f& translation)
	{
		HTMLRendererOpenGLGeometryHandler* esteemGeometry = reinterpret_cast<HTMLRendererOpenGLGeometryHandler*>(geometry);

		window->pushGLStates();
		InitViewport();

		glTranslatef(translation.x, translation.y, 0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		Texture2D* texture = reinterpret_cast<Texture2D*>(esteemGeometry->texture);

		glBindTexture(GL_TEXTURE_2D, texture ? texture->GetID() : 0);

		glEnable(GL_VERTEX_ARRAY);
		glEnable(GL_TEXTURE_COORD_ARRAY);
		glEnable(GL_COLOR_ARRAY);

		glBindBuffer(GL_ARRAY_BUFFER, esteemGeometry->vertexID);
		glVertexPointer(2, GL_FLOAT, sizeof(Rml::Vertex), (void*)offsetof(Rml::Vertex, Rml::Vertex::position));
		glTexCoordPointer(2, GL_FLOAT, sizeof(Rml::Vertex), (void*)offsetof(Rml::Vertex, Rml::Vertex::tex_coord));
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Rml::Vertex), (void*)offsetof(Rml::Vertex, Rml::Vertex::color));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, esteemGeometry->indexID);
		glDrawElements(GL_TRIANGLES, esteemGeometry->verticesCount, GL_UNSIGNED_INT, nullptr);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDisable(GL_COLOR_ARRAY);
		glDisable(GL_TEXTURE_COORD_ARRAY);
		glDisable(GL_VERTEX_ARRAY);

		glColor4f(1, 1, 1, 1);

		window->popGLStates();
	}

	// Called by RmlUi when it wants to release application-compiled geometry.		
	void HTMLRendererOpenGL::ReleaseCompiledGeometry(Rml::CompiledGeometryHandle geometry)
	{
		delete (HTMLRendererOpenGLGeometryHandler*)geometry;
	}

	// Called by RmlUi when it wants to enable or disable scissoring to clip content.		
	void HTMLRendererOpenGL::EnableScissorRegion(bool enable)
	{
		if (enable)
			glEnable(GL_SCISSOR_TEST);
		else
			glDisable(GL_SCISSOR_TEST);
	}

	// Called by RmlUi when it wants to change the scissor region.		
	void HTMLRendererOpenGL::SetScissorRegion(int x, int y, int width, int height)
	{
		glScissor(x, window->getSize().y - (y + height), width, height);
	}

	// Called by RmlUi when a texture is required by the library.		
	bool HTMLRendererOpenGL::LoadTexture(Rml::TextureHandle& texture_handle, Rml::Vector2i& texture_dimensions, const Rml::String& source)
	{
		cgc::strong_ptr<Texture2D> texture;

		std::string filePath;
		if (source[0] == '?')
			filePath = std::string(source.data() + 1, source.size() - 1);
		else
			filePath = source;

		cppu::hash_t hash = RT_HASH(filePath);
		auto found = textures.find(hash);
		if (found != textures.end())
		{
			texture = found->second;
		}
		else
		{
			if (!(texture = Texture2D::GetTexture(filePath)))
			{
				ConstructSettings::Texture2D constructSettings;
				constructSettings.mipmapped = false;
				constructSettings.repeat = TEXTURE_REPEAT::REPEAT;
				cgc::strong_ptr<Image> image = cgc::construct_new<Image>(filePath, false);
				texture = Texture2D::Construct(constructSettings, image);
			}
			
			textures.try_emplace(hash, texture);
		}

		texture_handle = (Rml::TextureHandle) texture.ptr();
		texture_dimensions = Rml::Vector2i(texture->GetSettings().size.x, texture->GetSettings().size.y);
		
		return true;
	}

	// Called by RmlUi when a texture is required to be built from an internally-generated sequence of pixels.
	bool HTMLRendererOpenGL::GenerateTexture(Rml::TextureHandle& texture_handle, const Rml::byte* source, const Rml::Vector2i& source_dimensions)
	{
		ConstructSettings::Texture2D constructSettings;
		constructSettings.size.x = source_dimensions.x;
		constructSettings.size.y = source_dimensions.y;
		constructSettings.custom = true;
		constructSettings.mipmapped = false;
		//constructSettings.

		cgc::strong_ptr<Image> image = cgc::construct_new<Image>(source,
			source_dimensions.x * source_dimensions.y * sizeof(byte) * 4,
			glm::ivec2(source_dimensions.x, source_dimensions.y), IMAGE_FORMAT::RGBA, TYPE_UBYTE, true);

		cgc::strong_ptr<Texture2D> texture = Texture2D::Construct(constructSettings, image);

		if (!texture)
			return false;

		generatedTextures.push_back(texture);
		texture_handle = reinterpret_cast<Rml::TextureHandle>(texture.ptr());

		return true;
	}

	// Called by RmlUi when a loaded texture is no longer required.		
	void HTMLRendererOpenGL::ReleaseTexture(Rml::TextureHandle texture_handle)
	{
		if (auto found = std::find_if(textures.begin(), textures.end(),
			[texture_handle](const auto& vt)
			{
				return vt.second.ptr() == reinterpret_cast<const cgc::strong_ptr<Texture2D>&>(texture_handle).ptr();
			}
		) != textures.end())
			textures.erase(found);
	}

	void HTMLRendererOpenGL::InitViewport()
	{
		glViewport(0, 0, window->getSize().x, window->getSize().y);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glOrtho(0, window->getSize().x, window->getSize().y, 0, -1, 1);
		glMatrixMode(GL_MODELVIEW);
	}
}