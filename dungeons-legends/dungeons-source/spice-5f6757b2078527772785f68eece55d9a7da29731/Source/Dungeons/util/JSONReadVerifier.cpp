/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "util/JSONReadVerifier.h"
#include "Core/Debug/Log.h"


void JSONReadVerifier::verifyRead(const std::string& file, Json::Reader& reader, bool result) {
	/*
	adors
	This helps us as devs (for now) solve problems in JSON. Read the result
	through the console, or put a breakpoint in the statement if you're doing
	a lot of iterating.

	This, however, should be replaced with some sort of error message to the user
	when time comes to ship. This is important, because breaking entities.json 
	will cause entity definitions to be lost, and our sitting wolves to scoot around,
	among other things of course.
	*/
	std::stringstream str;
	if (!result) {
		str << "\n------------------------------------------------------" << std::endl;
		str << "JSON Parse error in file: " << file.c_str() << std::endl;
		str << reader.getFormattedErrorMessages().c_str() << std::endl;
		str << "------------------------------------------------------" << std::endl;
		LOGE("%s\n", str.str().c_str());
		DEBUG_FAIL("Invalid JSON");
	}
}
