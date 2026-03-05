#pragma once

#include "common_header.h"

#include "client/renderer/texture/TextureUVCoordinateSet.h"

class TextureAtlasItem {
public:
	TextureAtlasItem() {}

	TextureAtlasItem(const std::string& name, size_t count)
		: mName(name) {
		mTextureUVs.resize(count);
	}

	const std::string& getName() const {
		return mName;
	}

	TextureUVCoordinateSet& operator[] (size_t id) {
		return mTextureUVs[id];
	}

	const TextureUVCoordinateSet& operator[] (size_t id) const {
		return mTextureUVs[id];
	}

	size_t uvCount() const {
		return mTextureUVs.size();
	}

	std::vector<TextureUVCoordinateSet>::iterator begin() {
		return mTextureUVs.begin();
	}

	std::vector<TextureUVCoordinateSet>::iterator end() {
		return mTextureUVs.end();
	}

	void setParsedNodeIndex(int index) {
		mParsedNodeIndex = index;
	}
	int getParsedNodeIndex() const {
		return mParsedNodeIndex;
	}
	
protected:

	std::string mName;
	int mParsedNodeIndex = 0;
	std::vector<TextureUVCoordinateSet> mTextureUVs;
};
