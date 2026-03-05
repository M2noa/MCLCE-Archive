/*********************************************************
*   (c) Mojang. All rights reserved                      *
*   (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

enum class Difficulty : int {
	Peaceful,
	Easy,
	Normal,
	Hard,

	Count
};

namespace DifficultyUtils {
	inline static std::string asString(Difficulty difficulty) {
		switch (difficulty) {
		case Difficulty::Peaceful:
			return "PEACEFUL";
		case Difficulty::Easy:
			return "EASY";
		case Difficulty::Normal:
			return "NORMAL";
		case Difficulty::Hard:
			return "HARD";
		default:
			DEBUG_FAIL("unknown difficulty");
			return "UNKNOWN";
		}
	}

	inline static int getMultiplier(Difficulty difficulty) {
		switch (difficulty) {
		case Difficulty::Peaceful:
			return 0;
		case Difficulty::Easy:
			return 1;
		case Difficulty::Normal:
			return 2;
		case Difficulty::Hard:
			return 3;
		default:
			DEBUG_FAIL("unknown difficulty");
			return 0;
		}
	}

}	// namespace Difficulty
