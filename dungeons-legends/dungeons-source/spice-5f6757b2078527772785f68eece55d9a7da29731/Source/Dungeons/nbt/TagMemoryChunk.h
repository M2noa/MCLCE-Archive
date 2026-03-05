#pragma once

#include "common_header.h"

///wrapper over legacy direct memory access
struct TagMemoryChunk {
	template<typename T>
	T* alloc(size_t count) {
		elements = count;
		buffer.resize(elements * sizeof(T));
		return (T*)buffer.data();
	}

	bool operator==(const TagMemoryChunk& rhs) const {
		return buffer == rhs.buffer;
	}

	size_t size() const {
		return elements;
	}

	const byte* data() const {
		return (byte*)buffer.data();
	}

protected:

	int elements = 0;
	std::string buffer;
};
