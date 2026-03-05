#pragma once

#include <vector>

class TextureData {
public:

//	using textureData_t = std::vector<uint8_t>;
//
//	TextureData();
	TextureData(UTexture2D* texture);
//	TextureData(const mce::ImageDescription& imageDescription);
//	TextureData(const uint32_t width, const uint32_t height);
//
//	~TextureData();
//
//	TextureData(const TextureData&) = delete;
//	TextureData& operator=(const TextureData&) = delete;
//
//	TextureData(TextureData&& move);
//
//	TextureData& operator=(TextureData&& rhs);
//
//	void clear();

	const uint8_t* getData() const;

//	uint8_t* getDataModify();
//
//	bool isValid() const;

	uint32_t getWidth() const;

	uint32_t getHeight() const;

	EPixelFormat getTextureFormat() const;

//	const textureData_t& getDataPacked() const;
//	textureData_t& getDataPackedModify();
//
//	const mce::ImageDescription& getImageDescription() const;
//
//	void setTextureData(const mce::ImageDescription& imageDescription, textureData_t textureData);
//
//protected:
//
//	textureData_t mInternalMemory;
//
//	mce::ImageDescription mImageDescription;
//
//	void _move(TextureData& rhs);

private:

	UTexture2D* mTexture;
};
