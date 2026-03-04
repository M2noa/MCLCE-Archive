#pragma once
using namespace std;

#include "TutorialTask.h"

// A task that creates an maintains an area constraint until it is activated
class AreaTask : public TutorialTask
{
public:
	enum EAreaTaskCompletionStates
	{
		eAreaTaskCompletion_CompleteOnActivation,
		eAreaTaskCompletion_CompleteOnConstraintsSatisfied,
	};
private:
	EAreaTaskCompletionStates m_completionState;
	eTutorial_State m_tutorialState;
public:
	AreaTask(eTutorial_State state, Tutorial *tutorial, vector<TutorialConstraint *> *inConstraints, int descriptionId = -1, EAreaTaskCompletionStates completionState = eAreaTaskCompletion_CompleteOnActivation);
	virtual bool isCompleted();
	virtual void setAsCurrentTask(bool active = true);
	virtual void onStateChange(eTutorial_State newState);

};