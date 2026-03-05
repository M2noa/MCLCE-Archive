/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

#include "Core/Utility/PrimitiveTypes.h"

#include "util/NewType.h"
#include "lovika/lib/optional/optional.hpp"

class Entity;
class Player;
class Feature;
class MobEffectInstance;
class Block;
class BlockSource;

template<typename T>
using Unique = std::unique_ptr<T>;

template<typename T>
using Shared = std::shared_ptr<T>;

template<typename T>
using Weak = std::weak_ptr<T>;

template<typename T>
using Ref = std::reference_wrapper<T>;

template<typename T>
using Optional = std::experimental::optional<T>;

typedef std::vector<Entity*> EntityList;
typedef std::vector<Unique<Entity>> OwnedEntityList;
typedef std::vector<Player*> PlayerList;
typedef SmallSet<Entity*> GlobalEntityList;
typedef SmallSet<Entity*> AutonomousEntityList;

typedef SmallSet<Unique<Entity> > OwnedEntitySet;

typedef std::vector<MobEffectInstance> MobEffectInstanceList;

typedef std::lock_guard<std::mutex> LockGuard;
typedef std::unique_lock<std::mutex> ScopedLock;

typedef std::unique_ptr<Feature> FeaturePtr;

typedef uint32_t RandomSeed;

typedef int16_t Height; //don't use fast_t because it will cause all ChunkBlockPos to become 8 bytes if this is not 2 bytes
typedef int_fast8_t FacingID;

enum class BlockProperty : unsigned int;

struct BlockID : public NewType<uint8_t> {
	static const BlockID AIR;

	BlockID() {}

	explicit BlockID(const uint8_t& v) :
		NewType(v) {
	}

	bool hasProperty(BlockProperty properties) const;
};

struct Brightness : public NewType<uint8_t> {
	static const Brightness MAX, MIN, INVALID;

	explicit Brightness(const uint8_t& v) :
		NewType(v) {
	}

	Brightness() :
		Brightness(MIN) {
	}

	Brightness& operator -= (const Brightness& rhs) {
		value -= rhs.value;
		return *this;
	}

	Brightness& operator += (const Brightness& rhs) {
		value += rhs.value;
		return *this;
	}
};

struct BrightnessPair {
	Brightness sky, block;
};

typedef uint8_t DataID;
//
// struct DataID : public NewType < uint_fast8_t > {
//  static const DataID NONE;
//
//  explicit DataID(const uint_fast8_t& v) :
//      NewType(v) {
//
//  }
//
//  explicit DataID(const int& v) :
//      NewType(v) {
//      DEBUG_ASSERT(v >= 0 && v <= 0xff, "Out of bounds computed dataID");
//  }
//
//  DataID& operator &=(const DataID& data) {
//      value &= data.value;
//      return *this;
//  }
//
//  DataID& operator |=(const DataID& data) {
//      value |= data.value;
//      return *this;
//  }
//
//  DataID operator~() const {
//      return DataID(~value);
//  }
// };

typedef unsigned short VertexUVType;

typedef unsigned int Index;

enum class Side {
	Left,
	Right
};

enum DimensionId : int {
	Overworld = 0,
	Nether = 1,
	TheEnd = 2,
	Count,
	Undefined = Count
};

struct FullBlock {
	static const FullBlock AIR;

	BlockID id;
	DataID data;

	static FullBlock fromInt(int i) {
		static_assert(sizeof(FullBlock) <= sizeof(int), "FullTile can't be written to int");
		return FullBlock((BlockID)(i & 0xff), (DataID)(i >> 8));
	}

	FullBlock() :
		id(0)
		, data(0) {
	}

	FullBlock(BlockID id) :
		id(id)
		, data(0) {
	}

	FullBlock(const BlockID& id, const DataID& data) :
		id(id)
		, data(data) {
	}

	bool operator != (const FullBlock& other) const {
		static_assert(sizeof(FullBlock) == 2, "byte trick");
		return *(short*)(this) != (*(short*)&other);
	}

	bool operator == (const FullBlock& other) const {
		static_assert(sizeof(FullBlock) == 2, "byte trick");
		return *(short*)(this) == (*(short*)&other);
	}

	int toInt() const {
		return id + (data << 8);
	}

	size_t hashCode() const {
		static_assert(sizeof(FullBlock) == sizeof(uint16_t), "Cannot downcast");
		return *((uint16_t*)this);
	}

	bool isAir() const {
		return id == 0;
	}

	const Block& getBlock() const;
};

namespace std {
	///hash specialization for unordered_maps
	template<>
	struct hash<FullBlock> {
		// hash functor for vector
		size_t operator()(const FullBlock& _Keyval) const {
			return _Keyval.hashCode();
		}

	};

}

using std::make_shared;

#include "util/range.h"
