/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

class ClassID {
public:
	using IDType = uint64_t;

	template<typename T>
	static IDType getID() {
		static IDType id = getNextID();
		return id;
	}

private:
	static IDType getNextID();
};
