/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "client/renderer/renderer/Tessellator.h"
#include "client/renderer/texture/TextureUVCoordinateSet.h"

struct Rect2D;
class TextureData;
class Tessellator;

class BlockTextureTessellator {
public:
	static const float TEXTURE_OFFSET;
	static void generateUV(const ResourceLocation& texLocation, const UTexture2D* data);
	static void clearRegisteredUVs();
	static void render(Tessellator& t, const Vec3& pos, const TextureUVCoordinateSet& tex, bool center = true, int width = 16, float rotation = 0, const Vec3& offsetBottom = Vec3::ZERO, bool renderBothSides = false, bool useNormals = false, float tileWidth = 0, bool flipHorizontal = false);

	const static std::vector<Rect2D>& getTessellatedTexturesInfo(const ResourceLocation& location);

private:
	static std::unordered_map<ResourceLocation, std::vector<Rect2D> > mTessellatedTextures;
};
