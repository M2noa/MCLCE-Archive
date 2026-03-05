/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "common_header.h"

#include "world/level/ChunkBlockPos.h"
#include "util/NibblePair.h"

template<size_t SIZE>
class DataLayer {
public:

	static const int BYTES = SIZE / 2;

	DataLayer() {

	}

	__inline uint8_t get(size_t pos) const {
		auto slot = pos >> 1;
		return (pos & 1) ? mData[slot].second : mData[slot].first;
	}

	__inline void set(size_t pos, uint8_t val) {
		auto slot = pos >> 1;
		
		DEBUG_ASSERT(val <= 0xf, "Causing a mData overflow. Please handle this overflow explicitly with 'mData & 0xf' if this is intended");

		if (pos & 1) {
			mData[slot].second = (val & 0xf);
		}
		else {
			mData[slot].first = (val & 0xf);
		}
	}
	
	const NibblePair* data() const {
		return mData;
	}

	NibblePair* data() {
		return mData;
	}

	bool isValid() {
		return !mData.empty();
	}

	buffer_span<NibblePair> span() {
		return mData;
	}

	buffer_span<NibblePair> span() const {
		return mData;
	}

protected:

	NibblePair mData[BYTES];
};
