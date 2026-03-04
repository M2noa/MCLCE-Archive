#pragma once
using namespace std;

class ProgressListener
{
public:
	// 4J Stu - Changed all messages here to be string Id's
	virtual void progressStagePercentage(int p) = 0;
	virtual void progressStart(int stringId) = 0;
	virtual void progressStartNoAbort(int stringId) = 0;
	virtual void progressStage(int stringId) = 0;

	// 4J-PB - added to display bytes retrieved of a save transfer
	virtual void progressStage(wstring &wstrText) = 0;
};