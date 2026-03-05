#include "Dungeons.h"

#include "client/renderer/block/BlockTextureTessellator.h"
#include "client/renderer/texture/TextureUVCoordinateSet.h"
#include "client/renderer/renderer/MinimumCoveringRectangles.h"
#include "world/phys/Vec3.h"
#include "client/renderer/texture/TextureData.h"
//#include "Renderer/TextureGroup.h"
// #include "Renderer/HAL/Enums/TextureFormat.h"

const float BlockTextureTessellator::TEXTURE_OFFSET = 0.0001f;

std::unordered_map<ResourceLocation, std::vector<Rect2D> > BlockTextureTessellator::mTessellatedTextures;

void BlockTextureTessellator::generateUV(const ResourceLocation& texLocation, const UTexture2D* data) {
	if (data == nullptr) {
		return;
	}
	DEBUG_ASSERT(mTessellatedTextures.count(texLocation) == 0, "Already generated");
	
// 	if (data.getImageDescription().mTextureFormat == mce::TextureFormat::R8G8B8A8_UNORM) {
// 		auto mask = std::vector<byte>(data.getWidth() * data.getHeight());
// 
// 		//generate the mask
// 		for (auto x : range(data.getWidth())) {
// 			for (auto y : range(data.getHeight())) {
// 				auto idx = x + y * data.getWidth();
// 				auto pixel = (byte*)data.getDataPacked().data() + idx * 4;
// 				mask[x * data.getHeight() + y] = pixel[3] > 10;
// 			}
// 		}
// 
// 		mTessellatedTextures[texLocation] = MinimumCoveringRectangles::getRectangles(mask, data.getWidth(), data.getHeight());
// 	}
// 	else {
		std::vector<Rect2D> fullRect;
		fullRect.push_back(Rect2D(0, 0, data->GetSizeX(), data->GetSizeY()));
		mTessellatedTextures[texLocation] = fullRect;
// 	}

	DEBUG_ASSERT(mTessellatedTextures[texLocation].size() > 0, "Could not tessellate this texture");
}

void BlockTextureTessellator::clearRegisteredUVs() {
	mTessellatedTextures.clear();
}

void BlockTextureTessellator::render(Tessellator& t, const Vec3& pos, const TextureUVCoordinateSet& tex,
	bool center, int width, float rotation, const Vec3& offsetBottom, bool renderBothSides, bool useNormals, float tileWidth, bool flipHorizontal) {
	Vec3 temp = pos;
	
	auto& rectangles = getTessellatedTexturesInfo(tex.sourceFileLocation);
	DEBUG_ASSERT(rectangles.size() > 0, "You need rectangles to render!");
	// offset from the top
	temp.y += 1;
	// Center x and z
	temp.z += 0.5f;	// -(vec[0].width / (2.0f * 16));
	temp.x += 0.5f;	// -(vec[0].height / (2.0f * 16));
	
	FMatrix _rotateMatrix;

	FVector normal(0, 0, 1);
	normal.RotateAngleAxis(rotation, FVector(0, 1, 0));
// 	_rotateMatrix = glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0, 1, 0));
// 	auto n = glm::vec4(0, 0, 1, 0) * _rotateMatrix;
// 	Vec3 normal = Vec3(n.x, n.y, n.z).normalized();

	int halfWidth = width / 2;

	float texWidth = float(tex.pixelWidth());
	float texHeight = float(tex.pixelHeight());
	float pixelWorldSize = 1 / float(tex.pixelWidth());

	if (tileWidth) {
		pixelWorldSize = 1 / tileWidth;
	}

	for (auto& rect : rectangles) {
		int rectX = rect.x;
		int rectWidth = rect.width;

		if (flipHorizontal) {
			rectX = width - rectX;
			rectWidth = -rectWidth;
		}

		float y0Offset = (rect.y) * pixelWorldSize;
		float y1Offset = (rect.y + rect.height) * pixelWorldSize;
		float y0 = temp.y - y0Offset;
		float y1 = temp.y - y1Offset;

		FVector rotatePos((rect.x - halfWidth) * pixelWorldSize, 0, 0);
		rotatePos = rotatePos.RotateAngleAxis(rotation, FVector(0, 1, 0));
// 		glm::vec4 rotatePos = glm::vec4((rectX - halfWidth) * pixelWorldSize, 0, 0, 1.0f);
// 		rotatePos = _rotateMatrix * rotatePos;
		float x0 = temp.x - rotatePos.X;
		float z0 = temp.z - rotatePos.Z;

		rotatePos = FVector((rect.x - halfWidth + rect.width) * pixelWorldSize, 0, 0);
		rotatePos = rotatePos.RotateAngleAxis(rotation, FVector(0, 1, 0));
// 		rotatePos = glm::vec4((rectX - halfWidth + rectWidth) * pixelWorldSize, 0, 0, 1.0f);
// 		rotatePos = _rotateMatrix * rotatePos;
		float x1 = temp.x - rotatePos.X;
		float z1 = temp.z - rotatePos.Z;

		float xy0 = offsetBottom.x == 0 ? 0 : Math::lerp(0.0f, offsetBottom.x, y0Offset);
		float xy1 = offsetBottom.x == 0 ? 0 : Math::lerp(0.0f, offsetBottom.x, y1Offset);

		float zy0 = offsetBottom.z == 0 ? 0 : Math::lerp(0.0f, offsetBottom.z, y0Offset);
		float zy1 = offsetBottom.z == 0 ? 0 : Math::lerp(0.0f, offsetBottom.z, y1Offset);

		float u0 = tex.offsetWidth(float(rect.x) / texWidth);
		float v0 = tex.offsetHeight(float(rect.y) / texHeight);
		float u1 = tex.offsetWidth(float(rect.x + rect.width) / texWidth);
		float v1 = tex.offsetHeight(float(rect.y + rect.height) / texHeight);

		if (u0 < u1) {
			u0 += TEXTURE_OFFSET;
			u1 -= TEXTURE_OFFSET;
		} else {
			u0 -= TEXTURE_OFFSET;
			u1 += TEXTURE_OFFSET;
		}

		if (v0 < v1) {
			v0 += TEXTURE_OFFSET;
			v1 -= TEXTURE_OFFSET;
		} else {
			v0 -= TEXTURE_OFFSET;
			v1 += TEXTURE_OFFSET;
		}

		if (useNormals) {
			t.normal(normal);
		}

		if (flipHorizontal) {
			t.vertexUV(x0 + xy0, y0, z0 + zy0, u0, v0);
			t.vertexUV(x1 + xy0, y0, z1 + zy0, u1, v0);
			t.vertexUV(x1 + xy1, y1, z1 + zy1, u1, v1);
			t.vertexUV(x0 + xy1, y1, z0 + zy1, u0, v1);
		}
		else {
			t.vertexUV(x0 + xy1, y1, z0 + zy1, u0, v1);
			t.vertexUV(x1 + xy1, y1, z1 + zy1, u1, v1);
			t.vertexUV(x1 + xy0, y0, z1 + zy0, u1, v0);
			t.vertexUV(x0 + xy0, y0, z0 + zy0, u0, v0);
		}
		if (renderBothSides) {
			if (useNormals) {
				t.normal(-normal);
			}

			t.vertexUV(x1 + xy1, y1, z1 + zy1, u1, v1);
			t.vertexUV(x0 + xy1, y1, z0 + zy1, u0, v1);
			t.vertexUV(x0 + xy0, y0, z0 + zy0, u0, v0);
			t.vertexUV(x1 + xy0, y0, z1 + zy0, u1, v0);
		}
	}
}

const  std::vector<Rect2D>& BlockTextureTessellator::getTessellatedTexturesInfo(const ResourceLocation& index) {
	DEBUG_ASSERT(mTessellatedTextures.count(index) > 0, "This texture wasn't tessellated!");

	return mTessellatedTextures[index];
}
