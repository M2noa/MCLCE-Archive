/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

#include "util/KeySpan.h"
#include "util/DataIO.h"
#include "util/StringByteInput.h"
#include "CommonTypes.h"

class KeyValueInput {

public:

	struct KeyStream {
		StringByteInput stream;
		const KeySpan& key;

		KeyStream(const KeySpan& key, const std::string& buffer) :
			stream(buffer, key.start, key.end)
			, key(key) {

		}

	};

	struct Iterator {
		Iterator(KeyValueInput& base, int key) :
			key(key)
			, base(base) {

		}

		void operator++() {
			++key;
		}

		bool operator !=(const Iterator& c) const {
			return c.key != key;
		}

		KeyStream operator*() {
			return KeyStream(base.mSpans[key], base.mBuffer);
		}

protected:

		int key;
		KeyValueInput& base;
	};

	Iterator begin() {
		return Iterator(*this, 0);
	}

	Iterator end() {
		return Iterator(*this, mSpans.size());
	}

	void add(const std::string& name, const char* s, int n) {
		mSpans.emplace_back(name, mBuffer.length(), mBuffer.length() + n);

		mBuffer.append(s, n);
	}

	bool empty() const {
		return mSpans.empty();
	}

	bool contains(const std::string& name) const {
		for (auto& k : mSpans) {
			if (k.name == name) {
				return true;
			}
		}
		return false;
	}

	Unique<KeyStream> operator[] (const std::string& name) const {

		for (auto& k : mSpans) {
			if (k.name == name) {
				return make_unique<KeyStream>(k, mBuffer);
			}
		}
		return nullptr;
	}

	void clear() {
		mSpans.clear();
		mBuffer.clear();
	}

protected:

	KeySpanList mSpans;
	std::string mBuffer;

};
