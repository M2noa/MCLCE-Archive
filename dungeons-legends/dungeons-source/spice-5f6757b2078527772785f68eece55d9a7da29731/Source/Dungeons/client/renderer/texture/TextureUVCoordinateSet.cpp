#include "Dungeons.h"

#include <cmath>

#include "client/renderer/texture/TextureUVCoordinateSet.h"

TextureUVCoordinateSet::TextureUVCoordinateSet(float u0, float v0, float u1, float v1, uint16_t texWidth, uint16_t texHeight, ResourceLocation file)
	: _u0(u0)
	, _v0(v0)
	, _u1(u1)
	, _v1(v1)
	, _texSizeW(texWidth)
	, _texSizeH(texHeight)
	, sourceFileLocation(file){
}

TextureUVCoordinateSet::TextureUVCoordinateSet() :
	_u0(0)
	, _v0(0)
	, _u1(0)
	, _v1(0) {

}

float TextureUVCoordinateSet::offsetWidth(float percent, float startWidth /*= 0*/) const
{
	if (startWidth != 0) {
		return startWidth + percent * width();
	}
	else {
		return _u0 + percent * width();
	}
}

float TextureUVCoordinateSet::offsetHeight(float percent, float startHeight /*= 0*/) const
{
	if (startHeight != 0) {
		return startHeight + percent * height();
	}
	else {
		return _v0 + percent * height();
	}
}

float TextureUVCoordinateSet::offsetWidthPixel(float pixels, float startWidth /*= 0*/) const
{
	float percent = pixels / pixelWidth();
	if (startWidth != 0) {
		return startWidth + percent * width();
	}
	else {
		return _u0 + percent * width();
	}
}

float TextureUVCoordinateSet::offsetHeightPixel(float pixels, float startHeight /*= 0*/) const
{
	float percent = pixels / pixelHeight();
	if (startHeight != 0) {
		return startHeight + percent * height();
	}
	else {
		return _v0 + percent * height();
	}
}

float TextureUVCoordinateSet::inverseOffsetWidth(float percent, float startsWith /*= 0*/) const
{
	if (startsWith != 0) {
		return startsWith - percent * width();
	}
	else {
		return _u1 - percent * width();
	}
}

float TextureUVCoordinateSet::inverseOffsetHeight(float percent, float startHeight /*= 0*/) const
{
	if (startHeight != 0) {
		return startHeight - percent * height();
	}
	else {
		return _v1 - percent * height();
	}
}

TextureUVCoordinateSet TextureUVCoordinateSet::offset(float percent) const
{
	float uvOffsetW = percent * width();
	float uvOffsetH = percent * height();
	return TextureUVCoordinateSet(_u0 + uvOffsetW, _v0 + uvOffsetH, _u1 + uvOffsetW, _v1 + uvOffsetH, _texSizeW, _texSizeH, sourceFileLocation);
}

TextureUVCoordinateSet TextureUVCoordinateSet::subTexture(float u0ffset, float v0ffset, int sizeW, int sizeH) const
{
	float pWidth = pixelSizeU();
	float pHeight = pixelSizeV();
	float newU0 = _u0 + pWidth * u0ffset;
	float newV0 = _v0 + pHeight * v0ffset;
	float newSizeW = sizeW * pWidth;
	float newSizeH = sizeH * pHeight;
	return TextureUVCoordinateSet(newU0, newV0, newU0 + newSizeW, newV0 + newSizeH, _texSizeW, _texSizeH, sourceFileLocation);
}

TextureUVCoordinateSet TextureUVCoordinateSet::fromOldSystem(int tex)
{
	float u0 = ((tex % 16) * 16 + 0) / 256.0f;
	float u1 = ((tex % 16) * 16 + 16) / 256.0f;
	float v0 = ((tex / 16) * 16 + 0) / 256.0f;
	float v1 = ((tex / 16) * 16 + 16) / 256.0f;
	return TextureUVCoordinateSet(u0, v0, u1, v1, 256, 256);
}
