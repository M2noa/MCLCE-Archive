#pragma once
using namespace std;

#include "TutorialHint.h"

class ItemInstance;

class LookAtTileHint : public TutorialHint
{
private:
	int *m_iTiles;
	unsigned int m_iTilesCount;
	int m_iconOverride;
	int m_iData;
	int m_iDataOverride;

public:
	LookAtTileHint(eTutorial_Hint id, Tutorial *tutorial, int tiles[], unsigned int tilesLength, int iconOverride = -1, int iData=-1, int iDataOverride  = -1);
	~LookAtTileHint();

	virtual bool onLookAt(int id, int iData=0);
};