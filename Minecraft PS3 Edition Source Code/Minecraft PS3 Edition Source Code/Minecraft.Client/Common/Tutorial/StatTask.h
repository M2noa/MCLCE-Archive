#pragma once
using namespace std;

#include "TutorialTask.h"

class Stat;

// 4J Stu - Tutorial tasks that can use the current stat trackin code. This is things like blocks mined/items crafted.
class StatTask : public TutorialTask
{
private:
	Stat *stat;
	int targetValue;

public:
	StatTask(Tutorial *tutorial, int descriptionId, bool enablePreCompletion, Stat *stat, int variance = 1);
	virtual bool isCompleted();
};