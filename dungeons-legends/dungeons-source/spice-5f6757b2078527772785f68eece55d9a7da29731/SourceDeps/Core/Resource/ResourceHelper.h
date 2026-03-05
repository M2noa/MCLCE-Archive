#pragma once

#include "ResourceUtil.h"
#include "Core/Utility/EnumCast.h"

//class TextureData;

class ResourceLocation {
public:
	ResourceLocation()
		: ResourceLocation("") {
	}

	explicit ResourceLocation(const std::string& path) 
		: mPath(path)
		, mFileSystem(ResourceFileSystem::UserPackage) {
	}

	ResourceLocation(const std::string& path, ResourceFileSystem fileSystem)
		: mPath(path)
		, mFileSystem(fileSystem) {
	}

	bool operator<(const ResourceLocation& rhs) const {
		return mPath < rhs.mPath;
	}

	bool operator==(const ResourceLocation& rhs) const {
		return mPath == rhs.mPath && mFileSystem == rhs.mFileSystem;
	}

	bool operator!=(const ResourceLocation& rhs) const {
		return mPath != rhs.mPath || mFileSystem != rhs.mFileSystem;
	}

	size_t hashCode() const {
		return std::hash<std::string>()(mPath) ^ std::hash<uint8_t>()(static_cast<uint8_t>(mFileSystem));
	}

	std::string getFileSystemName() const {
		return gsl::to_string(ResourceUtil::stringFromPath(mFileSystem));
	}

	// Typical use case you won't need this function.
	// Attempts to recreate the path from an associated ResourceLoader.
	std::string getFullPath() const;

	std::string mPath;
	ResourceFileSystem mFileSystem;
};

namespace std {
	///hash specialization for unordered_maps
	template<>
	struct hash<ResourceLocation> {
		// hash functor for vector
		size_t operator()(const ResourceLocation& _Keyval) const {
			return _Keyval.hashCode();
		}

	};
}

class ResourceInformation {
public:
	enum class ResourceType : int {
		Invalid,
		Resources,
		DataAddOn,
		ScriptAddOn,
		Interface,
		Mandatory,

		Count
	};

	static const std::string& StringFromResourceType(ResourceType value);
	static ResourceType ResourceTypeFromString(const std::string& value);

	ResourceInformation(const std::string& description, const std::string& version, const std::string& uuid, const std::string& type, const std::string& entry = "");
	ResourceInformation(const std::string& description, const std::string& version, const std::string& uuid, ResourceType type = ResourceType::Invalid, const std::string& entry = "");

	std::string mDescription;
	std::string mVersion;
	std::string mUUID;
	ResourceType mType;
	std::string mEntry;
};

class ResourceMetadata {
public:
	ResourceMetadata()
		: mAuthors()
		, mUrl("")
		, mLicense("") {
	}

	std::vector<std::string> mAuthors;
	std::string mUrl;
	std::string mLicense;
};

class ResourceLoader {
public:
	ResourceLoader(const std::string& path)
		: mPath(path) {
	}
	virtual ~ResourceLoader() { }

	virtual bool load(const ResourceLocation& resourceLocation, std::string& resourceStream) = 0;
	virtual std::vector<std::string> loadAllVersionsOf(const ResourceLocation& resourceLocation) = 0;

//#ifndef MCPE_HEADLESS
//	virtual TextureData loadTexture(const ResourceLocation& resourceLocation) = 0;
//#endif

	std::string mPath;
};


class Resource {
public:
	static void registerLoader(ResourceFileSystem fileSystem, std::unique_ptr<ResourceLoader> loader);
	static bool load(const ResourceLocation& resourceLocation, std::string& resourceStream);

//#ifndef MCPE_HEADLESS
//	static TextureData loadTexture(const ResourceLocation& resourceLocation);
//#endif
};

enum class ResourcePackStackType {
	SKIN = 0,
	LEVEL,
	ADDON,
	GLOBAL,
	VANILLA,
	SIZE
};

struct PackIdVersion {
	PackIdVersion() { }
	PackIdVersion(const std::string& id, const std::string& version) { mId = id; mVersion = version; }
	std::string mId;
	std::string mVersion;

	bool operator==(const PackIdVersion&) const;
};

namespace std {
	template <> struct hash<PackIdVersion> {
		std::size_t operator()(const PackIdVersion& rhs) const { 
			return std::hash<std::string>()(rhs.mId + rhs.mVersion);
		}
	};
}

enum class ResourcePackLocation {
	Package,
	World,
	DownloadCache
};
