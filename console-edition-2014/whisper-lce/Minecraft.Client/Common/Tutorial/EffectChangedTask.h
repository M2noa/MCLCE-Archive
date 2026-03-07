#pragma once
using namespace std;

#include "TutorialTask.h"

class MobEffect;

class EffectChangedTask : public TutorialTask
{
private:
	MobEffect *m_effect;
	bool m_apply;

public:
	EffectChangedTask(Tutorial *tutorial, int descriptionId, MobEffect *effect, bool apply = true,
		bool enablePreCompletion = true, bool bShowMinimumTime = false, bool bAllowFade = true, bool bTaskReminders = true );
	virtual bool isCompleted();
	virtual void onEffectChanged(MobEffect *effect, bool bRemoved=false);
};