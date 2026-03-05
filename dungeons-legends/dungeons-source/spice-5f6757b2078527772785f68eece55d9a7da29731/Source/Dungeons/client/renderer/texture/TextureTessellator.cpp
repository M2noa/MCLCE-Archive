#include "Dungeons.h"

#include "client/renderer/texture/TextureTessellator.h"
#include "util/Math.h"

TextureTessellator::TextureTessellator(Tessellator& t) :
	MemoryTracker("TextureTessellator")
	, t(t) {
}

void TextureTessellator::tessellate(const TextureData& texture, int topLeftX, int topLeftY, int bottomRightX, int bottomRightY, bool tessellateDown, bool tessellateUp){
	//first elapse each row, then elapse each column;
	//each pixel places the wall to its right (to lastPixel)
	//if a pixel is empty normals are flipped
	//at the end of the row/column, a fake empty pixel is executed
	bool baked = (bool)inverseLightDirection;

	quads.clear();

	//t.begin( mce::PrimitiveMode::TriangleList, 0 );

	//elapse by rows
	Vec3 position(0, 0, 0);
	int defaultColor = 0;

	float textureWidth = float(bottomRightX - topLeftX);
	float textureHeight = float(bottomRightY - topLeftY);
	float pixelWidth = 1.0f / texture.getWidth();
	float pixelHeight = 1.0f / texture.getHeight();

	//reset the position and draw every missing face!
	position = Vec3::ZERO;

	for (int x = topLeftX; x < bottomRightX; ++x, position.x++) {

		position.z = 0;

		for (int y = topLeftY; y < bottomRightY; ++y, position.z++) {
			int tx = ((x % texture.getWidth()) + texture.getWidth()) % texture.getWidth();
			int ty = ((y % texture.getHeight()) + texture.getHeight()) % texture.getHeight();
			int idx = tx + ty * texture.getWidth();

			auto u = float(x - topLeftX) / textureWidth + pixelWidth * 0.5f;
			auto v = float(y - topLeftY) / textureHeight + pixelHeight * 0.5f;

			auto pixel = texture.getData() + idx * 4;
			bool filtered = pixel[3] <= 1;

			//place top and bottom faces if visible
			if (!filtered) {
				if (tessellateDown) {
					_addLighting(texture, Vec3::NEG_UNIT_Y, pixel, tx, ty, baked);

					t.vertexUV(position.x + 1, position.y, position.z, u, v);
					t.vertexUV(position.x + 1, position.y, position.z + 1, u, v);
					t.vertexUV(position.x, position.y, position.z + 1, u, v);
					t.vertexUV(position.x, position.y, position.z, u, v);
					quads.push_back(false);
				}

				if (tessellateUp) {
					_addLighting(texture, Vec3::UNIT_Y, pixel, tx, ty, baked);

					t.vertexUV(position.x, position.y + 1, position.z, u, v);
					t.vertexUV(position.x, position.y + 1, position.z + 1, u, v);
					t.vertexUV(position.x + 1, position.y + 1, position.z + 1, u, v);
					t.vertexUV(position.x + 1, position.y + 1, position.z, u, v);
					quads.push_back(false);
				}
			}
		}
	}

	//reset the position and draw X faces
	position = Vec3::ZERO;

	for (int y = topLeftY; y < bottomRightY; ++y, position.z++) {
		auto lastPixelPtr = (const unsigned char*)&defaultColor;
		bool lastFiltered = true;
		position.x = 0;

		// <= to check one pixel beyond on texture X axis for tessellating X faces
		for (int x = topLeftX; x <= bottomRightX; ++x, position.x++) {

			int tx = ((x % texture.getWidth()) + texture.getWidth()) % texture.getWidth();
			int ty = ((y % texture.getHeight()) + texture.getHeight()) % texture.getHeight();
			int idx = tx + ty * texture.getWidth();

			float u0 = float(x - topLeftX) / textureWidth;
			float v0 = float(y - topLeftY) / textureHeight;
			float u1 = Math::clamp(u0 + pixelWidth, 0.0f, 1.0f);
			float v1 = Math::clamp(v0 + pixelHeight, 0.0f, 1.0f);

			auto pixelPtr = texture.getData() + idx * 4;
			bool filtered = (x == bottomRightX) || pixelPtr[3] < 10;

			//one is empty and one is not! place a quad on the left, flip the normal if this one is empty
			if (filtered != lastFiltered) {

				//apply lighting
				_addLighting(texture, filtered ? Vec3::UNIT_X : Vec3::NEG_UNIT_X, filtered ? lastPixelPtr : pixelPtr, tx, ty, baked);

				// If this pixel is empty, use previous pixel's UV
				if(filtered) {
					u0 -= pixelWidth;
					u1 -= pixelWidth;
				}

				//create the side quad
				t.vertexUV(position.x, position.y, position.z, u0, v0);
				t.vertexUV(position.x, position.y, position.z + 1, u0, v1);
				t.vertexUV(position.x, position.y + 1, position.z + 1, u1, v1);
				t.vertexUV(position.x, position.y + 1, position.z, u1, v0);

				quads.push_back(filtered);	//flip if filtered
			}
			lastFiltered = filtered;
			lastPixelPtr = pixelPtr;
		}
	}

	//reset the position and draw Z faces
	position = Vec3::ZERO;

	for (int x = topLeftX; x < bottomRightX; ++x, position.x++) {

		auto lastPixelPtr = (const unsigned char*)&defaultColor;
		bool lastFiltered = true;
		position.z = 0;

		// <= to check one pixel beyond on texture Y axis for tessellating Z faces
		for (int y = topLeftY; y <= bottomRightY; ++y, position.z++) {
			int tx = ((x % texture.getWidth()) + texture.getWidth()) % texture.getWidth();
			int ty = ((y % texture.getHeight()) + texture.getHeight()) % texture.getHeight();
			int idx = tx + ty * texture.getWidth();

			float u0 = float(x - topLeftX) / textureWidth;
			float v0 = float(y - topLeftY) / textureHeight;
			float u1 = Math::clamp(u0 + pixelWidth, 0.0f, 1.0f);
			float v1 = Math::clamp(v0 + pixelHeight, 0.0f, 1.0f);

			auto pixelPtr = texture.getData() + idx * 4;
			bool filtered = (y == bottomRightY) || pixelPtr[3] < 10;

			//one is empty and one is not! place a quad behind, flip the normal if this one is empty
			if (filtered != lastFiltered) {

				//apply lighting
				_addLighting(texture, filtered ? Vec3::UNIT_Z : Vec3::NEG_UNIT_Z, filtered ? lastPixelPtr : pixelPtr, tx, ty, baked);

				// If this pixel is empty, use previous pixel's UV
				if(filtered) {
					v0 -= pixelHeight;
					v1 -= pixelHeight;
				}

				t.vertexUV(position.x, position.y + 1, position.z, u0, v1);
				t.vertexUV(position.x + 1, position.y + 1, position.z, u1, v1);
				t.vertexUV(position.x + 1, position.y, position.z, u1, u0);
				t.vertexUV(position.x, position.y, position.z, u0, v0);
				quads.push_back(filtered);	//flip if filtered
			}

			lastFiltered = filtered;
			lastPixelPtr = pixelPtr;
		}
	}

	//do indices
	t.beginIndices( quads.size() * 6 );
	int count = 0;

	for( auto&& q : quads) {
		t.quad(count, q);
		count += 4;
	}
}

void TextureTessellator::_addLighting(const TextureData& texture, const Vec3& normal, const unsigned char* pixel, int px, int py, bool useBakedLighting){
	auto diffuse = Color::fromBytes(pixel);
	if (useBakedLighting) {
		//SEE terrain_light.fragment to see what this is copying
		const float AMBIENT = 0.45f;
		const float XFAC = -0.1f;
		const float ZFAC = 0.1f;

		float yLight = (1.f + normal.y) * 0.5f;
		float L = yLight * (1.f - AMBIENT) + normal.x * normal.x * XFAC + normal.z * normal.z * ZFAC + AMBIENT;
		L += 0.3f;
		L = Math::clamp(L, 0.f, 1.f);

		auto light = Color::fromIntensity(L);

		t.color( diffuse * light );
	}
	else {
		t.normal(normal);
		t.color(diffuse);
	}
}

//mce::Mesh TextureTessellator::end(){
//	//upload the chunk
//	return t.END();
//}

MemoryStats TextureTessellator::getStats() const {
	return MemoryStats(quads.capacity(), 1);
}
