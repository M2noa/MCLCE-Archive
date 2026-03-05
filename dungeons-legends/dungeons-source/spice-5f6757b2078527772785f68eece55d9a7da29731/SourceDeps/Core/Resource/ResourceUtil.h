#pragma once

#include <string>

#include "Core/Utility/PrimitiveTypes.h"

enum class ResourceFileSystem {
	UserPackage,
	AppPackage,
	Raw,
	SettingsDir,
	ExternalDir,
	ServerPackage,
	DataDir,
	UserDir,  //TODO this is never used in the code. Why is it here?
	ScreenshotsDir,
	Invalid,
};

class ResourceUtil {
public:
	static ResourceFileSystem pathFromString(string_span name);
	static string_span stringFromPath(ResourceFileSystem fileSystem);
};