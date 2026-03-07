#pragma once

#include "Layer.h"

class FlatLayer : public Layer
{
private:
	int val;

public:
	FlatLayer(int val);
	intArray getArea(int xo, int yo, int w, int h);
};