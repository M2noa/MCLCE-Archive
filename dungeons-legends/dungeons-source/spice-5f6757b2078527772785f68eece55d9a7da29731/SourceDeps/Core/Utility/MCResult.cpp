/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#include "pch_core.h"

#include "MCResult.h"

MCRESULT::MCRESULT(bool success, MCCATEGORY category, uint16_t code)
	: mSuccess(success)
	, mCategory(category)
	, mCode(code) {
}

MCRESULT::MCRESULT(int32_t fullCode)
	: mSuccess(fullCode >= 0)
	, mCategory((MCCATEGORY)(fullCode >> 16))
	, mCode((uint16_t)fullCode) {
}

bool MCRESULT::operator==(const MCRESULT& other) const {
	return mSuccess == other.mSuccess && mCategory == other.mCategory && mCode == other.mCode;
}

bool MCRESULT::operator!=(const MCRESULT& other) const {
	return !(*this == other);
}

bool MCRESULT::isSuccess() const {
	return mSuccess;
}

bool MCRESULT::isSuccess(int32_t fullCode) {
	return fullCode >= 0;
}

// Creates an MCRESULT. Similar to HRESULT, the first bit indicates success (0) or
// failure (1). The next 15 are a category code, indicating the system the error
// originated from. The remaining 16 bits are used to store a unique result code.
int32_t MCRESULT::getFullCode() const {
	uint32_t severity = mSuccess ? 0 : 1;
	return (int32_t)((severity << 31) | ((uint32_t)mCategory << 16) | ((uint32_t)mCode));
}

// Maximum Failure Code: 65536
const MCRESULT MCRESULT_FailedToParseCommand = MCRESULT(false, MCCATEGORY::Minecraft, 0);
const MCRESULT MCRESULT_CommandNotFound = MCRESULT(false, MCCATEGORY::Minecraft, 1);
const MCRESULT MCRESULT_NotEnoughPermissions = MCRESULT(false, MCCATEGORY::Minecraft, 2);
const MCRESULT MCRESULT_CommandVersionMismatch = MCRESULT(false, MCCATEGORY::Minecraft, 3);
const MCRESULT MCRESULT_InvalidOverloadSyntax = MCRESULT(false, MCCATEGORY::Minecraft, 4);
const MCRESULT MCRESULT_InvalidCommandContext = MCRESULT(false, MCCATEGORY::Minecraft, 5);
const MCRESULT MCRESULT_InvalidCommandCall = MCRESULT(false, MCCATEGORY::Minecraft, 6);
const MCRESULT MCRESULT_CommandsDisabled = MCRESULT(false, MCCATEGORY::Minecraft, 7);
const MCRESULT MCRESULT_NoChatPermissions = MCRESULT(false, MCCATEGORY::Minecraft, 8);

const MCRESULT MCRESULT_ExpectedRequestMsg = MCRESULT(false, MCCATEGORY::AutomationProtocol, 0);
const MCRESULT MCRESULT_MalformedRequest = MCRESULT(false, MCCATEGORY::AutomationProtocol, 1);
const MCRESULT MCRESULT_VersionMismatch = MCRESULT(false, MCCATEGORY::AutomationProtocol, 2);
const MCRESULT MCRESULT_TooManyPendingRequests = MCRESULT(false, MCCATEGORY::AutomationProtocol, 3);
const MCRESULT MCRESULT_MustSpecifyVersion = MCRESULT(false, MCCATEGORY::AutomationProtocol, 4);

const MCRESULT MCRESULT_ExecutionFail = MCRESULT(false, MCCATEGORY::MinecraftCommand, 0);
const MCRESULT MCRESULT_CommandStepFail = MCRESULT(false, MCCATEGORY::MinecraftCommand, 1);

// Maximum Success Code: 65535
const MCRESULT MCRESULT_Success = MCRESULT(true, MCCATEGORY::Minecraft, 0);
const MCRESULT MCRESULT_CommandStepDone = MCRESULT(true, MCCATEGORY::MinecraftCommand, 1);
const MCRESULT MCRESULT_CommandExecIncomplete = MCRESULT(true, MCCATEGORY::MinecraftCommand, 2);
const MCRESULT MCRESULT_CommandRequestInitiated = MCRESULT(true, MCCATEGORY::MinecraftCommand, 3);
const MCRESULT MCRESULT_NewCommandVersionAvailable = MCRESULT(true, MCCATEGORY::MinecraftCommand, 4);
