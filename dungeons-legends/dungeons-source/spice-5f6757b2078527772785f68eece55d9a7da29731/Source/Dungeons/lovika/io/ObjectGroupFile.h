#pragma once

#include "lovika/BlockCuboid.h"
#include "lovika/BlockRegion.h"
#include "lovika/RegionType.h"

namespace io {

struct Region {
	std::string name;
	BlockCuboid bounds;
	RegionType type;

	bool operator==(const Region& rhs) const {
		return name == rhs.name && bounds == rhs.bounds && type == rhs.type;
	}
};

struct Door {
	std::string name;
	BlockCuboid bounds;

	bool operator==(const Door& rhs) const {
		return name == rhs.name && bounds == rhs.bounds;
	}
};

struct Object {
	std::string id;
	BlockCuboid bounds;
	int y;
	std::string tags;
	std::vector<Door> doors;
	std::vector<Region> regions;
	Unique<BlockRegion> blocks;

	bool operator==(const Object& rhs) const {
		return id == rhs.id
				&& bounds == rhs.bounds
				&& y == rhs.y
				&& tags == rhs.tags
				&& doors == rhs.doors
				&& regions == rhs.regions
				&& *blocks == *rhs.blocks;
	}
};

struct ObjectGroup {
	std::vector<Object> objects;

	bool operator==(const ObjectGroup& rhs) const {
		return objects == rhs.objects;
	}
};


Unique<ObjectGroup> loadObjectGroup(std::istream&);
void saveObjectGroup(const ObjectGroup&, std::ostream&);

};
