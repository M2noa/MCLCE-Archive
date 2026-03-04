#pragma once

#include <map>
#include <vector>
#include <string>
//#include "rapidjson/Document.h"
#include "General/IFactory.h"
#include "Rendering/RenderingFactory.h"
#include "Model/ModelFactory.h"
//#include "World/WorldFactory.h"

#include <iostream>
#include <fstream>
#include "rapidjson/document.h"

namespace Esteem
{

	const std::string RESOURCES_PATH = "./resources/";
	const std::string TEXTURES_PATH = "textures/";
	const std::string MATERIALS_PATH = "materials/";
	const std::string SHADERS_PATH = "shaders/";
	const std::string MODELS_PATH = "models/";
	const std::string WORLD_PARTS_PATH = "world/";

	enum ListDirectoryOptions
	{
		FILES		= 0b1,
		DIRECTORIES	= 0b10,

		LIST_ALL	= 0b11
	};

	class Data
	{
	private:

		static std::vector<IFactory*> factories;
		static ModelFactory modelFactory;
		//static WorldFactory worldFactory;
		static std::vector<RenderingFactory*> renderingFactories;

		static bool destructing;

	public:
		static const std::string ROOT_PATH;

		/// \brief Initialize Data utility
		static void Initialize();

		static void Destruct();

		/// \brief Initialize all default required factories
		static void InitializeDefaultFactories();

	#pragma region Default OS operations
		// TODO: return file oject
		static bool FileExists(std::string_view path);
		static bool AssetExists(std::string_view path);
		static void LoadFile(std::string_view path);

		static std::string ReadAsset(std::string_view path);
		static cgc::strong_ptr<std::istream> StreamAsset(std::string_view path, size_t& size);
		static bool WriteFile(std::string_view path, const char* content, uint size);

		static std::vector<std::string> ListDirectory(std::string_view path, ListDirectoryOptions listOptions = ListDirectoryOptions::LIST_ALL);

		static rapidjson::Document ReadJSONFile(std::string_view path);
		static rapidjson::Document ReadJSON(const char* path);

		static void WriteJSON();
	#pragma endregion

	#pragma region Factories
		static void Register(IFactory* factory);
		static void Register(RenderingFactory* factory);

		static void UnRegister(IFactory* factory);
		static void UnRegister(RenderingFactory* factory);

		/// \brief Heavy operation to find the right factory, store a reference/pointer to this factory when you retrieved it.
		static IFactory* FindFactory(const char* typeName, const std::string& search);

		static RenderingFactory* GetRenderingFactory(uint index = 0);
		static ModelFactory& GetModelFactory();
		//static WorldFactory& GetWorldFactory();
	#pragma endregion

	#pragma region Cipboard
		static std::string GetStringFromClipBoard();
		static bool SetStringFromClipBoard(const std::string& text);
	#pragma endregion

	};
}