#include "Dungeons.h"

#include "RegionType.h"

std::string regionTypeToString(RegionType type) {
	if (RegionType::Prefab == type) {
		return "prefab";
	}
	if (RegionType::Spawn == type) {
		return "spawn";
	}
	if (RegionType::Trigger == type) {
		return "trigger";
	}
	if (RegionType::Door == type) {
		return "door";
	}
	if (RegionType::PropArea == type) {
		return "prop-area";
	}
	if (RegionType::PlacedPrefab == type) {
		return "placed-prefab";
	}
	if (RegionType::PrefabInstanceClone == type) {
		return "instance-clone";
	}
	return "unknown";
}

Optional<RegionType> regionTypeFromString(const std::string& regionTypeString) {
	auto ts = Util::toLower(regionTypeString);

	if ("prefab" == ts) {
		return RegionType::Prefab;
	}
	if ("spawn" == ts) {
		return RegionType::Spawn;
	}
	if ("trigger" == ts) {
		return RegionType::Trigger;
	}
	if ("door" == ts) {
		return RegionType::Door;
	}
	if ("prop-area" == ts) {
		return RegionType::PropArea;
	}
	if ("placed-prefab" == ts) {
		return RegionType::PlacedPrefab;
	}
	if ("instance-clone" == ts) {
		return RegionType::PrefabInstanceClone;
	}
	return{};
}
