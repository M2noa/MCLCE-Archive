#include "Dungeons.h"

#include "client/renderer/texture/TextureAtlasTile.h"
//#include "Renderer/HAL/Interface/Texture.h"
//#include "Renderer/TextureGroup.h"
#include "client/renderer/texture/TextureAtlas.h"
#include "lovika/io/TextureFile.h"
#include "ImageWrapper.h"

TextureAtlasTile::TextureAtlasTile(Color overlay, bool quad, ResourceLocation location)
	: overlay(overlay)
	, quad(quad)
	, location(location)
	, texture(nullptr)
	, textureEmissive(nullptr) {
}

TextureAtlasTile::TextureAtlasTile(TextureAtlasTile &&rhs)
	: overlay(rhs.overlay)
	, quad(rhs.quad)
	, location(rhs.location)
	, mipFadeRate(rhs.mipFadeRate)
	, mipFadeColor(rhs.mipFadeColor)
	, uvs(std::move(rhs.uvs))
	, texture(rhs.texture)
	, textureEmissive(rhs.textureEmissive) {
}

TextureAtlasTile& TextureAtlasTile::operator=(TextureAtlasTile &&rhs) {
	if (this != &rhs) {
		overlay = rhs.overlay;
		quad = rhs.quad;
		location = rhs.location;
		mipFadeRate = rhs.mipFadeRate;
		mipFadeColor = rhs.mipFadeColor;
		uvs = rhs.uvs;
		texture = rhs.texture;
		textureEmissive = rhs.textureEmissive;
	}
	return *this;
}

bool TextureAtlasTile::loadTexture(std::map<ResourceLocation, UTexture2D*>& textures, std::map<std::string, std::string>& resources) {
	if (textures[location] != nullptr) {
		texture = textures[location];
		return true;
	}

	bool isValid;
	uint32_t width;
	uint32_t height;

	std::string fileName = resources[location.mPath];
	EImageFormat::Type imageFormat = EImageFormat::PNG;
	if (Util::endsWith(fileName, ".png")) {
		imageFormat = EImageFormat::PNG;
	}
	else if (Util::endsWith(fileName, ".jpg")) {
		imageFormat = EImageFormat::JPEG;
	}
	FString pathLocation = FPaths::GameDir() + "Content/data/resourcepacks/vanilla/" + fileName.c_str();
	texture = io::loadTexture(pathLocation, imageFormat, isValid, width, height);
	if (isValid) {
		textures[location] = texture;
	}
	return isValid;
}

bool TextureAtlasTile::loadTextureEmissive(std::map<ResourceLocation, UTexture2D*>& textures, std::map<std::string, std::string>& resources) {
	ResourceLocation emissiveLocation(location.mPath + ".emissive");
	//std::string emissiveLocation = location.mPath + ".emissive";

	if (textures[emissiveLocation] != nullptr) {
		textureEmissive = textures[emissiveLocation];
		return true;
	}

	bool isValid;
	uint32_t width;
	uint32_t height;

	std::string fileName = resources[emissiveLocation.mPath];
	if (fileName.empty()) {
		return false;
	}
	EImageFormat::Type imageFormat = EImageFormat::PNG;
	if (Util::endsWith(fileName, ".png")) {
		imageFormat = EImageFormat::PNG;
	}
	else if (Util::endsWith(fileName, ".jpg")) {
		imageFormat = EImageFormat::JPEG;
	}
	FString pathLocation = FPaths::GameDir() + "Content/data/resourcepacks/vanilla/" + fileName.c_str();
	textureEmissive = io::loadTexture(pathLocation, imageFormat, isValid, width, height);
	if (isValid) {
		textures[emissiveLocation] = textureEmissive;
	}
	return isValid;
}

uint32_t TextureAtlasTile::getActualWidth() const {
	return std::max((uint32_t)texture->GetSizeX(), TextureAtlas::MIN_TILE_SIDE);
}

uint32_t TextureAtlasTile::getActualHeight() const {
	return std::max((uint32_t)texture->GetSizeY(), TextureAtlas::MIN_TILE_SIDE);
}

uint32_t TextureAtlasTile::getWidth() const {
	return std::max((uint32_t)texture->GetSizeX() * (quad ? 2 : 1), TextureAtlas::MIN_TILE_SIDE);
}

uint32_t TextureAtlasTile::getHeight() const {
	return std::max((uint32_t)texture->GetSizeY() * (quad ? 2 : 1), TextureAtlas::MIN_TILE_SIDE);
}

uint32_t TextureAtlasTile::getArea() const {
	return getWidth() * getHeight();
}