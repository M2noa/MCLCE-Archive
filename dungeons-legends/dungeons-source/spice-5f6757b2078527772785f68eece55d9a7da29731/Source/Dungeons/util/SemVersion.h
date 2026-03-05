/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

#include "common_header.h"

// Semantic Version class
// Implemented to specification found at http://semver.org/
class SemVersion {
public:
	SemVersion();
	SemVersion(uint16_t major, uint16_t minor = 0, uint16_t patch = 0, const std::string& preRelease = Util::EMPTY_STRING, const std::string& buildMeta = Util::EMPTY_STRING);
	SemVersion(const SemVersion&) = default;
	SemVersion& operator=(const SemVersion&) = default;

	static bool fromString(const std::string& src, SemVersion& output);

	bool operator==(const SemVersion& rhs) const;
	bool operator!=(const SemVersion& rhs) const;
	bool operator<(const SemVersion& rhs) const;
	bool operator>(const SemVersion& rhs) const;

	std::string toString() const;

	uint16_t getMajor() const;
	uint16_t getMinor() const;
	uint16_t getPatch() const;
	const std::string& getPreRelease() const;
	const std::string& getBuildMeta() const;
private:
	uint16_t mMajor;
	uint16_t mMinor;
	uint16_t mPatch;

	std::string mPreRelease;
	std::string mBuildMeta;
};

namespace std {
	template<>
	struct hash<SemVersion> {
		size_t operator()(const SemVersion& rhs) const {
			return std::hash<std::string>()(rhs.toString());
		}
	};
}