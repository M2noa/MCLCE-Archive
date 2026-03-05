#include "Dungeons.h"

#include "SharedConstants.h"

#include "util/StringUtils.h"
#include "PrivateConstants.h"

namespace SharedConstants {
	// v[MajorVersion].[MinorVersion].[PatchVersion].[RevisionVersion]
	const int MajorVersion = PRIVATE_MAJOR;
	const int MinorVersion = PRIVATE_MINOR;
	const int PatchVersion = PRIVATE_PATCH;
	const int RevisionVersion  = PRIVATE_REVISION;
	const int IsBeta = IS_BETA;

	// 0.12.0 Build 1 uses NPv28
	// 0.12.1 Build 2 uses NPv29 (New Raknet upgrade)
	// 0.12.1 Build 3 uses NPv30
	// 0.12.1 Build 4 uses NPv31
	// 0.12.1 Build 6 uses NPv32
	// 0.12.1 Build 7 uses NPv33
	// 0.12.1 Build 9 uses NPv34 (New Raknet upgrade)
	// 0.12.1 Build 9 uses NPv34 (New Raknet upgrade)
	// 0.13.0 uses NPv38
	// 0.13.1 uses NPv38 (shouldn't have)
	// 0.13.2 uses NPv39
	// 0.14.0.3  Android beta #3 uses NPv42
	// 0.14.0.4  Android beta #4 uses NPv43
	// 0.14.0.6  Android beta #6 uses NPv44
	// 0.14.0 RC1 uses NPv45
	// 0.14.2 uses NPv60 (sudden increase to leave space for 0.14.1 builds, hey ints are big)
	// Realms beta uses 61
	// 0.14.3 uses NPv70 
	// 0.15.0 starts at 80
	// 0.16.0 starts at 90
	// 1.0.0 starts at 100

	const int NetworkProtocolVersion = 100;

	const int AutomationProtocolVersion = 1;

	// To be increased whenever a breaking change is introduced for a companion app like Classroom Mode or the Learn To Code Helper
	const int CompanionAppProtocolVersion = 1;

	const int MaxChatLength = 100;

	const int LevelDBCompressorID = 16;

	const StorageVersion CurrentStorageVersion = StorageVersion::LevelDBSubChunks;
	const LevelChunkFormat CurrentLevelChunkFormat = LevelChunkFormat::v17_0;
	const SubLevelChunkFormat CurrentSubLevelChunkFormat = SubLevelChunkFormat::v17_0;

	const uint16_t NetworkDefaultGamePort = 19132;
	const uint16_t NetworkDefaultGamePortv6 = 19133; // For non-dual stack ip environments, bind to different port on ipv6
#ifdef MCPE_EDU
	const int NetworkDefaultMaxPlayers = 30;
#else
	const int NetworkDefaultMaxPlayers = 5;
#endif
}

namespace Common {

	std::string getGameVersionString(){
		if ( SharedConstants::IsBeta ) {
			return "alpha " + getGameVersionStringNet();
		}
		else{
			return "v" + getGameVersionStringNet();
		}
	}

	std::string getGameVersionStringNet() {
		std::string versionString = Util::toString(SharedConstants::MajorVersion) +
			'.' + Util::toString(SharedConstants::MinorVersion) +
			'.' + Util::toString(SharedConstants::PatchVersion);

		if (SharedConstants::IsBeta ) {
			versionString = versionString + '.' + Util::toString(SharedConstants::RevisionVersion);
		}

		return versionString;
	}

#ifdef COMMIT_ID
#define VERSION_STRING "Dev: " COMMIT_ID
#else
#define VERSION_STRING "Development"
#endif

	std::string getGameDevVersionString() {
#ifdef PUBLISH
		return "";
#else
		return VERSION_STRING;
#endif
	}

}

bool SharedConstants::isVersion(int major, int minor, int patch, int revision) {
	return major == MajorVersion && minor == MinorVersion && patch == PatchVersion && revision == RevisionVersion;
}

int SharedConstants::getVersionCode() {
	return getVersionCode(MajorVersion, MinorVersion, PatchVersion, RevisionVersion);
}

int SharedConstants::getVersionCode(int major, int minor, int patch, int revision) {
	return major * 10000000 + minor * 100000 + patch * 1000 + revision;
}
