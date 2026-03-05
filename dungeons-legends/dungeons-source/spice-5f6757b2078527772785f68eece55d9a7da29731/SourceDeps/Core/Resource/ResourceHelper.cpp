#include "pch_core.h"

#include "ResourceHelper.h"
//#include "Core/Resource/TextureData.h"
//#include "Core/Debug/DebugUtils.h"
#include "Core/Utility/EnumCast.h"

static std::map<ResourceFileSystem, std::unique_ptr<ResourceLoader> > loaders;

void Resource::registerLoader(ResourceFileSystem fileSystem, std::unique_ptr<ResourceLoader> loader) {
	loaders.emplace(fileSystem, std::move(loader));
}

bool Resource::load(const ResourceLocation& resourceLocation, std::string& resourceStream) {

	auto loader = loaders.find(resourceLocation.mFileSystem);
	if (loader != loaders.end()) {
		return loader->second->load(resourceLocation, resourceStream);
	}

	return false;
}

//#ifndef MCPE_HEADLESS
//TextureData Resource::loadTexture(const ResourceLocation& resourceLocation) {
//
//	auto loader = loaders.find(resourceLocation.mFileSystem);
//	if (loader != loaders.end()) {
//		return loader->second->loadTexture(resourceLocation);
//	}
//
//	return{ };
//}
//#endif

std::string ResourceLocation::getFullPath() const {

	auto loader = loaders.find(mFileSystem);
	if (loader != loaders.end()) {
		return loader->second->mPath + mPath;
	}

	return mPath;
}

static const std::array<std::string, enum_cast(ResourceInformation::ResourceType::Count)> ResourceTypeToStr = {
	"invalid",
	"resources",
	"data",
	"plugin",
	"interface",
	"mandatory"
};

const std::string& ResourceInformation::StringFromResourceType(ResourceType value) {
	auto intValue = enum_cast(value);
	if (static_cast<size_t>(intValue) < ResourceTypeToStr.size() && intValue >= 0) {
		return ResourceTypeToStr[intValue];
	}
	return ResourceTypeToStr.front();
}

ResourceInformation::ResourceType ResourceInformation::ResourceTypeFromString(const std::string& value) {
	auto typeItr = std::find(ResourceTypeToStr.begin(), ResourceTypeToStr.end(), value);
	if (typeItr != ResourceTypeToStr.end()) {
		return static_cast<ResourceType>(typeItr - ResourceTypeToStr.begin());
	}
	return ResourceType::Invalid;
}

ResourceInformation::ResourceInformation(const std::string& description, const std::string& version, const std::string& uuid, const std::string& type, const std::string& entry) 
	: mDescription(description)
	, mVersion(version)
	, mUUID(uuid)
	, mType(ResourceTypeFromString(type))
	, mEntry(entry) {
}

ResourceInformation::ResourceInformation(const std::string& description, const std::string& version, const std::string& uuid, ResourceType type /*= ResourceType::Invalid*/, const std::string& entry /*= ""*/)
	: mDescription(description)
		, mVersion(version)
		, mUUID(uuid)
		, mType(type)
		, mEntry(entry) {
}

bool PackIdVersion::operator==(const PackIdVersion& rhs) const {
	return (mId == rhs.mId) && (mVersion == rhs.mVersion);
}
