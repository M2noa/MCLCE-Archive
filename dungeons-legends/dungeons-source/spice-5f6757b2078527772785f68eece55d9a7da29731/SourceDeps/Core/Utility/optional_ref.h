#pragma once

#include "Core/Debug/DebugUtils.h"

template <class T>
class reference_wrapper {
public:
	// types
	typedef T type;

	// construct/copy/destroy
	reference_wrapper(T& ref) : ptr(std::addressof(ref)) { }
	reference_wrapper() : ptr(nullptr) { }
	reference_wrapper(T&&) = delete;
	reference_wrapper(const reference_wrapper&) = default;

	// access
	operator T& () const { return *ptr; }
	T& get() const { return *ptr; } //TODO remove and use C++17's operator dot here

	operator bool() const {
		return ptr != nullptr;
	}

private:
	T* ptr;
};

template<typename T>
class optional_ref {
public:
	optional_ref() : ptr(nullptr) { } //invalid constructor
	optional_ref(T& ref) : ptr(&ref) { } //valid constructor

	T& unwrap() const {
		DEBUG_ASSERT(ptr, "Invalid dereference");
		return *ptr;
	}

	reference_wrapper<T> cast() const {
		if (ptr) {
			return *ptr;
		}
		else {
			return{ };
		}
	}

	bool is_some() const {
		return ptr != nullptr;
	}

	bool is_none() const {
		return ptr == nullptr;
	}

	bool operator==(const T& ref) const {
		return ptr == &ref;
	}

	bool operator != (const optional_ref<T>& ref) const {
		return ptr != ref.ptr;
	}

protected:
	T* ptr;
};




