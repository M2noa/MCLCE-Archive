#pragma once

// Provides a unique identifier at compile time to every template instance of typeid<Foo>:
//
//  Usage:
//
//     typeid_t typeId = mcpe::type_id(foo);
//     if (typeId == someOtherTypeId) { ... }
//

//  Works with RTTI off in standards compliant manner.  However, note that aggressive optimization
//  techniques can theoretically cause this not to work due to COMDAT folding.  In practice, however,
//  the complexity of the class housing the result of the template function generally prevents this.
//  For a stronger guarantee, turn on RTTI and use std::type_index.

class typeid_t {
public:

	template<typename T>
	friend typeid_t type_id();

	bool operator==(const typeid_t& o) const {
		return mID == o.mID;
	}

	bool operator<(const typeid_t& o) const {
		return mID < o.mID;
	}

	bool operator<(size_t id) const {
		return mID < (uint16_t)id;
	}

	static void initialize() {
		count = 0;
	}

	uint16_t value() const {
		return mID;
	}

private:

	static uint16_t count;
	uint16_t mID;
	typeid_t() {
		mID = count++;
	}

};

template<typename T>
typeid_t type_id() {
	static typeid_t id;
	return id;
}
