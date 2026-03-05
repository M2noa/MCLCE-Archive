#include "pch_core.h"

#include "ResourceUtil.h"
#include "Core/Utility/EnumCast.h"

//this is an implicit map, the position is the index
const string_span gResourceFileSystemMap[] = {
	"InUserPackage",	// UserPackage,
	"InAppPackage",		// AppPackage,
	"RawPath",			// Raw,
	"InSettingDir",		// SettingsDir,
	"InExternalDir",	// ExternalDir,
	"InServerPackage",	// ServerPackage,		
	"InDataDir",		// DataDir,
	"InUserDir",		// UserDir,
	"InWorldDir",		// ScreenshotsDir
	{}					// Invalid,
};

ResourceFileSystem ResourceUtil::pathFromString(string_span name) {
	for (auto id = 0; id < enum_cast(ResourceFileSystem::Invalid); ++id) {
		if(gResourceFileSystemMap[id] == name) {
			return static_cast<ResourceFileSystem>(id);
		}
	}
	return ResourceFileSystem::Invalid;
}

string_span ResourceUtil::stringFromPath(ResourceFileSystem fileSystem) {
	return gResourceFileSystemMap[enum_cast(fileSystem)];
}
