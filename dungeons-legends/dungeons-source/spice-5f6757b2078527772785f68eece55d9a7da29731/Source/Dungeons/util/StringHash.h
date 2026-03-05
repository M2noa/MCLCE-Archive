// The MIT License(MIT)
// 
// Copyright(c) 2015 Stefan Reinalter
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
#pragma once

#include <cstdint>
#include <string>


//------------------------------------------------------------------------------
// All other platforms support compile time string hashing.
//------------------------------------------------------------------------------

namespace CompileTime {
	template <size_t I>
	struct Hash	{
		template <size_t N>
		constexpr static inline uint32_t generate(const char (&str)[N]) {
			return static_cast<uint32_t>(static_cast<uint64_t>(Hash<I - 1u>::generate(str) ^ uint32_t(str[I - 1u])) * 16777619ull);
		}
	};

	template <>
	struct Hash<0u> {
		template <size_t N>
		constexpr static inline uint32_t generate(const char (&str)[N]) {
			return 2166136261u;
		}
	};
}

namespace RunTime {
	inline uint32_t fnv1aHash(uint32_t hash, const char* str, size_t length) {
		for (size_t i = 0; i < length; ++i) {
			const uint32_t value = static_cast<uint32_t>(*str++);
			hash ^= value;
			hash *= 16777619u;
		}

		return hash;
	}

	inline uint32_t fnv1aHash(const char* str, size_t length) {
		return fnv1aHash(2166136261u, str, length);
	}

	inline uint32_t fnv1aHash(const char* str) {
		return fnv1aHash(str, strlen(str));
	}
}

namespace Detail {
	template <typename T>
	struct HashHelper {};

	template <>
	struct HashHelper<const char*> {
		static inline uint32_t generate(const char* str) {
			return RunTime::fnv1aHash(str);
		}
	};

	template <>
	struct HashHelper<std::string> {
		static inline uint32_t generate(const std::string& str) {
			return RunTime::fnv1aHash(str.c_str());
		}
	};

	template <size_t N>
	struct HashHelper<char [N]> {
		constexpr static inline uint32_t generate(const char (&str)[N]) {
			return CompileTime::Hash<N - 1u>::generate(str);
		}
	};
}

template <typename T>
constexpr static inline uint32_t generateHash(const T& str) {
	return Detail::HashHelper<T>::generate(str);
}

class StringHash {
public:
	template <typename T>
	StringHash(const T& str)
		: m_hash(generateHash(str)) {
	}

	inline uint32_t getHash(void) const {
		return m_hash;
	}

private:
	const uint32_t m_hash;
};
