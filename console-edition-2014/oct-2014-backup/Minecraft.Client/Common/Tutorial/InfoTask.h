#pragma once
using namespace std;

#include "TutorialTask.h"

// Information messages
class InfoTask : public TutorialTask
{
private:
	unordered_map<int, bool> completedMappings;

	ETelemetryChallenges m_eTelemetryEvent;

	bool CompletionMaskIsValid();
public:
	InfoTask(Tutorial *tutorial, int descriptionId, int promptId = -1, bool requiresUserInput = false, int iMapping = 0, ETelemetryChallenges telemetryEvent = eTelemetryChallenges_Unknown);
	virtual bool isCompleted();
	virtual int getPromptId();
	virtual void setAsCurrentTask(bool active = true);
	virtual void handleUIInput(int iAction);

private:
	void sendTelemetry();

};