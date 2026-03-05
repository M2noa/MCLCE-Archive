#pragma once

#include "common_header.h"
//#include "../ImageWrapper/Public/Interfaces/IImageWrapper.h"
#include "ImageWrapper.h"
#include "UnrealString.h"
#include "Engine/Texture2D.h"

namespace io {

UTexture2D* loadTexture(const FString& FullFilePath, EImageFormat::Type ImageFormat, bool& IsValid, uint32_t& Width, uint32_t& Height);

};
