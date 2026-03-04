#pragma once
class Options;

class ScreenSizeCalculator
{
private:
	int w;
    int h;
public:
	double rawWidth, rawHeight;
    int scale;
    ScreenSizeCalculator(Options *options, int width, int height, int forceScale = -1);		// 4J added forceScale parameter
    int getWidth();
    int getHeight();
};