/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

#include "common_header.h"
#include "SpinLock.h"

template<typename T>
class MPMCQueue {
public:

	void push(const T& item);
	void push(T&& item);
	bool pop(T& outItem);
	size_t size();

private:

	std::queue<T> mQueue;
	SpinLock mSpinLock;
};

template<typename T>
void MPMCQueue<T>::push(const T& item) {
	std::lock_guard<SpinLock> guard(mSpinLock);
	mQueue.push(item);
}

template<typename T>
void MPMCQueue<T>::push(T&& item) {
	std::lock_guard<SpinLock> guard(mSpinLock);
	mQueue.emplace(std::forward<T>(item));
}

template<typename T>
bool MPMCQueue<T>::pop(T& outItem) {
	std::lock_guard<SpinLock> guard(mSpinLock);
	if (mQueue.empty()) {
		return false;
	}
	else {
		outItem = std::move(mQueue.front());
		mQueue.pop();
		return true;
	}
}

template<typename T>
size_t MPMCQueue<T>::size() {
	std::lock_guard<SpinLock> guard(mSpinLock);
	return mQueue.size();
}
