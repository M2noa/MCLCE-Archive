#pragma once
#include "MemTextureProcessor.h"

class MobSkinMemTextureProcessor : public MemTextureProcessor
{
private:
	int *pixels;
	int width, height;
public:
	virtual BufferedImage *process(BufferedImage *in);

private:
	void setForceAlpha(int x0, int y0, int x1, int y1);
    void setNoAlpha(int x0, int y0, int x1, int y1);
    bool hasAlpha(int x0, int y0, int x1, int y1);
};