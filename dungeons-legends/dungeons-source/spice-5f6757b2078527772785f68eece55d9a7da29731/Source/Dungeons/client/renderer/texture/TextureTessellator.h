#pragma once

#include "common_header.h"

#include "client/renderer/renderer/Tessellator.h"
#include "client/renderer/texture/TextureData.h"
#include "world/phys/Vec3.h"
#include "CommonTypes.h"
#include "Core/Utility/MemoryTracker.h"

class TextureTessellator :
	public MemoryTracker {
public:

	//fake lighting info
	Vec3 inverseLightDirection;
	Color ambientColor = Color::BLACK;

	TextureTessellator();
	TextureTessellator(Tessellator& tessellator);

	void tessellate(const TextureData& texture, int topLeftX, int topLeftY, int bottomRightX, int bottomRightY, bool tessellateDown = true, bool tessellateUp = true);

	//mce::Mesh end();

	virtual MemoryStats getStats() const override;

protected:

	std::vector<bool> quads;//BUFFER
	Tessellator& t;

	void _addLighting(const TextureData& texture, const Vec3& normal, const unsigned char* pixel, int px, int py, bool useBakedLighting);

private:

};
