#include "OpenGLFactory.h"

#include <SFML/Graphics.hpp>

#include "Utils/Data.h"
#include "Utils/Debug.h"
#include "Utils/StringParser.h"
#include "Utils/CPreProcessor.h"
#include "./OpenGLDebug.h"

#include "../../Objects/Image.h"
#include "World/Objects/Entity.h"
#include "World/World.h"

#include "Rendering/Objects/AbstractMesh.h"

#ifndef WIN32
#include <pthread.h>
#include <sched.h>
#endif

#define AUTO_CREATE_MATERIALS
#define sizeofmember(type, member) sizeof(type::member)

namespace Esteem
{
	namespace OpenGL
	{
		OpenGLFactory::OpenGLFactory()
			: coUploaderContext()
			, coUploaderRunning(true)
			, allocatedMemory(0)
		{
			if (RenderingFactory::instance)
				Debug::LogError("OpenGLFactory created while another factory is already present, newly created instance has been ignored.");
			else
				RenderingFactory::instance = this;

			// co-uploader
			coUploaderContext.setActive(false);
			coUploader = std::thread(&OpenGLFactory::CoHandleQueue, this);
#ifdef WIN32
			SetThreadPriority(coUploader.native_handle(), THREAD_PRIORITY_BELOW_NORMAL);
#else
			int retcode;
			int policy;
			struct sched_param schedParam;

			if ((retcode = pthread_getschedparam(coUploader.native_handle(), &policy, &schedParam)) != 0)
				std::cout << "pthread_getschedparam, error code: " << retcode << " (OpenGL CoUploader)" << std::endl;

			schedParam.sched_priority = 4;
			policy = SCHED_OTHER;
			if ((retcode = pthread_setschedparam(coUploader.native_handle(), policy, &schedParam)) != 0)
				std::cout << "pthread_setschedparam, error code: " << retcode << " (OpenGL CoUploader)" << std::endl;
#endif

			Data::Register(this);
			lightsData.reserve(256);

			Command::RegisterListener("reload", DELEGATE(&OpenGLFactory::Command, this));

			// Create default texture
			uint8 defaultTextureData[4 * 3]{ 255, 0, 255, 0, 0, 0, 0, 0, 0, 255, 0, 255 };

			glGenTextures(1, &defaultTextureId);
			glBindTexture(GL_TEXTURE_2D, defaultTextureId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, &defaultTextureData);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		void OpenGLFactory::SetThreadID(const std::thread::id& id)
		{
			threadID = id;
		}

		OpenGLFactory::~OpenGLFactory()
		{
			Data::UnRegister(this);

			coUploaderRunning = false;
			coUploadThreadWait.notify_all();
			coUploader.join();

			// should be removed through asset_ptrs
			//for(auto it = subShaders.begin(); it != subShaders.end();)
			//	delete (it++)->second;

			if (RenderingFactory::instance)
				RenderingFactory::instance = nullptr;
		}

		void OpenGLFactory::ReInitialize()
		{
			for (auto it = vaos.begin(); it != vaos.end(); ++it)
			{
				cgc::strong_ptr<VAO> vao = it->second;

				// set new id
				uint id;
				glGenVertexArrays(1, &id);
				glBindVertexArray(id);
				vao->id = id;

				// get values
				cgc::strong_ptr<EBO> ebo = vao->ebo;
				cgc::strong_ptr<VBO> vbo = vao->vbo;

				// clear
				vao->ebo = cgc::strong_ptr<EBO>();
				vao->vbo = cgc::strong_ptr<VBO>();

				// reattach
				vao->AttachEBO(ebo);
				vao->AttachVBO(vbo);
			}

			for (auto it = fbos.begin(); it != fbos.end(); ++it)
			{
				FBO& fbo = *it->second;
				fbo.buffers.clear();

				// get new id
				uint id;
				glGenFramebuffers(1, &id);
				glBindFramebuffer(GL_FRAMEBUFFER, id);
				fbo.id = id;

				for (auto itField = fbo.attachedRenderTextures.begin(); itField != fbo.attachedRenderTextures.end(); ++itField)
					fbo.AttachRenderTexture(itField->second, itField->first);

				if (fbo.attachedDepthTexture != nullptr)
					fbo.AttachDepthTexture(fbo.attachedDepthTexture);

				if (fbo.attachedStencilTexture != nullptr)
					fbo.AttachStencilTexture(fbo.attachedStencilTexture);

				for (auto itField = fbo.attachedRender3DTextures.begin(); itField != fbo.attachedRender3DTextures.end(); ++itField)
					fbo.AttachRenderTexture(itField->second, itField->first);

				if (fbo.attachedDepth3DTexture != nullptr)
					fbo.AttachDepthTexture(fbo.attachedDepth3DTexture);

				if (fbo.attachedStencil3DTexture != nullptr)
					fbo.AttachStencilTexture(fbo.attachedStencil3DTexture);

				for (auto itField = fbo.attachedRenderBuffers.begin(); itField != fbo.attachedRenderBuffers.end(); ++itField)
					fbo.AttachRenderBuffer(itField->second, itField->first);

				if (fbo.attachedDepthBuffer != nullptr)
					fbo.AttachDepthBuffer(fbo.attachedDepthBuffer);

				if (fbo.attachedStencilBuffer != nullptr)
					fbo.AttachStencilBuffer(fbo.attachedStencilBuffer);

				if (!fbo.Validate())
					Debug::LogError("Could not recreate FBO correctly");
			}

			for (auto itShader = shaders.begin(); itShader != shaders.end(); ++itShader)
			{
				for (const auto& uniform : itShader->second->GetAllUniformVariables())
				{
					if (uniform.second.type == GL_UNIFORM_BLOCK_BINDING)
					{
						auto emplaced = uboIndices.try_emplace(uniform.first, uboIndices.size());
						glUniformBlockBinding(itShader->second->GetShaderID(), uniform.second.index, emplaced.first->second);
					}
				}
			}
		}

		void OpenGLFactory::Command(const std::string& command, const std::string& value)
		{
			switch(RT_HASH(value))
			{
			case CT_HASH("shaders"):
				Debug::Log("reloading shaders- not yet fully functional");

				for (auto itShader = shaders.begin(); itShader != shaders.end(); ++itShader)
				{
					for (const auto& uniform : itShader->second->GetAllUniformVariables())
					{
						if (uniform.second.type == GL_UNIFORM_BLOCK_BINDING)
						{
							auto emplaced = uboIndices.try_emplace(uniform.first, uboIndices.size());
							glUniformBlockBinding(itShader->second->GetShaderID(), uniform.second.index, emplaced.first->second);
						}
					}
				}

				Debug::Log("shaders reloaded");
				break;
			case CT_HASH("renderobjects"):
				Debug::Log("reloading VBOs");

				/*auto& arrays = renderObjects.get_arrays();
				for (size_t a = 0; a < arrays.size(); ++a)
				{
					auto& renderObjectsArray = *arrays[a];
					for (size_t i = 0; i < renderObjectsArray.size(); ++i)
					{
						renderObjectsArray[i]->vao = cgc::strong_ptr<VAO>();
						if (renderObjectsArray[i]->GetMesh()->GetMeshData())
							ThreadSafeCreateRenderObject(new Recipe::RenderObject(renderObjectsArray[i], renderObjectsArray[i]->GetMesh()->GetMeshData()));
					}
				}

				Debug::Log("VBOs reloaded");*/
				Debug::Log("VBOs can't be reloaded at this time, developers still need to re-integrate this");
				break;
			}
		}

		cgc::strong_ptr<IShader> OpenGLFactory::LoadShader(std::string_view path)
		{
			return LoadOpenGLShader(path);
		}

		cgc::strong_ptr<OpenGLShader> OpenGLFactory::LoadOpenGLShader(std::string_view path)
		{
			std::vector<std::string> paths;
			std::vector<GLenum> types;

			paths.emplace_back(path);
			types.push_back(GL_VERTEX_SHADER);
			paths.emplace_back(path);
			types.push_back(GL_FRAGMENT_SHADER);

			return LoadShader(paths, types);
		}

		cgc::strong_ptr<OpenGLShader> OpenGLFactory::LoadShader(std::vector<std::string>& paths, const std::vector<GLenum>& types)
		{
			for (uint i = 0; i < paths.size(); ++i)
				ParseToShaderPath(paths[i], types[i]);

			std::string uniqueName = OpenGLShader::UniqueName(paths);

			// Shader already loaded?
			auto foundShader = shaders.find(RT_HASH(uniqueName));
			if (foundShader != shaders.end())
				return foundShader->second;

			cgc::strong_ptr<OpenGLShader> shader = shaders.emplace(RT_HASH(uniqueName), 0u, uniqueName);
			Recipe::Shader recipe(shader, paths, types);
			ThreadSafeCreateShader(recipe);

			return shader;
		}

		cgc::strong_ptr<OpenGLSubShader> OpenGLFactory::LoadSubShader(std::string_view path, GLenum type)
		{
			// SubShader already loaded?
			//auto foundSubShader = subShaders.find(RT_HASH(path));
			//if (foundSubShader != subShaders.end())
			//	return foundSubShader->second;

			// we would like to know all the defines on the end
			std::unordered_map<std::string, std::string> defines;
			defines.insert(this->defines.begin(), this->defines.end());

			// Read file and continue when we got input
			std::string contents;
			size_t size;
			cgc::strong_ptr<std::istream> stream = Data::StreamAsset(path, size);
			if (stream)
			{
				std::vector<std::string> includes;
				contents = CPreProcessor::ProcessStreamToString(stream, std::string(path), RESOURCES_PATH + SHADERS_PATH, defines, includes);
			}
			else
				Debug::LogError("Could not find shader: ", path);

			if (contents != "") // ReadFile() will error to the user
			{
				switch (type)
				{
				case GL_VERTEX_SHADER:
					StringParser::ReplaceAll(contents, "POSITION", "vec4(vertex, 1)");
					break;
				case GL_FRAGMENT_SHADER:
					StringParser::ReplaceAll(contents, "POSITION", "position");
					break;
				case GL_GEOMETRY_SHADER:

					break;
#ifdef GL_COMPUTE_SHADER
				case GL_COMPUTE_SHADER:

					break;
#endif
				case GL_TESS_CONTROL_SHADER:

					break;
				case GL_TESS_EVALUATION_SHADER:

					break;
				}

				const char* fileContents = contents.c_str();
				uint id = glCreateShader(type);

				glShaderSource(id, 1, &fileContents, NULL);
				glCompileShader(id);

				GLint compileStatus;
				glGetShaderiv(id, GL_COMPILE_STATUS, &compileStatus);

				if (compileStatus == GL_FALSE)
				{
					char error[1024];
					glGetShaderInfoLog(id, 1024, NULL, error);

					/*std::string value;
					int rowNumber = 0;

					std::stringstream stream = std::stringstream(error);
					stream.seekg(std::ios::beg);
					stream.ignore(std::numeric_limits<std::streamsize>::max(), '(');
					std::getline(stream, value, ')');
					rowNumber = stoi(value);

					stream = std::stringstream(contents);
					stream.seekg(std::ios::beg);
					for (int i = 0; i < rowNumber - 1; ++i)
						stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

					std::getline(stream, value);*/

					Debug::LogError("Compiling shader \"", path, "\" failed: \n", std::string(error), "\n");
					std::istringstream iss(contents);
					std::string line;
					size_t nr = 1;
					while (std::getline(iss, line))
					{
						std::string lineNrString = std::to_string(nr++);
						Debug::Log(std::string(std::max(4 - lineNrString.size(), size_t(0)), ' '), lineNrString, ": ", line, '\n');
					}
				}
				else // successfully compiled create our object and send it back
				{
					cgc::strong_ptr<OpenGLSubShader> shader = subShaders.emplace(RT_HASH(path), type, id);
					auto found = defines.find("cutoff");
					if (found != defines.end())
						shader->cutoff = stof(found->second);

					return shader;
				}

				// not successfuly compiled delete the incomplete shader and return null
				glDeleteShader(id);
			}

			return cgc::strong_ptr<OpenGLSubShader>();
		}

		void OpenGLFactory::ParseToShaderPath(std::string& path, const GLenum& type)
		{
			// Attach extension to the file
			switch (type)
			{
			case GL_VERTEX_SHADER:
				path += VERTEX_SHADER_EXTENSION;
				break;
			case GL_FRAGMENT_SHADER:
				path += FRAGMENT_SHADER_EXTENSION;
				break;
			case GL_GEOMETRY_SHADER:
				path += GEOMETRY_SHADER_EXTENSION;
				break;
#ifdef GL_COMPUTE_SHADER
			case GL_COMPUTE_SHADER:
				path += FRAGMENT_SHADER_EXTENSION;
				break;
#endif
			case GL_TESS_CONTROL_SHADER:
				path += TESSELATION_CONTROL_SHADER_EXTENSION;
				break;
			case GL_TESS_EVALUATION_SHADER:
				path += TESSELATION_EVALUATION_SHADER_EXTENSION;
				break;
			}

			path = RESOURCES_PATH + SHADERS_PATH + path;
		}

		cgc::strong_ptr<Material> OpenGLFactory::LoadMaterial(std::string_view path, TEXTURE_FILTER textureFlter)
		{
			std::string newPath(path);
			if (path.empty())
				newPath = "def_diffuse";

			newPath = RESOURCES_PATH + MATERIALS_PATH + newPath.substr(0, newPath.find(".")) + ".mat";

			// Material already loaded ?
			auto foundMaterial = materials.find(RT_HASH(path));
			if (foundMaterial != materials.end())
				return static_cast<cgc::strong_ptr<OpenGLMaterial>>(foundMaterial->second);


			// read file and look for # commands
			std::string fileContents = Data::ReadAsset(newPath);
			if (fileContents.length() <= 0)
			{
#ifdef AUTO_CREATE_MATERIALS
				uint beginPos = path.find_last_of("/");
				if (beginPos == std::string::npos)
					beginPos = path.find_last_of("\\");
				if (beginPos == std::string::npos)
					beginPos = 0;

				std::string materialName(path.substr(beginPos + 1, path.find(".")));
				fileContents = std::string("{\n  \"name\": \"" + materialName + "\",\n  \"shader\" : \"diffuse\",\n  \"alpha\" : false,\n  \"textures\": {\n    \"texture\": \"").append(path) + ".tga\"\n  }\n}";
				//Data::WriteFile(path, fileContents);

				Debug::LogWarning("Couldn't load material \"", path, "\", creating default material.");
#else
				return LoadMaterial(std::string("def_diffuse"));
#endif
			}

			uint startJson = fileContents.find("{");
			std::vector<cgc::strong_ptr<Material>> baseMaterials;

			if (startJson != std::string::npos)
			{
				std::vector<HashCommand> hashCommands = CPreProcessor::RetrieveHashCommands(fileContents.substr(0, startJson));
				for (uint i = 0; i < hashCommands.size(); ++i)
				{
					if (hashCommands[i].command == "#include")
					{
						cgc::strong_ptr<Material> baseMaterial = LoadMaterial(hashCommands[i].value);
						if (baseMaterial != nullptr)
							baseMaterials.push_back(cgc::static_pointer_cast<OpenGLMaterial>(baseMaterial));
						else
							Debug::LogError("Material: Could not load base material: " + hashCommands[i].value);
					}

				}
				fileContents = fileContents.substr(startJson, fileContents.length() - startJson);
			}

			// Create new material
			rapidjson::Document jsonDoc = Data::ReadJSON(fileContents.c_str());
			if (jsonDoc.ObjectEmpty())
				return LoadMaterial(std::string("def_diffuse")); // ReadJsonDocument already reports errors

			cgc::strong_ptr<Material> material = CreateMaterial(path, jsonDoc, baseMaterials, textureFlter);

			return material;
		}

		cgc::strong_ptr<Material> OpenGLFactory::CreateMaterial(std::string_view path, const rapidjson::Document& json, const std::vector<cgc::strong_ptr<Material>>& baseMaterials, TEXTURE_FILTER textureFlter)
		{
			// initialize material with json data
			std::string name;;

			// Get name
			auto found = json.FindMember("name");
			name = found != json.MemberEnd() ? found->value.GetString() : "UNKNOWN";

			cgc::strong_ptr<OpenGLMaterial> material = materials.emplace(RT_HASH(path), materials.size(), path, name);

			// set all included material values as default values
			for (uint i = 0; i < baseMaterials.size(); ++i)
			{
				OpenGLMaterial* baseMaterial = static_cast<OpenGLMaterial*>(baseMaterials[i].ptr());

				material->shader = baseMaterial->shader;
				material->transparent = baseMaterial->transparent;
				material->doubleSided = baseMaterial->doubleSided;

				material->buffers = baseMaterial->buffers;

				for (auto it = baseMaterial->activeUniformValues.begin(); it != baseMaterial->activeUniformValues.end(); ++it)
					material->unactiveUniformValues[it->first] = it->second; // yes place them in the unactive list

				for (auto it = baseMaterial->unactiveUniformValues.begin(); it != baseMaterial->unactiveUniformValues.end(); ++it)
					material->unactiveUniformValues[it->first] = it->second;
			}

			// Alpha and reflective settings
			if ((found = json.FindMember("transparent")) != json.MemberEnd())
				material->transparent = found->value.IsBool() ? found->value.GetBool() : false;

			if ((found = json.FindMember("doublesided")) != json.MemberEnd())
				material->doubleSided = found->value.IsBool() ? found->value.GetBool() : false;

			if ((found = json.FindMember("reflective")) != json.MemberEnd())
				material->reflective = found->value.IsBool() ? found->value.GetBool() : false;

			if ((found = json.FindMember("textures")) != json.MemberEnd())
			{
				// Load texture data
				const rapidjson::Value& texturesRoot = found->value;
				for (auto it = texturesRoot.MemberBegin(); it != texturesRoot.MemberEnd(); it++)
				{
					std::string path = std::string(it->value.GetString());
					cgc::strong_ptr<Texture2D> texture2D;

					auto found = originalTexture2Ds.find(RT_HASH(path));
					if (found != originalTexture2Ds.end())
						texture2D = found->second;
					else
					{
						cgc::strong_ptr<Image> image = cgc::construct_new<Image>();
						if (image->SetFilePath(RESOURCES_PATH + TEXTURES_PATH + path))
						{
							ConstructSettings::Texture2D settings;
							settings.mipmapped = true;
							settings.keepImageOnCPU = false;
							settings.repeat = TEXTURE_REPEAT::REPEAT;
							settings.textureFilter = TEXTURE_FILTER::NEAREST;
							settings.mipMapFilter = textureFlter == TEXTURE_FILTER::NEAREST ? TEXTURE_FILTER::NEAREST_MIPMAP_NEAREST : TEXTURE_FILTER::LINEAR_MIPMAP_LINEAR;
							settings.format = TEXTURE_FORMAT::RGBA;
							settings.size = image->GetSize();

							texture2D = LoadTexture2D(settings, image);
						}
					}

					if (texture2D != nullptr)
					{
						std::string str(it->name.GetString());
						material->SetTexture2D(RT_HASH(str), texture2D);
					}
				}
			}

			if ((found = json.FindMember("uniform")) != json.MemberEnd())
			{
				// load all uniform data
				const rapidjson::Value& uniformRoot = found->value;
				for (auto it = uniformRoot.MemberBegin(); it != uniformRoot.MemberEnd(); it++)
				{
					OpenGLShader::UniformValue uniformValue;

					std::string* key = new std::string(it->name.GetString());
					const rapidjson::Value& value = it->value;
					if (value.IsArray())
					{
						// double to float parsing
						if (value[0u].IsDouble())
						{
							uint size = value.Size() > 4 ? 4 : value.Size();
							uniformValue.type = (size == 1) ? GL_FLOAT : GL_FLOAT_VEC2 + (size - 2);
							uniformValue.value.resize(size * sizeof(float));

							for (uint i = 0; i < size; ++i)
							{
								float number = 0;
								if (value[i].IsDouble())
									number = float(value[i].GetDouble());

								memcpy(&uniformValue.value[i * sizeof(float)], &number, sizeof(float));
							}

						}
						// integer parsing
						else if (value[0u].IsInt() || value[0u].IsInt64())
						{
							uint size = value.Size() > 4 ? 4 : value.Size();
							uniformValue.type = (size == 1) ? GL_INT : GL_INT_VEC2 + (size - 2);
							uniformValue.value.resize(size * sizeof(int));

							for (uint i = 0; i < size; ++i)
							{
								int number = 0;
								if (value[i].IsInt())
									number = value[i].GetInt();
								else if (value[i].IsInt64())
									number = int(value[0u].GetInt64());

								memcpy(&uniformValue.value[i * sizeof(int)], &number, sizeof(int));
							}
						}
						// boolean vector?
						else if (value[0u].IsBool())
						{
							uint size = value.Size() > 4 ? 4 : value.Size();
							uniformValue.type = (size == 1) ? GL_BOOL : GL_BOOL_VEC2 + (size - 2);
							uniformValue.value.resize(size * sizeof(bool));

							for (uint i = 0; i < size; ++i)
							{
								bool boolean = 0;
								if (value[i].IsInt())
									boolean = value[i].GetBool();

								memcpy(&uniformValue.value[i * sizeof(bool)], &boolean, sizeof(bool));
							}
						}
					}
					else if (value.IsDouble())
					{
						uniformValue.value.resize(sizeof(float));
						float number = float(value.GetDouble());
						memcpy(&uniformValue.value[0], &number, sizeof(float));
						uniformValue.type = GL_FLOAT;
					}
					else if (value.IsInt())
					{
						uniformValue.value.resize(sizeof(int));
						int number = value.GetInt();
						memcpy(&uniformValue.value[0], &number, sizeof(int));
						uniformValue.type = GL_INT;
					}
					else if (value.IsInt64())
					{
						uniformValue.value.resize(sizeof(int));
						int number = int(value.GetInt64());
						memcpy(&uniformValue.value[0], &number, sizeof(int));
						uniformValue.type = GL_INT;
					}
					else if (value.IsBool())
					{
						uniformValue.value.resize(sizeof(bool));
						bool boolean = value.GetBool();
						memcpy(&uniformValue.value[0], &boolean, sizeof(bool));
						uniformValue.type = GL_BOOL;
					}

					material->unactiveUniformValues[key->c_str()] = uniformValue;
				}
			}

			// Load shaders
			found = json.FindMember("openglshaders");
			const rapidjson::Value& shader = found != json.MemberEnd() ? found->value : json["shader"];
			if (shader.IsString())
				material->shader = LoadOpenGLShader(std::string(shader.GetString()));
			else if (shader.IsObject())
			{
				std::vector<std::string> paths;
				std::vector<GLenum> types;

				for (auto it = shader.MemberBegin(); it != shader.MemberEnd(); it++)
				{
					paths.push_back(std::string(it->value.GetString()));
					std::string key = it->name.GetString();
					switch (RT_HASH(key))
					{
					case CT_HASH("vertex"):
						types.push_back(GL_VERTEX_SHADER);
						break;
					case CT_HASH("fragment"):
						types.push_back(GL_FRAGMENT_SHADER);
						break;
					case CT_HASH("compute"):
#ifdef GL_COMPUTE_SHADER
						types.push_back(GL_COMPUTE_SHADER);
#else
						types.push_back(NULL);
#endif
						break;
					case CT_HASH("tess_control"):
						types.push_back(GL_TESS_CONTROL_SHADER);
						break;
					case CT_HASH("tess_evaluation"):
						types.push_back(GL_TESS_EVALUATION_SHADER);
						break;
					case CT_HASH("geometry"):
						types.push_back(GL_GEOMETRY_SHADER);
						break;
					}
				}

				material->shader = LoadShader(paths, types);
			}

			// add material to shader list so we can update this material's uniforms when they are available
			cgc::strong_ptr<OpenGLShader> s = material->shader;
			const std::deque<Recipe::Shader>::iterator shaderRecipe = shadersRecipesQueue.find_if([s](Recipe::Shader& a) -> bool { return a.shader == s; });
			if (shaderRecipe != shadersRecipesQueue.end())
				shaderRecipe->materials.push(material);
			else
				InitializeMaterialUniforms(material.ptr());

			return material;
		}

		cgc::strong_ptr<Material> OpenGLFactory::GetMaterial(std::string_view path)
		{
			auto foundMaterial = materials.find(RT_HASH(path));
			if (foundMaterial != materials.end())
				return static_cast<cgc::strong_ptr<OpenGLMaterial>>(foundMaterial->second);

			return nullptr;
		}

		void OpenGLFactory::InitializeMaterialUniforms(OpenGLMaterial* material)
		{
			if (material->shader == nullptr)
				return;

			cgc::strong_ptr<OpenGLShader> shader = material->shader;
			for (auto it = material->unactiveUniformValues.begin(); it != material->unactiveUniformValues.end();)
			{
				auto found = shader->uniforms.find(RT_HASH(it->first));
				if (found != shader->uniforms.end())
				{
					if (it->second.type == found->second.type)
					{
						it->second.variable = &found->second;
						it->second.active = true;

						switch (it->second.type)
						{
						case GL_FLOAT:
							it->second.func = OpenGLShader::SetUniform1f;
							break;
						case GL_FLOAT_VEC2:
							it->second.func = OpenGLShader::SetUniform2f;
							break;
						case GL_FLOAT_VEC3:
							it->second.func = OpenGLShader::SetUniform3f;
							break;
						case GL_FLOAT_VEC4:
							it->second.func = OpenGLShader::SetUniform4f;
							break;
						case GL_INT:
							it->second.func = OpenGLShader::SetUniform1i;
							break;
						case GL_INT_VEC2:
							it->second.func = OpenGLShader::SetUniform2i;
							break;
						case GL_INT_VEC3:
							it->second.func = OpenGLShader::SetUniform3i;
							break;
						case GL_INT_VEC4:
							it->second.func = OpenGLShader::SetUniform4i;
							break;
						case GL_UNSIGNED_INT:
							it->second.func = OpenGLShader::SetUniform1ui;
							break;
						case GL_UNSIGNED_INT_VEC2:
							it->second.func = OpenGLShader::SetUniform2ui;
							break;
						case GL_UNSIGNED_INT_VEC3:
							it->second.func = OpenGLShader::SetUniform3ui;
							break;
						case GL_UNSIGNED_INT_VEC4:
							it->second.func = OpenGLShader::SetUniform4ui;
							break;
						}


						material->activeUniformValues[it->first] = it->second;
						it = material->unactiveUniformValues.erase(it);

						continue;
					}
					else
						Debug::LogWarning("OpenGLFactory: Material types mismatch");
				}

				it++;
			}
		}

		cgc::strong_ptr<Buffer> OpenGLFactory::LoadBuffer(const ConstructSettings::Buffer& settings, const std::string& name, void* data, std::size_t offset, std::size_t size)
		{
			cgc::strong_ptr<Buffer> buffer;
			buffer = buffers.emplace(settings, defaultTextureId, 0, name);

			ThreadSafeCreateBuffer(Recipe::Buffer(buffer, settings, data, offset, size));

			return buffer;
		}

		cgc::strong_ptr<Texture1D> OpenGLFactory::LoadTexture1D(const ConstructSettings::Texture1D& settings, const cgc::strong_ptr<Image>& image)
		{
			cgc::strong_ptr<Texture1D> texture;
			if (image)
			{
				// Texture already loaded ?
				if (!settings.custom)
				{
					auto foundTexture = originalTexture1Ds.find(RT_HASH(image->GetPath()));
					if (foundTexture != originalTexture1Ds.end())
						return foundTexture->second;
				}

				// Will be used in the queue
				texture = texture1Ds.emplace(settings, defaultTextureId, image->GetPath(), image);

				if (!settings.custom)
					originalTexture1Ds.emplace(RT_HASH(image->GetPath()), texture);

				ThreadSafeCreateTexture1D(Recipe::Texture1D(texture, settings, image));
			}

			return texture;
		}

		cgc::strong_ptr<Texture1D> OpenGLFactory::LoadTexture1D(const ConstructSettings::Texture1D& settings, const std::string& name)
		{
			cgc::strong_ptr<Texture1D> texture;
			// Texture already loaded ?
			if (!settings.custom)
			{
				auto foundTexture = originalTexture1Ds.find(RT_HASH(name));
				if (foundTexture != originalTexture1Ds.end())
					return foundTexture->second;
			}

			// Will be used in the queue
			texture = texture1Ds.emplace(settings, defaultTextureId, name);

			if (!settings.custom)
				originalTexture1Ds.emplace(RT_HASH(name), texture);

			ThreadSafeCreateTexture1D(Recipe::Texture1D(texture, settings));

			return texture;
		}

		cgc::strong_ptr<Texture2D> OpenGLFactory::LoadTexture2D(const ConstructSettings::Texture2D& settings, const cgc::strong_ptr<Image>& image)
		{
			cgc::strong_ptr<Texture2D> texture;
			if (image)
			{
				// Texture already loaded ?
				if (!settings.custom)
				{
					auto foundTexture = originalTexture2Ds.find(RT_HASH(image->GetPath()));
					if (foundTexture != originalTexture2Ds.end())
						return foundTexture->second;
				}

				// Will be used in the queue
				texture = texture2Ds.emplace(settings, defaultTextureId, image->GetPath(), image);

				if (!settings.custom)
					originalTexture2Ds.emplace(RT_HASH(image->GetPath()), texture);

				ThreadSafeCreateTexture2D(Recipe::Texture2D(texture, settings, image));
			}

			return texture;
		}

		cgc::strong_ptr<Texture2D> OpenGLFactory::LoadTexture2D(const ConstructSettings::Texture2D& settings, const std::string& name)
		{
			cgc::strong_ptr<Texture2D> texture;
			// Texture already loaded ?
			if (!settings.custom)
			{
				auto foundTexture = originalTexture2Ds.find(RT_HASH(name));
				if (foundTexture != originalTexture2Ds.end())
					return foundTexture->second;
			}

			// Will be used in the queue
			texture = texture2Ds.emplace(settings, defaultTextureId, name);

			if (!settings.custom)
				originalTexture2Ds.emplace(RT_HASH(name), texture);

			ThreadSafeCreateTexture2D(Recipe::Texture2D(texture, settings));

			return texture;
		}

		cgc::strong_ptr<Texture3D> OpenGLFactory::LoadTexture3D(const ConstructSettings::Texture3D& settings, const std::string& name, const std::vector<cgc::strong_ptr<Image>>& images)
		{
			return cgc::strong_ptr<Texture3D>();
		}


		cgc::strong_ptr<Texture2D> OpenGLFactory::GetTexture2D(const std::string& name)
		{
			auto foundTexture = originalTexture2Ds.find(RT_HASH(name));
			if (foundTexture != originalTexture2Ds.end())
				return foundTexture->second;

			return {};
		}

		void OpenGLFactory::UpdateBuffer(const cgc::strong_ptr<Buffer>& buffer, const void* data, std::size_t offset, std::size_t size, bool enlarge)
		{
			if (data)
			{
				std::thread::id thisThreadID = std::this_thread::get_id();
				if (!(thisThreadID == threadID || thisThreadID == coUploaderThreadID))
				{
					bufferUpdateQueue.push(new Recipe::BufferUpdate(buffer, data, offset, size, enlarge));
					coUploadThreadWait.notify_one();

					return;
				}

				glBindBuffer(GL_TEXTURE_BUFFER, buffer->GetBufferID());
				if (enlarge && offset + size > buffer->GetSettings().size)
				{
					RemoveAllocatedMemory(buffer->GetSettings().size);
					glBufferData(GL_TEXTURE_BUFFER, offset + size, data, GL_DYNAMIC_DRAW);
					const_cast<Buffer::Settings&>(buffer->GetSettings()).size = offset + size;
					AddAllocatedMemory(offset + size);
				}
				else
					glBufferSubData(GL_TEXTURE_BUFFER, offset, size, data);

				glBindBuffer(GL_TEXTURE_BUFFER, 0);
				LogOpenGLErrors();
			}
		}

		void OpenGLFactory::UpdateTexture1D(const cgc::strong_ptr<Texture1D>& texture, const cgc::strong_ptr<Image>& image, uint offset)
		{
			std::thread::id thisThreadID = std::this_thread::get_id();
			if (!(thisThreadID == threadID || thisThreadID == coUploaderThreadID))
			{
				Texture1DUpdateQueue.push(new Recipe::Texture1DUpdate(texture, image, offset));
				coUploadThreadWait.notify_one();

				return;
			}

			Image* img = image.ptr();
			if (img && img->LoadFileIfNotLoaded())
			{
				glBindTexture(GL_TEXTURE_1D, texture->GetID());
				if (image->GetSize().x <= texture->GetSettings().size)
					glTexSubImage1D(GL_TEXTURE_1D, 0, (int)offset, (int)img->GetSize().x, GL_RGBA_INTEGER, GL_SHORT, img->GetPixelsPtr());
				else
					glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA16I, 0, (int)img->GetSize().x, GL_RGBA_INTEGER, GL_SHORT, img->GetPixelsPtr());

				//if (texture->GetSettings().IsMipMapped())
				//	glGenerateMipmap(GL_TEXTURE_1D);

				glBindTexture(GL_TEXTURE_1D, 0);
			}

			LogOpenGLErrors();
		}

		void OpenGLFactory::UpdateTexture2D(const cgc::strong_ptr<Texture2D>& texture, const cgc::strong_ptr<Image>& image, const glm::uvec2& offset)
		{
			std::thread::id thisThreadID = std::this_thread::get_id();
			if (!(thisThreadID == threadID || thisThreadID == coUploaderThreadID))
			{
				Texture2DUpdateQueue.push(new Recipe::Texture2DUpdate(texture, image, offset));
				coUploadThreadWait.notify_one();

				return;
			}

			Image* img = image.ptr();
			if (img && img->LoadFileIfNotLoaded())
			{
				glBindTexture(GL_TEXTURE_2D, texture->GetID());
				//if (image->GetSize().x * image->GetSize().y <= texture->GetSettings().size.x * texture->GetSettings().size.y)
					glTexSubImage2D(GL_TEXTURE_2D, 0, (int)offset.x, (int)offset.y, (int)img->GetSize().x, (int)img->GetSize().y, (GLenum)image->GetFormat(), (GLenum)image->GetInternalType(), img->GetPixelsPtr());
				/*else
					glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)texture->GetSettings().format, 0, (int)img->GetSize().x, (int)img->GetSize().y, (GLenum)image->GetFormat(), (GLenum)image->GetInternalType(), img->GetPixelsPtr());*/

				if (texture->GetSettings().IsMipMapped())
					glGenerateMipmap(GL_TEXTURE_2D);

				glBindTexture(GL_TEXTURE_2D, 0);
			}

			LogOpenGLErrors();
		}

		cgc::strong_ptr<TextureCube> OpenGLFactory::LoadTextureCube(const ConstructSettings::TextureCube& settings, const std::string& name, const std::vector<cgc::strong_ptr<Image>>& images)
		{

			// Texture already loaded ?
			auto foundTexture = texturesCubes.find(RT_HASH(name));
			if (foundTexture != texturesCubes.end())
				return foundTexture->second;

			// Will be used in the queue
			cgc::strong_ptr<TextureCube> texture = texturesCubes.emplace(RT_HASH(name), settings, defaultTextureId, name, images);
			ThreadSafeCreateTextureCube(Recipe::TextureCube(texture, settings, images));

			return texture;
		}

		cgc::strong_ptr<RenderObject> OpenGLFactory::LoadRenderObject(const cgc::strong_ptr<AbstractMesh>& mesh, const cgc::strong_ptr<Material>& material, UPLOAD_TYPE uploadType, TRIANGLE_TYPE triangleType)
		{
			cgc::strong_ptr<OpenGLRenderObject> renderObject = renderObjects.emplace(0, mesh, cgc::static_pointer_cast<OpenGLMaterial>(material));
			ThreadSafeCreateRenderObject(new Recipe::RenderObject(renderObject, mesh->GetMeshData(), uploadType, triangleType));

			return renderObject;
		}

		cgc::strong_ptr<RenderObject> OpenGLFactory::LoadRenderObject(const cgc::strong_ptr<RenderObject>& copy)
		{
			cgc::strong_ptr<RenderObject> renderObject = renderObjects.emplace(*static_cast<OpenGLRenderObject*>(copy.ptr()));
			return renderObject;
		}

		void OpenGLFactory::UpdateMesh(const cgc::strong_ptr<AbstractMesh>& mesh, const cgc::strong_ptr<IMeshData>& meshData)
		{
			std::thread::id thisThreadID = std::this_thread::get_id();
			if (!(thisThreadID == threadID/* ||thisThreadID == coUploaderThreadID */))
			{
				meshUpdateQueue.push(new Recipe::MeshUpdate(mesh, meshData));
				coUploadThreadWait.notify_one();

				return;
			}

			if (mesh->version == meshData->version)
			{
				mm::array_view indices = meshData->GetIndexMemInfo();
				mm::array_view vertices = meshData->GetVertexMemInfo();
								
				if (!indices.empty() && mesh->GetEBO() &&
					!vertices.empty() && mesh->GetVBO())
				{
					EBO* ebo = static_cast<EBO*>(mesh->GetEBO().ptr());
					VBO* vbo = static_cast<VBO*>(mesh->GetVBO().ptr());

					uint indexBufferId = ebo->id;
					uint vertexBufferId = vbo->id;

					if(!swapBuffers.pop(indexBufferId))
						glGenBuffers(1, &indexBufferId);

					if (!swapBuffers.pop(vertexBufferId))
						glGenBuffers(1, &vertexBufferId);

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
					glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);

					glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.type_size() * indices.size(), nullptr, GL_DYNAMIC_DRAW);
					glBufferData(GL_ARRAY_BUFFER, vertices.type_size() * vertices.size(), nullptr, GL_DYNAMIC_DRAW);

					RemoveAllocatedMemory(ebo->bufferSize);
					glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.type_size() * indices.size(), indices.data());
					ebo->bufferSize = indices.type_size() * indices.size();
					AddAllocatedMemory(indices.type_size() * indices.size());

					RemoveAllocatedMemory(vbo->bufferSize);
					glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.type_size() * vertices.size(), vertices.data());
					vbo->bufferSize = vertices.type_size() * vertices.size();
					AddAllocatedMemory(vertices.type_size() * vertices.size());

					std::swap(ebo->id, indexBufferId);
					std::swap(vbo->id, vertexBufferId);

					swapBuffers.push(indexBufferId);
					swapBuffers.push(vertexBufferId);

					//updateEBOId.push(std::pair<cgc::strong_ptr<IEBO>, uint>(mesh->GetEBO(), indexBufferId));
					//updateVBOId.push(std::pair<cgc::strong_ptr<IVBO>, uint>(mesh->GetVBO(), vertexBufferId));

					/*std::swap(static_cast<EBO*>(.ptr())->id, indexBufferId);
					std::swap(static_cast<VBO*>(mesh->GetVBO().ptr())->id, vertexBufferId);

					swapBuffers.push(indexBufferId);
					swapBuffers.push(vertexBufferId);*/

					//indexBuffer = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, indices.objSize * indices.count, GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);
					//vertexBuffer = glMapBufferRange(GL_ARRAY_BUFFER, 0, vertices.objSize * vertices.count, GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);

					//memcpy(indexBuffer, indices.pointer, indices.objSize * indices.count);
					//memcpy(vertexBuffer, vertices.pointer, vertices.objSize * vertices.count);

					//glFlushMappedBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, indices.objSize * indices.count);
					//glFlushMappedBufferRange(GL_ARRAY_BUFFER, 0, vertices.objSize * vertices.count);

					static_cast<EBO*>(mesh->GetEBO().ptr())->indices = indices.size();
					static_cast<VBO*>(mesh->GetVBO().ptr())->vertexCount = vertices.size();

					//glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
					//glUnmapBuffer(GL_ARRAY_BUFFER);
				}

				/*
				// create and lock both buffers
				if (updateIndices)
				{
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->GetEBO()->GetID());
					//if (static_cast<EBO*>(mesh->GetEBO().ptr())->bufferSize < indices.objSize * indices.count)
					{
						RemoveAllocatedMemory(static_cast<EBO*>(mesh->GetEBO().ptr())->bufferSize);
						glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.objSize * indices.count, nullptr, GL_DYNAMIC_DRAW);
						static_cast<EBO*>(mesh->GetEBO().ptr())->bufferSize = indices.objSize * indices.count;
						AddAllocatedMemory(indices.objSize * indices.count);
					}
					//indexBuffer = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
				}

				if (updateVertices)
				{
					glBindBuffer(GL_ARRAY_BUFFER, mesh->GetVBO()->GetID());
					//if (static_cast<VBO*>(mesh->GetVBO().ptr())->bufferSize < vertices.objSize * vertices.count)
					{
						RemoveAllocatedMemory(static_cast<VBO*>(mesh->GetVBO().ptr())->bufferSize);
						glBufferData(GL_ARRAY_BUFFER, vertices.objSize * vertices.count, nullptr, GL_DYNAMIC_DRAW);
						static_cast<VBO*>(mesh->GetVBO().ptr())->bufferSize = vertices.objSize * vertices.count;
						AddAllocatedMemory(vertices.objSize * vertices.count);
					}
					//vertexBuffer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
				}

				// upload new data to new and locked buffers
				if (updateIndices)
					glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.objSize * indices.count, indices.pointer);
					//memcpy(indexBuffer, indices.pointer, std::min(indices.objSize * indices.count, static_cast<EBO*>(mesh->GetEBO().ptr())->bufferSize));

				if (updateVertices)
					glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.objSize * vertices.count, vertices.pointer);
					//memcpy(vertexBuffer, vertices.pointer, std::min(vertices.objSize * vertices.count, static_cast<VBO*>(mesh->GetVBO().ptr())->bufferSize));
				
				// unloack new buffers
				if (updateIndices)
				{
					//glUnmapBuffer(GL_ARRAY_BUFFER);
					static_cast<EBO*>(mesh->GetEBO().ptr())->indices = indices.count;
				}

				if (updateVertices)
				{
					//glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
					static_cast<VBO*>(mesh->GetVBO().ptr())->vertexCount = vertices.count;
				}*/
			}
		}

		LightRenderData OpenGLFactory::LoadLight()
		{
			for (size_t i = 0; i < lightsData.size(); ++i)
			{
				LightData* light = &lightsData[i];
				if (light->type == LightData::NONE_REUSE)
					return { light, &lightMatrices[i], i };
			}

			uint32_t lightIndex = lightsData.size();
			LightData* lightData = &lightsData.emplace_back();
			glm::mat4* lightMatrix = &lightMatrices.emplace_back();

			return { lightData, lightMatrix, lightIndex };
		}

		cgc::strong_ptr<BoneMatrices> OpenGLFactory::LoadBoneMatrices(uint matrixCount)
		{
			// check which list/vector/array we will fill with this data
			// TODO: allow multiple series in one vector/array to be able to upload all data at once
			boneMatricesList.emplace_back();
			auto& matrices = boneMatricesList.back();
			matrices.resize(matrixCount, OpenGLBoneMatrices::value_type(1.f));

			// get offset where our matrices will start
			uint position = 0; // matrices->size();

			// add
			cgc::strong_ptr<OpenGLBoneMatrices> boneMatrixData = boneMatrixObjects.emplace(matrices.data(), position, matrixCount, nullptr);

			ThreadSafeCreateBoneMatrices(Recipe::BoneMatrices(boneMatrixData));

			return boneMatrixData;
		}

		cgc::strong_ptr<VAO> OpenGLFactory::LoadVAO()
		{
			uint id;
			glGenVertexArrays(1, &id);
			glBindVertexArray(id);

			return vaos.emplace(id, id);
		}

		cgc::strong_ptr<VBO> OpenGLFactory::LoadVBO(const mm::array_view& vertices, const GLenum drawType)
		{
			uint id;
			glGenBuffers(1, &id);
			glBindBuffer(GL_ARRAY_BUFFER, id);
			glBufferData(GL_ARRAY_BUFFER, vertices.type_size() * vertices.size(), vertices.data(), drawType);
			AddAllocatedMemory(vertices.type_size() * vertices.size());
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			return vbos.emplace(id, id, vertices.type_size() * vertices.size(), vertices.size());
		}

		cgc::strong_ptr<EBO> OpenGLFactory::LoadEBO(const mm::array_view& indices, const GLenum bufferType, TRIANGLE_TYPE triangleType)
		{
			uint id;
			glGenBuffers(1, &id);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.type_size() * indices.size(), indices.data(), bufferType);
			AddAllocatedMemory(indices.type_size() * indices.size());
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			// calculate to the proper version
#if GL_UNSIGNED_BYTE + 2 == GL_UNSIGNED_SHORT && GL_UNSIGNED_SHORT + 2 == GL_UNSIGNED_INT
			uint indexType = (GL_UNSIGNED_BYTE - 2) + (bs_rtol(indices.type_size()) + 1) * 2;
#else
			uint indexType;
			switch (indices.objSize) // nr of bytes
			{
			case 1:
				indexType = GL_UNSIGNED_BYTE;
				break;
			case 2:
				indexType = GL_UNSIGNED_SHORT;
				break;
			default:
				indexType = GL_UNSIGNED_INT;
				break;
			}
#endif			

			return ebos.emplace(id, id, indices.type_size() * indices.size(), indices.size(), indexType, triangleType);
		}

		cgc::strong_ptr<IUBO> OpenGLFactory::LoadUBO(void* data, int bufferSize, std::string_view bindingName, UPLOAD_TYPE usageType)
		{
			uint bufferType;
			switch (usageType)
			{
			case UPLOAD_TYPE::DYNAMIC:
				bufferType = GL_DYNAMIC_DRAW;
				break;
			case UPLOAD_TYPE::STREAM:
				bufferType = GL_STREAM_DRAW;
				break;
			default:
				bufferType = GL_STATIC_DRAW;
				break;
			}

			uint id;
			glGenBuffers(1, &id);
			glBindBuffer(GL_UNIFORM_BUFFER, id);
			if (bufferSize > 0)
			{
				glBufferData(GL_UNIFORM_BUFFER, bufferSize, data, bufferType);
				AddAllocatedMemory(bufferSize);
			}
			glBindBuffer(GL_UNIFORM_BUFFER, 0);


			hash_t hash = RT_HASH(bindingName);
			auto emplaced = uboIndices.try_emplace(hash, uboIndices.size());
			glBindBufferBase(GL_UNIFORM_BUFFER, emplaced.first->second, id);

			return ubos.emplace(id, emplaced.first->second, bindingName, bufferSize, bufferType);
		}

		cgc::strong_ptr<FBO> OpenGLFactory::LoadFBO(const std::string& name)
		{
			uint id;
			glGenFramebuffers(1, &id);
			glBindFramebuffer(GL_FRAMEBUFFER, id);

			return fbos.emplace(RT_HASH(name), id, name);
		}

		cgc::strong_ptr<RBO> OpenGLFactory::LoadRBO()
		{
			return cgc::strong_ptr<RBO>();
		}

		void OpenGLFactory::HandleQueue()
		{
			// Trash
			shaders.clean_garbage();
			subShaders.clean_garbage();
			texture2Ds.clean_garbage();
			texture3Ds.clean_garbage();
			texturesCubes.clean_garbage();
			vaos.clean_garbage();
			vbos.clean_garbage();
			ebos.clean_garbage();
			ubos.clean_garbage();
			fbos.clean_garbage();
			rbos.clean_garbage();

			//LogOpenGLErrors();

			// Recipes
			Recipe::RenderObject* renderObjectRecipe;
			while (renderObjectsRecipesQueue.pop(renderObjectRecipe))
			{
				ThreadSafeCreateRenderObject(renderObjectRecipe);
			}


			std::queue<AbstractMesh*> cleanMeshUpdateData;

			Recipe::MeshUpdate* renderUpdateObjectRecipe;
			while (meshUpdateQueue.pop(renderUpdateObjectRecipe))
			{
				cgc::strong_ptr<AbstractMesh>& mesh = renderUpdateObjectRecipe->mesh;

				UpdateMesh(mesh, renderUpdateObjectRecipe->meshData);
				if (!mesh->KeepOnCPU())
					cleanMeshUpdateData.push(mesh.ptr());

				delete renderUpdateObjectRecipe;
			}

			while (!cleanMeshUpdateData.empty())
			{
				AbstractMesh* mesh = cleanMeshUpdateData.front();
				cleanMeshUpdateData.pop();
				mesh->UnloadFromCPU();
			}

			std::pair<cgc::strong_ptr<IEBO>, uint> eboUpdate;
			while (updateEBOId.pop(eboUpdate))
			{
				std::swap(static_cast<EBO*>(eboUpdate.first.ptr())->id, eboUpdate.second);
				swapBuffers.push(eboUpdate.second);
			}

			std::pair<cgc::strong_ptr<IVBO>, uint> vboUpdate;
			while (updateVBOId.pop(vboUpdate))
			{
				std::swap(static_cast<VBO*>(vboUpdate.first.ptr())->id, vboUpdate.second);
				swapBuffers.push(vboUpdate.second);
			}
		}

		void OpenGLFactory::CoHandleQueue()
		{
			coUploaderThreadID = std::this_thread::get_id();
			coUploaderContext.setActive(true);
			glewInit();


			while (coUploaderRunning)
			{
				{
					std::unique_lock<std::mutex> lock(coUploadThreadLock);
					coUploadThreadWait.wait(lock);
				}

				while (true)
				{
					// Recipes
					Recipe::BoneMatrices boneMatricesRecipe;
					while (boneMatricesRecipesQueue.pop_front(boneMatricesRecipe))
						ThreadSafeCreateBoneMatrices(boneMatricesRecipe);

					Recipe::Texture2D textureRecipe2D;
					while (textures2DRecipesQueue.pop_front(textureRecipe2D))
						ThreadSafeCreateTexture2D(textureRecipe2D);

					Recipe::TextureCube textureRecipeCube;
					while (texturesCubeRecipesQueue.pop_front(textureRecipeCube))
						ThreadSafeCreateTextureCube(textureRecipeCube);

					Recipe::Shader shaderRecipe;
					while (shadersRecipesQueue.pop_front(shaderRecipe))
						ThreadSafeCreateShader(shaderRecipe);

					Recipe::Buffer bufferRecipe;
					while (bufferRecipesQueue.pop_front(bufferRecipe))
						ThreadSafeCreateBuffer(bufferRecipe);

					Recipe::Texture2DUpdate* texture2DUpdateRecipe;
					while (Texture2DUpdateQueue.pop(texture2DUpdateRecipe))
					{
						UpdateTexture2D(texture2DUpdateRecipe->texture, texture2DUpdateRecipe->image, texture2DUpdateRecipe->offset);
						// TODO remove image data if it's marked

						delete texture2DUpdateRecipe;
					}

					Recipe::BufferUpdate* bufferUpdateRecipe;
					while (bufferUpdateQueue.pop(bufferUpdateRecipe))
					{
						UpdateBuffer(bufferUpdateRecipe->buffer, bufferUpdateRecipe->data, bufferUpdateRecipe->offset, bufferUpdateRecipe->size, bufferUpdateRecipe->enlarge);

						delete bufferUpdateRecipe;
					}

					// make sure we upload everything before we ditch any data OpenGL might still need
					glFlush();

					// stop looping when there's no more data
					if (boneMatricesRecipesQueue.empty()
						&& textures2DRecipesQueue.empty()
						&& texturesCubeRecipesQueue.empty()
						&& shadersRecipesQueue.empty())
						break;
				}
			}
		}

		void OpenGLFactory::ThreadSafeCreateRenderObject(Recipe::RenderObject* renderObjectRecipe)
		{
			std::thread::id thisThreadID = std::this_thread::get_id();
			if (thisThreadID != threadID && thisThreadID != coUploaderThreadID)
			{
				renderObjectsRecipesQueue.push(renderObjectRecipe);
				return;
			}

			OpenGLRenderObject* renderObject = renderObjectRecipe->renderObject.ptr();
			AbstractMesh* mesh = renderObject->mesh.ptr();
			const cgc::strong_ptr<IMeshData>& meshData = renderObjectRecipe->meshData;

			mm::array_view vertexMemInfo = meshData->GetVertexMemInfo();
			uint bufferType;
			switch (renderObjectRecipe->uploadType)
			{
			case UPLOAD_TYPE::DYNAMIC:
				bufferType = GL_DYNAMIC_DRAW;
				break;
			case UPLOAD_TYPE::STREAM:
				bufferType = GL_STREAM_DRAW;
				break;
			default:
				bufferType = GL_STATIC_DRAW;
				break;
			}

			cgc::strong_ptr<VBO> vbo = cgc::static_pointer_cast<VBO>(mesh->GetVBO());
			if (vbo == nullptr)
			{
				vbo = LoadVBO(vertexMemInfo, bufferType);

				// TODO: look for a different way to solve this thread safe shader loading when shader is not loaded when renderobject is being loaded
				cgc::strong_ptr<OpenGLShader> openGLShader = static_cast<OpenGLMaterial*>(renderObject->material.ptr())->GetOpenGLShader();
				/*while(!openGLShader->initialized)
				{
					Recipe::Shader shaderRecipe = shadersRecipesQueue.front();
					shadersRecipesQueue.pop_front();
					ThreadSafeCreateShader(shaderRecipe);
				}*/

				vbo->SetShader(openGLShader);
				renderObject->mesh->SetVBO(vbo);
			}

			cgc::strong_ptr<EBO> ebo;
			mm::array_view indexMemInfo = meshData->GetIndexMemInfo();
			//if (!indexMemInfo.empty())
			ebo = LoadEBO(indexMemInfo, bufferType, renderObjectRecipe->triangleType);

			renderObject->vao = LoadVAO();
			cgc::strong_ptr<VAO>& vao = renderObject->vao;
			vao->AttachVBO(vbo);
			if (ebo)
				vao->AttachEBO(ebo);

			renderObject->mesh->SetEBO(ebo);

			renderObject->initialized = true;

			glFlush();

			if (!mesh->KeepOnCPU())
				mesh->UnloadFromCPU();

			delete renderObjectRecipe;
		}

		void OpenGLFactory::ThreadSafeCreateBuffer(const Recipe::Buffer& bufferRecipe)
		{
			std::thread::id thisThreadID = std::this_thread::get_id();
			if (!(thisThreadID == threadID || thisThreadID == coUploaderThreadID))
			{
				bufferRecipesQueue.push_back(bufferRecipe);
				coUploadThreadWait.notify_one();

				return;
			}

			const ConstructSettings::Buffer& settings = bufferRecipe.settings;
			cgc::strong_ptr<Buffer> buffer = bufferRecipe.buffer;

			// buffer
			uint bufferId;
			glGenBuffers(1, &bufferId);
			glBindBuffer(GL_TEXTURE_BUFFER, bufferId);
			glBufferData(GL_TEXTURE_BUFFER, bufferRecipe.size, bufferRecipe.data, GL_DYNAMIC_DRAW);
			AddAllocatedMemory(bufferRecipe.size);
			glBindBuffer(GL_TEXTURE_BUFFER, 0);

			// texture wrapper
			uint textureId;
			glGenTextures(1, &textureId);
			glBindTexture(GL_TEXTURE_BUFFER, textureId);
			glTexBuffer(GL_TEXTURE_BUFFER, (int)settings.format, bufferId);
			glBindTexture(GL_TEXTURE_BUFFER, 0);

			const_cast<Buffer::Settings&>(buffer->GetSettings()).size = bufferRecipe.offset + bufferRecipe.size;
			AlterBuffer(buffer, textureId, bufferId, &settings);

			LogOpenGLErrors();
		}

		void OpenGLFactory::ThreadSafeCreateTexture1D(const Recipe::Texture1D& textureRecipe)
		{
			std::thread::id thisThreadID = std::this_thread::get_id();
			if (!(thisThreadID == threadID || thisThreadID == coUploaderThreadID))
			{
				//if (std::find(textures2DRecipesQueue.begin(), textures2DRecipesQueue.end(), textureRecipe) == textures2DRecipesQueue.end())
				textures1DRecipesQueue.push_back(textureRecipe);
				coUploadThreadWait.notify_one();

				return;
			}

			const ConstructSettings::Texture1D& settings = textureRecipe.settings;

			cgc::strong_ptr<Texture1D> texture = textureRecipe.texture;
			int textureInterpolation = (int)settings.textureFilter; // GL_NEAREST; // GL_LINEAR;
			int mipMapInterpolation = (int)settings.mipMapFilter; // GL_NEAREST_MIPMAP_LINEAR; // GL_NEAREST_MIPMAP_NEAREST; // GL_LINEAR_MIPMAP_LINEAR;

			uint id;
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_1D, id);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, textureInterpolation);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, settings.mipmapped ? mipMapInterpolation : textureInterpolation);

			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);

			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, (int)settings.repeat);
			glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, (int)settings.repeat);
			glTexParameterfv(GL_TEXTURE_1D, GL_TEXTURE_BORDER_COLOR, &textureRecipe.settings.borderColor[0]);

			cgc::raw_ptr<Image> image = textureRecipe.image;
			if (image && image->LoadFileIfNotLoaded())
			{
				glPixelStorei(GL_UNPACK_ALIGNMENT, image->GetStride());

				uint imageSize = settings.size == 0 ? image->GetSize().x : settings.size;
				glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA16I, imageSize, 0, GL_RGBA_INTEGER, GL_SHORT, image->GetPixelsPtr());

				// Create mip maps
				if (settings.mipmapped)
					glGenerateMipmap(GL_TEXTURE_1D);

				// Adjust the image
				AlterTexture1D(texture, id, texture->GetPath(), &settings, settings.keepImageOnCPU ? textureRecipe.image : cgc::strong_ptr<Image>());
			}
			else
			{
				glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA16I, settings.size, 0, GL_RGBA_INTEGER, GL_SHORT, nullptr);
				AlterTexture1D(texture, id, texture->GetPath(), &settings, cgc::strong_ptr<Image>());
			}

			glBindTexture(GL_TEXTURE_1D, 0);

			LogOpenGLErrors();
		}

		void OpenGLFactory::ThreadSafeCreateTexture2D(const Recipe::Texture2D& textureRecipe)
		{
			std::thread::id thisThreadID = std::this_thread::get_id();
			if (!(thisThreadID == threadID || thisThreadID == coUploaderThreadID))
			{
				//if (std::find(textures2DRecipesQueue.begin(), textures2DRecipesQueue.end(), textureRecipe) == textures2DRecipesQueue.end())
				textures2DRecipesQueue.push_back(textureRecipe);
				coUploadThreadWait.notify_one();

				return;
			}

			const ConstructSettings::Texture2D& settings = textureRecipe.settings;

			cgc::strong_ptr<Texture2D> texture = textureRecipe.texture;
			int textureInterpolation = (int)settings.textureFilter; // GL_NEAREST; // GL_LINEAR;
			int mipMapInterpolation = (int)settings.mipMapFilter; // GL_NEAREST_MIPMAP_LINEAR; // GL_NEAREST_MIPMAP_NEAREST; // GL_LINEAR_MIPMAP_LINEAR;

			uint id;
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureInterpolation);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, settings.mipmapped ? mipMapInterpolation : textureInterpolation);

			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (int)settings.repeat);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (int)settings.repeat);
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &textureRecipe.settings.borderColor[0]);

			cgc::raw_ptr<Image> image = textureRecipe.image;
			if (image && image->LoadFileIfNotLoaded())
			{
				glPixelStorei(GL_UNPACK_ALIGNMENT, image->GetStride());


				if (settings.size.x == 0)
					const_cast<glm::uvec2&>(settings.size) = image->GetSize();

				const glm::ivec2& imageSize = settings.size;

				glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)settings.format, imageSize.x, imageSize.y, 0, (GLenum)image->GetFormat(), (GLenum)image->GetInternalType(), image->GetPixelsPtr());

				// Create mip maps
				if (settings.mipmapped)
					glGenerateMipmap(GL_TEXTURE_2D);

				// Adjust the image
				AlterTexture2D(texture, id, texture->GetPath(), &settings, settings.keepImageOnCPU ? textureRecipe.image : cgc::strong_ptr<Image>());
			}
			else
			{
				GLenum uploadFormat = GL_RGBA;
				switch ((GLenum)settings.format)
				{
				case GL_DEPTH_COMPONENT:
				case GL_DEPTH_COMPONENT16:
				case GL_DEPTH_COMPONENT24:
				case GL_DEPTH_COMPONENT32F:
					uploadFormat = GL_DEPTH_COMPONENT;
					break;
				case GL_DEPTH_STENCIL:
				case GL_DEPTH24_STENCIL8:
				case GL_DEPTH32F_STENCIL8:
					uploadFormat = GL_DEPTH_STENCIL;
					break;
				}

				glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)settings.format, settings.size.x, settings.size.y, 0, uploadFormat, GL_UNSIGNED_BYTE, nullptr);
				AlterTexture2D(texture, id, texture->GetPath(), &settings, cgc::strong_ptr<Image>());
			}

			glBindTexture(GL_TEXTURE_2D, 0);

			LogOpenGLErrors();
		}

		void OpenGLFactory::ThreadSafeCreateTextureCube(const Recipe::TextureCube& textureRecipe)
		{
			std::thread::id thisThreadID = std::this_thread::get_id();
			if (!(thisThreadID == threadID || thisThreadID == coUploaderThreadID))
			{
				texturesCubeRecipesQueue.push_back(textureRecipe);
				coUploadThreadWait.notify_one();
				return;
			}

			cgc::strong_ptr<TextureCube> texture = textureRecipe.texture;
			glm::uvec2 size;

			uint id;
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_CUBE_MAP, id);

			for (GLuint i = 0; i < textureRecipe.images.size(); ++i)
			{
				// TODO: let the data factory handle this
				Esteem::Image* img = textureRecipe.images[i].ptr();
				if (img && img->LoadFileIfNotLoaded())
				{
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, img->GetSize().x, img->GetSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->GetPixelsPtr());
					size = img->GetSize();
				}
			}

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

			// Adjust the image
			AlterTextureCube(texture, id, size);
		}

		void OpenGLFactory::ThreadSafeCreateShader(Recipe::Shader& shaderRecipe)
		{
			std::thread::id thisThreadID = std::this_thread::get_id();
			if (!(thisThreadID == threadID || thisThreadID == coUploaderThreadID))
			{
				//if (std::find(shadersRecipesQueue.begin(), shadersRecipesQueue.end(), shaderRecipe) == shadersRecipesQueue.end())
				shadersRecipesQueue.push_back(shaderRecipe);
				coUploadThreadWait.notify_one();

				return;
			}

			cgc::strong_ptr<OpenGLShader> shader = shaderRecipe.shader;
			LogOpenGLErrors();

			std::vector<cgc::strong_ptr<OpenGLSubShader>> subShaders;
			for (uint i = 0; i < shaderRecipe.paths.size() && i < shaderRecipe.types.size(); ++i)
			{
				if (shaderRecipe.types[i] != NULL)
				{
					std::string path = shaderRecipe.paths[i];
					cgc::strong_ptr<OpenGLSubShader> subshader = LoadSubShader(path, shaderRecipe.types[i]);
					LogOpenGLErrors();
					if (subshader != nullptr)
						subShaders.push_back(subshader);
				}
			}

			if (subShaders.size() == 0)
			{
				std::string shaderPaths = "";
				for (uint i = 0; i < shaderRecipe.paths.size(); ++i)
				{
					std::string path = shaderRecipe.paths[i];
					shaderPaths += " - " + path + (i < shaderRecipe.paths.size() - 1 ? "\n" : "");
				}

				Debug::LogError("Couldn't create shader with input shaders:\n", shaderPaths);
			}

			// check if we are using cutoff shaders
			for (uint i = 0; i < subShaders.size(); ++i)
			{
				if (subShaders[i]->GetCutoff() > 0.001f)
					shader->cutoff = true;
			}

			shader->shaders = std::move(subShaders);

			// Create new shader
			uint id = glCreateProgram();
			for (uint i = 0; i < shader->shaders.size(); ++i)
				glAttachShader(id, shader->shaders[i]->GetID());
			LogOpenGLErrors();

			glLinkProgram(id);

			int success = 0;
			glGetProgramiv(id, GL_LINK_STATUS, &success);
			if (success == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetProgramiv(id, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::string infoLog;
				infoLog.resize(maxLength);
				glGetProgramInfoLog(id, maxLength, &maxLength, infoLog.data());
				Debug::LogError(infoLog);

				// We don't need the program anymore.
				glDeleteProgram(id);
				id = 0;
			}
			LogOpenGLErrors();

			// clean up old sub shaders
			for (uint i = 0; i < shader->shaders.size(); ++i)
			{
				glDetachShader(id, shader->shaders[i]->GetID());
				glDeleteShader(shader->shaders[i]->GetID());
			}

			LogOpenGLErrors();

			shader->id = id;
			shader->LoadAllAttributesAndUniforms();

			for (const auto& uniform : shader->GetAllUniformVariables())
			{
				if (uniform.second.type == GL_UNIFORM_BLOCK_BINDING)
				{
					auto emplaced = uboIndices.try_emplace(uniform.first, uboIndices.size());
					glUniformBlockBinding(shader->GetShaderID(), uniform.second.index, emplaced.first->second);
				}
			}


			/*for (auto it = ubos.begin(); it != ubos.end(); it++)
				UBO::Bind(shader, it->first, it->second->bindingIndex);*/

			while (shaderRecipe.materials.size() > 0)
			{
				OpenGLMaterial* material = shaderRecipe.materials.front().ptr();
				InitializeMaterialUniforms(material);
				shaderRecipe.materials.pop();
			}

			shader->initialized = success == GL_TRUE;

			LogOpenGLErrors();
		}

		void OpenGLFactory::ThreadSafeCreateBoneMatrices(const Recipe::BoneMatrices& boneMatricesRecipe)
		{
			std::thread::id thisThreadID = std::this_thread::get_id();
			if (!(thisThreadID == threadID || thisThreadID == coUploaderThreadID))
			{
				//if (std::find(shadersRecipesQueue.begin(), shadersRecipesQueue.end(), shaderRecipe) == shadersRecipesQueue.end())
				boneMatricesRecipesQueue.push_back(boneMatricesRecipe);
				coUploadThreadWait.notify_one();

				return;
			}

			cgc::strong_ptr<UBO> ubo = cgc::static_pointer_cast<UBO>(LoadUBO(
				&(*boneMatricesRecipe.boneMatrices->matrices)[boneMatricesRecipe.boneMatrices->index],
				boneMatricesRecipe.boneMatrices->size * sizeof(BoneMatrices::value_type), "boneMatricesUB", UPLOAD_TYPE::DYNAMIC));

			const_cast<cgc::strong_ptr<UBO>&>(boneMatricesRecipe.boneMatrices->ubo) = ubo;
		}
		
		void OpenGLFactory::AddAllocatedMemory(std::size_t append)
		{
			allocatedMemory += append;
			//std::cout << allocatedMemory << " (+" << append << ")" << std::endl;
		}

		void OpenGLFactory::RemoveAllocatedMemory(std::size_t subtract)
		{
			allocatedMemory -= subtract;
			//std::cout << allocatedMemory << " (-" << subtract << ")" << std::endl;
		}
}
}
