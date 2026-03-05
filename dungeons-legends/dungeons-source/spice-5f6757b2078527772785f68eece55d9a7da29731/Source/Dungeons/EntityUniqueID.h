
#pragma once

#include "CommonTypes.h"
#include "Core/Utility/UUID.h"
#include "util/Math.h"
#include "network/BinaryStream.h"

class EntityRuntimeID {

public:
	uint64_t rawID = 0;

	EntityRuntimeID(){

	}

	explicit EntityRuntimeID(uint64_t bytes) {
		rawID = bytes;
	}

	size_t getHash() const {
		return Math::hash2(rawID >> 32, rawID & 0xffffffff);
	}

	explicit operator bool() const {
		return rawID != 0;
	}

	explicit operator uint64_t() const {
		return rawID;
	}

	bool operator==(const EntityRuntimeID& rhs) const {
		return rhs.rawID == rawID;
	}

	bool operator!=(const EntityRuntimeID& rhs) const {
		return rhs.rawID != rawID;
	}

	EntityRuntimeID& operator++() {
		++rawID;//it's enough to increment this by 1 to increment the ID inside this "run"
		return *this;
	}

};

//specialization for serialization
template<>
struct serialize<EntityRuntimeID>{
	void static write(const EntityRuntimeID& val, BinaryStream& stream){
		stream.writeUnsignedVarInt64(val.rawID);
	}

	EntityRuntimeID static read(ReadOnlyBinaryStream& stream){
		return EntityRuntimeID(stream.getUnsignedVarInt64());
	}
};

struct EntityUniqueID {
	int64_t rawID;

	/*
	   EntityUniqueID explainer:
	   we tried 3 methods for generating a truly unique ID efficiently
	   1. just incrementing a counter, forever. This does not work because if the game crashes, the counter
	   might be reset to an arbitrary point in the past
	   2. using steady_clock as the high 32-bits. This *almost* worked, however we could not find strict enough
	   guarantees about when the clock resets and crash reports point to the fact that it indeed does on some platforms.
	   3. (current method) each time we open a level, we DEcrement a 32-bit "runID" by 1. Starting from INT_MAX is to
	   avoid collisions with methods 1 and 2; then, generating a new unique ID is as simple as adding 1.
	   The Level manages storing the Run ID. This way each level can be opened 4 billion times, and each run 4 billion
	      entities
	   can be created.
	 */

	EntityUniqueID() :
		rawID(0xffffffffffffffff){
	}

	static EntityUniqueID fromUUID(const mce::UUID& uuid){
		return EntityUniqueID((int64_t) (uuid.getMostSignificantBits() ^ uuid.getLeastSignificantBits()));
	}

	static EntityUniqueID fromClientId(uint64_t clientId){
		// We can set because timestamp should be 0
		return EntityUniqueID((int64_t)clientId);
	}

	explicit EntityUniqueID(int64_t bytes) {
		rawID = bytes;
	}

	size_t getHash() const {
		return Math::hash2(rawID >> 32, rawID & 0xffffffff);
	}

	explicit operator bool() const {
		return rawID != 0xffffffffffffffff;
	}

	explicit operator int64_t() const {
		return rawID;
	}

	bool operator==(const EntityUniqueID& rhs) const {
		return rhs.rawID == rawID;
	}

	bool operator!=(const EntityUniqueID& rhs) const {
		return rhs.rawID != rawID;
	}

	bool operator>(const EntityUniqueID& rhs) const {
		return rhs.rawID > rawID;
	}

	bool operator<(const EntityUniqueID& rhs) const {
		return rhs.rawID < rawID;
	}

	EntityUniqueID& operator++() {
		++rawID;//it's enough to increment this by 1 to increment the ID inside this "run"
		return *this;
	}

};

//specialization for serialization
template<>
struct serialize<EntityUniqueID>{
	void static write(const EntityUniqueID& val, BinaryStream& stream){
		stream.writeVarInt64(val.rawID);
	}

	EntityUniqueID static read(ReadOnlyBinaryStream& stream){
		return EntityUniqueID(stream.getVarInt64());
	}
};

namespace std {
	///hash specialization for unordered_maps
	template<>
	struct hash<EntityUniqueID>{
		// hash functor for vector
		size_t operator()(const EntityUniqueID& _Keyval) const {
			return _Keyval.getHash();
		}

	};

	template<>
	struct hash<EntityRuntimeID>{
		// hash functor for vector
		size_t operator()(const EntityRuntimeID& _Keyval) const {
			return _Keyval.getHash();
		}

	};

}
