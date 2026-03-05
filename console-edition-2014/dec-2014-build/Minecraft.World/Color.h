#pragma once

class Color
{
private:
	int colour;

public:
	//Creates an opaque sRGB color with the specified red, green, and blue values in the range (0.0 - 1.0).
	Color( float r, float g, float b);
	Color( int r, int g, int b);

	static Color getHSBColor(float h, float s, float b);
	int getRGB();
};