#pragma once

#include <array>

template <class T, size_t CAPACITY>
class static_vector {
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

		T& operator*() {
			return *mPtr;
		}
	private:
		T* mPtr;
	};

	class const_iterator {
	public:
		const_iterator(const T* begin)
			: mPtr(begin) {
		}

		const_iterator& operator++() {
			++mPtr;
			return *this;
		}

		bool operator !=(const const_iterator& c) const {
			return c.mPtr != mPtr;
		}

		const T& operator*() {
			return *mPtr;
		}
	private:
		const T* mPtr;
	};

	static_vector() {}

	~static_vector() {
		clear();
	}
	
	size_t size() const {
		return mSize;
	}

	template<class ... Args>
	void emplace_back(Args&& ... args) {
		assert(mSize + 1 <= max_size());

		new (&_element(mSize)) T(std::forward<Args>(args) ...);
		++mSize;
	}

	void push_back(const T& val) {
		emplace_back(val);
	}

	void push_back(T&& val) {
		emplace_back(std::move(val));
	}

	void pop_back() {
		//decrease the size by 1 and destruct the element that ends out
		assert(mSize > 0);

		--mSize;
		_element(mSize).~T();
	}

	T& operator[](size_t idx) {
		assert(idx < mSize);
		return _element(idx);
	}

	const T& operator[](size_t idx) const {
		assert(idx < mSize);
		return _element(idx);
	}

	bool empty() const {
		return mSize == 0;
	}

	size_t capacity() const {
		return CAPACITY;
	}

	size_t max_size() const {
		return CAPACITY;
	}

	iterator begin() {
		return{ data() };
	}

	const_iterator begin() const {
		return{ data() };
	}

	iterator end() {
		return{ data() + mSize };
	}
	
	const_iterator end() const {
		return{ data() + mSize };
	}

	T* data() {
		return reinterpret_cast<T*>(mArray.data());
	}
	
	const T* data() const {
		return reinterpret_cast<const T*>(mArray.data());
	}

	T& back() {
		assert(mSize > 0);
		return _element(mSize - 1);
	}

	const T& back() const {
		assert(mSize > 0);
		return _element(mSize - 1);
	}


	void resize(size_t count) {
		_resize(count);
	}

	void resize(size_t count, const T& t) {
		_resize(count, t);
	}

	void resize(size_t count, T val) {
		_resize(count, std::move(val));
	}

	void clear() {
		resize(0);
	}

private:
	std::array<uint8_t, CAPACITY * sizeof(T)> mArray;
	size_t mSize = 0;

	T& _element(size_t idx) {
		return *(data() + idx);
	}

	const T& _element(size_t idx) const {
		return *(data() + idx);
	}

	template<class ... Args>
	void _resize(size_t count, Args&& ... args) {
		auto oldSize = mSize;

		assert(count <= max_size());
		assert(count >= 0);
		mSize = count;

		//init new items if the vector expanded
		for (auto i = oldSize; i < mSize; ++i) {
			new (&_element(i)) T(std::forward<Args>(args) ...);
		}

		//destruct old items if the vector contracted
		for (auto i = mSize; i < oldSize; ++i) {
			_element(i).~T();
		}
	}
};

