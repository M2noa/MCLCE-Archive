#pragma once
#include "HttpTextureProcessor.h"

class MobSkinTextureProcessor : public HttpTextureProcessor
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