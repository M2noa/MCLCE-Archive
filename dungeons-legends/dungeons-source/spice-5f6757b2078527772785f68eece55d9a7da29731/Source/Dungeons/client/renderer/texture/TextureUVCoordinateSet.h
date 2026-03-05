#pragma once

#include "common_header.h"

#include "Core/Resource/ResourceHelper.h"

struct TextureUVCoordinateSet {
	TextureUVCoordinateSet(float u0, float v0, float u1, float v1, uint16_t texWidth, uint16_t texHeight, ResourceLocation originalLocation = {});

	TextureUVCoordinateSet();

	float _u0;
	float _v0;
	float _u1;
	float _v1;
	uint16_t _texSizeW;
	uint16_t _texSizeH;
	ResourceLocation sourceFileLocation;

	float width() const {
		return _u1 - _u0;
	}

	float height() const {
		return _v1 - _v0;
	}

	float offsetWidth(float percent, float startWidth = 0) const;

	float offsetHeight(float percent, float startHeight = 0) const;

	float offsetWidthPixel(float pixels, float startWidth = 0) const;

	float offsetHeightPixel(float pixels, float startHeight = 0) const;

	float inverseOffsetWidth(float percent, float startsWith = 0) const;

	float inverseOffsetHeight(float percent, float startHeight = 0) const;

	TextureUVCoordinateSet getFlippedHorizontal() const {
		return TextureUVCoordinateSet(_u1, _v0, _u0, _v1, _texSizeW, _texSizeH, sourceFileLocation);
	}

	TextureUVCoordinateSet getFlippedVertically() const {
		return TextureUVCoordinateSet(_u0, _v1, _u1, _v0, _texSizeW, _texSizeH, sourceFileLocation);
	}

	TextureUVCoordinateSet offset(float percent) const;

	// The offset should be between 0 and 16
	TextureUVCoordinateSet subTexture(float u0ffset, float v0ffset, int sizeW, int sizeH) const;

	uint16_t asPixelU0() const {
		return (uint16_t)(Math::round(_u0 * _texSizeW));
	}

	uint16_t asPixelV0() const {
		return (uint16_t)(Math::round(_v0 * _texSizeH));
	}

	uint16_t asPixelU1() const {
		return (uint16_t)(Math::round(_u1 * _texSizeW));
	}

	uint16_t asPixelV1() const {
		return (uint16_t)(Math::round(_v1 * _texSizeH));
	}

	uint16_t pixelWidth() const {
		//also fix mirrored textures
		return (_u1 > _u0) ? (asPixelU1() - asPixelU0()) : (asPixelU0() - asPixelU1());
	}

	uint16_t pixelHeight() const {
		//also fix mirrored textures
		return (_v1 > _v0) ? (asPixelV1() - asPixelV0()) : (asPixelV0() - asPixelV1());
	}

	std::pair<uint16_t, uint16_t> topLeftPixel() const {
		return{
			(uint16_t)((_u0 < _u1 ? _u0 : _u1) * _texSizeW),
			(uint16_t)((_v0 < _v1 ? _v0 : _v1) * _texSizeH),
		};
	}

	float pixelSizeU() const {
		return 1.0f / _texSizeW;
	}

	float pixelSizeV() const {
		return 1.0f / _texSizeH;
	}
	
	size_t hashCode() const {
		return (size_t)(_u0 * 8976890 + _v0 * 981131 + _u1 * 29340923 + _v1);
	}

	bool operator==(const TextureUVCoordinateSet& other) const {
		return other._u0 == _u0 && other._u1 == _u1 && other._v0 == _v0 && other._v1 == _v1 && sourceFileLocation == other.sourceFileLocation;
	}

	static TextureUVCoordinateSet fromOldSystem(int tex);

};

namespace std {
	///hash specialization for unordered_maps
	template<>
	struct hash<TextureUVCoordinateSet> {
		// hash functor for vector
		size_t operator()(const TextureUVCoordinateSet& _Keyval) const {
			return _Keyval.hashCode();
		}
	};
}
