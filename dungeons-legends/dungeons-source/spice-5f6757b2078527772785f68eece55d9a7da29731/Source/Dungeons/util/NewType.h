/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

#include "common_header.h"

template<typename T>
struct NewType {
	typedef T Raw;
	Raw value;

	explicit NewType(const Raw& t_) :
		value(t_) {
	}

	NewType(){
	}

	NewType(const NewType& t_) :
		value(t_.value){
	}

	NewType& operator=(const NewType& rhs) {
		value = rhs.value;
		return *this;
	}

	operator const Raw& () const {
		return value;
	}

	bool operator==(const NewType& rhs) const {
		return value == rhs.value;
	}

	bool operator<(const NewType& rhs) const {
		return value < rhs.value;
	}

};
