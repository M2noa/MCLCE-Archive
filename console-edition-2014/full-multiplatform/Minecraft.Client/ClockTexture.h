#pragma once
#include "StitchedTexture.h"

class ClockTexture : public StitchedTexture
{
private:
	double rot, rota;
	int m_iPad;
	ClockTexture* m_dataTexture;

public:
	ClockTexture();
	ClockTexture(int iPad, ClockTexture *dataTexture);
	void cycleFrames();
	
	virtual int getSourceWidth() const;
	virtual int getSourceHeight() const;
	virtual int getFrames();
	virtual void freeFrameTextures();		// 4J added
	virtual bool hasOwnData();		// 4J Added
};