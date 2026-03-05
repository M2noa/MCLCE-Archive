/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/
#pragma once

// Predefined Minecraft Facilities
enum class MCCATEGORY : uint8_t {
	Minecraft = 0,
	AutomationProtocol,
	MinecraftCommand
};

struct MCRESULT {
public:
	MCRESULT(bool success, MCCATEGORY category, uint16_t code);
	explicit MCRESULT(int32_t fullCode);
	bool operator==(const MCRESULT& other) const;
	bool operator!=(const MCRESULT& other) const;

	bool isSuccess() const;
	static bool isSuccess(int32_t fullCode);
	int32_t getFullCode() const;

private:
	bool mSuccess;
	MCCATEGORY mCategory;
	uint16_t mCode;
};

// Failure Codes
extern const MCRESULT MCRESULT_FailedToParseCommand;
extern const MCRESULT MCRESULT_CommandNotFound;
extern const MCRESULT MCRESULT_NotEnoughPermissions;
extern const MCRESULT MCRESULT_CommandVersionMismatch;
extern const MCRESULT MCRESULT_InvalidOverloadSyntax;
extern const MCRESULT MCRESULT_InvalidCommandContext;
extern const MCRESULT MCRESULT_InvalidCommandCall;
extern const MCRESULT MCRESULT_CommandsDisabled;
extern const MCRESULT MCRESULT_NoChatPermissions;

extern const MCRESULT MCRESULT_ExpectedRequestMsg;
extern const MCRESULT MCRESULT_MalformedRequest;
extern const MCRESULT MCRESULT_VersionMismatch;
extern const MCRESULT MCRESULT_TooManyPendingRequests;
extern const MCRESULT MCRESULT_MustSpecifyVersion;

extern const MCRESULT MCRESULT_ExecutionFail;
extern const MCRESULT MCRESULT_CommandStepFail;

// Success Codes
extern const MCRESULT MCRESULT_Success;
extern const MCRESULT MCRESULT_CommandStepDone;
extern const MCRESULT MCRESULT_CommandExecIncomplete;
extern const MCRESULT MCRESULT_CommandRequestInitiated;
extern const MCRESULT MCRESULT_NewCommandVersionAvailable;
