#include "Data.h"

#include <algorithm>
#include <filesystem>

#include "details/izafstream.h"
#include "Utils/Debug.h"
#include <sys/stat.h>

#ifdef _WIN32
#	include <windows.h>
#else
#	include <stdlib.h>
#endif

#ifdef __APPLE__
#	include <CoreFoundation/CoreFoundation.h>
#endif

namespace Esteem
{
#ifdef __APPLE__
	inline const std::string GetApplicationRootPath()
	{
		char path[1024];
		CFBundleRef mainBundle = CFBundleGetMainBundle();
		if (!mainBundle)
			return "./";

		CFURLRef mainBundleURL = CFBundleCopyBundleURL(mainBundle);
		if (!mainBundleURL)
			return "./";

		CFStringRef cfStringRef = CFURLCopyFileSystemPath(mainBundleURL, kCFURLPOSIXPathStyle);
		if (!cfStringRef)
			return "./";

		CFStringGetCString(cfStringRef, path, 1024, kCFStringEncodingASCII);

		CFRelease(mainBundleURL);
		CFRelease(cfStringRef);

		std::string strPath;
		int len = strlen(path);
		if (len > 0)
		{
			strPath = std::string(path, len);
			strPath += (strcmp(&path[len - 4], ".app") == 0) ? "/Contents/Resources/" : "/";
		}
		else
			strPath = "./";

		std::cout << strPath << std::endl;

		return strPath;
	}
	const std::string Data::ROOT_PATH = GetApplicationRootPath();
#else
	const std::string Data::ROOT_PATH = "./";
#endif

	bool Data::destructing = false;
	std::vector<IFactory*> Data::factories = std::vector<IFactory*>();
	ModelFactory Data::modelFactory = ModelFactory();
	//WorldFactory Data::worldFactory = WorldFactory();
	std::vector<RenderingFactory*> Data::renderingFactories = std::vector<RenderingFactory*>();

	void Data::Initialize()
	{
		InitializeDefaultFactories();
	}

	void Data::Destruct()
	{
		destructing = true;

		for (auto it = factories.begin(); it != factories.end(); ++it)
			delete* it;
		factories.clear();

		//for (auto it = renderingFactories.begin(); it != renderingFactories.end(); ++it)
		//	delete* it;
		renderingFactories.clear();
	}

	void Data::InitializeDefaultFactories()
	{
		//modelFactory = new ModelFactory();
	}

	bool Data::FileExists(std::string_view path)
	{
		if (path.size() > 0)
		{
#ifdef __APPLE__
			std::string absolutePath;
			if (path[0] == '~')
			{
				char* homePath = std::getenv("HOME");
				if (homePath != nullptr)
				{
					std::size_t homeLength = strlen(homePath);
					absolutePath.resize(homeLength + path.size() - 1);
					memcpy(&absolutePath[0], homePath, homeLength);
					memcpy(&absolutePath[homeLength], &path[1], path.size() - 1);
					path = absolutePath;
				}
				else
				{
					Debug::LogError("Could not find user's home directory");
					return false;
				}
			}

#endif
			std::error_code errorCode;
			return std::filesystem::exists(path.data(), errorCode);
		}

		return false;
	}

	bool Data::AssetExists(std::string_view path)
	{
		if (path.size() > 0)
		{
#ifdef __APPLE__
			std::string absolutePath;
			if (path[0] == '~')
			{
				char* homePath = std::getenv("HOME");
				if (homePath != nullptr)
				{
					std::size_t homeLength = strlen(homePath);
					absolutePath.resize(homeLength + path.size() - 1);
					memcpy(&absolutePath[0], homePath, homeLength);
					memcpy(&absolutePath[homeLength], &path[1], path.size() - 1);
					path = absolutePath;
				}
				else
				{
					Debug::LogError("Could not find user's home directory");
					return false;
				}
			}

#endif
			std::error_code errorCode;
			if (std::filesystem::exists(path.data(), errorCode))
				return true;
			else
			{
				if (Data::FileExists(ROOT_PATH + "data.pak"))
				{
					void* zipFile = unzOpen((ROOT_PATH + "data.pak").c_str());
					if (zipFile != nullptr)
					{
						std::string p(path);
						const char* newPath = p.c_str();
						newPath += newPath[1] == '/' ? 2 : newPath[0] == '/' ? 1 : 0;

						if (unzLocateFile(zipFile, newPath, 2) == UNZ_OK)
						{
							unzClose(zipFile);
							return true;
						}

						unzClose(zipFile);
					}
				}
			}
		}

		return false;
	}

	void Data::LoadFile(std::string_view path)
	{
		return;
	}

	std::string Data::ReadAsset(std::string_view path)
	{
		if (path.size() > 0)
		{
#ifdef __APPLE__
			std::string absolutePath;
			if (path[0] == '~')
			{
				char* homePath = std::getenv("HOME");
				if (homePath != nullptr)
				{
					std::size_t homeLength = strlen(homePath);
					absolutePath.resize(homeLength + path.size() - 1);
					memcpy(&absolutePath[0], homePath, homeLength);
					memcpy(&absolutePath[homeLength], &path[1], path.size() - 1);
					path = absolutePath;
				}
				else
				{
					Debug::LogError("Could not find user's home directory");
					return "";
				}
			}

#endif
			if (Data::FileExists(path))
			{
				size_t fileSize;
				auto ifs = Data::StreamAsset(path, fileSize);
				if (ifs && fileSize)
				{
					ifs->seekg(0, std::ios::beg);

					std::string bytes;
					bytes.resize(fileSize);
					ifs->read(bytes.data(), fileSize);

					return bytes;
				}
			}
			else if (Data::FileExists(ROOT_PATH + "data.pak"))
			{
				void* zipFile = unzOpen((ROOT_PATH + "data.pak").c_str());
				if (zipFile != nullptr)
				{
					std::string p(path);
					const char* newPath = p.c_str();
					newPath += newPath[1] == '/' ? 2 : newPath[0] == '/' ? 1 : 0;

					if (unzLocateFile(zipFile, newPath, 2) == UNZ_OK)
					{
						if (unzOpenCurrentFile(zipFile) == UNZ_OK)
						{
							unz_file_info_s fileInfo;
							unzGetCurrentFileInfo(zipFile, &fileInfo, nullptr, 0, nullptr, 0, nullptr, 0);

							std::string contents;
							contents.resize(fileInfo.uncompressed_size);
							unzReadCurrentFile(zipFile, contents.data(), fileInfo.uncompressed_size);

							unzCloseCurrentFile(zipFile);
							unzClose(zipFile);

							return contents;
						}
					}

					unzClose(zipFile);
				}
			}
		}

		return "";
	}

	cgc::strong_ptr<std::istream> Data::StreamAsset(std::string_view path, size_t& size)
	{
		if (path.size() > 0)
		{
#ifdef __APPLE__
			std::string absolutePath;
			if (path[0] == '~')
			{
				char* homePath = std::getenv("HOME");
				if (homePath != nullptr)
				{
					std::size_t homeLength = strlen(homePath);
					absolutePath.resize(homeLength + path.size());
					memcpy(&absolutePath[0], homePath, homeLength);
					memcpy(&absolutePath[homeLength], path.data(), path.size());
					path = absolutePath;
				}
				else
				{
					Debug::LogError("Could not find user's home directory");
					return cgc::strong_ptr<std::istream>();
				}
			}

#endif
			if (Data::FileExists(path))
			{
				auto stream = cgc::construct_new<std::ifstream>(path.data(), std::ios::ate | std::ios::in | std::ios::binary);
				size = (size_t)stream->tellg();
				stream->seekg(0);
				return stream;
			}
			else if (Data::FileExists(ROOT_PATH + "data.pak"))
			{
				void* zipFile = unzOpen((ROOT_PATH + "data.pak").c_str());
				if (zipFile != nullptr)
				{
					std::string p(path);
					const char* newPath = p.c_str();
					newPath += newPath[1] == '/' ? 2 : newPath[0] == '/' ? 1 : 0;

					if (unzLocateFile(zipFile, newPath, 2) == UNZ_OK
						&& unzOpenCurrentFile(zipFile) == UNZ_OK)
					{
						auto stream = cgc::construct_new<izafstream>(zipFile);
						size = stream->get_uncompressed_size();
						return stream;
					}

					unzClose(zipFile);
				}
			}
		}

		return cgc::strong_ptr<std::istream>();
	}

	bool Data::WriteFile(std::string_view path, const char* content, uint size)
	{
		if (path.size() > 0)
		{
#ifdef __APPLE__
			std::string absolutePath;
			if (path[0] == '~')
			{
				char* homePath = std::getenv("HOME");
				if (homePath != nullptr)
				{
					std::size_t homeLength = strlen(homePath);
					absolutePath.resize(homeLength + path.size() - 1);
					memcpy(&absolutePath[0], homePath, homeLength);
					memcpy(&absolutePath[homeLength], &path[1], path.size() - 1);
					path = absolutePath;
				}
				else
				{
					Debug::LogError("Could not find user's home directory");
					return false;
				}
			}

#endif
			std::error_code errorCode;
			std::string folder = std::string(path.data(), path.find_last_of('/'));
			if (std::filesystem::is_directory(folder, errorCode) || std::filesystem::create_directories(folder, errorCode) || errorCode.value() == 0)
			{
				std::ofstream ofs(path.data(), std::ios::out | std::ios::binary);
				if (ofs.is_open())
				{
					ofs.write(content, size);
					ofs.close();

					return true;
				}
			}
			else
				Debug::LogError("Could not find or create directory \"" + std::string(path) + "\" error: " + errorCode.message() + " (" + std::to_string(errorCode.value()) + ")");
		}
		return false;
	}


	std::vector<std::string> Data::ListDirectory(std::string_view path, ListDirectoryOptions options)
	{
		std::vector<std::string> directories;

		std::error_code errorCode;
		
		if (options & (ListDirectoryOptions::FILES | ListDirectoryOptions::DIRECTORIES))
		{
			for (const auto& entry : std::filesystem::directory_iterator(path, errorCode))
			{
				directories.emplace_back(entry.path().string());
			}
		}
		else if (options & ListDirectoryOptions::FILES)
		{
			for (const auto& entry : std::filesystem::directory_iterator(path, errorCode))
			{
				if (entry.is_regular_file())
					directories.emplace_back(entry.path().string());
			}
		}
		else if (options & ListDirectoryOptions::DIRECTORIES)
		{
			for (const auto& entry : std::filesystem::directory_iterator(path, errorCode))
			{
				if (entry.is_directory())
					directories.emplace_back(entry.path().string());
			}
		}

		if(errorCode)
			Debug::LogWarning("Could not find or create directory \"" + std::string(path) + "\" error: " + errorCode.message() + " (" + std::to_string(errorCode.value()) + ")");

		return directories;
	}

	rapidjson::Document Data::ReadJSONFile(std::string_view path)
	{
		std::string file = ReadAsset(path);
		const char* json = file.c_str();

		return ReadJSON(json);
	}

	rapidjson::Document Data::ReadJSON(const char* json)
	{
		rapidjson::Document doc;
		if (doc.Parse(json).HasParseError())
			Debug::LogError("Couldn't JSON parse: given contents - " + std::to_string((unsigned)doc.GetErrorOffset()) + "\n" + std::string(json).substr(doc.GetErrorOffset(), 20) + "\n^");

		return doc;
	}

	void Data::WriteJSON()
	{
	}

	IFactory* Data::FindFactory(const char* typeName, const std::string& search)
	{
		for (uint i = 0; i < factories.size(); ++i)
		{
			if (factories[i]->EqualsSearch(search))
				return factories[i];
		}

		return nullptr;
	}

	ModelFactory& Data::GetModelFactory()
	{
		return modelFactory;
	}

	RenderingFactory* Data::GetRenderingFactory(uint index)
	{
		if (index < renderingFactories.size() && index >= 0)
			return renderingFactories[index];
		else
			return nullptr;
	}

	void Data::Register(IFactory* factory)
	{
		if (std::find(factories.begin(), factories.end(), factory) == factories.end())
			factories.push_back(factory);
	}

	void Data::Register(RenderingFactory* factory)
	{
		if (std::find(renderingFactories.begin(), renderingFactories.end(), factory) == renderingFactories.end())
			renderingFactories.push_back(factory);
	}

	void Data::UnRegister(IFactory* factory)
	{
		if (!destructing)
		{
			auto found = std::find(factories.begin(), factories.end(), factory);
			if (found != factories.end())
				factories.erase(found);
		}
	}

	void Data::UnRegister(RenderingFactory* factory)
	{
		//if (!destructing)
		//{
		auto found = std::find(renderingFactories.begin(), renderingFactories.end(), factory);
		if (found != renderingFactories.end())
			renderingFactories.erase(found);
		//}
	}

	std::string Data::GetStringFromClipBoard()
	{
#ifdef _WIN32
		std::string text;
		// Try opening the clipboard
		if (OpenClipboard(nullptr))
		{
			// Get handle of clipboard object for ANSI text
			void* hData = GetClipboardData(CF_TEXT);
			if (hData != nullptr)
			{
				// Lock the handle to get the actual text pointer
				char* pszText = static_cast<char*>(GlobalLock(hData));
				if (pszText != nullptr)
					text = std::string(pszText); // Save text in a string class instance

				GlobalUnlock(hData); // Release the lock
			}
		}
		else
			Debug::LogWarning("Could not get access to the clipboard");

		CloseClipboard(); // Release the clipboard
		return text;
#else
		FILE* pipe = popen("pbpaste", "r");
		if (!pipe)
			return "";

		char buffer[128];
		std::string result = "";
		while (!feof(pipe))
		{
			if (fgets(buffer, 128, pipe) != NULL)
			{
				result += buffer;
			}
		}
		pclose(pipe);
		return result;
#endif

	}

	bool Data::SetStringFromClipBoard(const std::string& text)
	{
#ifdef _WIN32
		// Try opening the clipboard
		if (OpenClipboard(nullptr))
		{
			if (EmptyClipboard())
			{
				HGLOBAL hGlob = GlobalAlloc(GMEM_FIXED, text.size() + 1);
				if (hGlob)
				{
					memcpy(static_cast<char*>(hGlob), text.data(), text.size());
					static_cast<char*>(hGlob)[text.size()] = '\0';

					// Get handle of clipboard object for ANSI text
					if (SetClipboardData(CF_TEXT, hGlob) != nullptr)
					{
						CloseClipboard();
						return true;
					}

					// according to MSVC's docs
					GlobalFree(hGlob);
				}
			}
		}
		
		Debug::LogWarning("Could not get access to the clipboard");

		CloseClipboard(); // Release the clipboard
		return false;
#else
		stringstream cmd;
		cmd << "echo \"" << text << "\" | pbcopy";
		FILE* pipe = popen(cmd.str().c_str(), "r");

		return pipe;
#endif
	}
}
