#pragma once

#include "Core/Platform/ErrorHandling.h"

class ByteMask {
public:

	ByteMask() : mask() {
	}

	void setEmpty() {
		mask = std::numeric_limits<decltype(mask)>::max();
	}

	void add(const uint8_t facing) {
		MCEASSERTMESSAGE(facing < 8, "Out of bounds");
		mask |= (0x1 << facing);
	}

	bool contains(const uint8_t facing) const {
		MCEASSERTMESSAGE(facing < 8, "Out of bounds");
		return ((mask >> facing) & 0x1) != 0;
	}

	explicit operator bool() const {
		return mask != 0;
	}

	uint8_t toByte() const {
		return mask;
	}

protected:

	uint8_t mask;

};
