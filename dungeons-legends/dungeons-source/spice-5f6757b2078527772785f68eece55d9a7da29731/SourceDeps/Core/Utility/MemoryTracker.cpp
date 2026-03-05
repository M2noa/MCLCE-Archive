#include "pch_core.h"

#include "MemoryTracker.h"

//#define USE_MEM_TRACKER

MemoryTracker* MemoryTracker::root;

#ifdef USE_MEM_TRACKER

MemoryTracker::MemoryTracker(const std::string& name, MemoryTracker* parent) {
	this->name = name;

	static bool initialized = false;
	if (!initialized) {

		root = new RootTracker();
		initialized = true;
	}

	if (!parent) {
		parent = root;
	}

	parent->children.emplace(this);
	this->parent = parent;
}

MemoryTracker::~MemoryTracker() {
	if (parent) {
		parent->children.erase(this);
	}
}

MemoryStats MemoryTracker::_fullStats() const {
	auto stats = getStats();

	for (auto& child : children) {
		stats += child->_fullStats();
	}

	return stats;
}

void MemoryTracker::memoryStats() {
	std::multimap<MemoryStats, MemoryTracker*> sizesort;

	int maxNameLength = 0;

	int total = 0;

	for (auto& tracker : root->children) {
		MemoryStats stats = tracker->_fullStats();
		maxNameLength = std::max((int)tracker->name.length(), maxNameLength);
		total += stats.totalBytes;
		sizesort.insert(std::pair<MemoryStats, MemoryTracker*>(stats, tracker));
	}

	LOGI("MEMORY STATS\n");

	int skip = std::max((int)sizesort.size() - 15, 0);
	if (skip) {
		LOGI(".. skipping %d smaller buffers.\n", skip);
	}

	int i = 0;

	for (auto& pair : sizesort) {

		if (i++ < skip) {
			continue;
		}

		auto& stats = pair.first;
		auto& tracker = pair.second;

		int used = stats.totalBytes;

		std::string paddedName = tracker->name;
		paddedName.append(maxNameLength - paddedName.length() + 3, ' ');

		LOGI("%s: %sb, \t%d%%\t %d elements \n",
			paddedName.c_str(),
			Util::toNiceString(used).c_str(),
			(int)(((float)used / (float)total) * 100.f),
			stats.elements);

	}

	LOGI("TOTAL: %sb\n", Util::toNiceString(total).c_str());
}

#else

MemoryTracker::MemoryTracker(const std::string& name, MemoryTracker* parent) {
}

MemoryTracker::~MemoryTracker() {
}

void MemoryTracker::memoryStats() {
}

#endif
