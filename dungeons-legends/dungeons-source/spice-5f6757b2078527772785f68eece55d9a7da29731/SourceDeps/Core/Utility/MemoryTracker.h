#pragma once

struct MemoryStats {
	unsigned int totalBytes;
	int elements;

	explicit MemoryStats(unsigned int total = 0, int elements = 0) :
		totalBytes(total)
		, elements(elements) {

	}

	MemoryStats& operator += (const MemoryStats& rhs) {
		totalBytes += rhs.totalBytes;
		elements += rhs.elements;
		return *this;
	}

	bool operator< (const MemoryStats& rhs) const {
		return totalBytes < rhs.totalBytes;
	}

};

class MemoryTracker {
protected:

	static MemoryTracker* root;

public:

	static void memoryStats();

	MemoryTracker(const std::string& name, MemoryTracker* parent = nullptr);

	virtual ~MemoryTracker();

	virtual MemoryStats getStats() const = 0;

protected:

	MemoryTracker() {

	}

#ifndef PUBLISH
	std::string name;

	MemoryTracker* parent = nullptr;
	std::unordered_set<MemoryTracker*> children;

	MemoryStats _fullStats() const;

#endif
private:

};
