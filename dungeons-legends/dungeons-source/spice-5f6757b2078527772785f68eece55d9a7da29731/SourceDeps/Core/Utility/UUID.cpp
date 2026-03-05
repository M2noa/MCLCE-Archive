#include "pch_core.h"

#include "UUID.h"

mce::UUID mce::UUID::EMPTY = mce::UUID();

mce::UUID mce::UUID::fromString(const std::string& in) {
	uint64_t mostSignificantBits = 0;
	uint64_t leastSignificantBits = 0;

	int count = 0;

	for (unsigned int i = 0; i < in.length(); ++i) {
		signed char c = in[i];

		if ((c >= '0' && c <= '9')) {
			c -= '0';
		}
		else if ((c >= 'a' && c <= 'f')) {
			c -= 'a' - 10;
		}
		else if ((c >= 'A' && c <= 'F')) {
			c -= 'A' - 10;
		}
		else {
			c = -1;
		}

		if (c >= 0) {
			++count;
			if (count <= 16) {
				mostSignificantBits |= (((uint64_t)c) << (64 - count * 4));
			}
			else {
				leastSignificantBits |= (((uint64_t)c) << (128 - count * 4));
			}
		}
	}

	if (count != 32) {
		return UUID();
	}

	return UUID(mostSignificantBits, leastSignificantBits);
}

std::string mce::UUID::toString() const {
	std::stringstream stream;
	stream << std::setfill('0') << std::setw(sizeof(uint32_t) * 2) << std::hex << ((uint32_t)(getMostSignificantBits() >> 32));
	stream << '-';
	stream << std::setfill('0') << std::setw(sizeof(uint16_t) * 2) << std::hex << ((uint16_t)((getMostSignificantBits() >> 16) & 0xFFFF));
	stream << '-';
	stream << std::setfill('0') << std::setw(sizeof(uint16_t) * 2) << std::hex << ((uint16_t)(getMostSignificantBits() & 0xFFFF));
	stream << '-';
	stream << std::setfill('0') << std::setw(sizeof(uint16_t) * 2) << std::hex << ((uint16_t)(getLeastSignificantBits() >> 48));
	stream << '-';
	stream << std::setfill('0') << std::setw(sizeof(uint32_t) * 2) << std::hex << ((uint32_t)((getLeastSignificantBits() >> 16)) & 0xFFFFFFFF);
	stream << std::setfill('0') << std::setw(sizeof(uint16_t) * 2) << std::hex << ((uint16_t)(getLeastSignificantBits() & 0xFFFF));

	return stream.str();
}
