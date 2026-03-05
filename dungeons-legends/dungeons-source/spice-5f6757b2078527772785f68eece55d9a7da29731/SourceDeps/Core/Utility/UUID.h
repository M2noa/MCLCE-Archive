#pragma once

#include <string>
#include <sstream>
#include <iomanip>

namespace mce {

	class UUID {
	public:
		static UUID EMPTY;

		//xxxxxxxx-xxxx-Mxxx-Nxxx-xxxxxxxxxxxx 8-4-4-12
		UUID() {
			Data[0] = 0;
			Data[1] = 0;
		}

		UUID(uint64_t mostSignificantBits, uint64_t leastSignificantBits) {
			Data[0] = mostSignificantBits;
			Data[1] = leastSignificantBits;
		}

		std::string toString() const;

		static UUID fromString(const std::string& in);

		uint64_t getMostSignificantBits() const {
			return Data[0];
		}

		uint64_t getLeastSignificantBits() const {
			return Data[1];
		}

		bool operator==(const UUID& rhs) const {
			return Data[0] == rhs.Data[0] && Data[1] == rhs.Data[1];
		}

		bool operator!=(const UUID& rhs) const {
			return Data[0] != rhs.Data[0] || Data[1] != rhs.Data[1];
		}

		size_t getHash() const {
			if (sizeof(size_t) == sizeof(int64_t)) {
				return static_cast<size_t>(getMostSignificantBits() ^ (getLeastSignificantBits() * 0x1f1f1f1f));
			}
			else {
				return static_cast<size_t>(((getMostSignificantBits() >> 32) ^ (getMostSignificantBits() * 0x1f1f1f1f)) ^ (((getLeastSignificantBits() * 0x1f1f1f1f) >> 32) ^ getLeastSignificantBits()));
			}
		}

	private:

		uint64_t Data[2];
	};

}

namespace std {
	///hash specialization for unordered_maps
	template<>
	struct hash<mce::UUID> {
		// hash functor for vector
		size_t operator()(const mce::UUID& _Keyval) const {
			return _Keyval.getHash();
		}

	};

}
