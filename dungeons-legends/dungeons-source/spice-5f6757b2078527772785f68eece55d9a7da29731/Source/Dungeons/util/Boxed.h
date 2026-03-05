/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

#include "util/PoolAllocator.h"

template<typename T>
class Boxed {

	struct Base {
		static PoolAllocator mAllocator;

		T mObj;
		std::atomic<int> mRefCount;

		static void* operator new(size_t size) {
			DEBUG_ASSERT_MAIN_THREAD;
			DEBUG_ASSERT(size == mAllocator.BLOCK_SIZE, "Wrong allocation size!");

			return mAllocator.get();
		}

		static void operator delete (void* p) {
			mAllocator.release(p);
		}

		template<class ... Args>
		Base(Args&& ... constructorArgs) :
			mObj(std::forward<Args>(constructorArgs) ...)
			, mRefCount(1){

		}

		Base* grab() {
			//DEBUG_ASSERT_MAIN_THREAD;
			++mRefCount;
			return this;
		}

		bool release() {
			//DEBUG_ASSERT_MAIN_THREAD;
			return mRefCount-- == 1;
		}

	};

public:

	static bool trimPool() {
		return Base::mAllocator.trim();
	}

	template<class ... Args>
	static Boxed<T> make(Args&& ... constructorArgs) {
		return Boxed<T>(new Base(std::forward<Args>(constructorArgs) ...));
	}

	static const PoolAllocator& getAllocator() {
		return Base::mAllocator;
	}

	Boxed() :
		mBase(nullptr) {
	}

	Boxed(const Boxed<T>& other) :
		mBase(other.mBase->grab()) {

	}

	Boxed(Boxed<T>&& other) :
		mBase(other.mBase) {
		other.mBase = nullptr;
	}

	Boxed<T>& operator=(const Boxed<T>& other) {
		reset();
		mBase = other.mBase->grab();
		return *this;
	}

	Boxed<T>& operator=(Boxed<T>&& temp) {
		reset();
		mBase = temp.mBase;
		temp.mBase = nullptr;
		return *this;
	}

	~Boxed() {
		_dropOne();
	}

	T& operator*() {
		DEBUG_ASSERT(mBase, "Cannot dereference an empty pointer");
		return mBase->mObj;
	}

	T* operator->() {
		DEBUG_ASSERT(mBase, "Cannot dereference an empty pointer");
		return &mBase->mObj;
	}

	T& operator*() const {
		DEBUG_ASSERT(mBase, "Cannot dereference an empty pointer");
		return mBase->mObj;
	}

	T* operator->() const {
		DEBUG_ASSERT(mBase, "Cannot dereference an empty pointer");
		return &mBase->mObj;
	}

	explicit operator bool() const {
		return mBase != nullptr;
	}

	bool isUnique() const {
		return mBase && mBase->mRefCount == 1;
	}

	T* get() const {
		return mBase ? &mBase->mObj : nullptr;
	}

	void reset() {
		_dropOne();
		mBase = nullptr;
	}

protected:

	Boxed(Base* base) :
		mBase(base) {
	}

	Base* mBase;

	void _dropOne() {
		if (mBase && mBase->release()) {
			delete mBase;
		}
	}

};

template<typename T>
PoolAllocator Boxed<T>::Base::mAllocator(sizeof(Boxed::Base));

template<typename T, class ... Args>
static Boxed<T> make_box(Args&& ... constructorArgs) {
	return Boxed<T>::make(std::forward<Args>(constructorArgs) ...);
}
