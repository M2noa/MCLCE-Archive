#pragma once

#include <cassert>

#include "static_vector.h"
//#include "Core/Debug/DebugUtils.h"

template<typename T>
class buffer_span_mut {
public:

	class iterator {
	public:
		iterator(T* begin)
			: mPtr(begin) {
		}

		iterator& operator++() {
			++mPtr;
			return *this;
		}

		bool operator !=(const iterator& c) const {
			return c.mPtr != mPtr;
		}

		bool operator==(const iterator& c) const {
			return c.mPtr == mPtr;
		}

		T& operator*() {
			return *mPtr;
		}


	private:
		T* mPtr;
	};

	buffer_span_mut(T* begin, T* end)
		: mBegin(begin)
		, mEnd(end) {
		assert(begin <= end);
	}

	buffer_span_mut() : buffer_span_mut(nullptr, nullptr) {}

	buffer_span_mut(std::vector<T>& buffer) : buffer_span_mut(buffer.data(), buffer.data() + buffer.size()) {}

	template<size_t SIZE>
	buffer_span_mut(std::array<T, SIZE>& buffer) : buffer_span_mut(buffer.data(), buffer.data() + buffer.size()) {}

	template<size_t SIZE>
	buffer_span_mut(T(&x)[SIZE]) : buffer_span_mut(x, x + sizeof(x)) {}

	template<size_t SIZE>
	buffer_span_mut(static_vector<T, SIZE>& buffer) : buffer_span_mut(buffer.data(), buffer.data() + buffer.size()) {}

	//initializer list members are guaranteed to be contiguous and castable to T*
	buffer_span_mut(std::initializer_list<T>& list) : buffer_span_mut(&(*list.begin()), &(*list.end())) {}

	size_t byte_size() const {
		return size() * sizeof(T);
	}

	size_t size() const {
		return mEnd - mBegin;
	}

	bool empty() const {
		return size() == 0;
	}

	T& operator[](size_t idx) {
		DEBUG_ASSERT(idx < size(), "out of bounds"); 

		return mBegin[idx];
	}

	const T& operator[](size_t idx) const {
		DEBUG_ASSERT(idx < size(), "out of bounds");

		return mBegin[idx];
	}

	buffer_span_mut<T> subspan(size_t idx1, size_t idx2) const {
		DEBUG_ASSERT(idx1 < idx2, "Invalid indices");
		DEBUG_ASSERT(mBegin + idx2 <= mEnd, "Out of bounds");

		return{ mBegin + idx1, mBegin + idx2 };
	}

	buffer_span_mut<T> tail(size_t idx1) const {
		DEBUG_ASSERT(mBegin + idx1 < mEnd, "Out of bounds");

		return{ mBegin + idx1, mEnd };
	}

	T* data() {
		return mBegin;
	}

	const T* data() const {
		return mBegin;
	}

	std::vector<T> copy() const {
		std::vector<T> res(size());
		for (auto&& elem : *this) {
			res.emplace_back(elem);
		}
		return res;
	}

	iterator begin() const {
		return{ mBegin };
	}

	iterator end() const {
		return{ mEnd };
	}

private:

	T* mBegin, *mEnd;

};

template<typename T>
class buffer_span {
public:

	class iterator {
	public:
		iterator(const T* begin) 
			: mPtr(begin) {
		}

		iterator& operator++() {
			++mPtr;
			return *this;
		}

		bool operator !=(const iterator& c) const {
			return c.mPtr != mPtr;
		}

		bool operator==(const iterator& c) const {
			return c.mPtr == mPtr;
		}

		const T& operator*() {
			return *mPtr;
		}


	private:
		const T* mPtr;
	};

	buffer_span(const T* begin, const T* end)
		: mBegin(begin)
		, mEnd(end) {
		assert(begin <= end);
	}

	buffer_span() : buffer_span(nullptr, nullptr) {}

	buffer_span(const std::vector<T>& buffer) : buffer_span(buffer.data(), buffer.data() + buffer.size()) {}

	template<size_t SIZE>
	buffer_span(const std::array<T, SIZE>& buffer) : buffer_span(buffer.data(), buffer.data() + buffer.size()) {}

	template<size_t SIZE>
	buffer_span(const T(&x)[SIZE]) : buffer_span(x, x + sizeof(x)) {}

	template<size_t SIZE>
	buffer_span(const static_vector<T, SIZE>& buffer) : buffer_span(buffer.data(), buffer.data() + buffer.size()) {}

	//initializer list members are guaranteed to be contiguous and castable to T*
	buffer_span(const std::initializer_list<T>& list) : buffer_span(&(*list.begin()), &(*list.end())) {}

	//can always construct a buffer_span from a buffer_span_mut
	buffer_span(const buffer_span_mut<T>& span) : buffer_span(span.data(), span.data() + span.size()) {}

	size_t byte_size() const {
		return size() * sizeof(T);
	}

	size_t size() const {
		return mEnd - mBegin;
	}

	bool empty() const {
		return size() == 0;
	}

	const T& operator[](size_t idx) const {
		DEBUG_ASSERT(idx < size(), "out of bounds");

		return mBegin[idx];
	}

	buffer_span<T> subspan(size_t idx1, size_t idx2) const {
		DEBUG_ASSERT(idx1 < idx2, "Invalid indices");
		DEBUG_ASSERT(mBegin + idx2 <= mEnd, "Out of bounds");

		return{ mBegin + idx1, mBegin + idx2 };
	}

	buffer_span<T> tail(size_t idx1) const {
		DEBUG_ASSERT(mBegin + idx1 < mEnd, "Out of bounds");

		return{ mBegin + idx1, mEnd };
	}

	const T* data() const {
		return mBegin;
	}

	iterator begin() const {
		return{ mBegin };
	}

	iterator end() const {
		return{ mEnd };
	}

	std::vector<T> copy() const {
		std::vector<T> res(size());
		for (auto&& elem : *this) {
			res.emplace_back(elem);
		}
		return res;
	}
private:

	const T* mBegin, *mEnd;
};


