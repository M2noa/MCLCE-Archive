#pragma once
#include "CommonTypes.h"

enum class RegionType {
	Prefab,
	Spawn,
	PropArea,
	Trigger,

	// For use in the editor and/or game -> editor serialization
	Door,
	PlacedPrefab,
	PrefabInstanceClone
};

std::string regionTypeToString(RegionType);
Optional<RegionType> regionTypeFromString(const std::string&);
