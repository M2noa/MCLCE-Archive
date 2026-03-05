#pragma once
#include "StitchedTexture.h"

class CompassTexture : public StitchedTexture
{
private:
	int m_iPad;
	CompassTexture* m_dataTexture;

public:
	static CompassTexture *instance;
	double rot, rota;

	CompassTexture();
	CompassTexture(int iPad, CompassTexture *dataTexture);

	void cycleFrames();
	void updateFromPosition(Level *level, double x, double z, double yRot, bool noNeedle, bool instant);
	
	virtual int getSourceWidth() const;
	virtual int getSourceHeight() const;
	virtual int getFrames();
	virtual void freeFrameTextures();		// 4J added
	virtual bool hasOwnData();		// 4J Added
};