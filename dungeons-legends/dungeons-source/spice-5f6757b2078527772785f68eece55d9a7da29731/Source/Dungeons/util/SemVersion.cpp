/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "Dungeons.h"

#include "util/SemVersion.h"

#include <regex>

SemVersion::SemVersion() 
	: mMajor(0)
	, mMinor(0)
	, mPatch(0)
	, mPreRelease()
	, mBuildMeta() {
}

SemVersion::SemVersion(uint16_t major, uint16_t minor /*= 0*/, uint16_t patch /*= 0*/, const std::string& preRelease /*= Util::EMPTY_STRING*/, const std::string& buildMeta /*= Util::EMPTY_STRING*/)
	: mMajor(major)
	, mMinor(minor)
	, mPatch(patch)
	, mPreRelease(preRelease)
	, mBuildMeta(buildMeta) {
}

bool SemVersion::fromString(const std::string& src, SemVersion& output) {
	static const std::regex SemVerRegex(
		"^(0|[1-9][0-9]*)" // Major
		"\\.(0|[1-9][0-9]*)" // Minor
		"\\.(0|[1-9][0-9]*)" // Patch
		"(?:\\-([0-9A-Za-z-]+[\\.0-9A-Za-z-]*))?" // Pre-release
		"(?:\\+([0-9A-Za-z-]+[\\.0-9A-Za-z-]*))?" // Build Mete
	);

	std::smatch results;
	if (std::regex_search(src, results, SemVerRegex)) {
		output.mMajor = std::atoi(results[1].str().c_str());
		output.mMinor = std::atoi(results[2].str().c_str());
		output.mPatch = std::atoi(results[3].str().c_str());
		
		output.mPreRelease = results[4].str();
		output.mBuildMeta = results[5].str();

		return true;
	}
	else {
		return false;
	}
}

bool SemVersion::operator<(const SemVersion& rhs) const {
	auto thisTuple = std::make_tuple(mMajor, mMinor, mPatch);
	auto rhsTuple = std::make_tuple(rhs.mMajor, rhs.mMinor, rhs.mPatch);

	if (thisTuple == rhsTuple) {
		if (mPreRelease.empty() != rhs.mPreRelease.empty()) {
			// If we have prerelease and they don't, we're smaller
			return !mPreRelease.empty();
		}

		// Compare prerelease identifiers
		auto thisPreReleaseIds = Util::split(mPreRelease, '.');
		auto rhsPreReleaseIds = Util::split(rhs.mPreRelease, '.');
		
		for (size_t i = 0; i < thisPreReleaseIds.size() && i < rhsPreReleaseIds.size(); ++i) {
			if (thisPreReleaseIds[i] == rhsPreReleaseIds[i]) {
				continue;
			}
			
			const bool thisIsIntegral = std::all_of(thisPreReleaseIds[i].begin(), thisPreReleaseIds[i].end(), ::isdigit);
			const bool rhsIsIntegral = std::all_of(rhsPreReleaseIds[i].begin(), rhsPreReleaseIds[i].end(), ::isdigit);

			// Both numbers
			if (thisIsIntegral && rhsIsIntegral) {
				return std::atoi(thisPreReleaseIds[i].c_str()) < std::atoi(rhsPreReleaseIds[i].c_str());
			}
			// ASCII/Numeric mismatch 
			else if (thisIsIntegral != rhsIsIntegral) {
				// We're smaller if we are the numeric id
				return (thisIsIntegral == true);
			}

			// Compare ASCII
			return thisPreReleaseIds[i] < rhsPreReleaseIds[i];
		}

		// If we have less identifiers we're smaller
		return thisPreReleaseIds.size() < rhsPreReleaseIds.size();
	}
	
	// Compare version numbers
	return thisTuple < rhsTuple;
}

bool SemVersion::operator>(const SemVersion& rhs) const {
	if (*this == rhs) {
		return false;
	}

	return !(*this < rhs);
}

bool SemVersion::operator!=(const SemVersion& rhs) const {
	return !(*this == rhs);
}

bool SemVersion::operator==(const SemVersion& rhs) const {
	return std::make_tuple(mMajor, mMinor, mPatch, mPreRelease) == std::make_tuple(rhs.mMajor, rhs.mMinor, rhs.mPatch, rhs.mPreRelease);
}

std::string SemVersion::toString() const {
	std::stringstream result;
	result << mMajor << "." << mMinor << "." << mPatch;

	if (!mPreRelease.empty()) {
		result << "-" << mPreRelease;
	}
	if (!mBuildMeta.empty()) {
		result << "+" << mBuildMeta;
	}

	return result.str();
}

uint16_t SemVersion::getMajor() const {
	return mMajor;
}

uint16_t SemVersion::getMinor() const {
	return mMinor;
}

uint16_t SemVersion::getPatch() const {
	return mPatch;
}

const std::string& SemVersion::getPreRelease() const {
	return mPreRelease;
}

const std::string& SemVersion::getBuildMeta() const {
	return mBuildMeta;
}
