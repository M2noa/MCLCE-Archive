#include "Dungeons.h"

#include "TextureData.h"
//#include "Core/Debug/DebugUtils.h"
//
//#include "Renderer/HAL/Enums/TextureFormat.h"

TextureData::TextureData(UTexture2D* texture)
	: mTexture(texture)
{

}

//TextureData::TextureData() {
//
//}
//
//TextureData::TextureData(const mce::ImageDescription& imageDescription)
//	: mImageDescription(imageDescription) {
//}
//
//void TextureData::_move(TextureData& rhs) {
//
//	mImageDescription = rhs.mImageDescription;
//	mInternalMemory = std::move(rhs.mInternalMemory);
//}
//
//TextureData::TextureData(TextureData&& rhs) {
//	_move(rhs);
//}
//
//TextureData::TextureData(const uint32_t width, const uint32_t height) {
//
//	mImageDescription.mWidth = width;
//	mImageDescription.mHeight = height;
//	mImageDescription.mTextureFormat = mce::TextureFormat::R8G8B8A8_UNORM;
//	mInternalMemory.resize(width * height * 4);
//}
//
//TextureData& TextureData::operator=(TextureData&& rhs) {
//
//	_move(rhs);
//
//	return *this;
//}
//
//void TextureData::clear() {
//	mInternalMemory.clear();
//}
//
//TextureData::~TextureData() {
//
//}

const uint8_t* TextureData::getData() const {
	//return mInternalMemory.data();

	FTexture2DMipMap& TextureMip = mTexture->PlatformData->Mips[0];
	uint8* textureData = (uint8*)TextureMip.BulkData.Lock(LOCK_READ_ONLY);
	int size = mTexture->GetSizeX() * mTexture->GetSizeY() * 4;
	uint8* data = new uint8[size];
	FMemory::Memcpy(data, textureData, size);
	TextureMip.BulkData.Unlock();

	return data;
}

//uint8_t* TextureData::getDataModify() {
//	return mInternalMemory.data();
//}
//
//bool TextureData::isValid() const {
//	return mImageDescription.mWidth > 0 && mImageDescription.mHeight > 0 && mInternalMemory.empty() == false;
//}

uint32_t TextureData::getWidth() const {
	//return mImageDescription.mWidth;
	return mTexture->GetSizeX();
}

uint32_t TextureData::getHeight() const {
	//return mImageDescription.mHeight;
	return mTexture->GetSizeY();
}

EPixelFormat TextureData::getTextureFormat() const {
// 	return mImageDescription.mTextureFormat;
	return mTexture->GetPixelFormat();
}

//const TextureData::textureData_t& TextureData::getDataPacked() const {
//	return mInternalMemory;
//}
//
//TextureData::textureData_t& TextureData::getDataPackedModify() {
//	return mInternalMemory;
//}
//
//const mce::ImageDescription& TextureData::getImageDescription() const {
//	return mImageDescription;
//}
//
//void TextureData::setTextureData(const mce::ImageDescription& imageDescription, textureData_t textureData) {
//	mImageDescription = imageDescription;
//	mInternalMemory = std::move(textureData);
//}
//
//
