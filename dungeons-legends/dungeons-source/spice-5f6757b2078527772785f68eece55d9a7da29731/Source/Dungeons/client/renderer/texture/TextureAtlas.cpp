#include "Dungeons.h"

#include "client/renderer/texture/TextureAtlas.h"
//#include "platform/AppPlatform.h"
#include "Core/Resource/ResourceHelper.h"
#include "util/range.h"

#include "client/renderer/block/BlockGraphics.h"
#include "util/JSONUtils.h"

#if !defined(MCPE_HEADLESS)
#include "client/renderer/renderer/Tessellator.h"
//#include "Renderer/HAL/Enums/RenderFeature.h"
//#include "Renderer/HAL/Enums/BindFlagsBit.h"
//#include "Renderer/HAL/Interface/RenderContextImmediate.h"
//#include "Renderer/HAL/Interface/RenderContext.h"
//#include "Renderer/HAL/Interface/RenderDevice.h"
//#include "Renderer/TextureGroup.h"
#endif

//#include <glm/gtc/type_precision.hpp>
//#include <glm/core/func_common.hpp>
//#include <glm/gtx/fast_square_root.hpp>

const uint32_t TextureAtlas::MIN_TILE_SIDE = 16;

TextureAtlas::~TextureAtlas() {

}

// #ifndef MCPE_HEADLESS

//TextureAtlas::TextureAtlas(const ResourceLocation& metaFile, ResourcePackManager& resourcePackManager, mce::TextureGroup* textures)
TextureAtlas::TextureAtlas()
	: mMissingTextureItem("", 255) // HACK: 255 different missing textures so objects can depend on specific ids inside it
	//, mMetaLocation(metaFile)
	//, mTextureGroup(textures)
	//, mResourcePackManager(resourcePackManager)
{

	//the Atlas needs to have a low priority to ensure that it gets the event after the materials are reloaded
	//initListener(10.f);
}

template<typename T>
T& select_min(T& A, T& B) {
	return A > B ? B : A;
}

bool TextureAtlas::_tryAssignUVs(uint32_t width, uint32_t height, std::vector<TextureAtlasTile>& tiles) {
	uint32_t x = getPadSize();
	uint32_t y = getPadSize();
	auto widthFloat = static_cast<float>(width);
	auto heightFloat = static_cast<float>(height);

	uint32_t rowStride = 0;
	for (auto&& tile : tiles) {
		auto aw = tile.getActualWidth();
		auto ah = tile.getActualHeight();
		auto w = tile.getWidth();
		auto h = tile.getHeight();
		
		if (ah > aw) {
			ah = aw;
		}
		if (h > w) {
			h = w;
		}

		uint32_t strideX = w + getPadSize() * 2;
		uint32_t strideY = h + getPadSize() * 2;

		rowStride = std::max(strideY, rowStride);

		if (y + h + getPadSize() > height) {
			return false; //woops won't fit, we failed :(
		}

		TextureUVCoordinateSet uvCoordSet = TextureUVCoordinateSet(
			x / widthFloat,
			y / heightFloat,
			(x + aw) / widthFloat,
			(y + ah) / heightFloat,
			width,
			height,
			tile.location
		);

		DEBUG_ASSERT(uvCoordSet._u0 >= 0.0f && uvCoordSet._u0 <= 1.0f, "Invalid UV");
		DEBUG_ASSERT(uvCoordSet._v0 >= 0.0f && uvCoordSet._v0 <= 1.0f, "Invalid UV");
		DEBUG_ASSERT(uvCoordSet._u1 >= 0.0f && uvCoordSet._u1 <= 1.0f, "Invalid UV");
		DEBUG_ASSERT(uvCoordSet._v1 >= 0.0f && uvCoordSet._v1 <= 1.0f, "Invalid UV");

		for (auto& tileUV : tile.uvs) {
			*tileUV = uvCoordSet;
		}

		x += strideX;
		if (x + w + getPadSize() >= width) {
			x = getPadSize();
			y += rowStride;
			rowStride = 0;
		}
	}
	return true;
};

inline int getIndex(int currentWidth, int x, int y) {
	return y * currentWidth * 2 + x;
}

void TextureAtlas::redrawAtlas() {
	//desc.mMipMapCount = mMipCount;

	atlasTexture = UTexture2D::CreateTransient(mWidth, mHeight, EPixelFormat::PF_B8G8R8A8);

	atlasTexture->MipGenSettings = TMGS_LeaveExistingMips;
	atlasTexture->AddressX = TextureAddress::TA_Wrap;
	atlasTexture->AddressY = TextureAddress::TA_Wrap;
	atlasTexture->Filter = TextureFilter::TF_Nearest;
	atlasTexture->PlatformData->NumSlices = 1;
	atlasTexture->NeverStream = true;

	int currentWidth = atlasTexture->GetSizeX();
	int currentHeight = atlasTexture->GetSizeY();
	int atlasTopWidth = currentWidth;
	int atlasTopHeight = currentHeight;

	for (int i = 1; i < 4; i++) {
		currentWidth = atlasTopWidth >> i;
		currentHeight = atlasTopHeight >> i;
		int currentBytes = currentWidth * currentHeight * 4;

		FTexture2DMipMap* Mip = new(atlasTexture->PlatformData->Mips) FTexture2DMipMap();
		Mip->SizeX = currentWidth;
		Mip->SizeY = currentHeight;

		Mip->BulkData.Lock(LOCK_READ_WRITE);
		Mip->BulkData.Realloc(currentBytes);
		Mip->BulkData.Unlock();
	}

	atlasTexture->UpdateResource();

	// Emissive atlas
	atlasTextureEmissive = UTexture2D::CreateTransient(mWidth, mHeight, EPixelFormat::PF_B8G8R8A8);
	atlasTextureEmissive->AddressX = TextureAddress::TA_Wrap;
	atlasTextureEmissive->AddressY = TextureAddress::TA_Wrap;
	atlasTextureEmissive->Filter = TextureFilter::TF_Nearest;

	_renderAtlasLevel(0, mTextureAtlasTiles);

	// Build mipmaps
	for (int i = 1; i < 4; i++) {
		currentWidth = atlasTopWidth >> i;
		currentHeight = atlasTopHeight >> i;

		FTexture2DMipMap& MipSource = atlasTexture->PlatformData->Mips[i - 1];
		uint8* dataSource = (uint8*)MipSource.BulkData.Lock(LOCK_READ_ONLY);

		FTexture2DMipMap& MipDest = atlasTexture->PlatformData->Mips[i];
		uint8* dataDest = (uint8*)MipDest.BulkData.Lock(LOCK_READ_WRITE);

		for (int y = 0; y < currentHeight; y++) {
			for (int x = 0; x < currentWidth; x++) {
				int index = y * currentWidth + x;
				int xs = x * 2;
				int ys = y * 2;

				dataDest[index * 4 + 0] = (
					dataSource[getIndex(currentWidth, xs, ys) * 4 + 0] +
					dataSource[getIndex(currentWidth, xs + 1, ys) * 4 + 0] +
					dataSource[getIndex(currentWidth, xs, ys + 1) * 4 + 0] +
					dataSource[getIndex(currentWidth, xs + 1, ys + 1) * 4 + 0]
					) / 4;
				dataDest[index * 4 + 1] = (
					dataSource[getIndex(currentWidth, xs, ys) * 4 + 1] +
					dataSource[getIndex(currentWidth, xs + 1, ys) * 4 + 1] +
					dataSource[getIndex(currentWidth, xs, ys + 1) * 4 + 1] +
					dataSource[getIndex(currentWidth, xs + 1, ys + 1) * 4 + 1]
					) / 4;
				dataDest[index * 4 + 2] = (
					dataSource[getIndex(currentWidth, xs, ys) * 4 + 2] +
					dataSource[getIndex(currentWidth, xs + 1, ys) * 4 + 2] +
					dataSource[getIndex(currentWidth, xs, ys + 1) * 4 + 2] +
					dataSource[getIndex(currentWidth, xs + 1, ys + 1) * 4 + 2]
					) / 4;
				dataDest[index * 4 + 3] = dataSource[getIndex(currentWidth, xs, ys) * 4 + 3];
			}
		}

		MipSource.BulkData.Unlock();
		MipDest.BulkData.Unlock();
	}

	atlasTexture->UpdateResource();
	atlasTextureEmissive->UpdateResource();
}

void TextureAtlas::_addElement(Json::Value texture, ParsedAtlasNode& newNode, int atIndex, float mipFadeAmount, const Color& mipFadeColor) {
 	if (texture.isObject()) {
 		ParsedAtlasNodeElement element;
 		element.location = ResourceLocation(texture["path"].asString());
 		auto overlayString = texture["overlay_color"].asString();
 		element.mipFadeAmount = texture["mip_fade_amount"].asFloat(mipFadeAmount);
 		element.mipFadeColor = mipFadeColor;
 		JsonUtil::parseValue(texture["mip_fade_color"], element.mipFadeColor);
 
 		if (overlayString.size() > 0) {
 			element.overlay = Color::fromARGB(strtoul(overlayString.substr(1).c_str(), nullptr, 16));
 			element.overlay.a = 1.f;
		
			newNode.elements[atIndex] = element;
		}
 		
//  		newNode.elements[atIndex] = element;
 	}
 	else if (texture.isString()) {
 		ParsedAtlasNodeElement element;
 		element.location = ResourceLocation(texture.asString());
 		element.mipFadeAmount = mipFadeAmount;
 		element.mipFadeColor = mipFadeColor;
 		newNode.elements[atIndex] = element;
 	}
 	else {
 		DEBUG_FAIL("INVALID");
 	}
}

void TextureAtlas::_readNode(Json::Value jsonNode, ParsedAtlasNode& node, bool isRefreshing) {
	node.quad = jsonNode["quad"].asBool(false);
	Json::Value texture = jsonNode["textures"];
	float defaultFadeMipAmount = jsonNode["mip_fade_amount"].asFloat();
	Color defaultFadeMipColor = Color::NIL;
	JsonUtil::parseValue(jsonNode["mip_fade_color"], defaultFadeMipColor);

	if (texture.isArray()) {
		if (!isRefreshing) {
			node.elements.resize(texture.size());
		}

		size_t elementIndex = 0;
		for (auto& textureElement : texture) {
			_addElement(textureElement, node, elementIndex, defaultFadeMipAmount, defaultFadeMipColor);
			++elementIndex;
			// This can happen if an override of a vanilla node has more elements in it than we expect
			if (elementIndex >= node.elements.size()) {
				break;
			}
		}
	}
	else {
		if (!isRefreshing) {
			node.elements.resize(1);
		}

		_addElement(texture, node, 0, defaultFadeMipAmount, defaultFadeMipColor);
	}
}

void TextureAtlas::_loadAtlasNodes(Json::Value root, std::vector<ParsedAtlasNode>& atlasNodes) {

	for (auto& nodeName : root.getMemberNames()) {
		ParsedAtlasNode newNode;
		Json::Value jsonNode = root[nodeName];

		newNode.name = nodeName;
		_readNode(jsonNode, newNode, false);

		atlasNodes.push_back(newNode);
	}
}

void TextureAtlas::_addTile(std::vector<TextureAtlasTile> &tiles, TextureUVCoordinateSet* uvSet, bool quad, float fadePerMipAmount, const Color& fadeMipColor, ResourceLocation& location, const Color& overlayColor) {
	bool found = false;
	for (auto& tileInstance : tiles) {
		if (tileInstance.overlay == overlayColor &&
			tileInstance.quad == quad &&
			tileInstance.location == location &&
			tileInstance.mipFadeRate == fadePerMipAmount &&
			tileInstance.mipFadeColor == fadeMipColor) {

			found = true;
			tileInstance.uvs.push_back(uvSet);
		}
	}

	if (!found) {
		TextureAtlasTile tile(overlayColor, quad, location);
		tile.mipFadeRate = fadePerMipAmount;
		tile.mipFadeColor = fadeMipColor;
		tiles.emplace_back(std::move(tile));
		tiles.back().uvs.push_back(uvSet);
	}
}

void TextureAtlas::loadMetaFile() {
	mTextureItems.clear();
	mTextureAtlasTiles.clear();
	mAtlasNodesList.clear();
	mTextureGroup.clear();
	mTextureGroupEmissive.clear();

	{
		//std::string parseString;
		Json::Value root;
		Json::Reader reader;

		//Resource::load(mMetaLocation, parseString);
		FString path = FPaths::GameDir() + "Content/data/resourcepacks/vanilla/resources.json";
		std::ifstream parseString(*path);

		bool parsingSucessful = reader.parse(parseString, root);
		if (!parsingSucessful) {
			return;
		}

		auto textureList = root["resources"]["textures"];
		for (auto& nodeName : textureList.getMemberNames()) {
			mResourceMap[nodeName] = textureList[nodeName].asString();
		}
	}

	//std::string parseString;
	Json::Value root;
	Json::Reader reader;

	//Resource::load(mMetaLocation, parseString);
	FString path = FPaths::GameDir() + "Content/data/resourcepacks/vanilla/images/terrain_texture.json";
	std::ifstream parseString(*path);

	bool parsingSucessful = reader.parse(parseString, root);
	if (!parsingSucessful) {
		return;
	}

	mPadSize = root.get("padding", 0).asInt();

	mResourceName = root["texture_name"].asString();

	_loadAtlasNodes(root["texture_data"], mAtlasNodesList);

	// Now that we have the properly overridden nodes,
	// we can use that list to generate the list of tiles
	for (auto& atlasNode : mAtlasNodesList) {
		for (auto& atlasNodeEntry : atlasNode.elements) {
			_addTile(mTextureAtlasTiles, &atlasNodeEntry.uv, atlasNode.quad, atlasNodeEntry.mipFadeAmount, atlasNodeEntry.mipFadeColor, atlasNodeEntry.location, atlasNodeEntry.overlay);
		}
	}

	//TODO how to guess the sizes on the server without loading the textures? Is it even needed? Is any of this needed?
	for (auto&& tile : mTextureAtlasTiles) {
		bool isValid = tile.loadTexture(mTextureGroup, mResourceMap);
		if (!isValid) {
			UE_LOG(LogTemp, Warning, TEXT("Could not load texture: %s"), UTF8_TO_TCHAR(tile.location.mPath.c_str()));
		}
		tile.loadTextureEmissive(mTextureGroupEmissive, mResourceMap);
	}

	mTextureAtlasTiles.erase(
		std::remove_if(
			mTextureAtlasTiles.begin(),
			mTextureAtlasTiles.end(),
			[](TextureAtlasTile& element) -> bool {
		return element.texture == nullptr;
	}
		),
		mTextureAtlasTiles.end()
		);

	mWidth = mHeight = 1;
	uint32_t area = 0;
	for (auto&& tile : mTextureAtlasTiles) {
		area += (tile.getWidth() + 2 * getPadSize()) * (tile.getHeight() + 2 * getPadSize());
	}

	while (mWidth * mHeight < area) {
		//double the smallest between the dimensions or width
		select_min(mWidth, mHeight) *= 2;
	}

	//assign the uvs to all textures based on their size
	//sort the tiles based on their size, biggest last
	std::stable_sort(mTextureAtlasTiles.begin(), mTextureAtlasTiles.end(), [](const TextureAtlasTile& A, const TextureAtlasTile& B) {
		return A.getArea() < B.getArea();
	});

	//try to fit all tiles inside the assigned sizes, doubling the size and retrying if it fails
	while (!_tryAssignUVs(mWidth, mHeight, mTextureAtlasTiles)) {
		select_min(mWidth, mHeight) *= 2;
	}

	// Now that all the uv's have been generated, we need to build the mTextureItems list
	int nodeIndex = 0;
	for (auto& atlasNode : mAtlasNodesList) {
		TextureAtlasItem newAtlasItem(atlasNode.name, atlasNode.elements.size());
		newAtlasItem.setParsedNodeIndex(nodeIndex);

		int uvIndex = 0;
		for (auto& atlasNodeEntry : atlasNode.elements) {
			newAtlasItem[uvIndex] = atlasNodeEntry.uv;
			++uvIndex;
		}
		mTextureItems[atlasNode.name] = newAtlasItem;
		++nodeIndex;
	}
}

const TextureAtlasItem& TextureAtlas::getTextureItem(const std::string& textureName) const {
	auto tex = mTextureItems.find(textureName);
	DEBUG_ASSERT(tex != mTextureItems.end(), "This texture item doesn't exist");
	if (tex == mTextureItems.end()) {
		return mMissingTextureItem;
	}
	return tex->second;
}

FVector2D TextureAtlas::getAtlasFaceSize() const {
	return mAtlasFaceSize;
}

int TextureAtlas::getPadSize() const {
	return mPadSize;
}

void TextureAtlas::onAppResumed() {
// 	mce::RenderContextImmediate::get().beginFrame();

	redrawAtlas();

// 	mce::RenderContextImmediate::get().endFrame();
}

// glm::vec4 gammaCorrectedAverage(const glm::vec4& a, const glm::vec4& b, const glm::vec4& c, const glm::vec4& d) {
// 
// 	// gamma of 2.0
// 	return glm::fastSqrt((a * a + b * b + c * c + d * d) / 4.f);
// }

// void readTexture(glm::u8vec4& color, const glm::vec2& uv, const TextureData& textureData, const uint32_t textureDataStride, const glm::ivec2& maxDimensions) {
// 	uint32_t width = textureData.getWidth();
// 	uint32_t height = textureData.getHeight();
// 
// 	if (height > width) {
// 		height = width;
// 	}
// 
// 	glm::ivec2 pixelOffset(static_cast<uint32_t>(uv.x * width), static_cast<uint32_t>(uv.y * height));
// 
// 	// clamping the uv's just like dx/ogl
// 	pixelOffset = glm::clamp(pixelOffset, glm::ivec2(0, 0), maxDimensions);
// 
// 	const uint32_t readPixelIndex = width * pixelOffset.y + pixelOffset.x;
// 	const uint32_t readBufferIndex = readPixelIndex * textureDataStride;
// 
// 	switch (textureData.getTextureFormat()) {
// 		case mce::TextureFormat::R8G8B8A8_UNORM:
// 		{
// 			color = reinterpret_cast<const glm::u8vec4&>(textureData.getDataPacked().at(readBufferIndex));
// 			break;
// 		}
// 		case mce::TextureFormat::R8G8B8_UNORM:
// 		{
// 			const glm::u8vec3& readColor = reinterpret_cast<const glm::u8vec3&>(textureData.getDataPacked().at(readBufferIndex));
// 			color = glm::u8vec4(readColor, std::numeric_limits<glm::u8>::max());
// 
// 			break;
// 		}
// 		default:
// 			MCEUNKOWNFAIL(textureData.getTextureFormat());
// 	}
// }

// void renderSingleTile(const TextureUVCoordinateSet& uvs, TextureData& atlasTexture, const TextureData& textureData, int padSize, const Color& overlay, const bool quad, const uint32_t mipID, float mipFadeAmount, Color mipFadeColor) {
// 	const float toUINT = std::numeric_limits<glm::u8>::max();
// 	const float toFloat = 1.f / toUINT;
// 
// 	const glm::vec4 CURRENT_COLOR(overlay.r, overlay.g, overlay.b, overlay.a);
// 
// 	uint32_t width = textureData.getWidth();
// 	uint32_t height = textureData.getHeight();
// 
// 	if (height > width) {
// 		height = width;
// 	}
// 
// 	const glm::ivec2 maxDimensions(width - 1, height - 1);
// 
// 	const int32_t blurRadius = 1 << mipID;
// 	const float inverseBlurRadius = 1.f / static_cast<float>(blurRadius);
// 	const float inverseBlurRadiusSquared = inverseBlurRadius * inverseBlurRadius;
// 
// 	// this using not modified pad size because it using texture size wich never change based on mipmap
// 	float uvPadWidth = (padSize / static_cast<float>(uvs._texSizeW));
// 	float uvPadHeight = (padSize / static_cast<float>(uvs._texSizeH));
// 
// 	//actual pad should scale down based on mipmap
// 	padSize >>= mipID;
// 
// 	float quadScale = quad ? 2.0f : 1.0f;
// 	float x0 = uvs._u0 - uvPadWidth;
// 	float y0 = uvs._v0 - uvPadHeight;
// 	float w = (uvs._u1 - uvs._u0) * quadScale + uvPadWidth * 2;
// 	float h = (uvs._v1 - uvs._v0) * quadScale + uvPadHeight * 2;
// 	float x1 = x0 + w;
// 	float y1 = y0 + h;
// 
// 
// 	// x pixel coordinate start/end
// 	const uint32_t xp0 = static_cast<uint32_t>(x0 *  atlasTexture.getWidth());
// 	const uint32_t xp1 = static_cast<uint32_t>(x1 *  atlasTexture.getWidth());
// 
// 	// y pixel coordinate start/end
// 	const uint32_t yp0 = static_cast<uint32_t>(y0 *  atlasTexture.getHeight());
// 	const uint32_t yp1 = static_cast<uint32_t>(y1 *  atlasTexture.getHeight());
// 
// 	const uint32_t writeStride = mce::TextureHelper::textureFormatToByteStride(atlasTexture.getTextureFormat());
// 	const uint32_t readStride = mce::TextureHelper::textureFormatToByteStride(textureData.getTextureFormat());
// 
// 	const glm::vec2 inverseReadTextureDimensions = glm::vec2(1.0f / (xp1 - xp0), 1.f / (yp1 - yp0));
// 	const glm::vec2 mipmappedTextureSize = glm::vec2(width >> mipID, height >> mipID);
// 
// 
// 	std::vector<uint8_t>& atlasBuffer = atlasTexture.getDataPackedModify();
// 
// 	for (uint32_t yPixelIndex = yp0; yPixelIndex != yp1; ++yPixelIndex) {
// 
// 		for (uint32_t xPixelIndex = xp0; xPixelIndex != xp1; ++xPixelIndex) {
// 
// 			// atlas
// 			const uint32_t atlasPixelIndex = atlasTexture.getWidth() * yPixelIndex + xPixelIndex;
// 			const uint32_t atlasBufferIndex = atlasPixelIndex * writeStride;
// 
// 			MCEASSERT(atlasBufferIndex < atlasBuffer.size());
// 
// 			// get the location of where we want to write in the atlas
// 			uint8_t* const colorPointer = &atlasBuffer.at(atlasBufferIndex);
// 			glm::u8vec4& atlasColor = *reinterpret_cast<glm::u8vec4*>(colorPointer);
// 
// 			// calculate uv's
// 			//0 to 1 for actual data padding will be 0 or 1
// 			glm::vec2 uv((float(xPixelIndex) - xp0 - padSize) / mipmappedTextureSize.x, (float(yPixelIndex) - yp0 - padSize) / mipmappedTextureSize.y);
// 
// 			// run our "shader code" to apply the color, convert to float
// 			glm::vec4 atlasColorFloat;
// 
// 			if (blurRadius <= 1) {
// 
// 				// finally read from the raw bytes into our buffer, this assumes RGBA8_UNORM for dest
// 				readTexture(atlasColor, uv, textureData, readStride, maxDimensions);
// 
// 				atlasColorFloat = glm::vec4(atlasColor.r * toFloat, atlasColor.g * toFloat, atlasColor.b * toFloat, atlasColor.a * toFloat);
// 			}
// 			else {
// 
// 				glm::vec4 filterColor;
// 				glm::u8vec4 filterTextureColor;
// 				glm::vec2 blurUV;
// 				for (int32_t i = 0; i < blurRadius; ++i) {
// 					for (int32_t j = 0; j < blurRadius; ++j) {
// 						blurUV = uv + inverseReadTextureDimensions * glm::vec2(j, i);
// 
// 						readTexture(filterTextureColor, blurUV, textureData, readStride, maxDimensions);
// 						filterColor += glm::vec4(filterTextureColor) * toFloat;
// 					}
// 				}
// 
// 				filterColor *= inverseBlurRadiusSquared;
// 
// 				atlasColorFloat = filterColor;
// 			}
// 
// 			const glm::vec4::value_type alpha = atlasColorFloat.a;
// 
// 			atlasColorFloat = glm::mix(atlasColorFloat, CURRENT_COLOR * atlasColorFloat, alpha * CURRENT_COLOR.a);
// 
// 			atlasColorFloat.a = alpha;
// 
// 			if (CURRENT_COLOR.a > 0) {
// 				atlasColorFloat.a = 1.f;
// 			}
// 
// 			if (mipFadeAmount > 0.0f) {
// 				Color asColor(atlasColorFloat.r, atlasColorFloat.g, atlasColorFloat.b, atlasColorFloat.a);
// 				asColor = Color::lerp(asColor, mipFadeColor, mipFadeAmount);
// 				atlasColorFloat.r = asColor.r;
// 				atlasColorFloat.g = asColor.g;
// 				atlasColorFloat.b = asColor.b;
// 				atlasColorFloat.a = asColor.a;
// 			}
// 
// 			// convert back to 8 bit UNORM
// 			atlasColorFloat *= toUINT;
// 
// 			// truncate and assign
// 			atlasColor = glm::u8vec4(atlasColorFloat.r, atlasColorFloat.g, atlasColorFloat.b, atlasColorFloat.a);
// 
// 		}
// 	}
//}

void renderSingleTile(uint8* atlasData, int atlasWidth, int atlasHeight, const TextureUVCoordinateSet& uvs, UTexture2D* texture, int padSize, const Color& overlay, bool quad) {
	// TODO: fix quad == true remap (water/lava)

	FTexture2DMipMap& Mip = texture->PlatformData->Mips[0];
	uint8* textureData = (uint8*)Mip.BulkData.Lock(LOCK_READ_ONLY);

	int textureW = texture->GetSizeX();
	int textureH = texture->GetSizeY();

	int atlasX = static_cast<uint32>(uvs._u0 * atlasWidth);
	int atlasY = static_cast<uint32>(uvs._v0 * atlasHeight);
	for (int y = 0; y < textureH; y++) {
		for (int x = 0; x < textureW; x++) {
			int i = y * textureW + x;
			int ia = (y + atlasY) * atlasWidth + x + atlasX;

			Color mixedColor((float)textureData[i * 4 + 2] / 255.0f, (float)textureData[i * 4 + 1] / 255.0f, (float)textureData[i * 4 + 0] / 255.0f, (float)textureData[i * 4 + 3] / 255.0f);
			mixedColor = Color::lerp(mixedColor, mixedColor * overlay, mixedColor.a * overlay.a);

			if (textureData[i * 4 + 3] > 0) {
				atlasData[ia * 4 + 0] = (uint8)(mixedColor.b * 255.0f);
				atlasData[ia * 4 + 1] = (uint8)(mixedColor.g * 255.0f);
				atlasData[ia * 4 + 2] = (uint8)(mixedColor.r * 255.0f);
				atlasData[ia * 4 + 3] = textureData[i * 4 + 3];
			}
		}
	}

	Mip.BulkData.Unlock();
}

// void TextureAtlas::_renderAtlasLevel(mce::TexturePtr& atlasTexture, const uint32_t mipID, const std::vector<TextureAtlasTile>& tiles) {
void TextureAtlas::_renderAtlasLevel(uint32_t mipID, std::vector<TextureAtlasTile>& tiles) {
	TextureAtlasTile* dirtTile = nullptr;
	std::vector<TextureAtlasTile*> grassSides;
	for (auto& tile : tiles) {
		if (tile.location.mPath == "block.dirt") {
			dirtTile = &tile;
		}
		else if (tile.location.mPath == "block.grass.side") {
			grassSides.emplace_back(&tile);
		}
	}

	FTexture2DMipMap& AtlasMip = atlasTexture->PlatformData->Mips[0];
	uint8* atlasData = (uint8*)AtlasMip.BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memset(atlasData, 0, atlasTexture->GetSizeX() * atlasTexture->GetSizeY() * 4);

	FTexture2DMipMap& AtlasMipEmissive = atlasTextureEmissive->PlatformData->Mips[0];
	uint8* atlasDataEmissive = (uint8*)AtlasMipEmissive.BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memset(atlasDataEmissive, 0, atlasTextureEmissive->GetSizeX() * atlasTextureEmissive->GetSizeY() * 4);

	if (dirtTile != nullptr) {
		for (auto* tile : grassSides) {
			renderSingleTile(atlasData, atlasTexture->GetSizeX(), atlasTexture->GetSizeY(), *tile->uvs[0], dirtTile->texture, getPadSize(), dirtTile->overlay, dirtTile->quad);
		}
	}

	for (auto& tile : tiles) {
		renderSingleTile(atlasData, atlasTexture->GetSizeX(), atlasTexture->GetSizeY(), *tile.uvs[0], tile.texture, getPadSize(), tile.overlay, tile.quad);

		if (tile.textureEmissive != nullptr) {
			renderSingleTile(atlasDataEmissive, atlasTextureEmissive->GetSizeX(), atlasTextureEmissive->GetSizeY(), *tile.uvs[0], tile.textureEmissive, getPadSize(), tile.overlay, tile.quad);
		}
	}

	AtlasMipEmissive.BulkData.Unlock();
	AtlasMip.BulkData.Unlock();

// 	auto& renderContext = mce::RenderContextImmediate::get();
// 
// 	mce::TextureDescription atlasDescription = atlasTexture->getDescription();
// 
// 	atlasDescription.mWidth >>= mipID;
// 	atlasDescription.mHeight >>= mipID;
// 
// 	TextureData atlasTextureData(atlasDescription);
// 
// 	atlasTextureData.getDataPackedModify().resize(atlasTexture->getDescription().getSizeInBytes());
// 
// 	for (auto& tile : tiles) {
// 		renderSingleTile(*tile.uvs[0], atlasTextureData, *tile.mTextureData, getPadSize(), tile.overlay, tile.quad, mipID, tile.mipFadeRate * mipID, tile.mipFadeColor);
// 	}
// 
// 	atlasTexture->subBuffer(renderContext, atlasTextureData.getDataPackedModify().data(), 0, 0, atlasTextureData.getWidth(), atlasTextureData.getHeight(), mipID);
}

// #else //#ifndef MCPE_HEADLESS
// 
// TextureAtlas::TextureAtlas(const ResourceLocation& metaFile, ResourcePackManager& resourcePackManager, mce::TextureGroup* textures)
// 	: AppPlatformListener(false)
// 	, mMetaLocation(metaFile)
// 	, mTextureGroup(textures)
// 	, mResourcePackManager(resourcePackManager)
// 	, mMissingTextureItem("", 255) // HACK: 255 different missing textures so objects can depend on specific ids inside it
// {
// 
// }
// 
// void TextureAtlas::loadMetaFile() {
// 
// }
// 
// void TextureAtlas::onAppResumed() {
// 
// }
// 
// const TextureAtlasItem& TextureAtlas::getTextureItem(const std::string& textureName) const {
// 	return mMissingTextureItem;
// }
// 
// void TextureAtlas::redrawAtlas() {
// 
// }
// 
// #endif //#ifndef MCPE_HEADLESS