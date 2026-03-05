#include "stdafx.h"
#include "Scale.h"

Scale::Scale(Synth *synth, double xScale, double yScale)
{
    this->synth = synth;
    this->xScale = 1.0 / xScale;
    this->yScale = 1.0 / yScale;
}

double Scale::getValue(double x, double y)
{
	return synth->getValue(x * xScale, y * yScale);
}
