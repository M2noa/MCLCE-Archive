/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

#include "common_header.h"

///some wheel reinventing to create a priority queue which allows moving in and iteration
template<typename T, class COMP = std::less<T> >
class MovePriorityQueue {
	typedef std::vector<T> Base;
public:

	typedef typename Base::const_iterator const_iterator;

	template<class ... Args>
	void emplace(Args&& ... args) {
		mC.emplace_back(std::forward<Args>(args) ...);

		std::push_heap(mC.begin(), mC.end(), COMP());
	}

	T pop() {
		std::pop_heap(mC.begin(), mC.end(), COMP());

		auto temp = std::move(mC.back());
		mC.pop_back();
		return temp;
	}

	T& top() {
		return mC.front();
	}

	bool empty() const {
		return mC.empty();
	}

	void clear() {
		mC.clear();
	}

	size_t size() const {
		return mC.size();
	}

	const_iterator begin() const {
		return mC.begin();
	}

	const_iterator end() const {
		return mC.end();
	}

protected:

	Base mC;

};
